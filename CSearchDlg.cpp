// CSearchDlg.cpp: 实现文件
//

#include "pch.h"
#include "MDI_Notepad_Project1.h"
#include "afxdialogex.h"
#include "CSearchDlg.h"
#include "resource.h"
#include "framework.h"
// CSearchDlg 对话框

IMPLEMENT_DYNAMIC(CSearchDlg, CDialogEx)

CSearchDlg::CSearchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    // 关键：关联控件 ID 和变量
    DDX_Text(pDX, IDC_EDIT_SEARCH_TEXT, m_strSearchPattern);
}

BEGIN_MESSAGE_MAP(CSearchDlg, CDialogEx)
END_MESSAGE_MAP()


// CSearchDlg 消息处理程序
