#include "MeshAnchorActor.h"
#include "CesiumGlobeAnchorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

AMeshAnchorActor::AMeshAnchorActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Componente raiz
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    // Componente de ancoragem Cesium (sem Attach)
    GlobeAnchor = CreateDefaultSubobject<UCesiumGlobeAnchorComponent>(TEXT("GlobeAnchor"));

    // Malha
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(SceneRoot);
}

void AMeshAnchorActor::InitMeshAndLocation(UStaticMesh* MeshAsset, FVector GeoCoords)
{
    if (MeshAsset)
    {
        Mesh->SetStaticMesh(MeshAsset);
    }

    // Coordenadas geográficas
    FVector3d Llh(GeoCoords.X, GeoCoords.Y, GeoCoords.Z);
    GlobeAnchor->MoveToLongitudeLatitudeHeight(Llh);

    // Raycast para solo
    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0, 0, 10000.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.bTraceComplex = true;

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        FVector Impact = Hit.ImpactPoint;
        SetActorLocation(Impact + FVector(0, 0, 5));
        UE_LOG(LogTemp, Warning, TEXT("Raycast OK: altura ajustada para %f"), Impact.Z);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Raycast falhou: altura permanece a 200"));
    }
}
