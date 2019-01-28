// MOsipa 2018

#include "GuardTowerController.h"
#include "UObject/ConstructorHelpers.h"
#include "AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "AIModule/Classes/Perception/AISenseConfig_Sight.h"
#include "GuardTower.h"
#include "RTSPlayerUnit.h"

//TODO create BT + tasks for GuardTowerController
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
		//If any enemy units are in building
		if (Cast<AGuardTower>(this->GetPawn())->IsAnybodyInside())
		{
			//If SensedActor is PlayerUnit's class
			if (SensedActor->GetClass()->IsChildOf<ARTSPlayerUnit>())
			{
				//TODO shooting only once not till unit dies - fix
				//TODO create BT + tasks for GuardTowerController and in tasks force it to shoot
				Cast<AGuardTower>(this->GetPawn())->PrepareToFire(Cast<ARTSPlayerUnit>(SensedActor));
			}
		}
	}
}