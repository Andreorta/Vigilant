#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshSpawnerActor.generated.h"

class UStaticMesh;

UCLASS()
class MYPROJECT6_API AMeshSpawnerActor : public AActor
{
    GENERATED_BODY()

public:
    AMeshSpawnerActor();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AActor> MeshActorClass;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    UStaticMesh* MeshAsset;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    TArray<FVector> GeoPositions;

    FTimerHandle RetryHandle;

    void SpawnTreesComRetry();

    void CarregarGeoJSON(FString Caminho);
};
