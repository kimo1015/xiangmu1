#include "pch.h"
#include "framework.h"
#include "MDI_Notepad_Project1.h"
#include "CSearchDlg.h"
#include "MDI_Notepad_Project1Doc.h"
#include "MDI_Notepad_Project1View.h"
#include <regex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMDINotepadProject1View, CView)



BEGIN_MESSAGE_MAP(CMDINotepadProject1View, CView)
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo) // 对应加速键表里的 ID_EDIT_UNDO
    ON_COMMAND(ID_EDIT_REDO, OnEditRedo) // 对应加速键表里的 ID_EDIT_REDO
END_MESSAGE_MAP()

CMDINotepadProject1View::CMDINotepadProject1View() noexcept
{
    m_bDarkMode = FALSE; // 初始化为亮色模式
}

CMDINotepadProject1View::~CMDINotepadProject1View() {}

// --- 核心绘制逻辑：处理行号、主题颜色、光标位置 ---
void CMDINotepadProject1View::OnDraw(CDC* pDC)
{
    CMDINotepadProject1Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc) return;

    CRect rect;
    GetClientRect(&rect);

    // 1. 根据主题设置颜色
    COLORREF clrBg = m_bDarkMode ? RGB(30, 30, 30) : RGB(255, 255, 255);
    COLORREF clrText = m_bDarkMode ? RGB(200, 200, 200) : RGB(0, 0, 0);
    COLORREF clrLineNumBg = m_bDarkMode ? RGB(50, 50, 50) : RGB(240, 240, 240);

    pDC->FillSolidRect(&rect, clrBg);

    // 2. 绘制行号区背景
    int nMargin = 50;
    pDC->FillSolidRect(0, 0, nMargin - 5, rect.Height(), clrLineNumBg);

    // 3. 统计行数并绘制行号
    pDC->SetBkMode(TRANSPARENT);
    int nLineHeight = 20;
    int lineCount = 1;
    for (int i = 0; i < pDoc->m_strContent.GetLength(); i++) {
        if (pDoc->m_strContent[i] == _T('\n')) lineCount++;
    }

    pDC->SetTextColor(RGB(128, 128, 128));
    for (int j = 1; j <= lineCount; j++) {
        CString strLine;
        strLine.Format(_T("%d"), j);
        pDC->TextOut(10, 10 + (j - 1) * nLineHeight, strLine);
    }
    // 在 OnDraw 绘制正文之前插入
    if (m_nHighlightLine != -1) {
        // 获取匹配行之前的完整文本以计算 X 偏移
        int curLine = 0;
        int lineStart = 0;
        for (int i = 0; i < pDoc->m_strContent.GetLength() && curLine < m_nHighlightLine; i++) {
            if (pDoc->m_strContent[i] == _T('\n')) {
                curLine++;
                lineStart = i + 1;
            }
        }

        CString strLineBefore = pDoc->m_strContent.Mid(lineStart, m_nHighlightStartInLine);
        CString strMatch = pDoc->m_strContent.Mid(lineStart + m_nHighlightStartInLine, m_nHighlightLen);

        CSize sizeBefore = pDC->GetTextExtent(strLineBefore);
        CSize sizeMatch = pDC->GetTextExtent(strMatch);

        CRect highlightRect;
        highlightRect.left = nMargin + sizeBefore.cx;
        // 根据行号计算 Y 坐标
        highlightRect.top = 10 + (m_nHighlightLine * nLineHeight);
        highlightRect.right = highlightRect.left + sizeMatch.cx;
        highlightRect.bottom = highlightRect.top + nLineHeight;

        pDC->FillSolidRect(&highlightRect, RGB(255, 255, 0));
    }
    // 4. 绘制正文
    pDC->SetTextColor(clrText);
    CRect textRect = rect;
    textRect.left += nMargin;
    textRect.top += 10;
    pDC->DrawText(pDoc->m_strContent, &textRect, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX | DT_EDITCONTROL);

    // 5. 动态计算光标 (Caret) 位置
    int lastNewLine = pDoc->m_strContent.ReverseFind(_T('\n'));
    CString lastLine = (lastNewLine == -1) ? pDoc->m_strContent : pDoc->m_strContent.Mid(lastNewLine + 1);
    CSize lastLineSize = pDC->GetTextExtent(lastLine);

    int caretX = nMargin + lastLineSize.cx;
    int caretY = 10 + (lineCount - 1) * nLineHeight;
    SetCaretPos(CPoint(caretX, caretY));

}

// --- 处理换行和字符输入 ---
void CMDINotepadProject1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CMDINotepadProject1Doc* pDoc = GetDocument();

    // 撤销/重做时不保存状态
    if (nChar != 26 && nChar != 25) pDoc->SaveState();

    if (nChar == VK_BACK) { // 退格
        if (!pDoc->m_strContent.IsEmpty())
            pDoc->m_strContent.Delete(pDoc->m_strContent.GetLength() - 1);
    }
    else if (nChar == VK_RETURN) { // 核心：处理换行输入
        pDoc->m_strContent += _T("\r\n");
    }
    else if (nChar >= 32) { // 普通字符
        pDoc->m_strContent += (TCHAR)nChar;
    }

    Invalidate(); // 重新触发 OnDraw 绘图
    pDoc->SetModifiedFlag();
}

// --- 处理主题切换快捷键 (T) 和 搜索 (Ctrl+F) ---
void CMDINotepadProject1View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CMDINotepadProject1Doc* pDoc = GetDocument();
    BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);

    if (bCtrl && nChar == 'F') { // 搜索
        CSearchDlg dlg;
        if (dlg.DoModal() == IDOK) SearchText(dlg.m_strSearchPattern);
    }
    else if (nChar == 'T') { // 核心：按下 'T' 键切换暗亮主题
        m_bDarkMode = !m_bDarkMode;
        Invalidate();
    }
    else if (bCtrl && nChar == 'R') { // Ctrl + R 触发替换验证
        // 自动将文中所有的数字替换为 [OK]
        GetDocument()->ReplaceText(_T("\\d+"), _T("[OK]"));
        return;
    }
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMDINotepadProject1View::SearchText(CString pattern)
{
    CMDINotepadProject1Doc* pDoc = GetDocument();
    std::wstring content = (LPCTSTR)pDoc->m_strContent;

    try {
        std::wregex r((LPCTSTR)pattern);
        std::wsmatch m;
        if (std::regex_search(content, m, r)) {
            int globalPos = (int)m.position();
            m_nHighlightLen = (int)m.length();

            // 计算该位置对应的行号和行内偏移
            int curLine = 0;
            int lastNewLinePos = -1;
            for (int i = 0; i < globalPos; i++) {
                if (pDoc->m_strContent[i] == _T('\n')) {
                    curLine++;
                    lastNewLinePos = i;
                }
            }
            m_nHighlightLine = curLine;
            m_nHighlightStartInLine = globalPos - (lastNewLinePos + 1);

            Invalidate();
            AfxMessageBox(_T("找到匹配项，已在文中突出显示"));
        }
        else {
            m_nHighlightLine = -1;
            Invalidate();
            AfxMessageBox(_T("未找到匹配内容"));
        }
    }
    catch (...) { AfxMessageBox(_T("正则表达式语法错误")); }
}

void CMDINotepadProject1View::OnEditUndo()
{
    CMDINotepadProject1Doc* pDoc = GetDocument();
    if (pDoc) {
        pDoc->Undo();
        Invalidate(); // 撤销后必须重新绘图，否则界面不显示变化
    }
}

void CMDINotepadProject1View::OnEditRedo()
{
    CMDINotepadProject1Doc* pDoc = GetDocument();
    if (pDoc) {
        pDoc->Redo();
        Invalidate(); // 重做后必须重新绘图
    }
}
// --- 补全光标和虚函数，解决链接错误 ---
void CMDINotepadProject1View::OnSetFocus(CWnd* pOldWnd) {
    CView::OnSetFocus(pOldWnd);
    CreateSolidCaret(2, 20);
    ShowCaret();
}
void CMDINotepadProject1View::OnKillFocus(CWnd* pNewWnd) {
    CView::OnKillFocus(pNewWnd);
    HideCaret();
    ::DestroyCaret();
}
BOOL CMDINotepadProject1View::PreCreateWindow(CREATESTRUCT& cs) { return CView::PreCreateWindow(cs); }
BOOL CMDINotepadProject1View::OnPreparePrinting(CPrintInfo* pInfo) { return DoPreparePrinting(pInfo); }
void CMDINotepadProject1View::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) {}
void CMDINotepadProject1View::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) {}

#ifdef _DEBUG
void CMDINotepadProject1View::AssertValid() const { CView::AssertValid(); }
void CMDINotepadProject1View::Dump(CDumpContext& dc) const { CView::Dump(dc); }
CMDINotepadProject1Doc* CMDINotepadProject1View::GetDocument() const {
    return (CMDINotepadProject1Doc*)m_pDocument;
}
#endif