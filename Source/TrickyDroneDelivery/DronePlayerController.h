//DronePlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DronePlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class TRICKYDRONEDELIVERY_API ADronePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ADronePlayerController();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* IMC_Drone;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_Look;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_Roll;

	virtual void BeginPlay() override;
};
