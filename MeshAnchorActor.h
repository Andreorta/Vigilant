#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshAnchorActor.generated.h"

class UCesiumGlobeAnchorComponent;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class MYPROJECT6_API AMeshAnchorActor : public AActor
{
    GENERATED_BODY()

public:
    AMeshAnchorActor();

    UPROPERTY(VisibleAnywhere)
    USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere)
    UCesiumGlobeAnchorComponent* GlobeAnchor;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    void InitMeshAndLocation(UStaticMesh* MeshAsset, FVector GeoCoords);

protected:
    virtual void BeginPlay() override;

private:
    void AplicarRaycast();

    FVector InitialGeoCoords;
    FTimerHandle RaycastTimerHandle;
};
