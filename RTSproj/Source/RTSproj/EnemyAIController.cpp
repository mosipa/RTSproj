// MOsipa 2018

#include "EnemyAIController.h"
#include "RTSEnemyUnit.h"
#include "AIModule/Classes/BehaviorTree/BlackboardData.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"

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


