// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPSender.h"

// Sets default values for this component's properties
UUDPSender::UUDPSender(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SenderSocket = nullptr;
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

bool UUDPSender::UDPSendPacket(const FUDPPacket& UDPPacket)
{
	if (!SenderSocket)
	{
		UE_LOG(LogTemp, Log, TEXT("There is no socket."));
		return false;
	}
    
	if (!UDPPacket.Structure || UDPPacket.Data.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid UDP packet to send."));
		return false;
	}
    
	int32 BytesSent = 0;
	SenderSocket->SendTo(UDPPacket.Data.GetData(), UDPPacket.Data.Num(), BytesSent, *RemoteAddr);
	
	if (BytesSent <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Socket exists, but receiver did not accept any packets."));
		return false;
	}

	return true;
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
