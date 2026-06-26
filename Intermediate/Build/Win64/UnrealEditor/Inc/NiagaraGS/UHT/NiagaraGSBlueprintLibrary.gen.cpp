// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "NiagaraGSBlueprintLibrary.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeNiagaraGSBlueprintLibrary() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary();
NIAGARA_API UClass* Z_Construct_UClass_UNiagaraComponent_NoRegister();
NIAGARAGS_API UClass* Z_Construct_UClass_UNiagaraGSBlueprintLibrary();
NIAGARAGS_API UClass* Z_Construct_UClass_UNiagaraGSBlueprintLibrary_NoRegister();
UPackage* Z_Construct_UPackage__Script_NiagaraGS();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UNiagaraGSBlueprintLibrary Function FlushGaussianSplatBuffers ************
struct Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics
{
	struct NiagaraGSBlueprintLibrary_eventFlushGaussianSplatBuffers_Parms
	{
		UNiagaraComponent* Component;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Gaussian Splats" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n     * Release the Gaussian Splat GPU buffers (VRAM) for the Niagara System\n     * instance owned by Component. Call this once the splat data has been copied\n     * into particle attributes and is no longer needed by the simulation.\n     *\n     * Safe to call on a system that has no Gaussian Splat DI (returns false).\n     * Only the passed component's instance is flushed; other live systems keep\n     * their buffers.\n     *\n     * @return true if a flush was queued for at least one Gaussian Splat DI.\n     */" },
#endif
		{ "DisplayName", "Flush Gaussian Splat GPU Buffers" },
		{ "ModuleRelativePath", "Public/NiagaraGSBlueprintLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Release the Gaussian Splat GPU buffers (VRAM) for the Niagara System\ninstance owned by Component. Call this once the splat data has been copied\ninto particle attributes and is no longer needed by the simulation.\n\nSafe to call on a system that has no Gaussian Splat DI (returns false).\nOnly the passed component's instance is flushed; other live systems keep\ntheir buffers.\n\n@return true if a flush was queued for at least one Gaussian Splat DI." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Component_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Component;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::NewProp_Component = { "Component", nullptr, (EPropertyFlags)0x0010000000080080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(NiagaraGSBlueprintLibrary_eventFlushGaussianSplatBuffers_Parms, Component), Z_Construct_UClass_UNiagaraComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Component_MetaData), NewProp_Component_MetaData) };
void Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((NiagaraGSBlueprintLibrary_eventFlushGaussianSplatBuffers_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(NiagaraGSBlueprintLibrary_eventFlushGaussianSplatBuffers_Parms), &Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::NewProp_Component,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UNiagaraGSBlueprintLibrary, nullptr, "FlushGaussianSplatBuffers", Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::PropPointers), sizeof(Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::NiagaraGSBlueprintLibrary_eventFlushGaussianSplatBuffers_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::Function_MetaDataParams), Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::NiagaraGSBlueprintLibrary_eventFlushGaussianSplatBuffers_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UNiagaraGSBlueprintLibrary::execFlushGaussianSplatBuffers)
{
	P_GET_OBJECT(UNiagaraComponent,Z_Param_Component);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=UNiagaraGSBlueprintLibrary::FlushGaussianSplatBuffers(Z_Param_Component);
	P_NATIVE_END;
}
// ********** End Class UNiagaraGSBlueprintLibrary Function FlushGaussianSplatBuffers **************

// ********** Begin Class UNiagaraGSBlueprintLibrary ***********************************************
void UNiagaraGSBlueprintLibrary::StaticRegisterNativesUNiagaraGSBlueprintLibrary()
{
	UClass* Class = UNiagaraGSBlueprintLibrary::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "FlushGaussianSplatBuffers", &UNiagaraGSBlueprintLibrary::execFlushGaussianSplatBuffers },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_UNiagaraGSBlueprintLibrary;
UClass* UNiagaraGSBlueprintLibrary::GetPrivateStaticClass()
{
	using TClass = UNiagaraGSBlueprintLibrary;
	if (!Z_Registration_Info_UClass_UNiagaraGSBlueprintLibrary.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("NiagaraGSBlueprintLibrary"),
			Z_Registration_Info_UClass_UNiagaraGSBlueprintLibrary.InnerSingleton,
			StaticRegisterNativesUNiagaraGSBlueprintLibrary,
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
	return Z_Registration_Info_UClass_UNiagaraGSBlueprintLibrary.InnerSingleton;
}
UClass* Z_Construct_UClass_UNiagaraGSBlueprintLibrary_NoRegister()
{
	return UNiagaraGSBlueprintLibrary::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UNiagaraGSBlueprintLibrary_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Blueprint helpers for the Gaussian Splat Data Interface.\n *\n * Lets you free a running splat system's GPU buffers on demand, with full\n * control over which instance is affected \xe2\x80\x94 you pass the exact UNiagaraComponent,\n * so two systems running side by side are never confused.\n */" },
#endif
		{ "IncludePath", "NiagaraGSBlueprintLibrary.h" },
		{ "ModuleRelativePath", "Public/NiagaraGSBlueprintLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Blueprint helpers for the Gaussian Splat Data Interface.\n\nLets you free a running splat system's GPU buffers on demand, with full\ncontrol over which instance is affected \xe2\x80\x94 you pass the exact UNiagaraComponent,\nso two systems running side by side are never confused." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UNiagaraGSBlueprintLibrary_FlushGaussianSplatBuffers, "FlushGaussianSplatBuffers" }, // 54577291
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UNiagaraGSBlueprintLibrary>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UNiagaraGSBlueprintLibrary_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBlueprintFunctionLibrary,
	(UObject* (*)())Z_Construct_UPackage__Script_NiagaraGS,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UNiagaraGSBlueprintLibrary_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UNiagaraGSBlueprintLibrary_Statics::ClassParams = {
	&UNiagaraGSBlueprintLibrary::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UNiagaraGSBlueprintLibrary_Statics::Class_MetaDataParams), Z_Construct_UClass_UNiagaraGSBlueprintLibrary_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UNiagaraGSBlueprintLibrary()
{
	if (!Z_Registration_Info_UClass_UNiagaraGSBlueprintLibrary.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UNiagaraGSBlueprintLibrary.OuterSingleton, Z_Construct_UClass_UNiagaraGSBlueprintLibrary_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UNiagaraGSBlueprintLibrary.OuterSingleton;
}
UNiagaraGSBlueprintLibrary::UNiagaraGSBlueprintLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UNiagaraGSBlueprintLibrary);
UNiagaraGSBlueprintLibrary::~UNiagaraGSBlueprintLibrary() {}
// ********** End Class UNiagaraGSBlueprintLibrary *************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h__Script_NiagaraGS_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UNiagaraGSBlueprintLibrary, UNiagaraGSBlueprintLibrary::StaticClass, TEXT("UNiagaraGSBlueprintLibrary"), &Z_Registration_Info_UClass_UNiagaraGSBlueprintLibrary, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UNiagaraGSBlueprintLibrary), 3118816343U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h__Script_NiagaraGS_3148732068(TEXT("/Script/NiagaraGS"),
	Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h__Script_NiagaraGS_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h__Script_NiagaraGS_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
