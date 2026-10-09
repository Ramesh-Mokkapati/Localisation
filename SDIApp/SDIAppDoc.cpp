#include "pch.h"
#include "SDIApp.h"
#include "SDIAppDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSDIAppDoc, CDocument)

BEGIN_MESSAGE_MAP(CSDIAppDoc, CDocument)
END_MESSAGE_MAP()

CSDIAppDoc::CSDIAppDoc() {}
CSDIAppDoc::~CSDIAppDoc() {}

BOOL CSDIAppDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    return TRUE;
}

void CSDIAppDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring()) { }
    else               { }
}

#ifdef _DEBUG
void CSDIAppDoc::AssertValid() const { CDocument::AssertValid(); }
void CSDIAppDoc::Dump(CDumpContext& dc) const { CDocument::Dump(dc); }
#endif
