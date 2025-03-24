#pragma once

#include "UDPPacketDefinition.h"
#include "Serialization/Archive.h"
#include "UDPDynamicData.generated.h"


USTRUCT(BlueprintType)
struct UDPCOMMUNICATION_API FUDPDynamicData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<uint8> Data;
    
	UPROPERTY(Transient)
	UUDPPacketDefinition* Definition;

	FUDPDynamicData();
    
	void InitWithDefinition(UUDPPacketDefinition* InDefinition);
    
	// Value setters
	void SetFloat(const FString& FieldName, float Value);
	void SetInt(const FString& FieldName, int32 Value);
	void SetBool(const FString& FieldName, bool Value);
	void SetString(const FString& FieldName, const FString& Value);
    
	// Value getters
	float GetFloat(const FString& FieldName) const;
	int32 GetInt(const FString& FieldName) const;
	bool GetBool(const FString& FieldName) const;
	FString GetString(const FString& FieldName) const;
};

// Serialization operator
FORCEINLINE FArchive& operator<<(FArchive& Ar, FUDPDynamicData& DynamicData)
{
	Ar << DynamicData.Data;
	return Ar;
}
