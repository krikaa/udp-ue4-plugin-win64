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

// ----------- SET AND GET FUNCTIONS FOR DIFFERENT DATATYPES -----------
	
// --- SINGLE VALUES ---
	
// Setters

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

void FUDPPacket::SetByte(const FString& FieldName, uint8 Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Byte)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(uint8));
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

void FUDPPacket::SetInt64(const FString& FieldName, int64 Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Int64)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(int64));
    }
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

void FUDPPacket::SetDouble(const FString& FieldName, double Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Double)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(double));
    }
}

void FUDPPacket::SetString(const FString& FieldName, const FString& Value)
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::String);
    if (Offset < 0)
        return;

    // Get the field to check MaxLength
    const FUDPField* Field = Structure->Fields.Find(FieldName);
    if (!Field)
        return;

    // Restrict string length to MaxLength
    FString TruncatedValue = Value;
    if (TruncatedValue.Len() > Field->MaxLength)
    {
        TruncatedValue = TruncatedValue.Left(Field->MaxLength);
        UE_LOG(LogTemp, Warning, TEXT("String '%s' truncated to max length of %d"), *FieldName, Field->MaxLength);
    }

    // Convert to UTF-8
    FTCHARToUTF8 UTF8String(*TruncatedValue);
    int32 Length = UTF8String.Length();
    
    // Make sure Data array is large enough
    if (Offset + sizeof(int32) + Field->MaxLength > Data.Num())
    {
        Data.SetNum(Offset + sizeof(int32) + Field->MaxLength);
    }

    // Write the length
    FMemory::Memcpy(Data.GetData() + Offset, &Length, sizeof(int32));
    
    // Write the string data
    if (Length > 0)
    {
        FMemory::Memcpy(Data.GetData() + Offset + sizeof(int32), UTF8String.Get(), Length);
    }
    
    // Zero out remaining bytes if any
    if (Length < Field->MaxLength)
    {
        FMemory::Memset(Data.GetData() + Offset + sizeof(int32) + Length, 0, Field->MaxLength - Length);
    }
}

void FUDPPacket::SetVector(const FString& FieldName, FVector Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Vector)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(FVector));
    }
}

void FUDPPacket::SetRotator(const FString& FieldName, FRotator Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Rotator)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(FRotator));
    }
}

void FUDPPacket::SetTransform(const FString& FieldName, FTransform Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Transform)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(FTransform));
    }
}

// Getters

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

uint8 FUDPPacket::GetByte(const FString& FieldName) const
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Byte);
    if (Offset < 0)
        return 0;
    
    uint8 Value = 0;
    
    if (Data.Num() >= Offset + sizeof(uint8))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(uint8));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for byte at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(uint8), Data.Num());
        return false;
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

int64 FUDPPacket::GetInt64(const FString& FieldName) const
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Int64);
    if (Offset < 0)
        return 0;
    
    int32 Value = 0;
    
    if (Data.Num() >= Offset + sizeof(int64))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(int64));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for integer64 at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(int64), Data.Num());
        return 0;
    }

    return Value;
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

double FUDPPacket::GetDouble(const FString& FieldName) const
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Double);
    if (Offset < 0)
        return 0.0f;
    
    double Value = 0.0f;
    
    if (Data.Num() >= Offset + sizeof(double))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(double));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for double at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(double), Data.Num());
        return 0.0f;
    }

    return Value;
}

FString FUDPPacket::GetString(const FString& FieldName) const
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::String);
    if (Offset < 0 || Offset + sizeof(int32) > Data.Num())
        return FString();

    int32 Length = 0;
    FMemory::Memcpy(&Length, Data.GetData() + Offset, sizeof(int32));

    // Get the field to check MaxLength
    const FUDPField* Field = Structure->Fields.Find(FieldName);
    if (!Field)
        return FString();

    // Ensure length is valid
    Length = FMath::Min(Length, Field->MaxLength);
    Length = FMath::Min(Length, int32(Data.Num() - Offset - sizeof(int32)));
    
    if (Length <= 0)
        return FString();

    // Create a temporary buffer for the UTF-8 string
    TArray<ANSICHAR> Utf8Buffer;
    Utf8Buffer.SetNum(Length + 1);  // +1 for null terminator
    FMemory::Memcpy(Utf8Buffer.GetData(), Data.GetData() + Offset + sizeof(int32), Length);
    Utf8Buffer[Length] = '\0';  // Ensure null termination

    // Convert UTF-8 to FString
    return FString(UTF8_TO_TCHAR(Utf8Buffer.GetData()));
}

FVector FUDPPacket::GetVector(const FString& FieldName) const
{
    FVector Value = FVector::ZeroVector;
    
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Vector);
    if (Offset < 0)
        return Value;
    
    if (Data.Num() >= Offset + sizeof(FVector))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(FVector));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for vector at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(FVector), Data.Num());
        return Value;
    }

    return Value;
}

FRotator FUDPPacket::GetRotator(const FString& FieldName) const
{
    FRotator Value = FRotator::ZeroRotator;
    
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Rotator);
    if (Offset < 0)
        return Value;
    
    if (Data.Num() >= Offset + sizeof(FRotator))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(FRotator));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for rotator at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(FRotator), Data.Num());
        return Value;
    }

    return Value;
}

FTransform FUDPPacket::GetTransform(const FString& FieldName) const
{
    FTransform Value = FTransform::Identity;
    
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Transform);
    if (Offset < 0)
        return Value;
    
    if (Data.Num() >= Offset + sizeof(FTransform))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(FTransform));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for transform at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(FTransform), Data.Num());
        return Value;
    }

    return Value;
}

// --- ARRAYS ---
	
// Setters

void FUDPPacket::SetBoolArray(const FString& FieldName, const TArray<bool>& Value)
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Bool);
    if (Offset < 0 || !Structure) return;

    const FUDPField* Field = Structure->Fields.Find(FieldName);
    if (!Field || !Field->IsArray) return;

    int32 ElementCount = FMath::Min(Value.Num(), Field->Count);
    if (ElementCount > 0)
    {
        FMemory::Memcpy(Data.GetData() + Offset, Value.GetData(), ElementCount * sizeof(bool));
    }
}

void FUDPPacket::SetIntArray(const FString& FieldName, const TArray<int32>& Value)
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Int);
    if (Offset < 0 || !Structure) return;
    
    const FUDPField* Field = Structure->Fields.Find(FieldName);
    if (!Field || !Field->IsArray) return;
    
    int32 ElementCount = FMath::Min(Value.Num(), Field->Count);
    if (ElementCount > 0)
    {
        FMemory::Memcpy(Data.GetData() + Offset, Value.GetData(), ElementCount * sizeof(int32));
    }
}

void FUDPPacket::SetFloatArray(const FString& FieldName, const TArray<float>& Value)
{
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Float);
    if (Offset < 0 || !Structure) return;

    const FUDPField* Field = Structure->Fields.Find(FieldName);
    if (!Field || !Field->IsArray) return;

    int32 ElementCount = FMath::Min(Value.Num(), Field->Count);
    if (ElementCount > 0)
    {
        FMemory::Memcpy(Data.GetData() + Offset, Value.GetData(), ElementCount * sizeof(float));
    }
}

// Getters

TArray<bool> FUDPPacket::GetBoolArray(const FString& FieldName) const
{
    TArray<bool> Result;
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Bool);
    if (Offset < 0 || !Structure) return Result;

    const FUDPField* Field = Structure->Fields.Find(FieldName);
    if (!Field || !Field->IsArray) return Result;

    const int32 ElementCount = Field->Count;
  
    if (Data.Num() >= Offset + (ElementCount * sizeof(bool)))
    {
        Result.SetNumUninitialized(ElementCount);
        FMemory::Memcpy(Result.GetData(), Data.GetData() + Offset, ElementCount * sizeof(bool));
    }

    return Result;
}

TArray<int32> FUDPPacket::GetIntArray(const FString& FieldName) const
{
    TArray<int32> Result;
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Int);
    if (Offset < 0 || !Structure) return Result;
    
    const FUDPField* Field = Structure->Fields.Find(FieldName);
    if (!Field || !Field->IsArray) return Result;

    const int32 ElementCount = Field->Count;
  
    if (Data.Num() >= Offset + (ElementCount * sizeof(int32)))
    {
        Result.SetNumUninitialized(ElementCount);
        FMemory::Memcpy(Result.GetData(), Data.GetData() + Offset, ElementCount * sizeof(int32));
    }

    return Result;
}

TArray<float> FUDPPacket::GetFloatArray(const FString& FieldName) const
{
    TArray<float> Result;
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Float);
    if (Offset < 0 || !Structure) return Result;

    const FUDPField* Field = Structure->Fields.Find(FieldName);
    if (!Field || !Field->IsArray) return Result;

    const int32 ElementCount = Field->Count;
  
    if (Data.Num() >= Offset + (ElementCount * sizeof(float)))
    {
        Result.SetNumUninitialized(ElementCount);
        FMemory::Memcpy(Result.GetData(), Data.GetData() + Offset, ElementCount * sizeof(float));
    }

    return Result;
}

// --- CUSTOM ---

// Setters

void FUDPPacket::SetCustomStruct(const FString& FieldName, const FUDPCustomStruct& Value)
{
    if (!Structure)
        return;
        
    int32 Offset = Structure->GetFieldOffset(FieldName);
    if (Offset >= 0 && Structure->GetFieldType(FieldName) == EUDPDataType::Custom)
    {
        FMemory::Memcpy(Data.GetData() + Offset, &Value, sizeof(FUDPCustomStruct));
    }
}

// Getters

FUDPCustomStruct FUDPPacket::GetCustomStruct(const FString& FieldName) const
{
    FUDPCustomStruct Value;
    
    int32 Offset = CheckAndGetOffset(FieldName, EUDPDataType::Custom);
    if (Offset < 0)
        return Value;
    
    if (Data.Num() >= Offset + sizeof(FUDPCustomStruct))
    {
        FMemory::Memcpy(&Value, Data.GetData() + Offset, sizeof(FUDPCustomStruct));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Data array too small for custom structure at offset %d (needs %llu bytes, has %d)"), 
               Offset, Offset + sizeof(FUDPCustomStruct), Data.Num());
        return Value;
    }

    return Value;
}
