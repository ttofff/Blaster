// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/LobbyPlayerController.h"
#include "IntoMapTime.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ALobbyPlayerController::SetHUDIntoMapTime(float CountdownTime)
{
	bool bHUDValid = IntoMapTime && IntoMapTime->WaitingTime;
	if (!bHUDValid) return;
	if (CountdownTime < 0.f)
	{
		FString ZeroTextText = FString::Printf(TEXT("00:00"));//格式化字符串
		IntoMapTime->WaitingTime->SetText(FText::FromString(ZeroTextText));//设置分数文本
		return;
	}
		
	int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);//获取分钟
	int32 Seconds = CountdownTime - Minutes * 60;//获取秒数
		
	FString CountdownTextText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);//格式化字符串
	IntoMapTime->WaitingTime->SetText(FText::FromString(CountdownTextText));//设置分数文本
}

void ALobbyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	SetHUDTime();
}

void ALobbyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerController, LobbyGameMode);
}

void ALobbyPlayerController::BrocastChangeInfo()
{
	ClientChangeInfo();
}

void ALobbyPlayerController::ClientChangeInfo_Implementation()
{
	if (IntoMapTimeClass && IntoMapTime == nullptr)
	{
		IntoMapTime = CreateWidget<UIntoMapTime>(GetWorld(), IntoMapTimeClass);
		IntoMapTime->AddToViewport();
	}

	LevelStartingTime = GetWorld()->GetTimeSeconds();
	bEnable = true;
	
	if (IntoMapTime)
	{
		if (IntoMapTime->WaitingTexts.Num() > 0)
		{
			SetHUDExplainText(MatchTypeIndex);
			IntoMapTime->WaitingText->SetText(IntoMapTime->WaitingTexts[1]);//设置等待文本
		}
	}
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	LevelStartingTime = GetWorld()->GetTimeSeconds();

	ServerCheckMatchState();
}

void ALobbyPlayerController::SetHUDTime()
{
	if(!bEnable) return;
	float TimeLeft = 0.f;
	TimeLeft = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		
	SetHUDIntoMapTime(TimeLeft);
}

void ALobbyPlayerController::SetHUDExplainText(int32 IndexMatchType)
{
	if (IntoMapTime && IntoMapTime->ExplainTexts.Num() > 0)
	{
		FText ExplainText = IntoMapTime->ExplainTexts[IndexMatchType];
		IntoMapTime->ExplainText->SetText(ExplainText);
	}
}

void ALobbyPlayerController::ClientJoinMidgame_Implementation(float Warmup, int32 IndexMatchType)
{
	WarmupTime = Warmup;
	MatchTypeIndex = IndexMatchType;

	if (IntoMapTimeClass && IntoMapTime == nullptr)
	{
		IntoMapTime = CreateWidget<UIntoMapTime>(GetWorld(), IntoMapTimeClass);
		IntoMapTime->AddToViewport();
			
		if (IntoMapTime)
		{
			IntoMapTime->WaitingTime->SetText(FText());//设置等待文本
			SetHUDExplainText(MatchTypeIndex);
			if (IntoMapTime->WaitingTexts.Num() > 0)
			{
				IntoMapTime->WaitingText->SetText(IntoMapTime->WaitingTexts[0]);//设置等待文本
			}
		}
	}
}

void ALobbyPlayerController::ServerCheckMatchState_Implementation()
{
	LobbyGameMode = LobbyGameMode == nullptr ? Cast<ALobbyGameMode>(UGameplayStatics::GetGameMode(this)) : LobbyGameMode;
	if (LobbyGameMode)
	{
		WarmupTime = LobbyGameMode->WarmupTime;
		MatchTypeIndex = LobbyGameMode->LobbyMatchTypeIndex;

		ClientJoinMidgame(WarmupTime, MatchTypeIndex);
	}
}
