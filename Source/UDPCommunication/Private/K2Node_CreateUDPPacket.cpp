// Fill out your copyright notice in the Description page of Project Settings.

#include "K2Node_CreateUDPPacket.h"

#define LOCTEXT_NAMESPACE "K2Node_UDPPacket"

// Input pin names
const FName UK2Node_CreateUDPPacket::PIN_PacketStructName(TEXT("PacketStructure"));
const FName UK2Node_CreateUDPPacket::PIN_OutputName(TEXT("UDPPacket"));

FText UK2Node_CreateUDPPacket::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("CreateUDPPacket_Title", "Create UDP Packet");
}

void UK2Node_CreateUDPPacket::AllocateDefaultPins()
{
    // Create execution pins
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
    
    // Create the PacketStructure input pin
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UUDPPacketStructure::StaticClass(), PIN_PacketStructName);

    // Create output pin (UDPPacket)
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FUDPPacket::StaticStruct(), PIN_OutputName);
}

void UK2Node_CreateUDPPacket::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    UE_LOG(LogTemp, Warning, TEXT("ExpandNode START for CreateUDPPacket"));

    Super::ExpandNode(CompilerContext, SourceGraph);

    // Find our pins
    UEdGraphPin* ExecPin = FindPinChecked(UEdGraphSchema_K2::PN_Execute);
    UEdGraphPin* ThenPin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
    UEdGraphPin* PacketStructPin = FindPinChecked(PIN_PacketStructName);
    UEdGraphPin* OutputPin = FindPinChecked(PIN_OutputName);

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
    
    // Create call to CreateUDPPacket
    UK2Node_CallFunction* CreateDataNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    CreateDataNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, CreateUDPPacket), UUDPPacketLibrary::StaticClass());
    CreateDataNode->AllocateDefaultPins();

    // Connect the execution flow start
    CompilerContext.MovePinLinksToIntermediate(*ExecPin, *CreateDataNode->GetExecPin());
    
    // Create call to InitWithStructure
    UK2Node_CallFunction* InitDefNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    InitDefNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, InitWithStructure), UUDPPacketLibrary::StaticClass());
    InitDefNode->AllocateDefaultPins();

    // Connect execution flow
    CreateDataNode->GetThenPin()->MakeLinkTo(InitDefNode->GetExecPin());

    // Connect create data output directly to init function input
    UEdGraphPin* PacketPin = InitDefNode->FindPin(TEXT("UDPPacket"));
    if (!PacketPin)
    {
        UE_LOG(LogTemp, Error, TEXT("UDPPacket pin not found in InitWithStructure function"));
        BreakAllNodeLinks();
        return;
    }
    CreateDataNode->GetReturnValuePin()->MakeLinkTo(PacketPin);

    // Connect packet structure to init function
    UEdGraphPin* StructurePin = InitDefNode->FindPin(TEXT("Structure"));
    if (!StructurePin)
    {
        UE_LOG(LogTemp, Error, TEXT("Packet structure pin not found in InitWithStructure function"));
        BreakAllNodeLinks();
        return;
    }
    CompilerContext.MovePinLinksToIntermediate(*PacketStructPin, *StructurePin);

    // Last execution node in the chain - we'll add to this for each field
    UEdGraphNode* LastNode = InitDefNode;

    // Process field values if we have a packet structure
    if (PacketStruct)
    {
        for (const auto& FieldPair : PacketStruct->Fields)
        {
            const FString& FieldName = FieldPair.Key;
            const FUDPField& Field = FieldPair.Value;
            
            // Find the input pin for this field
            UEdGraphPin* FieldPin = FindPin(FName(*FieldName));
            if (!FieldPin)
            {
                continue;
            }

            // Create a node to set this field value
            UK2Node_CallFunction* SetValueNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
            
            // Choose the appropriate "Set" function based on data type
            FName FunctionName;
            if (Field.IsArray && Field.DataType != EUDPDataType::String)
            {
                switch (Field.DataType)
                {
                case EUDPDataType::Float:
                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, SetFloatArray);
                    break;
                case EUDPDataType::Int:
                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, SetIntArray);
                    break;
                case EUDPDataType::Bool:
                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, SetBoolArray);
                    break;
                default:
                    continue;
                }
            }
            else
            {
                switch (Field.DataType)
                {
                case EUDPDataType::Float:
                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, SetFloat);
                    break;
                case EUDPDataType::Int:
                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, SetInt);
                    break;
                case EUDPDataType::Bool:
                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, SetBool);
                    break;
                case EUDPDataType::String:
                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, SetString);
                    break;
                case EUDPDataType::Custom:
                    FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPPacketLibrary, SetCustomStruct);
                    break;
                default:
                    continue;
                }
            }

            SetValueNode->FunctionReference.SetExternalMember(FunctionName, UUDPPacketLibrary::StaticClass());
            SetValueNode->AllocateDefaultPins();

            // Connect execution flow
            LastNode->FindPinChecked(UEdGraphSchema_K2::PN_Then)->MakeLinkTo(SetValueNode->GetExecPin());
            LastNode = SetValueNode;

            // Connect UDPPacket to the set function
            UEdGraphPin* SetDataPin = SetValueNode->FindPin(TEXT("UDPPacket"));
            if (SetDataPin)
            {
                // Connect from the output of CreateUDPPacket
                CreateDataNode->GetReturnValuePin()->MakeLinkTo(SetDataPin);
            }

            // Connect field name
            UEdGraphPin* FieldNamePin = SetValueNode->FindPin(TEXT("FieldName"));
            if (FieldNamePin)
            {
                FieldNamePin->DefaultValue = FieldName;
            }

            // Connect field value
            UEdGraphPin* ValuePin = SetValueNode->FindPin(TEXT("Value"));
            if (ValuePin)
            {
                CompilerContext.MovePinLinksToIntermediate(*FieldPin, *ValuePin);
                
                // If there's no connection, set the default value
                if (ValuePin->LinkedTo.Num() == 0 && !FieldPin->DefaultValue.IsEmpty())
                {
                    ValuePin->DefaultValue = FieldPin->DefaultValue;
                }
            }
        }
    }

    // Connect output directly from the packet data
    CompilerContext.MovePinLinksToIntermediate(*OutputPin, *CreateDataNode->GetReturnValuePin());

    // Connect the then pin to the last node in our chain
    CompilerContext.MovePinLinksToIntermediate(*ThenPin, *LastNode->FindPinChecked(UEdGraphSchema_K2::PN_Then));

    // Break all links at the end
    BreakAllNodeLinks();

    UE_LOG(LogTemp, Warning, TEXT("ExpandNode END for CreateUDPPacket"));
}

void UK2Node_CreateUDPPacket::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);
        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
        UE_LOG(LogTemp, Warning, TEXT("Registered CreateUDPPacket node in action database"));
    }
}

FText UK2Node_CreateUDPPacket::GetTooltipText() const
{
    return LOCTEXT("CreateUDPPacket_Tooltip", "Creates a UDP packet with the specified structure");
}

FText UK2Node_CreateUDPPacket::GetMenuCategory() const
{
    return FText::FromString("UDPCommunication");
}

FSlateIcon UK2Node_CreateUDPPacket::GetIconAndTint(FLinearColor& OutColor) const
{
    static FSlateIcon Icon("EditorStyle", "GraphEditor.MakeStruct_16x");
    OutColor = GetDefault<UGraphEditorSettings>()->ExecBranchNodeTitleColor;
    return Icon;
}

void UK2Node_CreateUDPPacket::PinConnectionListChanged(UEdGraphPin* Pin)
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

void UK2Node_CreateUDPPacket::PinDefaultValueChanged(UEdGraphPin* Pin)
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

void UK2Node_CreateUDPPacket::ReconstructNode()
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
            Pin->PinName != PIN_OutputName)
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

void UK2Node_CreateUDPPacket::CreateFieldPins(UUDPPacketStructure* PacketStruct)
{
    // Remove any existing field pins
    TArray<UEdGraphPin*> PinsToRemove;
    for (UEdGraphPin* ExistingPin : Pins)
    {
        // Exclude standard pins
        if (ExistingPin->PinName != UEdGraphSchema_K2::PN_Execute &&
            ExistingPin->PinName != UEdGraphSchema_K2::PN_Then &&
            ExistingPin->PinName != PIN_PacketStructName &&
            ExistingPin->PinName != PIN_OutputName)
        {
            PinsToRemove.Add(ExistingPin);
        }
    }

    // Remove the pins
    for (UEdGraphPin* PinToRemove : PinsToRemove)
    {
        RemovePin(PinToRemove);
    }

    // Skip if no packet structure
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
        case EUDPDataType::Float:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, FName(*FieldName));
            if (Field.IsArray)
                NewPin->PinType.ContainerType = EPinContainerType::Array;
            break;
        case EUDPDataType::Int:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, FName(*FieldName));
            if (Field.IsArray)
                NewPin->PinType.ContainerType = EPinContainerType::Array;
            break;
        case EUDPDataType::Bool:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, FName(*FieldName));
            if (Field.IsArray)
                NewPin->PinType.ContainerType = EPinContainerType::Array;
            break;
        case EUDPDataType::String:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, FName(*FieldName));
            break;
        case EUDPDataType::Custom:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FUDPCustomStruct::StaticStruct(), FName(*FieldName));
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

void UK2Node_CreateUDPPacket::PostLoad()
{
    // Call parent first
    Super::PostLoad();

    // Schedule a deferred action to mark the blueprint dirty
    // This will run after all assets are loaded when it's safe to access the blueprint
    if (GIsEditor && !GIsPlayInEditorWorld)
    {
        // Use a lambda that captures a weak pointer to this node
        TWeakObjectPtr<UK2Node_CreateUDPPacket> WeakThis(this);
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

void UK2Node_CreateUDPPacket::Serialize(FArchive& Ar)
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

void UK2Node_CreateUDPPacket::CreatePreservedFieldPins(const TSet<FName>& PinNames, const TMap<FName, TArray<UEdGraphPin*>>& PinConnections)
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
                // Create pin with matching type
                NewPin = CreatePin(EGPD_Input, ConnectedPin->PinType.PinCategory, PinName);
            }
        }
        
        // If we couldn't determine type from connections, default to float
        if (!NewPin)
        {
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, PinName);
        }
        
        if (NewPin)
        {
            NewPin->PinFriendlyName = FText::FromName(PinName);
        }
    }
}

#undef LOCTEXT_NAMESPACE