// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GaussianSplatData.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeGaussianSplatData() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FQuat4f();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector3f();
NIAGARAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGaussianSplatData();
UPackage* Z_Construct_UPackage__Script_NiagaraGS();
// ********** End Cross Module References **********************************************************

// ********** Begin ScriptStruct FGaussianSplatData ************************************************
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FGaussianSplatData;
class UScriptStruct* FGaussianSplatData::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FGaussianSplatData.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FGaussianSplatData.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FGaussianSplatData, (UObject*)Z_Construct_UPackage__Script_NiagaraGS(), TEXT("GaussianSplatData"));
	}
	return Z_Registration_Info_UScriptStruct_FGaussianSplatData.OuterSingleton;
}
struct Z_Construct_UScriptStruct_FGaussianSplatData_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Represents one fully parsed and UE-coordinate-converted Gaussian Splat.\n * Stored in a TArray on the CPU, then uploaded to GPU structured buffers.\n * Coordinate conversion is applied on import to prevent runtime math overhead.\n */" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatData.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Represents one fully parsed and UE-coordinate-converted Gaussian Splat.\nStored in a TArray on the CPU, then uploaded to GPU structured buffers.\nCoordinate conversion is applied on import to prevent runtime math overhead." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Position_MetaData[] = {
		{ "Category", "Gaussian Splat" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// World position in UE coordinates (cm), converted from PLY XYZ (scaled)\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatData.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "World position in UE coordinates (cm), converted from PLY XYZ (scaled)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Orientation_MetaData[] = {
		{ "Category", "Gaussian Splat" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Orientation as a quaternion in UE space, converted from PLY rot_0..3\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatData.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Orientation as a quaternion in UE space, converted from PLY rot_0..3" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Scale_MetaData[] = {
		{ "Category", "Gaussian Splat" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Scale in UE coordinates (cm), exp-activated from PLY log-scale values\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatData.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Scale in UE coordinates (cm), exp-activated from PLY log-scale values" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Opacity_MetaData[] = {
		{ "Category", "Gaussian Splat" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Opacity [0,1], sigmoid-activated from raw PLY logit value\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatData.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Opacity [0,1], sigmoid-activated from raw PLY logit value" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Color_MetaData[] = {
		{ "Category", "Gaussian Splat" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Base color from zero-order Spherical Harmonics (f_dc_0, f_dc_1, f_dc_2)\n// Converted to linear RGB [0,1]\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatData.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Base color from zero-order Spherical Harmonics (f_dc_0, f_dc_1, f_dc_2)\nConverted to linear RGB [0,1]" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SHCoefficients_MetaData[] = {
		{ "Category", "Gaussian Splat|Spherical Harmonics" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xe2\x94\x80\xe2\x94\x80 Spherical Harmonics \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n//\n// Higher-order SH coefficients, stored as raw floats in PLY order:\n//   [R_deg1_basis0, R_deg1_basis1, R_deg1_basis2,   (f_rest_0..2)\n//    R_deg2_basis0, ..., R_deg2_basis4,              (f_rest_3..7)\n//    R_deg3_basis0, ..., R_deg3_basis6,              (f_rest_8..14)\n//    G_deg1_basis0, ...,                             (f_rest_15..29)\n//    B_deg1_basis0, ...]                             (f_rest_30..44)\n//\n// Count is 0 (degree 0 only), 9 (deg 1), 24 (deg 2), or 45 (deg 3).\n// The GPU buffer pads every splat to SH_COEFFS_PADDED_COUNT floats\n// so the material can index without branching on per-splat count.\n" },
#endif
		{ "ModuleRelativePath", "Public/GaussianSplatData.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xe2\x94\x80\xe2\x94\x80 Spherical Harmonics \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n\nHigher-order SH coefficients, stored as raw floats in PLY order:\n  [R_deg1_basis0, R_deg1_basis1, R_deg1_basis2,   (f_rest_0..2)\n   R_deg2_basis0, ..., R_deg2_basis4,              (f_rest_3..7)\n   R_deg3_basis0, ..., R_deg3_basis6,              (f_rest_8..14)\n   G_deg1_basis0, ...,                             (f_rest_15..29)\n   B_deg1_basis0, ...]                             (f_rest_30..44)\n\nCount is 0 (degree 0 only), 9 (deg 1), 24 (deg 2), or 45 (deg 3).\nThe GPU buffer pads every splat to SH_COEFFS_PADDED_COUNT floats\nso the material can index without branching on per-splat count." },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_Position;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Orientation;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Scale;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Opacity;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Color;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SHCoefficients_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_SHCoefficients;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGaussianSplatData>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Position = { "Position", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FGaussianSplatData, Position), Z_Construct_UScriptStruct_FVector3f, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Position_MetaData), NewProp_Position_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Orientation = { "Orientation", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FGaussianSplatData, Orientation), Z_Construct_UScriptStruct_FQuat4f, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Orientation_MetaData), NewProp_Orientation_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Scale = { "Scale", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FGaussianSplatData, Scale), Z_Construct_UScriptStruct_FVector3f, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Scale_MetaData), NewProp_Scale_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Opacity = { "Opacity", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FGaussianSplatData, Opacity), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Opacity_MetaData), NewProp_Opacity_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Color = { "Color", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FGaussianSplatData, Color), Z_Construct_UScriptStruct_FVector3f, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Color_MetaData), NewProp_Color_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_SHCoefficients_Inner = { "SHCoefficients", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_SHCoefficients = { "SHCoefficients", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FGaussianSplatData, SHCoefficients), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SHCoefficients_MetaData), NewProp_SHCoefficients_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGaussianSplatData_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Position,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Orientation,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Scale,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Opacity,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_Color,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_SHCoefficients_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewProp_SHCoefficients,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FGaussianSplatData_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGaussianSplatData_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_NiagaraGS,
	nullptr,
	&NewStructOps,
	"GaussianSplatData",
	Z_Construct_UScriptStruct_FGaussianSplatData_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FGaussianSplatData_Statics::PropPointers),
	sizeof(FGaussianSplatData),
	alignof(FGaussianSplatData),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000201),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FGaussianSplatData_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FGaussianSplatData_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FGaussianSplatData()
{
	if (!Z_Registration_Info_UScriptStruct_FGaussianSplatData.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FGaussianSplatData.InnerSingleton, Z_Construct_UScriptStruct_FGaussianSplatData_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_FGaussianSplatData.InnerSingleton;
}
// ********** End ScriptStruct FGaussianSplatData **************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatData_h__Script_NiagaraGS_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FGaussianSplatData::StaticStruct, Z_Construct_UScriptStruct_FGaussianSplatData_Statics::NewStructOps, TEXT("GaussianSplatData"), &Z_Registration_Info_UScriptStruct_FGaussianSplatData, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FGaussianSplatData), 2981493528U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatData_h__Script_NiagaraGS_4120863679(TEXT("/Script/NiagaraGS"),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatData_h__Script_NiagaraGS_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatData_h__Script_NiagaraGS_Statics::ScriptStructInfo),
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
