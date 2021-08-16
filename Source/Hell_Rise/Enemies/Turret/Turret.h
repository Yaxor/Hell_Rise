//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Hell_Rise/Interfaces/DamageableInterface.h"

#include "Turret.generated.h"

UENUM(BlueprintType)
enum class ETurretStatus : uint8
{
    SEARCHING,
    SHOTING
};

UCLASS()
class HELL_RISE_API ATurret : public APawn, public IDamageableInterface
{
    GENERATED_BODY()

public:
    //*******************************************************************************************************************
    //                                        CONSTRUCTOR && PUBLIC COMPONENTS                                          *
    //*******************************************************************************************************************

    //!Constructor
    ATurret();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* TurretMesh;

    class USceneComponent*      RootComponent;

    //*******************************************************************************************************************
    //                                          INTERFACES FUNCTIONS                                                    *
    //*******************************************************************************************************************

    virtual void TakeDamage_Implementation(float DamageAmount) override;

protected:
    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadWrite, Category = "AI Data")
    ETurretStatus CurrentStatus;

    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    uint8 MaxHealth;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    /*Start Shot Timerhandle*/
    UFUNCTION(BlueprintCallable)
    void StartShoting();

private:
    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    class APlayerFPS* PlayerRef;

    bool bRotateToTheRight;
    bool bSeePlayer;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float Cadence;

    UPROPERTY(EditDefaultsOnly, Category = "Rotation")
    float SpeedRotation;
    UPROPERTY(EditDefaultsOnly, Category = "Rotation")
    float ChangeRotDelay;

    const uint16 TraceDistance = 6000;//60m

    const FName SpawnBulletTransformName = "Mussle";

    UPROPERTY(EditDefaultsOnly, Category = "Rotation")
    FRotator RotRight;
    UPROPERTY(EditDefaultsOnly, Category = "Rotation")
    FRotator RotLeft;

    FTimerHandle ShootingTimerHandle;
    FTimerHandle ChangePatrolRotDirTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category = "Objects Reference")
    TSubclassOf<class ACommonBullet> BulletRef;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* ExplotionVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* ExplotionSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* ShotSFX;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /*Rotate to the player*/
    void AimToPlayer();
    /*Each ChangeRotDelay time changes the direction of rotation*/
    void ChangeRotDir();
    /*Check with LineTrace if player in sight*/
    void CheckSeePlayerOnAttack();
    /*Explotion VFX and SFX*/
    void Death();
    /*Clear Shot Timerhandle*/
    void EndShoting();
    /*Rotation of the turret looking for the player*/
    void PatrolRotation();
    /*Instance the bullet from the canyon socket*/
    void Shot();
};
