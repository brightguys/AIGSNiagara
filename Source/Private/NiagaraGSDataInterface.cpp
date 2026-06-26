#include "NiagaraGSDataInterface.h"
#include "RHI.h"
#include "RHIResources.h"
#include "RHIDefinitions.h"
#include "NiagaraDataInterfaceRW.h"
#include "NiagaraTypes.h"
#include "NiagaraCustomVersion.h"
#include "NiagaraShaderParametersBuilder.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraCompileHashVisitor.h"
#include "VectorVM.h"
#include "NiagaraGSDataInterfaceGPU.h"
#include "GaussianSplatPLYParser.h"
#include "RHICommandList.h"
#include "RenderingThread.h"
#include "Misc/Paths.h"
#include "Misc/ScopeLock.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Static member definitions
// ─────────────────────────────────────────────────────────────────────────────

const FName UNiagaraGSDataInterface::Name_GetSplatCount(TEXT("GetSplatCount"));
const FName UNiagaraGSDataInterface::Name_GetSplatPosition(TEXT("GetSplatPosition"));
const FName UNiagaraGSDataInterface::Name_GetSplatScale(TEXT("GetSplatScale"));
const FName UNiagaraGSDataInterface::Name_GetSplatOrientation(TEXT("GetSplatOrientation"));
const FName UNiagaraGSDataInterface::Name_GetSplatColor(TEXT("GetSplatColor"));
const FName UNiagaraGSDataInterface::Name_GetSplatOpacity(TEXT("GetSplatOpacity"));
const FName UNiagaraGSDataInterface::Name_GetSplatSHColor(TEXT("GetSplatSHColor"));
const FName UNiagaraGSDataInterface::Name_GetSplatSHCoefficients(TEXT("GetSplatSHCoefficients"));
const FName UNiagaraGSDataInterface::Name_FlushGPUBuffers(TEXT("FlushGPUBuffers"));

TAtomic<uint64> UNiagaraGSDataInterface::GlobalFlushGeneration(0);

FCriticalSection UNiagaraGSDataInterface::DiskCacheCS;
TMap<FString, TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe>> UNiagaraGSDataInterface::DiskCache;

// ─────────────────────────────────────────────────────────────────────────────
//  Data-source resolution (imported asset OR raw .ply on disk)
// ─────────────────────────────────────────────────────────────────────────────

void UNiagaraGSDataInterface::EnsureSplatDataLoaded()
{
    // Asset wins; nothing to load from disk.
    if (SplatAsset)
    {
        ResolvedDiskData.Reset();
        return;
    }

    if (SourceFilePath.IsEmpty())
    {
        ResolvedDiskData.Reset();
        return;
    }

    // Resolve a project-relative path to absolute for the file reader / cache key.
    FString ResolvedPath = SourceFilePath;
    if (FPaths::IsRelative(ResolvedPath))
    {
        ResolvedPath = FPaths::ConvertRelativePathToFull(
            FPaths::ProjectDir(), ResolvedPath);
    }

    // Already resolved to this path on this DI — nothing to do.
    if (ResolvedDiskData.IsValid() && ResolvedDiskData->SourcePath == ResolvedPath)
    {
        return;
    }

    // Process-wide cache hit → instant (no re-parse). THIS is what stops the
    // per-instance re-parsing churn for big files.
    {
        FScopeLock Lock(&DiskCacheCS);
        if (TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe>* Found = DiskCache.Find(ResolvedPath))
        {
            ResolvedDiskData = *Found;
            return;
        }
    }

    // Cache miss → parse once (the only place that pays the file-read cost).
    TArray<FGaussianSplatData> Parsed;
    int32 ParsedDegree = 0;
    FString Error;

    if (FGaussianSplatPLYParser::ParsePLY(ResolvedPath, Parsed, ParsedDegree, Error))
    {
        TSharedPtr<FGSDiskSplatData, ESPMode::ThreadSafe> NewEntry =
            MakeShared<FGSDiskSplatData, ESPMode::ThreadSafe>();
        NewEntry->Splats = MoveTemp(Parsed);
        NewEntry->SHDegree = ParsedDegree;
        NewEntry->SourcePath = ResolvedPath;

        {
            FScopeLock Lock(&DiskCacheCS);
            DiskCache.Add(ResolvedPath, NewEntry);
        }
        ResolvedDiskData = NewEntry;

        UE_LOG(LogTemp, Log,
            TEXT("NiagaraGS: Loaded %d splats from disk '%s' (SH degree %d) — cached process-wide"),
            ResolvedDiskData->Splats.Num(), *ResolvedPath, ResolvedDiskData->SHDegree);
    }
    else
    {
        ResolvedDiskData.Reset();
        UE_LOG(LogTemp, Error,
            TEXT("NiagaraGS: Failed to load '%s' from disk: %s"),
            *ResolvedPath, *Error);
    }
}

const FGSDiskSplatData* UNiagaraGSDataInterface::ResolvedDiskPayload() const
{
    // Prefer this DI's own resolved handle (set during InitPerInstanceData).
    if (ResolvedDiskData.IsValid())
    {
        return ResolvedDiskData.Get();
    }

    // Otherwise resolve from the process-wide cache by path. This is what lets a
    // *rendering* DI (which may not be the one that parsed/uploaded) still reach
    // the data — it carries SourceFilePath, and the cache holds a strong ref so
    // the returned pointer stays valid after the lock is released.
    if (!SourceFilePath.IsEmpty())
    {
        FString ResolvedPath = SourceFilePath;
        if (FPaths::IsRelative(ResolvedPath))
        {
            ResolvedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), ResolvedPath);
        }

        FScopeLock Lock(&DiskCacheCS);
        if (const TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe>* Found = DiskCache.Find(ResolvedPath))
        {
            return Found->Get();
        }
    }
    return nullptr;
}

const TArray<FGaussianSplatData>* UNiagaraGSDataInterface::GetSplatArray() const
{
    if (SplatAsset)
    {
        return &SplatAsset->SplatData;
    }
    if (const FGSDiskSplatData* Payload = ResolvedDiskPayload())
    {
        return &Payload->Splats;
    }
    return nullptr;
}

int32 UNiagaraGSDataInterface::GetResolvedSHDegree() const
{
    if (SplatAsset)
    {
        return SplatAsset->SHDegree;
    }
    if (const FGSDiskSplatData* Payload = ResolvedDiskPayload())
    {
        return Payload->SHDegree;
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  UNiagaraDataInterface overrides
// ─────────────────────────────────────────────────────────────────────────────

bool UNiagaraGSDataInterface::CopyToInternal(UNiagaraDataInterface* Destination) const
{
    if (!Super::CopyToInternal(Destination)) return false;
    UNiagaraGSDataInterface* Dest = CastChecked<UNiagaraGSDataInterface>(Destination);
    Dest->SplatAsset = SplatAsset;
    Dest->SourceFilePath = SourceFilePath;
    // Hand the duplicate the already-parsed payload so Niagara's DI duplication
    // never triggers a re-parse of the file.
    Dest->ResolvedDiskData = ResolvedDiskData;
    return true;
}

bool UNiagaraGSDataInterface::Equals(const UNiagaraDataInterface* Other) const
{
    if (!Super::Equals(Other)) return false;
    const UNiagaraGSDataInterface* OtherDI = CastChecked<const UNiagaraGSDataInterface>(Other);
    return OtherDI->SplatAsset == SplatAsset
        && OtherDI->SourceFilePath == SourceFilePath;
}

int32 UNiagaraGSDataInterface::GetSplatCount() const
{
    const TArray<FGaussianSplatData>* Splats = GetSplatArray();
    return Splats ? Splats->Num() : 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Flush entry points (global generation — no per-instance lifecycle)
// ─────────────────────────────────────────────────────────────────────────────

void UNiagaraGSDataInterface::RequestGlobalFlush()
{
    const uint64 NewGen = GlobalFlushGeneration.IncrementExchange() + 1;
    UE_LOG(LogTemp, Log,
        TEXT("NiagaraGS: Flush requested (generation %llu) — proxies release on next render"),
        NewGen);
}

uint64 UNiagaraGSDataInterface::GetFlushGeneration()
{
    return GlobalFlushGeneration.Load();
}

bool UNiagaraGSDataInterface::FlushBuffersForSystemInstance(FNiagaraSystemInstanceID InstanceID)
{
    // Per-system targeting was removed with the per-instance lifecycle; today this
    // triggers a global flush. (Use a separate DI/component per system to isolate.)
    RequestGlobalFlush();
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  PostInitProperties / PostLoad
// ─────────────────────────────────────────────────────────────────────────────

void UNiagaraGSDataInterface::PostInitProperties()
{
    Super::PostInitProperties();
    if (HasAnyFlags(RF_ClassDefaultObject))
    {
        ENiagaraTypeRegistryFlags DIFlags =
            ENiagaraTypeRegistryFlags::AllowAnyVariable |
            ENiagaraTypeRegistryFlags::AllowParameter;
        FNiagaraTypeRegistry::Register(
            FNiagaraTypeDefinition(GetClass()), DIFlags);
    }
    if (!HasAnyFlags(RF_ClassDefaultObject))
    {
        Proxy = MakeUnique<FNDIGaussianSplatProxy>();
    }
}

void UNiagaraGSDataInterface::PostLoad()
{
    Super::PostLoad();

    // Parse the disk file once on load (game thread) so the cache is populated
    // before the render thread needs it. Asset-backed DIs already hold their data.
    EnsureSplatDataLoaded();
}

#if WITH_EDITOR
void UNiagaraGSDataInterface::PostEditChangeProperty(
    FPropertyChangedEvent& PropertyChangedEvent)
{
    // When the source path or asset changes, drop the stale handle and re-resolve
    // immediately so the editor preview picks up the new data.
    const FName PropName = PropertyChangedEvent.GetPropertyName();
    if (PropName == GET_MEMBER_NAME_CHECKED(UNiagaraGSDataInterface, SourceFilePath) ||
        PropName == GET_MEMBER_NAME_CHECKED(UNiagaraGSDataInterface, SplatAsset))
    {
        ResolvedDiskData.Reset();
        EnsureSplatDataLoaded();
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

// ─────────────────────────────────────────────────────────────────────────────
//  GetFunctionsInternal (editor-only — function discovery for the compiler)
// ─────────────────────────────────────────────────────────────────────────────

#if WITH_EDITORONLY_DATA
void UNiagaraGSDataInterface::GetFunctionsInternal(TArray<FNiagaraFunctionSignature>& OutFunctions) const
{
    auto NDISelf = [this]() -> FNiagaraVariable
        {
            return FNiagaraVariable(
                FNiagaraTypeDefinition(GetClass()), TEXT("GaussianSplatDI"));
        };

    // GetSplatCount
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_GetSplatCount;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = true;
        Sig.Inputs.Add(NDISelf());
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetIntDef(), TEXT("Count")));
        OutFunctions.Add(Sig);
    }

    // GetSplatPosition
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_GetSplatPosition;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = true;
        Sig.Inputs.Add(NDISelf());
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetIntDef(), TEXT("Index")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetVec3Def(), TEXT("Position")));
        OutFunctions.Add(Sig);
    }

    // GetSplatScale
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_GetSplatScale;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = true;
        Sig.Inputs.Add(NDISelf());
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetIntDef(), TEXT("Index")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetVec3Def(), TEXT("Scale")));
        OutFunctions.Add(Sig);
    }

    // GetSplatOrientation (split floats to avoid Niagara quat type limits)
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_GetSplatOrientation;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = true;
        Sig.Inputs.Add(NDISelf());
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetIntDef(), TEXT("Index")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetFloatDef(), TEXT("QX")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetFloatDef(), TEXT("QY")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetFloatDef(), TEXT("QZ")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetFloatDef(), TEXT("QW")));
        OutFunctions.Add(Sig);
    }

    // GetSplatColor
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_GetSplatColor;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = true;
        Sig.Inputs.Add(NDISelf());
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetIntDef(), TEXT("Index")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetColorDef(), TEXT("Color")));
        OutFunctions.Add(Sig);
    }

    // GetSplatOpacity
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_GetSplatOpacity;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = true;
        Sig.Inputs.Add(NDISelf());
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetIntDef(), TEXT("Index")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetFloatDef(), TEXT("Opacity")));
        OutFunctions.Add(Sig);
    }

    // GetSplatSHColor (GPU SH evaluation)
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_GetSplatSHColor;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = true;
        Sig.Inputs.Add(NDISelf());
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetIntDef(), TEXT("Index")));
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetVec3Def(), TEXT("ViewDirection")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetVec3Def(), TEXT("Color")));
        OutFunctions.Add(Sig);
    }

    // GetSplatSHCoefficients (raw passthrough, 15 float3 outputs)
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_GetSplatSHCoefficients;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = true;
        Sig.Inputs.Add(NDISelf());
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetIntDef(), TEXT("Index")));
        // D1: 3 bases
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D1_Basis0")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D1_Basis1")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D1_Basis2")));
        // D2: 5 bases
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D2_Basis0")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D2_Basis1")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D2_Basis2")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D2_Basis3")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D2_Basis4")));
        // D3: 7 bases
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D3_Basis0")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D3_Basis1")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D3_Basis2")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D3_Basis3")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D3_Basis4")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D3_Basis5")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("SH_D3_Basis6")));
        OutFunctions.Add(Sig);
    }

    // FlushGPUBuffers — CPU only, targets THIS Niagara System's instance
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = Name_FlushGPUBuffers;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        Sig.bSupportsCPU = true;
        Sig.bSupportsGPU = false;  // GPU can't free its own buffers
        Sig.Inputs.Add(NDISelf());
        Sig.Inputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetBoolDef(), TEXT("Flush")));
        Sig.Outputs.Add(FNiagaraVariable(
            FNiagaraTypeDefinition::GetBoolDef(), TEXT("Success")));
        OutFunctions.Add(Sig);
    }
}

bool UNiagaraGSDataInterface::AppendCompileHash(FNiagaraCompileHashVisitor* InVisitor) const
{
    bool bSuccess = Super::AppendCompileHash(InVisitor);
    // Bump this string whenever the DI's VM/HLSL function calling convention can
    // change (e.g. PerInstanceDataSize switching, which adds/removes the hidden
    // user-pointer input). It forces a clean recompile and avoids stale bytecode.
    bSuccess &= InVisitor->UpdateString(TEXT("NiagaraGS_ABI"), TEXT("SingleBuffer_NoPerInstance_v1"));
    return bSuccess;
}
#endif // WITH_EDITORONLY_DATA

// ─────────────────────────────────────────────────────────────────────────────
//  VM function binding
// ─────────────────────────────────────────────────────────────────────────────

DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatCount);
DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatPosition);
DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatScale);
DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatOrientation);
DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatColor);
DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatOpacity);
DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatSHColor);
DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatSHCoefficients);
DEFINE_NDI_DIRECT_FUNC_BINDER(UNiagaraGSDataInterface, FlushGPUBuffersVM);

void UNiagaraGSDataInterface::GetVMExternalFunction(
    const FVMExternalFunctionBindingInfo& BindingInfo,
    void* InstanceData,
    FVMExternalFunction& OutFunc)
{
    if (BindingInfo.Name == Name_GetSplatCount)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatCount)::Bind(this, OutFunc);
    else if (BindingInfo.Name == Name_GetSplatPosition)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatPosition)::Bind(this, OutFunc);
    else if (BindingInfo.Name == Name_GetSplatScale)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatScale)::Bind(this, OutFunc);
    else if (BindingInfo.Name == Name_GetSplatOrientation)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatOrientation)::Bind(this, OutFunc);
    else if (BindingInfo.Name == Name_GetSplatColor)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatColor)::Bind(this, OutFunc);
    else if (BindingInfo.Name == Name_GetSplatOpacity)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatOpacity)::Bind(this, OutFunc);
    else if (BindingInfo.Name == Name_GetSplatSHColor)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatSHColor)::Bind(this, OutFunc);
    else if (BindingInfo.Name == Name_GetSplatSHCoefficients)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, GetSplatSHCoefficients)::Bind(this, OutFunc);
    else if (BindingInfo.Name == Name_FlushGPUBuffers)
        NDI_FUNC_BINDER(UNiagaraGSDataInterface, FlushGPUBuffersVM)::Bind(this, OutFunc);
}

// ─────────────────────────────────────────────────────────────────────────────
//  CPU VM implementations
//
//  PerInstanceDataSize() == 0, so Niagara does NOT prepend a per-instance user
//  pointer — these read their declared inputs directly. (If per-instance data is
//  ever reintroduced, every function must consume an FUserPtrHandler first.)
// ─────────────────────────────────────────────────────────────────────────────

void UNiagaraGSDataInterface::GetSplatCount(FVectorVMExternalFunctionContext& Context)
{
    FNDIOutputParam<int32> OutCount(Context);

    const int32 Count = GetSplatCount();
    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
        OutCount.SetAndAdvance(Count);
}

void UNiagaraGSDataInterface::GetSplatPosition(FVectorVMExternalFunctionContext& Context)
{
    FNDIInputParam<int32>  InIndex(Context);
    FNDIOutputParam<float> OutX(Context);
    FNDIOutputParam<float> OutY(Context);
    FNDIOutputParam<float> OutZ(Context);

    const TArray<FGaussianSplatData>* Splats = GetSplatArray();

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        const int32 Idx = InIndex.GetAndAdvance();
        if (Splats && Splats->IsValidIndex(Idx))
        {
            const FVector3f& P = (*Splats)[Idx].Position;
            OutX.SetAndAdvance(P.X);
            OutY.SetAndAdvance(P.Y);
            OutZ.SetAndAdvance(P.Z);
        }
        else
        {
            OutX.SetAndAdvance(0.f);
            OutY.SetAndAdvance(0.f);
            OutZ.SetAndAdvance(0.f);
        }
    }
}

void UNiagaraGSDataInterface::GetSplatScale(FVectorVMExternalFunctionContext& Context)
{
    FNDIInputParam<int32>  InIndex(Context);
    FNDIOutputParam<float> OutX(Context);
    FNDIOutputParam<float> OutY(Context);
    FNDIOutputParam<float> OutZ(Context);

    const TArray<FGaussianSplatData>* Splats = GetSplatArray();

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        const int32 Idx = InIndex.GetAndAdvance();
        if (Splats && Splats->IsValidIndex(Idx))
        {
            const FVector3f& S = (*Splats)[Idx].Scale;
            OutX.SetAndAdvance(S.X);
            OutY.SetAndAdvance(S.Y);
            OutZ.SetAndAdvance(S.Z);
        }
        else
        {
            OutX.SetAndAdvance(1.f);
            OutY.SetAndAdvance(1.f);
            OutZ.SetAndAdvance(1.f);
        }
    }
}

void UNiagaraGSDataInterface::GetSplatOrientation(FVectorVMExternalFunctionContext& Context)
{
    FNDIInputParam<int32>  InIndex(Context);
    FNDIOutputParam<float> OutQX(Context);
    FNDIOutputParam<float> OutQY(Context);
    FNDIOutputParam<float> OutQZ(Context);
    FNDIOutputParam<float> OutQW(Context);

    const TArray<FGaussianSplatData>* Splats = GetSplatArray();

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        const int32 Idx = InIndex.GetAndAdvance();
        if (Splats && Splats->IsValidIndex(Idx))
        {
            const FQuat4f& Q = (*Splats)[Idx].Orientation;
            OutQX.SetAndAdvance(Q.X);
            OutQY.SetAndAdvance(Q.Y);
            OutQZ.SetAndAdvance(Q.Z);
            OutQW.SetAndAdvance(Q.W);
        }
        else
        {
            OutQX.SetAndAdvance(0.f);
            OutQY.SetAndAdvance(0.f);
            OutQZ.SetAndAdvance(0.f);
            OutQW.SetAndAdvance(1.f);
        }
    }
}

void UNiagaraGSDataInterface::GetSplatColor(FVectorVMExternalFunctionContext& Context)
{
    FNDIInputParam<int32>  InIndex(Context);
    FNDIOutputParam<float> OutR(Context);
    FNDIOutputParam<float> OutG(Context);
    FNDIOutputParam<float> OutB(Context);
    FNDIOutputParam<float> OutA(Context);

    const TArray<FGaussianSplatData>* Splats = GetSplatArray();

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        const int32 Idx = InIndex.GetAndAdvance();
        if (Splats && Splats->IsValidIndex(Idx))
        {
            const FVector3f& C = (*Splats)[Idx].Color;
            OutR.SetAndAdvance(C.X);
            OutG.SetAndAdvance(C.Y);
            OutB.SetAndAdvance(C.Z);
            OutA.SetAndAdvance((*Splats)[Idx].Opacity);
        }
        else
        {
            OutR.SetAndAdvance(0.5f);
            OutG.SetAndAdvance(0.5f);
            OutB.SetAndAdvance(0.5f);
            OutA.SetAndAdvance(1.0f);
        }
    }
}

void UNiagaraGSDataInterface::GetSplatOpacity(FVectorVMExternalFunctionContext& Context)
{
    FNDIInputParam<int32>  InIndex(Context);
    FNDIOutputParam<float> OutOpacity(Context);

    const TArray<FGaussianSplatData>* Splats = GetSplatArray();

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        const int32 Idx = InIndex.GetAndAdvance();
        OutOpacity.SetAndAdvance(
            (Splats && Splats->IsValidIndex(Idx))
            ? (*Splats)[Idx].Opacity
            : 0.0f);
    }
}

void UNiagaraGSDataInterface::GetSplatSHColor(FVectorVMExternalFunctionContext& Context)
{
    FNDIInputParam<int32>     InIndex(Context);
    FNDIInputParam<FVector3f> InViewDir(Context);
    FNDIOutputParam<FVector3f> OutColor(Context);

    const TArray<FGaussianSplatData>* Splats = GetSplatArray();

    // CPU path returns the base (DC) color, gamma-corrected to match the GPU's
    // final pow(2.2). Full view-dependent SH evaluation is done on the GPU; if
    // you need it on CPU, sample GetSplatSHCoefficients and evaluate in a module.
    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        const int32 Idx = InIndex.GetAndAdvance();
        InViewDir.GetAndAdvance();

        if (Splats && Splats->IsValidIndex(Idx))
        {
            const FVector3f C = (*Splats)[Idx].Color;
            OutColor.SetAndAdvance(FVector3f(
                FMath::Pow(FMath::Max(C.X, 0.f), 2.2f),
                FMath::Pow(FMath::Max(C.Y, 0.f), 2.2f),
                FMath::Pow(FMath::Max(C.Z, 0.f), 2.2f)));
        }
        else
        {
            OutColor.SetAndAdvance(FVector3f(1.f, 1.f, 1.f));
        }
    }
}

void UNiagaraGSDataInterface::GetSplatSHCoefficients(FVectorVMExternalFunctionContext& Context)
{
    FNDIInputParam<int32> InIndex(Context);

    FNDIOutputParam<FVector3f> OutD1_B0(Context); FNDIOutputParam<FVector3f> OutD1_B1(Context); FNDIOutputParam<FVector3f> OutD1_B2(Context);
    FNDIOutputParam<FVector3f> OutD2_B0(Context); FNDIOutputParam<FVector3f> OutD2_B1(Context); FNDIOutputParam<FVector3f> OutD2_B2(Context);
    FNDIOutputParam<FVector3f> OutD2_B3(Context); FNDIOutputParam<FVector3f> OutD2_B4(Context);
    FNDIOutputParam<FVector3f> OutD3_B0(Context); FNDIOutputParam<FVector3f> OutD3_B1(Context); FNDIOutputParam<FVector3f> OutD3_B2(Context);
    FNDIOutputParam<FVector3f> OutD3_B3(Context); FNDIOutputParam<FVector3f> OutD3_B4(Context); FNDIOutputParam<FVector3f> OutD3_B5(Context);
    FNDIOutputParam<FVector3f> OutD3_B6(Context);

    const TArray<FGaussianSplatData>* Splats = GetSplatArray();

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        const int32 Idx = InIndex.GetAndAdvance();

        // 15 view-dependent SH bases (degree 1:3, degree 2:5, degree 3:7), each
        // an RGB triple. Anything not present in the file stays zero — matching
        // the GPU path so CPU and GPU emitters produce identical particle data.
        FVector3f Bases[15];
        for (int32 b = 0; b < 15; ++b) { Bases[b] = FVector3f::ZeroVector; }

        if (Splats && Splats->IsValidIndex(Idx))
        {
            // PLY layout is channel-major: [R_0..N-1, G_0..N-1, B_0..N-1].
            const TArray<float>& SH = (*Splats)[Idx].SHCoefficients;
            const int32 NumBases = SH.Num() / 3;
            const int32 MaxBasis = FMath::Min(NumBases, 15);
            for (int32 b = 0; b < MaxBasis; ++b)
            {
                Bases[b] = FVector3f(SH[b], SH[NumBases + b], SH[2 * NumBases + b]);
            }
        }

        // Band split: D1 = bases 0..2, D2 = bases 3..7, D3 = bases 8..14.
        OutD1_B0.SetAndAdvance(Bases[0]);  OutD1_B1.SetAndAdvance(Bases[1]);  OutD1_B2.SetAndAdvance(Bases[2]);
        OutD2_B0.SetAndAdvance(Bases[3]);  OutD2_B1.SetAndAdvance(Bases[4]);  OutD2_B2.SetAndAdvance(Bases[5]);
        OutD2_B3.SetAndAdvance(Bases[6]);  OutD2_B4.SetAndAdvance(Bases[7]);
        OutD3_B0.SetAndAdvance(Bases[8]);  OutD3_B1.SetAndAdvance(Bases[9]);  OutD3_B2.SetAndAdvance(Bases[10]);
        OutD3_B3.SetAndAdvance(Bases[11]); OutD3_B4.SetAndAdvance(Bases[12]); OutD3_B5.SetAndAdvance(Bases[13]);
        OutD3_B6.SetAndAdvance(Bases[14]);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  FlushGPUBuffersVM
//
//  No per-instance data (PerInstanceDataSize()==0), so this raises a global flush
//  generation; every live Gaussian Splat proxy releases its VRAM on its next render.
//  Edge-latched so holding the bool high only triggers one flush.
// ─────────────────────────────────────────────────────────────────────────────

void UNiagaraGSDataInterface::FlushGPUBuffersVM(FVectorVMExternalFunctionContext& Context)
{
    FNDIInputParam<bool>  InFlush(Context);
    FNDIOutputParam<bool> OutSuccess(Context);

    bool bShouldFlush = false;
    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        bShouldFlush |= InFlush.GetAndAdvance();
    }

    if (bShouldFlush)
    {
        if (!bFlushLatch)
        {
            RequestGlobalFlush();   // rising edge only
            bFlushLatch = true;
        }
    }
    else
    {
        bFlushLatch = false;        // reset so the next true edge flushes again
    }

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
        OutSuccess.SetAndAdvance(bShouldFlush);
}

// ─────────────────────────────────────────────────────────────────────────────
//  GPU shader parameter plumbing
// ─────────────────────────────────────────────────────────────────────────────

void UNiagaraGSDataInterface::BuildShaderParameters(
    FNiagaraShaderParametersBuilder& ShaderParametersBuilder) const
{
    ShaderParametersBuilder.AddNestedStruct<FNiagaraGSShaderParameters>();
}

void UNiagaraGSDataInterface::SetShaderParameters(
    const FNiagaraDataInterfaceSetShaderParametersContext& Context) const
{
    FNDIGaussianSplatProxy& SplatProxy = Context.GetProxy<FNDIGaussianSplatProxy>();

    // Guarantee the shared fallback buffer exists before binding anything.
    SplatProxy.InitFallbackBuffer();

    FNiagaraGSShaderParameters* Params =
        Context.GetParameterNestedStruct<FNiagaraGSShaderParameters>();
    if (!Params) return;

    // This runs on the real rendering proxy, so everything here lands on exactly
    // the buffers the GPU draws from.

    // 1) Consume a pending flush: release the splat VRAM once when the global
    //    generation advances past the one this proxy last serviced.
    const uint64 CurrentGen = GetFlushGeneration();
    if (CurrentGen > SplatProxy.FlushedGeneration)
    {
        SplatProxy.ReleaseBuffers();
        SplatProxy.bManuallyFlushed = true;
        SplatProxy.FlushedGeneration = CurrentGen;
    }

    // 2) Self-heal: upload the buffers once from the resolved CPU data (asset or
    //    path-cached). This is what puts real data on the GPU and restores colour.
    //    A manual flush is respected — we do not re-upload after it.
    if (!SplatProxy.bBuffersReady && !SplatProxy.bManuallyFlushed)
    {
        const TArray<FGaussianSplatData>* Splats = GetSplatArray();
        if (Splats && Splats->Num() > 0)
        {
            SplatProxy.UploadData(Splats->GetData(), Splats->Num(), GetResolvedSHDegree());
        }
    }

    // 3) Bind.
    if (SplatProxy.bBuffersReady)
    {
        Params->SplatCount = SplatProxy.SplatCount;
        Params->SHDegree = SplatProxy.SHDegree;
        Params->Positions = SplatProxy.PositionsBuffer.SRV;
        Params->Scales = SplatProxy.ScalesBuffer.SRV;
        Params->Rotations = SplatProxy.RotationsBuffer.SRV;
        Params->ColorOpacity = SplatProxy.ColorOpacityBuffer.SRV;
        Params->SHCoefficients = SplatProxy.SHCoefficientsBuffer.SRV;
    }
    else
    {
        // Fallback: bind zeroed buffer so shaders never see a null SRV.
        FRHIShaderResourceView* Fallback = SplatProxy.FallbackBuffer.SRV;
        Params->SplatCount = 0;
        Params->SHDegree = 0;
        Params->Positions = Fallback;
        Params->Scales = Fallback;
        Params->Rotations = Fallback;
        Params->ColorOpacity = Fallback;
        Params->SHCoefficients = Fallback;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  HLSL generation (editor-only — cooked builds ship the compiled shaders)
// ─────────────────────────────────────────────────────────────────────────────

#if WITH_EDITORONLY_DATA
void UNiagaraGSDataInterface::GetParameterDefinitionHLSL(
    const FNiagaraDataInterfaceGPUParamInfo& ParamInfo,
    FString& OutHLSL)
{
    const FString& S = ParamInfo.DataInterfaceHLSLSymbol;

    OutHLSL += FString::Printf(TEXT("int %s_SplatCount;\n"), *S);
    OutHLSL += FString::Printf(TEXT("int %s_SHDegree;\n"), *S);
    OutHLSL += FString::Printf(TEXT("Buffer<float4> %s_Positions;\n"), *S);
    OutHLSL += FString::Printf(TEXT("Buffer<float4> %s_Scales;\n"), *S);
    OutHLSL += FString::Printf(TEXT("Buffer<float4> %s_Rotations;\n"), *S);
    OutHLSL += FString::Printf(TEXT("Buffer<float4> %s_ColorOpacity;\n"), *S);
    OutHLSL += FString::Printf(TEXT("Buffer<float4> %s_SHCoefficients;\n\n"), *S);

    // GetSplatCount
    OutHLSL += FString::Printf(TEXT(
        "void %s_GetSplatCount(out int OutCount)\n"
        "{\n"
        "    OutCount = %s_SplatCount;\n"
        "}\n\n"), *S, *S);

    // GetSplatPosition
    OutHLSL += FString::Printf(TEXT(
        "void %s_GetSplatPosition(int Index, out float3 OutPosition)\n"
        "{\n"
        "    OutPosition = (Index >= 0 && Index < %s_SplatCount)\n"
        "        ? %s_Positions[Index].xyz : float3(0,0,0);\n"
        "}\n\n"), *S, *S, *S);

    // GetSplatScale
    OutHLSL += FString::Printf(TEXT(
        "void %s_GetSplatScale(int Index, out float3 OutScale)\n"
        "{\n"
        "    OutScale = (Index >= 0 && Index < %s_SplatCount)\n"
        "        ? %s_Scales[Index].xyz : float3(1,1,1);\n"
        "}\n\n"), *S, *S, *S);

    // GetSplatOrientation
    OutHLSL += FString::Printf(TEXT(
        "void %s_GetSplatOrientation(int Index,\n"
        "    out float OutQX, out float OutQY, out float OutQZ, out float OutQW)\n"
        "{\n"
        "    if (Index >= 0 && Index < %s_SplatCount)\n"
        "    {\n"
        "        float4 Q = %s_Rotations[Index];\n"
        "        OutQX = Q.x; OutQY = Q.y; OutQZ = Q.z; OutQW = Q.w;\n"
        "    }\n"
        "    else { OutQX = 0; OutQY = 0; OutQZ = 0; OutQW = 1; }\n"
        "}\n\n"), *S, *S, *S);

    // GetSplatColor
    OutHLSL += FString::Printf(TEXT(
        "void %s_GetSplatColor(int Index, out float4 OutColor)\n"
        "{\n"
        "    OutColor = (Index >= 0 && Index < %s_SplatCount)\n"
        "        ? float4(%s_ColorOpacity[Index].xyz, 1.0)\n"
        "        : float4(0.5, 0.5, 0.5, 1.0);\n"
        "}\n\n"), *S, *S, *S);

    // GetSplatOpacity
    OutHLSL += FString::Printf(TEXT(
        "void %s_GetSplatOpacity(int Index, out float OutOpacity)\n"
        "{\n"
        "    OutOpacity = (Index >= 0 && Index < %s_SplatCount)\n"
        "        ? %s_ColorOpacity[Index].w : 0.0;\n"
        "}\n\n"), *S, *S, *S);

    // GetSplatSHColor — full degree 1/2/3 SH evaluation
    OutHLSL += FString::Printf(TEXT(
        "void %s_GetSplatSHColor(int Index, float3 ViewDir, out float3 OutColor)\n"
        "{\n"
        "    if (Index >= 0 && Index < %s_SplatCount)\n"
        "    {\n"
        "        float3 Color = %s_ColorOpacity[Index].xyz;\n"
        "        if (%s_SHDegree < 1) { OutColor = pow(max(Color, 0.0), 2.2); return; }\n"
        "        int Base = Index * 12;\n"
        "        float sh[45];\n"
        "        for (int i = 0; i < 11; i++)\n"
        "        {\n"
        "            float4 v = %s_SHCoefficients[Base + i];\n"
        "            sh[i*4+0]=v.x; sh[i*4+1]=v.y; sh[i*4+2]=v.z; sh[i*4+3]=v.w;\n"
        "        }\n"
        "        sh[44] = %s_SHCoefficients[Base + 11].x;\n"
        "        float3 Dir = normalize(-ViewDir);\n"
        "        float x=Dir.x, y=Dir.y, z=Dir.z;\n"
        "        float xx=x*x, yy=y*y, zz=z*z, xy=x*y, yz=y*z, xz=x*z;\n"
        "        const float SH_C1 = 0.4886025119029199;\n"
        "        Color.r += SH_C1*(-y*sh[0] + z*sh[3] - x*sh[6]);\n"
        "        Color.g += SH_C1*(-y*sh[1] + z*sh[4] - x*sh[7]);\n"
        "        Color.b += SH_C1*(-y*sh[2] + z*sh[5] - x*sh[8]);\n"
        "        if (%s_SHDegree > 1)\n"
        "        {\n"
        "            const float SH_C2_0= 1.0925484305920792;\n"
        "            const float SH_C2_1=-1.0925484305920792;\n"
        "            const float SH_C2_2= 0.31539156525252005;\n"
        "            const float SH_C2_3=-1.0925484305920792;\n"
        "            const float SH_C2_4= 0.5462742152960396;\n"
        "            float b2_0=SH_C2_0*xy, b2_1=SH_C2_1*yz;\n"
        "            float b2_2=SH_C2_2*(2.0*zz-xx-yy);\n"
        "            float b2_3=SH_C2_3*xz, b2_4=SH_C2_4*(xx-yy);\n"
        "            Color.r+=b2_0*sh[9] +b2_1*sh[12]+b2_2*sh[15]+b2_3*sh[18]+b2_4*sh[21];\n"
        "            Color.g+=b2_0*sh[10]+b2_1*sh[13]+b2_2*sh[16]+b2_3*sh[19]+b2_4*sh[22];\n"
        "            Color.b+=b2_0*sh[11]+b2_1*sh[14]+b2_2*sh[17]+b2_3*sh[20]+b2_4*sh[23];\n"
        "            if (%s_SHDegree > 2)\n"
        "            {\n"
        "                const float SH_C3_0=-0.5900435899266435;\n"
        "                const float SH_C3_1= 2.890611442640554;\n"
        "                const float SH_C3_2=-0.4570457994644658;\n"
        "                const float SH_C3_3= 0.3731763325901154;\n"
        "                const float SH_C3_4=-0.4570457994644658;\n"
        "                const float SH_C3_5= 1.445305721320277;\n"
        "                const float SH_C3_6=-0.5900435899266435;\n"
        "                float b3_0=SH_C3_0*y*(3.0*xx-yy);\n"
        "                float b3_1=SH_C3_1*xy*z;\n"
        "                float b3_2=SH_C3_2*y*(4.0*zz-xx-yy);\n"
        "                float b3_3=SH_C3_3*z*(2.0*zz-3.0*xx-3.0*yy);\n"
        "                float b3_4=SH_C3_4*x*(4.0*zz-xx-yy);\n"
        "                float b3_5=SH_C3_5*z*(xx-yy);\n"
        "                float b3_6=SH_C3_6*x*(xx-3.0*yy);\n"
        "                Color.r+=b3_0*sh[24]+b3_1*sh[27]+b3_2*sh[30]+b3_3*sh[33]+b3_4*sh[36]+b3_5*sh[39]+b3_6*sh[42];\n"
        "                Color.g+=b3_0*sh[25]+b3_1*sh[28]+b3_2*sh[31]+b3_3*sh[34]+b3_4*sh[37]+b3_5*sh[40]+b3_6*sh[43];\n"
        "                Color.b+=b3_0*sh[26]+b3_1*sh[29]+b3_2*sh[32]+b3_3*sh[35]+b3_4*sh[38]+b3_5*sh[41]+b3_6*sh[44];\n"
        "            }\n"
        "        }\n"
        "        OutColor = pow(max(Color, 0.0), 2.2);\n"
        "    }\n"
        "    else { OutColor = float3(1,1,1); }\n"
        "}\n\n"),
        *S, *S, *S, *S, *S, *S, *S, *S);

    // GetSplatSHCoefficients — raw passthrough, no math
    OutHLSL += FString::Printf(TEXT(
        "void %s_GetSplatSHCoefficients(\n"
        "    int Index,\n"
        "    out float3 OutD1_B0, out float3 OutD1_B1, out float3 OutD1_B2,\n"
        "    out float3 OutD2_B0, out float3 OutD2_B1, out float3 OutD2_B2,\n"
        "    out float3 OutD2_B3, out float3 OutD2_B4,\n"
        "    out float3 OutD3_B0, out float3 OutD3_B1, out float3 OutD3_B2,\n"
        "    out float3 OutD3_B3, out float3 OutD3_B4, out float3 OutD3_B5,\n"
        "    out float3 OutD3_B6)\n"
        "{\n"
        "    OutD1_B0=OutD1_B1=OutD1_B2=float3(0,0,0);\n"
        "    OutD2_B0=OutD2_B1=OutD2_B2=OutD2_B3=OutD2_B4=float3(0,0,0);\n"
        "    OutD3_B0=OutD3_B1=OutD3_B2=OutD3_B3=OutD3_B4=OutD3_B5=OutD3_B6=float3(0,0,0);\n"
        "    if (Index < 0 || Index >= %s_SplatCount) return;\n"
        "    int Base = Index * 12;\n"
        "    float sh[48];\n"
        "    [unroll] for (int i = 0; i < 12; i++)\n"
        "    {\n"
        "        float4 v = %s_SHCoefficients[Base + i];\n"
        "        sh[i*4+0]=v.x; sh[i*4+1]=v.y; sh[i*4+2]=v.z; sh[i*4+3]=v.w;\n"
        "    }\n"
        "    if (%s_SHDegree >= 1)\n"
        "    {\n"
        "        OutD1_B0=float3(sh[0],sh[1],sh[2]);\n"
        "        OutD1_B1=float3(sh[3],sh[4],sh[5]);\n"
        "        OutD1_B2=float3(sh[6],sh[7],sh[8]);\n"
        "    }\n"
        "    if (%s_SHDegree >= 2)\n"
        "    {\n"
        "        OutD2_B0=float3(sh[9], sh[10],sh[11]);\n"
        "        OutD2_B1=float3(sh[12],sh[13],sh[14]);\n"
        "        OutD2_B2=float3(sh[15],sh[16],sh[17]);\n"
        "        OutD2_B3=float3(sh[18],sh[19],sh[20]);\n"
        "        OutD2_B4=float3(sh[21],sh[22],sh[23]);\n"
        "    }\n"
        "    if (%s_SHDegree >= 3)\n"
        "    {\n"
        "        OutD3_B0=float3(sh[24],sh[25],sh[26]);\n"
        "        OutD3_B1=float3(sh[27],sh[28],sh[29]);\n"
        "        OutD3_B2=float3(sh[30],sh[31],sh[32]);\n"
        "        OutD3_B3=float3(sh[33],sh[34],sh[35]);\n"
        "        OutD3_B4=float3(sh[36],sh[37],sh[38]);\n"
        "        OutD3_B5=float3(sh[39],sh[40],sh[41]);\n"
        "        OutD3_B6=float3(sh[42],sh[43],sh[44]);\n"
        "    }\n"
        "}\n\n"),
        *S, *S, *S, *S, *S, *S);
}

bool UNiagaraGSDataInterface::GetFunctionHLSL(
    const FNiagaraDataInterfaceGPUParamInfo& ParamInfo,
    const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo,
    int FunctionInstanceIndex,
    FString& OutHLSL)
{
    const FString& S = ParamInfo.DataInterfaceHLSLSymbol;
    const FString& N = FunctionInfo.InstanceName;

    if (FunctionInfo.DefinitionName == Name_GetSplatCount)
    {
        OutHLSL += FString::Printf(TEXT(
            "void %s(out int OutCount)\n{ %s_GetSplatCount(OutCount); }\n"), *N, *S);
        return true;
    }
    if (FunctionInfo.DefinitionName == Name_GetSplatPosition)
    {
        OutHLSL += FString::Printf(TEXT(
            "void %s(int Index, out float3 OutPosition)\n"
            "{ %s_GetSplatPosition(Index, OutPosition); }\n"), *N, *S);
        return true;
    }
    if (FunctionInfo.DefinitionName == Name_GetSplatScale)
    {
        OutHLSL += FString::Printf(TEXT(
            "void %s(int Index, out float3 OutScale)\n"
            "{ %s_GetSplatScale(Index, OutScale); }\n"), *N, *S);
        return true;
    }
    if (FunctionInfo.DefinitionName == Name_GetSplatOrientation)
    {
        OutHLSL += FString::Printf(TEXT(
            "void %s(int Index, out float OutQX, out float OutQY,\n"
            "    out float OutQZ, out float OutQW)\n"
            "{ %s_GetSplatOrientation(Index, OutQX, OutQY, OutQZ, OutQW); }\n"), *N, *S);
        return true;
    }
    if (FunctionInfo.DefinitionName == Name_GetSplatColor)
    {
        OutHLSL += FString::Printf(TEXT(
            "void %s(int Index, out float4 OutColor)\n"
            "{ %s_GetSplatColor(Index, OutColor); }\n"), *N, *S);
        return true;
    }
    if (FunctionInfo.DefinitionName == Name_GetSplatOpacity)
    {
        OutHLSL += FString::Printf(TEXT(
            "void %s(int Index, out float OutOpacity)\n"
            "{ %s_GetSplatOpacity(Index, OutOpacity); }\n"), *N, *S);
        return true;
    }
    if (FunctionInfo.DefinitionName == Name_GetSplatSHColor)
    {
        OutHLSL += FString::Printf(TEXT(
            "void %s(int Index, float3 ViewDir, out float3 OutColor)\n"
            "{ %s_GetSplatSHColor(Index, ViewDir, OutColor); }\n"), *N, *S);
        return true;
    }
    if (FunctionInfo.DefinitionName == Name_GetSplatSHCoefficients)
    {
        OutHLSL += FString::Printf(TEXT(
            "void %s(\n"
            "    int Index,\n"
            "    out float3 OutD1_B0, out float3 OutD1_B1, out float3 OutD1_B2,\n"
            "    out float3 OutD2_B0, out float3 OutD2_B1, out float3 OutD2_B2,\n"
            "    out float3 OutD2_B3, out float3 OutD2_B4,\n"
            "    out float3 OutD3_B0, out float3 OutD3_B1, out float3 OutD3_B2,\n"
            "    out float3 OutD3_B3, out float3 OutD3_B4, out float3 OutD3_B5,\n"
            "    out float3 OutD3_B6)\n"
            "{\n"
            "    %s_GetSplatSHCoefficients(Index,\n"
            "        OutD1_B0,OutD1_B1,OutD1_B2,\n"
            "        OutD2_B0,OutD2_B1,OutD2_B2,OutD2_B3,OutD2_B4,\n"
            "        OutD3_B0,OutD3_B1,OutD3_B2,OutD3_B3,OutD3_B4,OutD3_B5,OutD3_B6);\n"
            "}\n"), *N, *S);
        return true;
    }
    return false;
}
#endif // WITH_EDITORONLY_DATA