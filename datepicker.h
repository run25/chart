#pragma once

enum DialMode
{
    MODE_HOUR,
    MODE_MINUTE
};

class CTimeDialSelector : public CWnd
{
public:
    int m_nHour = 11;
    int m_nMinute = 35;
    DialMode m_mode = MODE_MINUTE;

    BOOL Create(CWnd* pParent, UINT nID, CRect rect);
    void SetTime(int hour, int minute);
    CString GetFormattedTime() const;

protected:
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()

private:
    CRect m_rect;
    CFont m_font;
    void DrawDial(CDC* pDC);
    void DrawHeader(CDC* pDC);
    void HandleLabelClick(CPoint point);
    void HandleDialClick(CPoint point);
};
