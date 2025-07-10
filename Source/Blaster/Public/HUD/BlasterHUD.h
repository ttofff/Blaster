// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT()
struct FHUDPackage
{
	GENERATED_BODY()

	UPROPERTY()
	UTexture2D* CrosshairsCenter;
	UPROPERTY()
	UTexture2D* CrosshairsLeft;
	UPROPERTY()
	UTexture2D* CrosshairsRight;
	UPROPERTY()
	UTexture2D* CrosshairsBottom;
	UPROPERTY()
	UTexture2D* CrosshairsTop;
	UPROPERTY()
	float CrosshairSpread;
	UPROPERTY()
	FLinearColor CrosshairColor;
};

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackage;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2d Spread, FLinearColor CrosshairColor);
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& InHUDPackage) { HUDPackage = InHUDPackage; }
};
