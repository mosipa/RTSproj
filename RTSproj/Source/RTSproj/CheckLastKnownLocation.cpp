// MOsipa 2018

#include "CheckLastKnownLocation.h"
#include "EnemyAIController.h"
#include "RTSCharacter.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UCheckLastKnownLocation::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	EnemyAIController = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	if (!EnemyAIController) { return EBTNodeResult::Failed; }

	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();
	if (!Pawn) { return EBTNodeResult::Failed; }

	LastKnownLocation = BlackboardComponent->GetValueAsVector("LastKnownLocation");
	bool bInSight = BlackboardComponent->GetValueAsBool("PlayerInSight");

	UE_LOG(LogTemp, Warning, TEXT("Sight: %i"), bInSight);

	if(!bInSight)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(EnemyAIController, LastKnownLocation);

		if (Pawn->GetActorLocation().Equals(LastKnownLocation, 100.f))
		{
			BlackboardComponent->ClearValue("LastKnownLocation");
			BlackboardComponent->ClearValue("PlayerUnitKey");
			BlackboardComponent->ClearValue("PlayerUnitOnMove");
			BlackboardComponent->ClearValue("PlayerInSight");
			BlackboardComponent->ClearValue("PlayerInRange");
		}
	}

	return EBTNodeResult::Succeeded;
}