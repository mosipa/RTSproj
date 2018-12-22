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
	if (!Pawn) { return EBTNodeResult::Failed; }

	Target = Cast<AActor>(BlackboardComponent->GetValueAsObject("PlayerUnitKey"));
	if (!Target) { return EBTNodeResult::Failed; }

	//Get target location from Blackboard and set it for this moment (only do it once per arrest)
	if (!bPlayerUnitPrevLocationSet)
	{
		SetPlayerUnitPrevLocation(BlackboardComponent);
	}

	//TODO doesn't work when enemy AI tries to arrest 2 or more player units
	if (!bArrested && !(PlayerUnitPrevLocation.Equals(BlackboardComponent->GetValueAsVector("PlayerUnitLocation"))))
	{
		bPlayerUnitMoved = true;
	}

	//Rotate AI to face player unit 
	FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
	Pawn->SetActorRotation(BodyRotation);

	//Player unit tries to run away
	if (bPlayerUnitMoved)
	{
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 600.f;
		Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 600.f;
		BlackboardComponent->SetValueAsVector("PlayerUnitLocation", PlayerUnitPrevLocation);
		BlackboardComponent->SetValueAsBool("PlayerUnitOnMove", true);
		bArrested = false; 
		bPlayerUnitPrevLocationSet = false;
		bPlayerUnitMoved = false;
	}
	//Player unit doesn't move so get closer
	else
	{
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 200.f;
		UAIBlueprintHelperLibrary::SimpleMoveToActor(Cast<AEnemyAIController>(OwnerComp.GetOwner()), Target);

		float Distance = GetDistance(Pawn->GetActorLocation(), Target->GetActorLocation());

		//If close enough, make arrest - put him in prison
		if (GetDistance(Pawn->GetActorLocation(), Target->GetActorLocation()) <= 100.f)
		{
			bArrested = true;
			Target->SetActorRotation(Pawn->GetActorRotation());
			Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 200.f;
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cast<APawn>(Target)->GetController(), PRISON_LOCATION);
		}
	}

	float DistanceToPrison = GetDistance(Target->GetActorLocation(), PRISON_LOCATION);

	if (DistanceToPrison <= 100.f)
	{
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 600.f;
		Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 600.f;
		return EBTNodeResult::Succeeded;
	}

	//Update current player unit position
	PlayerUnitPrevLocation = Target->GetActorLocation();

	return EBTNodeResult::Succeeded;
}

void UMakeArrest::SetPlayerUnitPrevLocation(UBlackboardComponent* Blackboard)
{
	bPlayerUnitPrevLocationSet = true;
	PlayerUnitPrevLocation = Blackboard->GetValueAsVector("PlayerUnitLocation");
}

float UMakeArrest::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}