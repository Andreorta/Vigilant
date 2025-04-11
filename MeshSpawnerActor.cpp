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
#include "DrawDebugHelpers.h"
#include "CesiumGeoreference.h"
#include "CesiumGlobeAnchorComponent.h"
#include "EngineUtils.h"
#include "TimerManager.h"

AMeshSpawnerActor::AMeshSpawnerActor()
{
    PrimaryActorTick.bCanEverTick = false;

    ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("/Game/DZ_Trees/Meshes/Cork_Oak/SM_Cork_Oak_1.SM_Cork_Oak_1"));
    if (FoundMesh.Succeeded())
    {
        MeshAsset = FoundMesh.Object;
    }
}

void AMeshSpawnerActor::BeginPlay()
{
    Super::BeginPlay();

    FString GeoJsonPath = FPaths::ProjectContentDir() + TEXT("tree.geojson");
    CarregarGeoJSON(GeoJsonPath);

    GetWorld()->GetTimerManager().SetTimer(RetryHandle, this, &AMeshSpawnerActor::SpawnTreesComRetry, 2.0f, false);
}

void AMeshSpawnerActor::CarregarGeoJSON(FString Caminho)
{
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *Caminho)) return;

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
    TSharedPtr<FJsonObject> JsonObject;

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid()) return;

    const TArray<TSharedPtr<FJsonValue>>* FeaturesArray;
    if (!JsonObject->TryGetArrayField(TEXT("features"), FeaturesArray)) return;

    for (const TSharedPtr<FJsonValue>& FeatureVal : *FeaturesArray)
    {
        TSharedPtr<FJsonObject> FeatureObject = FeatureVal->AsObject();
        if (!FeatureObject.IsValid()) continue;

        if (FeatureObject->HasField(TEXT("geometry")))
        {
            TSharedPtr<FJsonObject> Geometry = FeatureObject->GetObjectField(TEXT("geometry"));
            const TArray<TSharedPtr<FJsonValue>>* CoordinatesArray;

            if (Geometry->TryGetArrayField(TEXT("coordinates"), CoordinatesArray) && CoordinatesArray->Num() >= 2)
            {
                float Lon = (*CoordinatesArray)[0]->AsNumber();
                float Lat = (*CoordinatesArray)[1]->AsNumber();
                float Height = 200.0f; // Altura inicial alta

                GeoPositions.Add(FVector(Lon, Lat, Height));
            }
        }
    }
}

void AMeshSpawnerActor::SpawnTreesComRetry()
{
    if (!MeshActorClass || !MeshAsset || GeoPositions.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Dados insuficientes para spawn: MeshActorClass ou MeshAsset ou GeoPositions."));
        return;
    }

    for (const FVector& GeoCoord : GeoPositions)
    {
        AMeshAnchorActor* NewActor = GetWorld()->SpawnActor<AMeshAnchorActor>(
            MeshActorClass, FVector::ZeroVector, FRotator::ZeroRotator);

        if (NewActor)
        {
            NewActor->InitMeshAndLocation(MeshAsset, GeoCoord);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Árvores spawnadas com sucesso após espera."));
}
