// MOsipa 2018

#include "RTSAIController.h"
#include "RTSCharacter.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "MyMathClass.h"
#include "Building.h"
#include "RTSPlayerUnit.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "RTSHud.h"
#include "Prison.h"
#include "PlayersHideout.h"

ARTSAIController::ARTSAIController()
{
	bUnitBusy = false;
}

void ARTSAIController::Move(FVector MoveTo)
{
	bUnitBusy = true;

	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(EnterBuildingTimerHandle);

	float Distance = MyMathClass::GetDistance(this->GetPawn()->GetActorLocation(), MoveTo);
	float MaxSpd = this->GetPawn()->GetMovementComponent()->GetMaxSpeed();
	float RequiredTime = Distance / MaxSpd;

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MoveTo);

	GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &ARTSAIController::PerformMove, RequiredTime, false);
}

void ARTSAIController::PerformMove()
{
	bUnitBusy = false;
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
}

void ARTSAIController::GetCloserToBuilding(ABuilding* Building, ARTSHud* HUDPtr, EUnitState UnitState)
{
	FTimerDelegate PerformDelegate;

	bUnitBusy = true;

	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(EnterBuildingTimerHandle);

	FVector MoveTo = Building->GetRootComponent()->GetComponentLocation();
	float Distance = MyMathClass::GetDistance(this->GetPawn()->GetActorLocation(), MoveTo);
	float MaxSpd = this->GetPawn()->GetMovementComponent()->GetMaxSpeed();
	float RequiredTime = Distance / MaxSpd ;

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MoveTo);

	if (UnitState == EUnitState::Entering)
	{
		PerformDelegate.BindUFunction(this, FName("PerformEnterBuilding"), Building, HUDPtr);
	}
	else if (UnitState == EUnitState::Releasing)
	{
		PerformDelegate.BindUFunction(this, FName("PerformReleasePrisoners"), Building);
	}
	
	GetWorld()->GetTimerManager().SetTimer(EnterBuildingTimerHandle, PerformDelegate, RequiredTime, false);
}

void ARTSAIController::PerformEnterBuilding(APlayersHideout* TargetBuilding, ARTSHud* HUDPtr)
{
	ARTSPlayerUnit* PlayerUnit = Cast<ARTSPlayerUnit>(this->GetPawn());

	if (PlayerUnit->IsCharacterInBuildingsRange()
		&& PlayerUnit->GetNearestBuilding()
		&& TargetBuilding)
	{
		if (PlayerUnit->GetNearestBuilding()->GetName().Equals(TargetBuilding->GetName()))
		{
			PlayerUnit->HealthBarInvisible(true);
			PlayerUnit->GetMovementComponent()->StopMovementImmediately();
			PlayerUnit->SetInBuilding(true);
			PlayerUnit->SetActorHiddenInGame(true);
			PlayerUnit->SetActorEnableCollision(false);

			//Add unit to buildings array
			TargetBuilding->UnitEntered(PlayerUnit);

			//Unselect unit that enetered building
			HUDPtr->RemoveUnitFromSelection();

			bUnitBusy = false;
			GetWorld()->GetTimerManager().ClearTimer(EnterBuildingTimerHandle);			
		}
	}
	else
	{
		this->GetCloserToBuilding(TargetBuilding, HUDPtr, EUnitState::Entering);
	}
	
}

void ARTSAIController::PerformReleasePrisoners(APrison* Prison)
{
	ARTSPlayerUnit* PlayerUnit = Cast<ARTSPlayerUnit>(this->GetPawn());

	if (PlayerUnit->IsCharacterInBuildingsRange()
		&& PlayerUnit->GetNearestBuilding()
		&& Prison)
	{
		if (PlayerUnit->GetNearestBuilding()->GetName().Equals(Prison->GetName()))
		{
			Prison->ReleaseUnits();
		}
	}
	else
	{
		this->GetCloserToBuilding(Prison, nullptr, EUnitState::Releasing);
	}
}

void ARTSAIController::Knife(FHitResult Hit)
{	
	bUnitBusy = true;

	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(EnterBuildingTimerHandle);

	if (Hit.GetActor()->GetClass()->IsChildOf<ARTSCharacter>())
	{
		Target = Hit.GetActor();

		//Prepare for attack only if you have target,
		//you're not the target
		//and your target isnt dead already
		if (Target && !(this->GetPawn()->GetName().Equals(Target->GetName()))
			&& !Cast<ARTSCharacter>(Target)->IsCharacterDead())
		{
			PrepareAttack();
		}
	}
}

void ARTSAIController::PrepareAttack()
{
	float Distance = MyMathClass::GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation());
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
	bUnitBusy = true;

	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(EnterBuildingTimerHandle);

	if (Hit.GetActor()->GetClass()->IsChildOf<ARTSCharacter>())
	{
		Target = Hit.GetActor();

		//Prepare for attack only if you have target,
		//you're not the target
		//and your target isnt dead already
		if (Target && !(this->GetPawn()->GetName().Equals(Target->GetName()))
			&& !Cast<ARTSCharacter>(Target)->IsCharacterDead())
		{
			PrepareToFire();
		}
	}
}

void ARTSAIController::PrepareToFire()
{
	float Distance = MyMathClass::GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation());
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
			//Check if close enough to perform stabbing
			if (MyMathClass::GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation()) <= 150.f)
			{
				UGameplayStatics::ApplyDamage(
					Target,
					Damage,
					this,
					this->GetPawn(),
					UDamageType::StaticClass()
				);

				//IMPORTANT:
				//only clear those values AFTER attack has been performed!
				bUnitBusy = false;
				GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
				Target = nullptr;
			}
			else
			{
				this->PrepareAttack();
			}
		}
		else if (InteralUnitState == EUnitState::FiringGun)
		{
			//Check if close enough to perform shooting
			if (MyMathClass::GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation()) <= 450.f)
			{
				this->GetPawn()->GetMovementComponent()->StopMovementImmediately();

				Cast<ARTSPlayerUnit>(this->GetPawn())->Shoot();

				//IMPORTANT:
				//only clear those values AFTER attack has been performed!
				bUnitBusy = false;
				GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
				Target = nullptr;
			}
			else
			{
				this->PrepareToFire();
			}
		}
	}
}

void ARTSAIController::Aid(FHitResult Hit, EUnitState UnitState)
{
	bUnitBusy = true;

	//Clearing timers of other actions (in case player changed his mind)
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GettingCloserTimerHandle); 
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(EnterBuildingTimerHandle);

	if (Hit.GetActor()->GetClass()->IsChildOf<ARTSCharacter>())
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
		if (Target && !Cast<ARTSCharacter>(Target)->IsCharacterDead())
		{
			//Only do it if Target is bleeding or is injured
			if (Cast<ARTSCharacter>(Target)->IsCharacterBleeding()
				|| Cast<ARTSCharacter>(Target)->IsCharacterInjured())
			{
				PrepareAid();
			}
		}
	}

}

void ARTSAIController::PrepareAid()
{
	float Distance = MyMathClass::GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation());
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
	//Prevent healing/cleansing unit if target unit moved away
	//Check if close enough to perform healing/cleansing
	//TODO choose between: 
	//Healing/cleansing is fully stopped if target of heal/cleanse has moved <-- This one is set now
	//Or chase the target and try to perform it when it stops again
	if (MyMathClass::GetDistance(Target->GetActorLocation(), this->GetPawn()->GetActorLocation()) <= 150.f)
	{
		if (InteralUnitState == EUnitState::Cleansing)
		{
			Cast<ARTSCharacter>(Target)->StopBleeding();
		}
		else if (InteralUnitState == EUnitState::Healing)
		{
			Cast<ARTSCharacter>(Target)->AddHealth(20.f);
			UE_LOG(LogTemp, Warning, TEXT("Target: %s healed for 20.f. Health left: %f"), *(Target->GetName()), Cast<ARTSCharacter>(Target)->GetHealth());
		}
	}

	bUnitBusy = false;
	GetWorld()->GetTimerManager().ClearTimer(AidTimerHandle);
	Target = nullptr;
}


void ARTSAIController::PresentYourself()
{
	UE_LOG(LogTemp, Warning, TEXT("Actor: %s has controller: %s"), *(this->GetPawn()->GetName()), *(this->GetName()));
}
