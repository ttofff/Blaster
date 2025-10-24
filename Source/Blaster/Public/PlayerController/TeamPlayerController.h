// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerController/BlasterPlayerController.h"
#include "TeamPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ATeamPlayerController : public ABlasterPlayerController
{
	GENERATED_BODY()

protected:
	virtual void HandleMatchHasStarted() override;
	virtual void HandleCooldown() override;
};
