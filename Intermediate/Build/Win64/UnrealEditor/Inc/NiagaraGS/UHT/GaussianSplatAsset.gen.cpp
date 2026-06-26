// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GaussianSplatAsset.h"
#include "GaussianSplatData.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeGaussianSplatAsset() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
NIAGARAGS_API UClass* Z_Construct_UClass_UGaussianSplatAsset();
NIAGARAGS_API UClass* Z_Construct_UClass_UGaussianSplatAsset_NoRegister();
NIAGARAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGaussianSplatData();
UPackage* Z_Construct_UPackage__Script_NiagaraGS();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UGaussianSplatAsset ******************************************************
void UGaussianSplatAsset::StaticRegisterNativesUGaussianSplatAsset()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UGaussianSplatAsset;
UClass* UGaussianSplatAsset::GetPrivateStaticClass()
{
	using TClass = UGaussianSplatAsset;
	if (!Z_Registration_Info_UClass_UGaussianSplatAsset.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("GaussianSplatAsset"),
			Z_Registration_Info_UClass_UGaussianSplatAsset.InnerSingleton,
			StaticRegisterNativesUGaussianSplatAsset,
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
	return Z_Registration_Info_UClass_UGaussianSplatAsset.InnerSingleton;
}
UClass* Z_Construct_UClass_UGaussianSplatAsset_NoRegister()
{
	return UGaussianSplatAsset::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UGaussianSplatAsset_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * A UAsset that holds all parsed splat data for one PLY file.\n * Drag a .ply into the Content Browser \xe2\x86\x92 this asset is created.\n * At runtime, the Niagara Data Interface reads directly from SplatData.\n */" },
#endif
		{ "IncludePath", "GaussianSplatAsset.h" },
		{ "ModuleRelativePath", "Public/GaussianSplatAsset.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "A UAsset that holds all parsed splat data for one PLY file.\nDrag a .ply into the Content Browser \xe2\x86\x92 this asset is created.\nAt runtime, the Niagara Data Interface reads directly from SplatData." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SplatData_MetaData[] = {
		{ "Category", "Gaussian Splats" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// All splats loaded from the source PLY, already converted to UE space.\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatAsset.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "All splats loaded from the source PLY, already converted to UE space." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SourceFilePath_MetaData[] = {
		{ "Category", "Gaussian Splats" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Path to the original .ply file (useful for re-importing)\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatAsset.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Path to the original .ply file (useful for re-importing)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SplatCount_MetaData[] = {
		{ "Category", "Gaussian Splats" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Total splat count, shown in the asset details panel\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatAsset.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Total splat count, shown in the asset details panel" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SHDegree_MetaData[] = {
		{ "Category", "Gaussian Splats" },
		{ "ModuleRelativePath", "Public/GaussianSplatAsset.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_SplatData_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_SplatData;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SourceFilePath;
	static const UECodeGen_Private::FIntPropertyParams NewProp_SplatCount;
	static const UECodeGen_Private::FIntPropertyParams NewProp_SHDegree;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGaussianSplatAsset>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SplatData_Inner = { "SplatData", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGaussianSplatData, METADATA_PARAMS(0, nullptr) }; // 2981493528
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SplatData = { "SplatData", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGaussianSplatAsset, SplatData), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SplatData_MetaData), NewProp_SplatData_MetaData) }; // 2981493528
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SourceFilePath = { "SourceFilePath", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGaussianSplatAsset, SourceFilePath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SourceFilePath_MetaData), NewProp_SourceFilePath_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SplatCount = { "SplatCount", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGaussianSplatAsset, SplatCount), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SplatCount_MetaData), NewProp_SplatCount_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SHDegree = { "SHDegree", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGaussianSplatAsset, SHDegree), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SHDegree_MetaData), NewProp_SHDegree_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGaussianSplatAsset_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SplatData_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SplatData,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SourceFilePath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SplatCount,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGaussianSplatAsset_Statics::NewProp_SHDegree,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGaussianSplatAsset_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UGaussianSplatAsset_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_NiagaraGS,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGaussianSplatAsset_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGaussianSplatAsset_Statics::ClassParams = {
	&UGaussianSplatAsset::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UGaussianSplatAsset_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UGaussianSplatAsset_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGaussianSplatAsset_Statics::Class_MetaDataParams), Z_Construct_UClass_UGaussianSplatAsset_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGaussianSplatAsset()
{
	if (!Z_Registration_Info_UClass_UGaussianSplatAsset.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGaussianSplatAsset.OuterSingleton, Z_Construct_UClass_UGaussianSplatAsset_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGaussianSplatAsset.OuterSingleton;
}
UGaussianSplatAsset::UGaussianSplatAsset(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGaussianSplatAsset);
UGaussianSplatAsset::~UGaussianSplatAsset() {}
// ********** End Class UGaussianSplatAsset ********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatAsset_h__Script_NiagaraGS_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGaussianSplatAsset, UGaussianSplatAsset::StaticClass, TEXT("UGaussianSplatAsset"), &Z_Registration_Info_UClass_UGaussianSplatAsset, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGaussianSplatAsset), 372964996U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatAsset_h__Script_NiagaraGS_697880975(TEXT("/Script/NiagaraGS"),
	Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatAsset_h__Script_NiagaraGS_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatAsset_h__Script_NiagaraGS_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
