// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPDataType.h"
#include "Engine/DataAsset.h"
#include "UDPPacketStructure.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct UDPCOMMUNICATION_API FUDPField
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	EUDPDataType DataType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication", 
		meta = (ClampMin = "1", ClampMax = "502", UIMin = "1", UIMax = "502", EditCondition = "DataType==EUDPDataType::String", EditConditionHides))
	int32 MaxLength = 128;
    
	int32 GetFieldSize() const;
	int32 GetFieldAlignment() const;
};

UCLASS()
class UDPCOMMUNICATION_API UUDPPacketStructure : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	TArray<FUDPField> Fields;

	// Compiled packet data for runtime optimization
	UPROPERTY(Transient)
	TMap<FString, int32> FieldOffsets;

	UPROPERTY(Transient)
	TMap<FString, EUDPDataType> FieldTypes;

	UPROPERTY(Transient)
	int32 PacketSize;

	UUDPPacketStructure();
    
	virtual void PostLoad() override;
	void CompileStructure();
    
	int32 GetFieldOffset(const FString& FieldName) const;
	EUDPDataType GetFieldType(const FString& FieldName) const;
};
