#include "UDPDynamicData.h"

FUDPDynamicData::FUDPDynamicData() : Definition(nullptr)
{
}

void FUDPDynamicData::InitWithDefinition(UUDPPacketDefinition* InDefinition)
{
    if (!InDefinition)
        return;
        
    Definition = InDefinition;
    Data.SetNumZeroed(Definition->PacketSize);
}

void FUDPDynamicData::SetFloat(const FString& FieldName, float Value)
{
    if (!Definition)
        return;
        
    int32 Offset = Definition->GetFieldOffset(FieldName);
    if (Offset >= 0 && Definition->GetFieldType(FieldName) == EUDPDataType::Float)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(float));
    }
}

void FUDPDynamicData::SetInt(const FString& FieldName, int32 Value)
{
    if (!Definition)
        return;
        
    int32 Offset = Definition->GetFieldOffset(FieldName);
    if (Offset >= 0 && Definition->GetFieldType(FieldName) == EUDPDataType::Int)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(int32));
    }
}

void FUDPDynamicData::SetBool(const FString& FieldName, bool Value)
{
    if (!Definition)
        return;
        
    int32 Offset = Definition->GetFieldOffset(FieldName);
    if (Offset >= 0 && Definition->GetFieldType(FieldName) == EUDPDataType::Bool)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(bool));
    }
}

void FUDPDynamicData::SetString(const FString& FieldName, const FString& Value)
{
    if (!Definition)
        return;
        
    int32 Offset = Definition->GetFieldOffset(FieldName);
    if (Offset >= 0 && Definition->GetFieldType(FieldName) == EUDPDataType::String)
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

float FUDPDynamicData::GetFloat(const FString& FieldName) const
{
    if (!Definition)
        return 0.0f;
        
    float Result = 0.0f;
    int32 Offset = Definition->GetFieldOffset(FieldName);
    if (Offset >= 0 && Definition->GetFieldType(FieldName) == EUDPDataType::Float)
    {
        FMemory::Memcpy(&Result, Data.GetData() + Offset, sizeof(float));
    }
    return Result;
}

int32 FUDPDynamicData::GetInt(const FString& FieldName) const
{
    if (!Definition)
        return 0;
        
    int32 Result = 0;
    int32 Offset = Definition->GetFieldOffset(FieldName);
    if (Offset >= 0 && Definition->GetFieldType(FieldName) == EUDPDataType::Int)
    {
        FMemory::Memcpy(&Result, Data.GetData() + Offset, sizeof(int32));
    }
    return Result;
}

bool FUDPDynamicData::GetBool(const FString& FieldName) const
{
    if (!Definition)
        return false;
        
    bool Result = false;
    int32 Offset = Definition->GetFieldOffset(FieldName);
    if (Offset >= 0 && Definition->GetFieldType(FieldName) == EUDPDataType::Bool)
    {
        FMemory::Memcpy(&Result, Data.GetData() + Offset, sizeof(bool));
    }
    return Result;
}

FString FUDPDynamicData::GetString(const FString& FieldName) const
{
    if (!Definition)
        return FString();
        
    int32 Offset = Definition->GetFieldOffset(FieldName);
    if (Offset >= 0 && Definition->GetFieldType(FieldName) == EUDPDataType::String)
    {
        int32 Length = 0;
        FMemory::Memcpy(&Length, Data.GetData() + Offset, sizeof(int32));
        
        if (Length <= 0)
            return FString();
            
        TArray<ANSICHAR> StringData;
        StringData.SetNumUninitialized(Length + 1);
        FMemory::Memcpy(StringData.GetData(), Data.GetData() + Offset + sizeof(int32), Length);
        StringData[Length] = 0; // Null terminate
        
        return FString(UTF8_TO_TCHAR(StringData.GetData()));
    }
    return FString();
}