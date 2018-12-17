// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RTSCharacter.generated.h"

UCLASS(Blueprintable)
class ARTSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARTSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void AddHealth(float Val) { Health = FMath::Clamp<float>(GetHealth() + Val, 0.f, 100.f); }
	void SubtractHealth(float Val);
	FORCEINLINE bool IsCharacterBleeding() { return bIsBleeding; }

	//BlueprintCallable for animation update
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsCharacterInjured() { return GetHealth() != 100.f; }

	//BlueprintCallable for animation update
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsCharacterDead() { return bIsDead; }

	void DestroyCharacter();
	void StopBleeding();
protected:
	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;	

	float Health;
	bool bIsDead;
	bool bIsBleeding;

	UFUNCTION()
		void Bleed();

	FTimerHandle BleedingTimerHandle;
};

