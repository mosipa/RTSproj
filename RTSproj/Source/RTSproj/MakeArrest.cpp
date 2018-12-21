// MOsipa 2018

#include "MakeArrest.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/World.h"

EBTNodeResult::Type UMakeArrest::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();
	if (!Pawn) { return EBTNodeResult::Aborted; }

	Target = Cast<AActor>(BlackboardComponent->GetValueAsObject("EnemyKey"));
	if (!Target) { return EBTNodeResult::Aborted; }

	//Get target location from Blackboard and set it for this moment (only do it once per arrest)
	if (!bEnemyPrevLocationSet)
	{
		SetEnemyPrevLocation(BlackboardComponent);
	}

	//TODO fix - it triggers even if enemy unit is under arrest and is going to the jail
	if (!bArrested && !(EnemyPrevLocation.Equals(BlackboardComponent->GetValueAsVector("EnemyLocation"))))
	{
		bEnemyMoved = true;
	}

	//Rotate AI to face Enemy 
	FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
	Pawn->SetActorRotation(BodyRotation);

	//Enemy tries to run away
	if (bEnemyMoved)
	{
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 600.f;
		Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 600.f;
		BlackboardComponent->SetValueAsVector("EnemyLocation", EnemyPrevLocation);
		BlackboardComponent->SetValueAsBool("EnemyOnMove", true);
		bArrested = false; 
		bEnemyPrevLocationSet = false;
		bEnemyMoved = false;
	}
	//Enemy doesn't move so get closer
	else
	{
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 200.f;
		UAIBlueprintHelperLibrary::SimpleMoveToActor(Cast<AEnemyAIController>(OwnerComp.GetOwner()), Target);

		float Distance = GetDistance(Pawn->GetActorLocation(), Target->GetActorLocation());
		float RequiredTime = Distance / Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed;

		//If close enough, make arrest - put him in prison
		if (GetDistance(Pawn->GetActorLocation(), Target->GetActorLocation()) <= 100.f)
		{
			bArrested = true;
			Target->SetActorRotation(Pawn->GetActorRotation());
			Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 200.f;
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cast<APawn>(Target)->GetController(), PRISON_LOCATION);
		}
	}

	//Update current enemy position
	EnemyPrevLocation = Target->GetActorLocation();

	return EBTNodeResult::Succeeded;
}

void UMakeArrest::SetEnemyPrevLocation(UBlackboardComponent* Blackboard)
{
	bEnemyPrevLocationSet = true;
	EnemyPrevLocation = Blackboard->GetValueAsVector("EnemyLocation");
}

float UMakeArrest::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}