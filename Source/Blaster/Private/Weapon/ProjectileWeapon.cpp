// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));//获取枪口骨骼
	UWorld* World = GetWorld();
	if (MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();//计算枪口位置到击中位置的方向向量
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
		
		if (bUseServerSideRewind)
		{
			if (InstigatorPawn->HasAuthority())
			{
				if (InstigatorPawn->IsLocallyControlled())
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams);
					if (SpawnProjectile)
					{
						SpawnProjectile->SetInitialSpeed(UpdateProjectileSpeed(SocketTransform, HitTarget));//初始化UProjectileMovementComponent的速度
						SpawnProjectile->bUseServerSideRewind = false;
						SpawnProjectile->Damage = Damage;
						SpawnProjectile->HeadShotDamage = HeadShotDamage;
					}
				}
				else
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(
                        ServerSideRewindProjectileClass,
                        SocketTransform.GetLocation(),
                        TargetRotation,
                        SpawnParams);
                    if (SpawnProjectile)
                    {
                    	SpawnProjectile->SetInitialSpeed(UpdateProjectileSpeed(SocketTransform, HitTarget));//初始化UProjectileMovementComponent的速度
                    	SpawnProjectile->bUseServerSideRewind = true;
                    }
				}
			}
			else
			{
				if (InstigatorPawn->IsLocallyControlled())
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(
					ServerSideRewindProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams);
					if (SpawnProjectile)
					{
						SpawnProjectile->SetInitialSpeed(UpdateProjectileSpeed(SocketTransform, HitTarget));//初始化UProjectileMovementComponent的速度
						SpawnProjectile->bUseServerSideRewind = true;
						SpawnProjectile->TraceStart = SocketTransform.GetLocation();
						SpawnProjectile->InitialVelocity =SpawnProjectile->GetActorForwardVector() * SpawnProjectile->InitialSpeed;
					}
				}
				else
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(
						ServerSideRewindProjectileClass,
						SocketTransform.GetLocation(),
						TargetRotation,
						SpawnParams);
					if (SpawnProjectile)
					{
						SpawnProjectile->SetInitialSpeed(UpdateProjectileSpeed(SocketTransform, HitTarget));//初始化UProjectileMovementComponent的速度
						SpawnProjectile->bUseServerSideRewind = false;
					}
				}
			}
		}
		else
		{
			if (InstigatorPawn->HasAuthority())
			{
				SpawnProjectile = World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams);
				if (SpawnProjectile)
				{
					SpawnProjectile->SetInitialSpeed(UpdateProjectileSpeed(SocketTransform, HitTarget));//初始化UProjectileMovementComponent的速度
					SpawnProjectile->bUseServerSideRewind = false;
					SpawnProjectile->Damage = Damage;
					SpawnProjectile->HeadShotDamage = HeadShotDamage;
				}
			}
		}
	}
}

float AProjectileWeapon::UpdateProjectileSpeed(FTransform SocketTransform, FVector HitTarget)
{
	float Distance = FVector::Distance(SocketTransform.GetLocation(), HitTarget);
	float Speed = 15000.f;
			
	if (Distance < 100.f)
	{
		Speed = 1200.f;
	}
	else if (Distance < 200.f)
	{
		Speed = 2000.f;
	}
	else if (Distance < 500.f)
	{
		Speed = 5000.f;
	}
	else if (Distance < 800.f)
	{
		Speed = 10000.f;
	}
	
	return Speed;
}
