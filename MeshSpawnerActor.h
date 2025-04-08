#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshSpawnerActor.generated.h"

// Forward declaration
class UStaticMesh;

UCLASS()
class MYPROJECT6_API AMeshSpawnerActor : public AActor
{
    GENERATED_BODY()

public:
    AMeshSpawnerActor();

protected:
    virtual void BeginPlay() override;

    // Classe base do actor a ser instanciado (MeshAnchorActor)
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AActor> MeshActorClass;

    // Malha estática a ser usada (carregada no constructor)
    UPROPERTY(EditAnywhere, Category = "Spawn")
    UStaticMesh* MeshAsset;

    // Coordenadas geográficas onde os meshes serão instanciados (Longitude, Latitude, Height)
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TArray<FVector> GeoPositions;

public:
    // Lê o ficheiro GeoJSON e preenche o array GeoPositions
    void CarregarGeoJSON(FString Caminho);

};
