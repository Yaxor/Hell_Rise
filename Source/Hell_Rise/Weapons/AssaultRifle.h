//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "AssaultRifle.generated.h"

UCLASS()
class HELL_RISE_API AAssaultRifle : public ABaseWeapon
{
    GENERATED_BODY()

public:

    //!Constructor
    AAssaultRifle();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
};
