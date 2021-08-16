//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "AssaultRifle.h"

//------------------------------------------------------------------------------------------------------------------------------------------
AAssaultRifle::AAssaultRifle()
{
    PrimaryActorTick.bCanEverTick = true;

    MaxBullets   = 30;
    MaxMagazines = 3;
    ShootLength  = 10000.0f;
    Damage       = 10.0f;
    FireRate     = 0.1f;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AAssaultRifle::BeginPlay()
{
    Super::BeginPlay();
    WeaponBeginPlay();
}
