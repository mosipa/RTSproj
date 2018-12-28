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

	float Distance = GetDistance(Pawn->GetActorLocation(), LastKnownLocation);

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(EnemyAIController, LastKnownLocation);

	//BlackboardComponent->SetValueAsBool("PlayerUnitOnMove", true);
	//BlackboardComponent->ClearValue("LastKnownLocation");

	
	return EBTNodeResult::Succeeded;
}

float UCheckLastKnownLocation::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}