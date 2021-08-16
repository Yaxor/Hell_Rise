//+--------------------------------------------------------+
//| Project    : Hell Rise                                 |
//| UE Version : 4.26                                      |
//| Author     : Matias Till                               |
//+--------------------------------------------------------+


#include "CommonBullet.h"

#include "Components/SphereComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "Hell_Rise/Player/PlayerFPS.h"

#include "Kismet/GameplayStatics.h"

//------------------------------------------------------------------------------------------------------------------------------------------
ACommonBullet::ACommonBullet()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>("RootComp");
    SetRootComponent(RootComponent);

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
    BulletMesh->SetupAttachment    (RootComponent);
    BulletMesh->SetRelativeScale3D (FVector (  2.0f, 2.0f, 2.0f));
    BulletMesh->SetRelativeLocation(FVector (  0.0f, 0.0f, 0.0f));
    BulletMesh->SetGenerateOverlapEvents(true);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->ProjectileGravityScale = 0.0f;
    ProjectileMovement->InitialSpeed           = 6000.0f;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ACommonBullet::BeginPlay()
{
    Super::BeginPlay();

    this->SetLifeSpan(LifeTime);

    BulletMesh->OnComponentBeginOverlap.AddDynamic(this, &ACommonBullet::BulletBeginOverlap);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ACommonBullet::MakeDamageToTarget(AActor* TargetToDamage)
{
    if (APlayerFPS* TempPlayer = Cast<APlayerFPS>(TargetToDamage))
    {
        float RandomDamage = FMath::FRandRange(MinDamage, MaxDamage);

        IDamageableInterface::Execute_TakeDamage(TargetToDamage, RandomDamage);
    }

    Destroy(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ACommonBullet::BulletBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    MakeDamageToTarget(OtherActor);
}
