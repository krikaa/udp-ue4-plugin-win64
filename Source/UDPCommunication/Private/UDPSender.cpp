


#include "UDPSender.h"


// Sets default values for this component's properties
UUDPSender::UUDPSender(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SenderSocket = nullptr;
}

// Called when the game starts
void UUDPSender::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUDPSender::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool UUDPSender::StartUDPSender(const FString& SocketName, const FString& IpAddress, const int32 Port)
{
	// Create Remote Address
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool IsValidIpAddress = false;
	RemoteAddr->SetIp(*IpAddress, IsValidIpAddress);
	RemoteAddr->SetPort(Port);

	if (!IsValidIpAddress)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong IP Address supplied"));
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*SocketName)
		.AsReusable()
		.WithBroadcast();

	// Set Send buffer size
	int32 SendSize = 2 * 1024 * 1024;
	SenderSocket->SetSendBufferSize(SendSize, SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);
	SenderSocket->SetBroadcast(true);

	return true;
}

// bool UUDPSender::UDPSendArray(FUDPData Data)
// {
// 	if (!SenderSocket)
// 	{
// 		UE_LOG(LogTemp, Log, TEXT("There is no socket."));
// 		return false;
// 	}
// 	int32 BytesSent = 0;
//
// 	FArrayWriter Writer;
// 	Writer << Data;
// 	SenderSocket->SendTo(Writer.GetData(), Writer.Num(), BytesSent, *RemoteAddr);
//
// 	if (BytesSent <= 0)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Socket exists , but receiver did not accept any packets."));
// 		return false;
// 	}
//
// 	return true;
// }

bool UUDPSender::UDPSendDynamicData(const FUDPDynamicData& DynamicData)
{
	if (!SenderSocket)
	{
		UE_LOG(LogTemp, Log, TEXT("There is no socket."));
		return false;
	}
    
	if (!DynamicData.Definition || DynamicData.Data.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid dynamic data to send."));
		return false;
	}
    
	int32 BytesSent = 0;

	// Serialize the data with the array length in front
	// FArrayWriter Writer;
	// Writer << const_cast<FUDPDynamicData&>(DynamicData);
	// SenderSocket->SendTo(Writer.GetData(), Writer.Num(), BytesSent, *RemoteAddr);
	
	// Serialize the data without the array length in front
	SenderSocket->SendTo(DynamicData.Data.GetData(), DynamicData.Data.Num(), BytesSent, *RemoteAddr);
	
	if (BytesSent <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Socket exists, but receiver did not accept any packets."));
		return false;
	}

	return true;
}

FUDPDynamicData UUDPSender::CreateDynamicData()
{
	FUDPDynamicData DynamicData;
	if (PacketDefinition)
	{
		DynamicData.InitWithDefinition(PacketDefinition);
	}
	return DynamicData;
}

void UUDPSender::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SenderSocket)
	{
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
	}
}
