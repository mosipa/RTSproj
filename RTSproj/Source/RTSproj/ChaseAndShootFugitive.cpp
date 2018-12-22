// MOsipa 2018

#include "ChaseAndShootFugitive.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Projectile.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSCharacter.h"

EBTNodeResult::Type UChaseAndShootFugitive::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("Fugitive trying to escape"));

	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();
	if (!Pawn) { return EBTNodeResult::Aborted; }
	
	FVector TargetLocation = BlackboardComponent->GetValueAsVector("PlayerUnitLocation");
	auto Target = Cast<ARTSCharacter>(BlackboardComponent->GetValueAsObject("PlayerUnitKey"));
	float Distance = GetDistance(Pawn->GetActorLocation(), TargetLocation);

	//Start chasing fugitive
	UAIBlueprintHelperLibrary::SimpleMoveToActor(Cast<AEnemyAIController>(OwnerComp.GetOwner()), Target);

	//If close enough start shooting
	if (Distance <= 400.f)
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
		UE_LOG(LogTemp, Warning, TEXT("Fugitive: %s died"), *(Target->GetName()));
		BlackboardComponent->ClearValue("PlayerUnitKey");
		BlackboardComponent->ClearValue("PlayerUnitLocation");
		BlackboardComponent->ClearValue("PlayerUnitOnMove");
	}

	return EBTNodeResult::Succeeded;
}

float UChaseAndShootFugitive::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}