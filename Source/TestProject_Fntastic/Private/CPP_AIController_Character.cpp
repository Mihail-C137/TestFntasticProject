// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_AIController_Character.h"

#include "TestProject_Fntastic/TestProject_FntasticCharacter.h"
#include "TestProject_Fntastic/Public/CPP_HelperFunctionLibrary.h"
#include "TestProject_Fntastic/Public/CPP_StateWidget.h"
#include "TestProject_Fntastic/Public/CPP_CharacterWidgetComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"

ACPP_AIController_Character::ACPP_AIController_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

void ACPP_AIController_Character::BeginPlay()
{
	Super::BeginPlay();

	PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ACPP_AIController_Character::OnTargetPerceprtionUpdated);

	SetStartedEssentials();
}

void ACPP_AIController_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	switch (CharacterAIState)
	{
		case ECharacterAIState::PATROLLING:
		{
			OnPatrolling();
			break;
		}
		case ECharacterAIState::INVESTIGATE:
		{
			OnInvestigate();
			break;
		}
		case ECharacterAIState::PERSECUTION:
		{
			OnPersecution();
			break;
		}
	}	
}

void ACPP_AIController_Character::OnTargetPerceprtionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UCPP_HelperFunctionLibrary::IsEnemy(GetPawn(), Actor)
		|| !UCPP_HelperFunctionLibrary::IsAlive(Actor)) return;
	
	UAISenseConfig* Config = PerceptionComponent->GetSenseConfig(Stimulus.Type);

	if (Config->IsA(UAISenseConfig_Sight::StaticClass()))
	{		
		OnSight(Actor, Stimulus.IsActive());
	}
	else if (Config->IsA(UAISenseConfig_Hearing::StaticClass()))
	{
		OnHear(Stimulus.StimulusLocation);
	}
	else if (Config->IsA(UAISenseConfig_Damage::StaticClass()))
	{
		OnDamage(Stimulus.StimulusLocation);
	}
}

void ACPP_AIController_Character::SetStartedEssentials()
{
	ControlledCharacter = Cast<ATestProject_FntasticCharacter>(GetPawn());

	if (ControlledCharacter)
	{
		ControlledCharacter->SetCharacterType(ECharacterType::BOT);

		StartPoint = ControlledCharacter->GetActorLocation();
		SearchPoint = StartPoint;

		OnCharacterAIStateChange.BindUObject(this, &ACPP_AIController_Character::CheckCharacterAIState);
	}

	GetPerceptionValues();


}

void ACPP_AIController_Character::OnPatrolling()
{
	ResultData = MoveToLocation(SearchPoint);

	if (ResultData == EPathFollowingRequestResult::Type::AlreadyAtGoal && !GetWorldTimerManager().IsTimerActive(PatrollingTimerHandle))
	{
		const float RandomTimer{ FMath::RandRange(3.f, RandomTimerPatrolling) };

		GetWorldTimerManager().SetTimer(PatrollingTimerHandle, this, &ACPP_AIController_Character::SetRandomPatrolPoint, RandomTimer, false);
	}
}

void ACPP_AIController_Character::OnInvestigate()
{
	ResultData = MoveToLocation(SearchPoint);

	if (ResultData == EPathFollowingRequestResult::Type::AlreadyAtGoal)
	{
		CharacterAIState = ECharacterAIState::PATROLLING;
		OnCharacterAIStateChange.Execute();
	}
}

void ACPP_AIController_Character::OnPersecution()
{
	if (EnemyCharacter && ControlledCharacter)
	{
		MoveToActor(EnemyCharacter, 500.f);

		const double DistanceToEnemy{ FVector::Distance(ControlledCharacter->GetActorLocation(), EnemyCharacter->GetActorLocation()) };

		if (DistanceToEnemy >= LoseAreaSight)
		{
			CharacterAIState = ECharacterAIState::INVESTIGATE;
			OnCharacterAIStateChange.Execute();

			SearchPoint = EnemyCharacter->GetActorLocation();

			EnemyCharacter = nullptr;
		}
	}
}

void ACPP_AIController_Character::GetPerceptionValues()
{
	UAIPerceptionComponent::TAISenseConfigConstIterator SensesConfigIterator = PerceptionComponent->GetSensesConfigIterator();

	for (auto Sense = SensesConfigIterator; Sense; Sense++)
	{
		if (UAISenseConfig_Sight* SenseConfig_Sight = Cast<UAISenseConfig_Sight>((*Sense)))
		{
			DistanceSight = SenseConfig_Sight->SightRadius;
			LoseAreaSight = SenseConfig_Sight->LoseSightRadius;

			UE_LOG(LogTemp, Error, TEXT("DistanceSight %f"), DistanceSight)
		}
		if (UAISenseConfig_Hearing* SenseConfig_Sight = Cast<UAISenseConfig_Hearing>((*Sense)))
		{
			DistanceHear = SenseConfig_Sight->HearingRange;

			UE_LOG(LogTemp, Error, TEXT("DistanceHear %f"), DistanceHear)
		}
	}
}

void ACPP_AIController_Character::OnSight(AActor* SightActor, const bool& IsSight)
{
	if (SightActor && ControlledCharacter)
	{
		if (CharacterAIState != ECharacterAIState::PERSECUTION)
		{
			const double DistanceToEnemy{ FVector::Distance(ControlledCharacter->GetActorLocation(), SightActor->GetActorLocation()) };

			if (DistanceToEnemy <= DistanceSight)
			{
				EnemyCharacter = SightActor;			

				CharacterAIState = ECharacterAIState::PERSECUTION;
				OnCharacterAIStateChange.Execute();

				GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
			}
		}
		else if (!IsSight && CharacterAIState != ECharacterAIState::INVESTIGATE)
		{
			SearchPoint = GetPointForInvestigate(EnemyCharacter->GetActorLocation(), 1.f);

			CharacterAIState = ECharacterAIState::INVESTIGATE;
			OnCharacterAIStateChange.Execute();

			EnemyCharacter = nullptr;
		}
	}
}

void ACPP_AIController_Character::OnHear(const FVector& StimulusLocation)
{
	if (ControlledCharacter)
	{
		if (CharacterAIState != ECharacterAIState::PERSECUTION && CharacterAIState != ECharacterAIState::INVESTIGATE)
		{
			const double DistanceNoise{ FVector::Distance(ControlledCharacter->GetActorLocation(), StimulusLocation) };
			
			if (DistanceNoise <= DistanceHear)
			{
				SearchPoint = GetRandomPointInArea(StimulusLocation, RandomAreaSearch);

				CharacterAIState = ECharacterAIState::INVESTIGATE;
				OnCharacterAIStateChange.Execute();
				
				GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
			}
		}
	}
}

void ACPP_AIController_Character::SetPerceptionEnable(const bool& Value)
{
	UAIPerceptionComponent::TAISenseConfigConstIterator SensesConfigIterator = PerceptionComponent->GetSensesConfigIterator();
	
	for (auto Sense = SensesConfigIterator; Sense; Sense++)
	{	
		PerceptionComponent->SetSenseEnabled((*Sense)->GetSenseImplementation(), Value);
	}

	Reset();
	GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
}

void ACPP_AIController_Character::OnDamage(const FVector& StimulusLocation)
{
	if (CharacterAIState != ECharacterAIState::PERSECUTION && CharacterAIState != ECharacterAIState::INVESTIGATE)
	{
		const FVector CharacterLocation{ ControlledCharacter->GetActorLocation() };
		const FRotator DirectionShoot{ (CharacterLocation - StimulusLocation).Rotation() };

		const double DistanceHit{ FVector::Distance(CharacterLocation, StimulusLocation) };

		const double DistanceShootSearch{ DistanceHit < DistanceShoot ? DistanceHit : DistanceShoot };

		const FVector ExpectedPoint{ CharacterLocation - DirectionShoot.Vector() * DistanceShootSearch };

		SearchPoint = GetRandomPointInArea(ExpectedPoint, ShootArea);

		CharacterAIState = ECharacterAIState::INVESTIGATE;
		OnCharacterAIStateChange.Execute();

		GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
	}
	
}

void ACPP_AIController_Character::TurnLogic(const bool& Value)
{
	SetActorTickEnabled(Value);
	SetPerceptionEnable(Value);

	CharacterAIState = Value ? ECharacterAIState::PATROLLING : ECharacterAIState::DISABLE;
	OnCharacterAIStateChange.Execute();
}

const FVector ACPP_AIController_Character::GetRandomPointInArea(const FVector& PointSearch, const double& AreaSearch)
{
	if (UWorld * World{ GetWorld() })
	{
		if (UNavigationSystemV1 * NavigationSystem{ UNavigationSystemV1::GetNavigationSystem(World) })
		{
			FNavLocation NavLocation;
			NavigationSystem->GetRandomReachablePointInRadius(PointSearch, AreaSearch, NavLocation);

			return NavLocation.Location;
		}
	}

	return PointSearch;
}

const FVector ACPP_AIController_Character::GetPointForInvestigate(const FVector& PointSearch, const double& AreaSearch)
{
	if (UWorld * World{ GetWorld() })
	{
		if (UNavigationSystemV1 * NavigationSystem{ UNavigationSystemV1::GetNavigationSystem(World) })
		{
			FNavLocation NavLocation;
			NavigationSystem->GetRandomPointInNavigableRadius(PointSearch, AreaSearch, NavLocation);

			return NavLocation.Location;
		}
	}

	return PointSearch;
}

void ACPP_AIController_Character::CheckCharacterAIState()
{
	switch (CharacterAIState)
	{
		case ECharacterAIState::PATROLLING:
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;			
			break;
		}
		case ECharacterAIState::INVESTIGATE:
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = InvestigateSpeed;
			break;
		}
		case ECharacterAIState::PERSECUTION:
		{
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = PersecutionSpeed;
			break;
		}
	}
	SetTextStateBot();
}

void ACPP_AIController_Character::SetRandomPatrolPoint()
{
	SearchPoint = GetRandomPointInArea(StartPoint, PatrollingAreaSearch);
}

void ACPP_AIController_Character::SetTextStateBot()
{
	if (StateWidget)
		StateWidget->SetTextBlock(UEnum::GetValueAsString(CharacterAIState));
}