#include "CoreTypes.h"
#include "Modules/ModuleManager.h"
#include <iostream>

IMPLEMENT_MODULE(FDefaultModuleImpl, HE1);

extern int32 LAUNCH_API GuardedMain(const TCHAR* CmdLine);

#if WITH_EDITOR
#include "Application.h"
#include "Resource/Resource.h"
#include "MainFrame/MainFrm.h"
#include "Document/MFCApplicationDoc.h"
#include "View/MFCApplicationView.h"
//#include "Engine/World.h"
//#include "GameFramework/Actor.h"

void LAUNCH_API EngineTick();

BEGIN_MESSAGE_MAP(CMFCApplication, CWinAppEx)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

CMFCApplication theApp;

BOOL CMFCApplication::InitInstance()
{
	CWinAppEx::InitInstance();
	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 애플리케이션의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMFCApplicationDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CMFCApplicationView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Engine Delegate
	//{
	//	::ActorSpawnedDelegate.AddRaw<CMFCApplication>(this, &CMFCApplication::OnActorSpawned);
	//	::WorldInitializedDelegate.AddRaw<CMFCApplication>(this, &CMFCApplication::OnWorldInitialized);
	//	::PIEToEditorDelegate.AddRaw<CMFCApplication>(this, &CMFCApplication::OnRefreshActors);
	//}

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	// 끌어서 놓기에 대한 열기를 활성화합니다.
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

void CMFCApplication::OnWorldInitialized(UWorld* NewWorld)
{
	//CMainFrame* MainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	//MainFrame->m_wndClassView.DeleteAllItems();
	//MainFrame->m_wndClassView.InsertRootItem(NewWorld->GetName().c_str());
	//MainFrame->m_wndClassView.Invalidate();
	//MainFrame->m_wndClassView.UpdateWindow();
}

void CMFCApplication::OnActorSpawned(AActor* NewActor)
{
	//const FString NewName = NewActor->GetName() + TEXT(" (") + NewActor->GetClass()->GetName() + TEXT(")");
	//// TODO Parent
	//CMainFrame* MainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	//MainFrame->m_wndClassView.InsertItem(NewActor, NewName.c_str(), 1, 1, NULL);
}

void CMFCApplication::OnRefreshActors(vector<shared_ptr<AActor>>& NewActors)
{
	/*OnActorSelected_Details(nullptr);
	CMainFrame* MainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	MainFrame->m_wndClassView.DeleteAllItems();
	MainFrame->m_wndClassView.InsertRootItem(GEngine->GetWorld()->GetName().c_str());

	for (shared_ptr<AActor>& It : NewActors)
	{
		OnActorSpawned(It.get());
	}*/
}

void CMFCApplication::OnActorSelected_Details(void* SelectedActor)
{
	//CMainFrame* MainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	//CMFCPropertyGridCtrl& Details = MainFrame->m_wndProperties.m_wndPropList;
	//AActor* Actor = static_cast<AActor*>(SelectedActor);

	//bool bSameActor = true;
	//if (LastSelectedActor != Actor)
	//{
	//	bSameActor = false;
	//	DetailsUI.clear();
	//	Details.RemoveAll();
	//	LastSelectedActor = Actor;
	//}

	//if (LastSelectedActor == nullptr)
	//{
	//	Details.Invalidate();
	//	Details.UpdateWindow();
	//	return;
	//}

	//// 이름 중복에 유의!
	//{
	//	{
	//		CMFCPropertyGridProperty* Prop_Name = nullptr;
	//		static const FString Prop_NameKey = TEXT("Prop_Name");
	//		if (bSameActor) { Prop_Name = DetailsUI.find(Prop_NameKey)->second; }
	//		else
	//		{
	//			Prop_Name = new CMFCPropertyGridProperty(TEXT("Name"), Actor->GetName().c_str());
	//			DetailsUI.emplace(Prop_NameKey, Prop_Name);
	//			Details.AddProperty(Prop_Name);
	//		}
	//	}
	//	{
	//		vector<UActorComponent*> Components;
	//		Actor->GetComponents<UActorComponent>(Components);
	//		for (UActorComponent* Component : Components)
	//		{
	//			// Component Start
	//			const FString ComponentName = Component->GetName() + TEXT(" (") + Component->GetClass()->GetName() + TEXT(")");
	//			CMFCPropertyGridProperty* Prop_Component = nullptr;
	//			if (bSameActor) { Prop_Component = DetailsUI.find(ComponentName)->second; }
	//			else
	//			{
	//				Prop_Component = new CMFCPropertyGridProperty(ComponentName.c_str());
	//				DetailsUI.emplace(ComponentName, Prop_Component);
	//			}

	//			// Component Reflection Data
	//			{
	//				type Type = resolve(Hash(Component->GetClass()->ClassName.data()));
	//				Type.data([&](meta::data Data)
	//					{
	//						Data.prop([&](meta::prop p)
	//							{
	//								FProperty Prop = p.value().cast<FProperty>();
	//								const FString PropName = to_wstring(Prop.Name);

	//								const FString Prop_Key = Component->GetName() + TEXT("_") + PropName + TEXT("_Prop_Key");

	//								switch (Prop.PropertyType)
	//								{
	//								case T_FVector:
	//								{
	//									FVector* Vector = (FVector*)Data.get(handle(Type.GetNode(), Component)).data();
	//									CMFCPropertyGridProperty* Property = nullptr;

	//									if (bSameActor)
	//									{
	//										Property = DetailsUI.find(Prop_Key)->second;
	//										CMFCPropertyGridProperty* Item = Property->GetSubItem(0);

	//										if (!FMath::IsNearlyEqual(Property->GetSubItem(0)->GetValue().fltVal, Vector->x, 0.1f))
	//										{
	//											Property->GetSubItem(0)->SetValue(Vector->x);
	//										}
	//										if (!FMath::IsNearlyEqual(Property->GetSubItem(1)->GetValue().fltVal, Vector->y, 0.1f))
	//										{
	//											Property->GetSubItem(1)->SetValue(Vector->y);
	//										}
	//										if (!FMath::IsNearlyEqual(Property->GetSubItem(2)->GetValue().fltVal, Vector->z, 0.1f))
	//										{
	//											Property->GetSubItem(2)->SetValue(Vector->z);
	//										}
	//									}
	//									else
	//									{
	//										Property = new CMFCPropertyGridProperty(PropName.c_str());
	//										DetailsUI.emplace(Prop_Key, Property);

	//										Property->AddSubItem(new CMFCPropertyGridProperty(_T("X"), (_variant_t)Vector->x, (PropName + _T(" X 값을 지정합니다.")).c_str(), (DWORD_PTR)&Vector->x));
	//										Property->AddSubItem(new CMFCPropertyGridProperty(_T("Y"), (_variant_t)Vector->y, (PropName + _T(" Y 값을 지정합니다.")).c_str(), (DWORD_PTR)&Vector->y));
	//										Property->AddSubItem(new CMFCPropertyGridProperty(_T("Z"), (_variant_t)Vector->z, (PropName + _T(" Z 값을 지정합니다.")).c_str(), (DWORD_PTR)&Vector->z));
	//										Prop_Component->AddSubItem(Property);
	//									}
	//									break;
	//								}
	//								case T_FRotator:
	//								{
	//									FRotator* Rotator = (FRotator*)Data.get(handle(Type.GetNode(), Component)).data();
	//									CMFCPropertyGridProperty* Property = nullptr;

	//									if (bSameActor)
	//									{
	//										Property = DetailsUI.find(Prop_Key)->second;
	//										CMFCPropertyGridProperty* Item = Property->GetSubItem(0);

	//										if (!FMath::IsNearlyEqual(Property->GetSubItem(0)->GetValue().fltVal, Rotator->Roll, 0.1f))
	//										{
	//											Property->GetSubItem(0)->SetValue(Rotator->Roll);
	//										}
	//										if (!FMath::IsNearlyEqual(Property->GetSubItem(1)->GetValue().fltVal, Rotator->Pitch, 0.1f))
	//										{
	//											Property->GetSubItem(1)->SetValue(Rotator->Pitch);
	//										}
	//										if (!FMath::IsNearlyEqual(Property->GetSubItem(2)->GetValue().fltVal, Rotator->Yaw, 0.1f))
	//										{
	//											Property->GetSubItem(2)->SetValue(Rotator->Yaw);
	//										}
	//									}
	//									else
	//									{
	//										Property = new CMFCPropertyGridProperty(PropName.c_str());
	//										DetailsUI.emplace(Prop_Key, Property);

	//										Property->AddSubItem(new CMFCPropertyGridProperty(_T("Roll"), (_variant_t)Rotator->Roll, (PropName + _T(" Roll 값을 지정합니다.")).c_str(), (DWORD_PTR)&Rotator->Roll));
	//										Property->AddSubItem(new CMFCPropertyGridProperty(_T("Pitch"), (_variant_t)Rotator->Pitch, (PropName + _T(" Pitch 값을 지정합니다.")).c_str(), (DWORD_PTR)&Rotator->Pitch));
	//										Property->AddSubItem(new CMFCPropertyGridProperty(_T("Yaw"), (_variant_t)Rotator->Yaw, (PropName + _T(" Yaw 값을 지정합니다.")).c_str(), (DWORD_PTR)&Rotator->Yaw));
	//										Prop_Component->AddSubItem(Property);
	//									}
	//									break;
	//								}
	//								}
	//							});
	//					});
	//			}

	//			if (!bSameActor)
	//			{
	//				Details.AddProperty(Prop_Component);
	//			}
	//		}
	//	}
	//}
}

void CMFCApplication::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}
void CMFCApplication::LoadCustomState()
{
}
void CMFCApplication::SaveCustomState()
{
}
BOOL CMFCApplication::OnIdle(LONG lCount)
{
	CWinAppEx::OnIdle(lCount);
	//bool bExit = IsEngineExitRequested();
	//if (bExit)
	//{
	//	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
	//	return FALSE;
	//}

	//EngineTick();
	////if (LastSelectedActor)
	////{
	////	OnActorSelected_Details(LastSelectedActor);
	////}

	return TRUE;
}
void CMFCApplication::OnClosingMainFrame(CFrameImpl* pFrameImpl)
{
	CWinAppEx::OnClosingMainFrame(pFrameImpl);
}
void CMFCApplication::OnAppAbout()
{
}
#else
int main()
{
	return GuardedMain(nullptr);
}
#endif