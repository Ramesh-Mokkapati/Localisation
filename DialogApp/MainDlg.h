#pragma once
#include "resource.h"

class CMainDlg : public CDialog
{
public:
    explicit CMainDlg(CWnd* pParent = nullptr);
    ~CMainDlg();

    enum { IDD = IDD_DIALOG_MAIN };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    afx_msg void OnRadioEnglish();
    afx_msg void OnRadioFrench();
    afx_msg void OnRadioGerman();
    afx_msg void OnBnClickedOk();

    DECLARE_MESSAGE_MAP()

private:
    void SwitchToEnglish();
    void SwitchToFrench();
    void SwitchToGerman();
    void UpdateStringControls();

    HMODULE m_hLangDLL;       // NULL = English (built-in), non-NULL = FrenchDLL.dll
    bool    m_bInitializing;  // true during OnInitDialog to suppress radio handlers

    CButton m_radioEnglish;
    CButton m_radioFrench;
    CButton m_radioGerman;
    CStatic m_staticHello;
    CStatic m_staticGreeting;
    CStatic m_staticLanguage;
    CStatic m_staticGoodbye;
    CStatic m_staticInstruction;
    CStatic m_staticStatus;
};
