// tests/Components/FluentCheckBoxTest.cpp
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentCheckBoxTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTextConstructor();

    // Content tests
    void testText();
    void testIcon();

    // State tests
    void testCheckState();
    void testChecked();
    void testTristate();
    void testEnabled();

    // Appearance tests
    void testCheckBoxStyle();
    void testCheckBoxSize();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardInteraction();
    void testToggle();

    // Animation tests
    void testAnimations();

    // Signal tests
    void testSignalEmission();

    // Validation tests
    void testValidation();

private:
    FluentCheckBox* m_checkBox{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentCheckBoxTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentCheckBoxTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentCheckBoxTest::init() {
    m_checkBox = new FluentCheckBox(m_testWidget);
}

void FluentCheckBoxTest::cleanup() {
    delete m_checkBox;
    m_checkBox = nullptr;
}

void FluentCheckBoxTest::testDefaultConstructor() {
    QVERIFY(m_checkBox != nullptr);
    QCOMPARE(m_checkBox->text(), QString());
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Unchecked);
    QVERIFY(!m_checkBox->isChecked());
    QVERIFY(!m_checkBox->isTristate());
    QVERIFY(m_checkBox->isEnabled());
}

void FluentCheckBoxTest::testTextConstructor() {
    auto* checkBox = new FluentCheckBox("Test CheckBox", m_testWidget);
    QCOMPARE(checkBox->text(), "Test CheckBox");
    delete checkBox;
}

void FluentCheckBoxTest::testText() {
    QSignalSpy textSpy(m_checkBox, &FluentCheckBox::textChanged);

    m_checkBox->setText("New Text");
    QCOMPARE(m_checkBox->text(), "New Text");
    QCOMPARE(textSpy.count(), 1);
    QCOMPARE(textSpy.first().first().toString(), "New Text");

    // Test empty text
    m_checkBox->setText("");
    QCOMPARE(m_checkBox->text(), "");
    QCOMPARE(textSpy.count(), 2);
}

void FluentCheckBoxTest::testIcon() {
    // Note: FluentCheckBox doesn't have setIcon/icon methods in current
    // implementation Testing iconSize instead which is available
    QSignalSpy iconSizeSpy(m_checkBox, &FluentCheckBox::iconSizeChanged);

    QSize testSize(24, 24);
    m_checkBox->setIconSize(testSize);
    QCOMPARE(m_checkBox->iconSize(), testSize);
    QCOMPARE(iconSizeSpy.count(), 1);

    // Test different icon size
    QSize smallSize(16, 16);
    m_checkBox->setIconSize(smallSize);
    QCOMPARE(m_checkBox->iconSize(), smallSize);
    QCOMPARE(iconSizeSpy.count(), 2);
}

void FluentCheckBoxTest::testCheckState() {
    QSignalSpy checkStateSpy(m_checkBox, &FluentCheckBox::checkStateChanged);

    m_checkBox->setCheckState(FluentCheckState::Checked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Checked);
    QVERIFY(m_checkBox->isChecked());
    QCOMPARE(checkStateSpy.count(), 1);

    m_checkBox->setCheckState(FluentCheckState::PartiallyChecked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::PartiallyChecked);
    QVERIFY(!m_checkBox->isChecked());
    QCOMPARE(checkStateSpy.count(), 2);

    m_checkBox->setCheckState(FluentCheckState::Unchecked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Unchecked);
    QVERIFY(!m_checkBox->isChecked());
    QCOMPARE(checkStateSpy.count(), 3);
}

void FluentCheckBoxTest::testChecked() {
    // Note: FluentCheckBox doesn't have checkedChanged signal, using toggled
    // instead
    QSignalSpy toggledSpy(m_checkBox, &FluentCheckBox::toggled);

    m_checkBox->setChecked(true);
    QVERIFY(m_checkBox->isChecked());
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Checked);
    QCOMPARE(toggledSpy.count(), 1);

    m_checkBox->setChecked(false);
    QVERIFY(!m_checkBox->isChecked());
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Unchecked);
    QCOMPARE(toggledSpy.count(), 2);
}

void FluentCheckBoxTest::testTristate() {
    QSignalSpy tristateSpy(m_checkBox, &FluentCheckBox::tristateChanged);

    m_checkBox->setTristate(true);
    QVERIFY(m_checkBox->isTristate());
    QCOMPARE(tristateSpy.count(), 1);

    // Test that tristate allows PartiallyChecked
    m_checkBox->setCheckState(FluentCheckState::PartiallyChecked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::PartiallyChecked);

    // Disable tristate
    m_checkBox->setTristate(false);
    QVERIFY(!m_checkBox->isTristate());
    QCOMPARE(tristateSpy.count(), 2);

    // PartiallyChecked should be converted to Unchecked when tristate is
    // disabled
    m_checkBox->setCheckState(FluentCheckState::PartiallyChecked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Unchecked);
}

void FluentCheckBoxTest::testEnabled() {
    QSignalSpy enabledSpy(m_checkBox, &FluentCheckBox::enabledChanged);

    m_checkBox->setEnabled(false);
    QVERIFY(!m_checkBox->isEnabled());
    QCOMPARE(enabledSpy.count(), 1);

    m_checkBox->setEnabled(true);
    QVERIFY(m_checkBox->isEnabled());
    QCOMPARE(enabledSpy.count(), 2);
}

void FluentCheckBoxTest::testCheckBoxStyle() {
    QSignalSpy styleSpy(m_checkBox, &FluentCheckBox::checkBoxStyleChanged);

    m_checkBox->setCheckBoxStyle(FluentCheckBoxStyle::Switch);
    QCOMPARE(m_checkBox->checkBoxStyle(), FluentCheckBoxStyle::Switch);
    QCOMPARE(styleSpy.count(), 1);

    m_checkBox->setCheckBoxStyle(FluentCheckBoxStyle::Standard);
    QCOMPARE(m_checkBox->checkBoxStyle(), FluentCheckBoxStyle::Standard);
    QCOMPARE(styleSpy.count(), 2);
}

void FluentCheckBoxTest::testCheckBoxSize() {
    QSignalSpy sizeSpy(m_checkBox, &FluentCheckBox::checkBoxSizeChanged);

    m_checkBox->setCheckBoxSize(FluentCheckBoxSize::Large);
    QCOMPARE(m_checkBox->checkBoxSize(), FluentCheckBoxSize::Large);
    QCOMPARE(sizeSpy.count(), 1);

    m_checkBox->setCheckBoxSize(FluentCheckBoxSize::Small);
    QCOMPARE(m_checkBox->checkBoxSize(), FluentCheckBoxSize::Small);
    QCOMPARE(sizeSpy.count(), 2);
}

void FluentCheckBoxTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);

    QSignalSpy clickedSpy(m_checkBox, &FluentCheckBox::clicked);
    QSignalSpy toggledSpy(m_checkBox, &FluentCheckBox::toggled);
    QSignalSpy checkedSpy(m_checkBox, &FluentCheckBox::checkedChanged);

    // Test mouse click
    QVERIFY(!m_checkBox->isChecked());
    QTest::mouseClick(m_checkBox, Qt::LeftButton);

    QVERIFY(m_checkBox->isChecked());
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(toggledSpy.count(), 1);
    QCOMPARE(checkedSpy.count(), 1);

    // Click again to uncheck
    QTest::mouseClick(m_checkBox, Qt::LeftButton);

    QVERIFY(!m_checkBox->isChecked());
    QCOMPARE(clickedSpy.count(), 2);
    QCOMPARE(toggledSpy.count(), 2);
    QCOMPARE(checkedSpy.count(), 2);
}

void FluentCheckBoxTest::testKeyboardInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_checkBox->setFocus();

    QSignalSpy clickedSpy(m_checkBox, &FluentCheckBox::clicked);
    QSignalSpy toggledSpy(m_checkBox, &FluentCheckBox::toggled);

    // Test Space key
    QVERIFY(!m_checkBox->isChecked());
    QTest::keyClick(m_checkBox, Qt::Key_Space);

    QVERIFY(m_checkBox->isChecked());
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(toggledSpy.count(), 1);

    // Test Enter key
    QTest::keyClick(m_checkBox, Qt::Key_Return);

    QVERIFY(!m_checkBox->isChecked());
    QCOMPARE(clickedSpy.count(), 2);
    QCOMPARE(toggledSpy.count(), 2);
}

void FluentCheckBoxTest::testToggle() {
    QSignalSpy toggledSpy(m_checkBox, &FluentCheckBox::toggled);
    QSignalSpy checkedSpy(m_checkBox, &FluentCheckBox::checkedChanged);

    // Test toggle method
    QVERIFY(!m_checkBox->isChecked());
    m_checkBox->toggle();

    QVERIFY(m_checkBox->isChecked());
    QCOMPARE(toggledSpy.count(), 1);
    QCOMPARE(checkedSpy.count(), 1);

    m_checkBox->toggle();

    QVERIFY(!m_checkBox->isChecked());
    QCOMPARE(toggledSpy.count(), 2);
    QCOMPARE(checkedSpy.count(), 2);

    // Test toggle with tristate
    m_checkBox->setTristate(true);
    m_checkBox->setCheckState(FluentCheckState::PartiallyChecked);
    m_checkBox->toggle();

    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Checked);
    QCOMPARE(toggledSpy.count(), 3);
}

void FluentCheckBoxTest::testAnimations() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);

    // Test that animations don't crash
    m_checkBox->setAnimationEnabled(true);
    QVERIFY(m_checkBox->animationEnabled());

    // Trigger state changes that should animate
    m_checkBox->setChecked(true);
    QTest::qWait(100);

    m_checkBox->setChecked(false);
    QTest::qWait(100);

    // Test hover animations
    QTest::mouseMove(m_checkBox);
    QTest::qWait(50);

    // Test focus animations
    m_checkBox->setFocus();
    QTest::qWait(50);

    // Disable animations
    m_checkBox->setAnimationEnabled(false);
    QVERIFY(!m_checkBox->animationEnabled());
}

void FluentCheckBoxTest::testSignalEmission() {
    QSignalSpy textSpy(m_checkBox, &FluentCheckBox::textChanged);
    QSignalSpy iconSpy(m_checkBox, &FluentCheckBox::iconChanged);
    QSignalSpy checkStateSpy(m_checkBox, &FluentCheckBox::checkStateChanged);
    QSignalSpy checkedSpy(m_checkBox, &FluentCheckBox::checkedChanged);
    QSignalSpy toggledSpy(m_checkBox, &FluentCheckBox::toggled);
    QSignalSpy clickedSpy(m_checkBox, &FluentCheckBox::clicked);
    QSignalSpy tristateSpy(m_checkBox, &FluentCheckBox::tristateChanged);
    QSignalSpy enabledSpy(m_checkBox, &FluentCheckBox::enabledChanged);
    QSignalSpy styleSpy(m_checkBox, &FluentCheckBox::checkBoxStyleChanged);
    QSignalSpy sizeSpy(m_checkBox, &FluentCheckBox::checkBoxSizeChanged);

    // Test all signals
    m_checkBox->setText("Test");
    m_checkBox->setIcon(QIcon());
    m_checkBox->setCheckState(FluentCheckState::Checked);
    m_checkBox->setChecked(false);
    m_checkBox->toggle();
    m_checkBox->click();
    m_checkBox->setTristate(true);
    m_checkBox->setEnabled(false);
    m_checkBox->setCheckBoxStyle(FluentCheckBoxStyle::Switch);
    m_checkBox->setCheckBoxSize(FluentCheckBoxSize::Large);

    QCOMPARE(textSpy.count(), 1);
    QCOMPARE(iconSpy.count(), 1);
    QCOMPARE(checkStateSpy.count(), 3);  // setCheckState, setChecked, toggle
    QCOMPARE(checkedSpy.count(), 2);     // setChecked, toggle
    QCOMPARE(toggledSpy.count(), 2);     // toggle, click
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(tristateSpy.count(), 1);
    QCOMPARE(enabledSpy.count(), 1);
    QCOMPARE(styleSpy.count(), 1);
    QCOMPARE(sizeSpy.count(), 1);
}

void FluentCheckBoxTest::testValidation() {
    // Test validation functionality if available
    m_checkBox->setRequired(true);
    QVERIFY(m_checkBox->isRequired());

    // Unchecked required checkbox should be invalid
    m_checkBox->setChecked(false);
    QVERIFY(!m_checkBox->isValid());

    // Checked required checkbox should be valid
    m_checkBox->setChecked(true);
    QVERIFY(m_checkBox->isValid());

    // Non-required checkbox should always be valid
    m_checkBox->setRequired(false);
    m_checkBox->setChecked(false);
    QVERIFY(m_checkBox->isValid());

    // Test validation state change signal
    QSignalSpy validationSpy(m_checkBox,
                             &FluentCheckBox::validationStateChanged);
    m_checkBox->setRequired(true);
    m_checkBox->setChecked(false);
    m_checkBox->setChecked(true);

    QVERIFY(validationSpy.count() >= 1);
}

QTEST_MAIN(FluentCheckBoxTest)
#include "FluentCheckBoxTest.moc"
