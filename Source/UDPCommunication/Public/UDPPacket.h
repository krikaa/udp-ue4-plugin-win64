#pragma once

#include "UDPPacketStructure.h"
#include "Serialization/Archive.h"
#include "UDPPacket.generated.h"


USTRUCT(BlueprintType)
struct UDPCOMMUNICATION_API FUDPPacket
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<uint8> Data;
    
	UPROPERTY(Transient)
	UUDPPacketStructure* Structure;

	FUDPPacket();
    
	void InitWithStructure(UUDPPacketStructure* InitStructure);
    
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
FORCEINLINE FArchive& operator<<(FArchive& Ar, FUDPPacket& UDPPacket)
{
	Ar << UDPPacket.Data;
	return Ar;
}
