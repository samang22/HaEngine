#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

AGameModeBase::AGameModeBase()
{
	PlayerControllerClass = APlayerController::StaticClass();
}

void AGameModeBase::InitGame()
{
    UWorld* World = GetWorld();

    FActorSpawnParameters SpawnInfo;
    //SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.ObjectFlags |= RF_Transient;    // 게임 세션을 맵에 저장하지 않기를 원합니다.
    //GameSession = World->SpawnActor<AGameSession>(GetGameSessionClass(), SpawnInfo);
    GameSession = World->SpawnActor<AGameSession>(AGameSession::StaticClass(), SpawnInfo);
}

APlayerController* AGameModeBase::Login(UPlayer* NewPlayer)
{
    if (GameSession == nullptr)
    {
        //ErrorMessage = TEXT("Failed to spawn player controller, GameSession is null");
        _ASSERT(false);
        return nullptr;
    }

    // 접속자수 관리 등
    /* ErrorMessage = GameSession->ApproveLogin(Options);
    if (!ErrorMessage.IsEmpty())
    {
        return nullptr;
    }*/

    APlayerController* const NewPlayerController = SpawnPlayerController(/*InRemoteRole, Options*/);
    if (NewPlayerController == nullptr)
    {
        _ASSERT(false);
        // Handle spawn failure.
        /*UE_LOG(LogGameMode, Log, TEXT("Login: Couldn't spawn player controller of class %s"), PlayerControllerClass ? *PlayerControllerClass->GetName() : TEXT("NULL"));
        ErrorMessage = FString::Printf(TEXT("Failed to spawn player controller"));*/
        return nullptr;
    }

    return NewPlayerController;
}

APlayerController* AGameModeBase::SpawnPlayerController()
{
    FActorSpawnParameters SpawnInfo;
    //SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.ObjectFlags |= RF_Transient;    // We never want to save player controllers into a map
    //SpawnInfo.bDeferConstruction = true;
    APlayerController* NewPC = GetWorld()->SpawnActor<APlayerController>(PlayerControllerClass, FTransform::Identity, SpawnInfo);
    //if (NewPC)
    //{
    //    if (InRemoteRole == ROLE_SimulatedProxy)
    //    {
    //        // 이는 권한/자율 원격 역할이 없기 때문에 로컬 플레이어입니다.
    //        NewPC->SetAsLocalPlayerController();
    //    }

    //    UGameplayStatics::FinishSpawningActor(NewPC, FTransform(SpawnRotation, SpawnLocation));
    //}

    return NewPC;
}
