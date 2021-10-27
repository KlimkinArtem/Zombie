// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

UCLASS(config=Game)
class AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AZombieCharacter();

	//Flashlight
	UPROPERTY(VisibleAnywhere, Category = "Light Switch")
	class USpotLightComponent* Flashlight;

	UPROPERTY(EditDefaultsOnly, Category= "Weapon")
	TSubclassOf<AActor> Weapon;

	UPROPERTY(EditDefaultsOnly, Category= "Help")
	float HelpAddValue = 1.f;

	UPROPERTY(EditDefaultsOnly, Category= "Help")
	float HelpMultiplyValue = 1.f;

	UPROPERTY(EditDefaultsOnly, Category= "Help")
	float HelpDivideValue = 1.f;

protected:
	
	void MoveForward(float Value);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


private:

	//rotate character 
	void RotateCharacter();

	//Flashlight
	void FlashlightTurnOnOff();
	bool bFlashlight = false;

	//fire
	void Fire();
	void SpawnWeapon();
	AActor* Pistol;
	
};



