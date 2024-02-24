#pragma once

#if WITH_AUTOMATION_TESTS

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Misc/AutomationTest.h"
#include "Blueprint/UserWidget.h"

inline UWorld* GetTestGameWorld()
{
    const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
    auto FindElem = Algo::FindByPredicate(WorldContexts, [](const FWorldContext& Context)
    {
        return (Context.WorldType == EWorldType::PIE || Context.WorldType == EWorldType::Game) && Context.World();
    });

    if (!FindElem)
    {
        FindElem = Algo::FindByPredicate(WorldContexts, [](const FWorldContext& Context)
        {
            return Context.WorldType == EWorldType::Editor && Context.World();
        });
    }

    return FindElem ? FindElem->World() : nullptr;
}

#endif

#include "AssetWorldTests.generated.h"

UCLASS()
class ASSETWORLDSYSTEM_API UAWSTestObject : public UObject
{
    GENERATED_BODY()


};

UCLASS()
class ASSETWORLDSYSTEM_API UAWSTestActorComponent : public UActorComponent
{
    GENERATED_BODY()


};

UCLASS()
class ASSETWORLDSYSTEM_API UAWSTestUserWidget : public UUserWidget
{
    GENERATED_BODY()


};

