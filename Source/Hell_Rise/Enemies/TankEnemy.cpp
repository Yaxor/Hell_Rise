//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "TankEnemy.h"

#include "Hell_Rise/Player/PlayerFPS.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Sound/SoundCue.h"

//------------------------------------------------------------------------------------------------------------------------------------------
ATankEnemy::ATankEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATankEnemy::AttackHit()
{
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSFX, GetActorLocation());
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackWeaponSFX, GetActorLocation());

    TArray<FHitResult> HitResults;

    const FVector& StartBox = GetActorLocation();
    const FVector& EndBox   = StartBox + (GetActorForwardVector() * AttackDistance);
    const FVector& BoxSize  = FVector(10.0f, 140.0f, 140.0f);

    TArray<AActor*> IgnoreTheseActors;
    IgnoreTheseActors.Add(this);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsTypes;
    ObjectsTypes.Add(static_cast<EObjectTypeQuery>(ECollisionChannel::ECC_Pawn));

    const bool bBoxHits = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), StartBox, EndBox, BoxSize,
                                                                        GetActorRotation(), ObjectsTypes, true,
                                                                        IgnoreTheseActors, EDrawDebugTrace::ForDuration,
                                                                        HitResults, true, FColor::Red, FColor::Green, 2.0f);

    //If something that I hit is the player, hurt him
    if (bBoxHits)
    {
        for (const auto& CurrentHit : HitResults)
        {
            if (APlayerFPS* TempPlayer = Cast<APlayerFPS>(CurrentHit.GetActor()))
            {
                IDamageableInterface::Execute_TakeDamage(CurrentHit.GetActor(), Damage);
                TempPlayer->StunPlayer();
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATankEnemy::BeginPlay()
{
    Super::BeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATankEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
