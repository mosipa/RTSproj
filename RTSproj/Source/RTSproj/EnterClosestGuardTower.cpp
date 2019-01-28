// MOsipa 2018

#include "EnterClosestGuardTower.h"
#include "MyMathClass.h"
#include "EnemyAIController.h"
#include "GuardTower.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"

EBTNodeResult::Type UEnterClosestGuardTower::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	EnemyAIController = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	if (!EnemyAIController) { return EBTNodeResult::Failed; }

	//Go to GuardTower
	//TODO Go to nearest GuardTower
	if (EnemyAIController->GetGuardTowersInLevel().Num() <= 0) { return EBTNodeResult::Failed; }
	FVector GuardTowerLocation = EnemyAIController->GetGuardTowersInLevel()[0]->GetActorLocation();

	float Distance = MyMathClass::GetDistance(EnemyAIController->GetPawn()->GetActorLocation(), GuardTowerLocation);

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(EnemyAIController, GuardTowerLocation);

	if (Distance <= 150.f)
	{
		//Enter Tower
		UE_LOG(LogTemp, Warning, TEXT("Entering tower"));
	}

	return EBTNodeResult::Succeeded;
}