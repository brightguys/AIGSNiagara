// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GaussianSplatAssetFactory.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeGaussianSplatAssetFactory() {}

// ********** Begin Cross Module References ********************************************************
NIAGARAGS_API UClass* Z_Construct_UClass_UGaussianSplatAssetFactory();
NIAGARAGS_API UClass* Z_Construct_UClass_UGaussianSplatAssetFactory_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UFactory();
UPackage* Z_Construct_UPackage__Script_NiagaraGS();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UGaussianSplatAssetFactory ***********************************************
void UGaussianSplatAssetFactory::StaticRegisterNativesUGaussianSplatAssetFactory()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UGaussianSplatAssetFactory;
UClass* UGaussianSplatAssetFactory::GetPrivateStaticClass()
{
	using TClass = UGaussianSplatAssetFactory;
	if (!Z_Registration_Info_UClass_UGaussianSplatAssetFactory.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("GaussianSplatAssetFactory"),
			Z_Registration_Info_UClass_UGaussianSplatAssetFactory.InnerSingleton,
			StaticRegisterNativesUGaussianSplatAssetFactory,
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
	return Z_Registration_Info_UClass_UGaussianSplatAssetFactory.InnerSingleton;
}
UClass* Z_Construct_UClass_UGaussianSplatAssetFactory_NoRegister()
{
	return UGaussianSplatAssetFactory::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UGaussianSplatAssetFactory_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Editor-only factory.\n * Unreal's import pipeline instantiates this when you drop a .ply\n * file into the Content Browser. FactoryCreateFile() does the work.\n */" },
#endif
		{ "IncludePath", "GaussianSplatAssetFactory.h" },
		{ "ModuleRelativePath", "Public/GaussianSplatAssetFactory.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Editor-only factory.\nUnreal's import pipeline instantiates this when you drop a .ply\nfile into the Content Browser. FactoryCreateFile() does the work." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGaussianSplatAssetFactory>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UGaussianSplatAssetFactory_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UFactory,
	(UObject* (*)())Z_Construct_UPackage__Script_NiagaraGS,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGaussianSplatAssetFactory_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGaussianSplatAssetFactory_Statics::ClassParams = {
	&UGaussianSplatAssetFactory::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x000000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGaussianSplatAssetFactory_Statics::Class_MetaDataParams), Z_Construct_UClass_UGaussianSplatAssetFactory_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGaussianSplatAssetFactory()
{
	if (!Z_Registration_Info_UClass_UGaussianSplatAssetFactory.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGaussianSplatAssetFactory.OuterSingleton, Z_Construct_UClass_UGaussianSplatAssetFactory_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGaussianSplatAssetFactory.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGaussianSplatAssetFactory);
UGaussianSplatAssetFactory::~UGaussianSplatAssetFactory() {}
// ********** End Class UGaussianSplatAssetFactory *************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatAssetFactory_h__Script_NiagaraGS_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGaussianSplatAssetFactory, UGaussianSplatAssetFactory::StaticClass, TEXT("UGaussianSplatAssetFactory"), &Z_Registration_Info_UClass_UGaussianSplatAssetFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGaussianSplatAssetFactory), 73550202U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatAssetFactory_h__Script_NiagaraGS_605332318(TEXT("/Script/NiagaraGS"),
	Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatAssetFactory_h__Script_NiagaraGS_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_GaussianSplatAssetFactory_h__Script_NiagaraGS_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
