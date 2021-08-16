//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Hell_Rise/Interfaces/DamageableInterface.h"

#include "BaseEnemy.generated.h"

UENUM(BlueprintType)
enum class ECombatStatus : uint8
{
    IDLE,
    COMBAT
};

UCLASS()
class HELL_RISE_API ABaseEnemy : public ACharacter, public IDamageableInterface
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UArrowComponent* SpawnFloorBloodArrow;

public:
     //!Constructor
    ABaseEnemy();

    //*******************************************************************************************************************
    //                                          INTERFACES FUNCTIONS                                                    *
    //*******************************************************************************************************************

    virtual void TakeDamage_Implementation(float DamageAmount) override;

protected:
    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadOnly, Category = "Anim Data")
    bool bIsDead;
    UPROPERTY(BlueprintReadOnly, Category = "Anim Data")
    bool bOnMeleeAttack;
    bool bCanFollow;
    bool bCanEmitChaseSound;

    float       CurrentHealth;
    const float FollowPlayerDelay = 0.6f;

    const uint8 CheckPlayerInSightRate = 12;
    const uint8 Damage                 = 33;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
    int          MaxHealth;
    const uint16 AttackDistance    = 340;
    const uint16 DeathForce        = 500;
    const uint16 TraceViewDistance = 6000;

    UPROPERTY(BlueprintReadWrite, Category = "Player")
    class APlayerFPS* PlayerRef;

    UPROPERTY(BlueprintReadWrite, Category = "AI Data")
    ECombatStatus CurrentStatus;

    const FName HeadBoneName = "head";

    FTimerHandle CanFollowAgainTimerHandle;
    FTimerHandle CheckPlayerIsInSightTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    TSubclassOf<class ADecalActor> BloodFloor;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* BloodVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* AttackSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* AttackWeaponSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* ChaseSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* DamageSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* DeathSFX;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************
 
    /*Follow the player if the combat status is COMBAT*/
    UFUNCTION(BlueprintImplementableEvent)
    void FollowPlayer();

    /*Wait FollowPlayerDelay seconds to start chasing player*/
    UFUNCTION(BlueprintCallable)
    void AttackEnd();
    /*Make a BoxTrace to hit the player*/
    UFUNCTION(BlueprintCallable)
    virtual void AttackHit();
    /*Stop the movement and set bInMeleeAtack in true*/
    UFUNCTION(BlueprintCallable)
    void AttackStart();
    /*Wait CheckPlayerInSightRate seconds to change bPlayerInSight to false*/
    UFUNCTION(BlueprintCallable)
    void CheckPlayerInSight();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /*Set bCanFollow to true and Clear timer CanFollowAgainTimerHandle*/
    void CanFollowAgain();
    /*If player is en COMBAT status and Can Follow and is Alive execute FollowPlayer*/
    void CheckIfPlayerHasBeenSeen();
    /*Spawn a blood particle and blood decal*/
    void DamageVFX();
    /*Turn off the capsule collision, start simulate physics in the mesh and add impulse in head bone*/
    void OnEnemyKilled();
    /*Change CombatStatus to IDLE*/
    void ReturnToIdle();
};
