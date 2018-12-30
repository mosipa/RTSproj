// MOsipa 2018

#include "ChaseAndShootFugitive.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Projectile.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSCharacter.h"
#include "RTSPlayerUnit.h"

EBTNodeResult::Type UChaseAndShootFugitive::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();
	if (!Pawn) { return EBTNodeResult::Failed; }
	
	auto Target = Cast<ARTSCharacter>(BlackboardComponent->GetValueAsObject("PlayerUnitKey"));
	FVector TargetLocation = BlackboardComponent->GetValueAsVector("LastKnownLocation");

	float Distance = GetDistance(Pawn->GetActorLocation(), TargetLocation);

	//Start chasing fugitive
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cast<AEnemyAIController>(OwnerComp.GetOwner()), TargetLocation);

	//If close enough start shooting
	//TODO shooting = 100% death of unit - make some sort of adjustment so playerunit has a chance to survive
	if (Distance <= 400.f)
	{
		BlackboardComponent->SetValueAsBool("PlayerInRange", true);
	}
	else
	{
		BlackboardComponent->SetValueAsBool("PlayerInRange", false);
	}

	bool bInRange = BlackboardComponent->GetValueAsBool("PlayerInRange");
	bool bInSight = BlackboardComponent->GetValueAsBool("PlayerInSight");

	UE_LOG(LogTemp, Warning, TEXT("Sight: %i, Range: %i"), bInSight, bInRange);

	if (bInRange && bInSight)
	{
		//Rotate AI to face Enemy 
		FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
		Pawn->SetActorRotation(BodyRotation);

		Pawn->GetMovementComponent()->StopMovementImmediately();

		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 100.f,
			Pawn->GetActorRotation()
			);

		if (!Projectile) { return EBTNodeResult::Aborted; }
		Projectile->LaunchProjectile();
	}

	//If fugitive dies clear all keys
	if (Target->IsCharacterDead())
	{
		BlackboardComponent->ClearValue("PlayerUnitKey");
		BlackboardComponent->ClearValue("PlayerUnitOnMove");
		BlackboardComponent->ClearValue("LastKnownLocation");
		BlackboardComponent->ClearValue("PlayerInRange");
		BlackboardComponent->ClearValue("PlayerInSight");
	}

	return EBTNodeResult::Succeeded;
}

float UChaseAndShootFugitive::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}