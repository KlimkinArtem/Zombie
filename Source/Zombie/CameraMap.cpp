// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraMap.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraMap::ACameraMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACameraMap::BeginPlay()
{
	Super::BeginPlay();
	ZChar = Cast<AZombieCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ZChar->CameraDelegate.AddDynamic(this, &ACameraMap::MapCamera);
}

void ACameraMap::MapCamera()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Cameraa Delegae is wortks")));
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 1.5f);
	//UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, 1.5f, VTBlend_Linear, 0, true);
}

