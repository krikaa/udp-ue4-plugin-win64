// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPDynamicDataLibrary.h"

void UUDPDynamicDataLibrary::InitWithDefinition(UPARAM(ref) FUDPDynamicData& DynamicData, UUDPPacketDefinition* Definition)
{
	if (!Definition)
	{
		UE_LOG(LogTemp, Error, TEXT("InitWithDefinition: Invalid packet definition"));
		return;
	}

	// Make sure the packet definition is compiled
	Definition->CompileStructure();

	// Set the packet definition reference
	DynamicData.Definition = Definition;

	// Resize the data buffer to match the packet size
	DynamicData.Data.SetNumZeroed(Definition->PacketSize);

	UE_LOG(LogTemp, Verbose, TEXT("Initialized dynamic data with packet definition %s (size: %d)"), 
		   *Definition->GetName(), Definition->PacketSize);
}

// In UUDPDynamicDataLibrary.cpp
FUDPDynamicData UUDPDynamicDataLibrary::CreateDynamicData()
{
	return FUDPDynamicData();
}

void UUDPDynamicDataLibrary::SetFloat(FUDPDynamicData& DynamicData, const FString& FieldName, float Value)
{
	DynamicData.SetFloat(FieldName, Value);
}

void UUDPDynamicDataLibrary::SetInt(FUDPDynamicData& DynamicData, const FString& FieldName, int32 Value)
{
	DynamicData.SetInt(FieldName, Value);
}

void UUDPDynamicDataLibrary::SetBool(FUDPDynamicData& DynamicData, const FString& FieldName, bool Value)
{
	DynamicData.SetBool(FieldName, Value);
}

void UUDPDynamicDataLibrary::SetString(FUDPDynamicData& DynamicData, const FString& FieldName, const FString& Value)
{
	DynamicData.SetString(FieldName, Value);
}

float UUDPDynamicDataLibrary::GetFloat(const FUDPDynamicData& DynamicData, const FString& FieldName)
{
	return DynamicData.GetFloat(FieldName);
}

int32 UUDPDynamicDataLibrary::GetInt(const FUDPDynamicData& DynamicData, const FString& FieldName)
{
	return DynamicData.GetInt(FieldName);
}

bool UUDPDynamicDataLibrary::GetBool(const FUDPDynamicData& DynamicData, const FString& FieldName)
{
	return DynamicData.GetBool(FieldName);
}

FString UUDPDynamicDataLibrary::GetString(const FUDPDynamicData& DynamicData, const FString& FieldName)
{
	return DynamicData.GetString(FieldName);
}