//DronePlayerController.cpp
#include "DronePlayerController.h"
#include "EnhancedInputSubsystems.h"

ADronePlayerController::ADronePlayerController():
	IMC_Drone(nullptr),
	IA_Move(nullptr),
	IA_Look(nullptr)
{
}

void ADronePlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer()) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
			if (IMC_Drone != nullptr) {
				Subsystem->AddMappingContext(IMC_Drone, 0);
			}
		}
	}
}
