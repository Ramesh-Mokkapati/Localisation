#include "pch.h"
#include "SDIApp.h"
#include "SDIAppDoc.h"
#include "SDIAppView.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSDIAppView, CView)

BEGIN_MESSAGE_MAP(CSDIAppView, CView)
END_MESSAGE_MAP()

CSDIAppView::CSDIAppView() {}
CSDIAppView::~CSDIAppView() {}

BOOL CSDIAppView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void CSDIAppView::OnDraw(CDC* pDC)
{
    CSDIAppDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // Load strings from whichever resource handle is currently active.
    // AfxSetResourceHandle() in CSDIAppApp switches this between
    // the app's own module (English) and FrenchDLL.dll (French).
    CString strHello, strGreeting, strLanguage, strGoodbye, strInstruction;
    strHello.LoadString(IDS_HELLO);
    strGreeting.LoadString(IDS_GREETING);
    strLanguage.LoadString(IDS_LANGUAGE);
    strGoodbye.LoadString(IDS_GOODBYE);
    strInstruction.LoadString(IDS_INSTRUCTION);

    // Choose a readable font
    CFont font;
    font.CreatePointFont(120, _T("Segoe UI"));
    CFont* pOldFont = pDC->SelectObject(&font);

    int x = 30, y = 20, lineHeight = 30;

    auto DrawLine = [&](const CString& label, const CString& value)
    {
        CString line;
        line.Format(_T("%-14s  %s"), (LPCTSTR)label, (LPCTSTR)value);
        pDC->TextOut(x, y, line);
        y += lineHeight;
    };

    // Header
    CFont fontBold;
    fontBold.CreatePointFont(140, _T("Segoe UI"));
    pDC->SelectObject(&fontBold);
    pDC->TextOut(x, y, _T("SDIApp  \x2014  Localisation Demo"));
    y += lineHeight + 10;

    // Divider
    pDC->MoveTo(x, y);
    pDC->LineTo(x + 600, y);
    y += 14;

    pDC->SelectObject(&font);
    DrawLine(_T("Hello:"),       strHello);
    DrawLine(_T("Greeting:"),    strGreeting);
    DrawLine(_T("Language:"),    strLanguage);
    DrawLine(_T("Goodbye:"),     strGoodbye);

    y += 10;
    pDC->MoveTo(x, y);
    pDC->LineTo(x + 600, y);
    y += 14;

    // Instruction shown in a slightly smaller font
    CFont fontSmall;
    fontSmall.CreatePointFont(90, _T("Segoe UI"));
    pDC->SelectObject(&fontSmall);
    pDC->SetTextColor(RGB(80, 80, 80));
    pDC->TextOut(x, y, strInstruction);

    pDC->SelectObject(pOldFont);
}

#ifdef _DEBUG
void CSDIAppView::AssertValid() const { CView::AssertValid(); }
void CSDIAppView::Dump(CDumpContext& dc) const { CView::Dump(dc); }

CSDIAppDoc* CSDIAppView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSDIAppDoc)));
    return (CSDIAppDoc*)m_pDocument;
}
#endif
