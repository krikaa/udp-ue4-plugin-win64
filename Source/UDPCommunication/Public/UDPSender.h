#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Networking.h"
#include "UDPDynamicData.h"
#include "UDPPacketDefinition.h"
#include "Serialization/Archive.h"
#include "UDPSender.generated.h"

UCLASS(ClassGroup=(Communication), meta=(BlueprintSpawnableComponent))
class UDPCOMMUNICATION_API UUDPSender : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUDPSender(const FObjectInitializer& ObjectInitializer);
	
	TSharedPtr<FInternetAddr> RemoteAddr;
	FSocket* SenderSocket;

	// Creates the socket for the sender
	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		bool StartUDPSender(
			const FString& SocketName,
			const FString& IpAddress,
			const int32 Port
			);

	// // Sends the serialized array
	// UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	// 	bool UDPSendArray(FUDPData Data);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDPCommunication")
	UUDPPacketDefinition* PacketDefinition;

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	bool UDPSendDynamicData(const FUDPDynamicData& DynamicData);

	// For backward compatibility
	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
	FUDPDynamicData CreateDynamicData();
};
