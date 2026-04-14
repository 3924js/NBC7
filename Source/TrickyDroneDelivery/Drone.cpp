// Drone.cpp
#include "Drone.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "DronePlayerController.h"
#include "Camera/CameraComponent.h"

// Sets default values
ADrone::ADrone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetSimulatePhysics(false);
	RootComponent = BoxComp;
	

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->SetRelativeRotation(FRotator(-10,0,0));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	CurrentVelocity = { 0, 0, 0 };
	DesiredVelocity = { 0, 0, 0 };
	IsRolling = false;
	OnTheGround = false;
}

// Called to bind functionality to input
void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		UE_LOG(LogTemp, Warning, TEXT("EnhancedInput Exists"));
		if (ADronePlayerController* PlayerController = Cast<ADronePlayerController>(GetController())) {
			UE_LOG(LogTemp, Warning, TEXT("PlayerController Exists"));
			if (PlayerController->IA_Move) {
				EnhancedInput->BindAction(PlayerController->IA_Move, ETriggerEvent::Triggered, this, &ADrone::ChangeDesiredVelocity);
				EnhancedInput->BindAction(PlayerController->IA_Move, ETriggerEvent::Completed, this, &ADrone::ResetDesiredVelocity);
			}
			if (PlayerController->IA_Look) {
				EnhancedInput->BindAction(PlayerController->IA_Look, ETriggerEvent::Triggered, this, &ADrone::Look);
			}
			if (PlayerController->IA_Roll) {
				EnhancedInput->BindAction(PlayerController->IA_Roll, ETriggerEvent::Triggered, this, &ADrone::BeginRolling);
				EnhancedInput->BindAction(PlayerController->IA_Roll, ETriggerEvent::Completed, this, &ADrone::EndRolling);
			}
		}
	}
}

void ADrone::ChangeDesiredVelocity(const FInputActionValue& Value)
{
	if (!Controller) return;
	//Set desired movement direction to unit vector of local coordinate
	FVector Input = Value.Get<FVector>().GetSafeNormal();
	FVector Velocity(0.0);
	if (!FMath::IsNearlyZero(Input.X)) {
		Velocity += GetActorForwardVector() * Input.X;
	}
	if (!FMath::IsNearlyZero(Input.Y)) {
		Velocity += GetActorRightVector() * Input.Y;
	}
	if (!FMath::IsNearlyZero(Input.Z)) {
		Velocity += GetActorUpVector() * Input.Z;
	}
	DesiredVelocity = Velocity.GetSafeNormal();
}
void ADrone::ResetDesiredVelocity(const FInputActionValue& Value)
{
	if (!Controller) return;
	//Reset desired movement velocity when the input disaapears
	DesiredVelocity = { 0, 0, 0 };
}

void ADrone::Look(const FInputActionValue& Value)
{
	if (!Controller) return;
	//Rotate view using mouse input
	FVector2D Input = Value.Get<FVector2D>();
	if (!FMath::IsNearlyZero(Input.X)) {
		AddActorWorldRotation(FRotator::MakeFromEuler(FVector(0, 0, Input.X)));
	}
	if (!FMath::IsNearlyZero(Input.Y)) {
		//limit from -90 to 90 degrees
		if (Input.Y < 0 && GetActorRotation().Euler().Y > -90) {
			AddActorLocalRotation(FRotator::MakeFromEuler(FVector(0, Input.Y, 0)));
		}
		if (Input.Y > 0 && GetActorRotation().Euler().Y < 90.0) {
			AddActorLocalRotation(FRotator::MakeFromEuler(FVector(0, Input.Y, 0)));
		}
	}
}

void ADrone::BeginRolling(const FInputActionValue& Value)
{
	if (!Controller) return;
	//Add Roll Rotation
	float Input = Value.Get<float>();
	if (!FMath::IsNearlyZero(Input)) {
		AddActorLocalRotation(FRotator::MakeFromEuler(FVector(Input, 0, 0)) * (Speed / 8) * GetWorld()->GetDeltaSeconds());
		IsRolling = true;	//Rolling Status
	}
}

void ADrone::EndRolling(const FInputActionValue& Value)
{
	if (!Controller) return;
	//Clear Rolling Status
	IsRolling = false;
}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Calculate Movement
	if (FMath::IsNearlyZero(DesiredVelocity.Length())) {// Gravity Applied when not moving
		CurrentVelocity = FMath::Lerp(CurrentVelocity, DesiredVelocity * (OnTheGround ? Speed * GroundSpeedRatio : Speed), MovementLerpRate) + FVector::UnitZ() * GravityScale * DeltaTime;
	}
	else {// No Gravity when moving
		CurrentVelocity = FMath::Lerp(CurrentVelocity, DesiredVelocity * (OnTheGround ? Speed * GroundSpeedRatio : Speed), FVector(MovementLerpRate));
		if (CurrentVelocity.Length() < MovementZeroThreshold) CurrentVelocity = FVector(0.0);
	}
	//Check if Colliding
	FHitResult Result;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(Result, GetActorLocation(), GetActorLocation() - FVector(0, 0, 1000), ECollisionChannel::ECC_Visibility, Params)) {
		float DistanceZ = (GetActorLocation() - Result.ImpactPoint).Length();
		if (DistanceZ < BottomDistance) {
			CurrentVelocity.Z = FMath::Max(CurrentVelocity.Z, 0);
			DesiredVelocity.Z = FMath::Max(DesiredVelocity.Z, 0);
			OnTheGround = true;	//On the Ground
		}
		else {//Not on the Ground
			OnTheGround = false;
		}
	}
	else { //Not on the Ground
		OnTheGround = false;
	}
	//AddMovement
	AddActorWorldOffset(CurrentVelocity * DeltaTime);
	

	//Reset Roll Rotation when not rolling
	if (!IsRolling) {
		FRotator CurrentRotation = GetActorRotation();
		double NewRoll = FMath::FInterpTo(CurrentRotation.Roll, 0, DeltaTime, RollLerpRate);;
		if (FMath::Abs(NewRoll) < RollZeroThreshold) {
			NewRoll = 0;
		}
		SetActorRotation(FRotator::MakeFromEuler(FVector(NewRoll, CurrentRotation.Pitch, CurrentRotation.Yaw)));
	}
}

