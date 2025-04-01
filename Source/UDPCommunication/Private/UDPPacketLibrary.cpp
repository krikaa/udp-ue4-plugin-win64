// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPPacketLibrary.h"

void UUDPPacketLibrary::InitWithStructure(UPARAM(ref) FUDPPacket& UDPPacket, UUDPPacketStructure* Structure)
{
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("InitWithStructure: Invalid packet structure"));
		return;
	}

	// Make sure the packet structure is compiled
	Structure->CompileStructure();

	// Set the packet structure reference
	UDPPacket.Structure = Structure;

	// Resize the data buffer to match the packet size
	UDPPacket.Data.SetNumZeroed(Structure->PacketSize);

	UE_LOG(LogTemp, Verbose, TEXT("Initialized UDP packet with structure %s (size: %d)"), 
		   *Structure->GetName(), Structure->PacketSize);
}

void UUDPPacketLibrary::ValidateWithStructure(UPARAM(ref) FUDPPacket& UDPPacket, UUDPPacketStructure* Structure)
{
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("InitWithStructure: Invalid packet structure"));
		return;
	}
	
	// Make sure the packet structure is compiled
	Structure->CompileStructure();

	// Set the packet structure reference
	UDPPacket.Structure = Structure;
    
	// Optional: Validate packet against structure schema
	// You could check if the packet data matches the expected structure
    
	return;
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