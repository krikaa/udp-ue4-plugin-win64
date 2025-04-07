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
#include "GraphEditorSettings.h"
#include "Containers/Ticker.h"
#include "K2Node_BreakUDPPacket.generated.h"

/**
 * 
 */
UCLASS()
class UDPCOMMUNICATION_API UK2Node_BreakUDPPacket : public UK2Node
{
	GENERATED_BODY()

public:
	
	// Pin name constants
	static const FName PIN_PacketStructName;
	static const FName PIN_UDPPacketName;
	
	// Helper functions 
	void CreateFieldPins(UUDPPacketStructure* PacketStruct);

	// Node appearance overrides
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetMenuCategory() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	
	// Node implementation overrides
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void AllocateDefaultPins() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void ReconstructNode() override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

private:
	// Serialized reference to the packet structure asset
	UPROPERTY()
	UUDPPacketStructure* CachedPacketStructure;

	// Override PostLoad to ensure pins are recreated correctly after deserialization
	virtual void PostLoad() override;

	// Override Serialize to save/load our cached data
	virtual void Serialize(FArchive& Ar) override;

	// Helper function to recreate pins based on previously existing pins
	void CreatePreservedFieldPins(const TSet<FName>& PinNames, const TMap<FName, TArray<UEdGraphPin*>>& PinConnections);
};
