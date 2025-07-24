#include "pch.h"
#include "CDualHandleSlider.h"

CDualHandleSlider::CDualHandleSlider()
    : m_min(0), m_max(290), m_startVal(60), m_stopVal(240),
      m_dragStart(false), m_dragStop(false)
{
}

CDualHandleSlider::~CDualHandleSlider() {}

BEGIN_MESSAGE_MAP(CDualHandleSlider, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CDualHandleSlider::Create(const CRect& rect, CWnd* pParent, UINT nID)
{
    return CWnd::Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, rect, pParent, nID);
}

void CDualHandleSlider::SetRange(int min, int max)
{
    m_min = min;
    m_max = max;
}

void CDualHandleSlider::GetValues(int& start, int& stop)
{
    start = m_startVal;
    stop = m_stopVal;
}

void CDualHandleSlider::OnPaint()
{
    CPaintDC dc(this);
    GetClientRect(&m_sliderRect);
    m_sliderRect.DeflateRect(30, 50);

    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(30, 30, 30));

    CString strStart = (m_startVal == 0) ? "Start: OFF" : "Start: " + FormatTime(m_startVal);
    CString strStop  = (m_stopVal == 290) ? "Stop: OFF" : "Stop: " + FormatTime(m_stopVal);

    CFont font;
    font.CreatePointFont(100, "Segoe UI");
    CFont* oldFont = dc.SelectObject(&font);

    dc.TextOutW(m_sliderRect.left, m_sliderRect.top - 35, strStart);
    dc.TextOutW(m_sliderRect.right - 150, m_sliderRect.top - 35, strStop);

    DrawSlider(&dc);

    dc.SelectObject(oldFont);
}

void CDualHandleSlider::DrawSlider(CDC* pDC)
{
    const int radius = 10;
    CRect track = m_sliderRect;
    track.top += 15;
    track.bottom -= 15;

    pDC->FillSolidRect(track, RGB(230, 230, 230));
    pDC->RoundRect(track, CPoint(radius, radius));

    int xStart = ValueToPosition(m_startVal);
    int xStop = ValueToPosition(m_stopVal);

    CRect fill;
    if (m_startVal < m_stopVal)
        fill.SetRect(xStart, track.top, xStop, track.bottom);
    else {
        pDC->FillSolidRect(CRect(track.left, track.top, xStop, track.bottom), RGB(200, 200, 255));
        pDC->FillSolidRect(CRect(xStart, track.top, track.right, track.bottom), RGB(200, 200, 255));
        fill.SetRectEmpty();
    }
    if (!fill.IsRectEmpty())
        pDC->FillSolidRect(fill, RGB(100, 160, 255));

    CPoint ptStart(xStart, (track.top + track.bottom) / 2);
    CPoint ptStop(xStop, (track.top + track.bottom) / 2);
    DrawHandle(pDC, ptStart, RGB(0, 122, 255));
    DrawHandle(pDC, ptStop, RGB(0, 122, 255));

    CFont labelFont;
    labelFont.CreatePointFont(90, "Segoe UI");
    CFont* oldFont = pDC->SelectObject(&labelFont);
    pDC->SetTextColor(RGB(130, 130, 130));

    for (int h = 0; h <= 23; ++h)
    {
        int val = h * 12;
        int x = ValueToPosition(val);
        CString label;
        label.Format("%02d", h);
        CSize sz = pDC->GetTextExtent(label);
        pDC->TextOutW(x - sz.cx / 2, track.bottom + 10, label);
    }

    pDC->SelectObject(oldFont);
}

void CDualHandleSlider::DrawHandle(CDC* pDC, const CPoint& center, COLORREF color)
{
    const int r = 10;
    CBrush whiteBrush(RGB(255, 255, 255));
    CPen pen(PS_SOLID, 2, color);

    CBrush* oldBrush = pDC->SelectObject(&whiteBrush);
    CPen* oldPen = pDC->SelectObject(&pen);

    pDC->Ellipse(center.x - r, center.y - r, center.x + r, center.y + r);

    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldPen);
}

int CDualHandleSlider::ValueToPosition(int val)
{
    double ratio = (double)(val - m_min) / (m_max - m_min);
    return m_sliderRect.left + (int)(ratio * m_sliderRect.Width());
}

int CDualHandleSlider::PositionToValue(int x)
{
    x = max(m_sliderRect.left, min(x, m_sliderRect.right));
    double ratio = (double)(x - m_sliderRect.left) / m_sliderRect.Width();
    return m_min + (int)(ratio * (m_max - m_min));
}

CString CDualHandleSlider::FormatTime(int val)
{
    int mins = val * 5;
    int h = mins / 60;
    int m = mins % 60;

    CString str;
    str.Format("%02d:%02d", h, m);
    return str;
}

void CDualHandleSlider::OnLButtonDown(UINT, CPoint pt)
{
    int xStart = ValueToPosition(m_startVal);
    int xStop  = ValueToPosition(m_stopVal);

    if (abs(pt.x - xStart) < 12) m_dragStart = true;
    else if (abs(pt.x - xStop) < 12) m_dragStop = true;

    SetCapture();
}

void CDualHandleSlider::OnLButtonUp(UINT, CPoint)
{
    m_dragStart = m_dragStop = false;
    ReleaseCapture();
}

void CDualHandleSlider::OnMouseMove(UINT, CPoint pt)
{
    if (!m_dragStart && !m_dragStop) return;

    int val = PositionToValue(pt.x);
    val = max(m_min, min(m_max, val));

    if (m_dragStart && val < m_max)
        m_startVal = val;
    else if (m_dragStop && val > m_min)
        m_stopVal = val;

    Invalidate();
}
