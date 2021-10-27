#include "ZombieCharacter.h"

#include "DrawDebugHelpers.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


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
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false;

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

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Some debug message!"));

	RotateCharacter();
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
}

void AZombieCharacter::MoveForward(float Value)
{
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
	Shoot.Broadcast();
	FHitResult MouseHitResult;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, MouseHitResult);
	//FVector Start = FollowCamera->GetComponentLocation();
	FVector Start = Pistol->GetActorLocation();
	
	//FVector FollowCameraForwardVector = FollowCamera->GetForwardVector();
	FVector End = MouseHitResult.ImpactPoint * HelpMultiplyValue;

	FHitResult OutHit;
	
	
	FCollisionQueryParams CollisionParams;
	FActorSpawnParameters SpawnInfo;
	CollisionParams.AddIgnoredActor(this);
	
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3, 0, 2);
	DrawDebugSphere(GetWorld(), End, 10, 10, FColor(181,0,0), true, 2, 0, 2);
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Pawn, CollisionParams);

	
	
	if(bIsHit)
	{
		if(GEngine)
		{
			if(OutHit.Actor->ActorHasTag("Enemy"))
			{
				//OutHit.GetActor()->TakeDamage(PistolDamage, FDamageEvent(), GetController(), this);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Health: %s"), *OutHit.BoneName.ToString()));
				
			}else if(OutHit.Actor->ActorHasTag("Destruction"))
			{
				//AActor* RadialForceSpawn = GetWorld()->SpawnActor<AActor>(RadialForce,OutHit.ImpactPoint, FRotator::ZeroRotator, SpawnInfo);
				//RadialForceSpawn->Destroy();
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Bottle!! %s"), *OutHit.ImpactPoint.ToString()));
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


