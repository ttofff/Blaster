// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"
#include "BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);//忽略摄像机碰撞
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);//忽略角色碰撞

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 850.0f, 0.0f);//每秒旋转850度

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;//每秒网络更新频率
	MinNetUpdateFrequency = 33.f;//最小网络更新频率
	
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

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
		AddControllerPitchInput(InputAxis.Y);
		AddControllerYawInput(InputAxis.X);
	}
}

void ABlasterCharacter::Action_ControllerMove(const FInputActionValue& Value)
{
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
	if (!bJump && !GetCharacterMovement()->IsFalling() && bCanMove && !bIsPressedJump)
	{
		bIsPressedJump = true;
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			bJump = true;

			FVector NewVelocity = GetCharacterMovement()->Velocity;
			NewVelocity.X = NewVelocity.X/2;
			NewVelocity.Y = NewVelocity.Y/2;
			GetCharacterMovement()->Velocity = NewVelocity;
	
			Jump();
		}
	}
}

void ABlasterCharacter::Action_ControllerStopJump(const FInputActionValue& Value)
{
	StopJumping();
	bIsPressedJump = false;
}

void ABlasterCharacter::Action_ControllerEquip(const FInputActionValue& Value)
{
	if (Combat)
	{
		if (HasAuthority())//HasAuthority()判断是否是房主
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServeEquipButtonPressed();//服务器端调用
		}
	}
}

void ABlasterCharacter::Action_ControllerCrouch(const FInputActionValue& Value)
{
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
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::Action_ControllerAimReleased(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::Action_ControllerFirePressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::Action_ControllerFireReleased(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
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

void ABlasterCharacter::ServeEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
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

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);

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
		EnhancedInputComponent->BindAction(ControllerCrouch, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerCrouch);
		EnhancedInputComponent->BindAction(ControllerAim, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerAimPressed);
		EnhancedInputComponent->BindAction(ControllerAim, ETriggerEvent::Completed, this, &ABlasterCharacter::Action_ControllerAimReleased);
		EnhancedInputComponent->BindAction(ControllerFire, ETriggerEvent::Triggered, this, &ABlasterCharacter::Action_ControllerFirePressed);
		EnhancedInputComponent->BindAction(ControllerFire, ETriggerEvent::Completed, this, &ABlasterCharacter::Action_ControllerFireReleased);
		
	}
}

//获取生命周期属性
void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

//初始化组件
void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
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

