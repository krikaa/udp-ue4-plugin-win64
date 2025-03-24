#pragma once

#include "CoreMinimal.h"
#include "UDPDataType.generated.h"

// Enum for declaring the types of data that will be sent/received
UENUM(BlueprintType)
enum class EUDPDataType : uint8
{
	Float   UMETA(DisplayName = "Float"),
	Int     UMETA(DisplayName = "Integer"),
	Bool    UMETA(DisplayName = "Boolean"),
	String  UMETA(DisplayName = "String")
};
