// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CPP_HelperFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_FNTASTIC_API UCPP_HelperFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool IsEnemy(AActor* ThisActor, AActor* EnemyActor);
	static bool IsAlive(AActor* EnemyActor);

};
