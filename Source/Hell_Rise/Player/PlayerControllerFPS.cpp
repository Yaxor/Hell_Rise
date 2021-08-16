//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#include "PlayerControllerFPS.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerFPS.h"

//------------------------------------------------------------------------------------------------------------------------------------------
APlayerControllerFPS::APlayerControllerFPS()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::BeginPlay()
{
    Super::BeginPlay();

    PlayerFPSRef = CastChecked<APlayerFPS>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    PlayerFPSRef-> PlayerControllerRef = this;

    AddUIToViewport();
    AddPauseMenuToViewport();
    bShowMouseCursor = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::SetupInputComponent()
{
    Super::SetupInputComponent();

    //Axis
    InputComponent->BindAxis("LookUp"     , this, &APlayerControllerFPS::LookUp     );
    InputComponent->BindAxis("LookRight"  , this, &APlayerControllerFPS::LookRight  );
    InputComponent->BindAxis("MoveForward", this, &APlayerControllerFPS::MoveForward);
    InputComponent->BindAxis("MoveRight"  , this, &APlayerControllerFPS::MoveRight  );

    //Actions
    InputComponent->BindAction("Jump"     , IE_Pressed , this, &APlayerControllerFPS::Jump                 );
    InputComponent->BindAction("Movement" , IE_Pressed , this, &APlayerControllerFPS::WalkSoundPlay        );
    InputComponent->BindAction("Pause"    , IE_Pressed , this, &APlayerControllerFPS::PauseTheGame         ).bExecuteWhenPaused = true;
    InputComponent->BindAction("Reload"   , IE_Pressed , this, &APlayerControllerFPS::PlayerReload         );
    InputComponent->BindAction("Run"      , IE_Pressed , this, &APlayerControllerFPS::PlayerRun            );
    InputComponent->BindAction("Run"      , IE_Released, this, &APlayerControllerFPS::PlayerStopRunning    );
    InputComponent->BindAction("Shoot"    , IE_Pressed , this, &APlayerControllerFPS::PlayerStartShoot     );
    InputComponent->BindAction("Shoot"    , IE_Released, this, &APlayerControllerFPS::PlayerEndShoot       );
    InputComponent->BindAction("WeaponOne", IE_Pressed , this, &APlayerControllerFPS::PlayerChangeWeapon<0>);
    InputComponent->BindAction("WeaponTwo", IE_Pressed , this, &APlayerControllerFPS::PlayerChangeWeapon<1>);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::MoveForward(float Val)
{
    if (PlayerFPSRef)
    {
        if (!PlayerFPSRef->bIsDead)
        {
            PlayerFPSRef->AddMovementInput(PlayerFPSRef->GetActorForwardVector(), Val);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::MoveRight(float Val)
{
    if (PlayerFPSRef)
    {
        if (!PlayerFPSRef->bIsDead)
        {
            PlayerFPSRef->AddMovementInput(PlayerFPSRef->GetActorRightVector(), Val);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::Jump()
{
    if (PlayerFPSRef)
    {
        if ((!PlayerFPSRef->bIsDead) && (!PlayerFPSRef->bIsStunned))
        {
            PlayerFPSRef->Jump();
            PlayerFPSRef->JumpSound();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::LookUp(float Val)
{
    if (PlayerFPSRef)
    {
        if ((!PlayerFPSRef->bIsDead) && (!PlayerFPSRef->bIsStunned))
        {
            AddPitchInput(Val);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::LookRight(float Val)
{
    if (PlayerFPSRef)
    {
        if ((!PlayerFPSRef->bIsDead) && (!PlayerFPSRef->bIsStunned))
        {
            AddYawInput(Val);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::PlayerRun()
{
    if (PlayerFPSRef)
    {
        PlayerFPSRef->Run();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::PlayerStopRunning()
{
    if (PlayerFPSRef)
    {
        PlayerFPSRef->StopRunning();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::PlayerStartShoot()
{
    if (PlayerFPSRef)
    {
        if ((!PlayerFPSRef->bIsDead) && (!PlayerFPSRef->bIsStunned))
        {
            PlayerFPSRef->PlayerStartFire();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::PlayerEndShoot()
{
    if (PlayerFPSRef)
    {
        PlayerFPSRef->PlayerStopFire();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::PlayerReload()
{
    if (PlayerFPSRef)
    {
        if ((!PlayerFPSRef->bIsDead) && (!PlayerFPSRef->bIsStunned))
        {
            PlayerFPSRef->PlayerReload();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::PlayerChangeWeapon(const uint8_t Index)
{
    if (PlayerFPSRef)
    {
        if ((!PlayerFPSRef->bIsDead) && (!PlayerFPSRef->bIsStunned))
        {
            PlayerFPSRef->ChangeWeapon(Index);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APlayerControllerFPS::WalkSoundPlay()
{
    if (PlayerFPSRef)
    {
        PlayerFPSRef->Walk();
    }
}
