#pragma once

class CMainFrame : public CFrameWnd
{
protected:
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void GetMessageString(UINT nID, CString& rMessage) const;
    virtual ~CMainFrame();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CStatusBar  m_wndStatusBar;

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
};
