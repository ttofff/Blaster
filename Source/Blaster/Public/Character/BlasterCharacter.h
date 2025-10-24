// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NiagaraComponent.h"
#include "BlasterTypes/CombatState.h"
#include "BlasterTypes/Team.h"
#include "BlasterTypes/TurningInPlace.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "PlayerController/BlasterPlayerController.h"
#include "Sound/SoundCue.h"
#include "Weapon/Weapon.h"
#include "BlasterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	/*
	 *播放Montage
	 */
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapMontage();

	void Elim(bool bPlayerLeftGame);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim(bool bPlayerLeftGame);

	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)//BlueprintImplementableEvent可以在蓝图中实现
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();

	void SpawnDefaultWeapon();

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	UPROPERTY(EditAnywhere)
	bool bIsPose = false;

	bool bFinishedSwapping = false;
	
	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	void SetTeamMessage(ETeam Team);

	UFUNCTION(NetMulticast, Reliable)
	void AttachSpeedTrail();

	UFUNCTION(NetMulticast, Reliable)
	void DetachSpeedTrail();
	
	UFUNCTION(NetMulticast, Reliable)
	void AttachJumpTrail();

	UFUNCTION(NetMulticast, Reliable)
	void DetachJumpTrail();

	UFUNCTION(Server , Reliable)
	void ServerDropFlag();

	UFUNCTION(Client, Reliable)
	void ClientDropFlag();
	
protected:
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;

	void Action_ControllerLook(const FInputActionValue& Value);
	void Action_ControllerMove(const FInputActionValue& Value);
	void Action_ControllerStartJump(const FInputActionValue& Value);
	void Action_ControllerStopJump(const FInputActionValue& Value);
	void Action_ControllerEquip(const FInputActionValue& Value);
	void Action_ControllerReload(const FInputActionValue& Value);
	void Action_ControllerCrouch(const FInputActionValue& Value);
	void Action_ControllerAimPressed(const FInputActionValue& Value);
	void Action_ControllerAimReleased(const FInputActionValue& Value);
	void Action_ControllerFirePressed(const FInputActionValue& Value);
	void Action_ControllerFireReleased(const FInputActionValue& Value);
	void Action_ControllerThrowGrenadePressed(const FInputActionValue& Value);

	void AimOffset(float DeltaTime);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	
	void RollInit();

	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();

	void SetSpawnPoint();
	void OnPlayerStateInitialized();

	/*
	 *用于服务器命中的碰撞盒
	 */

	UPROPERTY(EditAnywhere)
	class UBoxComponent* Head;//头

	UPROPERTY(EditAnywhere)
	UBoxComponent* Pelvis;//腰

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine_02;//腹

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine_03;//胸后

	UPROPERTY(EditAnywhere)
	UBoxComponent* Chest;//胸前

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArm_L;//左臂

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArm_R;//右臂

	UPROPERTY(EditAnywhere)
	UBoxComponent* LowerArm_L;//左小臂

	UPROPERTY(EditAnywhere)
	UBoxComponent* LowerArm_R;//右小臂

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hand_L;//左手

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hand_R;//右手

	UPROPERTY(EditAnywhere)
	UBoxComponent* Thigh_L;//左腿

	UPROPERTY(EditAnywhere)
	UBoxComponent* Thigh_R;//右腿

	UPROPERTY(EditAnywhere)
	UBoxComponent* Calf_L;//左小腿

	UPROPERTY(EditAnywhere)
	UBoxComponent* Calf_R;//右小腿

	UPROPERTY(EditAnywhere)
	UBoxComponent* Foot_L;//左脚

	UPROPERTY(EditAnywhere)
	UBoxComponent* Foot_R;//右脚

private:
	//增强输入输出系统
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Basic")
	TObjectPtr<UInputAction> ControllerLook;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Basic")
	TObjectPtr<UInputAction> ControllerMove;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Basic")
	TObjectPtr<UInputAction> ControllerJump;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Basic")
	TObjectPtr<UInputAction> ControllerCrouch;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Combat")
	TObjectPtr<UInputAction> ControllerEquip;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Combat")
	TObjectPtr<UInputAction> ControllerReload;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Combat")
	TObjectPtr<UInputAction> ControllerAim;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Combat")
	TObjectPtr<UInputAction> ControllerFire;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Combat")
	TObjectPtr<UInputAction> ControllerThrowGrenade;

	//变量

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Character)
	bool bIsPressedJump{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	/*
	 *组件
	 */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class ULagCompensationComponent* LagCompensation;

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation = FRotator(0.f, -90.f, 0.f);

	ETurningInPlace TurningInPlace;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage;

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;//显示摄像头的距离

	bool bElimmed = false;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;

	float MouseSpeed = .75f;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SpeedTrailComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* JumpTrailComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* SpeedTrailSystem;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* JumpTrailSystem;

	/*
	 *淘汰
	 */

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	
	/*
	 *玩家生命值
	 */

	UPROPERTY(EditAnywhere, Category = "Player States")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player States")
	float Health = 100.f;

	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController;

	/*
	 *护盾
	 */

	UPROPERTY(EditAnywhere, Category = "Player States")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player States")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);
	
	/*
	 *溶解效果
	 */

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UPROPERTY(EditAnywhere, Category = Elim)
	TArray<UMaterialInstanceDynamic*> DynamicDissolveMaterialInstances;

	UPROPERTY(EditAnywhere, Category = Elim)
	TArray<UMaterialInstance*> DissolveMaterialInstances;

	/*
	 *队伍
	 */
	UPROPERTY(EditAnywhere)
	USkeletalMesh* RedTeamMesh;

	UPROPERTY(EditAnywhere, Category = Elim)
	TArray<UMaterialInstance*> RedDissolveMatInst;
	
	UPROPERTY(EditAnywhere)
	USkeletalMesh* BlueTeamMesh;

	UPROPERTY(EditAnywhere, Category = Elim)
	TArray<UMaterialInstance*> BlueDissolveMatInst;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* OriginalTeamMesh;
	
	/*
	 *淘汰特效
	 */

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	USoundCue* ElimbotSound;

	UPROPERTY()
	ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* CrownSystem;

	UPROPERTY()
	UNiagaraComponent* CrownComponent;

	/*
	 *默认武器
	 */

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/*
	 *离开游戏
	 */
	bool bLeftGame = false;//是否离开游戏

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;
	
///函数

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServeEquipButtonPressed();
	
	void TurnInPlace(float DeltaTime);

	void HideCameraIfCharacterClose();

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	void ElimTimerFinish();

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	UFUNCTION(Server, Reliable)
	void ServerControlJumpDistance();

	void RotateInPlace(float DeltaTime);
	
public:
	UPROPERTY(Replicated, VisibleAnywhere, Category = Character)
	bool bJump = false;

	UPROPERTY(VisibleAnywhere, Category = Character)
	bool bCanMove = true;
	

	///函数

	void SetOverlappingWeapon(class AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float NewHealth) { Health = NewHealth; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float NewShield) { Shield = NewShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE bool IsHoldingTheFlag() const;
	FORCEINLINE float GetMouseSpeed() const { return MouseSpeed; }
	FORCEINLINE void SetMouseSpeed(float NewMouseSpeed) { MouseSpeed = NewMouseSpeed; }
	ETeam GetTeam();
	bool IsLocallyReloading();
	void SetHoldingTheFlag(bool bHolding);
};
