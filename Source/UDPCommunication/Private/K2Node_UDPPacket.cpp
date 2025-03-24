// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_UDPPacket.h"

#define LOCTEXT_NAMESPACE "K2Node_UDPPacket"

// Input pin names
const FName UK2Node_CreateUDPPacket::PIN_SenderName(TEXT("UDPSender"));
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

    // Create the UDPSender input pin
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UUDPSender::StaticClass(), PIN_SenderName);

    // Create the PacketDefinition input pin
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UUDPPacketDefinition::StaticClass(), PIN_PacketDefName);

    // Create output pin (UDPDynamicData)
    UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, nullptr, PIN_OutputName);
    OutputPin->PinType.PinSubCategoryObject = FUDPDynamicData::StaticStruct();
}

void UK2Node_CreateUDPPacket::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    UE_LOG(LogTemp, Warning, TEXT("ExpandNode START for CreateUDPPacket"));

    UFunction* CreateDynamicDataFunc = UUDPSender::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UUDPSender, CreateDynamicData));
    if (!CreateDynamicDataFunc)
    {
        UE_LOG(LogTemp, Error, TEXT("Function CreateDynamicData not found in UUDPSender"));
        return;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateDynamicData function found with %d parameters"), CreateDynamicDataFunc->NumParms);
        for (TFieldIterator<FProperty> It(CreateDynamicDataFunc); It; ++It)
        {
            UE_LOG(LogTemp, Warning, TEXT("  Parameter: %s, Type: %s"), *It->GetName(), *It->GetCPPType());
        }
    }
    
    Super::ExpandNode(CompilerContext, SourceGraph);

    // Find our pins
    UEdGraphPin* ExecPin = FindPinChecked(UEdGraphSchema_K2::PN_Execute);
    UE_LOG(LogTemp, Warning, TEXT("ExecPin found: %s"), *ExecPin->PinName.ToString());
    UEdGraphPin* ThenPin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
    UE_LOG(LogTemp, Warning, TEXT("ThenPin found: %s"), *ThenPin->PinName.ToString());
    UEdGraphPin* SenderPin = FindPinChecked(PIN_SenderName);
    UE_LOG(LogTemp, Warning, TEXT("SenderPin found: %s"), *SenderPin->PinName.ToString());
    UEdGraphPin* PacketDefPin = FindPinChecked(PIN_PacketDefName);
    UE_LOG(LogTemp, Warning, TEXT("PacketDefPin found: %s"), *PacketDefPin->PinName.ToString());
    UEdGraphPin* OutputPin = FindPinChecked(PIN_OutputName);
    UE_LOG(LogTemp, Warning, TEXT("OutpuPin found: %s"), *OutputPin->PinName.ToString());

    // No packet definition provided - log error if not connected
    if (PacketDefPin->LinkedTo.Num() == 0 && !PacketDefPin->DefaultObject)
    {
        CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "MissingPacketDefinition", "No packet definition connected to @@").ToString(), this);
        return;
    }

    // Create a call to UUDPSender::CreateDynamicData
    UK2Node_CallFunction* CreateDataNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    CreateDataNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UUDPSender, CreateDynamicData), UUDPSender::StaticClass());
    CreateDataNode->AllocateDefaultPins();

    if (!UUDPSender::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UUDPSender, CreateDynamicData)))
    {
        UE_LOG(LogTemp, Error, TEXT("Function CreateDynamicData not found in UUDPSender"));
    }

    // Connect execution flow
    //ExecPin->MakeLinkTo(CreateDataNode->GetExecPin());

    // Connect the sender pin to the self pin of the function call
    CompilerContext.MovePinLinksToIntermediate(*SenderPin, *CreateDataNode->FindPinChecked(TEXT("self")));

    // Create a temporary variable to store the dynamic data
    UK2Node_TemporaryVariable* TempVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>(this, SourceGraph);
    TempVarNode->VariableType.PinCategory = UEdGraphSchema_K2::PC_Struct;
    TempVarNode->VariableType.PinSubCategoryObject = FUDPDynamicData::StaticStruct();
    TempVarNode->AllocateDefaultPins();
    UEdGraphPin* TempVarPin = TempVarNode->GetVariablePin();

    // Assign the result of CreateDynamicData to our temp var
    UK2Node_AssignmentStatement* AssignNode = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph);
    AssignNode->AllocateDefaultPins();
    AssignNode->GetVariablePin()->MakeLinkTo(TempVarPin);
    AssignNode->GetValuePin()->MakeLinkTo(CreateDataNode->GetReturnValuePin());
    CreateDataNode->GetThenPin()->MakeLinkTo(AssignNode->GetExecPin());

    // Keep track of the last node's then pin
    UEdGraphPin* LastThenPin = AssignNode->GetThenPin();

    // Now create a call to UUDPDynamicDataLibrary::InitWithDefinition instead of a struct method
    UK2Node_CallFunction* SetPacketDefNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    SetPacketDefNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, InitWithDefinition), UUDPDynamicDataLibrary::StaticClass());
    SetPacketDefNode->AllocateDefaultPins();

    // Connect execution flow
    LastThenPin->MakeLinkTo(SetPacketDefNode->GetExecPin());
    LastThenPin = SetPacketDefNode->GetThenPin();

    // Connect the DynamicData parameter
    UEdGraphPin* DataPin = SetPacketDefNode->FindPinChecked(TEXT("DynamicData"));
    DataPin->MakeLinkTo(TempVarPin);

    // Connect the PacketDefinition pin
    CompilerContext.MovePinLinksToIntermediate(*PacketDefPin, *SetPacketDefNode->FindPinChecked(TEXT("Definition")));

    // For each field pin in the node, create set value calls
    for (UEdGraphPin* FieldPin : Pins)
    {
        // Skip standard pins
        if (FieldPin->PinName == UEdGraphSchema_K2::PN_Execute ||
            FieldPin->PinName == UEdGraphSchema_K2::PN_Then ||
            FieldPin->PinName == PIN_SenderName ||
            FieldPin->PinName == PIN_PacketDefName ||
            FieldPin->PinName == PIN_OutputName)
        {
            continue;
        }

        // Skip pins with no connections
        if (FieldPin->LinkedTo.Num() == 0 && FieldPin->DefaultValue.IsEmpty() &&
            FieldPin->DefaultObject == nullptr && FieldPin->DefaultTextValue.IsEmpty())
        {
            continue;
        }

        // Determine the field data type from pin type
        FName FunctionName;
        if (FieldPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Real)
        {
            FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, SetFloat);
        }
        else if (FieldPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Int)
        {
            FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, SetInt);
        }
        else if (FieldPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
        {
            FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, SetBool);
        }
        else if (FieldPin->PinType.PinCategory == UEdGraphSchema_K2::PC_String)
        {
            FunctionName = GET_FUNCTION_NAME_CHECKED(UUDPDynamicDataLibrary, SetString);
        }
        else
        {
            // Unsupported pin type
            continue;
        }

        // Create a call to the appropriate set function in UDPDynamicDataLibrary
        UK2Node_CallFunction* SetValueNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
        SetValueNode->FunctionReference.SetExternalMember(FunctionName, UUDPDynamicDataLibrary::StaticClass());
        SetValueNode->AllocateDefaultPins();

        // Connect execution flow
        LastThenPin->MakeLinkTo(SetValueNode->GetExecPin());
        LastThenPin = SetValueNode->GetThenPin();

        // Connect the DynamicData parameter to our temp var
        UEdGraphPin* DynamicDataPin = SetValueNode->FindPinChecked(TEXT("DynamicData"));
        DynamicDataPin->MakeLinkTo(TempVarPin);

        // Set field name
        UEdGraphPin* FieldNamePin = SetValueNode->FindPinChecked(TEXT("FieldName"));
        FieldNamePin->DefaultValue = FieldPin->PinName.ToString();

        // Connect field value from the original node's pin
        UEdGraphPin* ValuePin = SetValueNode->FindPinChecked(TEXT("Value"));
        CompilerContext.MovePinLinksToIntermediate(*FieldPin, *ValuePin);
    }

    // Connect the output pin directly to our temp variable
    CompilerContext.MovePinLinksToIntermediate(*OutputPin, *TempVarPin);

    // Connect the final then pin
    CompilerContext.MovePinLinksToIntermediate(*ThenPin, *LastThenPin);

    // In your UK2Node_CreateUDPPacket::ExpandNode function
    UK2Node_CallFunction* DebugNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    DebugNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, PrintString), UKismetSystemLibrary::StaticClass());
    DebugNode->AllocateDefaultPins();
    // Connect to execution flow
    ExecPin->MakeLinkTo(DebugNode->GetExecPin());
    // Set debug text
    DebugNode->FindPinChecked(TEXT("InString"))->DefaultValue = TEXT("UDP Packet Node Executed");
    // Connect to next node
    DebugNode->GetThenPin()->MakeLinkTo(CreateDataNode->GetExecPin());
    
    // Break all node links since we've expanded everything
    BreakAllNodeLinks();
}

void UK2Node_CreateUDPPacket::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);
        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
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
            ExistingPin->PinName != PIN_SenderName &&
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