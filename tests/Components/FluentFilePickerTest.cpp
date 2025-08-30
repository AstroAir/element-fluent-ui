// tests/Components/FluentFilePickerTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentFilePicker.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentFilePickerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // File selection tests
    void testSelectedFiles();
    void testSelectedFile();
    void testSetSelectedFiles();
    void testClearSelection();

    // Filter tests
    void testFileFilter();
    void testNameFilters();
    void testDefaultSuffix();

    // Directory tests
    void testDirectory();
    void testSetDirectory();

    // Mode tests
    void testFileMode();
    void testAcceptMode();
    void testViewMode();

    // Options tests
    void testOptions();
    void testReadOnly();
    void testResolveSymlinks();
    void testConfirmOverwrite();

    // Dialog tests
    void testShowDialog();
    void testDialogTitle();
    void testLabelText();

    // Signal tests
    void testSignalEmission();

    // Validation tests
    void testFileValidation();

private:
    FluentFilePicker* m_filePicker{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentFilePickerTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentFilePickerTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentFilePickerTest::init() {
    m_filePicker = new FluentFilePicker(m_testWidget);
}

void FluentFilePickerTest::cleanup() {
    delete m_filePicker;
    m_filePicker = nullptr;
}

void FluentFilePickerTest::testDefaultConstructor() {
    QVERIFY(m_filePicker != nullptr);
    QCOMPARE(m_filePicker->fileMode(), FluentFilePickerMode::ExistingFile);
    QCOMPARE(m_filePicker->acceptMode(), FluentFilePickerAcceptMode::AcceptOpen);
    QCOMPARE(m_filePicker->viewMode(), FluentFilePickerViewMode::Detail);
    QVERIFY(m_filePicker->selectedFiles().isEmpty());
    QVERIFY(m_filePicker->resolveSymlinks());
    QVERIFY(m_filePicker->confirmOverwrite());
}

void FluentFilePickerTest::testSelectedFiles() {
    QStringList testFiles = {
        "/path/to/file1.txt",
        "/path/to/file2.txt",
        "/path/to/file3.txt"
    };
    
    QSignalSpy selectedFilesSpy(m_filePicker, &FluentFilePicker::selectedFilesChanged);
    
    m_filePicker->setSelectedFiles(testFiles);
    QCOMPARE(m_filePicker->selectedFiles(), testFiles);
    QCOMPARE(selectedFilesSpy.count(), 1);
    
    // Test empty list
    m_filePicker->setSelectedFiles(QStringList());
    QVERIFY(m_filePicker->selectedFiles().isEmpty());
    QCOMPARE(selectedFilesSpy.count(), 2);
}

void FluentFilePickerTest::testSelectedFile() {
    QString testFile = "/path/to/test.txt";
    
    QSignalSpy selectedFilesSpy(m_filePicker, &FluentFilePicker::selectedFilesChanged);
    
    m_filePicker->setSelectedFile(testFile);
    QCOMPARE(m_filePicker->selectedFile(), testFile);
    QCOMPARE(m_filePicker->selectedFiles().count(), 1);
    QCOMPARE(m_filePicker->selectedFiles().first(), testFile);
    QCOMPARE(selectedFilesSpy.count(), 1);
    
    // Test empty file
    m_filePicker->setSelectedFile("");
    QCOMPARE(m_filePicker->selectedFile(), QString());
    QVERIFY(m_filePicker->selectedFiles().isEmpty());
}

void FluentFilePickerTest::testSetSelectedFiles() {
    QStringList files = {"/file1.txt", "/file2.txt"};
    
    m_filePicker->setFileMode(FluentFilePickerMode::ExistingFiles);
    m_filePicker->setSelectedFiles(files);
    
    QCOMPARE(m_filePicker->selectedFiles(), files);
    
    // Test single file mode with multiple files (should take first)
    m_filePicker->setFileMode(FluentFilePickerMode::ExistingFile);
    m_filePicker->setSelectedFiles(files);
    QCOMPARE(m_filePicker->selectedFiles().count(), 1);
    QCOMPARE(m_filePicker->selectedFile(), files.first());
}

void FluentFilePickerTest::testClearSelection() {
    m_filePicker->setSelectedFiles({"/file1.txt", "/file2.txt"});
    
    QSignalSpy selectedFilesSpy(m_filePicker, &FluentFilePicker::selectedFilesChanged);
    
    m_filePicker->clearSelection();
    QVERIFY(m_filePicker->selectedFiles().isEmpty());
    QCOMPARE(m_filePicker->selectedFile(), QString());
    QCOMPARE(selectedFilesSpy.count(), 1);
}

void FluentFilePickerTest::testFileFilter() {
    QSignalSpy filterSpy(m_filePicker, &FluentFilePicker::fileFilterChanged);
    
    QString filter = "Text files (*.txt);;All files (*.*)";
    m_filePicker->setFileFilter(filter);
    QCOMPARE(m_filePicker->fileFilter(), filter);
    QCOMPARE(filterSpy.count(), 1);
    
    // Test empty filter
    m_filePicker->setFileFilter("");
    QCOMPARE(m_filePicker->fileFilter(), "");
    QCOMPARE(filterSpy.count(), 2);
}

void FluentFilePickerTest::testNameFilters() {
    QSignalSpy nameFiltersSpy(m_filePicker, &FluentFilePicker::nameFiltersChanged);
    
    QStringList filters = {"*.txt", "*.doc", "*.pdf"};
    m_filePicker->setNameFilters(filters);
    QCOMPARE(m_filePicker->nameFilters(), filters);
    QCOMPARE(nameFiltersSpy.count(), 1);
    
    // Test single filter
    m_filePicker->setNameFilter("*.jpg");
    QCOMPARE(m_filePicker->nameFilters().count(), 1);
    QCOMPARE(m_filePicker->nameFilters().first(), "*.jpg");
    QCOMPARE(nameFiltersSpy.count(), 2);
}

void FluentFilePickerTest::testDefaultSuffix() {
    QSignalSpy suffixSpy(m_filePicker, &FluentFilePicker::defaultSuffixChanged);
    
    m_filePicker->setDefaultSuffix("txt");
    QCOMPARE(m_filePicker->defaultSuffix(), "txt");
    QCOMPARE(suffixSpy.count(), 1);
    
    // Test suffix with dot
    m_filePicker->setDefaultSuffix(".pdf");
    QCOMPARE(m_filePicker->defaultSuffix(), "pdf"); // Should remove leading dot
    QCOMPARE(suffixSpy.count(), 2);
}

void FluentFilePickerTest::testDirectory() {
    QString testDir = "/home/user/documents";
    
    QSignalSpy directorySpy(m_filePicker, &FluentFilePicker::directoryChanged);
    
    m_filePicker->setDirectory(testDir);
    QCOMPARE(m_filePicker->directory(), testDir);
    QCOMPARE(directorySpy.count(), 1);
    
    // Test QDir object
    QDir dir("/tmp");
    m_filePicker->setDirectory(dir);
    QCOMPARE(m_filePicker->directory(), dir.absolutePath());
    QCOMPARE(directorySpy.count(), 2);
}

void FluentFilePickerTest::testSetDirectory() {
    QString currentDir = QDir::currentPath();
    QString tempDir = QDir::tempPath();
    
    m_filePicker->setDirectory(currentDir);
    QCOMPARE(m_filePicker->directory(), currentDir);
    
    m_filePicker->setDirectory(tempDir);
    QCOMPARE(m_filePicker->directory(), tempDir);
    
    // Test invalid directory
    m_filePicker->setDirectory("/invalid/path/that/does/not/exist");
    // Should either remain unchanged or handle gracefully
}

void FluentFilePickerTest::testFileMode() {
    QSignalSpy fileModeSpy(m_filePicker, &FluentFilePicker::fileModeChanged);
    
    m_filePicker->setFileMode(FluentFilePickerMode::ExistingFiles);
    QCOMPARE(m_filePicker->fileMode(), FluentFilePickerMode::ExistingFiles);
    QCOMPARE(fileModeSpy.count(), 1);
    
    m_filePicker->setFileMode(FluentFilePickerMode::AnyFile);
    QCOMPARE(m_filePicker->fileMode(), FluentFilePickerMode::AnyFile);
    QCOMPARE(fileModeSpy.count(), 2);
    
    m_filePicker->setFileMode(FluentFilePickerMode::Directory);
    QCOMPARE(m_filePicker->fileMode(), FluentFilePickerMode::Directory);
    QCOMPARE(fileModeSpy.count(), 3);
}

void FluentFilePickerTest::testAcceptMode() {
    QSignalSpy acceptModeSpy(m_filePicker, &FluentFilePicker::acceptModeChanged);
    
    m_filePicker->setAcceptMode(FluentFilePickerAcceptMode::AcceptSave);
    QCOMPARE(m_filePicker->acceptMode(), FluentFilePickerAcceptMode::AcceptSave);
    QCOMPARE(acceptModeSpy.count(), 1);
    
    m_filePicker->setAcceptMode(FluentFilePickerAcceptMode::AcceptOpen);
    QCOMPARE(m_filePicker->acceptMode(), FluentFilePickerAcceptMode::AcceptOpen);
    QCOMPARE(acceptModeSpy.count(), 2);
}

void FluentFilePickerTest::testViewMode() {
    QSignalSpy viewModeSpy(m_filePicker, &FluentFilePicker::viewModeChanged);
    
    m_filePicker->setViewMode(FluentFilePickerViewMode::List);
    QCOMPARE(m_filePicker->viewMode(), FluentFilePickerViewMode::List);
    QCOMPARE(viewModeSpy.count(), 1);
    
    m_filePicker->setViewMode(FluentFilePickerViewMode::Detail);
    QCOMPARE(m_filePicker->viewMode(), FluentFilePickerViewMode::Detail);
    QCOMPARE(viewModeSpy.count(), 2);
}

void FluentFilePickerTest::testOptions() {
    QSignalSpy optionsSpy(m_filePicker, &FluentFilePicker::optionsChanged);
    
    FluentFilePickerOptions options = FluentFilePickerOption::ShowDirsOnly | 
                                     FluentFilePickerOption::DontResolveSymlinks;
    m_filePicker->setOptions(options);
    QCOMPARE(m_filePicker->options(), options);
    QCOMPARE(optionsSpy.count(), 1);
    
    // Test individual option
    m_filePicker->setOption(FluentFilePickerOption::ReadOnly, true);
    QVERIFY(m_filePicker->testOption(FluentFilePickerOption::ReadOnly));
    
    m_filePicker->setOption(FluentFilePickerOption::ReadOnly, false);
    QVERIFY(!m_filePicker->testOption(FluentFilePickerOption::ReadOnly));
}

void FluentFilePickerTest::testReadOnly() {
    QSignalSpy readOnlySpy(m_filePicker, &FluentFilePicker::readOnlyChanged);
    
    m_filePicker->setReadOnly(true);
    QVERIFY(m_filePicker->isReadOnly());
    QCOMPARE(readOnlySpy.count(), 1);
    
    m_filePicker->setReadOnly(false);
    QVERIFY(!m_filePicker->isReadOnly());
    QCOMPARE(readOnlySpy.count(), 2);
}

void FluentFilePickerTest::testResolveSymlinks() {
    QSignalSpy resolveSymlinksSpy(m_filePicker, &FluentFilePicker::resolveSymlinksChanged);
    
    m_filePicker->setResolveSymlinks(false);
    QVERIFY(!m_filePicker->resolveSymlinks());
    QCOMPARE(resolveSymlinksSpy.count(), 1);
    
    m_filePicker->setResolveSymlinks(true);
    QVERIFY(m_filePicker->resolveSymlinks());
    QCOMPARE(resolveSymlinksSpy.count(), 2);
}

void FluentFilePickerTest::testConfirmOverwrite() {
    QSignalSpy confirmOverwriteSpy(m_filePicker, &FluentFilePicker::confirmOverwriteChanged);
    
    m_filePicker->setConfirmOverwrite(false);
    QVERIFY(!m_filePicker->confirmOverwrite());
    QCOMPARE(confirmOverwriteSpy.count(), 1);
    
    m_filePicker->setConfirmOverwrite(true);
    QVERIFY(m_filePicker->confirmOverwrite());
    QCOMPARE(confirmOverwriteSpy.count(), 2);
}

void FluentFilePickerTest::testShowDialog() {
    QSignalSpy acceptedSpy(m_filePicker, &FluentFilePicker::accepted);
    QSignalSpy rejectedSpy(m_filePicker, &FluentFilePicker::rejected);
    QSignalSpy finishedSpy(m_filePicker, &FluentFilePicker::finished);
    
    // Test programmatic accept/reject (since we can't interact with actual dialog in tests)
    m_filePicker->accept();
    QCOMPARE(acceptedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    
    m_filePicker->reject();
    QCOMPARE(rejectedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 2);
    
    // Test exec method (non-blocking for tests)
    QTimer::singleShot(100, m_filePicker, &FluentFilePicker::reject);
    int result = m_filePicker->exec();
    QCOMPARE(result, QDialog::Rejected);
}

void FluentFilePickerTest::testDialogTitle() {
    QSignalSpy titleSpy(m_filePicker, &FluentFilePicker::windowTitleChanged);
    
    m_filePicker->setWindowTitle("Select File");
    QCOMPARE(m_filePicker->windowTitle(), "Select File");
    QCOMPARE(titleSpy.count(), 1);
    
    // Test empty title
    m_filePicker->setWindowTitle("");
    QCOMPARE(m_filePicker->windowTitle(), "");
    QCOMPARE(titleSpy.count(), 2);
}

void FluentFilePickerTest::testLabelText() {
    QSignalSpy labelTextSpy(m_filePicker, &FluentFilePicker::labelTextChanged);
    
    m_filePicker->setLabelText(FluentFilePickerLabel::Accept, "Open File");
    QCOMPARE(m_filePicker->labelText(FluentFilePickerLabel::Accept), "Open File");
    QCOMPARE(labelTextSpy.count(), 1);
    
    m_filePicker->setLabelText(FluentFilePickerLabel::Reject, "Cancel");
    QCOMPARE(m_filePicker->labelText(FluentFilePickerLabel::Reject), "Cancel");
    QCOMPARE(labelTextSpy.count(), 2);
    
    m_filePicker->setLabelText(FluentFilePickerLabel::LookIn, "Look in:");
    QCOMPARE(m_filePicker->labelText(FluentFilePickerLabel::LookIn), "Look in:");
    QCOMPARE(labelTextSpy.count(), 3);
}

void FluentFilePickerTest::testSignalEmission() {
    QSignalSpy selectedFilesSpy(m_filePicker, &FluentFilePicker::selectedFilesChanged);
    QSignalSpy fileFilterSpy(m_filePicker, &FluentFilePicker::fileFilterChanged);
    QSignalSpy nameFiltersSpy(m_filePicker, &FluentFilePicker::nameFiltersChanged);
    QSignalSpy defaultSuffixSpy(m_filePicker, &FluentFilePicker::defaultSuffixChanged);
    QSignalSpy directorySpy(m_filePicker, &FluentFilePicker::directoryChanged);
    QSignalSpy fileModeSpy(m_filePicker, &FluentFilePicker::fileModeChanged);
    QSignalSpy acceptModeSpy(m_filePicker, &FluentFilePicker::acceptModeChanged);
    QSignalSpy viewModeSpy(m_filePicker, &FluentFilePicker::viewModeChanged);
    QSignalSpy optionsSpy(m_filePicker, &FluentFilePicker::optionsChanged);
    QSignalSpy readOnlySpy(m_filePicker, &FluentFilePicker::readOnlyChanged);
    
    // Test all signals
    m_filePicker->setSelectedFiles({"/test.txt"});
    m_filePicker->setFileFilter("*.txt");
    m_filePicker->setNameFilters({"*.txt", "*.doc"});
    m_filePicker->setDefaultSuffix("txt");
    m_filePicker->setDirectory("/tmp");
    m_filePicker->setFileMode(FluentFilePickerMode::ExistingFiles);
    m_filePicker->setAcceptMode(FluentFilePickerAcceptMode::AcceptSave);
    m_filePicker->setViewMode(FluentFilePickerViewMode::List);
    m_filePicker->setOptions(FluentFilePickerOption::ReadOnly);
    m_filePicker->setReadOnly(true);
    
    QCOMPARE(selectedFilesSpy.count(), 1);
    QCOMPARE(fileFilterSpy.count(), 1);
    QCOMPARE(nameFiltersSpy.count(), 1);
    QCOMPARE(defaultSuffixSpy.count(), 1);
    QCOMPARE(directorySpy.count(), 1);
    QCOMPARE(fileModeSpy.count(), 1);
    QCOMPARE(acceptModeSpy.count(), 1);
    QCOMPARE(viewModeSpy.count(), 1);
    QCOMPARE(optionsSpy.count(), 1);
    QCOMPARE(readOnlySpy.count(), 1);
}

void FluentFilePickerTest::testFileValidation() {
    // Test file existence validation
    QString existingFile = QDir::tempPath() + "/test_file.txt";
    QFile file(existingFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write("test content");
        file.close();
        
        m_filePicker->setFileMode(FluentFilePickerMode::ExistingFile);
        m_filePicker->setSelectedFile(existingFile);
        QVERIFY(m_filePicker->isFileValid(existingFile));
        
        file.remove();
    }
    
    // Test non-existing file
    QString nonExistingFile = "/path/that/does/not/exist.txt";
    m_filePicker->setFileMode(FluentFilePickerMode::ExistingFile);
    QVERIFY(!m_filePicker->isFileValid(nonExistingFile));
    
    // Test file extension validation
    m_filePicker->setNameFilters({"*.txt", "*.doc"});
    QVERIFY(m_filePicker->isFileValid("document.txt"));
    QVERIFY(m_filePicker->isFileValid("document.doc"));
    QVERIFY(!m_filePicker->isFileValid("image.jpg"));
    
    // Test directory validation
    m_filePicker->setFileMode(FluentFilePickerMode::Directory);
    QVERIFY(m_filePicker->isFileValid(QDir::tempPath()));
    QVERIFY(!m_filePicker->isFileValid("/invalid/directory/path"));
}

QTEST_MAIN(FluentFilePickerTest)
#include "FluentFilePickerTest.moc"
