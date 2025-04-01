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

float FUDPPacket::GetFloat(const FString& FieldName) const
{
    if (!Structure)
        return 0.0f;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset < 0 || Structure->GetFieldType(FieldName) != EUDPDataType::Float)
    {
        return 0.0f;
    }

    float Value = 0.0f;
    if (Data.Num() >= Offset + sizeof(float))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(float));
    }

    return Value;
}

int32 FUDPPacket::GetInt(const FString& FieldName) const
{
    if (!Structure)
        return 0;
        
    int32 Result = 0;
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Int)
    {
        FMemory::Memcpy(&Result, Data.GetData() + Offset, sizeof(int32));
    }
    return Result;
}

bool FUDPPacket::GetBool(const FString& FieldName) const
{
    if (!Structure)
        return false;
        
    bool Result = false;
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Bool)
    {
        FMemory::Memcpy(&Result, Data.GetData() + Offset, sizeof(bool));
    }
    return Result;
}

FString FUDPPacket::GetString(const FString& FieldName) const
{
    if (!Structure)
        return FString();
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::String)
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