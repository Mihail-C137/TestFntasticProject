// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_CharacterWidgetComponent.h"
#include "TestProject_Fntastic/Public/CPP_StateWidget.h"

void UCPP_CharacterWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCPP_CharacterWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCPP_CharacterWidgetComponent::SetStateWidget()
{
	StateWidget = Cast<UCPP_StateWidget>(GetWidget());
}
