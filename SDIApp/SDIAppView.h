#pragma once

class CSDIAppDoc;

class CSDIAppView : public CView
{
protected:
    CSDIAppView();
    DECLARE_DYNCREATE(CSDIAppView)

public:
    CSDIAppDoc* GetDocument() const;

    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual ~CSDIAppView();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CSDIAppDoc* CSDIAppView::GetDocument() const
{
    return reinterpret_cast<CSDIAppDoc*>(m_pDocument);
}
#endif
