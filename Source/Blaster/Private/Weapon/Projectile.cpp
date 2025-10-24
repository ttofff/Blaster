// 在 Project Settings 的 Description 页面中填写您的版权声明。


#include "Weapon/Projectile.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Blaster/Blaster.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;// 服务器和客户端同步

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);
	
}

void AProjectile::SetInitialSpeed(float Speed)
{
	
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}

	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition);// 生成子弹的轨迹
	}
}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().ClearTimer(DestroyTimer);
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectile::DestroyTimerFinished,
		DestroyTime);
}

void AProjectile::DestroyTimerFinished()
{
	Destroy();
}

//子弹击中事件
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Hit: %s"), *OtherComp->GetName()));
	
	Destroy();
}

void AProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		//产生粒子效果
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false);
	}
}

//爆炸伤害
void AProjectile::ExplodeDamage()
{
	APawn* FiringPawn = GetInstigator();//获取射击者

	if (FiringPawn && HasAuthority())
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			//在发射位置产生衰减伤害
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,//世界上下文对象
				Damage,//伤害
				10.f,//最小伤害
				GetActorLocation(),//发射位置
				DamageInnerRadius,//伤害半径
				DamageOuterRadius,//伤害衰减距离
				1.f,//伤害衰减比例
				UDamageType::StaticClass(),//伤害类型
				TArray<AActor*> (),//忽略列表
				this,//发射者
				FiringController//射击者控制器
				);
		}
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

//销毁后会在所有服务端和客户端上调用
void AProjectile::Destroyed()
{
	Super::Destroyed();
	
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());// 生成子弹击中效果
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());// 播放子弹击中声音
	}
}

