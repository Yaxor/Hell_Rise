//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
    GENERATED_BODY()
};

class HELL_RISE_API IDamageableInterface
{
    GENERATED_BODY()

public:
    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    UFUNCTION(BlueprintNativeEvent)
    void TakeDamage(float DamageAmount);
};
