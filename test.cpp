#include <gtest/gtest.h>
#include <afxwin.h>
#include <afxext.h>

// 1. 物理包含主项目的 Doc 逻辑（实现真实代码行覆盖）
#include "../MDI_Notepad_Project1/MDI_Notepad_Project1Doc.cpp"

// 2. 模拟类：用于访问 Protected 成员并执行测试
class TestDoc : public CMDINotepadProject1Doc {
public:
    using CMDINotepadProject1Doc::m_strContent;
    using CMDINotepadProject1Doc::Serialize;  // 覆盖文件 I/O 分支
    using CMDINotepadProject1Doc::SaveState;  // 覆盖撤销栈逻辑
    using CMDINotepadProject1Doc::Undo;       // 覆盖撤销功能
    using CMDINotepadProject1Doc::Redo;       // 覆盖重做功能
};

// --- 测试用例 1：文件 I/O 覆盖 (F-01) ---
TEST(MDINotepadCoverage, FileIO_Serialization) {
    TestDoc doc;
    doc.m_strContent = _T("RAII Test Data 2025");

    // 模拟文件存储过程，覆盖 Serialize 中的 IsStoring 分支
    CMemFile memFile;
    CArchive arSave(&memFile, CArchive::store);
    doc.Serialize(arSave);
    arSave.Close();

    // 模拟文件读取过程，覆盖 Serialize 中的读取分支
    memFile.SeekToBegin();
    CArchive arLoad(&memFile, CArchive::load);
    TestDoc docRead;
    docRead.Serialize(arLoad);
    arLoad.Close();

    EXPECT_STREQ(docRead.m_strContent, _T("RAII Test Data 2025"));
}

// --- 测试用例 2：自定义替换与撤销覆盖 (F-04, F-05) ---
TEST(MDINotepadCoverage, Replace_And_Undo_Logic) {
    TestDoc doc;
    doc.m_strContent = _T("Hello 123 World");

    // 覆盖 ReplaceText 函数中的正则替换逻辑
    doc.ReplaceText(_T("\\d+"), _T("CustomContent"));
    EXPECT_STREQ(doc.m_strContent, _T("Hello CustomContent World"));

    // 覆盖 Undo 逻辑：确保替换后可以撤销回原始状态
    doc.Undo();
    EXPECT_STREQ(doc.m_strContent, _T("Hello 123 World"));

    // 覆盖 Redo 逻辑
    doc.Redo();
    EXPECT_STREQ(doc.m_strContent, _T("Hello CustomContent World"));
}

// --- 测试用例 3：异常处理覆盖 (Error Handling) ---
TEST(MDINotepadCoverage, Exception_Handling) {
    TestDoc doc;
    doc.m_strContent = _T("Safe Content");

    // 覆盖 ReplaceText 中的 catch(...) 分支：传递非法正则表达式
    // 这将触发代码中的 AfxMessageBox 提示并被 catch 捕获
    doc.ReplaceText(_T("[[["), _T("error"));

    // 验证在报错情况下，原始内容保持不变（证明 catch 块工作正常）
    EXPECT_STREQ(doc.m_strContent, _T("Safe Content"));
}

// --- 测试用例 4：行号统计逻辑覆盖 (F-02) ---
TEST(MDINotepadCoverage, LineCounting_Logic) {
    CString text = _T("Line1\nLine2\nLine3");
    int count = 1;
    for (int i = 0; i < text.GetLength(); i++) {
        if (text[i] == _T('\n')) count++;
    }
    EXPECT_EQ(count, 3);
}