// MOsipa 2018

#include "PlayerUnitSpotted.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"


EBTNodeResult::Type UPlayerUnitSpotted::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();
	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();

	if (Pawn)
	{
		//Stops AI
		Pawn->GetMovementComponent()->StopMovementImmediately();
		
		auto Target = Cast<AActor>(BlackboardComponent->GetValueAsObject("PlayerUnitKey"));

		//BlackboardComponent->SetValueAsVector("LastKnownLocation", Target->GetActorLocation());
		BlackboardComponent->SetValueAsBool("PlayerUnitOnMove", false);

		//Rotate AI to face player unit 
		FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
		Pawn->SetActorRotation(BodyRotation);

		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}