// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Blaster.h"
#include "BlasterComponents/BuffComponent.h"
#include "BlasterComponents/LagCompensationComponent.h"
#include "GameMode/BlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayerState/BlasterPlayerState.h"
#include "Components/BoxComponent.h"
#include "GameState/BlasterGameState.h"
#include "PlayerStart/TeamPlayerStart.h"
#include "Weapon/ProjectileGrenade.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 400.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; //旋转朝向运动

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);//设置可复制

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensationComponent"));
	LagCompensation->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);//设置角色碰撞类型
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);//忽略角色碰撞
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);//可见性碰撞

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 850.0f, 0.0f);//每秒旋转850度

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;//每秒网络更新频率
	MinNetUpdateFrequency = 33.f;//最小网络更新频率

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedGrenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*
	 *用于服务器命中的碰撞盒
	 */
	
	Head = CreateDefaultSubobject<UBoxComponent>(TEXT("Head"));
	Head->SetupAttachment(GetMesh(), FName("J_Bip_C_Head"));
	HitCollisionBoxes.Add(FName("Head"), Head);

	Pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("Pelvis"));
	Pelvis->SetupAttachment(GetMesh(), FName("J_Bip_C_Hips"));
	HitCollisionBoxes.Add(FName("Pelvis"), Pelvis);

	Chest = CreateDefaultSubobject<UBoxComponent>(TEXT("Chest"));
	Chest->SetupAttachment(GetMesh(), FName("J_Bip_C_UpperChest"));
	HitCollisionBoxes.Add(FName("Chest"), Chest);

	Spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_02"));
	Spine_02->SetupAttachment(GetMesh(), FName("J_Bip_C_Chest"));
	HitCollisionBoxes.Add(FName("Spine_02"), Spine_02);

	Spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_03"));
	Spine_03->SetupAttachment(GetMesh(), FName("J_Bip_C_UpperChest"));
	HitCollisionBoxes.Add(FName("Spine_03"), Spine_03);

	UpperArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArm_L"));
	UpperArm_L->SetupAttachment(GetMesh(), FName("J_Bip_L_UpperArm"));
	HitCollisionBoxes.Add(FName("UpperArm_L"), UpperArm_L);

	UpperArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArm_R"));
	UpperArm_R->SetupAttachment(GetMesh(), FName("J_Bip_R_UpperArm"));
	HitCollisionBoxes.Add(FName("UpperArm_R"), UpperArm_R);

	LowerArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerArm_L"));
	LowerArm_L->SetupAttachment(GetMesh(), FName("J_Bip_L_LowerArm"));
	HitCollisionBoxes.Add(FName("LowerArm_L"), LowerArm_L);

	LowerArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerArm_R"));
	LowerArm_R->SetupAttachment(GetMesh(), FName("J_Bip_R_LowerArm"));
	HitCollisionBoxes.Add(FName("LowerArm_R"), LowerArm_R);

	Hand_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand_L"));
	Hand_L->SetupAttachment(GetMesh(), FName("J_Bip_L_Hand"));
	HitCollisionBoxes.Add(FName("Hand_L"), Hand_L);

	Hand_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand_R"));
	Hand_R->SetupAttachment(GetMesh(), FName("J_Bip_R_Hand"));
	HitCollisionBoxes.Add(FName("Hand_R"), Hand_R);

	Thigh_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Thigh_L"));
	Thigh_L->SetupAttachment(GetMesh(), FName("J_Bip_L_UpperLeg"));
	HitCollisionBoxes.Add(FName("Thigh_L"), Thigh_L);
	
	Thigh_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Thigh_R"));
	Thigh_R->SetupAttachment(GetMesh(), FName("J_Bip_R_UpperLeg"));
	HitCollisionBoxes.Add(FName("Thigh_R"), Thigh_R);

	Calf_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Calf_L"));
	Calf_L->SetupAttachment(GetMesh(), FName("J_Bip_L_LowerLeg"));
	HitCollisionBoxes.Add(FName("Calf_L"), Calf_L);

	Calf_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Calf_R"));
	Calf_R->SetupAttachment(GetMesh(), FName("J_Bip_R_LowerLeg"));
	HitCollisionBoxes.Add(FName("Calf_R"), Calf_R);

	Foot_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Foot_L"));
	Foot_L->SetupAttachment(GetMesh(), FName("J_Bip_L_Foot"));
	HitCollisionBoxes.Add(FName("Foot_L"), Foot_L);

	Foot_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Foot_R"));
	Foot_R->SetupAttachment(GetMesh(), FName("J_Bip_R_Foot"));
	HitCollisionBoxes.Add(FName("Foot_R"), Foot_R);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	SpeedTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpeedTrailComponent"));
	SpeedTrailComponent->SetupAttachment(RootComponent);

	JumpTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("JumpTrailComponent"));
	JumpTrailComponent->SetupAttachment(RootComponent);
	
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();//销毁机器人特效
	}

	BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::MulticastGainedTheLead_Implementation()
{
	if (CrownSystem == nullptr) return;
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 80.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();//激活粒子系统
	}
}

void ABlasterCharacter::MulticastLostTheLead_Implementation()
{
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

void ABlasterCharacter::SetTeamMessage(ETeam Team)
{
	if (GetMesh() == nullptr || OriginalTeamMesh == nullptr) return;
	
	switch (Team)
	{
	case ETeam::ET_NoTeam:
		GetMesh()->SetSkeletalMeshAsset(OriginalTeamMesh);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DissolveMaterialInstances = BlueDissolveMatInst;
		break;
	case ETeam::ET_BlueTeam:
		GetMesh()->SetSkeletalMeshAsset(BlueTeamMesh);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DissolveMaterialInstances = BlueDissolveMatInst;
		break;
	case ETeam::ET_RedTeam:
		GetMesh()->SetSkeletalMeshAsset(RedTeamMesh);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DissolveMaterialInstances = RedDissolveMatInst;
		break;
	}
}

void ABlasterCharacter::DetachSpeedTrail_Implementation()
{
	SpeedTrailComponent->Deactivate();
	SpeedTrailComponent->SetActive(false);
	SpeedTrailComponent->SetAsset(nullptr);
}

void ABlasterCharacter::AttachSpeedTrail_Implementation()
{
	if (SpeedTrailSystem)
	{
		SpeedTrailComponent->SetAsset(SpeedTrailSystem);
		SpeedTrailComponent->Activate();
	}
}

void ABlasterCharacter::AttachJumpTrail_Implementation()
{
	if (JumpTrailSystem)
	{
		JumpTrailComponent->SetAsset(JumpTrailSystem);
		JumpTrailComponent->Activate();
	}
}

void ABlasterCharacter::DetachJumpTrail_Implementation()
{
	JumpTrailComponent->Deactivate();
	JumpTrailComponent->SetActive(false);
	JumpTrailComponent->SetAsset(nullptr);
}

void ABlasterCharacter::ServerDropFlag_Implementation()
{
	SetHoldingTheFlag(false);
	SetOverlappingWeapon(nullptr);
	ClientDropFlag();
	Combat->TheFlag->Dropped();
	Combat->TheFlag = nullptr;
}

void ABlasterCharacter::ClientDropFlag_Implementation()
{
	UnCrouch();
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultWeapon();

	if (bIsPose)
	{
		SetActorScale3D(FVector(0.9f));
		return;
	}
	
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}

	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABlasterCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Landed")));
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (!DynamicDissolveMaterialInstances.IsEmpty())
	{
		for (auto DynamicDissolveMaterialInstance : DynamicDissolveMaterialInstances)
		{
			DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
		}
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);//绑定UpdateDissolveMaterial函数

	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);//添加曲线
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (Combat && Combat->bHoldingTheFlag)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	if (Combat && Combat->EquippedWeapon)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	
	if (!bDisableGameplay)
	{
		AimOffset(DeltaTime);
	}
	else
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
}

void ABlasterCharacter::ServerLeaveGame_Implementation()
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
	if (BlasterGameMode && BlasterPlayerState)
	{
		BlasterGameMode->PlayerLeftGame(BlasterPlayerState);
	}
}

//同步武器
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::Action_ControllerLook(const FInputActionValue& Value)
{
	if (Controller)
	{
		FVector2D InputAxis = Value.Get<FVector2D>();
		AddControllerPitchInput(InputAxis.Y * MouseSpeed);
		AddControllerYawInput(InputAxis.X * MouseSpeed);
	}
}

void ABlasterCharacter::Action_ControllerMove(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Controller && bCanMove)
	{
		FVector2D InputAxis = Value.Get<FVector2D>();
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0,Rotation.Yaw,0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, InputAxis.Y);
		AddMovementInput(RightDirection, InputAxis.X);
	}
}

void ABlasterCharacter::Action_ControllerStartJump(const FInputActionValue& Value)
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (!bJump && !GetCharacterMovement()->IsFalling() && bCanMove && !bIsPressedJump)
	{
		bIsPressedJump = true;
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			ServerControlJumpDistance();
			
			Jump();
		}
	}
}

void ABlasterCharacter::ServerControlJumpDistance_Implementation()
{
	bJump = true;
	FVector NewVelocity = GetCharacterMovement()->Velocity;
	NewVelocity.X = NewVelocity.X/2;
	NewVelocity.Y = NewVelocity.Y/2;
	GetCharacterMovement()->Velocity = NewVelocity;
}

void ABlasterCharacter::Action_ControllerStopJump(const FInputActionValue& Value)
{
	StopJumping();
	bIsPressedJump = false;
}

//装备武器
void ABlasterCharacter::Action_ControllerEquip(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (Combat->bHoldingTheFlag)
		{
			ServerDropFlag();
			return;
		}
		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServeEquipButtonPressed();//服务器端调用
		bool bSwap = Combat->ShouldSwapWeapon() &&
			!HasAuthority() &&
				Combat->CombatState == ECombatState::ECS_Unoccupied &&
					OverlappingWeapon == nullptr;
		if (bSwap)
		{
			PlaySwapMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapon;
			bSwap = false;
		}
	}
}

//重新装弹
void ABlasterCharacter::Action_ControllerReload(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (Combat->bHoldingTheFlag) return;
		Combat->Reload();
	}
}

void ABlasterCharacter::Action_ControllerCrouch(const FInputActionValue& Value)
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::Action_ControllerAimPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (Combat->bHoldingTheFlag) return;
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::Action_ControllerAimReleased(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (Combat->bHoldingTheFlag) return;
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::Action_ControllerFirePressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (Combat->bHoldingTheFlag) return;
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::Action_ControllerFireReleased(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (Combat->bHoldingTheFlag) return;
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::Action_ControllerThrowGrenadePressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		if (Combat->bHoldingTheFlag) return;
		Combat->ThrowGrenade();
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if(Combat && Combat->EquippedWeapon == nullptr) return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.0f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(StartingAimRotation, CurrentAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if(TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if(Speed > 0.f || bIsInAir)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning; 
	}
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
	
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

//隐藏角色和武器
void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;

	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;//拥有者不显示
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = true;//拥有者不显示
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();	
	}
}

void ABlasterCharacter::ServeEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapon())
		{
			Combat->SwapWeapon();
		}
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)//先设置不显示，如果OverlappingWeapon = Weapon;不为空，就会显示。
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())//如果是本地控制（房主）
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return Combat && Combat->EquippedWeapon;
}

bool ABlasterCharacter::IsAiming()
{
	return Combat && Combat->bAiming;
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	return Combat != nullptr ? Combat->EquippedWeapon : nullptr;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if(Combat == nullptr) return FVector();

	return Combat->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	return Combat != nullptr ? Combat->CombatState : ECombatState::ECS_Max;
}

bool ABlasterCharacter::IsHoldingTheFlag() const
{
	if (Combat == nullptr) return false;
	return Combat->bHoldingTheFlag;
}

ETeam ABlasterCharacter::GetTeam()
{
	BlasterPlayerState = BlasterPlayerState == nullptr ? Cast<ABlasterPlayerState>(GetPlayerState()) : BlasterPlayerState;
	if (BlasterPlayerState == nullptr) return ETeam::ET_NoTeam;
	return BlasterPlayerState->GetTeam();
}

bool ABlasterCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

void ABlasterCharacter::SetHoldingTheFlag(bool bHolding)
{
	if (Combat == nullptr) return;
	Combat->bHoldingTheFlag = bHolding;
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPose) return;

	RotateInPlace(DeltaTime);

	HideCameraIfCharacterClose();

	RollInit();

}

// 调用以将功能绑定到输入
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//添加输入映射
	if (APlayerController* PlayerController = CastChecked<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(MappingContext,0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ControllerLook, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerLook);
		EnhancedInputComponent->BindAction(ControllerMove, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerMove);
		EnhancedInputComponent->BindAction(ControllerJump, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerStartJump);
		EnhancedInputComponent->BindAction(ControllerJump, ETriggerEvent::Completed, this, &ABlasterCharacter::Action_ControllerStopJump);
		EnhancedInputComponent->BindAction(ControllerEquip, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerEquip);
		EnhancedInputComponent->BindAction(ControllerReload, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerReload);
		EnhancedInputComponent->BindAction(ControllerCrouch, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerCrouch);
		EnhancedInputComponent->BindAction(ControllerAim, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerAimPressed);
		EnhancedInputComponent->BindAction(ControllerAim, ETriggerEvent::Completed, this, &ABlasterCharacter::Action_ControllerAimReleased);
		EnhancedInputComponent->BindAction(ControllerFire, ETriggerEvent::Started, this, &ABlasterCharacter::Action_ControllerFirePressed);
		EnhancedInputComponent->BindAction(ControllerFire, ETriggerEvent::Completed, this, &ABlasterCharacter::Action_ControllerFireReleased);
		EnhancedInputComponent->BindAction(ControllerThrowGrenade, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerThrowGrenadePressed);
		
	}
}

//获取生命周期属性
void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, Shield);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
	DOREPLIFETIME(ABlasterCharacter, bJump);
}

//初始化组件
void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed,
			GetCharacterMovement()->MaxWalkSpeedCrouched);//设置初始速度
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);//跳转到指定片段
	}
		
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);

		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);//跳转到指定片段
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void ABlasterCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void ABlasterCharacter::ElimTimerFinish()
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;//获取游戏模式
	if (BlasterGameMode && !bLeftGame)
	{
		BlasterGameMode->RequestRespawn(this, Controller);//请求复活
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
}

void ABlasterCharacter::Elim(bool bPlayerLeftGame)//本地调用
{
	DropOrDestroyWeapons();
	
	MulticastElim(bPlayerLeftGame);
	
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
		if (Combat->TheFlag)
		{
			Combat->TheFlag->Dropped();
		}
	}
}

void ABlasterCharacter::SetSpawnPoint()
{
	if (HasAuthority() && BlasterPlayerState->GetTeam() != ETeam::ET_NoTeam)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);
		TArray<ATeamPlayerStart*> TeamPlayerStarts;

		for (auto Start : PlayerStarts)
		{
			ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
			if (TeamStart && TeamStart->Team == BlasterPlayerState->GetTeam())
			{
				TeamPlayerStarts.Add(TeamStart);
			}
			if (TeamPlayerStarts.Num() > 0)
			{
				ATeamPlayerStart* ChosenPlayerStart = TeamPlayerStarts[FMath::RandRange(0, TeamPlayerStarts.Num() - 1)];
				SetActorLocationAndRotation(
					ChosenPlayerStart->GetActorLocation(),
					ChosenPlayerStart->GetActorRotation());
			}
		}
	}
}

void ABlasterCharacter::OnPlayerStateInitialized()
{
	BlasterPlayerState->AddToScore(0.f);//初始化分数
	BlasterPlayerState->AddToDefeats(0);//初始化击败数
	UpdateHUDAmmo();
	if (Combat)
	{
		Combat->SetGrenades(4);
		Combat->UpdateHUDGrenade();
	}
	
	SetTeamMessage(BlasterPlayerState->GetTeam());
	SetSpawnPoint();
}

//玩家被淘汰
void ABlasterCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)//服务器调用
{
	bLeftGame = bPlayerLeftGame;
	
	bElimmed = true;
	PlayElimMontage();

	//开始溶解
	if (!DissolveMaterialInstances.IsEmpty())
	{
		for (int32 i = 0; i < DissolveMaterialInstances.Num(); i++)
		{
			DynamicDissolveMaterialInstances[i] = UMaterialInstanceDynamic::Create(DissolveMaterialInstances[i], this);//创建动态材质
			GetMesh()->SetMaterial(i, DynamicDissolveMaterialInstances[i]);
			DynamicDissolveMaterialInstances[i]->SetScalarParameterValue(TEXT("Dissolve"), .55f);
			DynamicDissolveMaterialInstances[i]->SetScalarParameterValue(TEXT("Glow"), 100.f);
		}
	}
	StartDissolve();

	//禁用增强输入系统
	// if (BlasterPlayerController)
	// {
	// 	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BlasterPlayerController->GetLocalPlayer()))
	// 	{
	// 		EnhancedInputLocalPlayerSubsystem->RemoveMappingContext(MappingContext);
	// 	}
	// }
	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();//禁用移动

	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}

	//禁用碰撞
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//生成淘汰机器人
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);//设置机器人生成位置
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation());//生成机器人特效
	}
	if (ElimbotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimbotSound,
			GetActorLocation());//生成机器人声音
	}

	bool bHideSniperScope = IsLocallyControlled() &&
		Combat &&
			Combat->bAiming &&
				Combat->EquippedWeapon &&
					Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
	
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
	
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ABlasterCharacter::ElimTimerFinish,
		ElimDelay);
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);//跳转到指定片段
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatorController, AActor* DamageCauser)
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;//获取游戏模式
	if (bElimmed || BlasterGameMode == nullptr) return;

	Damage = BlasterGameMode->CalculateDamage(InstigatorController, Controller, Damage);

	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - DamageToHealth, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, MaxHealth);
			Shield = 0.f;
		}
	}
	
	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	
	UpdateHUDHealth();
	UpdateHUDShield();
	
	PlayHitReactMontage();

	if (Health <= 0.f)
	{
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;

			AProjectileGrenade* ProjectileGrenade = Cast<AProjectileGrenade>(DamageCauser);
			if (ProjectileGrenade && !ProjectileGrenade->bLauncherFire)BlasterPlayerController->bDeadGrenade = true;
				else BlasterPlayerController->bDeadGrenade = false;
			APlayerController* AttackerController = Cast<APlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
	
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);	
	}
}

void ABlasterCharacter::UpdateHUDAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(GetController()) : BlasterPlayerController;
	if (BlasterPlayerController && Combat && Combat->EquippedWeapon)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
		
	}
}

void ABlasterCharacter::SpawnDefaultWeapon()
{
	BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;//获取游戏模式
	UWorld* World = GetWorld();
	if ((BlasterGameMode && World && !bElimmed && DefaultWeaponClass) || bIsPose)
	{
		AWeapon* StartingWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->SetActorScale3D(FVector(0.8f));
		StartingWeapon->bDestroyWeapon = true;//设置默认武器销毁
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void ABlasterCharacter::RollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			OnPlayerStateInitialized();

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(BlasterPlayerState))
			{
				MulticastGainedTheLead();
			}
		}
	}
}

