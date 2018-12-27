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

AEnemyAIController::AEnemyAIController()
{
	BlackboardAsset = NewObject<UBlackboardData>();

	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("NextWaypoint"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Int>(FName("IndexKey"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(FName("PlayerUnitKey"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("PlayerUnitLocation"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("LastKnownLocation"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("PlayerUnitOnMove"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("SensedUnit"));

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
	if (Stimulus.WasSuccessfullySensed())
	{
		//If player unit was spotted
		//And player unit isn't dead already
		//And player unit isn't arrested already
		if (SensedActor->GetClass()->IsChildOf<ARTSPlayerUnit>()
			&& !(Cast<ARTSPlayerUnit>(SensedActor)->IsCharacterDead())
			&& !(Cast<ARTSPlayerUnit>(SensedActor)->IsCharacterArrested()))
		{
			this->BlackboardComponent->SetValueAsObject("PlayerUnitKey", SensedActor);
			this->BlackboardComponent->SetValueAsVector("PlayerUnitLocation", SensedActor->GetActorLocation());
			this->BlackboardComponent->SetValueAsBool("PlayerUnitOnMove", false);
			this->BlackboardComponent->SetValueAsBool("SensedUnit", true);
		}
	}
	//If noone is in sight radius
	else
	{
		this->BlackboardComponent->ClearValue("SensedUnit");
		this->BlackboardComponent->ClearValue("PlayerUnitKey");
		this->BlackboardComponent->ClearValue("PlayerUnitLocation");
		this->BlackboardComponent->ClearValue("PlayerUnitOnMove");
	}
}
