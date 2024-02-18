// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetWorldSubsystem.h"
#include "GameplayTagsManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"

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
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | | ---"));
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
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | | ---"));
    }

    const auto AssetWorldSystemShowDataValidate = IConsoleManager::Get().FindConsoleVariable(TEXT("AssetWorldSystem.ShowDataValidate"));
    if (AssetWorldSystemShowDataValidate && AssetWorldSystemShowDataValidate->GetBool() && GEngine && GetWorld() && !GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | | ---"));
        const FString NameType = UEnum::GetValueAsString(static_cast<ETypeStorageAsset_AWS>(TargetValidateStorage));
        const FString Result = FString::Printf(TEXT("TargetValidateStorage: [%i] | NameType: [%s]"), TargetValidateStorage, *NameType);
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, Result);
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | | ---"));
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

    if (!IsTemplate() && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(ValidateStorageTimerHandle, this, &ThisClass::RegisterValidateStorage, 1.0f, true);
    }
}

void UAssetWorldSubsystem::Deinitialize()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ValidateStorageTimerHandle);
    }
    Super::Deinitialize();
    LOG_ASSET_WORLD_SYSTEM(Display, "Empty StorageAssets");
    StorageAssets.Empty();
}

#pragma endregion

#pragma region API

bool UAssetWorldSubsystem::RegisterStorageAsset(FGameplayTag Tag, UObject* Asset, bool WithHardRef)
{
    if (CLOG_ASSET_WORLD_SYSTEM(!Tag.IsValid(), "Tag is not valid")) return false;
    if (CLOG_ASSET_WORLD_SYSTEM(Asset == nullptr, "Asset is nullptr")) return false;

    const ETypeStorageAsset_AWS TypeStorage = DetermineTypeObject(Asset);
    if (CLOG_ASSET_WORLD_SYSTEM(!StorageAssets.Contains(TypeStorage), "Type storage is not valid")) return false;
    if (CLOG_ASSET_WORLD_SYSTEM(FindStorageAsset(TypeStorage, Tag) == Asset, "Asset: [%s] has already been registered", *GetNameSafe(Asset))) return false;

    FStorageAssetData_AWS NewData;
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

    const auto* FindElem = StorageAssets[TypeStorage].FindByPredicate([Tag](const FStorageAssetData_AWS& Data)
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

    StorageAssets[TypeStorage].RemoveAll([Tag](const FStorageAssetData_AWS& Data)
    {
        return Data.Tag == Tag;
    });
    return true;
}

UObject* UAssetWorldSubsystem::SyncObjectLoading(const TSoftObjectPtr<UObject>& SoftObject)
{
    if (SoftObject.ToSoftObjectPath().IsValid())
    {
        return SoftObject.LoadSynchronous();
    }
    return nullptr;
}

TArray<UObject*> UAssetWorldSubsystem::SyncArrayObjectLoading(const TArray<TSoftObjectPtr<UObject>>& ArraySoftObject)
{
    TArray<UObject*> Objects;

    for (auto& SoftData : ArraySoftObject)
    {
        if (SoftData.ToSoftObjectPath().IsNull()) continue;
        if (UObject* Object = SoftData.LoadSynchronous())
        {
            Objects.Add(Object);
        }
    }

    return Objects;
}

TSubclassOf<UObject> UAssetWorldSubsystem::SyncClassLoading(const TSoftClassPtr<UObject>& SoftClass)
{
    if (SoftClass.ToSoftObjectPath().IsValid())
    {
        return SoftClass.LoadSynchronous();
    }
    return {};
}

TArray<TSubclassOf<UObject>> UAssetWorldSubsystem::SyncArrayClassLoading(const TArray<TSoftClassPtr<UObject>>& ArraySoftClass)
{
    TArray<TSubclassOf<UObject>> Classes;

    for (auto& SoftClass : ArraySoftClass)
    {
        if (SoftClass.ToSoftObjectPath().IsNull()) continue;
        if (UClass* Class = SoftClass.LoadSynchronous())
        {
            Classes.Add(Class);
        }
    }
    return Classes;
}

void UAssetWorldSubsystem::AsyncObjectLoading(TSoftObjectPtr<UObject> SoftObject)
{
    const UAssetManager* Manager = UAssetManager::GetIfInitialized();
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
    const UAssetManager* Manager = UAssetManager::GetIfInitialized();
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
    const UAssetManager* Manager = UAssetManager::GetIfInitialized();
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
    const UAssetManager* Manager = UAssetManager::GetIfInitialized();
    if (CLOG_ASSET_WORLD_SYSTEM(Manager == nullptr, "AssetManager is not valid")) return;
    FStreamableManager& StreamableManager = Manager->GetStreamableManager();
    const FSoftObjectPath& Path = SoftClass.ToSoftObjectPath();
    if (CLOG_ASSET_WORLD_SYSTEM(!Path.IsValid(), "Soft class path is not valid")) return;

    LOG_ASSET_WORLD_SYSTEM(Display, "Class request async loading: [%s]", *Path.ToString());
    StreamableManager.RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &ThisClass::RegisterAsyncClassCompleted, SoftClass, Callback));
}

ETypeStorageAsset_AWS UAssetWorldSubsystem::DetermineTypeObject(const UObject* CheckObj)
{
    if (!CheckObj) return ETypeStorageAsset_AWS::Object;

    if (CheckObj->IsA(AActor::StaticClass()))
    {
        return ETypeStorageAsset_AWS::Actor;
    }

    if (CheckObj->IsA(UUserWidget::StaticClass()))
    {
        return ETypeStorageAsset_AWS::UserWidget;
    }

    if (CheckObj->IsA(UTexture::StaticClass()))
    {
        return ETypeStorageAsset_AWS::Texture;
    }

    if (CheckObj->IsA(USoundBase::StaticClass()))
    {
        return ETypeStorageAsset_AWS::Sound;
    }

    if (CheckObj->IsA(UParticleSystem::StaticClass()))
    {
        return ETypeStorageAsset_AWS::Particle;
    }

    if (CheckObj->IsA(UNiagaraSystem::StaticClass()))
    {
        return ETypeStorageAsset_AWS::Niagara;
    }

    if (CheckObj->IsA(UMaterialInterface::StaticClass()))
    {
        return ETypeStorageAsset_AWS::Material;
    }

    if (CheckObj->IsA(UStaticMesh::StaticClass()))
    {
        return ETypeStorageAsset_AWS::StaticMesh;
    }

    if (CheckObj->IsA(USkeletalMesh::StaticClass()))
    {
        return ETypeStorageAsset_AWS::SkeletalMesh;
    }

    if (CheckObj->IsA(UAnimInstance::StaticClass()))
    {
        return ETypeStorageAsset_AWS::AnimInstance;
    }

    return ETypeStorageAsset_AWS::Object;
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

void UAssetWorldSubsystem::RegisterValidateStorage()
{
    ETypeStorageAsset_AWS TypeStorage = static_cast<ETypeStorageAsset_AWS>(TargetValidateStorage);
    if (StorageAssets.Contains(TypeStorage))
    {
        TArray<FGameplayTag> OnRemove;
        for (auto& Data : StorageAssets[TypeStorage])
        {
            if (Data.Asset.Get() == nullptr && Data.HardRef.Get() == nullptr)
            {
                OnRemove.Add(Data.Tag);
            }
        }

        for (auto& Data : OnRemove)
        {
            RemoveStorageAsset(TypeStorage, Data);
        }
    }

    const UEnum* ENUM_TypeStorage = StaticEnum<ETypeStorageAsset_AWS>();
    if (!ENUM_TypeStorage) return;
    if (ENUM_TypeStorage->NumEnums() <= TargetValidateStorage + 1)
    {
        TargetValidateStorage = 0;
    }
    else
    {
        ++TargetValidateStorage;
    }
}

#pragma endregion
