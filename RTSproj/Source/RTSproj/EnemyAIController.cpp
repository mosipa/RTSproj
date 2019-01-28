// MOsipa 2018

#include "EnemyAIController.h"
#include "RTSEnemyUnit.h"
#include "AIModule/Classes/BehaviorTree/BlackboardData.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "AIModule/Classes/Perception/AISenseConfig_Sight.h"
#include "RTSPlayerUnit.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GuardTower.h"

AEnemyAIController::AEnemyAIController()
{
	BlackboardAsset = NewObject<UBlackboardData>();

	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("NextWaypoint"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Int>(FName("IndexKey"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(FName("PlayerUnitKey"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("LastKnownLocation"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("PlayerUnitOnMove"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("PlayerInSight"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("PlayerInRange"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("LocationIsSet"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("Alarm"));

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
		Sight->SightRadius = 1000;
		Sight->LoseSightRadius = 1100;
		Sight->PeripheralVisionAngleDegrees = 114.f; //Actual human peripheral vision angle 
		Sight->DetectionByAffiliation.bDetectEnemies = true;
		Sight->DetectionByAffiliation.bDetectNeutrals = true;
		Sight->DetectionByAffiliation.bDetectFriendlies = true;

		AIPerceptionComponent->ConfigureSense(*Sight);
	}

	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);

	GuardTowersInLevel.Empty();
}

void AEnemyAIController::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	ARTSEnemyUnit* Character = Cast<ARTSEnemyUnit>(Pawn);

	if (Character)
	{
		BlackboardComponent->InitializeBlackboard(*BlackboardAsset);

		//Get all towers in level
		GetAllGuardTowers();
	}
}

void AEnemyAIController::GetAllGuardTowers()
{
	TArray<AActor*> GuardTowers;
	UGameplayStatics::GetAllActorsOfClass(this, AGuardTower::StaticClass(), GuardTowers);

	for (auto& Tower : GuardTowers)
	{
		GuardTowersInLevel.Add(Cast<AGuardTower>(Tower));
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
	if (Stimulus.WasSuccessfullySensed())
	{
		//If player unit was spotted
		//And player unit isn't dead already
		//And player unit isn't arrested already
		//And player unit isn't inside a building
		if (SensedActor->GetClass()->IsChildOf<ARTSPlayerUnit>()
			&& !(Cast<ARTSPlayerUnit>(SensedActor)->IsCharacterDead())
			&& !(Cast<ARTSPlayerUnit>(SensedActor)->IsCharacterArrested())
			&& !(Cast<ARTSPlayerUnit>(SensedActor)->IsCharacterInBuilding()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Sensed: %s"), *(SensedActor->GetName()));
			this->BlackboardComponent->SetValueAsObject("PlayerUnitKey", SensedActor);
			this->BlackboardComponent->SetValueAsBool("PlayerInSight", true);
		}
	}
	else
	{
		this->BlackboardComponent->SetValueAsBool("PlayerInSight", false);
	}
}