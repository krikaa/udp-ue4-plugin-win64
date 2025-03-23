// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Networking.h"
#include "UDPData.h"
#include "UDPReceiver.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Communication), meta=(BlueprintSpawnableComponent))
class UDPCOMMUNICATION_API UUDPReceiver : public UActorComponent
{
	GENERATED_BODY()
	
public:
	FUDPData ReceivedData;
	bool UpdateInterest = true;
	bool GotNewData = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "UDPCommunication")
		void BPEvent_DataReceived(const FUDPData& Data);

	FSocket* ListenSocket;
	FUdpSocketReceiver* Receiver = nullptr;
	void Receive(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& Endpoint);
	virtual void Archive(const FArrayReaderPtr& ArrayReaderPtr);
	virtual void UpdateReceiverData(FUDPData Data);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		FUDPData GetData();

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		bool IsNewDataReady();

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		bool StartUDPReceiver(const FString& SocketName, const int32 Port);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
