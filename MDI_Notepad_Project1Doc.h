#pragma once
#include <stack> // 必须包含，用于撤销功能
class CMDINotepadProject1Doc : public CDocument

{
protected:
    CMDINotepadProject1Doc() noexcept;
    DECLARE_DYNCREATE(CMDINotepadProject1Doc)

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    virtual void SetTitle(LPCTSTR lpszTitle); // 修复 C2509 错误

    // --- 核心数据 ---
    CString m_strContent;             // 正文内容
    std::stack<CString> m_undoStack;  // 撤销栈
    std::stack<CString> m_redoStack;  // 重做栈
    CString m_strLastContent;         // 备份内容

    // --- 成员函数声明 ---
    void SaveState();
    void Undo();
    void Redo();
    void ReplaceText(CString strRegex, CString strReplace); // 正则替换

    virtual ~CMDINotepadProject1Doc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()
};