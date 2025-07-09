// 在 Project Settings 的 Description 页面中填写您的版权声明。

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/Weapon.h"
#include "PlayerController/BlasterPlayerController.h"
#include "HUD/BlasterHUD.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	friend class ABlasterCharacter;//友元类

	void EquipWeapon(AWeapon* WeaponToEquip);
	
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServeFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCursor(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);
	
private:
	UPROPERTY()
	class ABlasterCharacter* Character;//友元类可以访问

	UPROPERTY()
	class ABlasterPlayerController* Controller;
	
	UPROPERTY()
	class ABlasterHUD* HUD;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	bool bFireButtonPressed;

	/**
	 *HUD 和 Crosshairs
	 */

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;

	FVector HitTarget;
public:	
	

		
};
