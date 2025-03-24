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
	FieldOffsets.Empty();
	FieldTypes.Empty();

	int32 CurrentOffset = 0;
	int32 FieldCounter = 1;

	// First pass: calculate offsets with proper alignment
	for (FUDPField& Field : Fields)
	{
		// Auto-name empty fields (Field 1, Field 2, etc.)
		if (Field.Name.IsEmpty())
		{
			Field.Name = FString::Printf(TEXT("Field %d"), FieldCounter);
		}
		FieldCounter++;

		int32 Alignment = Field.GetFieldAlignment();
		// Align the current offset
		CurrentOffset = (CurrentOffset + Alignment - 1) & ~(Alignment - 1);

		FieldOffsets.Add(Field.Name, CurrentOffset);
		FieldTypes.Add(Field.Name, Field.DataType);

		CurrentOffset += Field.GetFieldSize();
	}

	// Store the final size
	PacketSize = CurrentOffset;
}

int32 UUDPPacketStructure::GetFieldOffset(const FString& FieldName) const
{
	return FieldOffsets.Contains(FieldName) ? FieldOffsets[FieldName] : -1;
}

EUDPDataType UUDPPacketStructure::GetFieldType(const FString& FieldName) const
{
	return FieldTypes.Contains(FieldName) ? FieldTypes[FieldName] : EUDPDataType::Float;
}