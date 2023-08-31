// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CPP_AIController_Character.generated.h"

DECLARE_DELEGATE(FOnCharacterAIStateChange)

UENUM()
enum class ECharacterAIState : uint8
{
	PATROLLING UMETA(DisplayName = "Patrolling"),
	INVESTIGATE UMETA(DisplayName = "Investigate"),	
	PERSECUTION UMETA(DisplayName = "Persecution"),
	DISABLE UMETA(DisplayName = "Disable")
};

UCLASS()
class TESTPROJECT_FNTASTIC_API ACPP_AIController_Character : public AAIController
{
	GENERATED_BODY()

public:
	ACPP_AIController_Character();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY()
	class ATestProject_FntasticCharacter* ControlledCharacter{ nullptr };

	UPROPERTY()
	class AActor* EnemyCharacter{ nullptr };	

	UPROPERTY()
	class UCPP_StateWidget* StateWidget{ nullptr };	

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Sense)
	double PatrollingAreaSearch{ 2000.0 };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Sense)
	double RandomAreaSearch{ 1000.0 };
	/*Maximum range to search damage trigger */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Sense)
	double DistanceShoot{ 10000.0 };
	/*Random area search damage trigger*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Sense)
	double ShootArea{ 500.0 };
	/*Timers*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Sense)
	float RandomTimerPatrolling{ 5.f };

	/*Speed for difference state*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Sense)
	float PatrollingSpeed{ 200.f };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Sense)
	float InvestigateSpeed{ 300.f };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Sense)
	float PersecutionSpeed{ 500.f };
	
	double DistanceSight{ 0.0 };	
	double DistanceHear{ 0.0 };
	double LoseAreaSight{ 0.0 };

	FTimerHandle PatrollingTimerHandle;
	FOnCharacterAIStateChange OnCharacterAIStateChange;

	FVector StartPoint{ 0.0 };
	FVector SearchPoint{ 0.0 };	

	ECharacterAIState CharacterAIState = ECharacterAIState::PATROLLING;

	EPathFollowingRequestResult::Type ResultData = EPathFollowingRequestResult::Type::AlreadyAtGoal;

	bool bIsSetTimer{ false };

	UFUNCTION()
	void OnTargetPerceprtionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	void SetStartedEssentials();
	void GetPerceptionValues();

	void OnPatrolling();
	void OnInvestigate();
	void OnPersecution();	

	void OnSight(AActor* SightActor, const bool& IsSight);
	void OnHear(const FVector& StimulusLocation);	
	void OnDamage(const FVector& StimulusLocation);

	const FVector GetRandomPointInArea(const FVector& PointSearch, const double& AreaSearch);
	const FVector GetPointForInvestigate(const FVector& PointSearch, const double& AreaSearch);

	UFUNCTION()
	void CheckCharacterAIState();

	void SetRandomPatrolPoint();	
	
public:
	FORCEINLINE void SetStateWidget(class UCPP_StateWidget* StateWidgetPtr) { StateWidget = StateWidgetPtr; }
	FORCEINLINE class UCPP_StateWidget* GetStateWidget() const { return StateWidget; }

	void TurnLogic(const bool& Value);
	void SetPerceptionEnable(const bool& Value);

	void SetTextStateBot();
};
