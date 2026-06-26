#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIResources.h"
#include "RenderResource.h"
#include "RenderingThread.h"
#include "RHICommandList.h"
#include "NiagaraDataInterfaceRW.h"   // pulls in FNiagaraDataInterfaceProxy
#include "NiagaraCommon.h"
#include "GaussianSplatData.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Shader parameter struct
// ─────────────────────────────────────────────────────────────────────────────
BEGIN_SHADER_PARAMETER_STRUCT(FNiagaraGSShaderParameters, )
    SHADER_PARAMETER(int32, SplatCount)
    SHADER_PARAMETER(int32, SHDegree)
    SHADER_PARAMETER_SRV(Buffer<float4>, Positions)
    SHADER_PARAMETER_SRV(Buffer<float4>, Scales)
    SHADER_PARAMETER_SRV(Buffer<float4>, Rotations)
    SHADER_PARAMETER_SRV(Buffer<float4>, ColorOpacity)
    SHADER_PARAMETER_SRV(Buffer<float4>, SHCoefficients)
END_SHADER_PARAMETER_STRUCT()


// ─────────────────────────────────────────────────────────────────────────────
//  One GPU buffer + SRV pair
// ─────────────────────────────────────────────────────────────────────────────
struct FNiagaraGSSplatBuffer
{
    FBufferRHIRef             Buffer;
    FShaderResourceViewRHIRef SRV;

    bool IsValid() const { return Buffer.IsValid(); }

    void Release()
    {
        SRV.SafeRelease();
        Buffer.SafeRelease();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
//  Render-thread proxy — ONE buffer set per data interface.
//
//  The DI deliberately uses PerInstanceDataSize() == 0, so Niagara never runs the
//  per-instance Init/Destroy lifecycle (that lifecycle was what churned and broke
//  the GPU path). The proxy is created once with the DI and lives with it.
//
//  Buffers are uploaded lazily (once) by SetShaderParameters — which always runs
//  on THIS exact rendering proxy — so the data lands where the GPU actually reads.
//
//  FlushGeneration: a manual flush bumps a global counter; the proxy releases its
//  buffers the next time it renders and records the generation it serviced.
// ─────────────────────────────────────────────────────────────────────────────
struct FNDIGaussianSplatProxy : public FNiagaraDataInterfaceProxy
{
    FNiagaraGSSplatBuffer PositionsBuffer;
    FNiagaraGSSplatBuffer ScalesBuffer;
    FNiagaraGSSplatBuffer RotationsBuffer;
    FNiagaraGSSplatBuffer ColorOpacityBuffer;
    FNiagaraGSSplatBuffer SHCoefficientsBuffer;

    int32  SHDegree         = 0;
    int32  SplatCount       = 0;
    bool   bBuffersReady    = false;
    bool   bManuallyFlushed = false;
    uint64 FlushedGeneration = 0;

    // Shared zeroed fallback — bound when buffers aren't ready or were flushed,
    // so the shader never sees a null SRV.
    FNiagaraGSSplatBuffer FallbackBuffer;

    virtual int32 PerInstanceDataPassedToRenderThreadSize() const override { return 0; }

    /** Upload the splat buffers (render thread). Resets bManuallyFlushed. */
    void UploadData(const FGaussianSplatData* Data, int32 Count, int32 InSHDegree);

    /** Release the splat buffers, keep the fallback (render thread). */
    void ReleaseBuffers();

    /** Ensure the shared fallback buffer exists (idempotent, render thread). */
    void InitFallbackBuffer();

    // GC can run the destructor on the game thread, so RHI releases are marshalled
    // onto the render thread.
    virtual ~FNDIGaussianSplatProxy()
    {
        FNiagaraGSSplatBuffer P = PositionsBuffer, S = ScalesBuffer, R = RotationsBuffer,
                              C = ColorOpacityBuffer, SH = SHCoefficientsBuffer, F = FallbackBuffer;

        ENQUEUE_RENDER_COMMAND(NiagaraGS_ProxyReleaseRHI)(
            [P, S, R, C, SH, F](FRHICommandListImmediate&) mutable
            {
                P.Release(); S.Release(); R.Release(); C.Release(); SH.Release(); F.Release();
            });
    }
};
