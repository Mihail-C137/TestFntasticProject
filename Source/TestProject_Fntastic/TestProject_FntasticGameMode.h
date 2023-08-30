// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestProject_FntasticGameMode.generated.h"

class ATestProject_FntasticCharacter;

UENUM()
enum class EBotState : uint8
{
	ENABLE,
	DISABLE
};

UCLASS(minimalapi)
class ATestProject_FntasticGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATestProject_FntasticGameMode();

	void AddBot(ATestProject_FntasticCharacter* Bot);
	void TurnBot(const bool& Value);
	void DeleteBot(ATestProject_FntasticCharacter* Bot);

	bool IsEnableAI();

	FORCEINLINE TArray<ATestProject_FntasticCharacter*> GetBots() const { return Bots; }

private:
	EBotState BotState = EBotState::ENABLE;
	TArray<ATestProject_FntasticCharacter*> Bots;
};



