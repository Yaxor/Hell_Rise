//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "PlayerFPS.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Hell_Rise/Weapons/AssaultRifle.h"
#include "Hell_Rise/Weapons/BaseWeapon.h"
#include "Hell_Rise/Weapons/Rifle.h"

#include "Kismet/GameplayStatics.h"

#include "PlayerControllerFPS.h"

#include "Sound/SoundCue.h"

//------------------------------------------------------------------------------------------------------------------------------------------
APlayerFPS::APlayerFPS()
{
    PrimaryActorTick.bCanEverTick = true;
    CapsuleComp = GetCapsuleComponent();
    SetRootComponent(CapsuleComp);

    CharacterMovComp = GetCharacterMovement();

    SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    SpringArm->SetupAttachment(CapsuleComp);
    SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
    SpringArm->TargetArmLength         = 50.0f;
    SpringArm->bUsePawnControlRotation = true;

    PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
    PlayerCamera->SetupAttachment(SpringArm);

    ArmsMesh = GetMesh();
    ArmsMesh->SetupAttachment(SpringArm);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::UpdateCurrentBulletsAmount()
{
    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->UpdateAmmoAmountHUD(EquippedWeapon->GetCurrentAmmo(),
                                                 EquippedWeapon->GetCurrentMagazines());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::PickAmmo()
{
    if (bIsDead)
    {
        return;
    }

    AssaultRifleWeapon->PickAmmo();
    RifleWeapon->PickAmmo();

    UpdateCurrentBulletsAmount();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::PickHeal(uint8 HealAmount)
{
    if (bIsDead)
    {
        return;
    }

    CurrentLife += HealAmount;

    //Heal FeedBack


    if (CurrentLife > MaxLife)
    {
        CurrentLife = MaxLife;
    }

    OnUpdateCurrentLifeGI();
    UpdateLifePercentUI();

    if (CurrentLife >= (MaxLife / 4.0f))
    {
        HurtFeedbackOff();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::PickSpeedUpBoost()
{
    if ((bIsDead) || (bIsStunned) || (bIsSlowedDown))
    {
        return;
    }

    bIsSpeedUp = true;

    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->SpeedUpFeedbackOn();
    }

    const float NewSpeed           = NormalSpeed + SpeedBuff;
    CharacterMovComp->MaxWalkSpeed = NewSpeed;

    GetWorldTimerManager().ClearTimer(BoostTimerHandle);
    GetWorldTimerManager().SetTimer  (BoostTimerHandle, this, &APlayerFPS::ReturnToNormalSpeed, BoostDuration, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::PickSpeedDownDeBuff()
{
    if ((bIsDead) || (bIsStunned) || (bIsSpeedUp))
    {
        return;
    }

    if (bOnRun) 
    {
        StopRunning();
    }

    bIsSlowedDown = true;

    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->FreezeFeedbackOn();
    }

    const float NewSpeed           = NormalSpeed + SpeedDeBuff;
    CharacterMovComp->MaxWalkSpeed = NewSpeed;

    GetWorldTimerManager().ClearTimer(BoostTimerHandle);
    GetWorldTimerManager().SetTimer(BoostTimerHandle, this, &APlayerFPS::ReturnToNormalSpeed, BoostDuration, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::StunPlayer()
{
    if (bIsDead)
    {
        return;
    }

    bIsStunned = true;
    bOnRun     = false;

    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->StunFeedbackOn();
    }

    GetWorldTimerManager().PauseTimer(StepSFXTimerHandle);
    GetWorldTimerManager().PauseTimer(RunSFXTimerHandle);

    CharacterMovComp->MaxWalkSpeed = 0.0f;

    GetWorldTimerManager().ClearTimer(BoostTimerHandle);
    GetWorldTimerManager().ClearTimer(StunTimerHandle);
    GetWorldTimerManager().SetTimer(StunTimerHandle, this, &APlayerFPS::StunOff, StunDuration, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::OnReloadEnd()
{
    bOnReload = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::OnWeaponChangeEnd()
{
    bOnChangeWeapon = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::OnWeaponChange()
{
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), SwitchWeaponSFX, GetActorLocation());

    //If CurrentWeaponIndex equals 0, set EquippedWeapon to Rifle
    if (CurrentWeaponIndex == 0)
    {
        //Attach to Socket Back
        AssaultRifleWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AssaultRifleBackSocket);

        //Attach to Current Weapon Socket
        RifleWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RifleSocket);

        EquippedWeapon = RifleWeapon;
    }
    //If CurrentWeaponIndex equals 1, set EquippedWeapon to AssaultRifle
    else if (CurrentWeaponIndex == 1)
    {
        //Attach to Socket Back
        RifleWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RifleBackSocket);

        //Attach to Current Weapon Socket
        AssaultRifleWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AssaultRifleSocket);

        EquippedWeapon = AssaultRifleWeapon;
    }

    UpdateCurrentBulletsAmount();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::OnWeaponReady()
{
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReadySFX, GetActorLocation());
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::TakeDamage_Implementation(float DamageAmount)
{
    if (bIsDead)
    {
        return;
    }

    CurrentLife -= DamageAmount;

    if (CurrentLife <= 0)
    {
        CurrentLife = 0;
        UpdateLifePercentUI();
        OnUpdateCurrentLifeGI();
        Death();

        return;
    }

    if (CurrentLife > 0)
    {
        UpdateLifePercentUI();
        OnUpdateCurrentLifeGI();
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), HurtSFX, GetActorLocation());
    }

    if (CurrentLife <= (MaxLife / 4.0f))
    {
        HurtFeedback();
    }
    else
    {
        DamageFeedback();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::BeginPlay()
{
    Super::BeginPlay();

    bIsDead         = false;
    bOnChangeWeapon = false;
    bOnReload       = false;
    bOnRun          = false;

    GetWorldTimerManager().SetTimer(UpdateUIBeginPlayHandle, this, &APlayerFPS::UpdateLifePercentUI, 0.12f, false);
    CreateWeapons();
}

void APlayerFPS::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckIsOnAir();

    CheckToStopWalkSound();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::ChangeWeapon(uint8_t WeaponIndex)
{
    if ((bIsDead) || (CurrentWeaponIndex == WeaponIndex) || (bOnReload) || (bOnRun) || (bOnChangeWeapon))
    {
        return;
    }

    CurrentWeaponIndex = WeaponIndex;

    //Start Change Weapon Anim
    bOnChangeWeapon = true;
    PlayerStopFire();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::CheckIsOnAir()
{
    if (bIsDead)
    {
        return;
    }

    //If player is moving
    if (bIsMoving)
    {
        //If player in ground
        if (CharacterMovComp->IsMovingOnGround())
        {
            //If the player was jumping
            if (bIsJumping)
            {
                //If player is running
                if (bOnRun)
                {
                    //Start Run sound
                    GetWorldTimerManager().ClearTimer(RunSFXTimerHandle);
                    GetWorldTimerManager().SetTimer(RunSFXTimerHandle, this, &APlayerFPS::StepSound, RunSoundDelay, true, 0.0f);
                }
                //If player is walking
                else
                {
                    //Start walk sound
                    GetWorldTimerManager().ClearTimer(StepSFXTimerHandle);
                    GetWorldTimerManager().SetTimer(StepSFXTimerHandle, this, &APlayerFPS::StepSound, WalkSoundDelay, true, 0.0f);
                }

                //Set bIsJumping to false, because is in ground
                bIsJumping = false;
            }
        }
        else
        {
            //If not in ground clear all steps sounds
            bIsJumping = true;
            GetWorldTimerManager().ClearTimer(RunSFXTimerHandle);
            GetWorldTimerManager().ClearTimer(StepSFXTimerHandle);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::CreateWeapons()
{
    FActorSpawnParameters ActorSpawnParams;
    ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ActorSpawnParams.Instigator                     = this;

    //Spawn AssaultRifle and Set Scale and Attach to Socket
    AssaultRifleWeapon = GetWorld()->SpawnActor<AAssaultRifle>(AssaultRifle, GetActorTransform(), ActorSpawnParams);
    AssaultRifleWeapon->AttachToComponent(SpringArm, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    AssaultRifleWeapon->SetActorRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
    AssaultRifleWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AssaultRifleBackSocket);

    //Spawn Rifle and Set Scale and Attach to Socket
    RifleWeapon = GetWorld()->SpawnActor<ARifle>(Rifle, GetActorTransform(), ActorSpawnParams);
    RifleWeapon->SetActorRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
    RifleWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RifleSocket);

    EquippedWeapon = RifleWeapon;

    UpdateCurrentBulletsAmount();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::DamageFeedback()
{
    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->GetDamageFeedback();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::Death()
{
    bIsDead = true;

    PlayerStopFire();

    GetWorldTimerManager().ClearTimer(StepSFXTimerHandle);
    GetWorldTimerManager().ClearTimer(RunSFXTimerHandle );
    GetWorldTimerManager().SetTimer(GoToLoseMapTimerHandle, this, &APlayerFPS::GoToLoseMap, 0.5f, false);
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSFX, GetActorLocation());

    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->DeathFeedbackOn();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::GoToLoseMap()
{
    UGameplayStatics::OpenLevel(GetWorld(), "LoseMap");
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::HurtFeedback()
{
    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->HurtFeedback();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::HurtFeedbackOff()
{
    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->HurtFeedbackOff();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::JumpSound()
{
    if (!CharacterMovComp->IsFalling())
    {
        bIsJumping = true;
        GetWorldTimerManager().PauseTimer(StepSFXTimerHandle);
        GetWorldTimerManager().PauseTimer(RunSFXTimerHandle);
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), JumpSFX, GetActorLocation());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::PlayerReload()
{
    if ((bIsDead) || (bOnRun) || (bOnChangeWeapon))
    {
        return;
    }

    EquippedWeapon->Reload();
    UpdateCurrentBulletsAmount();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::PlayerStartFire()
{
    if ((bIsDead) || (bOnReload) || (bOnChangeWeapon) || (bOnRun))
    {
        return;
    }

    EquippedWeapon->StartFireWeapon();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::PlayerStopFire()
{
    EquippedWeapon->StopFireWeapon();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::Run()
{
    if ((bIsDead) || (bIsStunned) || (bIsSlowedDown) || (bOnReload))
    {
        return;
    }

    bOnRun = true;
    PlayerStopFire();

    const float CurrentMaxSpeed    = CharacterMovComp->GetMaxSpeed();
    const float NewSpeed           = CurrentMaxSpeed + RunSpeed;
    CharacterMovComp->MaxWalkSpeed = NewSpeed;

    //If player is in ground
    if (CharacterMovComp->IsMovingOnGround())
    {
        //Get my current speed and if greater than 0 play run's sound
        const float CurrentSpeed = GetVelocity().Size();
        if (CurrentSpeed > 0.0f)
        {
            GetWorldTimerManager().PauseTimer(StepSFXTimerHandle);
            GetWorldTimerManager().ClearTimer(RunSFXTimerHandle);
            GetWorldTimerManager().SetTimer(RunSFXTimerHandle, this, &APlayerFPS::StepSound, RunSoundDelay, true, 0.0f);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::ReturnToNormalSpeed()
{
    if (bIsDead)
    {
        return;
    }

    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->FreezeFeedbackOff();
        PlayerControllerRef->SpeedUpFeedbackOff();
    }

    bIsSlowedDown = false;
    bIsSpeedUp    = false;

    if (bOnRun)
    {
        const float NewSpeed           = NormalSpeed + RunSpeed;
        CharacterMovComp->MaxWalkSpeed = NewSpeed;
    }
    else
    {
        CharacterMovComp->MaxWalkSpeed = NormalSpeed;
    }

    GetWorldTimerManager().ClearTimer(BoostTimerHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::StepSound()
{
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), StepSFX, GetActorLocation());
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::StopRunning()
{
    if ((bIsDead) || (bIsStunned) || (bIsSlowedDown))
    {
        return;
    }

    GetWorldTimerManager().ClearTimer(RunSFXTimerHandle);
    GetWorldTimerManager().UnPauseTimer(StepSFXTimerHandle);

    bOnRun = false;

    if (bIsMoving)
    {
        Walk();
    }

    const float CurrentMaxSpeed = CharacterMovComp->GetMaxSpeed();
    float NewMaxSpeed           = CurrentMaxSpeed - RunSpeed;

    //If the player is not Slowed Down and the NewSpeed is less than NormalSpeed
    if (NewMaxSpeed < NormalSpeed)
    {
        //Set the NewSpeed to NormalSpeed
        NewMaxSpeed = NormalSpeed;
    }

    CharacterMovComp->MaxWalkSpeed = NewMaxSpeed;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::CheckToStopWalkSound()
{
    if ((CharacterMovComp->IsMovingOnGround()) && (CharacterMovComp->Velocity.Size() == 0) && (!CharacterMovComp->IsFalling()))
    {
        bIsMoving = false;
        GetWorldTimerManager().ClearTimer(StepSFXTimerHandle);
        GetWorldTimerManager().ClearTimer(RunSFXTimerHandle);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::StunOff()
{
    if (bIsDead)
    {
        return;
    }

    bIsStunned = false;

    if (PlayerControllerRef != nullptr)
    {
        PlayerControllerRef->StunFeedbackOff();
    }

    if (bOnRun)
    {
        const float NewSpeed = NormalSpeed + RunSpeed;
        CharacterMovComp->MaxWalkSpeed = NewSpeed;
    }
    else
    {
        CharacterMovComp->MaxWalkSpeed = NormalSpeed;
    }

    if (bIsMoving)
    {
        GetWorldTimerManager().UnPauseTimer(StepSFXTimerHandle);
    }

    GetWorldTimerManager().ClearTimer(StunTimerHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::UpdateLifePercentUI()
{
    if (PlayerControllerRef != nullptr)
    {
        const float LifePercentValue = CurrentLife / MaxLife;
        PlayerControllerRef->UpdateLifePercentHUD(LifePercentValue);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerFPS::Walk()
{
    if ((bIsDead) || (bIsStunned))
    {
        return;
    }

    bIsMoving = true;

    if (CharacterMovComp->IsMovingOnGround())
    {
        if (bOnRun)
        {
            GetWorldTimerManager().PauseTimer(StepSFXTimerHandle);
            GetWorldTimerManager().ClearTimer(RunSFXTimerHandle);
            GetWorldTimerManager().SetTimer(RunSFXTimerHandle, this, &APlayerFPS::StepSound, RunSoundDelay, true, 0.0f);
        }
        else
        {
            GetWorldTimerManager().SetTimer(StepSFXTimerHandle, this, &APlayerFPS::StepSound, WalkSoundDelay, true, 0.0f);
        }
    }
}
