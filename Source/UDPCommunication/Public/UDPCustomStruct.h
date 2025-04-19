// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Serialization/Archive.h"
#include "UDPCustomStruct.generated.h"

/**
 *  This structure can be used to define your own custom structure to be sent over UDP in case...
 *     ...the built-in data types, sizes or alignment do not fit your needs.
 *     ...you are updating an existing project using the plugin before UE5.5
 *		  and need to keep the same structure.
 *
 *  1. Modify the structure in the two sections between comments below (like the example).
 *  2. Compile the plugin.
 *  3. Add a 'byte' field to the 'UDPPacketStructure' asset,
 *     with the same length as your custom structure.
 *  4. When receiving/sending using the created 'UDPPacketStructure',
 *	   connect the byte pin to the 'UDPCustomData' node to break/create the packet.
 *
 *	NOTE! Structures have to be supported by the engine. Modify at your own risk.
 */

USTRUCT(BlueprintType, DisplayName = "Custom UDP Structure")
struct FUDPCustomStruct {
	GENERATED_BODY()
	
	// ------------------------- CUSTOM STRUCTURE START -------------------------
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		uint8 ByteField = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		int64 Int64Field = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		double DoubleField = 0;
		
	// -------------------------- CUSTOM STRUCTURE END --------------------------
	
	// FUDPCustomStruct() {
	//
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FUDPCustomStruct &Structure)
{
	// ------------------------- CUSTOM STRUCTURE START -------------------------
	
	Ar << Structure.ByteField;
	Ar << Structure.Int64Field;
	Ar << Structure.DoubleField;
	
	// -------------------------- CUSTOM STRUCTURE END --------------------------

	return Ar;
}