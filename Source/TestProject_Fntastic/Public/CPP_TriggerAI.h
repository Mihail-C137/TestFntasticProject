// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_TriggerAI.generated.h"

UCLASS()
class TESTPROJECT_FNTASTIC_API ACPP_TriggerAI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_TriggerAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	class UCPP_CharacterWidgetComponent* CharacterWidgetComponent{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	TSubclassOf<class UCPP_CharacterWidgetComponent> CharacterWidgetComponentInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	TSubclassOf<class UCPP_ContextButtonWidget> ContextWidgetInstance;
	

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	FVector CharacterWidgetComponentOffset{ 0.0 };

	FVector4 GreenColor{ FVector4(FColor::Green) };
	FVector4 RedColor{ FVector4(FColor::Red) };

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstanceDynamic{ nullptr };

public:
	void CreateContextWidget();
	void DestroyContextWidget();

	void ChangeButtonColor(const bool bIsEnable);
};
