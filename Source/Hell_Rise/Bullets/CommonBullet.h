//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CommonBullet.generated.h"

UCLASS()
class HELL_RISE_API ACommonBullet : public AActor
{
    GENERATED_BODY()

public:
    //*******************************************************************************************************************
    //                                   CONSTRUCTOR && PUBLIC COMPONENTS                                               *
    //*******************************************************************************************************************

    //!Constructor
    ACommonBullet();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivatesAcces = "true"))
    class UStaticMeshComponent*         BulletMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivatesAcces = "true"))
    class UProjectileMovementComponent* ProjectileMovement;
    class USceneComponent*              RootComponent;

    //*******************************************************************************************************************
    //                                       PUBLIC INLINE FUNCTIONS                                                    *
    //*******************************************************************************************************************

    FORCEINLINE float GetMinDamageBullet() { return MinDamage; }
    FORCEINLINE float GetMaxDamageBullet() { return MaxDamage; }

protected:


private:
    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    const uint8_t LifeTime = 12;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float MinDamage;
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float MaxDamage;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    /*Make random damage between MinDamage and MaxDamage to the objective of parameter*/
    void MakeDamageToTarget(AActor* TargetToDamage);

    UFUNCTION()
    void BulletBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                            AActor*              OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32                OtherBodyIndex,
                            bool                 bFromSweep,
                            const FHitResult&    SweepResult);
};
