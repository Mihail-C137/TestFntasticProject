// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TestProject_FntasticCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

enum class ECharacterType : uint8
{
	CHARACTER UMETA(DisplayName = "Character"),
	BOT UMETA(DisplayName = "Bot")
};

UCLASS(config=Game)
class ATestProject_FntasticCharacter : public ACharacter
{
	GENERATED_BODY()
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SpeedAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;
		
public:
	ATestProject_FntasticCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	void Speed(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

/********************************************/
/*Mihail*/
/********************************************/
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Weapon)
	TSubclassOf<class ACPP_WeaponDefault> WeaponInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	TSubclassOf<class UCPP_CrosshairWidget> CrosshairWidgetInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	TSubclassOf<class UCPP_CharacterWidgetComponent> CharacterWidgetComponentInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	TSubclassOf<class UCPP_StateWidget> StateWidgetInstance;
	

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Damage)
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(Replicated, EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	float Health{ 100.f };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	double RenderDistance{ 3000.0 };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	double MinRenderDistance{ 0.0 };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	double MinScale{ 0.0 };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccsess = "true"), Category = Widget)
	FVector CharacterWidgetComponentOffset{ 0.0 };

	UPROPERTY()
	class ACPP_WeaponDefault* Weapon{ nullptr };

	UPROPERTY()
	class UCPP_CrosshairWidget* CrosshairWidget{ nullptr };

	UPROPERTY()
	class UCPP_StateWidget* StateWidget{ nullptr };

	UPROPERTY()
	class UCPP_CharacterWidgetComponent* CharacterWidgetComponent{ nullptr };

	UPROPERTY()
	class ACPP_TriggerAI* TriggerAIBox{ nullptr };
	
	bool bIsButtonInTrace{ false };

	ECharacterType CharacterType = ECharacterType::CHARACTER;

	UFUNCTION()
	void CreateStateWidgets();

	UFUNCTION(Server, Reliable)
	void AddBotToArray_Server();

	void SetWidgetScale();
	void CheckButton();

	bool GetIsEnableAI();
	bool SwitchLogicAI();

	void InteractWithButton();

	class ATestProject_FntasticGameMode* GetGameMode();

	FORCEINLINE const double GetScaleCoef(const double& Distance)
	{
		return (1.0 - Distance / RenderDistance) * (1.0 - MinScale) + MinScale;
	}

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	FORCEINLINE class UCPP_CrosshairWidget* GetCrosshairWidget() const { return CrosshairWidget; }

	FORCEINLINE void SetCharacterType(const ECharacterType& CharacterTypeRef) { CharacterType = CharacterTypeRef; }
	FORCEINLINE const ECharacterType GetCharacterType() const { return CharacterType; }

	FORCEINLINE void SetStateWidget(class UCPP_StateWidget* StateWidgetPtr) { StateWidget = StateWidgetPtr; }
	FORCEINLINE class UCPP_StateWidget* GetStateWidget() const { return StateWidget; }

	FORCEINLINE class UCPP_CharacterWidgetComponent* GetCharacterWidgetComponent() const { return CharacterWidgetComponent; }

	FORCEINLINE const bool IsCharacterAlive() { return true; }

	void ApplyDamage(const float& Damage, AActor* DamageDealer, const FVector& PointHit, const FVector ImpulseHit);
	void SetRagdoll();
	void Death();

	void TurnLogicAI(const bool& Value);
};

