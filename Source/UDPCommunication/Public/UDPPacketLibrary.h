// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UDPPacket.h"
#include "UDPPacketLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UDPCOMMUNICATION_API UUDPPacketLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void InitWithStructure(UPARAM(ref) FUDPPacket& UDPPacket, UUDPPacketStructure* Structure);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void ValidateWithStructure(UPARAM(ref) FUDPPacket& UDPPacket, UUDPPacketStructure* Structure);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static FUDPPacket CreateUDPPacket();
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetFloat(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, float Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetInt(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, int32 Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetBool(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, bool Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetString(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const FString& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static float GetFloat(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static int32 GetInt(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static bool GetBool(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static FString GetString(const FUDPPacket& UDPPacket, const FString& FieldName);
};
