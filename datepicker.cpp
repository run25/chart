#include "pch.h"
#include "TimeDialSelector.h"
#include <math.h>

#define PI 3.1415926535

BEGIN_MESSAGE_MAP(CTimeDialSelector, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CTimeDialSelector::Create(CWnd* pParent, UINT nID, CRect rect)
{
    m_rect = rect;
    BOOL res = CWnd::Create(NULL, _T("TimeDial"), WS_CHILD | WS_VISIBLE, rect, pParent, nID);
    m_font.CreatePointFont(120, _T("Segoe UI"));
    return res;
}

void CTimeDialSelector::SetTime(int hour, int minute)
{
    m_nHour = hour % 24;
    m_nMinute = minute % 60;
    Invalidate();
}

CString CTimeDialSelector::GetFormattedTime() const
{
    CString str;
    str.Format(_T("%02d:%02d"), m_nHour, m_nMinute);
    return str;
}

void CTimeDialSelector::OnPaint()
{
    CPaintDC dc(this);
    CRect client;
    GetClientRect(&client);
    dc.FillSolidRect(client, RGB(255, 255, 255));
    dc.SelectObject(&m_font);

    DrawHeader(&dc);
    DrawDial(&dc);
}

void CTimeDialSelector::DrawHeader(CDC* pDC)
{
    CRect client;
    GetClientRect(&client);

    CString hourStr, minuteStr;
    hourStr.Format(_T("%02d"), m_nHour);
    minuteStr.Format(_T("%02d"), m_nMinute);

    int midY = client.top + 30;

    // 時ラベル
    CRect hourRect(client.left + 40, midY, client.left + 100, midY + 40);
    CRect minRect(client.left + 110, midY, client.left + 170, midY + 40);

    if (m_mode == MODE_HOUR)
        pDC->FillSolidRect(hourRect, RGB(0, 120, 215));
    else
        pDC->FillSolidRect(minRect, RGB(0, 120, 215));

    pDC->SetTextColor(RGB(255, 255, 255));
    pDC->DrawText(hourStr, hourRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    pDC->DrawText(_T(":"), CRect(hourRect.right, midY, hourRect.right + 10, midY + 40), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    pDC->DrawText(minuteStr, minRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void CTimeDialSelector::DrawDial(CDC* pDC)
{
    CRect client;
    GetClientRect(&client);
    int cx = client.Width() / 2;
    int cy = client.Height() / 2 + 40;
    int rOuter = 100;
    int rInner = 60;

    CBrush brushBlue(RGB(0, 120, 215));
    CFont* oldFont = pDC->SelectObject(&m_font);

    for (int i = 0; i < 12; ++i)
    {
        double angle = (i - 3) * 30.0 * PI / 180.0;
        int val;
        int x, y;

        if (m_mode == MODE_HOUR)
        {
            // 外円: 1〜12
            val = i + 1;
            x = cx + (int)(rOuter * cos(angle));
            y = cy + (int)(rOuter * sin(angle));

            if (val == m_nHour)
            {
                pDC->SelectStockObject(NULL_PEN);
                pDC->SelectObject(&brushBlue);
                pDC->Ellipse(x - 15, y - 15, x + 15, y + 15);
                pDC->SetTextColor(RGB(255, 255, 255));
            }
            else
                pDC->SetTextColor(RGB(0, 0, 0));

            CString str; str.Format(_T("%d"), val);
            pDC->DrawText(str, CRect(x - 15, y - 15, x + 15, y + 15), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // 内円: 13〜23, 0
            val = (i + 13) % 24;
            x = cx + (int)(rInner * cos(angle));
            y = cy + (int)(rInner * sin(angle));

            if (val == m_nHour)
            {
                pDC->SelectStockObject(NULL_PEN);
                pDC->SelectObject(&brushBlue);
                pDC->Ellipse(x - 15, y - 15, x + 15, y + 15);
                pDC->SetTextColor(RGB(255, 255, 255));
            }
            else
                pDC->SetTextColor(RGB(0, 0, 0));

            str.Format(_T("%02d"), val);
            pDC->DrawText(str, CRect(x - 15, y - 15, x + 15, y + 15), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        else
        {
            // 分: 0〜55（5刻み）
            val = i * 5;
            x = cx + (int)(rOuter * cos(angle));
            y = cy + (int)(rOuter * sin(angle));

            if (val == m_nMinute)
            {
                pDC->SelectStockObject(NULL_PEN);
                pDC->SelectObject(&brushBlue);
                pDC->Ellipse(x - 15, y - 15, x + 15, y + 15);
                pDC->SetTextColor(RGB(255, 255, 255));
            }
            else
                pDC->SetTextColor(RGB(0, 0, 0));

            CString str; str.Format(_T("%02d"), val);
            pDC->DrawText(str, CRect(x - 15, y - 15, x + 15, y + 15), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }

    pDC->SelectObject(oldFont);
}

void CTimeDialSelector::OnLButtonDown(UINT nFlags, CPoint point)
{
    HandleLabelClick(point);
    HandleDialClick(point);
    Invalidate();
}

void CTimeDialSelector::HandleLabelClick(CPoint point)
{
    CRect client;
    GetClientRect(&client);
    int midY = client.top + 30;

    CRect hourRect(client.left + 40, midY, client.left + 100, midY + 40);
    CRect minRect(client.left + 110, midY, client.left + 170, midY + 40);

    if (hourRect.PtInRect(point))
        m_mode = MODE_HOUR;
    else if (minRect.PtInRect(point))
        m_mode = MODE_MINUTE;
}

void CTimeDialSelector::HandleDialClick(CPoint point)
{
    CRect client;
    GetClientRect(&client);
    int cx = client.Width() / 2;
    int cy = client.Height() / 2 + 40;
    int dx = point.x - cx;
    int dy = point.y - cy;
    double dist = sqrt(dx * dx + dy * dy);
    double angle = atan2(dy, dx);
    if (angle < 0) angle += 2 * PI;

    int index = ((int)((angle * 180 / PI + 360 + 90) / 30)) % 12;

    if (m_mode == MODE_HOUR)
    {
        if (dist >= 80)        m_nHour = (index + 1);
        else if (dist >= 40)   m_nHour = (index + 13) % 24;
    }
    else
    {
        m_nMinute = (index * 5) % 60;
    }
}
