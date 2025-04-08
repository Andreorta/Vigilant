#include "MeshAnchorActor.h"
#include "Components/StaticMeshComponent.h"
#include "CesiumGlobeAnchorComponent.h"

AMeshAnchorActor::AMeshAnchorActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Criar o componente Cesium de ancoragem ao globo
    GlobeAnchor = CreateDefaultSubobject<UCesiumGlobeAnchorComponent>(TEXT("GlobeAnchor"));

    // ⚠️ Cast explícito necessário porque GlobeAnchor não é visto como USceneComponent diretamente
    SetRootComponent(Cast<USceneComponent>(GlobeAnchor));

    // Criar StaticMesh e anexá-lo ao componente de ancoragem
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Cast<USceneComponent>(GlobeAnchor));
}

void AMeshAnchorActor::InitMeshAndLocation(UStaticMesh* MeshAsset, FVector GeoCoords)
{
    if (MeshAsset)
    {
        Mesh->SetStaticMesh(MeshAsset);
    }

    FVector3d GeoCoordsD(GeoCoords.X, GeoCoords.Y, GeoCoords.Z);

    // ✅ Finalmente: função correta para versões estáveis do Cesium
    GlobeAnchor->MoveToLongitudeLatitudeHeight(GeoCoordsD);
}

