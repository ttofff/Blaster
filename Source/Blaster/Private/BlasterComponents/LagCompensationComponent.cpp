// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/LagCompensationComponent.h"
#include "Character/BlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Blaster.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

//保存帧数据
void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : Character;
	if (Character)
	{
		Package.Time = GetWorld()->TimeSeconds;
		Package.Character = Character;
		
		for (auto BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();//获取盒体位置
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();//获取盒体旋转
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();//获取盒体大小
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}

//在两个关键帧之间进行插值计算，用于网络延迟补偿
FFramePackage ULagCompensationComponent::InterpBetweenFrames(const FFramePackage& OlderFrame,
	const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFraction = FMath::Clamp((HitTime - OlderFrame.Time) / Distance, 0.f, 1.f);//插值比例
	FFramePackage InterpFramePackage;
	InterpFramePackage.Time = HitTime;

	for (auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;

		const FBoxInformation& OlderBoxInfo = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBoxInfo = YoungerFrame.HitBoxInfo[BoxInfoName];

		FBoxInformation InterpBoxInfo;

		InterpBoxInfo.Location = FMath::Lerp(OlderBoxInfo.Location, YoungerBoxInfo.Location, InterpFraction);
		InterpBoxInfo.Rotation = FMath::Lerp(OlderBoxInfo.Rotation, YoungerBoxInfo.Rotation, InterpFraction);
		InterpBoxInfo.BoxExtent = FMath::Lerp(OlderBoxInfo.BoxExtent, YoungerBoxInfo.BoxExtent, InterpFraction);

		InterpFramePackage.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}

	return InterpFramePackage;
}

FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& Package,
	ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if (HitCharacter == nullptr) return FServerSideRewindResult();

	FFramePackage CurrentFramePackage;
	CacheBoxPosition(HitCharacter, CurrentFramePackage);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);//禁用头部碰撞

	//启用头部碰撞
	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("Head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);

	FHitResult ConfirmHitResult;
	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	UWorld* World = GetWorld();
	if (World)
	{
		World->LineTraceSingleByChannel(
			ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_HitBox);
		
		if (ConfirmHitResult.bBlockingHit)
		{
			if (ConfirmHitResult.Component.IsValid())
			{
				ResetHitBoxes(HitCharacter, CurrentFramePackage);
				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);//启用头部碰撞
				return FServerSideRewindResult {true, true};
			}
		}
		else
		{
			for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
			{
				if (HitBoxPair.Value != nullptr)
				{
					HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
				}
			}
			World->LineTraceSingleByChannel(
				ConfirmHitResult,
				TraceStart,
				TraceEnd,
				ECC_HitBox);
			if (ConfirmHitResult.bBlockingHit)
			{
				ResetHitBoxes(HitCharacter, CurrentFramePackage);
				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);//启用头部碰撞
				return FServerSideRewindResult {true, false};
			}
		}
	}

	ResetHitBoxes(HitCharacter, CurrentFramePackage);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);//启用头部碰撞
	return FServerSideRewindResult {false, false};
}

FServerSideRewindResult ULagCompensationComponent::ProjectileConfirmHit(const FFramePackage& Package,ABlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100 InitialVelocity, float HitTime)
{
	FFramePackage CurrentFramePackage;
	CacheBoxPosition(HitCharacter, CurrentFramePackage);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);//禁用头部碰撞

	//启用头部碰撞
	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("Head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);

	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithCollision = true;
	PathParams.MaxSimTime = MaxRecordTime;
	PathParams.LaunchVelocity = InitialVelocity;
	PathParams.StartLocation = TraceStart;
	PathParams.SimFrequency = 15.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.TraceChannel = ECC_HitBox;
	PathParams.ActorsToIgnore.Add(GetOwner());

	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);//预测子弹路径

	if (PathResult.HitResult.bBlockingHit)//击中头部
	{
		ResetHitBoxes(HitCharacter, CurrentFramePackage);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);//启用头部碰撞
		return FServerSideRewindResult {true, true};
	}
	else//击中身体
	{
		for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
		{
			if (HitBoxPair.Value != nullptr)
			{
				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			}
		}

		UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);//预测子弹路径
		if (PathResult.HitResult.bBlockingHit)
		{
			ResetHitBoxes(HitCharacter, CurrentFramePackage);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);//启用头部碰撞
			return FServerSideRewindResult {true, false};
		}
	}

	//没有击中角色
	ResetHitBoxes(HitCharacter, CurrentFramePackage);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);//启用头部碰撞
	return FServerSideRewindResult {false, false};
}

FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunConfirmHit(const TArray<FFramePackage>& FramesPackages,
                                                                            const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations)
{
	for (auto& Frame : FramesPackages)
	{
		if (Frame.Character == nullptr) return FShotgunServerSideRewindResult();
	}
	FShotgunServerSideRewindResult ShotgunResult;
	TArray<FFramePackage> CurrentFrames;
	for (auto& Frame : FramesPackages)
	{
		FFramePackage CurrentFramePackage;
		CurrentFramePackage.Character = Frame.Character;
		CacheBoxPosition(Frame.Character, CurrentFramePackage);
		MoveBoxes(Frame.Character, Frame);
		EnableCharacterMeshCollision(Frame.Character, ECollisionEnabled::NoCollision);//禁用头部碰撞
	}
	for (auto& Frame : FramesPackages)
	{
		//启用头部碰撞
		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("Head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
	}
	UWorld* World = GetWorld();
	//检查爆头
	for (auto& HitLocation : HitLocations)
	{
		FHitResult ConfirmHitResult;
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
		if (World)
		{
			World->LineTraceSingleByChannel(
			ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_HitBox);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ConfirmHitResult.GetActor());
			if (BlasterCharacter)
			{
				if (ConfirmHitResult.bBlockingHit)
				{
					if (ShotgunResult.HeadShots.Contains(BlasterCharacter))
					{
						ShotgunResult.HeadShots[BlasterCharacter]++;
					}
					else
					{
						ShotgunResult.HeadShots.Emplace(BlasterCharacter, 1);
					}
				}
			}
		}
	}

	for (auto& Frame : FramesPackages)
	{
		for (auto& HitBoxPair : Frame.Character->HitCollisionBoxes)
		{
			if (HitBoxPair.Value != nullptr)
			{
				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			}
		}
		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("Head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//检查身体
	for (auto& HitLocation : HitLocations)
	{
		FHitResult ConfirmHitResult;
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
		if (World)
		{
			World->LineTraceSingleByChannel(
			ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_HitBox);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ConfirmHitResult.GetActor());
			if (BlasterCharacter)
			{
				if (ConfirmHitResult.bBlockingHit)
				{
					if (ShotgunResult.BodyShots.Contains(BlasterCharacter))
					{
						ShotgunResult.BodyShots[BlasterCharacter]++;
					}
					else
					{
						ShotgunResult.BodyShots.Emplace(BlasterCharacter, 1);
					}
				}
			}
		}
	}

	for (auto& Frame : CurrentFrames)
	{
		ResetHitBoxes(Frame.Character, Frame);
		EnableCharacterMeshCollision(Frame.Character, ECollisionEnabled::QueryAndPhysics);//启用头部碰撞
	}

	return ShotgunResult;
}

void ULagCompensationComponent::CacheBoxPosition(ABlasterCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	if (HitCharacter == nullptr) return;

    for (auto BoxPair : HitCharacter->HitCollisionBoxes)
    {
	    if (BoxPair.Value != nullptr)
	    {
	    	FBoxInformation BoxInfo;
	    	BoxInfo.Location = BoxPair.Value->GetComponentLocation();
	    	BoxInfo.Rotation = BoxPair.Value->GetComponentRotation();
	    	BoxInfo.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
	    	OutFramePackage.HitBoxInfo.Add(BoxPair.Key, BoxInfo);
	    }
    }
}

void ULagCompensationComponent::MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if (HitCharacter == nullptr) return;

    for (auto BoxPair : HitCharacter->HitCollisionBoxes)
    {
        if (BoxPair.Value != nullptr)
        {
            BoxPair.Value->SetWorldLocation(Package.HitBoxInfo[BoxPair.Key].Location);//设置盒体位置
        	BoxPair.Value->SetWorldRotation(Package.HitBoxInfo[BoxPair.Key].Rotation);//设置盒体旋转
        	BoxPair.Value->SetBoxExtent(Package.HitBoxInfo[BoxPair.Key].BoxExtent);//设置盒体大小
        }
    }
}

void ULagCompensationComponent::ResetHitBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if (HitCharacter == nullptr) return;

    for (auto BoxPair : HitCharacter->HitCollisionBoxes)
    {
        if (BoxPair.Value != nullptr)
        {
            BoxPair.Value->SetWorldLocation(Package.HitBoxInfo[BoxPair.Key].Location);//设置盒体位置
            BoxPair.Value->SetWorldRotation(Package.HitBoxInfo[BoxPair.Key].Rotation);//设置盒体旋转
            BoxPair.Value->SetBoxExtent(Package.HitBoxInfo[BoxPair.Key].BoxExtent);//设置盒体大小
        	BoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);//禁用碰撞
        }
    }
}

void ULagCompensationComponent::EnableCharacterMeshCollision(ABlasterCharacter* HitCharacter,
	ECollisionEnabled::Type CollisionEnabled)
{
	if (HitCharacter && HitCharacter->GetMesh())
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnabled);
	}
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color)
{
	for (auto BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			false,
			4.f);
	}
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(class ABlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);

	return ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
}

FServerSideRewindResult ULagCompensationComponent::ProjectileServerSideRewind(ABlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100 InitialVelocity, float HitTime)
{
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return ProjectileConfirmHit(FrameToCheck, HitCharacter, TraceStart, InitialVelocity, HitTime);
}

FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunServerSideRewind(
	const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	TArray<FFramePackage> FramesToCheck;

	for (auto HitCharacter : HitCharacters)
	{
		FramesToCheck.Add(GetFrameToCheck(HitCharacter, HitTime));
	}

	return ShotgunConfirmHit(FramesToCheck, TraceStart, HitLocations);
}

FFramePackage ULagCompensationComponent::GetFrameToCheck(ABlasterCharacter* HitCharacter, float HitTime)
{
	bool bReturn =
		HitCharacter == nullptr ||
			HitCharacter->GetLagCompensation() == nullptr ||
				HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
					HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;

	if (bReturn) return FFramePackage();
	
	FFramePackage FrameToCheck = FFramePackage();//要检查的帧
	bool bShouldInterpolate = true;//是否需要插值

	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;

	if (OldestHistoryTime > HitTime)
	{
		return FFramePackage();
	}
	if (OldestHistoryTime == HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldInterpolate = false;
	}
	if (NewestHistoryTime <= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
	}

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;
	while (Older->GetValue().Time > HitTime)
	{
		//直到OlderTime < HitTime < YoungerTime
		if (Older->GetNextNode() == nullptr) break;
		Older = Older->GetNextNode();
		if (Older->GetValue().Time > HitTime)
		{
			Younger = Older;
		}
	}
	if (Older->GetValue().Time == HitTime)
	{
		FrameToCheck = Older->GetValue();
		bShouldInterpolate = false;
	}
	if (bShouldInterpolate)
	{
		FrameToCheck = InterpBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
	}

	FrameToCheck.Character = HitCharacter;
	
	return FrameToCheck;
}

void ULagCompensationComponent::ServerScoreRequest_Implementation(ABlasterCharacter* HitCharacter,
                                                                  const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	FServerSideRewindResult Confirm = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);

	if (Character && HitCharacter && Character->GetEquippedWeapon() && Confirm.bHitConfirmed)
	{
		const float Damage = Confirm.bHeadShot ? Character->GetEquippedWeapon()->GetHeadShotDamage() : Character->GetEquippedWeapon()->GetDamage();
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			Damage,
			Character->Controller,
			Character->GetEquippedWeapon(),
			UDamageType::StaticClass()
			);
	}
}

void ULagCompensationComponent::ProjectileServerScoreRequest_Implementation(ABlasterCharacter* HitCharacter,const FVector_NetQuantize& TraceStart,
const FVector_NetQuantize100 InitialVelocity,float HitTime)
{
	FServerSideRewindResult Confirm = ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime);

	if (Character && HitCharacter && Confirm.bHitConfirmed && Character->GetEquippedWeapon())
	{
		const float Damage = Confirm.bHeadShot ? Character->GetEquippedWeapon()->GetHeadShotDamage() : Character->GetEquippedWeapon()->GetDamage();
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			Damage,
			Character->Controller,
			Character->GetEquippedWeapon(),
			UDamageType::StaticClass()
			);
	}
}

void ULagCompensationComponent::ShotgunServerScoreRequest_Implementation(
	const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	FShotgunServerSideRewindResult Confirm = ShotgunServerSideRewind(HitCharacters, TraceStart, HitLocations, HitTime);

	for (auto& HitCharacter : HitCharacters)
	{
		//if (HitCharacter == nullptr || HitCharacter->GetEquippedWeapon() || Character == nullptr) continue;
		if (HitCharacter  == nullptr) continue;
		if (Character == nullptr) continue;
		if (HitCharacter->GetEquippedWeapon() == nullptr) continue;
		float TotalDamage = 0.f;//总伤害
		if (Confirm.HeadShots.Contains(HitCharacter))
		{
			float HeadDamage = Confirm.HeadShots[HitCharacter] * HitCharacter->GetEquippedWeapon()->GetHeadShotDamage();
			TotalDamage += HeadDamage;
		}
		if (Confirm.BodyShots.Contains(HitCharacter))
		{
			float BodyDamage = Confirm.BodyShots[HitCharacter] * HitCharacter->GetEquippedWeapon()->GetDamage();
            TotalDamage += BodyDamage;
		}
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			TotalDamage,
			Character->Controller,
			Character->GetEquippedWeapon(),
			UDamageType::StaticClass()
			);
	}

}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickSaveFramePackage();
}

void ULagCompensationComponent::TickSaveFramePackage()
{
	if (Character == nullptr || !Character->HasAuthority()) return;
	
	if (FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryTime = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryTime > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryTime = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
		
		//ShowFramePackage(ThisFrame, FColor::Red);
	}
}
