// FileHelper.h
#pragma once

#include "pch.h"  // 必须放在所有代码之前
class CFileRAII {
private:
    CFile m_file;
public:
    // 构造时打开文件，失败则抛出异常
    CFileRAII(LPCTSTR path, UINT flags) {
        if (!m_file.Open(path, flags)) {
            // 对应功能 F-06：捕获异常弹出对话框 
            AfxMessageBox(_T("文件打开失败！"));
            throw CFileException();
        }
    }

    // 析构时自动调用 Close()，确保资源释放 [cite: 50, 101]
    ~CFileRAII() {
        m_file.Close();
    }

    CFile& Get() { return m_file; }
};