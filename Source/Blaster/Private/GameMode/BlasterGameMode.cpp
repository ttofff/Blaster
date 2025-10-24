// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BlasterGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "GameState/BlasterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/BlasterPlayerState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;//延迟开始
}

void ABlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;//倒计时
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = MatchTime + WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;//倒计时
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + MatchTime + WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;//倒计时
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

//玩家被淘汰
void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedPlayer, APlayerController* VictimController,
										APlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;//获取攻击者状态
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;//获取受害者状态
	EWeaponType WeaponType;
	if (AttackerPlayerState)
	{
		ABlasterPlayerController* VictimPlayerController = Cast<ABlasterPlayerController>(VictimController);
		if (VictimPlayerController && VictimPlayerController->bDeadGrenade)
		{
			WeaponType = EWeaponType::EWT_Grenade;
		}
		else
		{
			WeaponType = Cast<ABlasterCharacter>(AttackerController->GetPawn())->GetEquippedWeapon()->GetWeaponType();
		}
	}
	else WeaponType = EWeaponType::EWT_Max;
	
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	
	//攻击者加分
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
	{
		TArray<ABlasterPlayerState*> PlayerCurrentlyInTheLead;
		for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
		{
			PlayerCurrentlyInTheLead.Add(LeadPlayer);
		}
		
		AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerPlayerState);
		if (BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState))//如果攻击者在第一名
		{
			ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayerCurrentlyInTheLead.Num(); i++)
		{
			if (!BlasterGameState->TopScoringPlayers.Contains(PlayerCurrentlyInTheLead[i]))//如果不在第一名
			{
				ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayerCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}

	//受害者增加死亡次数
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	
	if (EliminatedPlayer)
	{
		EliminatedPlayer->Elim(false);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			FLinearColor AttackerColor = AttackerPlayerState->GetTeam() == ETeam::ET_NoTeam ? FLinearColor::White : AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam ? BlueTeamColor : RedTeamColor;
			FLinearColor VictimColor = VictimPlayerState->GetTeam() == ETeam::ET_NoTeam ? FLinearColor::White : VictimPlayerState->GetTeam() == ETeam::ET_BlueTeam ? BlueTeamColor : RedTeamColor;
			BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState, WeaponType, AttackerColor, VictimColor);
		}
	}
}

//玩家请求重生
void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();//重置
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);//获取所有玩家起始点
		int32 Slection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Slection]);
	}
}

float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)//计算伤害
{
	return BaseDamage;
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
	if (BlasterCharacter)
	{
		BlasterCharacter->Elim(true);
	}
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)//遍历获取所有玩家控制器
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState);
		}
	}
}
