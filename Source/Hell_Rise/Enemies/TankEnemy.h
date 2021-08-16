//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "TankEnemy.generated.h"

UCLASS()
class HELL_RISE_API ATankEnemy : public ABaseEnemy
{
    GENERATED_BODY()

public:
    //!Constructor
    ATankEnemy();

protected:
    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    const uint8  FollowPlayerDelay = 1;
    const uint8  Damage            = 50;
    const uint16 AttackDistance    = 400;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void AttackHit() override;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
};
