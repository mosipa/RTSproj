// MOsipa 2018

#include "EnemyAIController.h"
#include "RTSEnemyUnit.h"
#include "AIModule/Classes/BehaviorTree/BlackboardData.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "AIModule/Classes/Perception/AISenseConfig_Sight.h"

AEnemyAIController::AEnemyAIController()
{
	BlackboardAsset = NewObject<UBlackboardData>();

	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("NextWaypoint"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Int>(FName("IndexKey"));
	
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Blueprints/EnemyBT"));
	
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
		Sight->SightRadius = 500;
		Sight->LoseSightRadius = 600;
		Sight->PeripheralVisionAngleDegrees = 180.0f;
		Sight->DetectionByAffiliation.bDetectEnemies = true;
		Sight->DetectionByAffiliation.bDetectNeutrals = true;
		Sight->DetectionByAffiliation.bDetectFriendlies = true;

		AIPerceptionComponent->ConfigureSense(*Sight);
	}

	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	SetGenericTeamId(FGenericTeamId(1));
}

void AEnemyAIController::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	ARTSEnemyUnit* Character = Cast<ARTSEnemyUnit>(Pawn);

	if (Character)
	{
		BlackboardComponent->InitializeBlackboard(*BlackboardAsset);
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	auto AIPawnName = this->GetPawn()->GetName();
	UE_LOG(LogTemp, Warning, TEXT("%s has %s"), *(AIPawnName), *(this->GetName()));

	this->UseBlackboard(BlackboardAsset, BlackboardComponent);
	this->RunBehaviorTree(BehaviorTree);
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus Stimulus)
{
	if (Stimulus.IsActive())
	{
		UE_LOG(LogTemp, Warning, TEXT("IsActive"));
		if(Stimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp, Warning, TEXT("WasSuccessfullySensed"));
		}
	}
	if (SensedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sensed Actor: %s"), *(SensedActor->GetName()));
	}
}
