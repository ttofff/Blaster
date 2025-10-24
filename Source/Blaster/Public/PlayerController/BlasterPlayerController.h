// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "HUD/BlasterHUD.h"
#include "Weapon/WeaponType.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDWeaponTypeImage(EWeaponType WeaponType);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void HideTeamScore();
	void InitTeamScore();
	void SetHUDRedTeamScore(int32 RedScore);
	void SetHUDBlueTeamScore(int32 BlueScore);

	virtual void Tick(float DeltaSeconds) override;

	virtual float GetServerTime();

	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);

	virtual void HandleMatchHasStarted();
	virtual void HandleCooldown();

	float SingleTripTime = 0.f;

	FHighPingDelegate HighPingDelegate;

	bool bDeadGrenade = false;

	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim, EWeaponType WeaponType, FLinearColor AttackerColor, FLinearColor VictimColor);
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void SetHUDTime();

	void PollInit();

	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	//增强输入输出系统
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<class UInputMappingContext> MappingContext;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Combat")
	TObjectPtr<UInputAction> ControllerQuit;

	/*
	 *返回主菜单
	 */

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	FLinearColor WinTeamColor = FLinearColor::White;
	
	/*
	 *同步服务端与客户端时间
	 */

	//当客户端发送时间请求时，传入请求当前服务端时间
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	//报告当前服务器时间到客户端响应服务器请求时间
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceiveClientRequest);

	float ClientServerDelta = 0.f;//客户端与服务器时间差

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;//客户端与服务器时间同步频率

	float TimeSyncRunningTime = 0.f;//客户端与服务器时间同步运行时间

	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName State, float Warmup, float Match, float Cooldown, float StartingTime);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaSeconds);

	void ShowReturnToMainMenu();

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim, EWeaponType WeaponType, FLinearColor AttackerColor, FLinearColor VictimColor);
	
	FString GetInfoText(const TArray<class ABlasterPlayerState*>& Players);
	FString GetTeamInfoText(class ABlasterGameState* BlasterGameState);

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	UPROPERTY()
	class UReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

	float LevelStartingTime = 0.f;//关卡开始时间
	float MatchTime = 120.f;//对局时间
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;//对局状态

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	
	float HUDHealth;
	float HUDMaxHealth;
	bool bInitializeHealth = false;
	float HUDShield;
	float HUDMaxShield;
	bool bInitializeShield = false;
	float HUDScore;
	bool bInitializeScore = false;
	int32 HUDDefeats;
	bool bInitializeDefeats = false;
	int32 HUDGrenades;
	bool bInitializeGrenades = false;
	int32 HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;
	int32 HUDCarriedAmmo;
	bool bInitializeCarriedAmmo = false;
	EWeaponType HUDWeaponType;
	bool bInitializeWeaponTypeImage = false;

	float HighPingRunningTime = 0.f;//高延迟警告运行时间
	float PingAnimationRunningTime = 0.f;//Ping动画运行时间
	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;//高延迟警告持续时间
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;//高延迟检测频率
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;//高延迟阈值

	float DelayServerTime = 0.f;

	UPROPERTY(EditAnywhere)
	TArray<FString> InfoTeamStrings;
	
private:
	
};

