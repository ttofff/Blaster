// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDIntoMapTime(float CountdownTime);
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool bEnable = false;

	void BrocastChangeInfo();

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void SetHUDExplainText(int32 IndexMatchType);
	
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(float Warmup, int32 IndexMatchType);

	UFUNCTION(Client, Reliable)
	void ClientChangeInfo();

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UIntoMapTime> IntoMapTimeClass;

	UPROPERTY()
	UIntoMapTime* IntoMapTime;

	UPROPERTY(EditAnywhere)
	float WarmupTime = 0.f;

	int32 MatchTypeIndex = 0;

	uint32 CountdownInt = 0;
	float LevelStartingTime = 0.f;//关卡开始时间

	bool bIsStart = true;

	UPROPERTY(Replicated)
	class ALobbyGameMode* LobbyGameMode;
};
