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
    UE_LOG(LogTemp, Display, TEXT("Extracting float field '%s' from packet with %d bytes"), 
           *FieldName, Data.Num());
    
    if (!Structure)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing packet structure"));
        return 0.0f;
    }
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    UE_LOG(LogTemp, Display, TEXT("GetFloat: Field '%s' offset is %d"), *FieldName, Offset);
    
    if (Offset < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Field '%s' not found in structure"), *FieldName);
        return 0.0f;
    }

    if (Structure->GetFieldType(FieldName) != EUDPDataType::Float)
    {
        UE_LOG(LogTemp, Warning, TEXT("Field '%s' is not a float"), *FieldName);
        return 0.0f;
    }
    
    // Dump the raw bytes for debugging
    FString ByteDump = TEXT("Raw bytes at offset: ");
    for (int32 i = 0; i < sizeof(float) && Offset + i < Data.Num(); i++)
    {
        ByteDump.Append(FString::Printf(TEXT("%02X "), Data[Offset + i]));
    }
    UE_LOG(LogTemp, Display, TEXT("%s"), *ByteDump);

    float Value = 0.0f;
    if (Data.Num() >= Offset + sizeof(float))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(float));
        UE_LOG(LogTemp, Display, TEXT("Successfully extracted float %f from offset %d"), Value, Offset);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for float at offset %d (needs %d bytes, has %d)"), 
               Offset, Offset + sizeof(float), Data.Num());
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