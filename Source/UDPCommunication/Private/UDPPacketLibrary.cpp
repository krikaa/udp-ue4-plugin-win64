// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPPacketLibrary.h"

void UUDPPacketLibrary::InitWithStructure(UPARAM(ref) FUDPPacket& UDPPacket, UUDPPacketStructure* Structure)
{
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid packet structure"));
		return;
	}

	// Make sure the packet structure is compiled and set
	Structure->CompileStructure();
	UDPPacket.Structure = Structure;

	// Resize the data buffer to match the packet size
	UDPPacket.Data.SetNumZeroed(Structure->PacketSize);

	UE_LOG(LogTemp, Verbose, TEXT("Initialized UDP sender structure: %s (%d bytes, %d fields"), 
		   *Structure->GetName(),  UDPPacket.Data.Num(), Structure->Fields.Num());
}

void UUDPPacketLibrary::ValidateWithStructure(UPARAM(ref) FUDPPacket& UDPPacket, UUDPPacketStructure* Structure)
{
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid packet structure"));
		return;
	}
	
	UE_LOG(LogTemp, Display, TEXT("Initialized UDP receiver structure: %s (%d bytes, %d fields"), 
           *Structure->GetName(), UDPPacket.Data.Num(), Structure->Fields.Num());

    // Ensure the packet has the structure reference
    UDPPacket.Structure = Structure;
}

FUDPPacket UUDPPacketLibrary::CreateUDPPacket()
{
	return FUDPPacket();
}


// ----------- SET AND GET FUNCTIONS FOR DIFFERENT DATATYPES -----------

// --- SINGLE VALUES ---

// Setters

void UUDPPacketLibrary::SetBool(FUDPPacket& UDPPacket, const FString& FieldName, bool Value)
{
	UDPPacket.SetBool(FieldName, Value);
}

void UUDPPacketLibrary::SetByte(FUDPPacket& UDPPacket, const FString& FieldName, uint8 Value)
{
	UDPPacket.SetByte(FieldName, Value);
}

void UUDPPacketLibrary::SetInt(FUDPPacket& UDPPacket, const FString& FieldName, int32 Value)
{
	UDPPacket.SetInt(FieldName, Value);
}

void UUDPPacketLibrary::SetInt64(FUDPPacket& UDPPacket, const FString& FieldName, int64 Value)
{
	UDPPacket.SetInt64(FieldName, Value);
}

void UUDPPacketLibrary::SetFloat(FUDPPacket& UDPPacket, const FString& FieldName, float Value)
{
	UDPPacket.SetFloat(FieldName, Value);
}

void UUDPPacketLibrary::SetDouble(FUDPPacket& UDPPacket, const FString& FieldName, double Value)
{
	UDPPacket.SetDouble(FieldName, Value);
}

void UUDPPacketLibrary::SetString(FUDPPacket& UDPPacket, const FString& FieldName, const FString& Value)
{
	UDPPacket.SetString(FieldName, Value);
}

void UUDPPacketLibrary::SetVector(FUDPPacket& UDPPacket, const FString& FieldName, FVector Value)
{
	UDPPacket.SetVector(FieldName, Value);
}

void UUDPPacketLibrary::SetRotator(FUDPPacket& UDPPacket, const FString& FieldName, FRotator Value)
{
	UDPPacket.SetRotator(FieldName, Value);
}

void UUDPPacketLibrary::SetTransform(FUDPPacket& UDPPacket, const FString& FieldName, FTransform Value)
{
	UDPPacket.SetTransform(FieldName, Value);
}

// Getters

bool UUDPPacketLibrary::GetBool(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetBool(FieldName);
}

uint8 UUDPPacketLibrary::GetByte(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetByte(FieldName);
}

int32 UUDPPacketLibrary::GetInt(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetInt(FieldName);
}

int64 UUDPPacketLibrary::GetInt64(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetInt64(FieldName);
}

float UUDPPacketLibrary::GetFloat(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetFloat(FieldName);
}

double UUDPPacketLibrary::GetDouble(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetDouble(FieldName);
}

FString UUDPPacketLibrary::GetString(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetString(FieldName);
}

FVector UUDPPacketLibrary::GetVector(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetVector(FieldName);
}

FRotator UUDPPacketLibrary::GetRotator(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetRotator(FieldName);
}

FTransform UUDPPacketLibrary::GetTransform(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetTransform(FieldName);
}

// --- ARRAYS ---

// Setters

void UUDPPacketLibrary::SetBoolArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<bool>& Value)
{
	UDPPacket.SetBoolArray(FieldName, Value);
}

void UUDPPacketLibrary::SetIntArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<int32>& Value)
{
	UDPPacket.SetIntArray(FieldName, Value);
}

void UUDPPacketLibrary::SetFloatArray(FUDPPacket& UDPPacket, const FString& FieldName, const TArray<float>& Value)
{
	UDPPacket.SetFloatArray(FieldName, Value);
}

// Getters

TArray<bool> UUDPPacketLibrary::GetBoolArray(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetBoolArray(FieldName);
}

TArray<int32> UUDPPacketLibrary::GetIntArray(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetIntArray(FieldName);
}

TArray<float> UUDPPacketLibrary::GetFloatArray(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetFloatArray(FieldName);
}

// --- CUSTOM ----

// Setters

void UUDPPacketLibrary::SetCustomStruct(FUDPPacket& UDPPacket, const FString& FieldName, FUDPCustomStruct Value)
{
	UDPPacket.SetCustomStruct(FieldName, Value);
}

// Getters

FUDPCustomStruct UUDPPacketLibrary::GetCustomStruct(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetCustomStruct(FieldName);
}
