#include "pch.h"
#include "SDIApp.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

CMainFrame::CMainFrame() {}
CMainFrame::~CMainFrame() {}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    cs.cx = 700;
    cs.cy = 400;
    return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;
    }

    return 0;
}

// When the resource handle points to a language DLL, MFC-internal strings
// (e.g. AFX_IDS_IDLEMESSAGE = 0xE001 "Ready") won't be found there.  Fall
// back to the app's own module so the status bar always displays correctly.
void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
    if (rMessage.LoadString(nID))
        return;

    HINSTANCE hSaved = AfxGetResourceHandle();
    AfxSetResourceHandle(AfxGetApp()->m_hInstance);
    if (!rMessage.LoadString(nID))
        CFrameWnd::GetMessageString(nID, rMessage);
    AfxSetResourceHandle(hSaved);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const { CFrameWnd::AssertValid(); }
void CMainFrame::Dump(CDumpContext& dc) const { CFrameWnd::Dump(dc); }
#endif
