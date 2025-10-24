// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayerController/LobbyPlayerController.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	float WarmupTime = 10.f;

	UPROPERTY()
	bool bReady = false;
	
	FString MatchType;

	int32 LobbyMatchTypeIndex = 0;

protected:
	virtual void BeginPlay() override;

private:
	
	FTimerHandle FIntoMapTimer;
};
