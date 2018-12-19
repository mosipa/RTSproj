// MOsipa 2018

#include "EnemyAIController.h"
#include "RTSCharacter.h"
#include "AIModule/Classes/BehaviorTree/BlackboardData.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"

AEnemyAIController::AEnemyAIController()
{
	BlackboardAsset = NewObject<UBlackboardData>();

	//TEST KEYS
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("Location1"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("Location2"));

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Blueprints/EnemyBT"));
	
	if (BTObject.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("succeeded"));
		BehaviorTree = BTObject.Object;
		BehaviorTree->BlackboardAsset = BlackboardAsset;
	}	
}

void AEnemyAIController::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	ARTSCharacter* Character = Cast<ARTSCharacter>(Pawn);

	if (Character)
	{
		BlackboardComponent->InitializeBlackboard(*BlackboardAsset);

		//TEST VALUES
		BlackboardComponent->SetValueAsVector("Location1", FVector(800, -1300, 270));
		BlackboardComponent->SetValueAsVector("Location2", FVector(-1150, 1400, 270));
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


