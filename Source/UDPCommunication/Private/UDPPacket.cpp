// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPPacket.h"

FUDPPacket::FUDPPacket() : Structure(nullptr)
{
}

void FUDPPacket::InitWithStructure(UUDPPacketStructure* InitStructure)
{
    if (!InitStructure)
        return;
        
    Structure = InitStructure;
    Data.SetNumZeroed(Structure->PacketSize);
}

void FUDPPacket::SetFloat(const FString& FieldName, float Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Float)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(float));
    }
}

void FUDPPacket::SetInt(const FString& FieldName, int32 Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Int)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(int32));
    }
}

void FUDPPacket::SetBool(const FString& FieldName, bool Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Bool)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(bool));
    }
}

void FUDPPacket::SetString(const FString& FieldName, const FString& Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::String)
    {
        FTCHARToUTF8 Converted(*Value);
        int32 Length = FMath::Min(Converted.Length(), 127);
        
        // Write string length
        FMemory::Memcpy(Data.GetData() + Offset, &Length, sizeof(int32));
        
        // Write string data
        if (Length > 0)
        {
            FMemory::Memcpy(Data.GetData() + Offset + sizeof(int32), Converted.Get(), Length);
        }
    }
}

int32 FUDPPacket::CheckAndGetOffset(const FString& FieldName, EUDPDataType DataType) const
{
    if (!Structure)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing packet structure"));
        return -1;
    }

    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Field '%s' not found in structure, try refreshing the node"), *FieldName);
        return -1;
    }

    if (Structure->GetFieldType(FieldName) != DataType)
    {
        UE_LOG(LogTemp, Warning, TEXT("Field '%s' is not of the specified type"), *FieldName);
        return -1;
    }

    return Offset;
}

float FUDPPacket::GetFloat(const FString& FieldName) const
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Float);
    if (Offset < 0)
        return 0.0f;
    
    float Value = 0.0f;
    
    if (Data.Num() >= Offset + sizeof(float))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(float));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for float at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(float), Data.Num());
        return 0.0f;
    }

    return Value;
}

int32 FUDPPacket::GetInt(const FString& FieldName) const
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Int);
    if (Offset < 0)
        return 0;
    
    int32 Value = 0;
    
    if (Data.Num() >= Offset + sizeof(int32))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(int32));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for integer at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(int32), Data.Num());
        return 0;
    }

    return Value;
}

bool FUDPPacket::GetBool(const FString& FieldName) const
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Bool);
    if (Offset < 0)
        return false;
    
    bool Value = false;
    
    if (Data.Num() >= Offset + sizeof(bool))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(bool));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for bool at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(bool), Data.Num());
        return false;
    }

    return Value;
}

FString FUDPPacket::GetString(const FString& FieldName) const
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::String);
    if (Offset < 0)
        return TEXT("");

    int32 Length = 0;
    
    // Check if we have enough data to read the string length
    if (Data.Num() >= Offset + sizeof(int32))
    {
        FMemory::Memcpy(&Length, Data.GetData() + Offset, sizeof(int32));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for string length at offset %d (needs %d bytes, has %d)"),
               Offset, (int32)(Offset + sizeof(int32)), Data.Num());
        return TEXT("");
    }

    // Validate string length
    if (Length <= 0)
        return TEXT("");
        
    // Check if we have enough data for the full string
    if (Data.Num() >= Offset + sizeof(int32) + Length)
    {
        TArray<ANSICHAR> StringData;
        StringData.SetNumUninitialized(Length + 1);
        FMemory::Memcpy(StringData.GetData(), Data.GetData() + Offset + sizeof(int32), Length);
        StringData[Length] = 0; // Null terminate

        return FString(UTF8_TO_TCHAR(StringData.GetData()));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for string at offset %d (needs %d bytes, has %d)"),
               Offset, (int32)(Offset + sizeof(int32) + Length), Data.Num());
        return TEXT("");
    }
}