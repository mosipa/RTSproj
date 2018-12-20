// MOsipa 2018

#include "EnemySpotted.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"


EBTNodeResult::Type UEnemySpotted::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
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

		UE_LOG(LogTemp, Warning, TEXT("Spotted: %s"), *(Target->GetName()));

		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}