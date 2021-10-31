#include "ZombieCharacter.h"

#include "AI_Helper.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/OutputDeviceNull.h"


AZombieCharacter::AZombieCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;	
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetRelativeLocation(FVector(300,0,1500));
	FollowCamera->SetRelativeRotation(FRotator(270,0,0));

	
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Lantern"));
	Flashlight->SetupAttachment(RootComponent);

}

inline void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapon();
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
}

void AZombieCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!bDeath)
	{
		RotateCharacter();
	}
	
}

void AZombieCharacter::RotateCharacter()
{
	FHitResult HitResult;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, HitResult);
	FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
	FVector Target = FVector(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, 0);
	FRotator FindLookTarget = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	FRotator RotateMouse = FMath::RInterpTo(GetActorRotation(), FindLookTarget, 0.3f, 0.3f);
	GetController()->SetControlRotation(RotateMouse);
}

void AZombieCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AZombieCharacter::MoveForward);

	PlayerInputComponent->BindAction("Flashlight", IE_Pressed, this, &AZombieCharacter::FlashlightTurnOnOff);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AZombieCharacter::Fire);
	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &AZombieCharacter::Debug);
	PlayerInputComponent->BindAction("Reloading", IE_Pressed, this, &AZombieCharacter::Reloading);

	PlayerInputComponent->BindAction("ShowMap", IE_Pressed, this, &AZombieCharacter::ShowMap);
	PlayerInputComponent->BindAction("Help", IE_Pressed, this, &AZombieCharacter::Help);

}

void AZombieCharacter::MoveForward(float Value)
{
	if(!bEnableInput) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AZombieCharacter::FlashlightTurnOnOff()
{
	if(bFlashlight)
	{
		Flashlight->SetIntensity(100000.f);
		bFlashlight = false;
	}else
	{
		Flashlight->SetIntensity(0.f);
		bFlashlight = true;
	}
}

void AZombieCharacter::Fire()
{
	if(!bEnableInput) return;
	
	if(!bReloading || !ReloadingSystem() || (Ammo == -1.f)) return;
	
	Shoot.Broadcast();
	FHitResult MouseHitResult;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, MouseHitResult);

	UGameplayStatics::SpawnSound2D(this, ShootCue[0], 1);

	FVector Start = Pistol->GetActorLocation();
	FVector End = MouseHitResult.ImpactPoint * HelpMultiplyValue;
	
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(Pistol);

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Pawn, CollisionParams);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3, 0, 2);
	
	if(bIsHit)
	{
		if(GEngine)
		{
			if(OutHit.Actor->ActorHasTag("Enemy"))
			{
				OutHit.GetActor()->TakeDamage(10.f, FDamageEvent(), GetController(), this);
			}
		}
	}
	
}

void AZombieCharacter::SpawnWeapon()
{
	FVector Location = GetMesh()->GetSocketLocation("WeaponSocket");
	FRotator Rotation = GetMesh()->GetSocketRotation("WeaponSocket");
	
	FActorSpawnParameters SpawnInfo;
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	
	Pistol = GetWorld()->SpawnActor<AActor>(Weapon,Location, Rotation, SpawnInfo);
	
	Pistol->AttachToComponent(GetMesh(), AttachRules, "WeaponSocket");
}

float AZombieCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	
	Health -= DamageAmount;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Char helath = %f"), Health));
	
	if(Health <= 0)
	{
		UGameplayStatics::SpawnSound2D(this, TakeDamageCue[1], 1);
		Death();
	}else
	{
		UGameplayStatics::SpawnSound2D(this, TakeDamageCue[0], 1);
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
}

void AZombieCharacter::Death()
{
	GetMesh()->PlayAnimation(DeathAnim[0], false);
	bDeath = true;
	Flashlight->SetIntensity(0.f);
	this->DisableInput(GetWorld()->GetFirstPlayerController());
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
	{
		FOutputDeviceNull ar;
		this->CallFunctionByNameWithArguments(TEXT("DeathMenu"), ar, NULL, true);
	}, 5, false);
}

void AZombieCharacter::Debug()
{
	AmmoClip += 1;
	bNoAmmoClip = false;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("AmmoClip = %f"), AmmoClip));
}

bool AZombieCharacter::ReloadingSystem()
{
	if(bNoAmmoClip)
	{
		UGameplayStatics::SpawnSound2D(this, ShootCue[2], 1);
		return false;
	}
	Ammo -= 1;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Ammo = %f"), Ammo));
	if(Ammo <= 0)
	{
		if(AmmoClip != 0)
		{
			Reloading();
			return true;
		}else
		{
			bNoAmmoClip = true;
			return false;
		}
		
	}
	return true;
}

void AZombieCharacter::Reloading()
{
	if(AmmoClip == 0 || !bReloading) return;

	bReloading = false;
	UGameplayStatics::SpawnSound2D(this, ShootCue[1], 1);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
	{
		bReloading = true;
		AmmoClip -= 1;
		Ammo = 15.f;
	}, 1.5, false);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("AmmoClip = %f"), AmmoClip));

}

void AZombieCharacter::ShowMap()
{
	if(bCamera) return;
	
	if(bShowMap)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Broadcast")));
		CameraDelegate.Broadcast();
		bCamera = true;
		bShowMap = false;
		bEnableInput = false;
		GetWorld()->GetTimerManager().SetTimer(CameraTimerHandle, [&]()
		{
			bCamera = false;
		}, 2, false);
		
	}else
	{
		bCamera = true;
		bShowMap = true;
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 1.5f);
		GetWorld()->GetTimerManager().SetTimer(CameraTimerHandle, [&]()
		{
			bEnableInput = true;
			bCamera = false;
		}, 1.5, false);
	}
}

void AZombieCharacter::Help()
{
	if(!bSpawnHelper) return;

	bSpawnHelper = false;
	FActorSpawnParameters SpawnParameters;
	GetWorld()->SpawnActor<AActor>(HelpBot, FVector(12720.f, 11480.f, 108.f), FRotator(0,0,0), SpawnParameters);
	GetWorldTimerManager().SetTimer(HelperTimerHandle, this, &AZombieCharacter::HelperRefresh, 1.0f, true, 0.f);
}

void AZombieCharacter::HelperRefresh()
{
	if (CountTime == HelpTime)
	{
		GetWorldTimerManager().ClearTimer(HelperTimerHandle);
		CountTime = 0;
		bSpawnHelper = true;
		ScreenHelperTime = 100.f;
		return;
	}
	CountTime++;
	ScreenHelperTime--;
}