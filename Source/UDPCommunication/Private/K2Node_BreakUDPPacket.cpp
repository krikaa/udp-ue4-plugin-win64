// Fill out your copyright notice in the Description page of Project Settings.

#include "K2Node_BreakUDPPacket.h"

#include "K2Node_ExecutionSequence.h"


#define LOCTEXT_NAMESPACE "K2Node_BreakUDPPacket"

// Input pin names
const FName UK2Node_BreakUDPPacket::PIN_PacketStructName(TEXT("PacketStructure"));
const FName UK2Node_BreakUDPPacket::PIN_UDPPacketName(TEXT("UDPPacket"));

void UK2Node_BreakUDPPacket::CreateFieldPins(UUDPPacketStructure* PacketStruct)
{
	// Remove any existing field pins
	TArray<UEdGraphPin*> PinsToRemove;
	for (UEdGraphPin* ExistingPin : Pins)
	{
		// Exclude standard pins
		if (ExistingPin->PinName != UEdGraphSchema_K2::PN_Execute &&
			ExistingPin->PinName != UEdGraphSchema_K2::PN_Then &&
			ExistingPin->PinName != PIN_PacketStructName &&
			ExistingPin->PinName != PIN_UDPPacketName)
		{
			PinsToRemove.Add(ExistingPin);
		}
	}

	// Remove the pins
	for (UEdGraphPin* PinToRemove : PinsToRemove)
	{
		RemovePin(PinToRemove);
	}

	// Skip if no packet structure given
	if (!PacketStruct)
	{
		return;
	}

	// Make sure packet structure is compiled
	PacketStruct->CompileStructure();

	// Create new pins based on the packet structure fields
	for (const FUDPField& Field : PacketStruct->Fields)
	{
		UEdGraphPin* NewPin = nullptr;
        
		switch (Field.DataType)
		{
		case EUDPDataType::Float:
			NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Real, FName(*Field.Name));
			break;
		case EUDPDataType::Int:
			NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Int, FName(*Field.Name));
			break;
		case EUDPDataType::Bool:
			NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, FName(*Field.Name));
			break;
		case EUDPDataType::String:
			NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_String, FName(*Field.Name));
			break;
		}

		if (NewPin)
		{
			NewPin->PinFriendlyName = FText::FromString(Field.Name);
		}
	}

	// Notify the graph that the node has changed
	if (UEdGraph* Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

FText UK2Node_BreakUDPPacket::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("BreakUDPPacket_Title", "Break UDP Packet");
}

FText UK2Node_BreakUDPPacket::GetTooltipText() const
{
	return LOCTEXT("BreakUDPPacket_Tooltip", "Breaks the UDP packet with the specified structure");
}

FText UK2Node_BreakUDPPacket::GetMenuCategory() const
{
	return FText::FromString("UDPCommunication");
}

FSlateIcon UK2Node_BreakUDPPacket::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.BreakStruct_16x");
	OutColor = GetDefault<UGraphEditorSettings>()->ExecBranchNodeTitleColor;
	return Icon;
}

void UK2Node_BreakUDPPacket::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
		UE_LOG(LogTemp, Warning, TEXT("Registered BreakUDPPacket node in action database"));
	}
}

void UK2Node_BreakUDPPacket::AllocateDefaultPins()
{
	// Create execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
    
	// Create the PacketStructure input pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UUDPPacketStructure::StaticClass(), PIN_PacketStructName);

	// Create the UDP packet input pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FUDPPacket::StaticStruct(), PIN_UDPPacketName);
}

void UK2Node_BreakUDPPacket::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && Pin->PinName == PIN_PacketStructName)
	{
		// Check if we have a packet structure connected
		if (Pin->LinkedTo.Num() > 0)
		{
			UEdGraphPin* LinkedPin = Pin->LinkedTo[0];
			if (LinkedPin)
			{
				UUDPPacketStructure* PacketStruct = Cast<UUDPPacketStructure>(LinkedPin->DefaultObject);
				if (PacketStruct)
				{
					CreateFieldPins(PacketStruct);
				}
			}
		}
	}
}

void UK2Node_BreakUDPPacket::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	if (Pin && Pin->PinName == PIN_PacketStructName)
	{
		// Check if we have a packet structure set via default value
		if (Pin->DefaultObject != nullptr && Pin->DefaultObject->IsA(UUDPPacketStructure::StaticClass()))
		{
			UUDPPacketStructure* PacketStruct = Cast<UUDPPacketStructure>(Pin->DefaultObject);
			if (PacketStruct)
			{
				CreateFieldPins(PacketStruct);
			}
		}
	}
}

void UK2Node_BreakUDPPacket::ReconstructNode()
{
	// Store current packet structure
	UEdGraphPin* PacketStructPin = FindPin(PIN_PacketStructName);
	UUDPPacketStructure* PacketStruct = nullptr;
    
	if (PacketStructPin)
	{
		// Check linked pins first
		if (PacketStructPin->LinkedTo.Num() > 0)
		{
			UEdGraphPin* LinkedPin = PacketStructPin->LinkedTo[0];
			if (LinkedPin && LinkedPin->DefaultObject)
			{
				PacketStruct = Cast<UUDPPacketStructure>(LinkedPin->DefaultObject);
			}
		}
		// Otherwise check default object
		else if (PacketStructPin->DefaultObject)
		{
			PacketStruct = Cast<UUDPPacketStructure>(PacketStructPin->DefaultObject);
		}
	}
    
	// Call parent implementation to clear pins
	Super::ReconstructNode();
    
	// Recreate pins based on stored packet structure
	if (PacketStruct)
	{
		CreateFieldPins(PacketStruct);
	}
    
	// Make sure the graph knows about the changes
	GetGraph()->NotifyGraphChanged();
}

void UK2Node_BreakUDPPacket::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    // Find all pins
    UEdGraphPin* ExecPin = FindPinChecked(UEdGraphSchema_K2::PN_Execute);
    UEdGraphPin* ThenPin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
    UEdGraphPin* PacketStructPin = FindPinChecked(PIN_PacketStructName);
    UEdGraphPin* UDPPacketPin = FindPinChecked(PIN_UDPPacketName);

    // Get the packet structure
    UUDPPacketStructure* PacketStruct = nullptr;
    if (PacketStructPin->LinkedTo.Num() > 0)
    {
        UEdGraphPin* LinkedPin = PacketStructPin->LinkedTo[0];
        if (LinkedPin && LinkedPin->DefaultObject)
        {
            PacketStruct = Cast<UUDPPacketStructure>(LinkedPin->DefaultObject);
        }
    }
    else if (PacketStructPin->DefaultObject)
    {
        PacketStruct = Cast<UUDPPacketStructure>(PacketStructPin->DefaultObject);
    }

    // No packet structure - exit early
    if (!PacketStruct)
    {
        CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "MissingPacketStructure", 
            "No packet structure definition connected to @@").ToString(), this);
        return;
    }

    // Create the validate node
    UK2Node_CallFunction* ValidateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    ValidateNode->FunctionReference.SetExternalMember(
        GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, ValidateWithStructure),
        UUDPPacketLibrary::StaticClass());
    ValidateNode->AllocateDefaultPins();

    // Connect execution flow
    CompilerContext.MovePinLinksToIntermediate(*ExecPin, *ValidateNode->GetExecPin());
    
    // Connect UDPPacket input
    CompilerContext.MovePinLinksToIntermediate(*UDPPacketPin, *ValidateNode->FindPinChecked(TEXT("UDPPacket")));

    // Handle packet structure
    UEdGraphPin* ValidateStructurePin = ValidateNode->FindPinChecked(TEXT("Structure"));
    if (PacketStructPin->LinkedTo.Num() > 0)
    {
        CompilerContext.MovePinLinksToIntermediate(*PacketStructPin, *ValidateStructurePin);
    }
    else if (PacketStructPin->DefaultObject)
    {
        ValidateStructurePin->DefaultObject = PacketStructPin->DefaultObject;
    }

    // Count connected field pins
    TArray<UEdGraphPin*> ConnectedFieldPins;
    for (const FUDPField& Field : PacketStruct->Fields)
    {
        UEdGraphPin* FieldPin = FindPin(FName(*Field.Name));
        if (FieldPin && FieldPin->LinkedTo.Num() > 0)
        {
            ConnectedFieldPins.Add(FieldPin);
        }
    }

    // If we have connected field pins, we need to set up a sequence
    if (ConnectedFieldPins.Num() > 0)
    {
        // Create pure function nodes for all getters
        TArray<UK2Node_CallFunction*> GetterNodes;
        TMap<UEdGraphPin*, UK2Node_CallFunction*> FieldPinToGetterMap;

        // First create all the pure getter nodes
        for (const FUDPField& Field : PacketStruct->Fields)
        {
            UEdGraphPin* FieldPin = FindPin(FName(*Field.Name));
            if (!FieldPin || FieldPin->LinkedTo.Num() == 0)
            {
                continue;
            }

            UK2Node_CallFunction* GetterNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
            
            FName FunctionName;
            switch (Field.DataType)
            {
            case EUDPDataType::Float:
                FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetFloat);
                break;
            case EUDPDataType::Int:
                FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetInt);
                break;
            case EUDPDataType::Bool:
                FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetBool);
                break;
            case EUDPDataType::String:
                FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetString);
                break;
            default:
                continue;
            }

            GetterNode->FunctionReference.SetExternalMember(FunctionName, UUDPPacketLibrary::StaticClass());
            GetterNode->AllocateDefaultPins();

            // Connect UDP packet input
            CompilerContext.CopyPinLinksToIntermediate(*ValidateNode->FindPinChecked(TEXT("UDPPacket")), 
                *GetterNode->FindPinChecked(TEXT("UDPPacket")));

            // Set field name
            GetterNode->FindPinChecked(TEXT("FieldName"))->DefaultValue = Field.Name;
            
            // Connect output value
            CompilerContext.MovePinLinksToIntermediate(*FieldPin, *GetterNode->GetReturnValuePin());
            
            // Store for execution flow
            GetterNodes.Add(GetterNode);
            FieldPinToGetterMap.Add(FieldPin, GetterNode);
        }

        // If we have one getter, connect execution directly
        if (GetterNodes.Num() == 1)
        {
            // Connect validation to single getter
            ValidateNode->GetThenPin()->MakeLinkTo(GetterNodes[0]->GetExecPin());
            
            // Connect then pin to output
            CompilerContext.MovePinLinksToIntermediate(*ThenPin, *GetterNodes[0]->GetThenPin());
        }
        // If we have multiple getters, use a sequence node
        else if (GetterNodes.Num() > 1)
        {
            UK2Node_ExecutionSequence* SequenceNode = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
            SequenceNode->AllocateDefaultPins();
            
            // Number of sequence outputs we need = number of getters + 1 for final Then
            int32 NumOutputs = GetterNodes.Num() + 1;
            
            // Expand sequence node to have enough outputs
            // IMPORTANT: The sequence node automatically starts with 2 outputs
            for (int32 i = 2; i < NumOutputs; i++)
            {
                SequenceNode->AddInputPin();
            }
            
            // Connect validate to sequence
            ValidateNode->GetThenPin()->MakeLinkTo(SequenceNode->GetExecPin());
            
            // Connect all getters to sequence
            for (int32 i = 0; i < GetterNodes.Num(); i++)
            {
                UEdGraphPin* SequencePin = SequenceNode->GetThenPinGivenIndex(i);
                if (ensure(SequencePin && GetterNodes[i]->GetExecPin()))
                {
                    SequencePin->MakeLinkTo(GetterNodes[i]->GetExecPin());
                }
            }
            
            // Connect final sequence output to then pin
            UEdGraphPin* FinalSequencePin = SequenceNode->GetThenPinGivenIndex(GetterNodes.Num());
            CompilerContext.MovePinLinksToIntermediate(*ThenPin, *FinalSequencePin);
        }
    }
    else
    {
        // No connected field pins, just pass execution through
        CompilerContext.MovePinLinksToIntermediate(*ThenPin, *ValidateNode->GetThenPin());
    }

    // Clean up
    BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE

