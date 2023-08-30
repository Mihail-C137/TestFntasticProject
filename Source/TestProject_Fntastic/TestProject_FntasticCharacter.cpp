// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestProject_FntasticCharacter.h"
#include "TestProject_FntasticProjectile.h"
#include "TestProject_FntasticGameMode.h"

#include "Public/CPP_CharacterWidgetComponent.h"
#include "Public/CPP_AIController_Character.h"
#include "Public/CPP_StateWidget.h"
#include "Public/CPP_TriggerAI.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/CPP_CrosshairWidget.h"
#include "Public/CPP_WeaponDefault.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// ATestProject_FntasticCharacter

ATestProject_FntasticCharacter::ATestProject_FntasticCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void ATestProject_FntasticCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		CrosshairWidget = CreateWidget<UCPP_CrosshairWidget>(PlayerController, CrosshairWidgetInstance, "Crosshair Widget");

		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
		}

		UWorld* World = GetWorld();

		if (World)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(GetActorLocation());

			Weapon = World->SpawnActorDeferred<ACPP_WeaponDefault>(WeaponInstance, SpawnTransform);

			if (Weapon)
			{
				FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
				Weapon->AttachToComponent(GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
				Weapon->SetupMappingContext(PlayerController);
				Weapon->SetPlayerCharacter(this);
				// switch bHasRifle so the animation blueprint can switch to another animation set
				SetHasRifle(true);		
			}
		}

		TArray<AActor*> CharacterBots;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATestProject_FntasticCharacter::StaticClass(), CharacterBots);
		for (auto* Character : CharacterBots)
		{
			if (auto* Bot = Cast<ATestProject_FntasticCharacter>(Character))
			{				
				Bot->CreateStateWidgets();
			}
		}

		AActor* TriggerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_TriggerAI::StaticClass());

		TriggerAIBox = Cast<ACPP_TriggerAI>(TriggerActor);
	}
}

void ATestProject_FntasticCharacter::Tick(float DeltaTime)
{
	SetWidgetScale();
	CheckButton();
}

void ATestProject_FntasticCharacter::SetWidgetScale()
{
	if (GetController() && !GetController()->IsA(ACPP_AIController_Character::StaticClass()))
	{
		if (ATestProject_FntasticGameMode* GameMode = GetGameMode())
		{
			TArray<ATestProject_FntasticCharacter*> Bots = GameMode->GetBots();

			for (ATestProject_FntasticCharacter* Bot : Bots)
			{
				if (UCPP_StateWidget* BotStateWidget = Bot->GetStateWidget())
				{
					const double Distance = (FirstPersonCameraComponent->GetComponentLocation() - Bot->GetCharacterWidgetComponent()->GetComponentLocation()).Size();

					if (Distance > RenderDistance)
					{
						if (BotStateWidget->GetVisibility() != ESlateVisibility::Collapsed)
							BotStateWidget->SetVisibility(ESlateVisibility::Collapsed);
					}
					else
					{
						if (Distance > MinRenderDistance)
						{
							if (BotStateWidget->GetVisibility() != ESlateVisibility::Visible)
								BotStateWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

							const double Scale = GetScaleCoef(Distance);

							BotStateWidget->SetRenderScale(FVector2D(Scale, Scale));
						}
						else
						{
							if (BotStateWidget->GetVisibility() != ESlateVisibility::Collapsed)
								BotStateWidget->SetVisibility(ESlateVisibility::Collapsed);
						}
					}
				}
			}
		}	
	}
}

void ATestProject_FntasticCharacter::CheckButton()
{
	if (TriggerAIBox)
	{
		const FVector ActorLocation{ GetActorLocation() };

		const double DistanceButton{ FVector::Distance(ActorLocation, TriggerAIBox->GetActorLocation()) };

		if (DistanceButton <= 250.0)
		{
			FHitResult HitResult;

			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(this);

			GetWorld()->LineTraceSingleByChannel(HitResult, ActorLocation, ActorLocation + GetActorRotation().Vector() * 1000.0, ECC_MAX, CollisionQueryParams);

			if (HitResult.bBlockingHit)
			{				
				if (HitResult.GetActor()->IsA(ACPP_TriggerAI::StaticClass()))
				{
					if (!bIsButtonInTrace)
					{
						TriggerAIBox->CreateContextWidget();
						bIsButtonInTrace = true;
					}
				}
				else if (bIsButtonInTrace)
				{
					TriggerAIBox->DestroyContextWidget();
					bIsButtonInTrace = false;
				}
			}
			else if (bIsButtonInTrace)
			{
				TriggerAIBox->DestroyContextWidget();
				bIsButtonInTrace = false;
			}
		}
		else if (bIsButtonInTrace)
		{
			TriggerAIBox->DestroyContextWidget();
			bIsButtonInTrace = false;
		}

	}
}

bool ATestProject_FntasticCharacter::GetIsEnableAI()
{
	if (ATestProject_FntasticGameMode* GameMode = GetGameMode())
	{
		return GameMode->IsEnableAI();
	}
	return false;
}

bool ATestProject_FntasticCharacter::SwitchLogicAI()
{
	return false;
}

void ATestProject_FntasticCharacter::InteractWithButton()
{
	if (TriggerAIBox && bIsButtonInTrace)
	{
		const bool bLogicParameter = !GetIsEnableAI();

		TriggerAIBox->ChangeButtonColor(bLogicParameter);
		TurnLogicAI(bLogicParameter);

		if (ATestProject_FntasticGameMode* GameMode = GetGameMode())
			GameMode->TurnBot(bLogicParameter);
	}
}

ATestProject_FntasticGameMode* ATestProject_FntasticCharacter::GetGameMode()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ATestProject_FntasticGameMode* GameMode = Cast<ATestProject_FntasticGameMode>(World->GetAuthGameMode());

		if (GameMode)
		{
			return GameMode;
		}
	}
	return nullptr;
}

void ATestProject_FntasticCharacter::TurnLogicAI(const bool& Value)
{
	if (ACPP_AIController_Character* AIController = Cast<ACPP_AIController_Character>(GetController()))
	{
		AIController->TurnLogic(Value);
	}
}

void ATestProject_FntasticCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATestProject_FntasticCharacter, Health);
}

//////////////////////////////////////////////////////////////////////////// Input

void ATestProject_FntasticCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATestProject_FntasticCharacter::Move);
		//Moving
		EnhancedInputComponent->BindAction(SpeedAction, ETriggerEvent::Started, this, &ATestProject_FntasticCharacter::Speed);
		EnhancedInputComponent->BindAction(SpeedAction, ETriggerEvent::Completed, this, &ATestProject_FntasticCharacter::Speed);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATestProject_FntasticCharacter::Look);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATestProject_FntasticCharacter::InteractWithButton);
	}
}



void ATestProject_FntasticCharacter::AddBotToArray_Server_Implementation()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ATestProject_FntasticGameMode* GameMode = Cast<ATestProject_FntasticGameMode>(World->GetAuthGameMode());

		if (GameMode)
		{
			GameMode->AddBot(this);
		}
	}
}

void ATestProject_FntasticCharacter::CreateStateWidgets()
{
	if (ACPP_AIController_Character* AIController = Cast<ACPP_AIController_Character>(GetController()))
	{
		CharacterWidgetComponent = NewObject<UCPP_CharacterWidgetComponent>(this, CharacterWidgetComponentInstance, "CharacterWidgetComponent");

		if (CharacterWidgetComponent)
		{			
			CharacterWidgetComponent->RegisterComponent();		
			CharacterWidgetComponent->SetWidgetClass(StateWidgetInstance);
			CharacterWidgetComponent->SetStateWidget();
			CharacterWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			CharacterWidgetComponent->AddLocalOffset(CharacterWidgetComponentOffset);


			if (CharacterWidgetComponent->GetStateWidget())
			{
				AIController->SetStateWidget(CharacterWidgetComponent->GetStateWidget());
				AIController->SetTextStateBot();

				SetStateWidget(CharacterWidgetComponent->GetStateWidget());
				
				AddBotToArray_Server();
			}
		}
	}	
}

void ATestProject_FntasticCharacter::SetRagdoll()
{
	GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), 1.f);
	GetMesh()->SetCollisionProfileName("Ragdoll");
}

void ATestProject_FntasticCharacter::Death()
{
	Health = 0.f;
	GetStateWidget()->StartHideWidget();

	if (ACPP_AIController_Character* AIController = Cast<ACPP_AIController_Character>(GetController()))
	{
		AIController->SetPerceptionEnable(false);
		AIController->SetActorTickEnabled(false);

		if (ATestProject_FntasticGameMode* GameMode = GetGameMode())
		{
			GameMode->DeleteBot(this);
		}
	}
	
}

void ATestProject_FntasticCharacter::ApplyDamage(const float& Damage, AActor* DamageDealer, const FVector& PointHit, const FVector ImpulseHit)
{
	if (Health != 0.f)
	{
		if (Damage >= Health)
		{
			SetRagdoll();
			Death();
		}
		else
		{
			Health -= Damage;
		}

		if (StateWidget)
		{
			StateWidget->SetCurrentHealth(Health);
		}
	}
}


void ATestProject_FntasticCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
		MakeNoise(1.f, this, GetActorLocation(), 5000.f);
	}
}

void ATestProject_FntasticCharacter::Speed(const FInputActionValue& Value)
{
	const bool InputActionValue = Value.Get<bool>();

	GetCharacterMovement()->MaxWalkSpeed = InputActionValue ? 900.f : 600.f;
}

void ATestProject_FntasticCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATestProject_FntasticCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ATestProject_FntasticCharacter::GetHasRifle()
{
	return bHasRifle;
}