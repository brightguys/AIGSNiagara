// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "NiagaraGSDataInterface.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeNiagaraGSDataInterface() {}

// ********** Begin Cross Module References ********************************************************
NIAGARA_API UClass* Z_Construct_UClass_UNiagaraDataInterface();
NIAGARAGS_API UClass* Z_Construct_UClass_UGaussianSplatAsset_NoRegister();
NIAGARAGS_API UClass* Z_Construct_UClass_UNiagaraGSDataInterface();
NIAGARAGS_API UClass* Z_Construct_UClass_UNiagaraGSDataInterface_NoRegister();
UPackage* Z_Construct_UPackage__Script_NiagaraGS();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UNiagaraGSDataInterface **************************************************
void UNiagaraGSDataInterface::StaticRegisterNativesUNiagaraGSDataInterface()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UNiagaraGSDataInterface;
UClass* UNiagaraGSDataInterface::GetPrivateStaticClass()
{
	using TClass = UNiagaraGSDataInterface;
	if (!Z_Registration_Info_UClass_UNiagaraGSDataInterface.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("NiagaraGSDataInterface"),
			Z_Registration_Info_UClass_UNiagaraGSDataInterface.InnerSingleton,
			StaticRegisterNativesUNiagaraGSDataInterface,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UNiagaraGSDataInterface.InnerSingleton;
}
UClass* Z_Construct_UClass_UNiagaraGSDataInterface_NoRegister()
{
	return UNiagaraGSDataInterface::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UNiagaraGSDataInterface_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Category", "Gaussian Splats" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Custom Niagara Data Interface exposing Gaussian Splat data to GPU particles.\n *\n * ARCHITECTURE\n * \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n * The DI deliberately uses PerInstanceDataSize() == 0, so Niagara never runs the\n * per-instance Init/Destroy lifecycle. (An earlier per-instance design churned \xe2\x80\x94\n * constant init/destroy \xe2\x80\x94 which broke the GPU path and tanked performance.) The\n * render-thread proxy is created once with the DI and owns ONE GPU buffer set.\n *\n *   \xe2\x80\xa2 SetShaderParameters() runs on the real rendering proxy. It lazily uploads\n *     the buffers once (self-heal) from the resolved CPU data, then binds them.\n *   \xe2\x80\xa2 FlushGPUBuffers (scratchpad bool) or the Blueprint node bump a global flush\n *     generation; the proxy releases its buffers the next time it renders and\n *     binds the zeroed fallback so the shader never sees a null SRV.\n *\n * DATA SOURCE\n * \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n *   \xe2\x80\xa2 SplatAsset      \xe2\x80\x94 an imported UGaussianSplatAsset (.ply dragged into UE), or\n *   \xe2\x80\xa2 SourceFilePath  \xe2\x80\x94 a raw .ply path read straight off disk, no import. Parsed\n *                       once and cached process-wide.\n * SplatAsset takes priority when both are set.\n * \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n */" },
#endif
		{ "DisplayName", "Gaussian Splat Data Interface" },
		{ "IncludePath", "NiagaraGSDataInterface.h" },
		{ "ModuleRelativePath", "Public/NiagaraGSDataInterface.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Custom Niagara Data Interface exposing Gaussian Splat data to GPU particles.\n\nARCHITECTURE\n\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\nThe DI deliberately uses PerInstanceDataSize() == 0, so Niagara never runs the\nper-instance Init/Destroy lifecycle. (An earlier per-instance design churned \xe2\x80\x94\nconstant init/destroy \xe2\x80\x94 which broke the GPU path and tanked performance.) The\nrender-thread proxy is created once with the DI and owns ONE GPU buffer set.\n\n  \xe2\x80\xa2 SetShaderParameters() runs on the real rendering proxy. It lazily uploads\n    the buffers once (self-heal) from the resolved CPU data, then binds them.\n  \xe2\x80\xa2 FlushGPUBuffers (scratchpad bool) or the Blueprint node bump a global flush\n    generation; the proxy releases its buffers the next time it renders and\n    binds the zeroed fallback so the shader never sees a null SRV.\n\nDATA SOURCE\n\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n  \xe2\x80\xa2 SplatAsset      \xe2\x80\x94 an imported UGaussianSplatAsset (.ply dragged into UE), or\n  \xe2\x80\xa2 SourceFilePath  \xe2\x80\x94 a raw .ply path read straight off disk, no import. Parsed\n                      once and cached process-wide.\nSplatAsset takes priority when both are set.\n\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SplatAsset_MetaData[] = {
		{ "Category", "Gaussian Splats" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xe2\x94\x80\xe2\x94\x80 Data source: imported asset \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n" },
#endif
		{ "ModuleRelativePath", "Public/NiagaraGSDataInterface.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xe2\x94\x80\xe2\x94\x80 Data source: imported asset \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SourceFilePath_MetaData[] = {
		{ "Category", "Gaussian Splats" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xe2\x94\x80\xe2\x94\x80 Data source: raw .ply path on disk (used only when SplatAsset is null) \xe2\x94\x80\n" },
#endif
		{ "DisplayName", "Source File Path (.ply, no import)" },
		{ "ModuleRelativePath", "Public/NiagaraGSDataInterface.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xe2\x94\x80\xe2\x94\x80 Data source: raw .ply path on disk (used only when SplatAsset is null) \xe2\x94\x80" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_SplatAsset;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SourceFilePath;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UNiagaraGSDataInterface>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UNiagaraGSDataInterface_Statics::NewProp_SplatAsset = { "SplatAsset", nullptr, (EPropertyFlags)0x0114000000000005, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UNiagaraGSDataInterface, SplatAsset), Z_Construct_UClass_UGaussianSplatAsset_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SplatAsset_MetaData), NewProp_SplatAsset_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UNiagaraGSDataInterface_Statics::NewProp_SourceFilePath = { "SourceFilePath", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UNiagaraGSDataInterface, SourceFilePath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SourceFilePath_MetaData), NewProp_SourceFilePath_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UNiagaraGSDataInterface_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UNiagaraGSDataInterface_Statics::NewProp_SplatAsset,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UNiagaraGSDataInterface_Statics::NewProp_SourceFilePath,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UNiagaraGSDataInterface_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UNiagaraGSDataInterface_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UNiagaraDataInterface,
	(UObject* (*)())Z_Construct_UPackage__Script_NiagaraGS,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UNiagaraGSDataInterface_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UNiagaraGSDataInterface_Statics::ClassParams = {
	&UNiagaraGSDataInterface::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UNiagaraGSDataInterface_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UNiagaraGSDataInterface_Statics::PropPointers),
	0,
	0x001010A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UNiagaraGSDataInterface_Statics::Class_MetaDataParams), Z_Construct_UClass_UNiagaraGSDataInterface_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UNiagaraGSDataInterface()
{
	if (!Z_Registration_Info_UClass_UNiagaraGSDataInterface.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UNiagaraGSDataInterface.OuterSingleton, Z_Construct_UClass_UNiagaraGSDataInterface_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UNiagaraGSDataInterface.OuterSingleton;
}
UNiagaraGSDataInterface::UNiagaraGSDataInterface() {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UNiagaraGSDataInterface);
UNiagaraGSDataInterface::~UNiagaraGSDataInterface() {}
// ********** End Class UNiagaraGSDataInterface ****************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSDataInterface_h__Script_NiagaraGS_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UNiagaraGSDataInterface, UNiagaraGSDataInterface::StaticClass, TEXT("UNiagaraGSDataInterface"), &Z_Registration_Info_UClass_UNiagaraGSDataInterface, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UNiagaraGSDataInterface), 2567152095U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSDataInterface_h__Script_NiagaraGS_2728444725(TEXT("/Script/NiagaraGS"),
	Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSDataInterface_h__Script_NiagaraGS_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSDataInterface_h__Script_NiagaraGS_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
