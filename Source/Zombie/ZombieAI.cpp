#include "ZombieAI.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AZombieAI::AZombieAI()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AZombieAI::BeginPlay()
{
	Super::BeginPlay();
	
	ZombieCharacter = Cast<AZombieCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

float AZombieAI::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Health -= DamageAmount;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Zombie helath = %f"), Health));
	
	if(Health <= 0)
	{
		Death();
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AZombieAI::Death()
{
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetAllBodiesPhysicsBlendWeight(1.f, false);
	bDeath = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
	{
		Destroy();
	}, 20, false);
}


void AZombieAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateZombie();
	PlayMontages();
}

void AZombieAI::RotateZombie()
{
	if(GetDistanceTo(ZombieCharacter) < 100.f)
	{
		FRotator PlayerRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ZombieCharacter->GetActorLocation());
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), PlayerRotator, GetWorld()->GetDeltaSeconds(), 8.f));
	}
}

void AZombieAI::PlayMontages()
{
	if(!bDeath)
	{
		if(GetDistanceTo(ZombieCharacter) < 120.f)
		{
			this->CallFunctionByNameWithArguments(TEXT("Attack"), ar, NULL, true);
		}
	}
}


void AZombieAI::MeleAttack(float Radius, int32 Segments, bool DrawDebug)
{
	FVector Start = GetMesh()->GetSocketLocation(TEXT("AttackSocket"));
	
	
	
	TArray<FHitResult> OutHits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	
	bool bIsHit = GetWorld()->SweepMultiByChannel(OutHits, Start, Start, FQuat::Identity, ECC_Pawn, Sphere, Params);
	
	if (bIsHit)
	{
		for (auto& Hit : OutHits)
		{
			if (GEngine) 
			{
				if(Hit.Actor->ActorHasTag("Player"))
				{
					Hit.GetActor()->TakeDamage(10.f, FDamageEvent(), GetController(), this);
				}
			}						
		}
	}

	if(DrawDebug)
	{
		DrawDebugSphere(GetWorld(), Start, Radius, Segments, FColor(181,0,0), true, 2, 0, 2);
	}

	//GetWorld()->SweepSingleByChannel();
}
