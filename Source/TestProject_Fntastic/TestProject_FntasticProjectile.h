// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestProject_FntasticProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class ATestProject_FntasticProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY()
	class ATestProject_FntasticCharacter* Character{ nullptr };

protected:
	virtual void BeginPlay() override;

public:
	ATestProject_FntasticProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	FORCEINLINE void SetActorHit(class ATestProject_FntasticCharacter* CharacterPtr) { Character = CharacterPtr; }

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Settings)
	float ProjectileSpeed{ 5000.f };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Settings)
	float ProjectileDamage{ 20.0 };

	FVector StartShootPoint{ 0.0 };
};

