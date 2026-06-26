#include "NiagaraGSDataInterfaceGPU.h"
#include "RHICommandList.h"
#include "RenderingThread.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Local helper — create a typed Buffer<float4> + SRV (UE 5.6 RHI API)
// ─────────────────────────────────────────────────────────────────────────────
namespace NiagaraGSGPULocal
{
    static void CreateFloat4Buffer(
        FRHICommandListBase&     CmdList,
        FNiagaraGSSplatBuffer&   OutBuffer,
        const TArray<FVector4f>& Data,
        const TCHAR*             DebugName)
    {
        const int32 Sz = Data.Num() * sizeof(FVector4f);
        if (Sz <= 0)
        {
            return;
        }

        FRHIBufferCreateDesc Desc =
            FRHIBufferCreateDesc::Create(DebugName,
                EBufferUsageFlags::Static | EBufferUsageFlags::ShaderResource)
            .SetSize(Sz)
            .SetStride(sizeof(FVector4f))
            .SetInitialState(ERHIAccess::SRVCompute);

        OutBuffer.Buffer = CmdList.CreateBuffer(Desc);

        void* Dest = CmdList.LockBuffer(OutBuffer.Buffer, 0, Sz, RLM_WriteOnly);
        FMemory::Memcpy(Dest, Data.GetData(), Sz);
        CmdList.UnlockBuffer(OutBuffer.Buffer);

        OutBuffer.SRV = CmdList.CreateShaderResourceView(
            OutBuffer.Buffer,
            FRHIViewDesc::CreateBufferSRV()
                .SetType(FRHIViewDesc::EBufferType::Typed)
                .SetFormat(PF_A32B32G32R32F));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Fallback buffer — one zeroed float4, always valid
// ─────────────────────────────────────────────────────────────────────────────

void FNDIGaussianSplatProxy::InitFallbackBuffer()
{
    if (FallbackBuffer.IsValid()) return;
    check(IsInRenderingThread());

    FRHICommandListImmediate& CmdList = FRHICommandListExecutor::GetImmediateCommandList();

    TArray<FVector4f> ZeroData;
    ZeroData.Add(FVector4f(0.f, 0.f, 0.f, 0.f));

    NiagaraGSGPULocal::CreateFloat4Buffer(CmdList, FallbackBuffer, ZeroData, TEXT("GS_Fallback"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  ReleaseBuffers — free the splat VRAM, keep the fallback
// ─────────────────────────────────────────────────────────────────────────────

void FNDIGaussianSplatProxy::ReleaseBuffers()
{
    check(IsInRenderingThread());
    PositionsBuffer.Release();
    ScalesBuffer.Release();
    RotationsBuffer.Release();
    ColorOpacityBuffer.Release();
    SHCoefficientsBuffer.Release();
    bBuffersReady = false;
    SplatCount    = 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  UploadData (render thread)
// ─────────────────────────────────────────────────────────────────────────────

void FNDIGaussianSplatProxy::UploadData(
    const FGaussianSplatData* Data, int32 Count, int32 InSHDegree)
{
    check(IsInRenderingThread());
    InitFallbackBuffer();

    ReleaseBuffers();

    if (!Data || Count <= 0)
    {
        return;
    }

    constexpr int32 SH_VEC4S_PER_SPLAT = SH_VEC4_PER_SPLAT;   // 12

    TArray<FVector4f> PackedPositions, PackedScales, PackedRotations, PackedColorOpacity;
    PackedPositions.SetNumUninitialized(Count);
    PackedScales.SetNumUninitialized(Count);
    PackedRotations.SetNumUninitialized(Count);
    PackedColorOpacity.SetNumUninitialized(Count);

    TArray<FVector4f> PackedSH;
    PackedSH.SetNumZeroed(Count * SH_VEC4S_PER_SPLAT);

    for (int32 i = 0; i < Count; ++i)
    {
        const FGaussianSplatData& S = Data[i];

        PackedPositions[i]    = FVector4f(S.Position.X,    S.Position.Y,    S.Position.Z,    0.f);
        PackedScales[i]       = FVector4f(S.Scale.X,       S.Scale.Y,       S.Scale.Z,       0.f);
        PackedRotations[i]    = FVector4f(S.Orientation.X, S.Orientation.Y, S.Orientation.Z, S.Orientation.W);
        PackedColorOpacity[i] = FVector4f(S.Color.X,       S.Color.Y,       S.Color.Z,       S.Opacity);

        // Re-interleave SH: PLY channel-major → GPU basis-major
        const int32 NumCoefs = FMath::Min(S.SHCoefficients.Num(), SH_COEFFS_PER_SPLAT);
        const int32 NumBases = NumCoefs / 3;
        const int32 BaseIdx  = i * SH_VEC4S_PER_SPLAT;

        for (int32 b = 0; b < NumBases; ++b)
        {
            const float r  = S.SHCoefficients[b];
            const float g  = S.SHCoefficients[NumBases + b];
            const float bv = S.SHCoefficients[2 * NumBases + b];

            auto WriteFloat = [&](int32 FlatIndex, float Value)
            {
                reinterpret_cast<float*>(&PackedSH[BaseIdx + FlatIndex / 4])[FlatIndex % 4] = Value;
            };

            WriteFloat(b * 3 + 0, r);
            WriteFloat(b * 3 + 1, g);
            WriteFloat(b * 3 + 2, bv);
        }
    }

    FRHICommandListImmediate& CmdList = FRHICommandListExecutor::GetImmediateCommandList();

    using NiagaraGSGPULocal::CreateFloat4Buffer;
    CreateFloat4Buffer(CmdList, PositionsBuffer,      PackedPositions,    TEXT("GS_Positions"));
    CreateFloat4Buffer(CmdList, ScalesBuffer,         PackedScales,       TEXT("GS_Scales"));
    CreateFloat4Buffer(CmdList, RotationsBuffer,      PackedRotations,    TEXT("GS_Rotations"));
    CreateFloat4Buffer(CmdList, ColorOpacityBuffer,   PackedColorOpacity, TEXT("GS_ColorOpacity"));
    CreateFloat4Buffer(CmdList, SHCoefficientsBuffer, PackedSH,           TEXT("GS_SH"));

    SHDegree         = InSHDegree;
    SplatCount       = Count;
    bBuffersReady    = true;
    bManuallyFlushed = false;

    const int64 TotalBytes = (int64)Count * sizeof(FVector4f) * (4 + SH_VEC4S_PER_SPLAT);
    UE_LOG(LogTemp, Log,
        TEXT("NiagaraGS: Uploaded %d splats (~%lld MB VRAM, SH degree %d)"),
        Count, TotalBytes / (1024 * 1024), InSHDegree);
}
