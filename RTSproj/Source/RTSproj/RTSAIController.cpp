// MOsipa 2018

#include "RTSAIController.h"
#include "RTSprojCharacter.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

float ARTSAIController::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}

void ARTSAIController::Move(FVector MoveTo)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MoveTo);
}

void ARTSAIController::Knife(FHitResult Hit)
{	
	if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>())
	{
		Target = Hit.GetActor();

		//Prepare for attack only if you have target,
		//you're not the target
		//and your target isnt dead already
		if (Target && !(this->GetPawn()->GetName().Equals(Target->GetName()))
			&& !Cast<ARTSprojCharacter>(Target)->IsCharacterDead())
		{
			PrepareAttack();
		}
	}
}

void ARTSAIController::PrepareAttack()
{
	float Distance = GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation());
	float MaxSpd = this->GetPawn()->GetMovementComponent()->GetMaxSpeed();
	float TimeToReach = Distance / MaxSpd;

	float DotProd = Target->GetDotProductTo(this->GetPawn());

	//Behind the target
	if (DotProd <= 0)
	{
		Damage = 100.f;
	}
	//In front of the target
	else
	{
		Damage = 50.f;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToActor(this, Target);

	//TODO distance between points is taken in straight line, ignoring any obsticles actor preparing attack may occur
	GetWorld()->GetTimerManager().SetTimer(KnifeTimerHandle, this, &ARTSAIController::PerformAttack, TimeToReach, false);
}

void ARTSAIController::PerformAttack()
{
	if (Target)
	{
		//Rotation of a body (in case of player is in range so he doesnt have to move around to the target location)
		FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(this->GetPawn()->GetActorLocation(), Target->GetActorLocation());
		this->GetPawn()->SetActorRotation(BodyRotation);

		UGameplayStatics::ApplyDamage(
			Target,
			Damage,
			this,
			this->GetPawn(),
			UDamageType::StaticClass()
		);
	}

	GetWorld()->GetTimerManager().ClearTimer(KnifeTimerHandle);
	Target = nullptr;
}

void ARTSAIController::PresentYourself()
{
	UE_LOG(LogTemp, Warning, TEXT("Actor: %s has controller: %s"), *(this->GetPawn()->GetName()), *(this->GetName()));
}
