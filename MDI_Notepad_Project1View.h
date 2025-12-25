// MDI_Notepad_Project1View.h: CMDINotepadProject1View 类的接口
//

#pragma once
#include <regex> // 引入 C++ 标准正则库

class CMDINotepadProject1View : public CView
{
protected: // 仅从序列化创建
	CMDINotepadProject1View() noexcept;
	DECLARE_DYNCREATE(CMDINotepadProject1View)


	// 特性
public:
	CMDINotepadProject1Doc* GetDocument() const;

	// F-04 主题切换标记
	BOOL m_bDarkMode;

	// 操作
public:
	// F-03 正则搜索功能声明
	void SearchText(CString strPattern);
	// 在 View.h 的 public 模块增加：
	int m_nHighlightStart = -1;
	int m_nHighlightLen = 0;
	// 在 CMDINotepadProject1View.h 的 public 模块增加
	int m_nHighlightLine = -1;  // 匹配项所在的行号（从0开始）
	int m_nHighlightStartInLine = -1; // 匹配项在当前行内的起始位置

	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 实现
public:
	virtual ~CMDINotepadProject1View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);    // 字符输入
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); // 快捷键
	afx_msg void OnSetFocus(CWnd* pOldWnd);                        // 获得焦点
	afx_msg void OnKillFocus(CWnd* pNewWnd);                       // 失去焦点
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // MDI_Notepad_Project1View.cpp 中的调试版本
inline CMDINotepadProject1Doc* CMDINotepadProject1View::GetDocument() const
{
	return reinterpret_cast<CMDINotepadProject1Doc*>(m_pDocument);
}
#endif