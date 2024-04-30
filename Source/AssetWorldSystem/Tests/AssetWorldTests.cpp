
#if WITH_AUTOMATION_TESTS

#include "AssetWorldTests.h"
#include "AssetWorldSystem/AssetWorldSubsystem.h"
#include "GameplayTagsManager.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterObjectTest, "AssetWorldSystem.RegisterObject",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterActorTest, "AssetWorldSystem.RegisterActor",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterActorComponentTest, "AssetWorldSystem.RegisterActorComponent",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterUserWidgetTest, "AssetWorldSystem.RegisterUserWidget",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterTextureTest, "AssetWorldSystem.RegisterTexture",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterSoundTest, "AssetWorldSystem.RegisterSound",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterParticleTest, "AssetWorldSystem.RegisterParticle",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterNiagaraTest, "AssetWorldSystem.RegisterNiagara",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterMaterialTest, "AssetWorldSystem.RegisterMaterial",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterStaticMeshTest, "AssetWorldSystem.RegisterStaticMesh",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterSkeletalMeshTest, "AssetWorldSystem.RegisterSkeletalMesh",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetWorldSystemRegisterAnimInstanceTest, "AssetWorldSystem.RegisterAnimInstance",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

template<typename Class>
bool TestObject(FAutomationTestBase* ATB, const FName& RegisterObjectName, ETypeStorageAsset_AWS CheckType)
{
    UWorld* World = GetTestGameWorld();
    if (!ATB->TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!ATB->TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterObjectName);
    FString ErrorTextWhatTagNode = FString::Printf(TEXT("TagNode: %s is nullptr"), *RegisterObjectName.ToString());
    if (!ATB->TestNotNull(ErrorTextWhatTagNode, TagNode.Get())) return false;
    FGameplayTag RegisterObjectTag = TagNode.Get()->GetCompleteTag();

    Class* OriginObject = NewObject<Class>(World, Class::StaticClass(), NAME_None, RF_Transient);
    if (!ATB->TestNotNull("OriginObject is nullptr", OriginObject)) return false;
    AWS->RegisterStorageAsset(RegisterObjectTag, OriginObject);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterObjectTag);
    FString ErrorTypeWhat = FString::Printf(TEXT("The storage cluster is not selected correctly. Must be: %s"), *UEnum::GetValueAsString(CheckType));
    if (!ATB->TestEqual(ErrorTypeWhat, Type, CheckType)) return false;

    Class* FindObject = AWS->FindStorageAssetByType<Class>(CheckType, RegisterObjectTag);
    if (!ATB->TestEqual("Origin object is not equal Find object.", OriginObject, FindObject)) return false;

    AWS->RemoveStorageAsset(CheckType, RegisterObjectTag);
    Class* FindNullObject = AWS->FindStorageAssetByType<Class>(CheckType, RegisterObjectTag);
    if (!ATB->TestNull("FindNullObject is not nullptr", FindNullObject)) return false;

    if (OriginObject)
    {
        OriginObject->MarkAsGarbage();
    }
    return true;
}

bool FAssetWorldSystemRegisterObjectTest::RunTest(const FString& Parameters)
{
    return TestObject<UAWSTestObject>(this, FName(TEXT("AssetWorldSystemTest.RegisterObject")), ETypeStorageAsset_AWS::Object);
}

bool FAssetWorldSystemRegisterActorTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterActorName(TEXT("AssetWorldSystemTest.RegisterActor"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterActorName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterActor is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterActorTag = TagNode.Get()->GetCompleteTag();

    AActor* OriginActor = World->SpawnActor(AActor::StaticClass());
    if (!TestNotNull("OriginActor is nullptr", OriginActor)) return false;
    AWS->RegisterStorageAsset(RegisterActorTag, OriginActor);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterActorTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::Actor", Type, ETypeStorageAsset_AWS::Actor)) return false;

    AActor* FindActor = AWS->FindStorageAssetByType<AActor>(ETypeStorageAsset_AWS::Actor, RegisterActorTag);
    if (!TestEqual("Origin Actor is not equal Find Actor.", OriginActor, FindActor)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::Actor, RegisterActorTag);
    AActor* FindNullActor = AWS->FindStorageAssetByType<AActor>(ETypeStorageAsset_AWS::Actor, RegisterActorTag);
    if (!TestNull("FindNullActor is not nullptr", FindNullActor)) return false;

    if (OriginActor)
    {
        OriginActor->Destroy();
    }
    return true;
}

bool FAssetWorldSystemRegisterActorComponentTest::RunTest(const FString& Parameters)
{
    return TestObject<UAWSTestActorComponent>(this, FName(TEXT("AssetWorldSystemTest.RegisterActorComponent")), ETypeStorageAsset_AWS::ActorComponent);
}

bool FAssetWorldSystemRegisterUserWidgetTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterUserWidgetName(TEXT("AssetWorldSystemTest.RegisterUserWidget"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterUserWidgetName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterUserWidget is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterUserWidgetTag = TagNode.Get()->GetCompleteTag();

    UUserWidget* OriginWidget = UUserWidget::CreateWidgetInstance(*World, UAWSTestUserWidget::StaticClass(), FName(RegisterUserWidgetTag.ToString()));
    if (!TestNotNull("OriginWidget is nullptr", OriginWidget)) return false;

    AWS->RegisterStorageAsset(RegisterUserWidgetTag, OriginWidget);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterUserWidgetTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::UserWidget", Type, ETypeStorageAsset_AWS::UserWidget)) return false;

    UUserWidget* FindUserWidget= AWS->FindStorageAssetByType<UUserWidget>(ETypeStorageAsset_AWS::UserWidget, RegisterUserWidgetTag);
    if (!TestEqual("Origin UserWidget is not equal FindUserWidget.", OriginWidget, FindUserWidget)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::UserWidget, RegisterUserWidgetTag);
    UUserWidget* FindNullUserWidget = AWS->FindStorageAssetByType<UUserWidget>(ETypeStorageAsset_AWS::UserWidget, RegisterUserWidgetTag);
    if (!TestNull("FindNullUserWidget is not nullptr", FindNullUserWidget)) return false;

    if (OriginWidget)
    {
        OriginWidget->MarkAsGarbage();
    }

    return true;
}

bool FAssetWorldSystemRegisterTextureTest::RunTest(const FString& Parameters)
{
    return TestObject<UTexture2D>(this, FName(TEXT("AssetWorldSystemTest.RegisterTexture")), ETypeStorageAsset_AWS::Texture);
}

bool FAssetWorldSystemRegisterSoundTest::RunTest(const FString& Parameters)
{
    return TestObject<USoundCue>(this, FName(TEXT("AssetWorldSystemTest.RegisterSound")), ETypeStorageAsset_AWS::Sound);
}

bool FAssetWorldSystemRegisterParticleTest::RunTest(const FString& Parameters)
{
    return TestObject<UParticleSystem>(this, FName(TEXT("AssetWorldSystemTest.RegisterParticle")), ETypeStorageAsset_AWS::Particle);
}

bool FAssetWorldSystemRegisterNiagaraTest::RunTest(const FString& Parameters)
{
    return TestObject<UNiagaraSystem>(this, FName(TEXT("AssetWorldSystemTest.RegisterNiagara")), ETypeStorageAsset_AWS::Niagara);
}

bool FAssetWorldSystemRegisterMaterialTest::RunTest(const FString& Parameters)
{
    return TestObject<UMaterial>(this, FName(TEXT("AssetWorldSystemTest.RegisterMaterial")), ETypeStorageAsset_AWS::Material);
}

bool FAssetWorldSystemRegisterStaticMeshTest::RunTest(const FString& Parameters)
{
    return TestObject<UStaticMesh>(this, FName(TEXT("AssetWorldSystemTest.RegisterStaticMesh")), ETypeStorageAsset_AWS::StaticMesh);
}

bool FAssetWorldSystemRegisterSkeletalMeshTest::RunTest(const FString& Parameters)
{
    return TestObject<USkeletalMesh>(this, FName(TEXT("AssetWorldSystemTest.RegisterSkeletalMesh")), ETypeStorageAsset_AWS::SkeletalMesh);
}

bool FAssetWorldSystemRegisterAnimInstanceTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterAnimInstanceName(TEXT("AssetWorldSystemTest.RegisterAnimInstance"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterAnimInstanceName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterAnimInstance is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterAnimInstanceTag = TagNode.Get()->GetCompleteTag();

    UAnimInstance* OriginAnimInstance = UAnimInstance::StaticClass()->GetDefaultObject<UAnimInstance>();
    if (!TestNotNull("OriginAnimInstance is nullptr", OriginAnimInstance)) return false;

    AWS->RegisterStorageAsset(RegisterAnimInstanceTag, OriginAnimInstance);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterAnimInstanceTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::AnimInstance", Type, ETypeStorageAsset_AWS::AnimInstance)) return false;

    UAnimInstance* FindAnimInstance = AWS->FindStorageAssetByType<UAnimInstance>(ETypeStorageAsset_AWS::AnimInstance, RegisterAnimInstanceTag);
    if (!TestEqual("OriginAnimInstance is not equal FindAnimInstance.", OriginAnimInstance, FindAnimInstance)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::AnimInstance, RegisterAnimInstanceTag);
    UAnimInstance* FindNullAnimInstance = AWS->FindStorageAssetByType<UAnimInstance>(ETypeStorageAsset_AWS::AnimInstance, RegisterAnimInstanceTag);
    if (!TestNull("FindNullAnimInstance is not nullptr", FindNullAnimInstance)) return false;

    return true;
}

#endif
