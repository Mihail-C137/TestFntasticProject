// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_StateWidget.generated.h"

struct FSlateColor;

UCLASS()
class TESTPROJECT_FNTASTIC_API UCPP_StateWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTick) override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentStateText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Color)
	FSlateColor PatrollingColor;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Color)
	FSlateColor InvestigateColor;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Color)
	FSlateColor PersecutionColor;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Color)
	FSlateColor DisableColor;	

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Color)
	float HideTimer{ 3.f };

	float DeltaTimer{ 0.f };
	bool bIsStartHideWidget{ false };

	void SetTextColorState(const FString& CurrentState);

public:
	void SetTextBlock(const FString& CurrentState);
	void SetCurrentHealth(const float& HealthAmount);
	void StartHideWidget();
};
