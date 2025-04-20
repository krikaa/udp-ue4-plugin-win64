// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UDPCustomStruct.h"
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
    
	// ----------- SET AND GET FUNCTIONS FOR DIFFERENT DATATYPES -----------
	
	// --- SINGLE VALUES ---
	
	// Setters
	void SetBool(const FString& FieldName, bool Value);
	void SetByte(const FString& FieldName, uint8 Value);
	void SetInt(const FString& FieldName, int32 Value);
	void SetInt64(const FString& FieldName, int64 Value);
	void SetFloat(const FString& FieldName, float Value);
	void SetDouble(const FString& FieldName, double Value);
	void SetString(const FString& FieldName, const FString& Value);
	void SetVector(const FString& FieldName, FVector Value);
	void SetRotator(const FString& FieldName, FRotator Value);
	void SetTransform(const FString& FieldName, FTransform Value);
	
	// Getters
	bool GetBool(const FString& FieldName) const;
	uint8 GetByte(const FString& FieldName) const;
	int32 GetInt(const FString& FieldName) const;
	int64 GetInt64(const FString& FieldName) const;
	float GetFloat(const FString& FieldName) const;
	double GetDouble(const FString& FieldName) const;
	FString GetString(const FString& FieldName) const;
	FVector GetVector(const FString& FieldName) const;
	FRotator GetRotator(const FString& FieldName) const;
	FTransform GetTransform(const FString& FieldName) const;

	// --- ARRAYS ---
	
	// Setters
	void SetBoolArray(const FString& FieldName, const TArray<bool>& Value);
	void SetIntArray(const FString& FieldName, const TArray<int32>& Value);
	void SetFloatArray(const FString& FieldName, const TArray<float>& Value);
	
	// Getters
	TArray<bool> GetBoolArray(const FString& FieldName) const;
	TArray<int32> GetIntArray(const FString& FieldName) const;
	TArray<float> GetFloatArray(const FString& FieldName) const;

	// --- CUSTOM ---

	// Setters
	void SetCustomStruct(const FString& FieldName, const FUDPCustomStruct& Value);

	// Getters
	FUDPCustomStruct GetCustomStruct(const FString& FieldName) const;
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, FUDPPacket& Packet)
{
    if (Ar.IsLoading())
    {
        Ar << Packet.Data;
    }
    else
    {
        // When serializing, we need to handle variable-length strings
        if (Packet.Structure)
        {
            TArray<uint8> SerializedData = Packet.Data;
        	
            for (const auto& FieldPair : Packet.Structure->Fields)
            {
				const FString& FieldName = FieldPair.Key;
            	const FUDPField& Field = FieldPair.Value;
            	
            	if (Field.DataType == EUDPDataType::String)
            	{
            		int32 Offset = Packet.Structure->GetFieldOffset(FieldName);
            		if (Offset >= 0 && Offset < SerializedData.Num())
            		{
            			int32 Length = 0;
            			// Make sure we have enough data to read the length
            			if (Offset + sizeof(int32) <= SerializedData.Num())
            			{
            				FMemory::Memcpy(&Length, SerializedData.GetData() + Offset, sizeof(int32));
                            
            				// Ensure length is valid
            				Length = FMath::Min(Length, Field.MaxLength);
                            
            				// Calculate the size of the string in the buffer
            				int32 StringSize = sizeof(int32) + Length;
            				int32 NextFieldStart = Offset + sizeof(int32) + Field.MaxLength;

            				// Make sure we're not going out of bounds
            				if (NextFieldStart < SerializedData.Num())
            				{
            					int32 BytesToRemove = Field.MaxLength - Length;
            					// Ensure we're not removing too many bytes
            					if (BytesToRemove > 0 && Offset + StringSize + BytesToRemove <= SerializedData.Num())
            					{
            						SerializedData.RemoveAt(Offset + StringSize, BytesToRemove);
            					}
            				}
            			}
            		}
            	}
            }
        	
            Ar << SerializedData;
        }
        else
        {
            Ar << Packet.Data;
        }
    }
    
    return Ar;
}
