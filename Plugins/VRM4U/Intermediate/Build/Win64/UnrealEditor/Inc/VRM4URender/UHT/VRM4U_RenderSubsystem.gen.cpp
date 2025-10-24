// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "VRM4URender/Public/VRM4U_RenderSubsystem.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeVRM4U_RenderSubsystem() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_UEngineSubsystem();
	ENGINE_API UClass* Z_Construct_UClass_UTextureRenderTarget2D_NoRegister();
	UPackage* Z_Construct_UPackage__Script_VRM4URender();
	VRM4URENDER_API UClass* Z_Construct_UClass_UVRM4U_RenderSubsystem();
	VRM4URENDER_API UClass* Z_Construct_UClass_UVRM4U_RenderSubsystem_NoRegister();
	VRM4URENDER_API UEnum* Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource();
// End Cross Module References
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EVRM4U_CaptureSource;
	static UEnum* EVRM4U_CaptureSource_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EVRM4U_CaptureSource.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EVRM4U_CaptureSource.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource, (UObject*)Z_Construct_UPackage__Script_VRM4URender(), TEXT("EVRM4U_CaptureSource"));
		}
		return Z_Registration_Info_UEnum_EVRM4U_CaptureSource.OuterSingleton;
	}
	template<> VRM4URENDER_API UEnum* StaticEnum<EVRM4U_CaptureSource>()
	{
		return EVRM4U_CaptureSource_StaticEnum();
	}
	struct Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics::Enumerators[] = {
		{ "ColorTexturePostOpaque", (int64)ColorTexturePostOpaque },
		{ "ColorTextureOverlay", (int64)ColorTextureOverlay },
		{ "DepthTexture", (int64)DepthTexture },
		{ "NormalTexture", (int64)NormalTexture },
		{ "VelocityTexture", (int64)VelocityTexture },
		{ "SceneColorTexturePostOpaque", (int64)SceneColorTexturePostOpaque },
		{ "SceneColorTextureOverlay", (int64)SceneColorTextureOverlay },
		{ "SceneDepthTexture", (int64)SceneDepthTexture },
		{ "GBufferATexture", (int64)GBufferATexture },
		{ "GBufferBTexture", (int64)GBufferBTexture },
		{ "GBufferCTexture", (int64)GBufferCTexture },
		{ "GBufferDTexture", (int64)GBufferDTexture },
		{ "GBufferETexture", (int64)GBufferETexture },
		{ "GBufferFTexture", (int64)GBufferFTexture },
		{ "GBufferVelocityTexture", (int64)GBufferVelocityTexture },
		{ "ScreenSpaceAOTexture", (int64)ScreenSpaceAOTexture },
		{ "CustomDepthTexture", (int64)CustomDepthTexture },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics::Enum_MetaDataParams[] = {
		{ "ColorTextureOverlay.Name", "ColorTextureOverlay" },
		{ "ColorTexturePostOpaque.Name", "ColorTexturePostOpaque" },
		{ "CustomDepthTexture.Comment", "// Custom Depth / Stencil\n" },
		{ "CustomDepthTexture.Name", "CustomDepthTexture" },
		{ "CustomDepthTexture.ToolTip", "Custom Depth / Stencil" },
		{ "DepthTexture.Name", "DepthTexture" },
		{ "GBufferATexture.Comment", "// GBuffer\n" },
		{ "GBufferATexture.Name", "GBufferATexture" },
		{ "GBufferATexture.ToolTip", "GBuffer" },
		{ "GBufferBTexture.Name", "GBufferBTexture" },
		{ "GBufferCTexture.Name", "GBufferCTexture" },
		{ "GBufferDTexture.Name", "GBufferDTexture" },
		{ "GBufferETexture.Name", "GBufferETexture" },
		{ "GBufferFTexture.Name", "GBufferFTexture" },
		{ "GBufferVelocityTexture.Name", "GBufferVelocityTexture" },
		{ "ModuleRelativePath", "Public/VRM4U_RenderSubsystem.h" },
		{ "NormalTexture.Name", "NormalTexture" },
		{ "SceneColorTextureOverlay.Name", "SceneColorTextureOverlay" },
		{ "SceneColorTexturePostOpaque.Comment", "//SmallDepthTexture,\n" },
		{ "SceneColorTexturePostOpaque.Name", "SceneColorTexturePostOpaque" },
		{ "SceneColorTexturePostOpaque.ToolTip", "SmallDepthTexture," },
		{ "SceneDepthTexture.Name", "SceneDepthTexture" },
		{ "ScreenSpaceAOTexture.Comment", "// SSAO\n" },
		{ "ScreenSpaceAOTexture.Name", "ScreenSpaceAOTexture" },
		{ "ScreenSpaceAOTexture.ToolTip", "SSAO" },
		{ "VelocityTexture.Name", "VelocityTexture" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_VRM4URender,
		nullptr,
		"EVRM4U_CaptureSource",
		"EVRM4U_CaptureSource",
		Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics::Enumerators,
		UE_ARRAY_COUNT(Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics::Enumerators),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::Regular,
		METADATA_PARAMS(Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics::Enum_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics::Enum_MetaDataParams))
	};
	UEnum* Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource()
	{
		if (!Z_Registration_Info_UEnum_EVRM4U_CaptureSource.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EVRM4U_CaptureSource.InnerSingleton, Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EVRM4U_CaptureSource.InnerSingleton;
	}
	DEFINE_FUNCTION(UVRM4U_RenderSubsystem::execResetSceneTextureExtentHistory)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->ResetSceneTextureExtentHistory();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UVRM4U_RenderSubsystem::execRemoveAllCaptureTexture)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->RemoveAllCaptureTexture();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UVRM4U_RenderSubsystem::execRemoveCaptureTexture)
	{
		P_GET_OBJECT(UTextureRenderTarget2D,Z_Param_Texture);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->RemoveCaptureTexture(Z_Param_Texture);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UVRM4U_RenderSubsystem::execAddCaptureTexture)
	{
		P_GET_OBJECT(UTextureRenderTarget2D,Z_Param_Texture);
		P_GET_PROPERTY(FByteProperty,Z_Param_CaptureSource);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->AddCaptureTexture(Z_Param_Texture,EVRM4U_CaptureSource(Z_Param_CaptureSource));
		P_NATIVE_END;
	}
	void UVRM4U_RenderSubsystem::StaticRegisterNativesUVRM4U_RenderSubsystem()
	{
		UClass* Class = UVRM4U_RenderSubsystem::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "AddCaptureTexture", &UVRM4U_RenderSubsystem::execAddCaptureTexture },
			{ "RemoveAllCaptureTexture", &UVRM4U_RenderSubsystem::execRemoveAllCaptureTexture },
			{ "RemoveCaptureTexture", &UVRM4U_RenderSubsystem::execRemoveCaptureTexture },
			{ "ResetSceneTextureExtentHistory", &UVRM4U_RenderSubsystem::execResetSceneTextureExtentHistory },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics
	{
		struct VRM4U_RenderSubsystem_eventAddCaptureTexture_Parms
		{
			UTextureRenderTarget2D* Texture;
			TEnumAsByte<EVRM4U_CaptureSource> CaptureSource;
		};
		static const UECodeGen_Private::FObjectPropertyParams NewProp_Texture;
		static const UECodeGen_Private::FBytePropertyParams NewProp_CaptureSource;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::NewProp_Texture = { "Texture", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(VRM4U_RenderSubsystem_eventAddCaptureTexture_Parms, Texture), Z_Construct_UClass_UTextureRenderTarget2D_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::NewProp_CaptureSource = { "CaptureSource", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(VRM4U_RenderSubsystem_eventAddCaptureTexture_Parms, CaptureSource), Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource, METADATA_PARAMS(nullptr, 0) }; // 2884524719
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::NewProp_Texture,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::NewProp_CaptureSource,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::Function_MetaDataParams[] = {
		{ "Category", "VRM4U" },
		{ "ModuleRelativePath", "Public/VRM4U_RenderSubsystem.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UVRM4U_RenderSubsystem, nullptr, "AddCaptureTexture", nullptr, nullptr, sizeof(Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::VRM4U_RenderSubsystem_eventAddCaptureTexture_Parms), Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveAllCaptureTexture_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveAllCaptureTexture_Statics::Function_MetaDataParams[] = {
		{ "Category", "VRM4U" },
		{ "ModuleRelativePath", "Public/VRM4U_RenderSubsystem.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveAllCaptureTexture_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UVRM4U_RenderSubsystem, nullptr, "RemoveAllCaptureTexture", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveAllCaptureTexture_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveAllCaptureTexture_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveAllCaptureTexture()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveAllCaptureTexture_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics
	{
		struct VRM4U_RenderSubsystem_eventRemoveCaptureTexture_Parms
		{
			UTextureRenderTarget2D* Texture;
		};
		static const UECodeGen_Private::FObjectPropertyParams NewProp_Texture;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::NewProp_Texture = { "Texture", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(VRM4U_RenderSubsystem_eventRemoveCaptureTexture_Parms, Texture), Z_Construct_UClass_UTextureRenderTarget2D_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::NewProp_Texture,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::Function_MetaDataParams[] = {
		{ "Category", "VRM4U" },
		{ "ModuleRelativePath", "Public/VRM4U_RenderSubsystem.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UVRM4U_RenderSubsystem, nullptr, "RemoveCaptureTexture", nullptr, nullptr, sizeof(Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::VRM4U_RenderSubsystem_eventRemoveCaptureTexture_Parms), Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UVRM4U_RenderSubsystem_ResetSceneTextureExtentHistory_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UVRM4U_RenderSubsystem_ResetSceneTextureExtentHistory_Statics::Function_MetaDataParams[] = {
		{ "Category", "VRM4U" },
		{ "ModuleRelativePath", "Public/VRM4U_RenderSubsystem.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UVRM4U_RenderSubsystem_ResetSceneTextureExtentHistory_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UVRM4U_RenderSubsystem, nullptr, "ResetSceneTextureExtentHistory", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UVRM4U_RenderSubsystem_ResetSceneTextureExtentHistory_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UVRM4U_RenderSubsystem_ResetSceneTextureExtentHistory_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UVRM4U_RenderSubsystem_ResetSceneTextureExtentHistory()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UVRM4U_RenderSubsystem_ResetSceneTextureExtentHistory_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UVRM4U_RenderSubsystem);
	UClass* Z_Construct_UClass_UVRM4U_RenderSubsystem_NoRegister()
	{
		return UVRM4U_RenderSubsystem::StaticClass();
	}
	struct Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_CaptureList_ValueProp;
		static const UECodeGen_Private::FObjectPtrPropertyParams NewProp_CaptureList_Key_KeyProp;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CaptureList_MetaData[];
#endif
		static const UECodeGen_Private::FMapPropertyParams NewProp_CaptureList;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UEngineSubsystem,
		(UObject* (*)())Z_Construct_UPackage__Script_VRM4URender,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UVRM4U_RenderSubsystem_AddCaptureTexture, "AddCaptureTexture" }, // 2246562215
		{ &Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveAllCaptureTexture, "RemoveAllCaptureTexture" }, // 2403935696
		{ &Z_Construct_UFunction_UVRM4U_RenderSubsystem_RemoveCaptureTexture, "RemoveCaptureTexture" }, // 419452215
		{ &Z_Construct_UFunction_UVRM4U_RenderSubsystem_ResetSceneTextureExtentHistory, "ResetSceneTextureExtentHistory" }, // 3600162488
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "VRM4U_RenderSubsystem.h" },
		{ "ModuleRelativePath", "Public/VRM4U_RenderSubsystem.h" },
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList_ValueProp = { "CaptureList", nullptr, (EPropertyFlags)0x0000000000000001, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 1, Z_Construct_UEnum_VRM4URender_EVRM4U_CaptureSource, METADATA_PARAMS(nullptr, 0) }; // 2884524719
	const UECodeGen_Private::FObjectPtrPropertyParams Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList_Key_KeyProp = { "CaptureList_Key", nullptr, (EPropertyFlags)0x0004000000000001, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, Z_Construct_UClass_UTextureRenderTarget2D_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList_MetaData[] = {
		{ "Category", "VRM4U" },
		{ "ModuleRelativePath", "Public/VRM4U_RenderSubsystem.h" },
	};
#endif
	const UECodeGen_Private::FMapPropertyParams Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList = { "CaptureList", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UVRM4U_RenderSubsystem, CaptureList), EMapPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList_MetaData)) }; // 2884524719
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList_ValueProp,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList_Key_KeyProp,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::NewProp_CaptureList,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UVRM4U_RenderSubsystem>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::ClassParams = {
		&UVRM4U_RenderSubsystem::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UVRM4U_RenderSubsystem()
	{
		if (!Z_Registration_Info_UClass_UVRM4U_RenderSubsystem.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UVRM4U_RenderSubsystem.OuterSingleton, Z_Construct_UClass_UVRM4U_RenderSubsystem_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UVRM4U_RenderSubsystem.OuterSingleton;
	}
	template<> VRM4URENDER_API UClass* StaticClass<UVRM4U_RenderSubsystem>()
	{
		return UVRM4U_RenderSubsystem::StaticClass();
	}
	UVRM4U_RenderSubsystem::UVRM4U_RenderSubsystem() {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UVRM4U_RenderSubsystem);
	UVRM4U_RenderSubsystem::~UVRM4U_RenderSubsystem() {}
	struct Z_CompiledInDeferFile_FID_UE_project_Blaster_Plugins_VRM4U_Source_VRM4URender_Public_VRM4U_RenderSubsystem_h_Statics
	{
		static const FEnumRegisterCompiledInInfo EnumInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FEnumRegisterCompiledInInfo Z_CompiledInDeferFile_FID_UE_project_Blaster_Plugins_VRM4U_Source_VRM4URender_Public_VRM4U_RenderSubsystem_h_Statics::EnumInfo[] = {
		{ EVRM4U_CaptureSource_StaticEnum, TEXT("EVRM4U_CaptureSource"), &Z_Registration_Info_UEnum_EVRM4U_CaptureSource, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2884524719U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_UE_project_Blaster_Plugins_VRM4U_Source_VRM4URender_Public_VRM4U_RenderSubsystem_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UVRM4U_RenderSubsystem, UVRM4U_RenderSubsystem::StaticClass, TEXT("UVRM4U_RenderSubsystem"), &Z_Registration_Info_UClass_UVRM4U_RenderSubsystem, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UVRM4U_RenderSubsystem), 2999734046U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_UE_project_Blaster_Plugins_VRM4U_Source_VRM4URender_Public_VRM4U_RenderSubsystem_h_1807244464(TEXT("/Script/VRM4URender"),
		Z_CompiledInDeferFile_FID_UE_project_Blaster_Plugins_VRM4U_Source_VRM4URender_Public_VRM4U_RenderSubsystem_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_UE_project_Blaster_Plugins_VRM4U_Source_VRM4URender_Public_VRM4U_RenderSubsystem_h_Statics::ClassInfo),
		nullptr, 0,
		Z_CompiledInDeferFile_FID_UE_project_Blaster_Plugins_VRM4U_Source_VRM4URender_Public_VRM4U_RenderSubsystem_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_UE_project_Blaster_Plugins_VRM4U_Source_VRM4URender_Public_VRM4U_RenderSubsystem_h_Statics::EnumInfo));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
