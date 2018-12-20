// MOsipa 2018

#include "MakeArrest.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"


EBTNodeResult::Type UMakeArrest::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();
	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();
	
	if (Pawn)
	{
		//Stops AI
		Pawn->GetMovementComponent()->StopMovementImmediately();

		auto Target = Cast<AActor>(Blackboard->GetValueAsObject("EnemyKey"));

		//Rotate AI to face Enemy 
		FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
		Pawn->SetActorRotation(BodyRotation);

		return EBTNodeResult::InProgress;
	}	
	else
	{
		return EBTNodeResult::Failed;
	}	
}