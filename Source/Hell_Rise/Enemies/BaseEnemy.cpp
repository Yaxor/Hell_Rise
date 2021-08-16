//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "BaseEnemy.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

#include "Hell_Rise/Player/PlayerFPS.h"

#include "Engine/DecalActor.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Sound/SoundCue.h"

//------------------------------------------------------------------------------------------------------------------------------------------
ABaseEnemy::ABaseEnemy()
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
void ABaseEnemy::TakeDamage_Implementation(float DamageAmount)
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
        bIsDead       = true;
        OnEnemyKilled();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::AttackEnd()
{
    bOnMeleeAttack = false;

    GetWorldTimerManager().SetTimer(CanFollowAgainTimerHandle, this, &ABaseEnemy::CanFollowAgain, FollowPlayerDelay, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::AttackHit()
{
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSFX, GetActorLocation());
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackWeaponSFX, GetActorLocation());

    TArray<FHitResult> HitResults;

    const FVector& StartBox = GetActorLocation();
    const FVector& EndBox   = StartBox + (GetActorForwardVector() * AttackDistance);
    const FVector& BoxSize  = FVector(10.0f, 100.0f, 100.0f);

    TArray<AActor*> IgnoreTheseActors;
    IgnoreTheseActors.Add(this);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsTypes;
    ObjectsTypes.Add(static_cast<EObjectTypeQuery>(ECollisionChannel::ECC_Pawn));

    const bool bBoxHits = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld() , StartBox, EndBox, BoxSize,
                                                                        GetActorRotation()   , ObjectsTypes   , true,
                                                                        IgnoreTheseActors    , EDrawDebugTrace::ForDuration,
                                                                        HitResults, true     , FColor::Red    , FColor::Green, 2.0f);

    //If something that I hit is the player, hurt him
    if (bBoxHits)
    {
        for (const auto& CurrentHit : HitResults)
        {
            if (APlayerFPS* TempPlayer = Cast<APlayerFPS>(CurrentHit.GetActor()))
            {
                IDamageableInterface::Execute_TakeDamage(CurrentHit.GetActor(), Damage);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::AttackStart()
{
    bCanFollow     = false;
    bOnMeleeAttack = true;
    GetMovementComponent()->StopMovementImmediately();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::BeginPlay()
{
    Super::BeginPlay();

    bCanEmitChaseSound = true;
    bCanFollow         = true;
    CurrentHealth      = MaxHealth;
    CurrentStatus      = ECombatStatus::IDLE;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckIfPlayerHasBeenSeen();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::CanFollowAgain()
{
    bCanFollow = true;
    GetWorldTimerManager().ClearTimer(CanFollowAgainTimerHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::CheckPlayerInSight()
{
    GetWorldTimerManager().ClearTimer(CheckPlayerIsInSightTimerHandle);
    GetWorldTimerManager().SetTimer(CheckPlayerIsInSightTimerHandle, this, &ABaseEnemy::ReturnToIdle, CheckPlayerInSightRate, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::CheckIfPlayerHasBeenSeen()
{
    if ((CurrentStatus == ECombatStatus::COMBAT) && (bCanFollow) && (!bIsDead))
    {
        FollowPlayer();

        if (bCanEmitChaseSound)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), ChaseSFX, GetActorLocation());
            bCanEmitChaseSound = false;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::DamageVFX()
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.bNoFail = true;

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX , GetActorLocation(),
                                             FRotator::ZeroRotator, FVector(0.1f), true,
                                             EPSCPoolMethod::AutoRelease,   true);

    GetWorld()->SpawnActor<ADecalActor>(BloodFloor, SpawnFloorBloodArrow->GetComponentTransform(), SpawnParams);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::OnEnemyKilled()
{
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSFX, GetActorLocation());

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetMesh()->SetSimulatePhysics (true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

    const auto&    PlayerReference = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    const FVector& DeathImpulse    = UKismetMathLibrary::Normal(PlayerReference->GetActorForwardVector());

    GetMesh()->AddImpulse(DeathImpulse * DeathForce, HeadBoneName, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseEnemy::ReturnToIdle()
{
    CurrentStatus      = ECombatStatus::IDLE;
    bCanEmitChaseSound = true;
    GetMovementComponent()->StopMovementImmediately();
}
