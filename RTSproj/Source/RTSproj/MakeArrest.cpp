// MOsipa 2018

#include "MakeArrest.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"


EBTNodeResult::Type UMakeArrest::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();
	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();

	auto Target = Cast<AActor>(BlackboardComponent->GetValueAsObject("EnemyKey"));

	//Get target location from Blackboard and set it for this moment (only do it once per arrest)
	if (!IsEnemyPrevLocationSet())
	{
		SetEnemyPrevLocation(BlackboardComponent);
	}
	
	if (EnemyPrevLocation.Equals(BlackboardComponent->GetValueAsVector("EnemyLocation")))
	{
		bEnemyMoved = false;
	}
	else
	{
		bEnemyMoved = true;
	}

	//Rotate AI to face Enemy 
	FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
	Pawn->SetActorRotation(BodyRotation);

	//TODO add logic (follow/shoot enemy)
	if (bEnemyMoved)
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY MOVES"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY STAYS"));
	}

	//Update current enemy position
	EnemyPrevLocation = Target->GetActorLocation();

	return EBTNodeResult::Succeeded;
}

void UMakeArrest::SetEnemyPrevLocation(UBlackboardComponent* Blackboard)
{
	bEnemyPrevLocationSet = true;
	EnemyPrevLocation = Blackboard->GetValueAsVector("EnemyLocation");
}

bool UMakeArrest::IsEnemyPrevLocationSet()
{
	return bEnemyPrevLocationSet;
}