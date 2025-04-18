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

void UUDPPacketLibrary::SetFloat(FUDPPacket& UDPPacket, const FString& FieldName, float Value)
{
	UDPPacket.SetFloat(FieldName, Value);
}

void UUDPPacketLibrary::SetInt(FUDPPacket& UDPPacket, const FString& FieldName, int32 Value)
{
	UDPPacket.SetInt(FieldName, Value);
}

void UUDPPacketLibrary::SetBool(FUDPPacket& UDPPacket, const FString& FieldName, bool Value)
{
	UDPPacket.SetBool(FieldName, Value);
}

void UUDPPacketLibrary::SetString(FUDPPacket& UDPPacket, const FString& FieldName, const FString& Value)
{
	UDPPacket.SetString(FieldName, Value);
}

float UUDPPacketLibrary::GetFloat(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetFloat(FieldName);
}

int32 UUDPPacketLibrary::GetInt(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetInt(FieldName);
}

bool UUDPPacketLibrary::GetBool(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetBool(FieldName);
}

FString UUDPPacketLibrary::GetString(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetString(FieldName);
}

void UUDPPacketLibrary::SetFloatArray(FUDPPacket& UDPPacket, const FString& FieldName, const TArray<float>& Value)
{
	UDPPacket.SetFloatArray(FieldName, Value);
}

void UUDPPacketLibrary::SetIntArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<int32>& Value)
{
	UDPPacket.SetIntArray(FieldName, Value);
}

void UUDPPacketLibrary::SetBoolArray(UPARAM(ref) FUDPPacket& UDPPacket, const FString& FieldName, const TArray<bool>& Value)
{
	UDPPacket.SetBoolArray(FieldName, Value);
}

TArray<float> UUDPPacketLibrary::GetFloatArray(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetFloatArray(FieldName);
}

TArray<int32> UUDPPacketLibrary::GetIntArray(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetIntArray(FieldName);
}

TArray<bool> UUDPPacketLibrary::GetBoolArray(const FUDPPacket& UDPPacket, const FString& FieldName)
{
	return UDPPacket.GetBoolArray(FieldName);
}