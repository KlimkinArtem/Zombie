#include "Pistol.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


APistol::APistol()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();

	ZombieCharacter = Cast<AZombieCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ZombieCharacter->Shoot.AddDynamic(this, &APistol::Shoot);
}


void APistol::Shoot()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Some debug message!"));
	FVector Location = RootComponent->GetSocketLocation("Fire");
	FRotator Rotation = RootComponent->GetSocketRotation("Fire");

	FActorSpawnParameters SpawnInfo;
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	
	GetWorld()->SpawnActor<AActor>(Particle,Location, Rotation, SpawnInfo)->AttachToComponent(RootComponent, AttachRules, "Fire");
	
	
}

