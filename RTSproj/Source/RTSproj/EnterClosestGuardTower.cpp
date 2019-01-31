// MOsipa 2018

#include "EnterClosestGuardTower.h"
#include "MyMathClass.h"
#include "EnemyAIController.h"
#include "GuardTower.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSEnemyUnit.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

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

	if (bIsUnitNearTower)
	{
		//Enter Tower
		TowerToEnter->UnitEntered(Cast<ARTSEnemyUnit>(EnemyAIController->GetPawn()));
		EnemyUnit->SetActorHiddenInGame(true);
		EnemyUnit->SetActorEnableCollision(false);
		EnemyUnit->GetMovementComponent()->StopMovementImmediately();

		if (!bHasTimerStarted)
		{
			bHasTimerStarted = true;
			GetWorld()->GetTimerManager().SetTimer(ExitingTimer, this, &UEnterClosestGuardTower::GettingOut, 15.f, false);
		}
	}

	return EBTNodeResult::Succeeded;
}

void UEnterClosestGuardTower::GettingOut()
{
	ARTSEnemyUnit* EnemyUnit = Cast<ARTSEnemyUnit>(EnemyAIController->GetPawn());
	AGuardTower* TowerToLeave = EnemyUnit->GetTower();

	if (TowerToLeave)
	{
		TowerToLeave->UnitLeft(EnemyUnit);

		EnemyUnit->SetActorHiddenInGame(false);
		EnemyUnit->SetActorEnableCollision(true);
		EnemyUnit->InsideTower(nullptr);
		EnemyUnit->SetNearTower(false);

		bHasTimerStarted = false;
		BlackboardComponent->ClearValue("Alarm");
	}

	GetWorld()->GetTimerManager().ClearTimer(ExitingTimer);
}