// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPPacketStructure.h"

UUDPPacketStructure::UUDPPacketStructure()
{
	PacketSize = 0;
}

int32 FUDPField::GetFieldSize() const
{
    switch (DataType)
    {
    case EUDPDataType::Bool: return sizeof(bool);
    case EUDPDataType::Byte: return sizeof(uint8);
    case EUDPDataType::Int: return sizeof(int32);
    case EUDPDataType::Int64: return sizeof(int64);
    case EUDPDataType::Float: return sizeof(float);
    case EUDPDataType::Double: return sizeof(double);
    case EUDPDataType::String: return sizeof(int32) + MaxLength;
    case EUDPDataType::Vector: return sizeof(FVector);
    case EUDPDataType::Rotator: return sizeof(FRotator);
    case EUDPDataType::Transform: return sizeof(FTransform);
    case EUDPDataType::Custom: return sizeof(FUDPCustomStruct);
    default: return 0;
    }
}

int32 FUDPField::GetFieldAlignment() const
{
	switch (DataType)
	{
	case EUDPDataType::Bool: return alignof(bool);
	case EUDPDataType::Byte: return alignof(uint8);
	case EUDPDataType::Int: return alignof(int32);
	case EUDPDataType::Int64: return alignof(int64);
	case EUDPDataType::Float: return alignof(float);
	case EUDPDataType::Double: return alignof(double);
	case EUDPDataType::String: return alignof(int32);
	case EUDPDataType::Vector: return alignof(FVector);
	case EUDPDataType::Rotator: return alignof(FRotator);
	case EUDPDataType::Transform: return alignof(FTransform);
	case EUDPDataType::Custom: return alignof(FUDPCustomStruct);
	default: return 1;
	}
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
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
		{
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
		
		CompileStructure();
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UUDPPacketStructure, Fields))
	{
		if (PropertyChangedEvent.Property && 
			PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(FUDPField, DataType))
		{
			for (auto& Pair : Fields)
			{
				FUDPField& Field = Pair.Value;
				
				if (Field.IsArray && 
					(Field.DataType != EUDPDataType::Bool || 
					 Field.DataType != EUDPDataType::Int ||
					 Field.DataType != EUDPDataType::Float))
				{
					Field.IsArray = false;
					Field.Count = 1;
				}
			}
		}
        
		// Recompile the structure
		CompileStructure();
	}
}

bool UUDPPacketStructure::IsFullyLoaded() const
{
	return !HasAnyFlags(RF_NeedLoad | RF_NeedPostLoad);
}

int32 UUDPPacketStructure::GetFieldOffset(const FString& FieldName) const
{
	return Fields.Contains(FieldName) ? Fields[FieldName].Offset : -1;
}

EUDPDataType UUDPPacketStructure::GetFieldType(const FString& FieldName) const
{
	return Fields.Contains(FieldName) ? Fields[FieldName].DataType : EUDPDataType::Float;
}