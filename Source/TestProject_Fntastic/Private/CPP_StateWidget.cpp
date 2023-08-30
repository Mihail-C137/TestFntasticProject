// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_StateWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UCPP_StateWidget::SetTextBlock(const FString& CurrentState)
{
	SetTextColorState(CurrentState);
}

void UCPP_StateWidget::SetCurrentHealth(const float& HealthAmount)
{
	HealthBar->SetPercent(HealthAmount / 100.f);
}

void UCPP_StateWidget::StartHideWidget()
{
	bIsStartHideWidget = true;

	DeltaTimer = HideTimer;
}

void UCPP_StateWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTick)
{
	Super::NativeTick(MyGeometry, DeltaTick);

	if (bIsStartHideWidget)
	{
		if (DeltaTimer <= 0)
		{
			bIsStartHideWidget = false;
		}
		else
		{
			SetRenderOpacity(DeltaTimer / HideTimer);
		}
		DeltaTimer -= DeltaTick;
	}
	
}

void UCPP_StateWidget::SetTextColorState(const FString& CurrentState)
{
	if (CurrentState.Contains("PATROLLING"))
	{
		CurrentStateText->SetText(FText::FromName("PATROLLING"));
		CurrentStateText->SetColorAndOpacity(PatrollingColor);
	}
	else if (CurrentState.Contains("INVESTIGATE"))
	{
		CurrentStateText->SetText(FText::FromName("INVESTIGATE"));
		CurrentStateText->SetColorAndOpacity(InvestigateColor);
	}
	else if (CurrentState.Contains("PERSECUTION"))
	{
		CurrentStateText->SetText(FText::FromName("PERSECUTION"));
		CurrentStateText->SetColorAndOpacity(PersecutionColor);
	}
	else if (CurrentState.Contains("DISABLE"))
	{
		CurrentStateText->SetText(FText::FromName("DISABLE"));
		CurrentStateText->SetColorAndOpacity(DisableColor);
	}
	
}
