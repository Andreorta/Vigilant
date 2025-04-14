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

    // Componente de ancoragem Cesium
    GlobeAnchor = CreateDefaultSubobject<UCesiumGlobeAnchorComponent>(TEXT("GlobeAnchor"));

    // Malha
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(SceneRoot);

    // Ignorar colisão com outras árvores inicialmente
    Mesh->SetCollisionObjectType(ECC_WorldDynamic);
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

void AMeshAnchorActor::BeginPlay()
{
    Super::BeginPlay();

    // Aguarda 5 segundos antes de aplicar o raycast
    GetWorld()->GetTimerManager().SetTimer(RaycastTimerHandle, this, &AMeshAnchorActor::AplicarRaycast, 5.0f, false);
}

void AMeshAnchorActor::InitMeshAndLocation(UStaticMesh* MeshAsset, FVector GeoCoords)
{
    if (MeshAsset)
    {
        Mesh->SetStaticMesh(MeshAsset);
    }

    FVector3d Llh(GeoCoords.X, GeoCoords.Y, GeoCoords.Z);
    GlobeAnchor->MoveToLongitudeLatitudeHeight(Llh);
    InitialGeoCoords = GeoCoords;
}

void AMeshAnchorActor::AplicarRaycast()
{
    // Ativar colisão apenas com terreno
    //Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0, 0, 10000.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.bTraceComplex = true;

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && !HitActor->IsA(AMeshAnchorActor::StaticClass()))
        {
            FVector Impact = Hit.ImpactPoint;
            SetActorLocation(Impact + FVector(0, 0, 5));
            UE_LOG(LogTemp, Warning, TEXT("Raycast OK: altura ajustada para %f"), Impact.Z);
        }
        //else
        //{
           // UE_LOG(LogTemp, Warning, TEXT("Ignorado: acertou noutra árvore. Eliminada."));
            //Destroy();
        //}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Raycast falhou: árvore destruída."));
       // Destroy();
    }
}
