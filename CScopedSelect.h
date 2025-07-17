template <typename T>
class CScopedSelect
{
    CDC* m_pDC;
    T* m_pOld;

    // ★ ここが違い！コピーを禁止する
    CScopedSelect(const CScopedSelect&) = delete;
    CScopedSelect& operator=(const CScopedSelect&) = delete;

public:
    CScopedSelect(CDC* pDC, T* pNew)
    {
        m_pDC = pDC;
        m_pOld = (T*)pDC->SelectObject(pNew);
    }
    ~CScopedSelect()
    {
        if (m_pDC && m_pOld)
            m_pDC->SelectObject(m_pOld);
    }
};
