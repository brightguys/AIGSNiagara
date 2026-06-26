// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeNiagaraGS_init() {}
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_NiagaraGS;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_NiagaraGS()
	{
		if (!Z_Registration_Info_UPackage__Script_NiagaraGS.OuterSingleton)
		{
			static const UECodeGen_Private::FPackageParams PackageParams = {
				"/Script/NiagaraGS",
				nullptr,
				0,
				PKG_CompiledIn | 0x00000000,
				0xDF536A30,
				0x3ACFC331,
				METADATA_PARAMS(0, nullptr)
			};
			UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_NiagaraGS.OuterSingleton, PackageParams);
		}
		return Z_Registration_Info_UPackage__Script_NiagaraGS.OuterSingleton;
	}
	static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_NiagaraGS(Z_Construct_UPackage__Script_NiagaraGS, TEXT("/Script/NiagaraGS"), Z_Registration_Info_UPackage__Script_NiagaraGS, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0xDF536A30, 0x3ACFC331));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
