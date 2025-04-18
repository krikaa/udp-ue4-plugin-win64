// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UDPPacketStructure.h"
#include "Serialization/Archive.h"
#include "UDPPacket.generated.h"


USTRUCT(BlueprintInternalUseOnly, meta = (BlueprintSpawnableComponent = false))
struct UDPCOMMUNICATION_API FUDPPacket
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<uint8> Data;
    
	UPROPERTY(Transient)
	UUDPPacketStructure* Structure;

	FUDPPacket();
    
	void InitWithStructure(UUDPPacketStructure* InitStructure);
	int32 CheckAndGetOffset(const FString& FieldName, EUDPDataType DataType) const;
    
	// Value setters
	void SetFloat(const FString& FieldName, float Value);
	void SetInt(const FString& FieldName, int32 Value);
	void SetBool(const FString& FieldName, bool Value);
	void SetString(const FString& FieldName, const FString& Value);
	void SetFloatArray(const FString& FieldName, const TArray<float>& Value);
	auto SetIntArray(const FString& FieldName, const TArray<int32>& Value) -> void;
	void SetBoolArray(const FString& FieldName, const TArray<bool>& Value);
	
	// Value getters
	float GetFloat(const FString& FieldName) const;
	int32 GetInt(const FString& FieldName) const;
	bool GetBool(const FString& FieldName) const;
	FString GetString(const FString& FieldName) const;
	TArray<float> GetFloatArray(const FString& FieldName) const;
	TArray<int32> GetIntArray(const FString& FieldName) const;
	TArray<bool> GetBoolArray(const FString& FieldName) const;
};

// Serialization operator
FORCEINLINE FArchive& operator<<(FArchive& Ar, FUDPPacket& Packet)
{
    if (Ar.IsLoading())
    {
        // When loading (deserializing), make sure we read the data properly
        Ar << Packet.Data;
    }
    else
    {
        // When saving (serializing), we need to handle variable-length strings
        if (Packet.Structure)
        {
            TArray<uint8> SerializedData = Packet.Data;
            
            // For each field, check if it's a string and if so, adjust the data
            for (const auto& FieldPair : Packet.Structure->Fields)
            {
				const FString& FieldName = FieldPair.Key;
            	const FUDPField& Field = FieldPair.Value;
            	
                if (Field.DataType == EUDPDataType::String)
                {
                    int32 Offset = Packet.Structure->GetFieldOffset(FieldName);
                    if (Offset >= 0)
                    {
                        // Read the string length
                        int32 Length = 0;
                        FMemory::Memcpy(&Length, Packet.Data.GetData() + Offset, sizeof(int32));
                        
                        // We only need to include actual string length in the final packet
                        // Not the max length that's reserved
                        int32 StringSize = sizeof(int32) + Length;
                        
                        // Rest of packet data after this string
                        int32 NextFieldStart = Offset + sizeof(int32) + Field.MaxLength;
                        
                        // Adjust the serialized data - remove any padding that isn't needed
                        if (NextFieldStart < SerializedData.Num())
                        {
                            int32 BytesToRemove = Field.MaxLength - Length;
                            SerializedData.RemoveAt(Offset + StringSize, BytesToRemove);
                        }
                    }
                }
            }
            
            // Send the optimized data
            Ar << SerializedData;
        }
        else
        {
            // No structure, just send as is
            Ar << Packet.Data;
        }
    }
    
    return Ar;
}
