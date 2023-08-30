// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "CPP_CharacterWidgetComponent.generated.h"

class UCPP_StateWidget;

UCLASS()
class TESTPROJECT_FNTASTIC_API UCPP_CharacterWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY()
	UCPP_StateWidget* StateWidget{ nullptr };

public:
	FORCEINLINE UCPP_StateWidget* GetStateWidget() const { return StateWidget; }
	void SetStateWidget();
};
