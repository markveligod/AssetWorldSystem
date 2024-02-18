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
    UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "SoftObject"))
    static UObject* SyncObjectLoading(const TSoftObjectPtr<UObject>& SoftObject);

    /** @public Synchronous array object loading **/
    UFUNCTION(BlueprintPure)
    static TArray<UObject*> SyncArrayObjectLoading(const TArray<TSoftObjectPtr<UObject>>& ArraySoftObject);

    /** @public Synchronous class loading **/
    UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "SoftClass"))
    static TSubclassOf<UObject> SyncClassLoading(const TSoftClassPtr<UObject>& SoftClass);

    /** @public Synchronous array class loading **/
    UFUNCTION(BlueprintPure)
    static TArray<TSubclassOf<UObject>> SyncArrayClassLoading(const TArray<TSoftClassPtr<UObject>>& ArraySoftClass);

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
    static ETypeStorageAsset_AWS DetermineTypeObject(const UObject* CheckObj);

    /** @public is the timer for checking stored data activated **/
    UFUNCTION(BlueprintPure)
    bool IsActiveValidateStorageTimerHandle() const { return GetWorld() ? GetWorld()->GetTimerManager().IsTimerActive(ValidateStorageTimerHandle) : false; }

    /** @public Getting the value of the target iteration check of the stored data **/
    UFUNCTION(BlueprintPure)
    uint8 GetTargetValidateStorage() const { return TargetValidateStorage; }

private:
    /** @private Registering the completion of asynchronous object loading **/
    void RegisterAsyncObjectCompleted(TSoftObjectPtr<UObject> SoftObject);

    /** @private Registering the completion of asynchronous object loading **/
    void RegisterAsyncObjectCompleted(TSoftObjectPtr<UObject> SoftObject, FAsyncLoadingObjectCallbackSignature Callback);

    /** @private Registering the completion of asynchronous class loading **/
    void RegisterAsyncClassCompleted(TSoftClassPtr<UObject> SoftClass);

    /** @private Registering the completion of asynchronous class loading **/
    void RegisterAsyncClassCompleted(TSoftClassPtr<UObject> SoftClass, FAsyncLoadingClassCallbackSignature Callback);

    /** @private Checking the validity of stored data **/
    void RegisterValidateStorage();

#pragma endregion

#pragma region Data

private:

    /** @private To register an object **/
    TMap<ETypeStorageAsset_AWS, TArray<FStorageAssetData_AWS>> StorageAssets;

    /** @private Timer for checking stored data **/
    FTimerHandle ValidateStorageTimerHandle;

    /** @private The value of the target iteration check of the stored data**/
    uint8 TargetValidateStorage{0};

#pragma endregion

#pragma region Signature

private:
    /** @private  **/
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
