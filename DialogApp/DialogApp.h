#pragma once
#include "resource.h"

class CDialogAppApp : public CWinApp
{
public:
    CDialogAppApp();
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CDialogAppApp theApp;
