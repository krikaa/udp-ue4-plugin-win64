// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Networking.h"
#include "UDPPacket.h"
#include "UDPPacketStructure.h"
#include "UDPReceiver.generated.h"

UENUM(BlueprintType)
enum class EDataReadyBranches : uint8
{
	Ready,
	NotReady
};

/**
 * 
 */
UCLASS(ClassGroup=(UDPCommunication), meta=(BlueprintSpawnableComponent))
class UDPCOMMUNICATION_API UUDPReceiver : public UActorComponent
{
	GENERATED_BODY()
	
public:
	bool UpdateInterest = true;
	bool GotNewData = false;

	FSocket* ListenSocket;
	FUdpSocketReceiver* Receiver = nullptr;
	void Receive(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& Endpoint);
	virtual void Archive(const FArrayReaderPtr& ArrayReaderPtr);
	
	UFUNCTION(BlueprintCallable, Category = "UDPCommunication", DisplayName = "Is New Data Ready?",
		meta = (ExpandEnumAsExecs = "Branches", 
		ToolTip = "Checks if new data has been received.\nNOTE! Data is marked as new as long the 'Get UDP Packet' node is not called."))
		void IsNewDataReady(EDataReadyBranches& Branches);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication", DisplayName = "Start UDP Receiver",
		meta = (ToolTip = "Creates a UDP socket to receive data from."))
		bool StartUDPReceiver(const FString& SocketName, const int32 Port);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	UUDPPacketStructure* PacketStructure;

	UPROPERTY(BlueprintReadOnly, Category = "UDPCommunication")
	FUDPPacket ReceivedUDPData;

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication", DisplayName = "Get UDP Packet",
		meta = (ToolTip = "Gets the last UDP packet received.\nBreak the packet using the node 'Break UDP Packet'"))
	FUDPPacket GetUDPPacket();
};
