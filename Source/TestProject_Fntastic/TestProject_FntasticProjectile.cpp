// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestProject_FntasticProjectile.h"
#include "TestProject_Fntastic/TestProject_FntasticCharacter.h"
#include "Perception/AISense_Damage.h"
#include "Public/CPP_AIController_Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

void ATestProject_FntasticProjectile::BeginPlay()
{
	Super::BeginPlay();

	StartShootPoint = GetActorLocation();
}

ATestProject_FntasticProjectile::ATestProject_FntasticProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ATestProject_FntasticProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	InitialLifeSpan = 0.0f;
}

void ATestProject_FntasticProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		ATestProject_FntasticCharacter* ThisCharacter = Cast<ATestProject_FntasticCharacter>(OtherActor);
		
		if (ThisCharacter)
		{
			ThisCharacter->ApplyDamage(ProjectileDamage, ThisCharacter, GetActorLocation(), NormalImpulse);

			UAISense_Damage::ReportDamageEvent(GetWorld(), OtherActor, Character, 100.f, StartShootPoint, GetActorLocation());

			Character = nullptr;
		}

		if (OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 10.0f, GetActorLocation());;
		}		
	}

	Destroy();	
}