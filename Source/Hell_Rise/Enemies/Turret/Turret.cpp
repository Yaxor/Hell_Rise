//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "Turret.h"

#include "Hell_Rise/Bullets/CommonBullet.h"

#include "Hell_Rise/Player/PlayerFPS.h"

#include "Components/StaticMeshComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Sound/SoundCue.h"

//------------------------------------------------------------------------------------------------------------------------------------------
ATurret::ATurret()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
    SetRootComponent(RootComponent);

    TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
    TurretMesh->SetupAttachment(RootComponent);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::StartShoting()
{
    //Change TurretStatus, Aim to player, Clear patrol rotate timer and Set shot timer
    if (CurrentStatus != ETurretStatus::SHOTING)
    {
        CurrentStatus = ETurretStatus::SHOTING;
        AimToPlayer();
        GetWorldTimerManager().ClearTimer(ChangePatrolRotDirTimerHandle);
        GetWorldTimerManager().SetTimer(ShootingTimerHandle, this, &ATurret::Shot, Cadence, true, 0.0f);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::TakeDamage_Implementation(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0)
    {
        Death();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::BeginPlay()
{
    Super::BeginPlay();

    CurrentStatus = ETurretStatus::SEARCHING;
    PlayerRef = CastChecked<APlayerFPS>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    CurrentHealth = MaxHealth;
    GetWorldTimerManager().SetTimer(ChangePatrolRotDirTimerHandle, this, &ATurret::ChangeRotDir, ChangeRotDelay, true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    PatrolRotation();
    CheckSeePlayerOnAttack();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::AimToPlayer()
{
    if (PlayerRef != nullptr)
    {
        //Direction to player
        const FRotator& LookToPlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerRef->GetActorLocation());

        //Set rotation
        SetActorRotation(FRotator(GetActorRotation().Pitch, LookToPlayerRot.Yaw, GetActorRotation().Roll));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::ChangeRotDir()
{
    bRotateToTheRight = !bRotateToTheRight;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::CheckSeePlayerOnAttack()
{
    if (CurrentStatus == ETurretStatus::SHOTING)
    {
        FHitResult HitResult;

        const FVector& StartTraceLocation = TurretMesh->GetSocketLocation(SpawnBulletTransformName);
        const FVector& EndTraceLocation   = StartTraceLocation + (GetActorForwardVector() * TraceDistance);

        FCollisionQueryParams C_QueryParams;
        C_QueryParams.AddIgnoredActor(this);
        C_QueryParams.bTraceComplex = true;

        const bool& bOnHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTraceLocation, EndTraceLocation,
                                                                  ECollisionChannel::ECC_WorldDynamic, C_QueryParams);

        //If hit something
        if (bOnHit)
        {
            //If is the player
            if (APlayerFPS* PlayerFPSRef = Cast<APlayerFPS>(HitResult.GetActor()))
            {
                bSeePlayer = true;
            }
            else
            {
                bSeePlayer = false;
            }
        }
        else
        {
            bSeePlayer = false;
        }

        if (bSeePlayer)
        {
            AimToPlayer();
        }
        else
        {
            EndShoting();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::Death()
{
    //Spawn explotion particle
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplotionVFX, GetActorTransform(),
                                                  true, EPSCPoolMethod::AutoRelease, true);

    //Play explotion sound
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplotionSFX, GetActorLocation());

    Destroy(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::EndShoting()
{
    //Change TurretStatus, Clear shot timer and Set patrol rotate timer 
    CurrentStatus = ETurretStatus::SEARCHING;
    GetWorldTimerManager().ClearTimer(ShootingTimerHandle);
    GetWorldTimerManager().SetTimer(ChangePatrolRotDirTimerHandle, this, &ATurret::ChangeRotDir, ChangeRotDelay, true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::PatrolRotation()
{
    if (CurrentStatus == ETurretStatus::SEARCHING)
    {
        if (bRotateToTheRight)
        {
            SetActorRotation(FMath::Lerp(GetActorRotation(), RotRight, SpeedRotation));
        }
        else
        {
            SetActorRotation(FMath::Lerp(GetActorRotation(), RotLeft, SpeedRotation));
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATurret::Shot()
{
    if (PlayerRef->GetPlayerIsAlive())
    {
        if (BulletRef)
        {
            //Play shot sound
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSFX, GetActorLocation());

            //Get socket transform
            const FTransform SpawnBulletTransform = TurretMesh->GetSocketTransform(SpawnBulletTransformName);

            //Set spawn params
            FActorSpawnParameters BulletSpawnParams;
            BulletSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            //Spawn bullet
            ACommonBullet* BulletTemp = GetWorld()->SpawnActor<ACommonBullet>(BulletRef,
                                                                              SpawnBulletTransform,
                                                                              BulletSpawnParams);
        }
    }
}