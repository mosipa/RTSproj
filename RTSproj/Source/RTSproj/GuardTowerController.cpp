// MOsipa 2018

#include "GuardTowerController.h"
#include "UObject/ConstructorHelpers.h"
#include "AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "AIModule/Classes/Perception/AISenseConfig_Sight.h"
#include "GuardTower.h"
#include "RTSPlayerUnit.h"
#include "AIModule/Classes/BehaviorTree/BlackboardData.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"

//TODO create BT + tasks for GuardTowerController
AGuardTowerController::AGuardTowerController()
{
	BlackboardAsset = NewObject<UBlackboardData>();

	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("Shoot"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(FName("PlayerUnitKey"));

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Blueprints/GuardTowerBT"));

	if (BTObject.Succeeded())
	{
		BehaviorTree = BTObject.Object;
		BehaviorTree->BlackboardAsset = BlackboardAsset;
	}

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
		Sight->PeripheralVisionAngleDegrees = 360.f;
		Sight->DetectionByAffiliation.bDetectEnemies = true;
		Sight->DetectionByAffiliation.bDetectNeutrals = true;
		Sight->DetectionByAffiliation.bDetectFriendlies = true;
		
		AIPerceptionComponent->ConfigureSense(*Sight);
	}

	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGuardTowerController::OnTargetPerceptionUpdated);

}

void AGuardTowerController::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	AGuardTower* Tower = Cast<AGuardTower>(Pawn);

	if (Tower)
	{
		BlackboardComponent->InitializeBlackboard(*BlackboardAsset);
		this->RunBehaviorTree(BehaviorTree);
	}
}

void AGuardTowerController::OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		//If any enemy units are in tower
		if (Cast<AGuardTower>(this->GetPawn())->IsAnybodyInside())
		{
			//If SensedActor is PlayerUnit's class
			if (SensedActor->GetClass()->IsChildOf<ARTSPlayerUnit>())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s sensing %s"), *(this->GetPawn()->GetName()), *(SensedActor->GetName()));
				BlackboardComponent->SetValueAsObject("PlayerUnitKey", SensedActor);
				BlackboardComponent->SetValueAsBool("Shoot", true);
			}
		}
	}
	else
	{
		BlackboardComponent->ClearValue("PlayerUnitKey");
		BlackboardComponent->ClearValue("Shoot");
	}
}