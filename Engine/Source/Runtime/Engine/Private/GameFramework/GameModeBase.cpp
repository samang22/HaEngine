#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

AGameModeBase::AGameModeBase()
{
	PlayerControllerClass = APlayerController::StaticClass();
}

void AGameModeBase::PreInitializeComponents()
{
    Super::PreInitializeComponents();

    FActorSpawnParameters SpawnInfo;
    //SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.ObjectFlags |= RF_Transient;  // GameState나 네트워크 관리자들을 맵에 저장하고 싶지 않습니다.

    // GameState가 지정되지 않은 경우 기본 GameState로 대체합니다.
    if (GameStateClass == nullptr)
    {
        E_LOG(Warning, TEXT("No GameStateClass was specified in {} ({})"), GetName(), GetClass()->GetName());
        GameStateClass = AGameStateBase::StaticClass();
    }

    UWorld* World = GetWorld();
    GameState = World->SpawnActor<AGameStateBase>(GameStateClass, SpawnInfo);
    World->SetGameState(GameState);
    if (GameState)
    {
        GameState->AuthorityGameMode = this;
    }

    // 네트워크 게임에서는 서버에만 NetworkManager가 필요합니다.
    //   AWorldSettings* WorldSettings = World->GetWorldSettings();
    //World->NetworkManager = WorldSettings->GameNetworkManagerClass ? World->SpawnActor<AGameNetworkManager>(WorldSettings->GameNetworkManagerClass, SpawnInfo) : nullptr;

    //InitGameState();
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

void AGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    // 서버 접속 중에도 발생할 수 있는 공통 초기화 실행
    //GenericPlayerInitialization(NewPlayer);

    // 서버에 처음 접속할 때만 발생하는 초기화 수행
    UWorld* World = GetWorld();

    //NewPlayer->ClientCapBandwidth(NewPlayer->Player->CurrentNetSpeed);

    //if (MustSpectate(NewPlayer))
    //{
    //    NewPlayer->ClientGotoState(NAME_Spectating);
    //}
    //else
    //{
    //    // NewPlayer가 단순한 관전자가 아니고 유효한 ID를 가진 경우, 이를 리플레이에 사용자로 추가합니다.
    //    const FUniqueNetIdRepl& NewPlayerStateUniqueId = NewPlayer->PlayerState->GetUniqueId();
    //    if (NewPlayerStateUniqueId.IsValid() && NewPlayerStateUniqueId.IsV1())
    //    {
    //        GetGameInstance()->AddUserToReplay(NewPlayerStateUniqueId.ToString());
    //    }
    //}

    if (GameSession)
    {
        GameSession->PostLogin(NewPlayer);
    }

    DispatchPostLogin(NewPlayer);

    // 초기화가 완료되었으므로, 플레이어의 폰을 생성하고 경기를 시작하려고 시도합니다.
    //HandleStartingNewPlayer(NewPlayer);
}

void AGameModeBase::DispatchPostLogin(AController* NewPlayer)
{
    //if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
    //{
    //    K2_PostLogin(NewPC);
    //    FGameModeEvents::GameModePostLoginEvent.Broadcast(this, NewPC);
    //}

    OnPostLogin(NewPlayer);
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

void AGameModeBase::StartPlay()
{
    GameState->HandleBeginPlay();
}
