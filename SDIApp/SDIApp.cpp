#include "pch.h"
#include "SDIApp.h"
#include "MainFrm.h"
#include "SDIAppDoc.h"
#include "SDIAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSDIAppApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT,            &CSDIAppApp::OnAppAbout)
    ON_COMMAND(ID_LANGUAGE_ENGLISH,     &CSDIAppApp::OnLanguageEnglish)
    ON_COMMAND(ID_LANGUAGE_FRENCH,      &CSDIAppApp::OnLanguageFrench)
    ON_COMMAND(ID_LANGUAGE_GERMAN,      &CSDIAppApp::OnLanguageGerman)
    ON_UPDATE_COMMAND_UI(ID_LANGUAGE_ENGLISH, &CSDIAppApp::OnUpdateLanguageEnglish)
    ON_UPDATE_COMMAND_UI(ID_LANGUAGE_FRENCH,  &CSDIAppApp::OnUpdateLanguageFrench)
    ON_UPDATE_COMMAND_UI(ID_LANGUAGE_GERMAN,  &CSDIAppApp::OnUpdateLanguageGerman)
END_MESSAGE_MAP()

// Shared registry path – same value used by CommandLine and DialogApp so
// a language change in any one app is visible to the others on next launch.
static const TCHAR* k_SharedRegPath = _T("Software\\LocalisationDemo");
static const TCHAR* k_LangValue     = _T("Language");

static CString ReadSharedLangPref()
{
    CString result = _T("English");
    HKEY hKey = NULL;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, k_SharedRegPath, 0,
            KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        TCHAR buf[64] = {};
        DWORD cb   = sizeof(buf);
        DWORD type = REG_SZ;
        if (::RegQueryValueEx(hKey, k_LangValue, NULL, &type,
                reinterpret_cast<LPBYTE>(buf), &cb) == ERROR_SUCCESS)
            result = buf;
        ::RegCloseKey(hKey);
    }
    return result;
}

static void WriteSharedLangPref(const CString& value)
{
    HKEY hKey = NULL;
    if (::RegCreateKeyEx(HKEY_CURRENT_USER, k_SharedRegPath,
            0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        ::RegSetValueEx(hKey, k_LangValue, 0, REG_SZ,
            reinterpret_cast<const BYTE*>((LPCTSTR)value),
            static_cast<DWORD>((value.GetLength() + 1) * sizeof(TCHAR)));
        ::RegCloseKey(hKey);
    }
}

CSDIAppApp theApp;

CSDIAppApp::CSDIAppApp()
    : m_hLangDLL(NULL)
    , m_curLang(LangEnglish)
{
}

BOOL CSDIAppApp::InitInstance()
{
    CWinApp::InitInstance();

    SetRegistryKey(_T("LocalisationDemo"));

    // Register the single document template
    CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CSDIAppDoc),
        RUNTIME_CLASS(CMainFrame),
        RUNTIME_CLASS(CSDIAppView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);

    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    // Restore the language the user chose last time (shared with all apps)
    CString strSaved = ReadSharedLangPref();
    if      (strSaved == _T("French"))  OnLanguageFrench();
    else if (strSaved == _T("German"))  OnLanguageGerman();

    return TRUE;
}

int CSDIAppApp::ExitInstance()
{
    if (m_hLangDLL)
    {
        AfxSetResourceHandle(m_hInstance);   // restore before freeing
        FreeLibrary(m_hLangDLL);
        m_hLangDLL = NULL;
    }
    return CWinApp::ExitInstance();
}

// -----------------------------------------------------------------------
// Language switching
// -----------------------------------------------------------------------

void CSDIAppApp::SwitchLanguage(HMODULE hNewDLL, Language lang)
{
    if (m_hLangDLL)
    {
        AfxSetResourceHandle(m_hInstance);
        FreeLibrary(m_hLangDLL);
        m_hLangDLL = NULL;
    }

    if (hNewDLL)
    {
        m_hLangDLL = hNewDLL;
        AfxSetResourceHandle(m_hLangDLL);
    }

    m_curLang = lang;
    RefreshAllViews();
}

void CSDIAppApp::RefreshAllViews()
{
    // Cast to CFrameWnd* so we can update m_hMenuDefault after swapping the
    // menu.  CWnd::SetMenu is non-virtual and does NOT update m_hMenuDefault;
    // leaving it pointing to the destroyed old HMENU causes an assertion in
    // CFrameWnd::UpdateFrameMenu (winfrm.cpp) on the next language switch.
    CFrameWnd* pFrame = DYNAMIC_DOWNCAST(CFrameWnd, m_pMainWnd);
    if (pFrame && pFrame->GetSafeHwnd())
    {
        HMENU hOldMenu = ::GetMenu(pFrame->GetSafeHwnd());

        CMenu newMenu;
        if (newMenu.LoadMenu(IDR_MAINFRAME))
        {
            HMENU hNewMenu = newMenu.Detach();      // frame will own the HMENU
            ::SetMenu(pFrame->GetSafeHwnd(), hNewMenu);
            pFrame->m_hMenuDefault = hNewMenu;      // keep MFC's cached handle in sync
            if (hOldMenu && hOldMenu != hNewMenu)
                ::DestroyMenu(hOldMenu);
            pFrame->DrawMenuBar();
        }

        pFrame->Invalidate();
    }

    // Force every view to repaint its localised strings
    POSITION posT = GetFirstDocTemplatePosition();
    while (posT)
    {
        CDocTemplate* pTpl = GetNextDocTemplate(posT);
        POSITION posD = pTpl->GetFirstDocPosition();
        while (posD)
        {
            CDocument* pDoc = pTpl->GetNextDoc(posD);
            pDoc->UpdateAllViews(NULL);
        }
    }
}

void CSDIAppApp::OnLanguageEnglish()
{
    if (m_curLang == LangEnglish) return;
    SwitchLanguage(NULL, LangEnglish);
    WriteSharedLangPref(_T("English"));
}

void CSDIAppApp::OnLanguageFrench()
{
    if (m_curLang == LangFrench) return;
    HMODULE h = LoadLibrary(_T("FrenchDLL.dll"));
    if (!h)
    {
        CString msg;
        msg.Format(_T("Cannot load FrenchDLL.dll\n\nWin32 error: %lu"), GetLastError());
        AfxMessageBox(msg, MB_ICONERROR);
        return;
    }
    SwitchLanguage(h, LangFrench);
    WriteSharedLangPref(_T("French"));
}

void CSDIAppApp::OnLanguageGerman()
{
    if (m_curLang == LangGerman) return;
    HMODULE h = LoadLibrary(_T("GermanDLL.dll"));
    if (!h)
    {
        CString msg;
        msg.Format(_T("Cannot load GermanDLL.dll\n\nWin32 error: %lu"), GetLastError());
        AfxMessageBox(msg, MB_ICONERROR);
        return;
    }
    SwitchLanguage(h, LangGerman);
    WriteSharedLangPref(_T("German"));
}

void CSDIAppApp::OnUpdateLanguageEnglish(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(m_curLang == LangEnglish);
}

void CSDIAppApp::OnUpdateLanguageFrench(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(m_curLang == LangFrench);
}

void CSDIAppApp::OnUpdateLanguageGerman(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(m_curLang == LangGerman);
}

void CSDIAppApp::OnAppAbout()
{
    CString strTitle, strText;
    strTitle.LoadString(IDS_ABOUT_TITLE);
    strText.LoadString(IDS_ABOUT_TEXT);
    ::MessageBox(
        m_pMainWnd ? m_pMainWnd->GetSafeHwnd() : NULL,
        strText, strTitle, MB_ICONINFORMATION | MB_OK);
}
