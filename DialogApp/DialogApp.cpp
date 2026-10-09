#include "pch.h"
#include "DialogApp.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDialogAppApp, CWinApp)
END_MESSAGE_MAP()

CDialogAppApp theApp;

CDialogAppApp::CDialogAppApp() {}

BOOL CDialogAppApp::InitInstance()
{
    CWinApp::InitInstance();
    SetRegistryKey(_T("LocalisationDemo"));

    CMainDlg dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();

    // Return FALSE so MFC exits when the dialog closes
    return FALSE;
}
