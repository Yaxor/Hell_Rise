//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "RangeEnemy.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

#include "Hell_Rise/Bullets/CommonBullet.h"
#include "Hell_Rise/Player/PlayerFPS.h"

#include "Engine/DecalActor.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Sound/SoundCue.h"

//------------------------------------------------------------------------------------------------------------------------------------------
ARangeEnemy::ARangeEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    SpawnFloorBloodArrow = CreateDefaultSubobject<UArrowComponent>("Blood");
    SpawnFloorBloodArrow->SetupAttachment(GetMesh());

    GetCapsuleComponent()->SetGenerateOverlapEvents(false);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetGenerateOverlapEvents(true);
    GetMesh()->bReceivesDecals = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::TakeDamage_Implementation(float DamageAmount)
{
    if (bIsDead)
    {
        return;
    }

    if (CurrentHealth >= 0)
    {
        CurrentHealth -= DamageAmount;

        UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSFX, GetActorLocation());
        DamageVFX();
    }

    if ((CurrentHealth <= 0) && (!bIsDead))
    {
        CurrentHealth = 0.0f;
        bIsDead = true;
        OnEnemyKilled();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::StopShoot()
{
    bOnShoot           = false;
    bIsSearchingPlayer = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::StartShoot()
{
    bIsSearchingPlayer = false;
    bOnShoot           = true;
    AimToPlayer();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::Shoot()
{
    if (PlayerRef->GetPlayerIsAlive())
    {
        if (BulletRef)
        {
            //Play shot sound
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootSFX, GetActorLocation());

            //Get socket transform
            const FTransform SpawnBulletTransform = GetMesh()->GetSocketTransform(MuzzleSocket);

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

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::BeginPlay()
{
    Super::BeginPlay();
    bIsSearchingPlayer = true;
    CurrentHealth      = MaxHealth;
    CurrentStatus      = ERangeStatus::IDLE;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckIfPlayerHasBeenSeen();
    CheckPlayerInSight();
    AimToPlayer();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::AimToPlayer()
{
    if (PlayerRef != nullptr)
    {
        if (bOnShoot)
        {
            //Direction to player
            const FRotator& LookToPlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerRef->GetActorLocation());
            const FRotator& LookDirFixed    = FRotator(GetActorRotation().Pitch, 
                                                       LookToPlayerRot.Yaw + FixRot.Yaw,
                                                       GetActorRotation().Roll);

            //Set rotation
            SetActorRotation(LookDirFixed);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::CheckIfPlayerHasBeenSeen()
{
    if ((CurrentStatus == ERangeStatus::COMBAT) && (bIsSearchingPlayer) && (!bIsDead))
    {
        StartShoot();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::CheckPlayerInSight()
{
    if (CurrentStatus == ERangeStatus::COMBAT)
    {
        FHitResult HitResult;

        const FVector& StartTraceLocation = GetMesh()->GetSocketLocation(MuzzleSocket);
        const FVector& EndTraceLocation = StartTraceLocation + (GetActorForwardVector() * AttackDistance);

        FCollisionQueryParams C_QueryParams;
        C_QueryParams.AddIgnoredActor(this);
        C_QueryParams.bTraceComplex = true;

        const bool& bOnHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTraceLocation, EndTraceLocation,
                                                                  ECollisionChannel::ECC_WorldDynamic, C_QueryParams);

        //If hit something
        if (bOnHit)
        {
            //If see the player
            if (APlayerFPS* PlayerFPSRef = Cast<APlayerFPS>(HitResult.GetActor()))
            {
                AimToPlayer();
            }
            else
            {
                ReturnToIdle();
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::DamageVFX()
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.bNoFail = true;

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX, GetActorLocation(),
                                             FRotator::ZeroRotator, FVector(0.1f), true,
                                             EPSCPoolMethod::AutoRelease, true);

    GetWorld()->SpawnActor<ADecalActor>(BloodFloor, SpawnFloorBloodArrow->GetComponentTransform(), SpawnParams);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::OnEnemyKilled()
{
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSFX, GetActorLocation());

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

    const auto&    PlayerReference = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    const FVector& DeathImpulse    = UKismetMathLibrary::Normal(PlayerReference->GetActorForwardVector());

    GetMesh()->AddImpulse(DeathImpulse * DeathForce, HeadBoneName, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ARangeEnemy::ReturnToIdle()
{
    CurrentStatus = ERangeStatus::IDLE;
}
