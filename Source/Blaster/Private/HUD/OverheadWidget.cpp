// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();//获取远程角色
	FString Role;
	switch (RemoteRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");//模拟代理
		break;
	case ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");//自主代理
		break;
	case ROLE_Authority:
		Role = FString("Authority");//权限
		break;
	default:
		Role = FString("ERROR");
		break;
	}

	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
