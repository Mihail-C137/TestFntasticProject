// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestProject_FntasticGameMode.h"
#include "TestProject_FntasticCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATestProject_FntasticGameMode::ATestProject_FntasticGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void ATestProject_FntasticGameMode::AddBot(ATestProject_FntasticCharacter* Bot)
{
	Bots.Emplace(Bot);
}

void ATestProject_FntasticGameMode::TurnBot(const bool& Value)
{
	for (ATestProject_FntasticCharacter* Bot : Bots)
	{
		Bot->TurnLogicAI(Value);
	}

	BotState = Value ? EBotState::ENABLE : EBotState::DISABLE;
}

void ATestProject_FntasticGameMode::DeleteBot(ATestProject_FntasticCharacter* Bot)
{
	Bots.Remove(Bot);
}

bool ATestProject_FntasticGameMode::IsEnableAI()
{
	return BotState == EBotState::ENABLE ? true : false;
}
