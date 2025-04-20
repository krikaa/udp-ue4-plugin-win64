// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPDataType.generated.h"

// Enum for declaring the types of data that will be sent/received
UENUM(NotBlueprintType)
enum class EUDPDataType : uint8
{
	Bool		UMETA(DisplayName = "Boolean", ToolTip="8 bits"),
	Byte		UMETA(DisplayName = "Byte", ToolTip="8 bits"),
	Int			UMETA(DisplayName = "Integer", ToolTip="32 bits"),
	Int64		UMETA(DisplayName = "Integer64", ToolTip="64 bits"),
	Float		UMETA(DisplayName = "Float", ToolTip="32 bits"),
	Double		UMETA(DisplayName = "Double", ToolTip="64 bits"),
	String		UMETA(DisplayName = "String",
					  ToolTip="32 bit header + 8 bits per UTF-8 character"),
	Vector		UMETA(DisplayName = "Vector", ToolTip="3 x double (64 bits) - 192 bits total"),
	Rotator		UMETA(DisplayName = "Rotator", ToolTip="3 x float (32 bits) - 96 bits total"),
	Transform	UMETA(DisplayName = "Transform",
					  ToolTip="3 x double (64 bits), 3 x float (32 bits), 3 x double - 480 bits total"),
	Custom		UMETA(DisplayName = "Custom",
				      ToolTip="Custom structure defined in UDPCustomStruct (Requires C++ project, see documentation)"),
};
