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
	for (const auto& FieldPair : PacketStruct->Fields)
	{
		const FString& FieldName = FieldPair.Key;
		const FUDPField& Field = FieldPair.Value;
		
		UEdGraphPin* NewPin = nullptr;
        
		switch (Field.DataType)
		{
			case EUDPDataType::Bool:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, FName(*FieldName));
	            if (Field.IsArray)
	                NewPin->PinType.ContainerType = EPinContainerType::Array;
	            break;
	        case EUDPDataType::Byte:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Byte, FName(*FieldName));
	            break;
	        case EUDPDataType::Int:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Int, FName(*FieldName));
	            if (Field.IsArray)
	                NewPin->PinType.ContainerType = EPinContainerType::Array;
	            break;
	        case EUDPDataType::Int64:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Int64, FName(*FieldName));
	            break;
	        case EUDPDataType::Float:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Real, UEdGraphSchema_K2::PC_Float, FName(*FieldName));
	            if (Field.IsArray)
	                NewPin->PinType.ContainerType = EPinContainerType::Array;
	            break;
	        case EUDPDataType::Double:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Real, UEdGraphSchema_K2::PC_Double, FName(*FieldName));
	            break;
	        case EUDPDataType::String:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_String, FName(*FieldName));
	            break;
	        case EUDPDataType::Vector:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, TBaseStructure<FVector>::Get(), FName(*FieldName));
	            break;
	        case EUDPDataType::Rotator:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, TBaseStructure<FRotator>::Get(), FName(*FieldName));
	            break;
	        case EUDPDataType::Transform:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, TBaseStructure<FTransform>::Get(), FName(*FieldName));
	            break;
	        case EUDPDataType::Custom:
	            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FUDPCustomStruct::StaticStruct(), FName(*FieldName));
	            break;
		}

		if (NewPin)
		{
			NewPin->PinFriendlyName = FText::FromString(FieldName);
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

    // Store existing pin info
    TMap<FName, TArray<UEdGraphPin*>> PinConnections;
    TMap<FName, FString> PinDefaultValues;
    TMap<FName, UObject*> PinDefaultObjects;
    TSet<FName> ExistingFieldPins;

    // Remember all pin states
    for (UEdGraphPin* Pin : Pins)
    {
        // Identify field pins
        if (Pin->PinName != UEdGraphSchema_K2::PN_Execute &&
            Pin->PinName != UEdGraphSchema_K2::PN_Then &&
            Pin->PinName != PIN_PacketStructName &&
            Pin->PinName != PIN_UDPPacketName)
        {
            ExistingFieldPins.Add(Pin->PinName);
        }

        // Store connections
        if (Pin->LinkedTo.Num() > 0)
        {
            PinConnections.Add(Pin->PinName, Pin->LinkedTo);
        }

        // Store default values
        if (Pin->LinkedTo.Num() == 0)
        {
            if (!Pin->DefaultValue.IsEmpty())
            {
                PinDefaultValues.Add(Pin->PinName, Pin->DefaultValue);
            }

            if (Pin->DefaultObject != nullptr)
            {
                PinDefaultObjects.Add(Pin->PinName, Pin->DefaultObject);
            }
        }
    }

    // Get packet structure
    if (PacketStructPin)
    {
        // Get from connections
        if (PacketStructPin->LinkedTo.Num() > 0)
        {
            UEdGraphPin* LinkedPin = PacketStructPin->LinkedTo[0];
            if (LinkedPin && LinkedPin->DefaultObject)
            {
                PacketStruct = Cast<UUDPPacketStructure>(LinkedPin->DefaultObject);
            }
        }
        // Get from default object
        else if (PacketStructPin->DefaultObject)
        {
            PacketStruct = Cast<UUDPPacketStructure>(PacketStructPin->DefaultObject);
        }
    }

    // Use cached structure as fallback
    if (!PacketStruct && CachedPacketStructure)
    {
        PacketStruct = CachedPacketStructure;
    }

    // Handle special case: If packet structure isn't fully loaded, delay reconstruction
    if (PacketStruct && PacketStruct->HasAnyFlags(RF_NeedLoad | RF_NeedPostLoad))
    {
        // Schedule reconstruction for next tick when asset is fully loaded
        FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateLambda([this](float DeltaTime) {
                ReconstructNode();
                return false; // One-shot ticker
            }),
            0.1f
        );
        
        // Update cache and exit without reconstructing
        if (PacketStruct)
        {
            CachedPacketStructure = PacketStruct;
        }
        return;
    }

    // Update the cached structure
    if (PacketStruct)
    {
        CachedPacketStructure = PacketStruct;
    }

    // Rebuild node structure
    Super::ReconstructNode();

    // Create pins based on what's available
    if (PacketStruct)
    {
        CreateFieldPins(PacketStruct);
    }
    else if (ExistingFieldPins.Num() > 0)
    {
        CreatePreservedFieldPins(ExistingFieldPins, PinConnections);
    }

    // Restore connections and values
    for (UEdGraphPin* Pin : Pins)
    {
        // Restore connections
        if (PinConnections.Contains(Pin->PinName))
        {
            for (UEdGraphPin* ConnectedPin : PinConnections[Pin->PinName])
            {
                if (ConnectedPin && !ConnectedPin->IsPendingKill())
                {
                    Pin->MakeLinkTo(ConnectedPin);
                }
            }
        }

        // Restore default values for unconnected pins
        if (Pin->LinkedTo.Num() == 0)
        {
            if (PinDefaultValues.Contains(Pin->PinName))
            {
                Pin->DefaultValue = PinDefaultValues[Pin->PinName];
            }

            if (PinDefaultObjects.Contains(Pin->PinName))
            {
                Pin->DefaultObject = PinDefaultObjects[Pin->PinName];
            }
        }
    }

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
	for (const auto& FieldPair : PacketStruct->Fields)
	{
		const FString& FieldName = FieldPair.Key;
		UEdGraphPin* FieldPin = FindPin(FName(*FieldName));
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
    	for (const auto& FieldPair : PacketStruct->Fields)
        {
    		const FString& FieldName = FieldPair.Key;
    		const FUDPField& Field = FieldPair.Value;
    		
            UEdGraphPin* FieldPin = FindPin(FName(*FieldName));
            if (!FieldPin || FieldPin->LinkedTo.Num() == 0)
            {
                continue;
            }

            UK2Node_CallFunction* GetterNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
            
            FName FunctionName;
        	if (Field.IsArray && Field.DataType != EUDPDataType::String)
        	{
        		switch (Field.DataType)
        		{
        		case EUDPDataType::Float:
        			FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetFloatArray);
        			break;
        		case EUDPDataType::Int:
        			FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetIntArray);
        			break;
        		case EUDPDataType::Bool:
        			FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetBoolArray);
        			break;
        		default:
        			continue;
        		}
        	}
        	else
        	{
        		switch (Field.DataType)
        		{
        			case EUDPDataType::Bool:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetBool);
	                    break;
	                case EUDPDataType::Byte:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetByte);
	                    break;
	                case EUDPDataType::Int:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetInt);
	                    break;
	                case EUDPDataType::Int64:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetInt64);
	                    break;
	                case EUDPDataType::Float:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetFloat);
	                    break;
	                case EUDPDataType::Double:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetDouble);
	                    break;
	                case EUDPDataType::String:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetString);
	                    break;
	                case EUDPDataType::Vector:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetVector);
	                    break;
	                case EUDPDataType::Rotator:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetRotator);
	                    break;
	                case EUDPDataType::Transform:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetTransform);
	                    break;
	                case EUDPDataType::Custom:
	                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, GetCustomStruct);
	                    break;
        			default:
        				continue;
        		}
        	}

            GetterNode->FunctionReference.SetExternalMember(FunctionName, UUDPPacketLibrary::StaticClass());
            GetterNode->AllocateDefaultPins();

            // Connect UDP packet input
            CompilerContext.CopyPinLinksToIntermediate(*ValidateNode->FindPinChecked(TEXT("UDPPacket")), 
                *GetterNode->FindPinChecked(TEXT("UDPPacket")));

            // Set field name
            GetterNode->FindPinChecked(TEXT("FieldName"))->DefaultValue = FieldName;
            
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

void UK2Node_BreakUDPPacket::CreatePreservedFieldPins(const TSet<FName>& PinNames, const TMap<FName, TArray<UEdGraphPin*>>& PinConnections)
{
    for (const FName& PinName : PinNames)
    {
        UEdGraphPin* NewPin = nullptr;
        
        // Try to determine the pin type based on connections
        if (PinConnections.Contains(PinName) && PinConnections[PinName].Num() > 0)
        {
            UEdGraphPin* ConnectedPin = PinConnections[PinName][0];
            if (ConnectedPin)
            {
                // Create pin with matching type - Output for Break node
                NewPin = CreatePin(EGPD_Output, ConnectedPin->PinType.PinCategory, PinName);
            }
        }
        
        // If we couldn't determine type from connections, default to float
        if (!NewPin)
        {
            NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Real,  PinName);
        }
        
        if (NewPin)
        {
            NewPin->PinFriendlyName = FText::FromName(PinName);
        }
    }
}

void UK2Node_BreakUDPPacket::PostLoad()
{
	// Call parent first
	Super::PostLoad();

	// Schedule a deferred action to mark the blueprint dirty
	// This will run after all assets are loaded when it's safe to access the blueprint
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		// Use a lambda that captures a weak pointer to this node
		TWeakObjectPtr<UK2Node_BreakUDPPacket> WeakThis(this);
		FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([WeakThis](float DeltaTime) {
			if (WeakThis.IsValid())
			{
				UEdGraph* Graph = WeakThis->GetGraph();
				if (Graph)
				{
					// Reconstruct node after everything is loaded
					WeakThis->ReconstructNode();
                        
					// Force trigger node updates
					Graph->NotifyGraphChanged();
                        
					UBlueprint* Blueprint = Graph->GetTypedOuter<UBlueprint>();
					if (Blueprint)
					{
						// Request a compile without marking dirty
						FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::None, nullptr);
					}
				}
			}
			return false; // One-shot ticker
		}),
		0.5f // Half-second delay to ensure everything is loaded
		);
	}

	// Request node reconstruction
	GetGraph()->NotifyNodeChanged(this);
}

void UK2Node_BreakUDPPacket::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
    
	// Before saving, store the current structure reference
	if (Ar.IsSaving())
	{
		UEdGraphPin* PacketStructPin = FindPin(PIN_PacketStructName);
		CachedPacketStructure = nullptr;
        
		if (PacketStructPin)
		{
			if (PacketStructPin->LinkedTo.Num() > 0 && PacketStructPin->LinkedTo[0]->DefaultObject)
			{
				CachedPacketStructure = Cast<UUDPPacketStructure>(PacketStructPin->LinkedTo[0]->DefaultObject);
			}
			else if (PacketStructPin->DefaultObject)
			{
				CachedPacketStructure = Cast<UUDPPacketStructure>(PacketStructPin->DefaultObject);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

