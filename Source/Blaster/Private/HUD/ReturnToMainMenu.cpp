// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ReturnToMainMenu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Character/BlasterCharacter.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "GameFramework/GameMode.h"
#include "PlayerController/BlasterPlayerController.h"

void UReturnToMainMenu::MenuSet()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);//设置可视性为可视
	SetIsFocusable(true);//设置可聚焦

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(TakeWidget());//设置聚焦的控件
            PlayerController->SetInputMode(InputMode);
            PlayerController->SetShowMouseCursor(true);//设置鼠标可见
		}
		ABlasterCharacter* Character = Cast<ABlasterCharacter>(PlayerController->GetPawn());
		if (Character)
		{
			MouseSpeedSpinBox->SetValue(Character->GetMouseSpeed() * 2);
		}

		if (MouseSpeedSpinBox)
		{
			MouseSpeedSpinBox->OnValueChanged.AddDynamic(this, &UReturnToMainMenu::MouseSpeedChanged);
		}
	}

	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UReturnToMainMenu::OnDestroySession);
		}
	}
}

void UReturnToMainMenu::MenuTearDown()
{
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(false);//设置鼠标不可见
		}
		ABlasterCharacter* Character = Cast<ABlasterCharacter>(PlayerController->GetPawn());
		if (Character)
		{
			MouseSpeedSpinBox->SetValue(Character->GetMouseSpeed() * 2);
		}
	}
	
	if (ReturnButton && ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}
	if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UReturnToMainMenu::OnDestroySession);
	}
	
	RemoveFromParent();//移除父控件
}

bool UReturnToMainMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UReturnToMainMenu::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ReturnButton->SetIsEnabled(true);//设置按钮可用
		return;
	}
	
	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
		if (GameMode)
		{
			GameMode->ReturnToMainMenuHost();//返回主菜单
		}
		else
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UReturnToMainMenu::OnPlayerLeftGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void UReturnToMainMenu::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);

	UWorld* World = GetWorld();
	if (World)
	{
		ABlasterPlayerController* FirstPlayerController = World->GetFirstPlayerController<ABlasterPlayerController>();
		if (FirstPlayerController)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerController->GetPawn());
			if (BlasterCharacter)
			{
				BlasterCharacter->ServerLeaveGame();
				BlasterCharacter->OnLeftGame.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);
			}
			else
			{
				ReturnButton->SetIsEnabled(true);
			}
		}
	}
}

void UReturnToMainMenu::MouseSpeedChanged(float Value)
{
	PlayerController = PlayerController == nullptr ? GetWorld()->GetFirstPlayerController() : PlayerController;
	ABlasterCharacter* Character = Cast<ABlasterCharacter>(PlayerController->GetPawn());
	if (Character)
	{
		Character->SetMouseSpeed(Value/2);
	}
}
