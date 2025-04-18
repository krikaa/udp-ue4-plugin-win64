// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPReceiver.h"

void UUDPReceiver::Receive(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& Endpoint)
{
	if (!ArrayReaderPtr.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot read array, nullptr returned."));
		return;
	}

	GotNewData = true;
	if (UpdateInterest)	
		Archive(ArrayReaderPtr);
}

bool UUDPReceiver::StartUDPReceiver(const FString& SocketName, const int32 Port)
{
	FIPv4Endpoint EndPoint(FIPv4Address::Any, Port);
	int32 BufferSize = 2 * 1024 * 1024;
	ListenSocket = FUdpSocketBuilder(*SocketName).AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(EndPoint)
		.WithReceiveBufferSize(BufferSize);

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	Receiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP Receiver"));
	Receiver->OnDataReceived().BindUObject(this, &UUDPReceiver::Receive);
	Receiver->Start();
	return true;
}

void UUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	delete Receiver;
	Receiver = nullptr;

	// Clear all sockets
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}

void UUDPReceiver::IsNewDataReady(EDataReadyBranches& Branches)
{
	Branches = GotNewData ? EDataReadyBranches::Ready : EDataReadyBranches::NotReady;
}

FUDPPacket UUDPReceiver::GetUDPPacket()
{
	UpdateInterest = true;
	GotNewData = false;
	return ReceivedUDPData;
}

// Add possible legacy support for older versions?
void UUDPReceiver::Archive(const FArrayReaderPtr& ArrayReaderPtr)
{
	FUDPPacket UDPPacket;
    
	// Get the raw data from the array reader
	int32 ByteCount = ArrayReaderPtr->Num();
	if (ByteCount > 0)
	{
		// Resize the data array first
		UDPPacket.Data.SetNumUninitialized(ByteCount);
        
		// Direct memory copy from the array reader data to our packet data
		FMemory::Memcpy(UDPPacket.Data.GetData(), ArrayReaderPtr->GetData(), ByteCount);
        
		UE_LOG(LogTemp, Display, TEXT("Received UDP packet with %d bytes"), ByteCount);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Received empty UDP packet"));
	}

	// Assign the structure if not already assigned
	if (PacketStructure)
	{
		UDPPacket.Structure = PacketStructure;
	}
	
	ReceivedUDPData = UDPPacket;
}
