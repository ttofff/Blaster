// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ElimAnnouncement.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UElimAnnouncement::SetElimAnnouncementText(FString AttackerName, FString VictimName, FLinearColor AttackerColor, FLinearColor VictimColor, EWeaponType WeaponType)
{
	FString AttackText = FString::Printf(TEXT("%s "), *AttackerName);
	FString VictimsText = FString::Printf(TEXT("%s"), *VictimName);
	
	if (AttackerText && VictimText && WeaponTypeImage && WeaponTypeImageMap.Num() > 0)
	{
		AttackerText->SetText(FText::FromString(AttackText));
		AttackerText->SetColorAndOpacity(AttackerColor);
		VictimText->SetText(FText::FromString(VictimsText));
		VictimText->SetColorAndOpacity(VictimColor);

		if (WeaponTypeImageMap.Contains(WeaponType))
		{
			WeaponTypeImage->SetBrushFromTexture(WeaponTypeImageMap[WeaponType]);
		}
	}
}
