#include "pch.h"
#include "DialogApp.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
    ON_BN_CLICKED(IDC_RADIO_ENGLISH, &CMainDlg::OnRadioEnglish)
    ON_BN_CLICKED(IDC_RADIO_FRENCH,  &CMainDlg::OnRadioFrench)
    ON_BN_CLICKED(IDC_RADIO_GERMAN,  &CMainDlg::OnRadioGerman)
    ON_BN_CLICKED(IDOK,              &CMainDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// Shared registry path – all three apps (CommandLine, SDIApp, DialogApp)
// read and write this same value so a language change in one is picked up
// by the others on their next launch.
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

CMainDlg::CMainDlg(CWnd* pParent)
    : CDialog(IDD_DIALOG_MAIN, pParent)
    , m_hLangDLL(NULL)
    , m_bInitializing(false)
{
}

CMainDlg::~CMainDlg()
{
    // Restore resource handle before freeing the DLL
    if (m_hLangDLL)
    {
        AfxSetResourceHandle(AfxGetApp()->m_hInstance);
        FreeLibrary(m_hLangDLL);
        m_hLangDLL = NULL;
    }
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RADIO_ENGLISH,       m_radioEnglish);
    DDX_Control(pDX, IDC_RADIO_FRENCH,        m_radioFrench);
    DDX_Control(pDX, IDC_RADIO_GERMAN,        m_radioGerman);
    DDX_Control(pDX, IDC_STATIC_HELLO,        m_staticHello);
    DDX_Control(pDX, IDC_STATIC_GREETING,     m_staticGreeting);
    DDX_Control(pDX, IDC_STATIC_LANGUAGE,     m_staticLanguage);
    DDX_Control(pDX, IDC_STATIC_GOODBYE,      m_staticGoodbye);
    DDX_Control(pDX, IDC_STATIC_INSTRUCTION,  m_staticInstruction);
    DDX_Control(pDX, IDC_STATIC_STATUS,       m_staticStatus);
}

BOOL CMainDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Guard: prevent OnRadioEnglish / OnRadioFrench from acting while we
    // programmatically set the radio check state below.
    m_bInitializing = true;

    CString strSaved = ReadSharedLangPref();
    const TCHAR* dllName = (strSaved == _T("French")) ? _T("FrenchDLL.dll")
                         : (strSaved == _T("German")) ? _T("GermanDLL.dll")
                         : nullptr;

    if (dllName)
    {
        HMODULE h = LoadLibrary(dllName);
        if (h)
        {
            m_hLangDLL = h;
            AfxSetResourceHandle(m_hLangDLL);
        }
        else
            strSaved = _T("English");   // fallback
    }

    m_radioEnglish.SetCheck(strSaved == _T("English") ? BST_CHECKED : BST_UNCHECKED);
    m_radioFrench.SetCheck (strSaved == _T("French")  ? BST_CHECKED : BST_UNCHECKED);
    m_radioGerman.SetCheck (strSaved == _T("German")  ? BST_CHECKED : BST_UNCHECKED);

    m_bInitializing = false;

    UpdateStringControls();
    return TRUE;
}

// ---------------------------------------------------------------------------
// Language switching
// ---------------------------------------------------------------------------

void CMainDlg::UpdateStringControls()
{
    // LoadString transparently reads from whichever module handle
    // AfxSetResourceHandle() currently points to.
    CString s;

    s.LoadString(IDS_HELLO);        m_staticHello.SetWindowText(s);
    s.LoadString(IDS_GREETING);     m_staticGreeting.SetWindowText(s);
    s.LoadString(IDS_GOODBYE);      m_staticGoodbye.SetWindowText(s);
    s.LoadString(IDS_INSTRUCTION);  m_staticInstruction.SetWindowText(s);

    // IDS_LANGUAGE doubles as the visible language indicator
    // ("Language: English (Default)"  or  "Langue : Français")
    s.LoadString(IDS_LANGUAGE);
    m_staticLanguage.SetWindowText(s);
    m_staticStatus.SetWindowText(s);

    // Update dialog title bar
    CString title;
    title.Format(_T("Localisation Demo  \x2014  %s"), (LPCTSTR)s);
    SetWindowText(title);

    // Translate the Exit button
    CString sBtnExit;
    sBtnExit.LoadString(IDS_BTN_EXIT);
    CWnd* pBtn = GetDlgItem(IDOK);
    if (pBtn) pBtn->SetWindowText(sBtnExit);
}

// Helper: release the current DLL and reset resource handle to app module.
void CMainDlg::SwitchToEnglish()
{
    if (!m_hLangDLL) return;
    AfxSetResourceHandle(AfxGetApp()->m_hInstance);
    FreeLibrary(m_hLangDLL);
    m_hLangDLL = NULL;
    WriteSharedLangPref(_T("English"));
    UpdateStringControls();
}

// Helper: load a language DLL, update the resource handle, save preference.
static bool LoadLangDLL(const TCHAR* dllName, HMODULE& hOut, CWnd* pMsgParent)
{
    HMODULE h = LoadLibrary(dllName);
    if (!h)
    {
        CString msg;
        msg.Format(_T("Cannot load %s\n\nWin32 error: %lu"), dllName, GetLastError());
        MessageBox(pMsgParent ? pMsgParent->GetSafeHwnd() : NULL,
                   msg, _T("Error"), MB_ICONERROR);
        return false;
    }
    hOut = h;
    AfxSetResourceHandle(h);
    return true;
}

void CMainDlg::SwitchToFrench()
{
    if (m_hLangDLL) { SwitchToEnglish(); }  // release current DLL first
    if (!LoadLangDLL(_T("FrenchDLL.dll"), m_hLangDLL, this))
    {
        m_radioEnglish.SetCheck(BST_CHECKED);
        m_radioFrench.SetCheck(BST_UNCHECKED);
        m_radioGerman.SetCheck(BST_UNCHECKED);
        return;
    }
    WriteSharedLangPref(_T("French"));
    UpdateStringControls();
}

void CMainDlg::SwitchToGerman()
{
    if (m_hLangDLL) { SwitchToEnglish(); }  // release current DLL first
    if (!LoadLangDLL(_T("GermanDLL.dll"), m_hLangDLL, this))
    {
        m_radioEnglish.SetCheck(BST_CHECKED);
        m_radioFrench.SetCheck(BST_UNCHECKED);
        m_radioGerman.SetCheck(BST_UNCHECKED);
        return;
    }
    WriteSharedLangPref(_T("German"));
    UpdateStringControls();
}

// ---------------------------------------------------------------------------
// Message handlers
// ---------------------------------------------------------------------------

void CMainDlg::OnRadioEnglish()
{
    if (!m_bInitializing) SwitchToEnglish();
}

void CMainDlg::OnRadioFrench()
{
    if (!m_bInitializing) SwitchToFrench();
}

void CMainDlg::OnRadioGerman()
{
    if (!m_bInitializing) SwitchToGerman();
}

void CMainDlg::OnBnClickedOk()
{
    CDialog::OnOK();
}
