//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Hell_Rise/Interfaces/DamageableInterface.h"

#include "RangeEnemy.generated.h"

UENUM(BlueprintType)
enum class ERangeStatus : uint8
{
    IDLE,
    COMBAT
};

UCLASS()
class HELL_RISE_API ARangeEnemy : public ACharacter, public IDamageableInterface
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UArrowComponent* SpawnFloorBloodArrow;

public:
    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //!Constructor
    ARangeEnemy();

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
    bool bOnShoot;
    bool bIsSearchingPlayer;

    float CurrentHealth;

    UPROPERTY(EditDefaultsOnly)
    FRotator FixRot;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
    int          MaxHealth;
    const uint8  CheckPlayerInSightRate = 12;
    const uint8  Damage                 = 15;
    const uint16 DeathForce             = 500;
    const uint16 AttackDistance         = 6000;

    UPROPERTY(BlueprintReadWrite, Category = "Player")
    class APlayerFPS* PlayerRef;

    const FName MuzzleSocket = "Muzzle";
    const FName HeadBoneName = "head";

    UPROPERTY(BlueprintReadWrite, Category = "AI Data")
    ERangeStatus CurrentStatus;

    FTimerHandle CheckPlayerIsInSightTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category = "Objects Reference")
    TSubclassOf<class ACommonBullet> BulletRef;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    TSubclassOf<class ADecalActor> BloodFloor;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* BloodVFX;
    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* ShootVFX;
    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* ShootHitVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* ShootSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* DamageSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* DeathSFX;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    UFUNCTION(BlueprintCallable)
    void StopShoot();
    UFUNCTION(BlueprintCallable)
    void StartShoot();
    UFUNCTION(BlueprintCallable)
    void Shoot();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /*Look at player*/
    void AimToPlayer();
    /*If player is en COMBAT status and Can Follow and is Alive execute FollowPlayer*/
    void CheckIfPlayerHasBeenSeen();
    /*Check with LineTrace if player in sight*/
    void CheckPlayerInSight();
    /*Spawn a blood particle and blood decal*/
    void DamageVFX();
    /*Turn off the capsule collision, start simulate physics in the mesh and add impulse in head bone*/
    void OnEnemyKilled();
    /*Change CombatStatus to IDLE*/
    void ReturnToIdle();
};
