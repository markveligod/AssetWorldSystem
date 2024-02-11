// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AssetWorldSystemDataTypes.h"
#include "AssetWorldSubsystem.generated.h"

/**
 * @class A class for searching and loading an object
 */
UCLASS()
class ASSETWORLDSYSTEM_API UAssetWorldSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

#pragma region Default

public:

    /** @public Get Singleton class **/
    static UAssetWorldSubsystem* Get(UWorld* World);

    /** @public Get Singleton class **/
    UFUNCTION(BlueprintPure)
    static UAssetWorldSubsystem* GetAssetWorldSubsystemSingleton(UObject* Context);

    // FTickableGameObject implementation Begin
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    // FTickableGameObject implementation End

    // USubsystem implementation Begin
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // USubsystem implementation End

#pragma endregion

#pragma region API

public:

    /** @public Register an object in the subsystem **/
    UFUNCTION(BlueprintCallable)
    bool RegisterStorageAsset(UPARAM(meta=(Categories="AssetWorldSystem"))FGameplayTag Tag, UObject* Asset, bool WithHardRef = false);

    /** @public Search for a registered object in the subsystem **/
    UFUNCTION(BlueprintCallable)
    UObject* FindStorageAsset(ETypeStorageAsset_AWS TypeStorage, UPARAM(meta=(Categories="AssetWorldSystem"))FGameplayTag Tag);

    /** @public Deleting an object from the storage **/
    UFUNCTION(BlueprintCallable)
    bool RemoveStorageAsset(ETypeStorageAsset_AWS TypeStorage, UPARAM(meta=(Categories="AssetWorldSystem"))FGameplayTag Tag);

    /** @public Synchronous object loading **/
    UFUNCTION(BlueprintPure)
    UObject* SyncObjectLoading(TSoftObjectPtr<UObject> SoftObject);

    /** @public Synchronous class loading **/
    UFUNCTION(BlueprintPure)
    TSubclassOf<UObject> SyncClassLoading(TSoftClassPtr<UObject> SoftClass);

    /** @public Asynchronous object loading **/
    UFUNCTION(BlueprintCallable)
    void AsyncObjectLoading(TSoftObjectPtr<UObject> SoftObject);

    /** @public Asynchronous object loading with callback delegate **/
    UFUNCTION(BlueprintCallable)
    void AsyncObjectLoadingWithCallback(TSoftObjectPtr<UObject> SoftObject, const FAsyncLoadingObjectCallbackSignature& Callback);

    /** @public Asynchronous class loading **/
    UFUNCTION(BlueprintCallable)
    void AsyncClassLoading(TSoftClassPtr<UObject> SoftClass);

    /** @public Asynchronous class loading with callback delegate **/
    UFUNCTION(BlueprintCallable)
    void AsyncClassLoadingWithCallback(TSoftClassPtr<UObject> SoftClass, const FAsyncLoadingClassCallbackSignature& Callback);

    /** @public Determine the type of object **/
    UFUNCTION(BlueprintPure)
    ETypeStorageAsset_AWS DetermineTypeObject(const UObject* CheckObj);

private:

    /** @private **/
    void RegisterAsyncObjectCompleted(TSoftObjectPtr<UObject> SoftObject);

    /** @private **/
    void RegisterAsyncObjectCompleted(TSoftObjectPtr<UObject> SoftObject, FAsyncLoadingObjectCallbackSignature Callback);

    /** @private **/
    void RegisterAsyncClassCompleted(TSoftClassPtr<UObject> SoftClass);

    /** @private **/
    void RegisterAsyncClassCompleted(TSoftClassPtr<UObject> SoftClass, FAsyncLoadingClassCallbackSignature Callback);

    /** @private **/
    void RegisterValidateStorage();
    
#pragma endregion

#pragma region Data

private:

    /** @private To register an object **/
    TMap<ETypeStorageAsset_AWS, TArray<FStorageAssetData_AWS>> StorageAssets;

    /** @private **/
    FTimerHandle ValidateStorageTimerHandle;

    /** @private **/
    uint8 TargetValidateStorage{0};

#pragma endregion

#pragma region Signature

private:

    /** @private **/
    UPROPERTY(BlueprintAssignable)
    FAsyncLoadingObjectCompletedSignature OnAsyncLoadingObjectCompleted;

    /** @private **/
    UPROPERTY(BlueprintAssignable)
    FAsyncLoadingClassCompletedSignature OnAsyncLoadingClassCompleted;
    
    /** @private **/
    UPROPERTY(BlueprintAssignable)
    FRegisterAssetCompletedSignature OnRegisterAssetCompleted;

    /** @private **/
    UPROPERTY(BlueprintAssignable)
    FAssetRemovedSignature OnAssetRemoved;
    
#pragma endregion
    
};
