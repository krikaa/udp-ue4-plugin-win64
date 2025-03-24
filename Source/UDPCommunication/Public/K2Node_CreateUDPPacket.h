// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "UDPPacketStructure.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UDPPacketLibrary.h"
#include "K2Node_CreateUDPPacket.generated.h"

/**
 * 
 */
UCLASS()
class UDPCOMMUNICATION_API UK2Node_CreateUDPPacket : public UK2Node
{
	GENERATED_BODY()
public:

	// Pin name constants
	static const FName PIN_SenderName;
	static const FName PIN_PacketStructName;
	static const FName PIN_OutputName;

	void CreateFieldPins(UUDPPacketStructure* PacketStruct);

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void ReconstructNode() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetMenuCategory() const override;
	// End of UK2Node interface
};
