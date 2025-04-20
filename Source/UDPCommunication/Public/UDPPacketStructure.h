// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPDataType.h"
#include "Engine/DataAsset.h"
#include "UDPCustomStruct.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UDPPacketStructure.generated.h"

/**
 * 
 */

USTRUCT(BlueprintInternalUseOnly, meta = (BlueprintSpawnableComponent = false))
struct UDPCOMMUNICATION_API FUDPField
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Datatype",
		meta=(ToolTip = "Type of data to be sent/received."))
	EUDPDataType DataType = EUDPDataType::Float;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Maximum Length (Character Count)",
		meta = (ClampMin = "1", ClampMax = "502", UIMin = "1", UIMax = "502", NoResetToDefault,
		EditCondition = "DataType==EUDPDataType::Name || DataType==EUDPDataType::String || DataType==EUDPDataType::Text", EditConditionHides,
		ToolTip="Maximum length of the string. If a shorter string is sent, it still takes the max length in bytes.\nNOTE! If there is another field after this one, padding until the next 4th byte is added (Example: max length = 5, actual length = 8)."))
	int32 MaxLength = 128;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Is an Array", 
		meta = (ClampMin = "1", ClampMax = "500", UIMin = "1", UIMax = "500",
		EditCondition = "DataType==EUDPDataType::Bool || DataType==EUDPDataType::Int || DataType==EUDPDataType::Float", EditConditionHides,
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication",
		meta=(ToolTip="A map of fields in the packet structure to be sent/received. A unique name can be given to each field for better identification."))
	TMap<FString, FUDPField> Fields;

	UPROPERTY(Transient)
	int32 PacketSize;

	UUDPPacketStructure();
    
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void CompileStructure();
	bool IsFullyLoaded() const;
	
	int32 GetFieldOffset(const FString& FieldName) const;
	EUDPDataType GetFieldType(const FString& FieldName) const;
};
