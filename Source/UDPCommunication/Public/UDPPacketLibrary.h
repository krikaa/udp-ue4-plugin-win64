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

	
	// ----------- SET AND GET FUNCTIONS FOR DIFFERENT DATATYPES -----------

	// --- SINGLE VALUES ---

	// Setters
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetBool(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, bool Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetByte(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, uint8 Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetInt(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, int32 Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetInt64(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, int64 Value);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetFloat(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, float Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetDouble(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, double Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetString(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const FString& Value);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetVector(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, FVector Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetRotator(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, FRotator Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetTransform(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, FTransform Value);

	// Getters

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static bool GetBool(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static uint8 GetByte(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static int32 GetInt(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static int64 GetInt64(const FUDPPacket& UDPPacket, const FString& FieldName);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static float GetFloat(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static double GetDouble(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static FString GetString(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static FVector GetVector(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static FRotator GetRotator(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static FTransform GetTransform(const FUDPPacket& UDPPacket, const FString& FieldName);
		
	// --- ARRAYS ---

	// Setters

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetBoolArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<bool>& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetIntArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<int32>& Value);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetFloatArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<float>& Value);

	// Getters
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static TArray<bool> GetBoolArray(const FUDPPacket& UDPPacket, const FString& FieldName);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static TArray<int32> GetIntArray(const FUDPPacket& UDPPacket, const FString& FieldName);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static TArray<float> GetFloatArray(const FUDPPacket& UDPPacket, const FString& FieldName);

	// --- CUSTOM ----

	// Setters
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetCustomStruct(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, FUDPCustomStruct Value);

	// Getters
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static FUDPCustomStruct GetCustomStruct(const FUDPPacket& UDPPacket, const FString& FieldName);
};
