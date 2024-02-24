
#if WITH_AUTOMATION_TESTS

#include "AssetWorldTests.h"
#include "AssetWorldSystem/AssetWorldSubsystem.h"
#include "GameplayTagsManager.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterObjectTest, "AssetWorldSystem.RegisterObject",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterActorTest, "AssetWorldSystem.RegisterActor",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterActorComponentTest, "AssetWorldSystem.RegisterActorComponent",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterUserWidgetTest, "AssetWorldSystem.RegisterUserWidget",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterTextureTest, "AssetWorldSystem.RegisterTexture",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterSoundTest, "AssetWorldSystem.RegisterSound",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterParticleTest, "AssetWorldSystem.RegisterParticle",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterNiagaraTest, "AssetWorldSystem.RegisterNiagara",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterMaterialTest, "AssetWorldSystem.RegisterMaterial",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterStaticMeshTest, "AssetWorldSystem.RegisterStaticMesh",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterSkeletalMeshTest, "AssetWorldSystem.RegisterSkeletalMesh",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegisterAnimInstanceTest, "AssetWorldSystem.RegisterAnimInstance",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)


bool FRegisterObjectTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterObjectName(TEXT("AssetWorldSystemTest.RegisterObject"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterObjectName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterObject is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterObjectTag = TagNode.Get()->GetCompleteTag();

    UObject* OriginObject = NewObject<UAWSTestObject>(World, UAWSTestObject::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginObject is nullptr", OriginObject)) return false;
    AWS->RegisterStorageAsset(RegisterObjectTag, OriginObject);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterObjectTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::Object", Type, ETypeStorageAsset_AWS::Object)) return false;

    UObject* FindObject = AWS->FindStorageAsset(ETypeStorageAsset_AWS::Object, RegisterObjectTag);
    if (!TestEqual("Origin object is not equal Find object.", OriginObject, FindObject)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::Object, RegisterObjectTag);
    UObject* FindNullObject = AWS->FindStorageAsset(ETypeStorageAsset_AWS::Object, RegisterObjectTag);
    if (!TestNull("FindNullObject is not nullptr", FindNullObject)) return false;

    if (OriginObject)
    {
        OriginObject->MarkAsGarbage();
    }

    return true;
}

bool FRegisterActorTest::RunTest(const FString& Parameters)
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

bool FRegisterActorComponentTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterActorComponentName(TEXT("AssetWorldSystemTest.RegisterActorComponent"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterActorComponentName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterActorComponent is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterActorComponentTag = TagNode.Get()->GetCompleteTag();

    UActorComponent* OriginActorComponent = NewObject<UAWSTestActorComponent>(World, UAWSTestActorComponent::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginActorComponent is nullptr", OriginActorComponent)) return false;

    AWS->RegisterStorageAsset(RegisterActorComponentTag, OriginActorComponent);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterActorComponentTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::ActorComponent", Type, ETypeStorageAsset_AWS::ActorComponent)) return false;

    UActorComponent* FindActorComponent = AWS->FindStorageAssetByType<UActorComponent>(ETypeStorageAsset_AWS::ActorComponent, RegisterActorComponentTag);
    if (!TestEqual("Origin ActorComponent is not equal Find ActorComponent.", OriginActorComponent, FindActorComponent)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::ActorComponent, RegisterActorComponentTag);
    UActorComponent* FindNullActorComponent = AWS->FindStorageAssetByType<UActorComponent>(ETypeStorageAsset_AWS::ActorComponent, RegisterActorComponentTag);
    if (!TestNull("FindNullActorComponent is not nullptr", FindNullActorComponent)) return false;

    if (OriginActorComponent)
    {
        OriginActorComponent->MarkAsGarbage();
    }
    
    return true;
}

bool FRegisterUserWidgetTest::RunTest(const FString& Parameters)
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

bool FRegisterTextureTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterTextureName(TEXT("AssetWorldSystemTest.RegisterTexture"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterTextureName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterTexture is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterTextureTag = TagNode.Get()->GetCompleteTag();

    UTexture2D* OriginTexture2D = NewObject<UTexture2D>(World, UTexture2D::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginTexture2D is nullptr", OriginTexture2D)) return false;

    AWS->RegisterStorageAsset(RegisterTextureTag, OriginTexture2D);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterTextureTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::Texture", Type, ETypeStorageAsset_AWS::Texture)) return false;

    UTexture2D* FindTexture2D = AWS->FindStorageAssetByType<UTexture2D>(ETypeStorageAsset_AWS::Texture, RegisterTextureTag);
    if (!TestEqual("Origin Texture2D is not equal Find Texture2D.", OriginTexture2D, FindTexture2D)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::Texture, RegisterTextureTag);
    UActorComponent* FindNullTexture2D = AWS->FindStorageAssetByType<UActorComponent>(ETypeStorageAsset_AWS::Texture, RegisterTextureTag);
    if (!TestNull("FindNullTexture2D is not nullptr", FindNullTexture2D)) return false;

    if (OriginTexture2D)
    {
        OriginTexture2D->MarkAsGarbage();
    }
    
    return true;
}

bool FRegisterSoundTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterSoundName(TEXT("AssetWorldSystemTest.RegisterSound"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterSoundName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterSound is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterSoundTag = TagNode.Get()->GetCompleteTag();

    USoundBase* OriginSound = NewObject<USoundCue>(World, USoundCue::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginSound is nullptr", OriginSound)) return false;

    AWS->RegisterStorageAsset(RegisterSoundTag, OriginSound);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterSoundTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::Sound", Type, ETypeStorageAsset_AWS::Sound)) return false;

    USoundBase* FindSound = AWS->FindStorageAssetByType<USoundBase>(ETypeStorageAsset_AWS::Sound, RegisterSoundTag);
    if (!TestEqual("Origin Sound is not equal Find Sound.", OriginSound, FindSound)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::Sound, RegisterSoundTag);
    USoundBase* FindNullSound = AWS->FindStorageAssetByType<USoundBase>(ETypeStorageAsset_AWS::Sound, RegisterSoundTag);
    if (!TestNull("FindNullSound is not nullptr", FindNullSound)) return false;

    if (OriginSound)
    {
        OriginSound->MarkAsGarbage();
    }

    return true;
}

bool FRegisterParticleTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterParticleName(TEXT("AssetWorldSystemTest.RegisterParticle"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterParticleName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterParticle is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterParticleTag = TagNode.Get()->GetCompleteTag();

    UParticleSystem* OriginParticleSystem = NewObject<UParticleSystem>(World, UParticleSystem::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginParticleSystem is nullptr", OriginParticleSystem)) return false;

    AWS->RegisterStorageAsset(RegisterParticleTag, OriginParticleSystem);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterParticleTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::Particle", Type, ETypeStorageAsset_AWS::Particle)) return false;

    UParticleSystem* FindParticleSystem = AWS->FindStorageAssetByType<UParticleSystem>(ETypeStorageAsset_AWS::Particle, RegisterParticleTag);
    if (!TestEqual("OriginParticleSystem is not equal FindParticleSystem.", OriginParticleSystem, FindParticleSystem)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::Particle, RegisterParticleTag);
    UParticleSystem* FindNullParticleSystem = AWS->FindStorageAssetByType<UParticleSystem>(ETypeStorageAsset_AWS::Particle, RegisterParticleTag);
    if (!TestNull("FindNullParticleSystem is not nullptr", FindNullParticleSystem)) return false;

    if (OriginParticleSystem)
    {
        OriginParticleSystem->MarkAsGarbage();
    }
    return true;
}

bool FRegisterNiagaraTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterNiagaraName(TEXT("AssetWorldSystemTest.RegisterNiagara"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterNiagaraName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterNiagara is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterNiagaraTag = TagNode.Get()->GetCompleteTag();

    UNiagaraSystem* OriginNiagaraSystem = NewObject<UNiagaraSystem>(World, UNiagaraSystem::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginNiagaraSystem is nullptr", OriginNiagaraSystem)) return false;

    AWS->RegisterStorageAsset(RegisterNiagaraTag, OriginNiagaraSystem);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterNiagaraTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::Niagara", Type, ETypeStorageAsset_AWS::Niagara)) return false;

    UNiagaraSystem* FindNiagaraSystem = AWS->FindStorageAssetByType<UNiagaraSystem>(ETypeStorageAsset_AWS::Niagara, RegisterNiagaraTag);
    if (!TestEqual("OriginNiagaraSystem is not equal FindNiagaraSystem.", OriginNiagaraSystem, FindNiagaraSystem)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::Niagara, RegisterNiagaraTag);
    UNiagaraSystem* FindNullNiagaraSystem = AWS->FindStorageAssetByType<UNiagaraSystem>(ETypeStorageAsset_AWS::Niagara, RegisterNiagaraTag);
    if (!TestNull("FindNullParticleSystem is not nullptr", FindNullNiagaraSystem)) return false;

    if (OriginNiagaraSystem)
    {
        OriginNiagaraSystem->MarkAsGarbage();
    }
    return true;
}

bool FRegisterMaterialTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterMaterialName(TEXT("AssetWorldSystemTest.RegisterMaterial"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterMaterialName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterMaterial is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterMaterialTag = TagNode.Get()->GetCompleteTag();

    UMaterial* OriginMaterial = NewObject<UMaterial>(World, UMaterial::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginMaterial is nullptr", OriginMaterial)) return false;

    AWS->RegisterStorageAsset(RegisterMaterialTag, OriginMaterial);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterMaterialTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::Material", Type, ETypeStorageAsset_AWS::Material)) return false;

    UMaterial* FindMaterial = AWS->FindStorageAssetByType<UMaterial>(ETypeStorageAsset_AWS::Material, RegisterMaterialTag);
    if (!TestEqual("OriginMaterial is not equal FindMaterial.", OriginMaterial, FindMaterial)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::Material, RegisterMaterialTag);
    UMaterial* FindNullMaterial = AWS->FindStorageAssetByType<UMaterial>(ETypeStorageAsset_AWS::Material, RegisterMaterialTag);
    if (!TestNull("FindNullMaterial is not nullptr", FindNullMaterial)) return false;

    if (OriginMaterial)
    {
        OriginMaterial->MarkAsGarbage();
    }
    return true;
}

bool FRegisterStaticMeshTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterStaticMeshName(TEXT("AssetWorldSystemTest.RegisterStaticMesh"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterStaticMeshName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterStaticMesh is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterStaticMeshTag = TagNode.Get()->GetCompleteTag();

    UStaticMesh* OriginStaticMesh = NewObject<UStaticMesh>(World, UStaticMesh::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginStaticMesh is nullptr", OriginStaticMesh)) return false;

    AWS->RegisterStorageAsset(RegisterStaticMeshTag, OriginStaticMesh);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterStaticMeshTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::StaticMesh", Type, ETypeStorageAsset_AWS::StaticMesh)) return false;

    UStaticMesh* FindStaticMesh = AWS->FindStorageAssetByType<UStaticMesh>(ETypeStorageAsset_AWS::StaticMesh, RegisterStaticMeshTag);
    if (!TestEqual("OriginStaticMesh is not equal FindStaticMesh.", OriginStaticMesh, FindStaticMesh)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::StaticMesh, RegisterStaticMeshTag);
    UStaticMesh* FindNullStaticMesh = AWS->FindStorageAssetByType<UStaticMesh>(ETypeStorageAsset_AWS::StaticMesh, RegisterStaticMeshTag);
    if (!TestNull("FindNullStaticMesh is not nullptr", FindNullStaticMesh)) return false;

    if (OriginStaticMesh)
    {
        OriginStaticMesh->MarkAsGarbage();
    }
    return true;
}

bool FRegisterSkeletalMeshTest::RunTest(const FString& Parameters)
{
    UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UAssetWorldSubsystem* AWS = World->GetSubsystem<UAssetWorldSubsystem>();
    if (!TestNotNull("AssetWorldSubsystem is nullptr", AWS)) return false;

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    const FName RegisterSkeletalMeshName(TEXT("AssetWorldSystemTest.RegisterSkeletalMesh"));
    TSharedPtr<FGameplayTagNode> TagNode = GameplayTagsManager.FindTagNode(RegisterSkeletalMeshName);
    if (!TestNotNull("TagNode: AssetWorldSystemTest.RegisterSkeletalMesh is nullptr", TagNode.Get())) return false;
    FGameplayTag RegisterSkeletalMeshTag = TagNode.Get()->GetCompleteTag();

    USkeletalMesh* OriginSkeletalMesh = NewObject<USkeletalMesh>(World, USkeletalMesh::StaticClass(), NAME_None, RF_Transient);
    if (!TestNotNull("OriginSkeletalMesh is nullptr", OriginSkeletalMesh)) return false;

    AWS->RegisterStorageAsset(RegisterSkeletalMeshTag, OriginSkeletalMesh);
    ETypeStorageAsset_AWS Type = AWS->GetTypeStorageAsset(RegisterSkeletalMeshTag);
    if (!TestEqual("The storage cluster is not selected correctly. Must be: ETypeStorageAsset_AWS::SkeletalMesh", Type, ETypeStorageAsset_AWS::SkeletalMesh)) return false;

    USkeletalMesh* FindSkeletalMesh = AWS->FindStorageAssetByType<USkeletalMesh>(ETypeStorageAsset_AWS::SkeletalMesh, RegisterSkeletalMeshTag);
    if (!TestEqual("OriginSkeletalMesh is not equal FindSkeletalMesh.", OriginSkeletalMesh, FindSkeletalMesh)) return false;

    AWS->RemoveStorageAsset(ETypeStorageAsset_AWS::SkeletalMesh, RegisterSkeletalMeshTag);
    USkeletalMesh* FindNullSkeletalMesh = AWS->FindStorageAssetByType<USkeletalMesh>(ETypeStorageAsset_AWS::SkeletalMesh, RegisterSkeletalMeshTag);
    if (!TestNull("FindNullSkeletalMesh is not nullptr", FindNullSkeletalMesh)) return false;

    if (OriginSkeletalMesh)
    {
        OriginSkeletalMesh->MarkAsGarbage();
    }
    return true;
}

bool FRegisterAnimInstanceTest::RunTest(const FString& Parameters)
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
