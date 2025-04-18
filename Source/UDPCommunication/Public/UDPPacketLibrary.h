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

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetFloatArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<float>& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetIntArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<int32>& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static void SetBoolArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<bool>& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static TArray<float> GetFloatArray(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static TArray<int32> GetIntArray(const FUDPPacket& UDPPacket, const FString& FieldName);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static TArray<bool> GetBoolArray(const FUDPPacket& UDPPacket, const FString& FieldName);
	

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication|Debug")
	static FString DumpPacketBytes(const FUDPPacket& UDPPacket)
	{
		FString Result = FString::Printf(TEXT("Packet Size: %d bytes\n"), UDPPacket.Data.Num());
    
		for (int32 i = 0; i < UDPPacket.Data.Num(); i++)
		{
			Result.Append(FString::Printf(TEXT("[%d] %02X "), i, UDPPacket.Data[i]));
			if ((i + 1) % 8 == 0) Result.Append(TEXT("\n"));
		}
    
		return Result;
	}

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication|Debug")
	static float DebugExtractFloat(const FUDPPacket& UDPPacket, const FString& FieldName, int32 Offset)
	{
		if (UDPPacket.Data.Num() < Offset + sizeof(float))
		{
			UE_LOG(LogTemp, Warning, TEXT("DebugExtractFloat: Not enough data"));
			return 0.0f;
		}

		float Value = 0.0f;
		FMemory::Memcpy(&Value, UDPPacket.Data.GetData() + Offset, sizeof(float));
		return Value;
	}
};
