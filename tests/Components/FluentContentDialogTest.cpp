// tests/Components/FluentContentDialogTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentContentDialog.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentContentDialogTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTitleConstructor();

    // Content tests
    void testTitle();
    void testContent();
    void testContentWidget();

    // Button tests
    void testPrimaryButton();
    void testSecondaryButton();
    void testCloseButton();
    void testButtonText();

    // Dialog behavior tests
    void testModal();
    void testClosable();
    void testResizable();
    void testDefaultButton();

    // Result tests
    void testDialogResult();
    void testAccept();
    void testReject();

    // Interaction tests
    void testButtonClicks();
    void testKeyboardInteraction();
    void testEscapeKey();

    // Signal tests
    void testSignalEmission();

    // Show/Hide tests
    void testShowDialog();
    void testHideDialog();

private:
    FluentContentDialog* m_dialog{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentContentDialogTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentContentDialogTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentContentDialogTest::init() {
    m_dialog = new FluentContentDialog(m_testWidget);
}

void FluentContentDialogTest::cleanup() {
    delete m_dialog;
    m_dialog = nullptr;
}

void FluentContentDialogTest::testDefaultConstructor() {
    QVERIFY(m_dialog != nullptr);
    QCOMPARE(m_dialog->title(), QString());
    QCOMPARE(m_dialog->content(), QString());
    QVERIFY(m_dialog->isModal());
    QVERIFY(m_dialog->isClosable());
    QCOMPARE(m_dialog->dialogResult(), FluentDialogResult::None);
}

void FluentContentDialogTest::testTitleConstructor() {
    auto* dialog = new FluentContentDialog("Test Title", m_testWidget);
    QCOMPARE(dialog->title(), "Test Title");
    delete dialog;
}

void FluentContentDialogTest::testTitle() {
    QSignalSpy titleSpy(m_dialog, &FluentContentDialog::titleChanged);
    
    m_dialog->setTitle("Dialog Title");
    QCOMPARE(m_dialog->title(), "Dialog Title");
    QCOMPARE(titleSpy.count(), 1);
    QCOMPARE(titleSpy.first().first().toString(), "Dialog Title");
    
    // Test empty title
    m_dialog->setTitle("");
    QCOMPARE(m_dialog->title(), "");
    QCOMPARE(titleSpy.count(), 2);
}

void FluentContentDialogTest::testContent() {
    QSignalSpy contentSpy(m_dialog, &FluentContentDialog::contentChanged);
    
    m_dialog->setContent("This is the dialog content.");
    QCOMPARE(m_dialog->content(), "This is the dialog content.");
    QCOMPARE(contentSpy.count(), 1);
    
    // Test HTML content
    m_dialog->setContent("<b>Bold</b> and <i>italic</i> text");
    QCOMPARE(m_dialog->content(), "<b>Bold</b> and <i>italic</i> text");
    QCOMPARE(contentSpy.count(), 2);
}

void FluentContentDialogTest::testContentWidget() {
    QSignalSpy contentWidgetSpy(m_dialog, &FluentContentDialog::contentWidgetChanged);
    
    auto* customWidget = new QWidget();
    m_dialog->setContentWidget(customWidget);
    QCOMPARE(m_dialog->contentWidget(), customWidget);
    QCOMPARE(contentWidgetSpy.count(), 1);
    
    // Test setting null widget
    m_dialog->setContentWidget(nullptr);
    QCOMPARE(m_dialog->contentWidget(), nullptr);
    QCOMPARE(contentWidgetSpy.count(), 2);
}

void FluentContentDialogTest::testPrimaryButton() {
    QSignalSpy primaryButtonSpy(m_dialog, &FluentContentDialog::primaryButtonChanged);
    
    m_dialog->setPrimaryButtonText("OK");
    QCOMPARE(m_dialog->primaryButtonText(), "OK");
    QVERIFY(m_dialog->isPrimaryButtonVisible());
    QCOMPARE(primaryButtonSpy.count(), 1);
    
    m_dialog->setPrimaryButtonVisible(false);
    QVERIFY(!m_dialog->isPrimaryButtonVisible());
    QCOMPARE(primaryButtonSpy.count(), 2);
    
    m_dialog->setPrimaryButtonEnabled(false);
    QVERIFY(!m_dialog->isPrimaryButtonEnabled());
    QCOMPARE(primaryButtonSpy.count(), 3);
}

void FluentContentDialogTest::testSecondaryButton() {
    QSignalSpy secondaryButtonSpy(m_dialog, &FluentContentDialog::secondaryButtonChanged);
    
    m_dialog->setSecondaryButtonText("Cancel");
    QCOMPARE(m_dialog->secondaryButtonText(), "Cancel");
    QVERIFY(m_dialog->isSecondaryButtonVisible());
    QCOMPARE(secondaryButtonSpy.count(), 1);
    
    m_dialog->setSecondaryButtonVisible(false);
    QVERIFY(!m_dialog->isSecondaryButtonVisible());
    QCOMPARE(secondaryButtonSpy.count(), 2);
    
    m_dialog->setSecondaryButtonEnabled(false);
    QVERIFY(!m_dialog->isSecondaryButtonEnabled());
    QCOMPARE(secondaryButtonSpy.count(), 3);
}

void FluentContentDialogTest::testCloseButton() {
    QSignalSpy closeButtonSpy(m_dialog, &FluentContentDialog::closeButtonChanged);
    
    m_dialog->setCloseButtonText("Close");
    QCOMPARE(m_dialog->closeButtonText(), "Close");
    QVERIFY(m_dialog->isCloseButtonVisible());
    QCOMPARE(closeButtonSpy.count(), 1);
    
    m_dialog->setCloseButtonVisible(false);
    QVERIFY(!m_dialog->isCloseButtonVisible());
    QCOMPARE(closeButtonSpy.count(), 2);
}

void FluentContentDialogTest::testButtonText() {
    // Test setting all button texts
    m_dialog->setPrimaryButtonText("Accept");
    m_dialog->setSecondaryButtonText("Decline");
    m_dialog->setCloseButtonText("Exit");
    
    QCOMPARE(m_dialog->primaryButtonText(), "Accept");
    QCOMPARE(m_dialog->secondaryButtonText(), "Decline");
    QCOMPARE(m_dialog->closeButtonText(), "Exit");
}

void FluentContentDialogTest::testModal() {
    QSignalSpy modalSpy(m_dialog, &FluentContentDialog::modalChanged);
    
    m_dialog->setModal(false);
    QVERIFY(!m_dialog->isModal());
    QCOMPARE(modalSpy.count(), 1);
    
    m_dialog->setModal(true);
    QVERIFY(m_dialog->isModal());
    QCOMPARE(modalSpy.count(), 2);
}

void FluentContentDialogTest::testClosable() {
    QSignalSpy closableSpy(m_dialog, &FluentContentDialog::closableChanged);
    
    m_dialog->setClosable(false);
    QVERIFY(!m_dialog->isClosable());
    QCOMPARE(closableSpy.count(), 1);
    
    m_dialog->setClosable(true);
    QVERIFY(m_dialog->isClosable());
    QCOMPARE(closableSpy.count(), 2);
}

void FluentContentDialogTest::testResizable() {
    QSignalSpy resizableSpy(m_dialog, &FluentContentDialog::resizableChanged);
    
    m_dialog->setResizable(true);
    QVERIFY(m_dialog->isResizable());
    QCOMPARE(resizableSpy.count(), 1);
    
    m_dialog->setResizable(false);
    QVERIFY(!m_dialog->isResizable());
    QCOMPARE(resizableSpy.count(), 2);
}

void FluentContentDialogTest::testDefaultButton() {
    QSignalSpy defaultButtonSpy(m_dialog, &FluentContentDialog::defaultButtonChanged);
    
    m_dialog->setDefaultButton(FluentDialogButton::Primary);
    QCOMPARE(m_dialog->defaultButton(), FluentDialogButton::Primary);
    QCOMPARE(defaultButtonSpy.count(), 1);
    
    m_dialog->setDefaultButton(FluentDialogButton::Secondary);
    QCOMPARE(m_dialog->defaultButton(), FluentDialogButton::Secondary);
    QCOMPARE(defaultButtonSpy.count(), 2);
    
    m_dialog->setDefaultButton(FluentDialogButton::Close);
    QCOMPARE(m_dialog->defaultButton(), FluentDialogButton::Close);
    QCOMPARE(defaultButtonSpy.count(), 3);
}

void FluentContentDialogTest::testDialogResult() {
    QSignalSpy resultSpy(m_dialog, &FluentContentDialog::dialogResultChanged);
    
    m_dialog->setDialogResult(FluentDialogResult::Primary);
    QCOMPARE(m_dialog->dialogResult(), FluentDialogResult::Primary);
    QCOMPARE(resultSpy.count(), 1);
    
    m_dialog->setDialogResult(FluentDialogResult::Secondary);
    QCOMPARE(m_dialog->dialogResult(), FluentDialogResult::Secondary);
    QCOMPARE(resultSpy.count(), 2);
    
    m_dialog->setDialogResult(FluentDialogResult::Close);
    QCOMPARE(m_dialog->dialogResult(), FluentDialogResult::Close);
    QCOMPARE(resultSpy.count(), 3);
}

void FluentContentDialogTest::testAccept() {
    QSignalSpy acceptedSpy(m_dialog, &FluentContentDialog::accepted);
    QSignalSpy finishedSpy(m_dialog, &FluentContentDialog::finished);
    
    m_dialog->accept();
    QCOMPARE(m_dialog->dialogResult(), FluentDialogResult::Primary);
    QCOMPARE(acceptedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
}

void FluentContentDialogTest::testReject() {
    QSignalSpy rejectedSpy(m_dialog, &FluentContentDialog::rejected);
    QSignalSpy finishedSpy(m_dialog, &FluentContentDialog::finished);
    
    m_dialog->reject();
    QCOMPARE(m_dialog->dialogResult(), FluentDialogResult::Secondary);
    QCOMPARE(rejectedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
}

void FluentContentDialogTest::testButtonClicks() {
    QSignalSpy primaryClickedSpy(m_dialog, &FluentContentDialog::primaryButtonClicked);
    QSignalSpy secondaryClickedSpy(m_dialog, &FluentContentDialog::secondaryButtonClicked);
    QSignalSpy closeClickedSpy(m_dialog, &FluentContentDialog::closeButtonClicked);
    
    // Simulate button clicks
    m_dialog->clickPrimaryButton();
    QCOMPARE(primaryClickedSpy.count(), 1);
    
    m_dialog->clickSecondaryButton();
    QCOMPARE(secondaryClickedSpy.count(), 1);
    
    m_dialog->clickCloseButton();
    QCOMPARE(closeClickedSpy.count(), 1);
}

void FluentContentDialogTest::testKeyboardInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Show dialog for keyboard testing
    m_dialog->show();
    QTest::qWaitForWindowExposed(m_dialog);
    
    QSignalSpy acceptedSpy(m_dialog, &FluentContentDialog::accepted);
    QSignalSpy rejectedSpy(m_dialog, &FluentContentDialog::rejected);
    
    // Test Enter key (should trigger default button)
    m_dialog->setDefaultButton(FluentDialogButton::Primary);
    QTest::keyClick(m_dialog, Qt::Key_Return);
    
    // Test Tab navigation between buttons
    QTest::keyClick(m_dialog, Qt::Key_Tab);
    QTest::keyClick(m_dialog, Qt::Key_Tab);
    
    m_dialog->hide();
}

void FluentContentDialogTest::testEscapeKey() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_dialog->show();
    QTest::qWaitForWindowExposed(m_dialog);
    
    QSignalSpy rejectedSpy(m_dialog, &FluentContentDialog::rejected);
    QSignalSpy hiddenSpy(m_dialog, &FluentContentDialog::hidden);
    
    // Test Escape key (should close dialog if closable)
    m_dialog->setClosable(true);
    QTest::keyClick(m_dialog, Qt::Key_Escape);
    
    QCOMPARE(rejectedSpy.count(), 1);
    
    // Test Escape key when not closable
    m_dialog->setClosable(false);
    m_dialog->show();
    QTest::keyClick(m_dialog, Qt::Key_Escape);
    // Should not close when not closable
    
    m_dialog->hide();
}

void FluentContentDialogTest::testSignalEmission() {
    QSignalSpy titleSpy(m_dialog, &FluentContentDialog::titleChanged);
    QSignalSpy contentSpy(m_dialog, &FluentContentDialog::contentChanged);
    QSignalSpy contentWidgetSpy(m_dialog, &FluentContentDialog::contentWidgetChanged);
    QSignalSpy primaryButtonSpy(m_dialog, &FluentContentDialog::primaryButtonChanged);
    QSignalSpy secondaryButtonSpy(m_dialog, &FluentContentDialog::secondaryButtonChanged);
    QSignalSpy closeButtonSpy(m_dialog, &FluentContentDialog::closeButtonChanged);
    QSignalSpy modalSpy(m_dialog, &FluentContentDialog::modalChanged);
    QSignalSpy closableSpy(m_dialog, &FluentContentDialog::closableChanged);
    QSignalSpy resizableSpy(m_dialog, &FluentContentDialog::resizableChanged);
    QSignalSpy defaultButtonSpy(m_dialog, &FluentContentDialog::defaultButtonChanged);
    QSignalSpy resultSpy(m_dialog, &FluentContentDialog::dialogResultChanged);
    
    // Test all signals
    m_dialog->setTitle("Test");
    m_dialog->setContent("Content");
    m_dialog->setContentWidget(new QWidget());
    m_dialog->setPrimaryButtonText("OK");
    m_dialog->setSecondaryButtonText("Cancel");
    m_dialog->setCloseButtonText("Close");
    m_dialog->setModal(false);
    m_dialog->setClosable(false);
    m_dialog->setResizable(true);
    m_dialog->setDefaultButton(FluentDialogButton::Primary);
    m_dialog->setDialogResult(FluentDialogResult::Primary);
    
    QCOMPARE(titleSpy.count(), 1);
    QCOMPARE(contentSpy.count(), 1);
    QCOMPARE(contentWidgetSpy.count(), 1);
    QCOMPARE(primaryButtonSpy.count(), 1);
    QCOMPARE(secondaryButtonSpy.count(), 1);
    QCOMPARE(closeButtonSpy.count(), 1);
    QCOMPARE(modalSpy.count(), 1);
    QCOMPARE(closableSpy.count(), 1);
    QCOMPARE(resizableSpy.count(), 1);
    QCOMPARE(defaultButtonSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);
}

void FluentContentDialogTest::testShowDialog() {
    QSignalSpy shownSpy(m_dialog, &FluentContentDialog::shown);
    QSignalSpy visibilityChangedSpy(m_dialog, &FluentContentDialog::visibilityChanged);
    
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_dialog->show();
    QTest::qWaitForWindowExposed(m_dialog);
    
    QVERIFY(m_dialog->isVisible());
    QCOMPARE(shownSpy.count(), 1);
    QCOMPARE(visibilityChangedSpy.count(), 1);
    
    m_dialog->hide();
}

void FluentContentDialogTest::testHideDialog() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_dialog->show();
    QTest::qWaitForWindowExposed(m_dialog);
    
    QSignalSpy hiddenSpy(m_dialog, &FluentContentDialog::hidden);
    QSignalSpy visibilityChangedSpy(m_dialog, &FluentContentDialog::visibilityChanged);
    
    m_dialog->hide();
    
    QVERIFY(!m_dialog->isVisible());
    QCOMPARE(hiddenSpy.count(), 1);
    QCOMPARE(visibilityChangedSpy.count(), 1);
}

QTEST_MAIN(FluentContentDialogTest)
#include "FluentContentDialogTest.moc"
