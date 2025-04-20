// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Networking.h"
#include "UDPPacket.h"
#include "UDPPacketStructure.h"
#include "Serialization/Archive.h"
#include "UDPSender.generated.h"

UCLASS(ClassGroup=(UDPCommunication), meta=(BlueprintSpawnableComponent))
class UDPCOMMUNICATION_API UUDPSender : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUDPSender(const FObjectInitializer& ObjectInitializer);
	
	TSharedPtr<FInternetAddr> RemoteAddr;
	FSocket* SenderSocket;

	// Creates the socket for the sender
	UFUNCTION(BlueprintCallable, Category = "UDPCommunication", DisplayName = "Start UDP Sender",
		meta = (ToolTip = "Creates a UDP socket to send data from."))
		bool StartUDPSender(
			const FString& SocketName,
			const FString& IpAddress,
			const int32 Port
			);

protected:


public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication", DisplayName = "Send UDP Packet",
		meta = (ToolTip = "Sends the created UDP packet.\nTo create a packet, use the 'Create UDP Packet' node."))
	bool UDPSendPacket(const FUDPPacket& UDPPacket);
};
