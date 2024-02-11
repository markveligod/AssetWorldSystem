#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AssetWorldSystemDataTypes.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogAssetWorldSystem, All, All);

UENUM(BlueprintType)
enum class ETypeStorageAsset_AWS : uint8
{
    Object,
    Actor,
    UserWidget,
    Texture,
    Sound,
    Particle,
    Niagara,
    Material,
    StaticMesh,
    SkeletalMesh,
    AnimInstance,
};

struct FStorageAssetData_AWS
{
    FGameplayTag Tag;
    FSoftObjectPath Path;
    TWeakObjectPtr<> Asset;
    TStrongObjectPtr<UObject> HardRef{nullptr};

    FString ToString() const
    {
        return FString::Printf(TEXT("Tag: [%s] | Path: [%s] | WeakAsset: [%i] | HardRef: [%i]"), *Tag.ToString(), *Path.ToString(), IsValid(Asset.Get()), IsValid(HardRef.Get()));
    }
};

namespace AssetWorldSystemSpace
{
inline bool IsLogPrint()
{
#if WITH_EDITOR
    return true;
#endif
#if !UE_BUILD_SHIPPING
    const auto AssetWorldSystemShowLog = IConsoleManager::Get().FindConsoleVariable(TEXT("AssetWorldSystem.ShowLog"));
    return AssetWorldSystemShowLog ? AssetWorldSystemShowLog->GetBool() : false;
#endif
    return false;
}

inline bool ClogPrint(bool Cond, TCHAR* NameFunction, const FString& Text)
{
    if (Cond)
    {
        UE_LOG(LogAssetWorldSystem, Error, TEXT("[%s] | TEXT:[%s]"), NameFunction, *Text);
    }
    return Cond;
}
}  // namespace AssetWorldSystemSpace

#define LOG_ASSET_WORLD_SYSTEM(Verbosity, Format, ...)                                                                 \
    {                                                                                                             \
        if (AssetWorldSystemSpace::IsLogPrint())                                                                  \
        {                                                                                                         \
            const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__);                                     \
            UE_LOG(LogAssetWorldSystem, Verbosity, TEXT("[%s] | TEXT:[%s]"), ANSI_TO_TCHAR(__FUNCTION__), *Msg); \
        }                                                                                                         \
    }

#define CLOG_ASSET_WORLD_SYSTEM(Cond, Format, ...) \
    AssetWorldSystemSpace::ClogPrint(Cond, ANSI_TO_TCHAR(__FUNCTION__), FString::Printf(TEXT(Format), ##__VA_ARGS__))


DECLARE_DYNAMIC_DELEGATE_OneParam(FAsyncLoadingObjectCallbackSignature, UObject*, Asset);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAsyncLoadingClassCallbackSignature, TSubclassOf<UObject>, Class);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncLoadingObjectCompletedSignature, UObject*, Asset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncLoadingClassCompletedSignature, TSubclassOf<UObject>, Class);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRegisterAssetCompletedSignature, ETypeStorageAsset_AWS, TypeStorage, FGameplayTag, Tag, UObject*, Asset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAssetRemovedSignature, ETypeStorageAsset_AWS, TypeStorage, FGameplayTag, Tag);
