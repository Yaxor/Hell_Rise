//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class HELL_RISE_API ABaseWeapon : public AActor
{
    GENERATED_BODY()

    friend class APlayerFPS;

public:

    //!Constructor
    ABaseWeapon();

    //*******************************************************************************************************************
    //                                                      COMPONENTS                                                  *
    //*******************************************************************************************************************

    UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess = "true"), Category = "Components")
    class UStaticMeshComponent* WeaponMesh;


    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Return the CurrentBullets amount*/
    virtual uint8_t GetCurrentAmmo();
    /*Return the CurrentMagazines amount*/
    virtual uint8_t GetCurrentMagazines();
    /*Start FireWeapon TimerHadle*/
    virtual void StartFireWeapon();
    /*Create a LineTrace to make damage what implement DamageableInterface, VFX and SFX*/
    virtual void FireWeapon();
    /*Stop FireWeapon TimerHadle*/
    virtual void StopFireWeapon();
    /*Reload Weapon*/
    virtual void Reload();
    /*Set CurrentMagazines to the Max Amount of Magazines*/
    virtual void PickAmmo();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    class APlayerFPS* PlayerFPSRef;

    uint8_t MaxBullets;
    uint8_t MaxMagazines;
    uint8_t CurrentBullets;
    uint8_t CurrentMagazines;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* BloodHitVFX;
    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* BulletHitVFX;
    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* ShootVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float FireRate;
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float Damage;
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float ShootLength;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* EmptyShotSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* ReloadSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* ShotSFX;

    FTimerHandle FireWeapon_TimerHandle;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    /*Execute in BeginPlay in his childs*/
    virtual void WeaponBeginPlay();
};
