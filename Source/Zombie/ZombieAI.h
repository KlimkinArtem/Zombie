#pragma once

#include "CoreMinimal.h"
#include "ZombieCharacter.h"
#include "GameFramework/Character.h"
#include "Misc/OutputDeviceNull.h"
#include "ZombieAI.generated.h"

UCLASS()
class ZOMBIE_API AZombieAI : public ACharacter
{
	GENERATED_BODY()

public:
	AZombieAI();

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void MeleAttack(float Radius, int32 Segments, bool DrawDebug);

private:

	AZombieCharacter* ZombieCharacter;

	void RotateZombie();
	void PlayMontages();


	FOutputDeviceNull ar;

	float Health = 100.f;

	void Death();

	bool bDeath = false;
	FTimerHandle TimerHandle;

};
