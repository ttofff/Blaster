// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Flag.h"
#include "Character/BlasterCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AFlag::AFlag()
{
	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	SetRootComponent(FlagMesh);

	FlagMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetAreaSphere()->SetupAttachment(FlagMesh);
	GetPickupWidget()->SetupAttachment(FlagMesh);
}

void AFlag::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);//设置武器状态
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);//分离规则，保持世界坐标，并保持相对位置
	FlagMesh->DetachFromComponent(DetachRules);//分离武器
	SetOwner(nullptr);//设置拥有者为空

	BlasterOwnerController = nullptr;
	BlasterOwnerCharacter = nullptr;
}

//重置旗子位置与旋转
void AFlag::ResetFlag()
{
	ABlasterCharacter* FlagBearer = Cast<ABlasterCharacter>(GetOwner());//获取旗子拥有者
	if (FlagBearer)
	{
		FlagBearer->SetHoldingTheFlag(false);
		FlagBearer->SetOverlappingWeapon(nullptr);
		FlagBearer->UnCrouch();
	}

	if(!HasAuthority()) return;
	
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);//分离规则，保持世界坐标，并保持相对位置
	FlagMesh->DetachFromComponent(DetachRules);//分离武器

	SetWeaponState(EWeaponState::EWS_Initial);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//开启碰撞
	GetAreaSphere()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);//设置碰撞响应
	
	SetOwner(nullptr);//设置拥有者为空
	BlasterOwnerController = nullptr;
	BlasterOwnerCharacter = nullptr;

	SetActorTransform(InitialTransform);
}

void AFlag::OnEquipped()
{
	ShowPickupWidget(false);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	FlagMesh->SetSimulatePhysics(false);//关闭物理模拟
	FlagMesh->SetEnableGravity(false);//关闭重力
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FlagMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	
	EnableCustomDepth(false);// 禁用自定义深度
	GetWorldTimerManager().ClearTimer(ResetFlagTimerHandle);
}

void AFlag::OnDropped()
{
	if (HasAuthority())
	{
		GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	FlagMesh->SetSimulatePhysics(true);//开启物理模拟
	FlagMesh->SetEnableGravity(true);//开启重力
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//开启碰撞
	FlagMesh->SetCollisionResponseToAllChannels(ECR_Block);
	FlagMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	FlagMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	FlagMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	FlagMesh->MarkRenderStateDirty();//标记渲染状态
	EnableCustomDepth(true);

	GetWorldTimerManager().SetTimer(ResetFlagTimerHandle, this, &AFlag::FinishResetFlag, ResetFlagDelay, false);
}

void AFlag::BeginPlay()
{
	Super::BeginPlay();
	InitialTransform = GetActorTransform();
}

void AFlag::FinishResetFlag()
{
	OnEquipped();
	ResetFlag();
}
