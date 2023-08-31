// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_TriggerAI.h"
#include "TestProject_Fntastic/Public/CPP_CharacterWidgetComponent.h"
#include "TestProject_Fntastic/Public/CPP_ContextButtonWidget.h"

// Sets default values
ACPP_TriggerAI::ACPP_TriggerAI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACPP_TriggerAI::BeginPlay()
{
	Super::BeginPlay();

	UActorComponent* ActorComponent = GetComponentByClass(UStaticMeshComponent::StaticClass());

	if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent))
	{		
		if (UMaterialInterface* MaterialInterface = StaticMeshComponent->GetMaterial(0))
		{
			MaterialInstanceDynamic = StaticMeshComponent->CreateDynamicMaterialInstance(0, MaterialInterface);
		}				
	}	
}

// Called every frame
void ACPP_TriggerAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_TriggerAI::CreateContextWidget()
{
	
	CharacterWidgetComponent = NewObject<UCPP_CharacterWidgetComponent>(this, CharacterWidgetComponentInstance, "CharacterWidgetComponent");

	if (CharacterWidgetComponent)
	{
		CharacterWidgetComponent->RegisterComponent();
		CharacterWidgetComponent->SetWidgetClass(ContextWidgetInstance);
		CharacterWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		CharacterWidgetComponent->AddLocalOffset(CharacterWidgetComponentOffset);
	}
		
}

void ACPP_TriggerAI::DestroyContextWidget()
{
	if (CharacterWidgetComponent)
	{
		CharacterWidgetComponent->DestroyComponent();
	}
}

void ACPP_TriggerAI::ChangeButtonColor(const bool bIsEnable)
{
	if (MaterialInstanceDynamic)
	{	
		MaterialInstanceDynamic->SetVectorParameterValue("Color", bIsEnable ? GreenColor : RedColor);
	}
}

