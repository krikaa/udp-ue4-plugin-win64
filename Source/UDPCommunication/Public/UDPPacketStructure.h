// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPDataType.h"
#include "Engine/DataAsset.h"
#include "UDPPacketStructure.generated.h"

/**
 * 
 */

USTRUCT(BlueprintInternalUseOnly, meta = (BlueprintSpawnableComponent = false))
struct UDPCOMMUNICATION_API FUDPField
{
	GENERATED_BODY()

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Field Name (Optional)",
	// 	meta=(ToolTip = "Name of the field in the packet structure (defaults to Field <index>)."))
	// FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Datatype",
		meta=(ToolTip = "Type of data to be sent/received."))
	EUDPDataType DataType = EUDPDataType::Float;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Maximum Length (Character Count)",
		meta = (ClampMin = "1", ClampMax = "502", UIMin = "1", UIMax = "502", NoResetToDefault,
		EditCondition = "DataType==EUDPDataType::String", EditConditionHides,
		ToolTip="Maximum length of the string. If a shorter string is sent, it still takes the max length in bytes. NOTE! If there is a field after this one, padding will be added to align the next field."))
	int32 MaxLength = 128;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Is an Array", 
		meta = (ClampMin = "1", ClampMax = "500", UIMin = "1", UIMax = "500",
		EditCondition = "DataType!=EUDPDataType::String", EditConditionHides,
		ToolTip="Changes the pin and structure to take/receive an array instead of a single element."))
	bool IsArray = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Length (Element Count)",
		meta = (ClampMin = "1", ClampMax = "124", UIMin = "1", UIMax = "124", NoResetToDefault,
		EditCondition = "IsArray==true", EditConditionHides,
		ToolTip="Count of elements in the array. If a shorter array is sent, it still takes the max length in bytes."))
	int32 Count = 1;

	UPROPERTY(NotBlueprintType)
	int32 Offset = -1;
    
	int32 GetFieldSize() const;
	int32 GetFieldAlignment() const;
};

UCLASS()
class UDPCOMMUNICATION_API UUDPPacketStructure : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	TMap<FString, FUDPField> Fields;

	// Compiled packet data for runtime optimization
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")                    
	// TMap<FString, FUDPField> FieldMap;     
	
	// UPROPERTY(Transient)
	// TMap<FString, int32> FieldOffsets;
	//
	// UPROPERTY(Transient)
	// TMap<FString, EUDPDataType> FieldTypes;


	UPROPERTY(Transient)
	int32 PacketSize;

	UUDPPacketStructure();
    
	virtual void PostLoad() override;
	void CompileStructure();
    
	int32 GetFieldOffset(const FString& FieldName) const;
	EUDPDataType GetFieldType(const FString& FieldName) const;

	bool IsFullyLoaded() const
	{
		return !HasAnyFlags(RF_NeedLoad | RF_NeedPostLoad);
	}
};
