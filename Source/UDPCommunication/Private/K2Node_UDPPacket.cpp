// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_UDPPacket.h"

#define LOCTEXT_NAMESPACE "K2Node_UDPPacket"

// Input pin names
const FName UK2Node_CreateUDPPacket::PIN_PacketDefName(TEXT("PacketDefinition"));
const FName UK2Node_CreateUDPPacket::PIN_OutputName(TEXT("DynamicData"));

FText UK2Node_CreateUDPPacket::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("CreateUDPPacket_Title", "Create UDP Packet");
}

void UK2Node_CreateUDPPacket::AllocateDefaultPins()
{
    // Create execution pins
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
    
    // Create the PacketDefinition input pin
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UUDPPacketDefinition::StaticClass(), PIN_PacketDefName);

    // Create output pin (UDPDynamicData)
    UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, nullptr, PIN_OutputName);
    OutputPin->PinType.PinSubCategoryObject = FUDPDynamicData::StaticStruct();
}

void UK2Node_CreateUDPPacket::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    UE_LOG(LogTemp, Warning, TEXT("ExpandNode START for CreateUDPPacket"));

    Super::ExpandNode(CompilerContext, SourceGraph);

    // Find our pins
    UEdGraphPin* ExecPin = FindPinChecked(UEdGraphSchema_K2::PN_Execute);
    UEdGraphPin* ThenPin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
    UEdGraphPin* PacketDefPin = FindPinChecked(PIN_PacketDefName);
    UEdGraphPin* OutputPin = FindPinChecked(PIN_OutputName);

    // No packet definition provided - log error if not connected
    if (PacketDefPin->LinkedTo.Num() == 0 && !PacketDefPin->DefaultObject)
    {
        CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "MissingPacketDefinition", "No packet definition connected to @@").ToString(), this);
        BreakAllNodeLinks();
        return;
    }

    // Get the packet definition to find field pins
    UUDPPacketDefinition* PacketDef = nullptr;
    if (PacketDefPin->LinkedTo.Num() > 0)
    {
        UEdGraphPin* LinkedPin = PacketDefPin->LinkedTo[0];
        if (LinkedPin && LinkedPin->DefaultObject)
        {
            PacketDef = Cast<UUDPPacketDefinition>(LinkedPin->DefaultObject);
        }
    }
    else if (PacketDefPin->DefaultObject)
    {
        PacketDef = Cast<UUDPPacketDefinition>(PacketDefPin->DefaultObject);
    }

    // Debug print node
    UK2Node_CallFunction* DebugNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    DebugNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, PrintString), UKismetSystemLibrary::StaticClass());
    DebugNode->AllocateDefaultPins();
    DebugNode->FindPinChecked(TEXT("InString"))->DefaultValue = TEXT("UDP Packet Node Executed");
    DebugNode->FindPinChecked(TEXT("bPrintToScreen"))->DefaultValue = TEXT("true");
    DebugNode->FindPinChecked(TEXT("bPrintToLog"))->DefaultValue = TEXT("true");

    // Connect the execution flow start
    CompilerContext.MovePinLinksToIntermediate(*ExecPin, *DebugNode->GetExecPin());

    UK2Node_CallFunction* CreateDataNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    CreateDataNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, CreateDynamicData), UUDPDynamicDataLibrary::StaticClass());
    CreateDataNode->AllocateDefaultPins();

    // Connect debug node to create data node
    DebugNode->GetThenPin()->MakeLinkTo(CreateDataNode->GetExecPin());
    
    // Create call to InitWithDefinition
    UK2Node_CallFunction* InitDefNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    InitDefNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, InitWithDefinition), UUDPDynamicDataLibrary::StaticClass());
    InitDefNode->AllocateDefaultPins();

    // Connect execution flow
    CreateDataNode->GetThenPin()->MakeLinkTo(InitDefNode->GetExecPin());

    // Connect create data output directly to init function input
    UEdGraphPin* DynamicDataPin = InitDefNode->FindPin(TEXT("DynamicData"));
    if (!DynamicDataPin)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicData pin not found in InitWithDefinition function"));
        BreakAllNodeLinks();
        return;
    }
    CreateDataNode->GetReturnValuePin()->MakeLinkTo(DynamicDataPin);

    // Connect packet definition to init function
    UEdGraphPin* DefinitionPin = InitDefNode->FindPin(TEXT("Definition"));
    if (!DefinitionPin)
    {
        UE_LOG(LogTemp, Error, TEXT("Definition pin not found in InitWithDefinition function"));
        BreakAllNodeLinks();
        return;
    }
    CompilerContext.MovePinLinksToIntermediate(*PacketDefPin, *DefinitionPin);

    // Last execution node in the chain - we'll add to this for each field
    UEdGraphNode* LastNode = InitDefNode;

    // Process field values if we have a packet definition
    if (PacketDef)
    {
        for (const FUDPFieldDefinition& Field : PacketDef->Fields)
        {
            // Find the input pin for this field
            UEdGraphPin* FieldPin = FindPin(FName(*Field.Name));
            if (!FieldPin)
            {
                continue;
            }

            // Create a node to set this field value
            UK2Node_CallFunction* SetValueNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
            
            // Choose the appropriate "Set" function based on data type
            FName FunctionName;
            switch (Field.DataType)
            {
            case EUDPDataType::Float:
                FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, SetFloat);
                break;
            case EUDPDataType::Int:
                FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, SetInt);
                break;
            case EUDPDataType::Bool:
                FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, SetBool);
                break;
            case EUDPDataType::String:
                FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, SetString);
                break;
            default:
                continue;
            }

            SetValueNode->FunctionReference.SetExternalMember(FunctionName, UUDPDynamicDataLibrary::StaticClass());
            SetValueNode->AllocateDefaultPins();

            // Connect execution flow
            LastNode->FindPinChecked(UEdGraphSchema_K2::PN_Then)->MakeLinkTo(SetValueNode->GetExecPin());
            LastNode = SetValueNode;

            // Connect DynamicData to the set function
            UEdGraphPin* SetDataPin = SetValueNode->FindPin(TEXT("DynamicData"));
            if (SetDataPin)
            {
                // Connect from the output of CreateDynamicData
                CreateDataNode->GetReturnValuePin()->MakeLinkTo(SetDataPin);
            }

            // Connect field name
            UEdGraphPin* FieldNamePin = SetValueNode->FindPin(TEXT("FieldName"));
            if (FieldNamePin)
            {
                FieldNamePin->DefaultValue = Field.Name;
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

    // Connect output directly from the dynamic data
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

void UK2Node_CreateUDPPacket::PinConnectionListChanged(UEdGraphPin* Pin)
{
    Super::PinConnectionListChanged(Pin);

    if (Pin && Pin->PinName == PIN_PacketDefName)
    {
        // Check if we have a packet definition connected
        if (Pin->LinkedTo.Num() > 0)
        {
            UEdGraphPin* LinkedPin = Pin->LinkedTo[0];
            if (LinkedPin)
            {
                UUDPPacketDefinition* PacketDef = Cast<UUDPPacketDefinition>(LinkedPin->DefaultObject);
                if (PacketDef)
                {
                    CreateFieldPins(PacketDef);
                }
            }
        }
    }
}

void UK2Node_CreateUDPPacket::PinDefaultValueChanged(UEdGraphPin* Pin)
{
    Super::PinDefaultValueChanged(Pin);

    if (Pin && Pin->PinName == PIN_PacketDefName)
    {
        // Check if we have a packet definition set via default value
        if (Pin->DefaultObject != nullptr && Pin->DefaultObject->IsA(UUDPPacketDefinition::StaticClass()))
        {
            UUDPPacketDefinition* PacketDef = Cast<UUDPPacketDefinition>(Pin->DefaultObject);
            if (PacketDef)
            {
                CreateFieldPins(PacketDef);
            }
        }
    }
}

void UK2Node_CreateUDPPacket::ReconstructNode()
{
    // Store current packet definition
    UEdGraphPin* PacketDefPin = FindPin(PIN_PacketDefName);
    UUDPPacketDefinition* PacketDef = nullptr;
    
    if (PacketDefPin)
    {
        // Check linked pins first
        if (PacketDefPin->LinkedTo.Num() > 0)
        {
            UEdGraphPin* LinkedPin = PacketDefPin->LinkedTo[0];
            if (LinkedPin && LinkedPin->DefaultObject)
            {
                PacketDef = Cast<UUDPPacketDefinition>(LinkedPin->DefaultObject);
            }
        }
        // Otherwise check default object
        else if (PacketDefPin->DefaultObject)
        {
            PacketDef = Cast<UUDPPacketDefinition>(PacketDefPin->DefaultObject);
        }
    }
    
    // Call parent implementation to clear pins
    Super::ReconstructNode();
    
    // Recreate pins based on stored packet definition
    if (PacketDef)
    {
        CreateFieldPins(PacketDef);
    }
    
    // Make sure the graph knows about the changes
    GetGraph()->NotifyGraphChanged();
}

void UK2Node_CreateUDPPacket::CreateFieldPins(UUDPPacketDefinition* PacketDef)
{
    // Remove any existing field pins
    TArray<UEdGraphPin*> PinsToRemove;
    for (UEdGraphPin* ExistingPin : Pins)
    {
        // Exclude standard pins
        if (ExistingPin->PinName != UEdGraphSchema_K2::PN_Execute &&
            ExistingPin->PinName != UEdGraphSchema_K2::PN_Then &&
            ExistingPin->PinName != PIN_PacketDefName &&
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

    // Skip if no packet definition
    if (!PacketDef)
    {
        return;
    }

    // Make sure packet definition is compiled
    PacketDef->CompileStructure();

    // Create new pins based on the packet definition fields
    for (const FUDPFieldDefinition& Field : PacketDef->Fields)
    {
        UEdGraphPin* NewPin = nullptr;
        
        switch (Field.DataType)
        {
        case EUDPDataType::Float:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, FName(*Field.Name));
            break;
        case EUDPDataType::Int:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, FName(*Field.Name));
            break;
        case EUDPDataType::Bool:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, FName(*Field.Name));
            break;
        case EUDPDataType::String:
            NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, FName(*Field.Name));
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

#undef LOCTEXT_NAMESPACE