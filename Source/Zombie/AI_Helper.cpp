// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Helper.h"

// Sets default values
AAI_Helper::AAI_Helper()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAI_Helper::BeginPlay()
{
	Super::BeginPlay();
}


// Called to bind functionality to input
void AAI_Helper::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

