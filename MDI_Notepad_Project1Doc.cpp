#include "pch.h"
#include "framework.h"
#include "MDI_Notepad_Project1.h"
#include "MDI_Notepad_Project1Doc.h"
#include <propkey.h>
#include <regex>
#include "FileRAII.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMDINotepadProject1Doc, CDocument)
BEGIN_MESSAGE_MAP(CMDINotepadProject1Doc, CDocument)
END_MESSAGE_MAP()

CMDINotepadProject1Doc::CMDINotepadProject1Doc() noexcept {}
CMDINotepadProject1Doc::~CMDINotepadProject1Doc() {}

BOOL CMDINotepadProject1Doc::OnNewDocument()
{
    if (!CDocument::OnNewDocument()) return FALSE;
    m_strContent.Empty();
    while (!m_undoStack.empty()) m_undoStack.pop();
    while (!m_redoStack.empty()) m_redoStack.pop();
    return TRUE;
}

void CMDINotepadProject1Doc::SaveState()
{
    if (m_undoStack.empty() || m_undoStack.top() != m_strContent) {
        m_undoStack.push(m_strContent);
        if (m_undoStack.size() > 50) m_undoStack.pop();
        while (!m_redoStack.empty()) m_redoStack.pop();
    }
}

void CMDINotepadProject1Doc::Undo()
{
    if (!m_undoStack.empty()) {
        m_redoStack.push(m_strContent);
        m_strContent = m_undoStack.top();
        m_undoStack.pop();
        UpdateAllViews(NULL);
    }
}

void CMDINotepadProject1Doc::Redo()
{
    if (!m_redoStack.empty()) {
        m_undoStack.push(m_strContent);
        m_strContent = m_redoStack.top();
        m_redoStack.pop();
        UpdateAllViews(NULL);
    }
}

void CMDINotepadProject1Doc::ReplaceText(CString strRegex, CString strReplace)
{
    // 1. 先保存当前状态，方便撤销
    SaveState();

    std::wstring content = (LPCTSTR)m_strContent;
    try {
        std::wregex re((LPCTSTR)strRegex);
        // 2. 执行正则替换
        std::wstring result = std::regex_replace(content, re, (LPCTSTR)strReplace);
        m_strContent = result.c_str();

        // 3. 刷新所有视图显示新内容
        UpdateAllViews(NULL);
        AfxMessageBox(_T("替换操作已完成"));
    }
    catch (...) {
        AfxMessageBox(_T("正则语法错误，请检查输入"));
    }
}

void CMDINotepadProject1Doc::Serialize(CArchive& ar)
{
    try {
        if (ar.IsStoring()) {
            // 保存逻辑
            CString strPathName = ar.GetFile()->GetFilePath();
            if (strPathName.Right(4).MakeLower() == _T(".txt")) {
                ar.WriteString(m_strContent);
            }
            else {
                // 自定义加密格式存储
                ar << CString(_T("20250313001Z")) << m_strContent << CString(_T("Key123")) << CString(_T("IV99"));
            }
        }
        else {
            // 读取逻辑
            CString strPathName = ar.GetFile()->GetFilePath();
            if (strPathName.Right(4).MakeLower() == _T(".txt")) {
                m_strContent.Empty();
                CString line;
                while (ar.ReadString(line)) {
                    m_strContent += line + _T("\r\n");
                }
            }
            else {
                // 读取自定义格式
                CString id, key, iv;
                ar >> id >> m_strContent >> key >> iv;
            }
        }
    }
    // 捕获文件相关的异常
    catch (CFileException* e) {
        TCHAR szErr[512];
        e->GetErrorMessage(szErr, 512);
        AfxMessageBox(_T("磁盘操作错误: ") + CString(szErr), MB_ICONERROR);
        e->Delete();
    }
    // 捕获存档格式异常（如打开了损坏的文件）
    catch (CArchiveException* e) {
        AfxMessageBox(_T("文档数据格式非法或已损坏！"), MB_ICONERROR);
        e->Delete();
    }
    catch (...) {
        AfxMessageBox(_T("发生未知错误！"), MB_ICONERROR);
    }
}

void CMDINotepadProject1Doc::SetTitle(LPCTSTR lpszTitle)
{
    CDocument::SetTitle(lpszTitle);
    if (AfxGetMainWnd()) AfxGetMainWnd()->SetWindowText(_T("我的编辑器 - ") + GetTitle());
}

#ifdef _DEBUG
void CMDINotepadProject1Doc::AssertValid() const { CDocument::AssertValid(); }
void CMDINotepadProject1Doc::Dump(CDumpContext& dc) const { CDocument::Dump(dc); }
#endif