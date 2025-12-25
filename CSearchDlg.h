#pragma once
#include "afxdialogex.h"


// CSearchDlg 对话框

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
public:
	// 背景：用户在对话框输入的正则表达式将存放在这里
	CString m_strSearchPattern;
	CSearchDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSearchDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
