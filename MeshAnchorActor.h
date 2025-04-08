#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CesiumGlobeAnchorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MeshAnchorActor.generated.h"

UCLASS()
class MYPROJECT6_API AMeshAnchorActor : public AActor
{
    GENERATED_BODY()

public:
    AMeshAnchorActor();

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    UCesiumGlobeAnchorComponent* GlobeAnchor;

    void InitMeshAndLocation(UStaticMesh* MeshAsset, FVector GeoCoords);
};
