// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPReceiver.h"

void UUDPReceiver::Receive(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& Endpoint)
{
	if (!&ArrayReaderPtr)
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

void UUDPReceiver::Archive(const FArrayReaderPtr& ArrayReaderPtr)
{
	FUDPData Data;
	*ArrayReaderPtr << Data;
	
	// Class Parameter Update function to set object ready for a query from blueprint
	UpdateReceiverData(Data);
}

void UUDPReceiver::UpdateReceiverData(FUDPData Data)
{
	ReceivedData = Data;
}

FUDPData UUDPReceiver::GetData()
{
	UpdateInterest = true; // Ready for new data
	GotNewData = false;
	return ReceivedData;
}

bool UUDPReceiver::IsNewDataReady()
{
	return GotNewData;
}
