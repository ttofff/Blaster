// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumOfPublicConnections = 4, FString MatchOfType = TEXT("FreeForAll"), FString LobbyPath = TEXT("/Game/ThirdPerson/Maps/Lobby"));

	UFUNCTION(BlueprintImplementableEvent)
	void SetNumPlayerBoxVisible(bool bVisible);
protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//回调函数
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& Results, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type SessionResults);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ReturnButton;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CreateRoom;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	UFUNCTION()
	void ConfirmButtonClicked();
	
	UFUNCTION()
	void ReturnButtonClicked();

	void MenuTearDown();

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 NumPublicConnections{4};

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString MatchType{TEXT("FreeForAll")};
	
	FString PathToLobby{TEXT("")};

	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	bool bIsCreate = false;
};
