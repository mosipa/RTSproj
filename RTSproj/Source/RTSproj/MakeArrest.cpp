// MOsipa 2018

#include "MakeArrest.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/World.h"
#include "RTSAIController.h"
#include "RTSPlayerUnit.h"
#include "MyMathClass.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Prison.h"

EBTNodeResult::Type UMakeArrest::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();
	if (!Pawn) { return EBTNodeResult::Failed; }

	Target = Cast<ARTSPlayerUnit>(BlackboardComponent->GetValueAsObject("PlayerUnitKey"));
	if (!Target) { return EBTNodeResult::Failed; }

	if (!bFindPrisonLocation)
	{
		FindPrisonLocation();
	}

	bool bPlayerUnitBehaveWierd = Cast<ARTSAIController>(Target->GetController())->IsUnitBusy();

	//Rotate AI to face player unit 
	FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
	Pawn->SetActorRotation(BodyRotation);

	float DistanceToPrison = MyMathClass::GetDistance(Target->GetActorLocation(), PRISON_LOCATION);
	float Distance = MyMathClass::GetDistance(Pawn->GetActorLocation(), Target->GetActorLocation());
	
	UAIBlueprintHelperLibrary::SimpleMoveToActor(Pawn->GetController(), Target);

	if (Distance <= 400.f)
	{
		BlackboardComponent->SetValueAsBool("PlayerInRange", true);
	}
	else
	{
		BlackboardComponent->SetValueAsBool("PlayerInRange", false);
	}

	//Player unit tries to do something
	if (bPlayerUnitBehaveWierd)
	{
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 450.f;
		Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 600.f;
		BlackboardComponent->SetValueAsBool("PlayerUnitOnMove", true);
		bCollisionToggle = false;
	}
	//Player unit doesn't move so get closer
	else if(!bPlayerUnitBehaveWierd)
	{
		//If close enough, make arrest - put him in prison
		if (Distance <= 100.f)
		{
			if (!bCollisionToggle)
			{
				Pawn->SetActorEnableCollision(false);
				bCollisionToggle = true;
				GetWorld()->GetTimerManager().SetTimer(CollisionTimerHandle, this, &UMakeArrest::PutCollisionBackOn, 0.75f, false);
			}
			
			Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 200.f;
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cast<APawn>(Target)->GetController(), PRISON_LOCATION);

			if (DistanceToPrison > 150.f)
			{
				Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 200.f;
				UAIBlueprintHelperLibrary::SimpleMoveToActor(Cast<AEnemyAIController>(OwnerComp.GetOwner()), Target);
			}
			else
			{
				Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 600.f;
				
				Target->SetArrested(true);
				Target->HealthBarInvisible(true);
				Target->GetMovementComponent()->StopMovementImmediately();
				Target->SetInBuilding(true);
				Target->SetActorHiddenInGame(true);
				Target->SetActorEnableCollision(false);

				//Add unit to buildings array
				if (Prison)
				{
					Prison->UnitEntered(Target);
				}

				//TODO cancel selection of unit that just went to the prison

				BlackboardComponent->ClearValue("PlayerUnitKey");
				BlackboardComponent->ClearValue("PlayerUnitOnMove");
				BlackboardComponent->ClearValue("PlayerInSight");
				BlackboardComponent->ClearValue("PlayerInRange");
				bCollisionToggle = false; 
			}
		}
	}

	return EBTNodeResult::Succeeded;
}

void UMakeArrest::PutCollisionBackOn()
{
	Pawn->SetActorEnableCollision(true);
	GetWorld()->GetTimerManager().ClearTimer(CollisionTimerHandle);
}

void UMakeArrest::FindPrisonLocation()
{
	bFindPrisonLocation = true;

	TArray<AActor*> Prisons;
	UGameplayStatics::GetAllActorsOfClass(this, APrison::StaticClass(), Prisons);

	//TODO maybe there's a better way
	//TODO maybe add multiple prisons and find the closest one?
	if (Prisons.Num() > 0)
	{
		Prison = Cast<APrison>(Prisons[0]);
		PRISON_LOCATION = Prison->GetPrisonLocation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No prison in level!"));
		Prison = nullptr;
		PRISON_LOCATION = FVector(0.f, 0.f, 0.f);
	}
}