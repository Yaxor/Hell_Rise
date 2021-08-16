//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerFPS.generated.h"

UCLASS()
class HELL_RISE_API APlayerControllerFPS : public APlayerController
{
    GENERATED_BODY()

    friend class APlayerFPS;

public:

    //!Constructor
    APlayerControllerFPS();

    //*******************************************************************************************************************
    //                                             UI FUNCTIONS                                                         *
    //*******************************************************************************************************************

    UFUNCTION(BlueprintImplementableEvent)
    void DeathFeedbackOn();
    UFUNCTION(BlueprintImplementableEvent)
    void FreezeFeedbackOn();
    UFUNCTION(BlueprintImplementableEvent)
    void FreezeFeedbackOff();
    UFUNCTION(BlueprintImplementableEvent)
    void GetDamageFeedback();
    UFUNCTION(BlueprintImplementableEvent)
    void HurtFeedback();
    UFUNCTION(BlueprintImplementableEvent)
    void HurtFeedbackOff();
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void PauseTheGame();
    UFUNCTION(BlueprintImplementableEvent)
    void SpeedUpFeedbackOn();
    UFUNCTION(BlueprintImplementableEvent)
    void SpeedUpFeedbackOff();
    UFUNCTION(BlueprintImplementableEvent)
    void StunFeedbackOn();
    UFUNCTION(BlueprintImplementableEvent)
    void StunFeedbackOff();
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateLifePercentHUD(const float LifeValuePercent);
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateAmmoAmountHUD(const uint8 AmmoAmount, const uint8 MagAmount);

protected:
    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadWrite)
    bool bGameIsOnPause;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    UFUNCTION(BlueprintImplementableEvent)
    void AddUIToViewport();
    UFUNCTION(BlueprintImplementableEvent)
    void AddPauseMenuToViewport();

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    class APlayerFPS* PlayerFPSRef;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    virtual void BeginPlay();
    virtual void SetupInputComponent() override;

    /*Character Move Forward*/
    void MoveForward(float Val);
    /*Character Move Righr*/
    void MoveRight(float Val);
    /*Character Jump*/
    void Jump();
    /*Camera Rotate Vertical*/
    void LookUp(float Val);
    /*Camera Rotate Horizontal*/
    void LookRight(float Val);
    /*Player Run*/
    void PlayerRun();
    /*Player left Run*/
    void PlayerStopRunning();
    /*Call Player Start Shoot Method*/
    void PlayerStartShoot();
    /*Call Player Sop Shoot Method*/
    void PlayerEndShoot();
    /*Call Player Reload*/
    void PlayerReload();
    /*Call Player Weapon Change*/
    void PlayerChangeWeapon(const uint8_t Index);

    template<uint8_t Index>
    void PlayerChangeWeapon()
    {
        PlayerChangeWeapon(Index);
    }

    /*Call the Walk sound player function*/
    void WalkSoundPlay();
};
