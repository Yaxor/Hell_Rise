//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "BaseWeapon.h"

#include "Hell_Rise/Hell_Rise.h"

#include "Camera/CameraComponent.h"

#include "Hell_Rise/Player/PlayerFPS.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Sound/SoundCue.h"

#define SPAWN_PARTICLE_SOCKET "Muzzle"

//------------------------------------------------------------------------------------------------------------------------------------------
ABaseWeapon::ABaseWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ShootLength = 1000.0f;
    Damage      = 5.0f;
    FireRate    = 0.1f;
}

//------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ABaseWeapon::GetCurrentAmmo()
{
    return CurrentBullets;
}

//------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ABaseWeapon::GetCurrentMagazines()
{
    return CurrentMagazines;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::WeaponBeginPlay()
{
    PlayerFPSRef = CastChecked<APlayerFPS>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    CurrentBullets   = MaxBullets;
    CurrentMagazines = MaxMagazines;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::StartFireWeapon()
{
    GetWorldTimerManager().SetTimer(FireWeapon_TimerHandle, this, &ABaseWeapon::FireWeapon, FireRate, true, 0.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::FireWeapon()
{
    //If got Bullets
    if (CurrentBullets <= 0)
    {
        //Play Feedback
        PlayerFPSRef->bOnFireWeapon = false;
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptyShotSFX, GetActorLocation());

        return;
    }

    PlayerFPSRef->bOnFireWeapon = true;
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSFX, GetActorLocation());

    CurrentBullets--;

    //Update Current Bullets in UI
    PlayerFPSRef->UpdateCurrentBulletsAmount();

    const FTransform& SpawnMuzzleTransform = WeaponMesh->GetSocketTransform(SPAWN_PARTICLE_SOCKET);

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShootVFX, SpawnMuzzleTransform, true, EPSCPoolMethod::AutoRelease, true);

    //Set the Start and End Location of the LineTrace
    FHitResult HitResult;
    const FVector& StartTraceLocation = PlayerFPSRef->PlayerCamera->GetComponentLocation();
    const FVector& EndTraceLocation   = StartTraceLocation + (PlayerFPSRef->PlayerCamera->GetForwardVector() * ShootLength);

    //Set parameters to the CollisionQuery (Ignore the weapon and player)
    FCollisionQueryParams C_QueryParams;
    C_QueryParams.AddIgnoredActor(this);
    C_QueryParams.AddIgnoredActor(PlayerFPSRef);

    const bool& bOnHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTraceLocation, EndTraceLocation,
                                                              PLAYER_WEAPON_TRACE, C_QueryParams);

    if (bOnHit)
    {
        //Get the Location of the impact, and set the Rotation and Scale of the VFX
        FTransform SpawnBulletHitVFXTransform;
        SpawnBulletHitVFXTransform.SetLocation(HitResult.ImpactPoint);
        SpawnBulletHitVFXTransform.SetRotation(FQuat(FRotator::ZeroRotator));
        SpawnBulletHitVFXTransform.SetScale3D (FVector(1.0f, 1.0f, 1.0f));

        if (UKismetSystemLibrary::DoesImplementInterface(HitResult.GetActor(), UDamageableInterface::StaticClass()))
        {
            IDamageableInterface::Execute_TakeDamage(HitResult.GetActor(), Damage);

            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodHitVFX, SpawnBulletHitVFXTransform,
                                                                 true, EPSCPoolMethod::AutoRelease, true);
        }
        else
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitVFX, SpawnBulletHitVFXTransform,
                                                                  true, EPSCPoolMethod::AutoRelease, true);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::StopFireWeapon()
{
    GetWorldTimerManager().ClearTimer(FireWeapon_TimerHandle);

    PlayerFPSRef->bOnFireWeapon = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::Reload()
{
    if ((CurrentMagazines <= 0) || (CurrentBullets == MaxBullets))
    {
        return;
    }

    //Start the Reload Anim
    PlayerFPSRef->bOnReload = true;
    PlayerFPSRef->PlayerStopFire();

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSFX, GetActorLocation());

    CurrentBullets = MaxBullets;

    CurrentMagazines--;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::PickAmmo()
{
    CurrentBullets   = MaxBullets;
    CurrentMagazines = MaxMagazines;
}

#undef SPAWN_PARTICLE_SOCKET