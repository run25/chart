#pragma once

class CDualHandleSlider : public CWnd
{
public:
    CDualHandleSlider();
    virtual ~CDualHandleSlider();

    BOOL Create(const CRect& rect, CWnd* pParent, UINT nID);
    void SetRange(int min, int max);
    void GetValues(int& start, int& stop);

protected:
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT, CPoint);
    afx_msg void OnLButtonUp(UINT, CPoint);
    afx_msg void OnMouseMove(UINT, CPoint);
    DECLARE_MESSAGE_MAP()

private:
    int m_min, m_max;
    int m_startVal, m_stopVal;
    bool m_dragStart, m_dragStop;

    CRect m_sliderRect;

    void DrawSlider(CDC* pDC);
    void DrawHandle(CDC* pDC, const CPoint& center, COLORREF color);
    int ValueToPosition(int val);
    int PositionToValue(int x);
    CString FormatTime(int val);
};
