// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPPacketStructure.h"

int32 FUDPField::GetFieldSize() const
{
	switch (DataType)
	{
	case EUDPDataType::Float: return sizeof(float) * FMath::Max(1, ArraySize);
	case EUDPDataType::Int: return sizeof(int32) * FMath::Max(1, ArraySize);
	case EUDPDataType::Bool: return sizeof(bool) * FMath::Max(1, ArraySize);
	case EUDPDataType::String: return sizeof(int32) + (128 * FMath::Max(1, ArraySize)); // Size + chars
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
    
	// First pass: calculate offsets with proper alignment
	for (const FUDPField& Field : Fields)
	{
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