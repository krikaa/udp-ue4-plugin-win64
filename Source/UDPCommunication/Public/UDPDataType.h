// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPDataType.generated.h"

// Enum for declaring the types of data that will be sent/received
UENUM(NotBlueprintType)
enum class EUDPDataType : uint8
{
	Float		UMETA(DisplayName = "Float", ToolTip="32 bits"),
	Int			UMETA(DisplayName = "Integer", ToolTip="32 bits"),
	Bool		UMETA(DisplayName = "Boolean", ToolTip="8 bits"),
	String		UMETA(DisplayName = "String", ToolTip="32 bit header + 8 bits per UTF-8 character"),
	Custom		UMETA(DisplayName = "Custom", ToolTip="Custom structure defined in C++ (UDPCustomStruct)"),
};
