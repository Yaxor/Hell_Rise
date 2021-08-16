//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Hell_Rise/Interfaces/DamageableInterface.h"

#include "PlayerFPS.generated.h"

UCLASS()
class HELL_RISE_API APlayerFPS : public ACharacter, public IDamageableInterface
{
    GENERATED_BODY()

    friend class APlayerControllerFPS;
    friend class ABaseWeapon;
    friend class ARifle;

public:

    //!Constructor
    APlayerFPS();

    //*******************************************************************************************************************
    //                                                  COMPONENTS                                                      *
    //*******************************************************************************************************************

    UPROPERTY(EditAnywhere , Category = Components, meta = (AllowPrivateAccess = "true"))
    class USkeletalMeshComponent* ArmsMesh;
    UPROPERTY(EditAnywhere , Category = Components, meta = (AllowPrivateAccess = "true"))
    class UCapsuleComponent* CapsuleComp;
    UPROPERTY(EditAnywhere , Category = Components, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArm;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* PlayerCamera;

    class UCharacterMovementComponent* CharacterMovComp;

    //*******************************************************************************************************************
    //                                                 PUBLIC VARIABLES                                                 *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadWrite, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
    class ABaseWeapon* EquippedWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "Weapons")
    TSubclassOf<class AAssaultRifle> AssaultRifle;
    UPROPERTY(EditDefaultsOnly, Category = "Weapons")
    TSubclassOf<class ARifle>        Rifle;

    //*******************************************************************************************************************
    //                                                 PUBLIC FUNCTIONS                                                 *
    //*******************************************************************************************************************

    /*Update Current Bullets and Max Bullets in UI*/
    void UpdateCurrentBulletsAmount();
    /*Set the Magazines to Max Amount*/
    UFUNCTION(BlueprintCallable)
    void PickAmmo();
    /*Healing the Player*/
    UFUNCTION(BlueprintCallable)
    void PickHeal(uint8 HealAmount);
    /*Increase the Walk Speed*/
    UFUNCTION(BlueprintCallable)
    void PickSpeedUpBoost();
    /*Decrease the Walk Speed*/
    UFUNCTION(BlueprintCallable)
    void PickSpeedDownDeBuff();
    /*Set to 0 the Walk Speed*/
    UFUNCTION(BlueprintCallable)
    void StunPlayer();

    //*******************************************************************************************************************
    //                                               PUBLIC INLINE FUNCTIONS                                            *
    //*******************************************************************************************************************

    FORCEINLINE bool GetPlayerIsAlive() { return !bIsDead; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE uint8 GetPlayerMaxLife() { return MaxLife; }

    //*******************************************************************************************************************
    //                                                 INTERFACES FUNCTIONS                                             *
    //*******************************************************************************************************************

    virtual void TakeDamage_Implementation(float DamageAmount) override;

protected:

    //*******************************************************************************************************************
    //                                                  PROTECTED FUNCTIONS                                             *
    //*******************************************************************************************************************

    /*Call in end of Reload Animation, set bOnReload to false*/
    UFUNCTION(BlueprintCallable)
    void OnReloadEnd();
    /*Call in end of Change Weapon Animation, set bOnChangeWeapon to false*/
    UFUNCTION(BlueprintCallable)
    void OnWeaponChangeEnd();
    /*Call in the middle of the change weapon animation, change weapon*/
    UFUNCTION(BlueprintCallable)
    void OnWeaponChange();
    /*Call when the weapon is ready before the change*/
    UFUNCTION(BlueprintCallable)
    void OnWeaponReady();
    /*Call when player receive damage or Pick Heal, Update CurrentLife in GameInstance*/
    UFUNCTION(BlueprintImplementableEvent)
    void OnUpdateCurrentLifeGI();

    //*******************************************************************************************************************
    //                                                  PROTECTED VARIABLES                                             *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadOnly, Category = "Anim Data")
    bool bOnChangeWeapon;
    UPROPERTY(BlueprintReadOnly, Category = "Anim Data")
    bool bOnFireWeapon;
    UPROPERTY(BlueprintReadOnly, Category = "Anim Data")
    bool bOnReload;
    UPROPERTY(BlueprintReadOnly, Category = "Anim Data")
    bool bOnRun;

    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    float CurrentLife;

private:

    //*******************************************************************************************************************
    //                                                  PRIVATE VARIABLES                                               *
    //*******************************************************************************************************************

    bool bIsDead;
    bool bIsStunned;
    bool bIsMoving;
    bool bIsJumping;
    bool bIsSlowedDown;
    bool bIsSpeedUp;

    UPROPERTY(EditDefaultsOnly  , Category = "Stats")
    float NormalSpeed;
    UPROPERTY(EditDefaultsOnly  , Category = "Stats")
    float SpeedBuff;
    UPROPERTY(EditDefaultsOnly  , Category = "Stats")
    float SpeedDeBuff;
    UPROPERTY(EditDefaultsOnly  , Category = "Stats")
    float RunSpeed;

    UPROPERTY(EditDefaultsOnly  , Category = "Stats")
    float BoostDuration;
    UPROPERTY(EditDefaultsOnly  , Category = "Stats")
    float StunDuration;
    UPROPERTY(EditDefaultsOnly  , Category = "Stats")
    float DamageFeedbackDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    float WalkSoundDelay;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    float RunSoundDelay;

    const uint8 MaxLife = 100;

    const FName DamageScreenParamName  = "BloodScreenAlpha";
    const FName AssaultRifleSocket     = "AssaultRifleSocket";
    const FName AssaultRifleBackSocket = "AssaultRifleBackSocket";
    const FName RifleSocket            = "RifleSocket";
    const FName RifleBackSocket        = "RifleBackSocket";

    uint8_t CurrentWeaponIndex;

    class APlayerControllerFPS* PlayerControllerRef;

    class AAssaultRifle* AssaultRifleWeapon;
    class ARifle*        RifleWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* DeathSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* HurtSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* JumpSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* ReadySFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* StepSFX;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundCue* SwitchWeaponSFX;

    UPROPERTY(EditAnywhere, Category = "Material")
    UMaterialInterface* MyUIInterface;

    UMaterialInstanceDynamic* MyUIMaterial;

    FTimerHandle BloodScreenTimerHandle;
    FTimerHandle BoostTimerHandle;
    FTimerHandle GoToLoseMapTimerHandle;
    FTimerHandle RunSFXTimerHandle;
    FTimerHandle StepSFXTimerHandle;
    FTimerHandle StunTimerHandle;
    FTimerHandle UpdateUIBeginPlayHandle;

    //*******************************************************************************************************************
    //                                                  PRIVATE FUNCTIONS                                               *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /*Change the equipped weapon to that of the number passed by parameter*/
    void ChangeWeapon(uint8_t WeaponIndex);
    /*Check if the player is in the air or jumping to activate or deactivate steps sounds*/
    void CheckIsOnAir();
    /*Spawn and Position Weapons*/
    void CreateWeapons();
    /*Do Temporarily Damage Feedback*/
    void DamageFeedback();
    /*Set bIsDead true and death feedback*/
    void Death();
    //Change Map to LoseMap 
    void GoToLoseMap();
    /*Start Hurt feedback*/
    void HurtFeedback();
    /*Stop Hurt feedback*/
    void HurtFeedbackOff();
    /*Play Jump Sound*/
    void JumpSound();
    /*Reload Current Weapon*/
    void PlayerReload();
    /*Call the Fire Method of the equipped weapon*/
    void PlayerStartFire();
    /*Call the StopFire Method of the equipped weapon*/
    void PlayerStopFire();
    /*Increase the Move Speed in RunSpeed*/
    void Run();
    /*Set the MaxSpeed to NormalSpeed*/
    void ReturnToNormalSpeed();
    /*Play Step Sound*/
    void StepSound();
    /*Decrease the Move Speed in RunSpeed*/
    void StopRunning();
    /*Stop Walk Sound*/
    void CheckToStopWalkSound();
    /*Turn off the stun*/
    void StunOff();
    /*Update Life in UI*/
    void UpdateLifePercentUI();
    /*Initialize Walk feedback*/
    void Walk();
};
