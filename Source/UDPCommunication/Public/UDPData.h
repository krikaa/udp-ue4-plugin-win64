#pragma once

#include "Serialization/Archive.h"
#include "UDPData.generated.h"

USTRUCT(BlueprintType)
struct FUDPData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
		float Float1 = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
		float Float2 = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
		float Float3 = 0.0f;
	
	FUDPData()
	{
	}
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FUDPData &Structure)
{
	Ar << Structure.Float1;
	Ar << Structure.Float2;
	Ar << Structure.Float3;
	return Ar;
}
