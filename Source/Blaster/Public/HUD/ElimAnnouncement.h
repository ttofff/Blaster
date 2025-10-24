// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Weapon/WeaponType.h"
#include "ElimAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UElimAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetElimAnnouncementText(FString AttackerName, FString VictimName, FLinearColor AttackerColor, FLinearColor VictimColor, EWeaponType WeaponType);
	
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* AnnouncementBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AttackerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* VictimText;

	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponTypeImage;

	UPROPERTY(EditAnywhere)
	TMap<EWeaponType, UTexture2D*> WeaponTypeImageMap;
};
