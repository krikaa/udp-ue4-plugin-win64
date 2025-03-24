// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UDPDynamicData.h"
#include "UDPDynamicDataLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UDPCOMMUNICATION_API UUDPDynamicDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UDP|Dynamic Data")
	static void InitWithDefinition(UPARAM(ref) FUDPDynamicData& DynamicData, UUDPPacketDefinition* Definition);
	
	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	static void SetFloat(UPARAM(ref) FUDPDynamicData& DynamicData, const FString& FieldName, float Value);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	static void SetInt(UPARAM(ref) FUDPDynamicData& DynamicData, const FString& FieldName, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	static void SetBool(UPARAM(ref) FUDPDynamicData& DynamicData, const FString& FieldName, bool Value);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	static void SetString(UPARAM(ref) FUDPDynamicData& DynamicData, const FString& FieldName, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	static float GetFloat(const FUDPDynamicData& DynamicData, const FString& FieldName);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	static int32 GetInt(const FUDPDynamicData& DynamicData, const FString& FieldName);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	static bool GetBool(const FUDPDynamicData& DynamicData, const FString& FieldName);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	static FString GetString(const FUDPDynamicData& DynamicData, const FString& FieldName);
};
