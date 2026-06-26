// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "NiagaraGSBlueprintLibrary.h"

#ifdef NIAGARAGS_NiagaraGSBlueprintLibrary_generated_h
#error "NiagaraGSBlueprintLibrary.generated.h already included, missing '#pragma once' in NiagaraGSBlueprintLibrary.h"
#endif
#define NIAGARAGS_NiagaraGSBlueprintLibrary_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

class UNiagaraComponent;

// ********** Begin Class UNiagaraGSBlueprintLibrary ***********************************************
#define FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h_19_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execFlushGaussianSplatBuffers);


NIAGARAGS_API UClass* Z_Construct_UClass_UNiagaraGSBlueprintLibrary_NoRegister();

#define FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h_19_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUNiagaraGSBlueprintLibrary(); \
	friend struct Z_Construct_UClass_UNiagaraGSBlueprintLibrary_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend NIAGARAGS_API UClass* Z_Construct_UClass_UNiagaraGSBlueprintLibrary_NoRegister(); \
public: \
	DECLARE_CLASS2(UNiagaraGSBlueprintLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/NiagaraGS"), Z_Construct_UClass_UNiagaraGSBlueprintLibrary_NoRegister) \
	DECLARE_SERIALIZER(UNiagaraGSBlueprintLibrary)


#define FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h_19_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UNiagaraGSBlueprintLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UNiagaraGSBlueprintLibrary(UNiagaraGSBlueprintLibrary&&) = delete; \
	UNiagaraGSBlueprintLibrary(const UNiagaraGSBlueprintLibrary&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UNiagaraGSBlueprintLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UNiagaraGSBlueprintLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UNiagaraGSBlueprintLibrary) \
	NO_API virtual ~UNiagaraGSBlueprintLibrary();


#define FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h_16_PROLOG
#define FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h_19_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h_19_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h_19_INCLASS_NO_PURE_DECLS \
	FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h_19_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UNiagaraGSBlueprintLibrary;

// ********** End Class UNiagaraGSBlueprintLibrary *************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_deety_Documents_Unreal_Projects_GSPlugin_Plugins_NiagaraGS_Source_Public_NiagaraGSBlueprintLibrary_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
