// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "UDPPacketDefinition.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "UDPSender.h"
#include "UDPDynamicDataLibrary.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_UDPPacket.generated.h"

/**
 * 
 */
UCLASS()
class UDPCOMMUNICATION_API UK2Node_CreateUDPPacket : public UK2Node
{
	GENERATED_BODY()
public:
	// Remove the property since we're using a pin instead
	// UPROPERTY(EditAnywhere, Category = "UDPCommunication")
	// UUDPPacketDefinition* PacketDefinition;

	// Pin name constants
	static const FName PIN_SenderName;
	static const FName PIN_PacketDefName;
	static const FName PIN_OutputName;

	void CreateFieldPins(UUDPPacketDefinition* PacketDef);

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
