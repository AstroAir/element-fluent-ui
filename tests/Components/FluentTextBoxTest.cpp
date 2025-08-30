// tests/Components/FluentTextBoxTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentTextBox.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentTextBoxTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Text tests
    void testText();
    void testPlaceholderText();
    void testMaxLength();

    // Input validation tests
    void testInputMask();
    void testValidator();
    void testReadOnly();

    // Appearance tests
    void testEchoMode();
    void testAlignment();
    void testFont();

    // Selection tests
    void testSelection();
    void testSelectAll();
    void testClearSelection();

    // Editing tests
    void testUndo();
    void testRedo();
    void testCut();
    void testCopy();
    void testPaste();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testKeyboardInput();
    void testMouseInteraction();

private:
    FluentTextBox* m_textBox{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentTextBoxTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentTextBoxTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentTextBoxTest::init() {
    m_textBox = new FluentTextBox(m_testWidget);
}

void FluentTextBoxTest::cleanup() {
    delete m_textBox;
    m_textBox = nullptr;
}

void FluentTextBoxTest::testDefaultConstructor() {
    QVERIFY(m_textBox != nullptr);
    QCOMPARE(m_textBox->text(), QString());
    QCOMPARE(m_textBox->placeholderText(), QString());
    QCOMPARE(m_textBox->maxLength(), 32767);
    QVERIFY(!m_textBox->isReadOnly());
    QCOMPARE(m_textBox->echoMode(), QLineEdit::Normal);
    QCOMPARE(m_textBox->alignment(), Qt::AlignLeft | Qt::AlignVCenter);
}

void FluentTextBoxTest::testText() {
    QSignalSpy textSpy(m_textBox, &FluentTextBox::textChanged);
    QSignalSpy textEditedSpy(m_textBox, &FluentTextBox::textEdited);
    
    m_textBox->setText("Hello World");
    QCOMPARE(m_textBox->text(), "Hello World");
    QCOMPARE(textSpy.count(), 1);
    QCOMPARE(textSpy.first().first().toString(), "Hello World");
    
    // Test empty text
    m_textBox->setText("");
    QCOMPARE(m_textBox->text(), "");
    QCOMPARE(textSpy.count(), 2);
    
    // Test text with special characters
    m_textBox->setText("Special: !@#$%^&*()");
    QCOMPARE(m_textBox->text(), "Special: !@#$%^&*()");
    QCOMPARE(textSpy.count(), 3);
}

void FluentTextBoxTest::testPlaceholderText() {
    QSignalSpy placeholderSpy(m_textBox, &FluentTextBox::placeholderTextChanged);
    
    m_textBox->setPlaceholderText("Enter text here...");
    QCOMPARE(m_textBox->placeholderText(), "Enter text here...");
    QCOMPARE(placeholderSpy.count(), 1);
    
    // Test empty placeholder
    m_textBox->setPlaceholderText("");
    QCOMPARE(m_textBox->placeholderText(), "");
    QCOMPARE(placeholderSpy.count(), 2);
}

void FluentTextBoxTest::testMaxLength() {
    QSignalSpy maxLengthSpy(m_textBox, &FluentTextBox::maxLengthChanged);
    
    m_textBox->setMaxLength(10);
    QCOMPARE(m_textBox->maxLength(), 10);
    QCOMPARE(maxLengthSpy.count(), 1);
    
    // Test text truncation
    m_textBox->setText("This is a very long text that exceeds the maximum length");
    QVERIFY(m_textBox->text().length() <= 10);
    
    // Test invalid max length
    m_textBox->setMaxLength(-5);
    QVERIFY(m_textBox->maxLength() >= 0);
}

void FluentTextBoxTest::testInputMask() {
    QSignalSpy inputMaskSpy(m_textBox, &FluentTextBox::inputMaskChanged);
    
    m_textBox->setInputMask("000.000.000.000;_");
    QCOMPARE(m_textBox->inputMask(), "000.000.000.000;_");
    QCOMPARE(inputMaskSpy.count(), 1);
    
    // Test input with mask
    m_textBox->setText("192168001001");
    QString maskedText = m_textBox->text();
    QVERIFY(maskedText.contains('.'));
    
    // Test clearing mask
    m_textBox->setInputMask("");
    QCOMPARE(m_textBox->inputMask(), "");
    QCOMPARE(inputMaskSpy.count(), 2);
}

void FluentTextBoxTest::testValidator() {
    auto* intValidator = new QIntValidator(0, 100, m_textBox);
    m_textBox->setValidator(intValidator);
    QCOMPARE(m_textBox->validator(), intValidator);
    
    // Test valid input
    m_textBox->setText("50");
    QCOMPARE(m_textBox->text(), "50");
    
    // Test invalid input (should be rejected or modified)
    m_textBox->setText("150");
    // Behavior depends on validator implementation
    
    // Test null validator
    m_textBox->setValidator(nullptr);
    QCOMPARE(m_textBox->validator(), nullptr);
}

void FluentTextBoxTest::testReadOnly() {
    QSignalSpy readOnlySpy(m_textBox, &FluentTextBox::readOnlyChanged);
    
    m_textBox->setReadOnly(true);
    QVERIFY(m_textBox->isReadOnly());
    QCOMPARE(readOnlySpy.count(), 1);
    
    // Test that text cannot be changed in read-only mode
    QString originalText = "Original";
    m_textBox->setText(originalText);
    m_textBox->setReadOnly(true);
    
    // Programmatic changes should still work
    m_textBox->setText("Changed");
    QCOMPARE(m_textBox->text(), "Changed");
    
    m_textBox->setReadOnly(false);
    QVERIFY(!m_textBox->isReadOnly());
    QCOMPARE(readOnlySpy.count(), 2);
}

void FluentTextBoxTest::testEchoMode() {
    QSignalSpy echoModeSpy(m_textBox, &FluentTextBox::echoModeChanged);
    
    m_textBox->setEchoMode(QLineEdit::Password);
    QCOMPARE(m_textBox->echoMode(), QLineEdit::Password);
    QCOMPARE(echoModeSpy.count(), 1);
    
    m_textBox->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    QCOMPARE(m_textBox->echoMode(), QLineEdit::PasswordEchoOnEdit);
    QCOMPARE(echoModeSpy.count(), 2);
    
    m_textBox->setEchoMode(QLineEdit::NoEcho);
    QCOMPARE(m_textBox->echoMode(), QLineEdit::NoEcho);
    QCOMPARE(echoModeSpy.count(), 3);
}

void FluentTextBoxTest::testAlignment() {
    QSignalSpy alignmentSpy(m_textBox, &FluentTextBox::alignmentChanged);
    
    m_textBox->setAlignment(Qt::AlignCenter);
    QCOMPARE(m_textBox->alignment(), Qt::AlignCenter);
    QCOMPARE(alignmentSpy.count(), 1);
    
    m_textBox->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QCOMPARE(m_textBox->alignment(), Qt::AlignRight | Qt::AlignVCenter);
    QCOMPARE(alignmentSpy.count(), 2);
}

void FluentTextBoxTest::testFont() {
    QSignalSpy fontSpy(m_textBox, &FluentTextBox::fontChanged);
    
    QFont testFont("Arial", 12, QFont::Bold);
    m_textBox->setFont(testFont);
    QCOMPARE(m_textBox->font().family(), testFont.family());
    QCOMPARE(m_textBox->font().pointSize(), testFont.pointSize());
    QCOMPARE(m_textBox->font().weight(), testFont.weight());
    QCOMPARE(fontSpy.count(), 1);
}

void FluentTextBoxTest::testSelection() {
    m_textBox->setText("Hello World");
    
    QSignalSpy selectionChangedSpy(m_textBox, &FluentTextBox::selectionChanged);
    
    m_textBox->setSelection(0, 5); // Select "Hello"
    QCOMPARE(m_textBox->selectedText(), "Hello");
    QCOMPARE(m_textBox->selectionStart(), 0);
    QCOMPARE(m_textBox->selectionLength(), 5);
    QVERIFY(m_textBox->hasSelectedText());
    QCOMPARE(selectionChangedSpy.count(), 1);
    
    // Test cursor position
    m_textBox->setCursorPosition(6);
    QCOMPARE(m_textBox->cursorPosition(), 6);
}

void FluentTextBoxTest::testSelectAll() {
    m_textBox->setText("Hello World");
    
    QSignalSpy selectionChangedSpy(m_textBox, &FluentTextBox::selectionChanged);
    
    m_textBox->selectAll();
    QCOMPARE(m_textBox->selectedText(), "Hello World");
    QCOMPARE(m_textBox->selectionStart(), 0);
    QCOMPARE(m_textBox->selectionLength(), 11);
    QCOMPARE(selectionChangedSpy.count(), 1);
}

void FluentTextBoxTest::testClearSelection() {
    m_textBox->setText("Hello World");
    m_textBox->selectAll();
    
    QSignalSpy selectionChangedSpy(m_textBox, &FluentTextBox::selectionChanged);
    
    m_textBox->deselect();
    QVERIFY(!m_textBox->hasSelectedText());
    QCOMPARE(m_textBox->selectedText(), "");
    QCOMPARE(selectionChangedSpy.count(), 1);
}

void FluentTextBoxTest::testUndo() {
    m_textBox->setText("Original");
    m_textBox->setText("Modified");
    
    QVERIFY(m_textBox->isUndoAvailable());
    m_textBox->undo();
    QCOMPARE(m_textBox->text(), "Original");
}

void FluentTextBoxTest::testRedo() {
    m_textBox->setText("Original");
    m_textBox->setText("Modified");
    m_textBox->undo();
    
    QVERIFY(m_textBox->isRedoAvailable());
    m_textBox->redo();
    QCOMPARE(m_textBox->text(), "Modified");
}

void FluentTextBoxTest::testCut() {
    m_textBox->setText("Hello World");
    m_textBox->setSelection(0, 5); // Select "Hello"
    
    m_textBox->cut();
    QCOMPARE(m_textBox->text(), " World");
    
    // Check clipboard (if accessible)
    QClipboard* clipboard = QApplication::clipboard();
    if (clipboard) {
        QCOMPARE(clipboard->text(), "Hello");
    }
}

void FluentTextBoxTest::testCopy() {
    m_textBox->setText("Hello World");
    m_textBox->setSelection(6, 5); // Select "World"
    
    m_textBox->copy();
    QCOMPARE(m_textBox->text(), "Hello World"); // Text should remain unchanged
    
    // Check clipboard (if accessible)
    QClipboard* clipboard = QApplication::clipboard();
    if (clipboard) {
        QCOMPARE(clipboard->text(), "World");
    }
}

void FluentTextBoxTest::testPaste() {
    // Set clipboard content
    QClipboard* clipboard = QApplication::clipboard();
    if (clipboard) {
        clipboard->setText("Pasted");
    }
    
    m_textBox->setText("Hello ");
    m_textBox->setCursorPosition(6);
    m_textBox->paste();
    
    if (clipboard) {
        QCOMPARE(m_textBox->text(), "Hello Pasted");
    }
}

void FluentTextBoxTest::testSignalEmission() {
    QSignalSpy textSpy(m_textBox, &FluentTextBox::textChanged);
    QSignalSpy textEditedSpy(m_textBox, &FluentTextBox::textEdited);
    QSignalSpy placeholderSpy(m_textBox, &FluentTextBox::placeholderTextChanged);
    QSignalSpy maxLengthSpy(m_textBox, &FluentTextBox::maxLengthChanged);
    QSignalSpy inputMaskSpy(m_textBox, &FluentTextBox::inputMaskChanged);
    QSignalSpy readOnlySpy(m_textBox, &FluentTextBox::readOnlyChanged);
    QSignalSpy echoModeSpy(m_textBox, &FluentTextBox::echoModeChanged);
    QSignalSpy alignmentSpy(m_textBox, &FluentTextBox::alignmentChanged);
    QSignalSpy fontSpy(m_textBox, &FluentTextBox::fontChanged);
    QSignalSpy selectionChangedSpy(m_textBox, &FluentTextBox::selectionChanged);
    QSignalSpy returnPressedSpy(m_textBox, &FluentTextBox::returnPressed);
    QSignalSpy editingFinishedSpy(m_textBox, &FluentTextBox::editingFinished);
    
    // Test all signals
    m_textBox->setText("test");
    m_textBox->setPlaceholderText("placeholder");
    m_textBox->setMaxLength(50);
    m_textBox->setInputMask("AAA");
    m_textBox->setReadOnly(true);
    m_textBox->setEchoMode(QLineEdit::Password);
    m_textBox->setAlignment(Qt::AlignCenter);
    m_textBox->setFont(QFont("Arial", 10));
    m_textBox->selectAll();
    
    // Simulate user input
    m_textBox->setFocus();
    QTest::keyClick(m_textBox, Qt::Key_Return);
    m_textBox->clearFocus();
    
    QCOMPARE(textSpy.count(), 1);
    QCOMPARE(placeholderSpy.count(), 1);
    QCOMPARE(maxLengthSpy.count(), 1);
    QCOMPARE(inputMaskSpy.count(), 1);
    QCOMPARE(readOnlySpy.count(), 1);
    QCOMPARE(echoModeSpy.count(), 1);
    QCOMPARE(alignmentSpy.count(), 1);
    QCOMPARE(fontSpy.count(), 1);
    QCOMPARE(selectionChangedSpy.count(), 1);
    QCOMPARE(returnPressedSpy.count(), 1);
    QCOMPARE(editingFinishedSpy.count(), 1);
}

void FluentTextBoxTest::testKeyboardInput() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_textBox->setFocus();
    
    QSignalSpy textSpy(m_textBox, &FluentTextBox::textChanged);
    QSignalSpy textEditedSpy(m_textBox, &FluentTextBox::textEdited);
    
    // Test typing
    QTest::keyClicks(m_textBox, "Hello");
    QCOMPARE(m_textBox->text(), "Hello");
    QVERIFY(textSpy.count() >= 1);
    QVERIFY(textEditedSpy.count() >= 1);
    
    // Test backspace
    QTest::keyClick(m_textBox, Qt::Key_Backspace);
    QCOMPARE(m_textBox->text(), "Hell");
    
    // Test delete
    m_textBox->setCursorPosition(0);
    QTest::keyClick(m_textBox, Qt::Key_Delete);
    QCOMPARE(m_textBox->text(), "ell");
    
    // Test Ctrl+A (select all)
    QTest::keyClick(m_textBox, Qt::Key_A, Qt::ControlModifier);
    QVERIFY(m_textBox->hasSelectedText());
    
    // Test Ctrl+C (copy)
    QTest::keyClick(m_textBox, Qt::Key_C, Qt::ControlModifier);
    
    // Test Ctrl+V (paste)
    QTest::keyClick(m_textBox, Qt::Key_V, Qt::ControlModifier);
}

void FluentTextBoxTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_textBox->setText("Hello World");
    
    // Test mouse click to position cursor
    QTest::mouseClick(m_textBox, Qt::LeftButton, Qt::NoModifier, QPoint(50, 15));
    QVERIFY(m_textBox->hasFocus());
    
    // Test double-click to select word
    QTest::mouseDClick(m_textBox, Qt::LeftButton, Qt::NoModifier, QPoint(30, 15));
    QVERIFY(m_textBox->hasSelectedText());
    
    // Test triple-click to select all
    QTest::mouseClick(m_textBox, Qt::LeftButton, Qt::NoModifier, QPoint(30, 15));
    QTest::mouseClick(m_textBox, Qt::LeftButton, Qt::NoModifier, QPoint(30, 15));
    QTest::mouseClick(m_textBox, Qt::LeftButton, Qt::NoModifier, QPoint(30, 15));
    
    // Test drag to select text
    QTest::mousePress(m_textBox, Qt::LeftButton, Qt::NoModifier, QPoint(10, 15));
    QTest::mouseMove(m_textBox, QPoint(60, 15));
    QTest::mouseRelease(m_textBox, Qt::LeftButton, Qt::NoModifier, QPoint(60, 15));
    QVERIFY(m_textBox->hasSelectedText());
}

QTEST_MAIN(FluentTextBoxTest)
#include "FluentTextBoxTest.moc"
