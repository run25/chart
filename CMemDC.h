class CMemDC : public CDC {
private:
    CBitmap m_bitmap;
    CBitmap* m_oldBitmap;
    CDC* m_pDC;
    CRect m_rect;
    BOOL m_bMemDC;

public:
    CMemDC(CDC* pDC, const CRect* pRect = nullptr) : CDC(), m_oldBitmap(nullptr), m_pDC(pDC) {
        m_bMemDC = !pDC->IsPrinting();

        if (m_bMemDC) {
            pDC->GetClipBox(&m_rect);
            if (pRect) m_rect = *pRect;

            CreateCompatibleDC(pDC);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_oldBitmap = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);
        } else {
            m_pDC = pDC;
        }
    }

    ~CMemDC() {
        if (m_bMemDC) {
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), this, m_rect.left, m_rect.top, SRCCOPY);
            SelectObject(m_oldBitmap);
        }
    }

    CMemDC* operator->() { return this; }
    operator CMemDC*() { return this; }

void AlphaBlendFillRect(CDC* pDestDC, const CRect& rcDest, BYTE alpha, COLORREF fillColor)
{
    CBrush brush(fillColor);
    FillRect(CRect(0, 0, rcDest.Width(), rcDest.Height()), &brush);

    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = alpha;
    bf.AlphaFormat = 0;

    pDestDC->AlphaBlend(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
                        this, 0, 0, rcDest.Width(), rcDest.Height(), bf);
}
};
