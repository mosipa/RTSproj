// MOsipa 2018

#include "GuardTowerController.h"
#include "UObject/ConstructorHelpers.h"
#include "AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "AIModule/Classes/Perception/AISenseConfig_Sight.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

AGuardTowerController::AGuardTowerController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	if (AIPerceptionComponent)
	{
		SetPerceptionComponent(*AIPerceptionComponent);
	}

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSense"));
	if (Sight)
	{
		Sight->SightRadius = 1000;
		Sight->LoseSightRadius = 1100;
		Sight->PeripheralVisionAngleDegrees = 45.f;
		Sight->DetectionByAffiliation.bDetectEnemies = true;
		Sight->DetectionByAffiliation.bDetectNeutrals = true;
		Sight->DetectionByAffiliation.bDetectFriendlies = true;
		
		AIPerceptionComponent->ConfigureSense(*Sight);
	}

	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGuardTowerController::OnTargetPerceptionUpdated);

}

void AGuardTowerController::OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (SensedActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Sensed actor: %s by %s"), *(SensedActor->GetName()), *(this->GetName()));
		}
	}
}