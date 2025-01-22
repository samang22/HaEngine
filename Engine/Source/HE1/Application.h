#pragma once

#if WITH_EDITOR
#include "Misc/MFCHeaders.h"

// CMFCApplication:
// 이 클래스의 구현에 대해서는 Application.cpp을(를) 참조하세요.
//
class AActor;

class CMFCApplication : public CWinAppEx
{
	friend class CPropertiesWnd;
public:
	CMFCApplication() noexcept {}

public:
	void OnWorldInitialized(class UWorld* NewWorld);
	void OnActorSpawned(AActor* NewActor);
	void OnRefreshActors(vector<shared_ptr<AActor>>& NewActors);
	void OnActorSelected_Details(void* SelectedActor);

	// 재정의입니다.
protected:
	AActor* LastSelectedActor = nullptr;
	std::map<std::wstring, CMFCPropertyGridProperty*> DetailsUI;

public:
	virtual BOOL InitInstance();

	// 구현입니다.
	BOOL  m_bHiColorIcons = TRUE;

	virtual void PreLoadState() override;
	virtual void LoadCustomState() override;
	virtual void SaveCustomState() override;

	// https://learn.microsoft.com/ko-kr/cpp/mfc/idle-loop-processing?view=msvc-170
	virtual BOOL OnIdle(LONG lCount) override;
	virtual void OnClosingMainFrame(CFrameImpl* pFrameImpl) override;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCApplication theApp;
#endif