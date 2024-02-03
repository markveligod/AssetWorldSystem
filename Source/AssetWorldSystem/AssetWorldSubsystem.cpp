// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetWorldSubsystem.h"
#include "GameplayTagsManager.h"
#include "Engine/AssetManager.h"

#pragma region Default

UAssetWorldSubsystem* UAssetWorldSubsystem::Get(UWorld* World)
{
    if (CLOG_ASSET_WORLD_SYSTEM(World == nullptr, "World is nullptr")) return nullptr;
    return World->GetSubsystem<UAssetWorldSubsystem>();
}

UAssetWorldSubsystem* UAssetWorldSubsystem::GetAssetWorldSubsystemSingleton(UObject* Context)
{
    if (CLOG_ASSET_WORLD_SYSTEM(Context == nullptr, "Context is nullptr")) return nullptr;
    UWorld* World = GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::LogAndReturnNull);
    if (CLOG_ASSET_WORLD_SYSTEM(World == nullptr, "World is nullptr")) return nullptr;
    return World->GetSubsystem<UAssetWorldSubsystem>();
}

void UAssetWorldSubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

#if !UE_BUILD_SHIPPING

    const auto AssetWorldSystemShowDataStorage = IConsoleManager::Get().FindConsoleVariable(TEXT("AssetWorldSystem.ShowDataStorage"));
    if (AssetWorldSystemShowDataStorage && AssetWorldSystemShowDataStorage->GetBool() && GEngine && GetWorld() && !GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        for (auto& Pair : StorageAssets)
        {
            for (auto& Data : Pair.Value)
            {
                FString StrData = FString::Printf(TEXT("    %s"), *Data.ToString());
                GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, StrData);
            }
            FString StrType = FString::Printf(TEXT("Type: [%s]"), *UEnum::GetValueAsString(Pair.Key));
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, StrType);
        }
    }

#endif
}

TStatId UAssetWorldSubsystem::GetStatId() const
{
    return TStatId();
}

void UAssetWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    if (const UEnum* EnumTypeStorage = StaticEnum<ETypeStorageAsset_AWS>())
    {
        for (int32 i = 0; i < EnumTypeStorage->NumEnums() - 1; i++)
        {
            ETypeStorageAsset_AWS TypeElem = static_cast<ETypeStorageAsset_AWS>(i);
            if (!StorageAssets.Contains(TypeElem))
            {
                LOG_ASSET_WORLD_SYSTEM(Display, "Add TypeElem: [%s]", *UEnum::GetValueAsString(TypeElem));
                StorageAssets.Add(TypeElem, {});
            }
        }
    }

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RootName(TEXT("AssetWorldSystem"));
    const TSharedPtr<FGameplayTagNode> FindNode = GameplayTagsManager.FindTagNode(RootName);
    if (!FindNode.IsValid())
    {
        LOG_ASSET_WORLD_SYSTEM(Display, "Add Native Gameplay tag: [%s]", *RootName.ToString());
        GameplayTagsManager.AddNativeGameplayTag(RootName);
    }
}

void UAssetWorldSubsystem::Deinitialize()
{
    Super::Deinitialize();
    LOG_ASSET_WORLD_SYSTEM(Display, "Empty StorageAssets");
    StorageAssets.Empty();
}

#pragma endregion

#pragma region API

bool UAssetWorldSubsystem::RegisterStorageAsset(ETypeStorageAsset_AWS TypeStorage, FGameplayTag Tag, UObject* Asset, bool WithHardRef)
{
    if (CLOG_ASSET_WORLD_SYSTEM(!Tag.IsValid(), "Tag is not valid")) return false;
    if (CLOG_ASSET_WORLD_SYSTEM(!StorageAssets.Contains(TypeStorage), "Type storage is not valid")) return false;
    if (CLOG_ASSET_WORLD_SYSTEM(Asset == nullptr, "Asset is nullptr")) return false;
    if (CLOG_ASSET_WORLD_SYSTEM(FindStorageAsset(TypeStorage, Tag) == Asset, "Asset: [%s] has already been registered", *GetNameSafe(Asset))) return false;

    FStorageAssetData NewData;
    NewData.Tag = Tag;
    NewData.Path = FSoftObjectPath(Asset);
    NewData.Asset = Asset;
    if (WithHardRef)
    {
        NewData.HardRef = TStrongObjectPtr<UObject>(Asset);
    }
    LOG_ASSET_WORLD_SYSTEM(Display, "TypeStorage: [%s] | Add new data: [%s]", *UEnum::GetValueAsString(TypeStorage), *NewData.ToString())
    StorageAssets[TypeStorage].Add(NewData);
    OnRegisterAssetCompleted.Broadcast(TypeStorage, Tag, Asset);
    return true;
}

UObject* UAssetWorldSubsystem::FindStorageAsset(ETypeStorageAsset_AWS TypeStorage, FGameplayTag Tag)
{
    if (CLOG_ASSET_WORLD_SYSTEM(!Tag.IsValid(), "Tag is not valid")) return nullptr;
    if (CLOG_ASSET_WORLD_SYSTEM(!StorageAssets.Contains(TypeStorage), "Type storage is not valid")) return nullptr;
    UObject* Object{nullptr};

    const auto* FindElem = StorageAssets[TypeStorage].FindByPredicate([Tag](const FStorageAssetData& Data)
    {
        return Data.Tag == Tag;
    });
    if (FindElem)
    {
        if (FindElem->Asset.Get())
        {
            Object = FindElem->Asset.Get();
        }

        if (FindElem->HardRef.Get())
        {
            Object = FindElem->HardRef.Get();
        }
    }

    return Object;
}

bool UAssetWorldSubsystem::RemoveStorageAsset(ETypeStorageAsset_AWS TypeStorage, FGameplayTag Tag)
{
    if (CLOG_ASSET_WORLD_SYSTEM(!Tag.IsValid(), "Tag is not valid")) return false;
    if (CLOG_ASSET_WORLD_SYSTEM(!StorageAssets.Contains(TypeStorage), "Type storage is not valid")) return false;

    StorageAssets[TypeStorage].RemoveAll([Tag](const FStorageAssetData& Data)
    {
        return Data.Tag == Tag;
    });
    return true;
}

UObject* UAssetWorldSubsystem::SyncObjectLoading(TSoftObjectPtr<UObject> SoftObject)
{
    if (SoftObject.ToSoftObjectPath().IsValid())
    {
        return SoftObject.LoadSynchronous();
    }
    return nullptr;
}

TSubclassOf<UObject> UAssetWorldSubsystem::SyncClassLoading(TSoftClassPtr<UObject> SoftClass)
{
    if (SoftClass.ToSoftObjectPath().IsValid())
    {
        return SoftClass.LoadSynchronous();
    }
    return {};
}

void UAssetWorldSubsystem::AsyncObjectLoading(TSoftObjectPtr<UObject> SoftObject)
{
    UAssetManager* Manager = UAssetManager::GetIfInitialized();
    if (CLOG_ASSET_WORLD_SYSTEM(Manager == nullptr, "AssetManager is not valid")) return;
    FStreamableManager& StreamableManager = Manager->GetStreamableManager();
    const FSoftObjectPath& Path = SoftObject.ToSoftObjectPath();
    if (CLOG_ASSET_WORLD_SYSTEM(!Path.IsValid(), "Soft object path is not valid")) return;

    if (SoftObject.Get() != nullptr)
    {
        LOG_ASSET_WORLD_SYSTEM(Warning, "Object is loaded in memory | Path: [%s]", *Path.ToString());
    }

    StreamableManager.RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &ThisClass::RegisterAsyncObjectCompleted, SoftObject));
}

void UAssetWorldSubsystem::AsyncObjectLoadingWithCallback(TSoftObjectPtr<UObject> SoftObject,
    const FAsyncLoadingObjectCallbackSignature& Callback)
{
    UAssetManager* Manager = UAssetManager::GetIfInitialized();
    if (CLOG_ASSET_WORLD_SYSTEM(Manager == nullptr, "AssetManager is not valid")) return;
    FStreamableManager& StreamableManager = Manager->GetStreamableManager();
    const FSoftObjectPath& Path = SoftObject.ToSoftObjectPath();
    if (CLOG_ASSET_WORLD_SYSTEM(!Path.IsValid(), "Soft object path is not valid")) return;

    if (SoftObject.Get() != nullptr)
    {
        LOG_ASSET_WORLD_SYSTEM(Warning, "Object is loaded in memory | Path: [%s]", *Path.ToString());
    }

    LOG_ASSET_WORLD_SYSTEM(Display, "Asset request async loading: [%s]", *Path.ToString());
    StreamableManager.RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &ThisClass::RegisterAsyncObjectCompleted, SoftObject, Callback));
}

void UAssetWorldSubsystem::AsyncClassLoading(TSoftClassPtr<UObject> SoftClass)
{
    UAssetManager* Manager = UAssetManager::GetIfInitialized();
    if (CLOG_ASSET_WORLD_SYSTEM(Manager == nullptr, "AssetManager is not valid")) return;
    FStreamableManager& StreamableManager = Manager->GetStreamableManager();
    const FSoftObjectPath& Path = SoftClass.ToSoftObjectPath();
    if (CLOG_ASSET_WORLD_SYSTEM(!Path.IsValid(), "Soft class path is not valid")) return;

    if (SoftClass.Get() != nullptr)
    {
        LOG_ASSET_WORLD_SYSTEM(Warning, "Class is loaded in memory | Path: [%s]", *Path.ToString());
    }

    StreamableManager.RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &ThisClass::RegisterAsyncClassCompleted, SoftClass));
}

void UAssetWorldSubsystem::AsyncClassLoadingWithCallback(TSoftClassPtr<UObject> SoftClass,
    const FAsyncLoadingClassCallbackSignature& Callback)
{
    UAssetManager* Manager = UAssetManager::GetIfInitialized();
    if (CLOG_ASSET_WORLD_SYSTEM(Manager == nullptr, "AssetManager is not valid")) return;
    FStreamableManager& StreamableManager = Manager->GetStreamableManager();
    const FSoftObjectPath& Path = SoftClass.ToSoftObjectPath();
    if (CLOG_ASSET_WORLD_SYSTEM(!Path.IsValid(), "Soft class path is not valid")) return;

    LOG_ASSET_WORLD_SYSTEM(Display, "Class request async loading: [%s]", *Path.ToString());
    StreamableManager.RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &ThisClass::RegisterAsyncClassCompleted, SoftClass, Callback));
}

void UAssetWorldSubsystem::RegisterAsyncObjectCompleted(TSoftObjectPtr<UObject> SoftObject)
{
    LOG_ASSET_WORLD_SYSTEM(Display, "Asset loading complete | Path: [%s] | IsValidObject: [%i]", *SoftObject.ToSoftObjectPath().ToString(), IsValid(SoftObject.Get()));
    OnAsyncLoadingObjectCompleted.Broadcast(SoftObject.Get());
}

void UAssetWorldSubsystem::RegisterAsyncObjectCompleted(TSoftObjectPtr<UObject> SoftObject,
    FAsyncLoadingObjectCallbackSignature Callback)
{
    LOG_ASSET_WORLD_SYSTEM(Display, "Asset loading complete | Path: [%s] | IsValidObject: [%i]", *SoftObject.ToSoftObjectPath().ToString(), IsValid(SoftObject.Get()));
    Callback.ExecuteIfBound(SoftObject.Get());
}

void UAssetWorldSubsystem::RegisterAsyncClassCompleted(TSoftClassPtr<UObject> SoftClass)
{
    LOG_ASSET_WORLD_SYSTEM(Display, "Class loading complete | Path: [%s] | IsValidClass: [%i]", *SoftClass.ToSoftObjectPath().ToString(), IsValid(SoftClass.Get()));
    OnAsyncLoadingClassCompleted.Broadcast(SoftClass.Get());
}

void UAssetWorldSubsystem::RegisterAsyncClassCompleted(TSoftClassPtr<UObject> SoftClass, FAsyncLoadingClassCallbackSignature Callback)
{
    LOG_ASSET_WORLD_SYSTEM(Display, "Asset loading complete | Path: [%s] | IsValidObject: [%i]", *SoftClass.ToSoftObjectPath().ToString(), IsValid(SoftClass.Get()));
    Callback.ExecuteIfBound(SoftClass.Get());
}

#pragma endregion
