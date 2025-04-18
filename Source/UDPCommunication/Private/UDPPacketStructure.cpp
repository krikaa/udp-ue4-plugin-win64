// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPPacketStructure.h"

int32 FUDPField::GetFieldSize() const
{
    switch (DataType)
    {
    case EUDPDataType::Float: return sizeof(float);
    case EUDPDataType::Int: return sizeof(int32);
    // case EUDPDataType::Byte: return sizeof(uint8);
    case EUDPDataType::Bool: return sizeof(bool);
    case EUDPDataType::String: return sizeof(int32) + MaxLength;
    // case EUDPDataType::Vector: return sizeof(FVector);
    // case EUDPDataType::Vector2D: return sizeof(FVector2D);
    // case EUDPDataType::Rotator: return sizeof(FRotator);
    // case EUDPDataType::Quat: return sizeof(FQuat);
    // case EUDPDataType::Color: return sizeof(FColor);
    // case EUDPDataType::UInt16: return sizeof(uint16);
    // case EUDPDataType::UInt32: return sizeof(uint32);
    // case EUDPDataType::UInt64: return sizeof(uint64);
    // case EUDPDataType::Int64: return sizeof(int64);
    // case EUDPDataType::Double: return sizeof(double);
    	
    default: return 0;
    }
}

int32 FUDPField::GetFieldAlignment() const
{
	switch (DataType)
	{
	case EUDPDataType::Float: return alignof(float);
	case EUDPDataType::Int: return alignof(int32);
	case EUDPDataType::Bool: return alignof(bool);
	case EUDPDataType::String: return alignof(int32);
	default: return 1;
	}
}

UUDPPacketStructure::UUDPPacketStructure()
{
	PacketSize = 0;
}

void UUDPPacketStructure::PostLoad()
{
	Super::PostLoad();
	CompileStructure();
}

void UUDPPacketStructure::CompileStructure()
{
	int32 CurrentOffset = 0;

	// Calculate offsets with proper alignment
	for (auto& FieldPair : Fields)
	{
		FUDPField& Field = FieldPair.Value;
        
		// Apply alignment
		int32 Alignment = Field.GetFieldAlignment();
		CurrentOffset = (CurrentOffset + Alignment - 1) & ~(Alignment - 1);
        
		// Store offset directly in the field
		Field.Offset = CurrentOffset;
        
		// Calculate field size based on type and array status
		int32 ElementSize = Field.GetFieldSize();
        
		// Advance the offset
		if (Field.IsArray && Field.DataType != EUDPDataType::String)
		{
			CurrentOffset += ElementSize * Field.Count;
		}
		else
		{
			CurrentOffset += ElementSize;
		}
	}

	// Store the final size
	PacketSize = CurrentOffset;
}

void UUDPPacketStructure::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UUDPPacketStructure, Fields))
	{
		// Check if an element was added (MapProperty will have MapAdd notification)
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
		{
			// Find the newly added element with empty key and give it a name
			for (auto& Pair : Fields)
			{
				if (Pair.Key.IsEmpty())
				{
					// Generate a unique name
					int32 Index = 1;
					FString NewName;
					do {
						NewName = FString::Printf(TEXT("Field %d"), Index++);
					} while (Fields.Contains(NewName) && Index < 1000);
                    
					// Rename the key
					Fields.Add(NewName, Pair.Value);
					Fields.Remove("");
					break;
				}
			}
		}
        
		// Always recompile when the structure changes
		CompileStructure();
	}
}

int32 UUDPPacketStructure::GetFieldOffset(const FString& FieldName) const
{
	return Fields.Contains(FieldName) ? Fields[FieldName].Offset : -1;
}

EUDPDataType UUDPPacketStructure::GetFieldType(const FString& FieldName) const
{
	return Fields.Contains(FieldName) ? Fields[FieldName].DataType : EUDPDataType::Float;
}