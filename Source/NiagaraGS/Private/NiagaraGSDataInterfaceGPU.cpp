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
    const TArray<FVector4f>& PackedPositions,
    const TArray<FVector4f>& PackedScales,
    const TArray<FVector4f>& PackedRotations,
    const TArray<FVector4f>& PackedColorOpacity,
    const TArray<FVector4f>& PackedSH,
    int32 InSHDegree)
{
    check(IsInRenderingThread());
    InitFallbackBuffer();

    ReleaseBuffers();

    const int32 Count = PackedPositions.Num();
    if (Count <= 0)
    {
        return;
    }

    // Pure RHI work from here: the CPU-side per-splat packing already happened
    // once, at load time (PackSplatsForGPU, off the render thread — see
    // FGSDiskSplatData). No per-splat loop on the render thread.
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

    constexpr int32 SH_VEC4S_PER_SPLAT = SH_VEC4_PER_SPLAT;   // 12
    const int64 TotalBytes = (int64)Count * sizeof(FVector4f) * (4 + SH_VEC4S_PER_SPLAT);
    UE_LOG(LogTemp, Log,
        TEXT("NiagaraGS: Uploaded %d splats (~%lld MB VRAM, SH degree %d)"),
        Count, TotalBytes / (1024 * 1024), InSHDegree);
}
