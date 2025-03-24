// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPDataType.h"
#include "Engine/DataAsset.h"
#include "UDPPacketDefinition.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct UDPCOMMUNICATION_API FUDPFieldDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	EUDPDataType DataType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	int32 ArraySize = 1;
    
	int32 GetFieldSize() const;
	int32 GetFieldAlignment() const;
};

UCLASS()
class UDPCOMMUNICATION_API UUDPPacketDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	TArray<FUDPFieldDefinition> Fields;

	// Compiled packet data for runtime optimization
	UPROPERTY(Transient)
	TMap<FString, int32> FieldOffsets;

	UPROPERTY(Transient)
	TMap<FString, EUDPDataType> FieldTypes;

	UPROPERTY(Transient)
	int32 PacketSize;

	UUDPPacketDefinition();
    
	virtual void PostLoad() override;
	void CompileStructure();
    
	int32 GetFieldOffset(const FString& FieldName) const;
	EUDPDataType GetFieldType(const FString& FieldName) const;
};
