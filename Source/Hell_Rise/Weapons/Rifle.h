//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Rifle.generated.h"

UCLASS()
class HELL_RISE_API ARifle : public ABaseWeapon
{
    GENERATED_BODY()

public:

    //!Constructor
    ARifle();

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Call Fire Weapon*/
    virtual void StartFireWeapon() override;
    /*Create a LineTrace to make damage what implement DamageableInterface, VFX and SFX and activate bHasTriggered*/
    virtual void FireWeapon() override;
    /*Stop FireWeapon TimerHadle and reset bHasTriggered*/
    virtual void StopFireWeapon() override;

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    bool bHasTriggered;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    void TriggerOff();
};
