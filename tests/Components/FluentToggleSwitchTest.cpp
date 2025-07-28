// tests/Components/FluentToggleSwitchTest.cpp
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QMouseEvent>
#include <QKeyEvent>
#include "FluentQt/Components/FluentToggleSwitch.h"

using namespace FluentQt::Components;

class FluentToggleSwitchTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testDefaultConstruction();
    void testCheckedState();
    void testText();
    void testSize();
    void testLabelPosition();

    // State text and icons tests
    void testStateText();
    void testStateIcons();
    void testShowStateText();
    void testShowIcons();

    // Animation tests
    void testAnimated();
    void testAnimationDuration();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardInteraction();
    void testToggleMethod();

    // Signal tests
    void testCheckedChangedSignal();
    void testClickedSignal();
    void testToggledSignal();

    // Visual tests
    void testEnabled();
    void testFocus();

private:
    FluentToggleSwitch* m_toggle{nullptr};
};

void FluentToggleSwitchTest::initTestCase() {
    // Global test setup
}

void FluentToggleSwitchTest::cleanupTestCase() {
    // Global test cleanup
}

void FluentToggleSwitchTest::init() {
    m_toggle = new FluentToggleSwitch();
}

void FluentToggleSwitchTest::cleanup() {
    delete m_toggle;
    m_toggle = nullptr;
}

void FluentToggleSwitchTest::testDefaultConstruction() {
    QVERIFY(!m_toggle->isChecked());
    QCOMPARE(m_toggle->text(), QString());
    QCOMPARE(m_toggle->size(), FluentToggleSwitchSize::Medium);
    QCOMPARE(m_toggle->labelPosition(), FluentToggleSwitchLabelPosition::Right);
    QVERIFY(!m_toggle->showStateText());
    QVERIFY(!m_toggle->showIcons());
    QVERIFY(m_toggle->isAnimated());
    QCOMPARE(m_toggle->animationDuration(), 200);
    QVERIFY(m_toggle->isEnabled());
}

void FluentToggleSwitchTest::testCheckedState() {
    QSignalSpy spy(m_toggle, &FluentToggleSwitch::checkedChanged);
    
    // Test setting checked state
    m_toggle->setChecked(true);
    QVERIFY(m_toggle->isChecked());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    
    // Test setting same state (should not emit signal)
    m_toggle->setChecked(true);
    QCOMPARE(spy.count(), 1);
    
    // Test unchecking
    m_toggle->setChecked(false);
    QVERIFY(!m_toggle->isChecked());
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void FluentToggleSwitchTest::testText() {
    const QString testText = "Enable notifications";
    m_toggle->setText(testText);
    QCOMPARE(m_toggle->text(), testText);
}

void FluentToggleSwitchTest::testSize() {
    // Test small size
    m_toggle->setSize(FluentToggleSwitchSize::Small);
    QCOMPARE(m_toggle->size(), FluentToggleSwitchSize::Small);
    
    // Test medium size
    m_toggle->setSize(FluentToggleSwitchSize::Medium);
    QCOMPARE(m_toggle->size(), FluentToggleSwitchSize::Medium);
    
    // Test large size
    m_toggle->setSize(FluentToggleSwitchSize::Large);
    QCOMPARE(m_toggle->size(), FluentToggleSwitchSize::Large);
}

void FluentToggleSwitchTest::testLabelPosition() {
    // Test right position
    m_toggle->setLabelPosition(FluentToggleSwitchLabelPosition::Right);
    QCOMPARE(m_toggle->labelPosition(), FluentToggleSwitchLabelPosition::Right);
    
    // Test left position
    m_toggle->setLabelPosition(FluentToggleSwitchLabelPosition::Left);
    QCOMPARE(m_toggle->labelPosition(), FluentToggleSwitchLabelPosition::Left);
    
    // Test above position
    m_toggle->setLabelPosition(FluentToggleSwitchLabelPosition::Above);
    QCOMPARE(m_toggle->labelPosition(), FluentToggleSwitchLabelPosition::Above);
    
    // Test below position
    m_toggle->setLabelPosition(FluentToggleSwitchLabelPosition::Below);
    QCOMPARE(m_toggle->labelPosition(), FluentToggleSwitchLabelPosition::Below);
    
    // Test none position
    m_toggle->setLabelPosition(FluentToggleSwitchLabelPosition::None);
    QCOMPARE(m_toggle->labelPosition(), FluentToggleSwitchLabelPosition::None);
}

void FluentToggleSwitchTest::testStateText() {
    const QString onText = "On";
    const QString offText = "Off";
    
    m_toggle->setOnText(onText);
    m_toggle->setOffText(offText);
    
    QCOMPARE(m_toggle->onText(), onText);
    QCOMPARE(m_toggle->offText(), offText);
    
    // Test current state text
    m_toggle->setChecked(true);
    QCOMPARE(m_toggle->currentStateText(), onText);
    
    m_toggle->setChecked(false);
    QCOMPARE(m_toggle->currentStateText(), offText);
}

void FluentToggleSwitchTest::testStateIcons() {
    QIcon onIcon(":/icons/on.png");
    QIcon offIcon(":/icons/off.png");
    
    m_toggle->setOnIcon(onIcon);
    m_toggle->setOffIcon(offIcon);
    
    QCOMPARE(m_toggle->onIcon().cacheKey(), onIcon.cacheKey());
    QCOMPARE(m_toggle->offIcon().cacheKey(), offIcon.cacheKey());
    
    // Test current icon
    m_toggle->setChecked(true);
    QCOMPARE(m_toggle->currentIcon().cacheKey(), onIcon.cacheKey());
    
    m_toggle->setChecked(false);
    QCOMPARE(m_toggle->currentIcon().cacheKey(), offIcon.cacheKey());
}

void FluentToggleSwitchTest::testShowStateText() {
    m_toggle->setShowStateText(true);
    QVERIFY(m_toggle->showStateText());
    
    m_toggle->setShowStateText(false);
    QVERIFY(!m_toggle->showStateText());
}

void FluentToggleSwitchTest::testShowIcons() {
    m_toggle->setShowIcons(true);
    QVERIFY(m_toggle->showIcons());
    
    m_toggle->setShowIcons(false);
    QVERIFY(!m_toggle->showIcons());
}

void FluentToggleSwitchTest::testAnimated() {
    m_toggle->setAnimated(true);
    QVERIFY(m_toggle->isAnimated());
    
    m_toggle->setAnimated(false);
    QVERIFY(!m_toggle->isAnimated());
}

void FluentToggleSwitchTest::testAnimationDuration() {
    const int duration = 500;
    m_toggle->setAnimationDuration(duration);
    QCOMPARE(m_toggle->animationDuration(), duration);
}

void FluentToggleSwitchTest::testMouseInteraction() {
    m_toggle->resize(100, 50);
    QSignalSpy clickedSpy(m_toggle, &FluentToggleSwitch::clicked);
    QSignalSpy checkedSpy(m_toggle, &FluentToggleSwitch::checkedChanged);
    
    // Simulate mouse click on switch area
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(30, 25), 
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_toggle, &pressEvent);
    
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(30, 25), 
                            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_toggle, &releaseEvent);
    
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(checkedSpy.count(), 1);
    QVERIFY(m_toggle->isChecked());
}

void FluentToggleSwitchTest::testKeyboardInteraction() {
    m_toggle->setFocus();
    QSignalSpy checkedSpy(m_toggle, &FluentToggleSwitch::checkedChanged);
    
    // Test space key
    QKeyEvent spaceEvent(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    QApplication::sendEvent(m_toggle, &spaceEvent);
    
    QCOMPARE(checkedSpy.count(), 1);
    QVERIFY(m_toggle->isChecked());
    
    // Test return key
    QKeyEvent returnEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_toggle, &returnEvent);
    
    QCOMPARE(checkedSpy.count(), 2);
    QVERIFY(!m_toggle->isChecked());
    
    // Test enter key
    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    QApplication::sendEvent(m_toggle, &enterEvent);
    
    QCOMPARE(checkedSpy.count(), 3);
    QVERIFY(m_toggle->isChecked());
}

void FluentToggleSwitchTest::testToggleMethod() {
    QSignalSpy spy(m_toggle, &FluentToggleSwitch::toggled);
    
    // Test toggle from unchecked
    QVERIFY(!m_toggle->isChecked());
    m_toggle->toggle();
    QVERIFY(m_toggle->isChecked());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    
    // Test toggle from checked
    m_toggle->toggle();
    QVERIFY(!m_toggle->isChecked());
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void FluentToggleSwitchTest::testCheckedChangedSignal() {
    QSignalSpy spy(m_toggle, &FluentToggleSwitch::checkedChanged);
    
    m_toggle->setChecked(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    
    m_toggle->setChecked(false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
    
    // Setting same value should not emit signal
    m_toggle->setChecked(false);
    QCOMPARE(spy.count(), 2);
}

void FluentToggleSwitchTest::testClickedSignal() {
    QSignalSpy spy(m_toggle, &FluentToggleSwitch::clicked);
    
    // Simulate click
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(30, 25), 
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_toggle, &pressEvent);
    
    QCOMPARE(spy.count(), 1);
}

void FluentToggleSwitchTest::testToggledSignal() {
    QSignalSpy spy(m_toggle, &FluentToggleSwitch::toggled);
    
    m_toggle->toggle();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    
    m_toggle->toggle();
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void FluentToggleSwitchTest::testEnabled() {
    QVERIFY(m_toggle->isEnabled());
    
    m_toggle->setEnabled(false);
    QVERIFY(!m_toggle->isEnabled());
    
    // Disabled toggle should not respond to clicks
    QSignalSpy spy(m_toggle, &FluentToggleSwitch::checkedChanged);
    
    QMouseEvent clickEvent(QEvent::MouseButtonPress, QPoint(30, 25), 
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_toggle, &clickEvent);
    
    QCOMPARE(spy.count(), 0);
    QVERIFY(!m_toggle->isChecked());
}

void FluentToggleSwitchTest::testFocus() {
    QVERIFY(!m_toggle->hasFocus());
    
    m_toggle->setFocus();
    QVERIFY(m_toggle->hasFocus());
    
    m_toggle->clearFocus();
    QVERIFY(!m_toggle->hasFocus());
}

QTEST_MAIN(FluentToggleSwitchTest)
#include "FluentToggleSwitchTest.moc"
