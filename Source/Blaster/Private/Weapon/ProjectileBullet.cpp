// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileBullet.h"
#include "BlasterComponents/LagCompensationComponent.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);//启用子弹的同步
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
	
}

void AProjectileBullet::SetInitialSpeed(float Speed)
{
	Super::SetInitialSpeed(Speed);

	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * Speed; // 立即生效
}

#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if(PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))//如果修改了InitialSpeed属性
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = InitialSpeed;
			ProjectileMovementComponent->MaxSpeed = InitialSpeed;
		}
	}
}
#endif


void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerCharacter->Controller);
        if (OwnerController)
        {
	        if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
	        {
	        	const float DamageToCause = Hit.BoneName.ToString() == FString("J_Bip_C_Head") ? HeadShotDamage : Damage;
	        	
	        	UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, OwnerController, this, UDamageType::StaticClass());
	        	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
				return;
	        }

        	ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OtherActor);
	        if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
	        {
		        OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
		        	HitCharacter,
		        	TraceStart,
		        	InitialVelocity,
		        	OwnerController->GetServerTime() - OwnerController->SingleTripTime);
	        }
        }
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	/*
	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithChannel = true;//启用通道检测
	PathParams.bTraceWithCollision = true;//启用碰撞检测
	PathParams.OverrideGravityZ = 0.f;//覆盖重力
	PathParams.DrawDebugTime = 5.0f;//绘制调试时间
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;//绘制调试类型,持续绘制
	PathParams.LaunchVelocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;//发射速度
	PathParams.MaxSimTime = 4.f;//最大模拟时间
	PathParams.ProjectileRadius = 5.f;//子弹半径
	PathParams.SimFrequency = 30.f;//模拟频率
	PathParams.StartLocation = GetActorLocation();//起始位置
	PathParams.TraceChannel = ECC_Visibility;//通道类型
	PathParams.ActorsToIgnore.Add(this);//忽略自身

	FPredictProjectilePathResult PathResult;

	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);//预测子弹路径
	*/
}
