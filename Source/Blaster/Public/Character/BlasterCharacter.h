// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

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

	void PlayFireMontage(bool bAiming);
	
protected:
	virtual void BeginPlay() override;

	void Action_ControllerLook(const FInputActionValue& Value);
	void Action_ControllerMove(const FInputActionValue& Value);
	void Action_ControllerStartJump(const FInputActionValue& Value);
	void Action_ControllerStopJump(const FInputActionValue& Value);
	void Action_ControllerEquip(const FInputActionValue& Value);
	void Action_ControllerCrouch(const FInputActionValue& Value);
	void Action_ControllerAimPressed(const FInputActionValue& Value);
	void Action_ControllerAimReleased(const FInputActionValue& Value);
	void Action_ControllerFirePressed(const FInputActionValue& Value);
	void Action_ControllerFireReleased(const FInputActionValue& Value);

	void AimOffset(float DeltaTime);
	
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
	TObjectPtr<UInputAction> ControllerAim;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput | Combat")
	TObjectPtr<UInputAction> ControllerFire;

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

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation = FRotator(0.f, -90.f, 0.f);

	ETurningInPlace TurningInPlace;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
///函数

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServeEquipButtonPressed();
	
	void TurnInPlace(float DeltaTime);

	void HideCameraIfCharacterClose();
	
public:
	UPROPERTY(VisibleAnywhere, Category = Character)
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
};
