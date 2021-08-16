//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "Rifle.h"

#include "Hell_Rise/Player/PlayerFPS.h"

//------------------------------------------------------------------------------------------------------------------------------------------
ARifle::ARifle()
{
    PrimaryActorTick.bCanEverTick = true;

    MaxBullets   = 20;
    MaxMagazines = 3;
    ShootLength  = 10000.0f;
    Damage       = 25.0f;
    FireRate     = 0.15f;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARifle::BeginPlay()
{
    Super::BeginPlay();
    WeaponBeginPlay();

    bHasTriggered = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARifle::StartFireWeapon()
{
    FireWeapon();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARifle::FireWeapon()
{
    if (!bHasTriggered)
    {
        GetWorldTimerManager().SetTimer(FireWeapon_TimerHandle, this, &ARifle::TriggerOff, FireRate, false);
        Super::FireWeapon();
        bHasTriggered = true;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARifle::StopFireWeapon()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARifle::TriggerOff()
{
    GetWorldTimerManager().ClearTimer(FireWeapon_TimerHandle);
    bHasTriggered = false;

    //Stop Player Fire Weapon Anim
    PlayerFPSRef->bOnFireWeapon = false;
}