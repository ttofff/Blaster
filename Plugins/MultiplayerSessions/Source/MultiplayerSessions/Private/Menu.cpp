// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Components/CanvasPanel.h"
#include "Interfaces/OnlineSessionInterface.h"

void UMenu::MenuSetup(int32 NumOfPublicConnections, FString MatchOfType, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);//设置路径
	NumPublicConnections = NumOfPublicConnections;
	MatchType = MatchOfType;
	
	AddToViewport();//添加到视口
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);//设置焦点

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());//获取焦点
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);//不锁定鼠标
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true); 
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);//创建会话完成回调
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);//查找会话完成回调
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);//加入会话完成回调
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);//销毁会话完成回调
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);//启动会话完成回调
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::ConfirmButtonClicked);
	}
	if (ReturnButton)
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::ReturnButtonClicked);
	}
	
	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();//销毁菜单
	
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);//服务器跳转到大厅
		}
	}
	else
	{
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if(MultiplayerSessionsSubsystem == nullptr) return;
	bool bIsActive = false;

	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);//加入会话
			bIsActive = true;
		}
	}
	if(!bWasSuccessful || SessionResults.Num() == 0 || !bIsActive)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr Sessioninterface = Subsystem->GetSessionInterface();
		if (Sessioninterface.IsValid())
		{
			FString Address;
			Sessioninterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);//客户端跳转到服务器
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);

	if (CreateRoom)
	{
		bIsCreate = true;
		CreateRoom->SetVisibility(ESlateVisibility::Visible);
		SetNumPlayerBoxVisible(true);
		ReturnButton->SetIsEnabled(true);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (CreateRoom)
	{
		bIsCreate = false;
		CreateRoom->SetVisibility(ESlateVisibility::Visible);
		SetNumPlayerBoxVisible(false);
		ReturnButton->SetIsEnabled(true);
	}
}

void UMenu::ConfirmButtonClicked()
{
	if (bIsCreate)
	{
		ConfirmButton->SetIsEnabled(false);
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);//创建会话
		}
	}
	else
	{
		ConfirmButton->SetIsEnabled(false);
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->FindSessions(10000);//查找会话
		}
	}
}

void UMenu::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);

	if (CreateRoom)
	{
		//折叠
		CreateRoom->SetVisibility(ESlateVisibility::Collapsed);
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();//从父节点移除
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);//隐藏鼠标	
		}
	}
}
