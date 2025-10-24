// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "IntoMapTime.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UIntoMapTime : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WaitingText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WaitingTime;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ExplainText;
	
	UPROPERTY(EditAnywhere)
	TArray<FText> ExplainTexts;

	UPROPERTY(EditAnywhere)
	TArray<FText> WaitingTexts;
};
