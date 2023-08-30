// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_HelperFunctionLibrary.h"
#include "TestProject_Fntastic/TestProject_FntasticCharacter.h"

bool UCPP_HelperFunctionLibrary::IsEnemy(AActor* ThisActor, AActor* EnemyActor)
{
	if (ThisActor && EnemyActor)
	{
		ATestProject_FntasticCharacter* Character = Cast<ATestProject_FntasticCharacter>(ThisActor);
		ATestProject_FntasticCharacter* Enemy = Cast<ATestProject_FntasticCharacter>(EnemyActor);

		if (Character && Enemy)
		{
			return Character->GetCharacterType() == Enemy->GetCharacterType();
		}
	}
	return false;
}

bool UCPP_HelperFunctionLibrary::IsAlive(AActor* EnemyActor)
{
	if (EnemyActor)
	{
		ATestProject_FntasticCharacter* Enemy = Cast<ATestProject_FntasticCharacter>(EnemyActor);

		if (Enemy)
		{
			return Enemy->IsCharacterAlive();
		}
	}
	return false;
}
