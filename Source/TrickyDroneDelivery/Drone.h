// Drone.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Drone.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UMovementComponent;
struct FInputActionValue;

UCLASS()
class TRICKYDRONEDELIVERY_API ADrone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADrone();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Drone)
	UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Drone)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Drone)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Drone)
	UCameraComponent* CameraComp;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone)
	float Speed;			//Movement Speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone)
	float MovementLerpRate;	//Rate to balance with inertia/DesiredVelocity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone)
	float MovementZeroThreshold;	//Bottom Threshold to clear the roll value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone)
	float RollLerpRate;		//Rate to rebalance after rolling
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone)
	float RollZeroThreshold;	//Bottom Threshold to clear the roll value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone)
	float GravityScale;		//Gravity strength
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone)
	float BottomDistance;	//Bottom collision distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone)
	float GroundSpeedRatio;	//Ratio to slow down on the ground

	bool IsRolling;			//whether the roll input in on
	bool OnTheGround;		//whether it's touching the ground

	UFUNCTION()
	void ChangeDesiredVelocity(const FInputActionValue& Value);
	UFUNCTION()
	void ResetDesiredVelocity(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void BeginRolling(const FInputActionValue& Value);
	UFUNCTION()
	void EndRolling(const FInputActionValue& Value);

	FVector CurrentVelocity;	//Movement veclocity
	FVector DesiredVelocity;	//Desired Movement Direction with speed
};
