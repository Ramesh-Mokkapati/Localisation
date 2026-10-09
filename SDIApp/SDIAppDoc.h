#pragma once

class CSDIAppDoc : public CDocument
{
protected:
    CSDIAppDoc();
    DECLARE_DYNCREATE(CSDIAppDoc)

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    virtual ~CSDIAppDoc();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    DECLARE_MESSAGE_MAP()
};
