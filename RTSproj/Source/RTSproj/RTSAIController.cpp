// MOsipa 2018

#include "RTSAIController.h"
#include "RTSprojCharacter.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Projectile.h"

float ARTSAIController::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}

void ARTSAIController::Move(FVector MoveTo)
{
	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MoveTo);
}

void ARTSAIController::Knife(FHitResult Hit)
{	
	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);

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

	InteralUnitState = EUnitState::Knifing;
	//TODO distance between points is taken in straight line, ignoring any obsticles actor preparing attack may occur
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ARTSAIController::PerformAttack, TimeToReach, false);
}

void ARTSAIController::FirePistol(FHitResult Hit)
{
	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);

	if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>())
	{
		Target = Hit.GetActor();

		//Prepare for attack only if you have target,
		//you're not the target
		//and your target isnt dead already
		if (Target && !(this->GetPawn()->GetName().Equals(Target->GetName()))
			&& !Cast<ARTSprojCharacter>(Target)->IsCharacterDead())
		{
			PrepareToFire();
		}
	}
}

void ARTSAIController::PrepareToFire()
{
	float Distance = GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation());
	float TimeToReach;

	//Range is 450.f
	if (Distance > 450.f) 
	{ 
		Distance -= 450.f; 
		float MaxSpd = this->GetPawn()->GetMovementComponent()->GetMaxSpeed();
		TimeToReach = Distance / MaxSpd;

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this->GetPawn()->GetController(), Target->GetActorLocation());
	}
	else 
	{ 
		TimeToReach = 0.1f;
	}

	InteralUnitState = EUnitState::FiringGun;
	//TODO distance between points is taken in straight line, ignoring any obsticles actor preparing attack may occur
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ARTSAIController::PerformAttack, TimeToReach, false);
}

void ARTSAIController::PerformAttack()
{
	if (Target)
	{
		//Rotation of a body (in case of player is in range so he doesnt have to move around to the target location)
		FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(this->GetPawn()->GetActorLocation(), Target->GetActorLocation());
		this->GetPawn()->SetActorRotation(BodyRotation);

		if (InteralUnitState == EUnitState::Knifing)
		{
			UGameplayStatics::ApplyDamage(
				Target,
				Damage,
				this,
				this->GetPawn(),
				UDamageType::StaticClass()
			);
		}
		else if (InteralUnitState == EUnitState::FiringGun)
		{
			this->GetPawn()->GetMovementComponent()->StopMovementImmediately();

			auto Projectile = GetWorld()->SpawnActor<AProjectile>(
				this->GetPawn()->GetActorLocation() + this->GetPawn()->GetActorForwardVector() * 100.f,
				this->GetPawn()->GetActorRotation()
				);

			if (!Projectile) { return; }
			Projectile->LaunchProjectile();
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	Target = nullptr;
}

void ARTSAIController::Aid(FHitResult Hit, EUnitState UnitState)
{
	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle); 
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);

	if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>())
	{
		Target = Hit.GetActor();
		if (UnitState == EUnitState::Cleansing)
		{
			InteralUnitState = EUnitState::Cleansing;
			AidTime = 5.f;
		}
		else if (UnitState == EUnitState::Healing)
		{
			InteralUnitState = EUnitState::Healing;
			AidTime = 2.f;
		}

		//There's character to heal
		//and he's not dead
		if (Target && !Cast<ARTSprojCharacter>(Target)->IsCharacterDead())
		{
			//Only do it if Target is bleeding or is injured
			if (Cast<ARTSprojCharacter>(Target)->IsCharacterBleeding()
				|| Cast<ARTSprojCharacter>(Target)->IsCharacterInjured())
			{
				PrepareAid();
			}
		}
	}

}

void ARTSAIController::PrepareAid()
{
	float Distance = GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation());
	float MaxSpd = this->GetPawn()->GetMovementComponent()->GetMaxSpeed();
	float TimeToReach = Distance / MaxSpd;

	if (!Target->GetName().Equals(this->GetPawn()->GetName()))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(this, Target);
		//As character that aids isn't the target there's no penalty for Aiding
		SelfAidPenalty = 0.f;
	}
	//If target equals character, it takes him longer to self-heal or self-cleanse
	else
	{
		SelfAidPenalty = 2.f;
		TimeToReach = 0.1f;
	}

	//TODO distance between points is taken in straight line, ignoring any obsticles actor preparing attack may occur
	GetWorld()->GetTimerManager().SetTimer(GettingCloserTimerHandle, this, &ARTSAIController::GetCloserForAid, TimeToReach, false);
}

void ARTSAIController::GetCloserForAid()
{
	if (Target)
	{
		//Rotation of a actor's body (in case of player is in range so he doesnt have to move around to the target location)
		//Rotation of target's body (faces actor that's aiding him)
		//Only do it if the target and actor isn't the same character
		if (!Target->GetName().Equals(this->GetPawn()->GetName()))
		{
			FRotator ActorBodyRotation = UKismetMathLibrary::FindLookAtRotation(this->GetPawn()->GetActorLocation(), Target->GetActorLocation());
			this->GetPawn()->SetActorRotation(ActorBodyRotation);

			FRotator TargetBodyRotation = UKismetMathLibrary::FindLookAtRotation(Target->GetActorLocation(), this->GetPawn()->GetActorLocation());
			Target->SetActorRotation(TargetBodyRotation);
		}

		GetWorld()->GetTimerManager().SetTimer(AidTimerHandle, this, &ARTSAIController::PerformAid, AidTime + SelfAidPenalty, false);
	}

	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle);
}

void ARTSAIController::PerformAid()
{
	if (InteralUnitState == EUnitState::Cleansing)
	{
		Cast<ARTSprojCharacter>(Target)->StopBleeding();
	}
	else if (InteralUnitState == EUnitState::Healing)
	{
		Cast<ARTSprojCharacter>(Target)->AddHealth(20.f);
		UE_LOG(LogTemp, Warning, TEXT("Target: %s healed for 20.f. Health left: %f"), *(Target->GetName()), Cast<ARTSprojCharacter>(Target)->GetHealth());
	}

	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);
	Target = nullptr;
}


void ARTSAIController::PresentYourself()
{
	UE_LOG(LogTemp, Warning, TEXT("Actor: %s has controller: %s"), *(this->GetPawn()->GetName()), *(this->GetName()));
}
