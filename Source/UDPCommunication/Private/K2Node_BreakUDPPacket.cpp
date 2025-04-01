// Fill out your copyright notice in the Description page of Project Settings.

#include "K2Node_BreakUDPPacket.h"


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
	UE_LOG(LogTemp, Warning, TEXT("ExpandNode START for BreakUDPPacket"));

    Super::ExpandNode(CompilerContext, SourceGraph);

    // Find our pins
    UEdGraphPin* ExecPin = FindPinChecked(UEdGraphSchema_K2::PN_Execute);
    UEdGraphPin* ThenPin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
    UEdGraphPin* PacketStructPin = FindPinChecked(PIN_PacketStructName);
    UEdGraphPin* UDPPacketPin = FindPinChecked(PIN_UDPPacketName);

    // No packet structure provided - log error if not connected
    if (PacketStructPin->LinkedTo.Num() == 0 && !PacketStructPin->DefaultObject)
    {
        CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "MissingPacketStructure", "No packet structure definition connected to @@").ToString(), this);
        BreakAllNodeLinks();
        return;
    }

    // Get the packet structure to find field pins
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

    // // Debug print node
    // UK2Node_CallFunction* DebugNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    // DebugNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, PrintString), UKismetSystemLibrary::StaticClass());
    // DebugNode->AllocateDefaultPins();
    // DebugNode->FindPinChecked(TEXT("InString"))->DefaultValue = TEXT("UDP Packet Node Executed");
    // DebugNode->FindPinChecked(TEXT("bPrintToScreen"))->DefaultValue = TEXT("true");
    // DebugNode->FindPinChecked(TEXT("bPrintToLog"))->DefaultValue = TEXT("true");

	// Last execution node in the chain
	UEdGraphNode* LastNode = nullptr;
    
	// Set up the start of the execution chain
	UK2Node_CallFunction* ValidateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	ValidateNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, ValidateWithStructure), UUDPPacketLibrary::StaticClass());
	ValidateNode->AllocateDefaultPins();
	
    // Connect the execution flow start
    CompilerContext.MovePinLinksToIntermediate(*ExecPin, *ValidateNode->GetExecPin());

	// Connect packet pin to validate function
	UEdGraphPin* ValidatePacketPin = ValidateNode->FindPinChecked(TEXT("UDPPacket"));
	CompilerContext.MovePinLinksToIntermediate(*UDPPacketPin, *ValidatePacketPin);
    
	// Connect structure pin to validate function
	UEdGraphPin* ValidateStructurePin = ValidateNode->FindPinChecked(TEXT("Structure"));
	CompilerContext.MovePinLinksToIntermediate(*PacketStructPin, *ValidateStructurePin);
    
	// Set last node to the validate node
	LastNode = ValidateNode;

    // Process field values if we have a packet structure
    if (PacketStruct)
    {
        for (const FUDPField& Field : PacketStruct->Fields)
        {
            // Find the input pin for this field
            UEdGraphPin* FieldPin = FindPin(FName(*Field.Name));
            if (!FieldPin)
            {
                continue;
            }

            // Create a node to set this field value
            UK2Node_CallFunction* GetValueNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
            
            // Choose the appropriate "Get" function based on data type
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

            GetValueNode->FunctionReference.SetExternalMember(FunctionName, UUDPPacketLibrary::StaticClass());
            GetValueNode->AllocateDefaultPins();

        	// Connect UDPPacket to the get function
        	UEdGraphPin* GetPacketPin = GetValueNode->FindPin(TEXT("UDPPacket"));
        	if (GetPacketPin)
        	{
        		UEdGraphPin* ValidPacketOut = ValidateNode->FindPin(TEXT("ValidatedPacket"));
        		if (ValidPacketOut)
        		{
        			ValidPacketOut->MakeLinkTo(GetPacketPin);
        		}
        	}

            // Connect field name
            UEdGraphPin* FieldNamePin = GetValueNode->FindPin(TEXT("FieldName"));
            if (FieldNamePin)
            {
                FieldNamePin->DefaultValue = Field.Name;
            }

        	// Connect the output value to our field pin
        	UEdGraphPin* ReturnPin = GetValueNode->GetReturnValuePin();
        	if (ReturnPin)
        	{
        		CompilerContext.MovePinLinksToIntermediate(*FieldPin, *ReturnPin);
        	}
        }
    }

	// Connect the then pin to the validate node
	CompilerContext.MovePinLinksToIntermediate(*ThenPin, *ValidateNode->FindPinChecked(UEdGraphSchema_K2::PN_Then));

	// Break all links at the end
	BreakAllNodeLinks();

    UE_LOG(LogTemp, Warning, TEXT("ExpandNode END for CreateUDPPacket"));
}

#undef LOCTEXT_NAMESPACE

