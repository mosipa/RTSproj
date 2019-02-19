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

	//Check if Towers are in level
	if (EnemyAIController->GetGuardTowersInLevel().Num() <= 0) 
	{ 
		UE_LOG(LogTemp, Error, TEXT("There are no towers in level. Add Towers to Level."));
		return EBTNodeResult::Failed; 
	}

	//Set closest tower, if not set already
	if (!bHasClosestTowerSet)
	{
		bHasClosestTowerSet = true;
		GetClosestTower();
	}
	
	if (!ClosestTower) { return EBTNodeResult::Failed; }

	FVector GuardTowerLocation = ClosestTower->GetActorLocation();

	float Distance = MyMathClass::GetDistance(EnemyAIController->GetPawn()->GetActorLocation(), GuardTowerLocation);

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(EnemyAIController, GuardTowerLocation);
	ARTSEnemyUnit* EnemyUnit = Cast<ARTSEnemyUnit>(EnemyAIController->GetPawn());

	bool bIsUnitNearTower = EnemyUnit->IsNearTower();

	if (bIsUnitNearTower)
	{
		//Enter Tower
		ClosestTower->UnitEntered(Cast<ARTSEnemyUnit>(EnemyAIController->GetPawn()));
		EnemyUnit->SetActorHiddenInGame(true);
		EnemyUnit->SetActorEnableCollision(false);
		EnemyUnit->InsideTower(ClosestTower);
		EnemyUnit->SetNearTower(true);
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

		//Reset this values
		bHasTimerStarted = false;
		bHasClosestTowerSet = false;
		DistanceMin = 10000.f;
		BlackboardComponent->ClearValue("Alarm");
	}

	GetWorld()->GetTimerManager().ClearTimer(ExitingTimer);
}

void UEnterClosestGuardTower::GetClosestTower()
{
	for (auto& GT : EnemyAIController->GetGuardTowersInLevel())
	{
		float Distance = MyMathClass::GetDistance(GT->GetActorLocation(), EnemyAIController->GetPawn()->GetActorLocation());

		if (DistanceMin > Distance)
		{
			DistanceMin = Distance;
			ClosestTower = GT;
		}
	}
}