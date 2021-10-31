// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieCharacter.h"
#include "GameFramework/Actor.h"
#include "CameraMap.generated.h"

UCLASS()
class ZOMBIE_API ACameraMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraMap();
	
	UFUNCTION()
	void MapCamera();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
private:
	AZombieCharacter* ZChar;

};
