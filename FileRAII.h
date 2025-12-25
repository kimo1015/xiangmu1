#pragma once
#include <afx.h>

// RAII 包装类：确保文件和存档在生命周期结束时自动关闭
class CFileArchiveRAII {
public:
    CFile m_file;
    CArchive* m_pArchive;
    BOOL m_bIsOpen;

    // 构造函数：尝试打开文件
    CFileArchiveRAII(LPCTSTR lpszFileName, UINT nOpenFlags) : m_pArchive(nullptr), m_bIsOpen(FALSE) {
        if (m_file.Open(lpszFileName, nOpenFlags)) {
            m_bIsOpen = TRUE;
        }
        else {
            // 如果打开失败，抛出文件异常
            AfxThrowFileException(CFileException::genericException);
        }
    }

    // 创建存档对象
    void CreateArchive(UINT nArchiveFlags) {
        m_pArchive = new CArchive(&m_file, nArchiveFlags);
    }

    // 析构函数：RAII 的核心，自动释放资源
    ~CFileArchiveRAII() {
        if (m_pArchive) {
            m_pArchive->Close();
            delete m_pArchive;
        }
        if (m_bIsOpen) {
            m_file.Close();
        }
    }
};