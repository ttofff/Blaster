// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"
#include "IntoMapTime.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerSessionsSubsystem.h"
#include "PlayerController/LobbyPlayerController.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);

		MatchType = Subsystem->DesiredMatchType;

		if (MatchType == "FreeForAll")
		{
			LobbyMatchTypeIndex = 0;
		}
		else if (MatchType == "Teams")
		{
			LobbyMatchTypeIndex = 1;
		}
		else if (MatchType == "CaptureTheFlag")
		{
			LobbyMatchTypeIndex = 2;
		}

		if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections)//到达指定人数时，进入游戏
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bUseSeamlessTravel = true;

				bReady = true;
				
				GetWorldTimerManager().SetTimerForNextTick([this]()
				{
					//遍历场景中所有的玩家控制器
					for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
					{
						ALobbyPlayerController* LobbyPlayer = Cast<ALobbyPlayerController>(*It);
						if (LobbyPlayer)
						{
							LobbyPlayer->BrocastChangeInfo();
						}
					}
				});

				GetWorldTimerManager().SetTimer(
					FIntoMapTimer,
					[World,this]()
					{
						if (MatchType == "FreeForAll")
						{
							 World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
						}
						else if (MatchType == "Teams")
						{
							World->ServerTravel(FString("/Game/Maps/Teams?listen"));
						}
						else if (MatchType == "CaptureTheFlag")
						{
							World->ServerTravel(FString("/Game/Maps/CaptureTheFlag?listen"));
						}
					}, WarmupTime,false);
			}
		}
	}
}
void ALobbyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

