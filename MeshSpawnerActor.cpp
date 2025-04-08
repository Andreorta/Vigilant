#include "MeshSpawnerActor.h"
#include "MeshAnchorActor.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "JsonUtilities.h"

// Construtor
AMeshSpawnerActor::AMeshSpawnerActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Carregar a mesh SM_Cork_Oak_1
    ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("/Game/DZ_Trees/Meshes/Cork_Oak/SM_Cork_Oak_1.SM_Cork_Oak_1"));
    if (FoundMesh.Succeeded())
    {
        MeshAsset = FoundMesh.Object;
    }
}

// BeginPlay
void AMeshSpawnerActor::BeginPlay()
{
    Super::BeginPlay();

    // Carregar as posições do ficheiro GeoJSON
    FString GeoJsonPath = FPaths::ProjectContentDir() + TEXT("tree.geojson");
    CarregarGeoJSON(GeoJsonPath);

    if (!MeshActorClass || !MeshAsset || GeoPositions.Num() == 0) return;    

    // Spawna para cada coordenada geográfica
    for (const FVector& GeoCoord : GeoPositions)
    {
        AMeshAnchorActor* NewActor = GetWorld()->SpawnActor<AMeshAnchorActor>(
            MeshActorClass, FVector::ZeroVector, FRotator::ZeroRotator);

        if (NewActor)
        {
            NewActor->InitMeshAndLocation(MeshAsset, GeoCoord);
           
        }
    }
}

// Lê as coordenadas do GeoJSON
void AMeshSpawnerActor::CarregarGeoJSON(FString Caminho)
{
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *Caminho))
    {
        UE_LOG(LogTemp, Error, TEXT("Erro ao abrir o ficheiro GeoJSON: %s"), *Caminho);
        return;
    }

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
    TSharedPtr<FJsonObject> JsonObject;

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Erro ao fazer parsing do GeoJSON."));
        return;
    }

    const TArray<TSharedPtr<FJsonValue>>* FeaturesArray;
    if (JsonObject->TryGetArrayField(TEXT("features"), FeaturesArray))
    {
        for (const TSharedPtr<FJsonValue>& FeatureVal : *FeaturesArray)
        {
            TSharedPtr<FJsonObject> FeatureObject = FeatureVal->AsObject();
            if (!FeatureObject.IsValid()) continue;

            if (FeatureObject->HasField(TEXT("geometry")))
            {
                TSharedPtr<FJsonObject> Geometry = FeatureObject->GetObjectField(TEXT("geometry"));
                const TArray<TSharedPtr<FJsonValue>>* CoordinatesArray;

                if (Geometry->TryGetArrayField(TEXT("coordinates"), CoordinatesArray) &&
                    CoordinatesArray->Num() >= 2)
                {
                    float Lon = static_cast<float>((*CoordinatesArray)[0]->AsNumber());
                    float Lat = static_cast<float>((*CoordinatesArray)[1]->AsNumber());
                    float Height = 138.0f;

                    if (CoordinatesArray->Num() >= 3)
                        Height = static_cast<float>((*CoordinatesArray)[2]->AsNumber());

                    GeoPositions.Add(FVector(Lon, Lat, Height));
                }
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Foram carregadas %d coordenadas do GeoJSON."), GeoPositions.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Campo 'features' não encontrado no GeoJSON."));
    }
}
