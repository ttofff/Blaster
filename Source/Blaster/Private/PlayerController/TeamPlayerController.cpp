// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/TeamPlayerController.h"
#include "BlasterComponents/CombatComponent.h"
#include "Character/BlasterCharacter.h"
#include "GameState/BlasterGameState.h"
#include "Kismet/GameplayStatics.h"

void ATeamPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay == nullptr) BlasterHUD->AddCharacterOverlay();//添加角色HUD
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);//隐藏公告
		}

		if (!HasAuthority()) return;

		InitTeamScore();
	}
}

void ATeamPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent();//移除角色HUD
		bool bHUDValid = BlasterHUD
		&& BlasterHUD->Announcement
		&& BlasterHUD->Announcement->NewAnnouncementText != FString("")
		&& BlasterHUD->Announcement->InfoText;
		if (bHUDValid)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);//显示公告
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(BlasterHUD->Announcement->NewAnnouncementText));//设置公告文本

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			if (BlasterGameState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString = GetTeamInfoText(BlasterGameState);//获取信息文本
				
				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));//设置信息文本
				BlasterHUD->Announcement->InfoText->SetColorAndOpacity(WinTeamColor);
			}
			
		}
	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombat())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}
