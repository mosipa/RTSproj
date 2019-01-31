// MOsipa 2018

#include "EnterClosestGuardTower.h"
#include "MyMathClass.h"
#include "EnemyAIController.h"
#include "GuardTower.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSEnemyUnit.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UEnterClosestGuardTower::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	EnemyAIController = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	if (!EnemyAIController) { return EBTNodeResult::Failed; }

	//Go to GuardTower
	//TODO Go to nearest GuardTower
	if (EnemyAIController->GetGuardTowersInLevel().Num() <= 0) { return EBTNodeResult::Failed; }
	AGuardTower* TowerToEnter = EnemyAIController->GetGuardTowersInLevel()[0];
	FVector GuardTowerLocation = TowerToEnter->GetActorLocation();

	float Distance = MyMathClass::GetDistance(EnemyAIController->GetPawn()->GetActorLocation(), GuardTowerLocation);

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(EnemyAIController, GuardTowerLocation);
	ARTSEnemyUnit* EnemyUnit = Cast<ARTSEnemyUnit>(EnemyAIController->GetPawn());

	bool bIsUnitNearTower = EnemyUnit->IsNearTower();
	UE_LOG(LogTemp, Warning, TEXT("%s, %i"), *(EnemyUnit->GetName()), bIsUnitNearTower);
	if (bIsUnitNearTower)
	{
		//Enter Tower
		TowerToEnter->UnitEntered(Cast<ARTSEnemyUnit>(EnemyAIController->GetPawn()));
		EnemyUnit->SetActorHiddenInGame(true);
		EnemyUnit->SetActorEnableCollision(false);
		EnemyUnit->GetMovementComponent()->StopMovementImmediately();
		UE_LOG(LogTemp, Warning, TEXT("%s in Tower"), *(EnemyUnit->GetName()));
	}
	UE_LOG(LogTemp, Warning, TEXT("Entering closest tower"));
	return EBTNodeResult::Succeeded;
}