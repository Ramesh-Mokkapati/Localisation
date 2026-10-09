#pragma once

#include "resource.h"

class CSDIAppApp : public CWinApp
{
public:
    enum Language { LangEnglish, LangFrench, LangGerman };

    CSDIAppApp();

    HMODULE  m_hLangDLL;   // handle to the currently loaded language DLL (NULL = English)
    Language m_curLang;    // which language is active — drives the update-UI radio bullets

    virtual BOOL InitInstance();
    virtual int  ExitInstance();

    afx_msg void OnLanguageEnglish();
    afx_msg void OnLanguageFrench();
    afx_msg void OnLanguageGerman();
    afx_msg void OnUpdateLanguageEnglish(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLanguageFrench(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLanguageGerman(CCmdUI* pCmdUI);
    afx_msg void OnAppAbout();

    DECLARE_MESSAGE_MAP()

private:
    void SwitchLanguage(HMODULE hNewDLL, Language lang);
    void RefreshAllViews();
};

extern CSDIAppApp theApp;
