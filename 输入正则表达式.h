#pragma once
#include "afxdialogex.h"


// 输入正则表达式 对话框

class 输入正则表达式 : public CDialogEx
{
	DECLARE_DYNAMIC(输入正则表达式)

public:
	输入正则表达式(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~输入正则表达式();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
