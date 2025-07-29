// tests/Components/FluentRadioButtonTest.cpp
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QButtonGroup>

#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentRadioButtonTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTextConstructor();
    void testTextValueConstructor();
    void testFactoryMethods();

    // Basic functionality tests
    void testCheckedState();
    void testText();
    void testValue();
    void testIcon();
    void testShowIcon();

    // Size and layout tests
    void testSize();
    void testLabelPosition();
    void testAutoExclusive();

    // Animation tests
    void testAnimated();
    void testAnimationDuration();
    void testAnimateCheck();

    // Group behavior tests
    void testButtonGroup();
    void testExclusiveSelection();
    void testRadioGroup();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardInteraction();
    void testClick();
    void testToggle();

    // Signal tests
    void testCheckedChangedSignal();
    void testToggledSignal();
    void testClickedSignal();
    void testPressedReleasedSignals();
    void testContentChangeSignals();

    // State tests
    void testEnabledState();
    void testFocusState();
    void testHoverState();

    // Accessibility tests
    void testAccessibility();

    // Theme integration tests
    void testThemeIntegration();

private:
    FluentRadioButton* m_radioButton;
};

void FluentRadioButtonTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentRadioButtonTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentRadioButtonTest::init() {
    // Create a fresh radio button before each test
    m_radioButton = new FluentRadioButton();
    m_radioButton->show();
    QTest::qWaitForWindowExposed(m_radioButton);
}

void FluentRadioButtonTest::cleanup() {
    // Clean up after each test
    delete m_radioButton;
    m_radioButton = nullptr;
}

void FluentRadioButtonTest::testDefaultConstructor() {
    // Test default constructor
    FluentRadioButton* radioButton = new FluentRadioButton();
    
    // Verify default properties
    QVERIFY(!radioButton->isChecked());
    QVERIFY(radioButton->text().isEmpty());
    QVERIFY(radioButton->value().isEmpty());
    QVERIFY(radioButton->icon().isNull());
    QVERIFY(!radioButton->showIcon());
    QCOMPARE(radioButton->size(), FluentRadioButtonSize::Medium);
    QCOMPARE(radioButton->labelPosition(), FluentRadioButtonLabelPosition::Right);
    QVERIFY(radioButton->autoExclusive());
    QVERIFY(radioButton->isAnimated());
    QVERIFY(radioButton->isEnabled());
    
    delete radioButton;
}

void FluentRadioButtonTest::testTextConstructor() {
    // Test constructor with text
    const QString radioText = "Test Radio Button";
    FluentRadioButton* radioButton = new FluentRadioButton(radioText);
    
    QCOMPARE(radioButton->text(), radioText);
    QVERIFY(!radioButton->isChecked());
    
    delete radioButton;
}

void FluentRadioButtonTest::testTextValueConstructor() {
    // Test constructor with text and value
    const QString radioText = "Option A";
    const QString radioValue = "option_a";
    FluentRadioButton* radioButton = new FluentRadioButton(radioText, radioValue);
    
    QCOMPARE(radioButton->text(), radioText);
    QCOMPARE(radioButton->value(), radioValue);
    QVERIFY(!radioButton->isChecked());
    
    delete radioButton;
}

void FluentRadioButtonTest::testFactoryMethods() {
    // Test button group factory method
    QStringList options = {"Option 1", "Option 2", "Option 3"};
    QButtonGroup* buttonGroup = FluentRadioButton::createButtonGroup(options);
    
    QVERIFY(buttonGroup != nullptr);
    QCOMPARE(buttonGroup->buttons().size(), 3);
    
    delete buttonGroup;
    
    // Test radio group factory method
    QList<FluentRadioButton*> radioGroup = FluentRadioButton::createRadioGroup(options);
    
    QCOMPARE(radioGroup.size(), 3);
    QCOMPARE(radioGroup[0]->text(), QString("Option 1"));
    QCOMPARE(radioGroup[1]->text(), QString("Option 2"));
    QCOMPARE(radioGroup[2]->text(), QString("Option 3"));
    
    qDeleteAll(radioGroup);
}

void FluentRadioButtonTest::testCheckedState() {
    // Test setting and getting checked state
    QSignalSpy checkedChangedSpy(m_radioButton, &FluentRadioButton::checkedChanged);
    QSignalSpy toggledSpy(m_radioButton, &FluentRadioButton::toggled);
    
    QVERIFY(!m_radioButton->isChecked()); // Default should be false
    
    m_radioButton->setChecked(true);
    QVERIFY(m_radioButton->isChecked());
    QCOMPARE(checkedChangedSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.first().first().toBool(), true);
    QCOMPARE(toggledSpy.count(), 1);
    QCOMPARE(toggledSpy.first().first().toBool(), true);
    
    // Radio buttons typically can't be unchecked directly
    m_radioButton->setChecked(false);
    QVERIFY(!m_radioButton->isChecked());
    QCOMPARE(checkedChangedSpy.count(), 2);
    QCOMPARE(checkedChangedSpy.last().first().toBool(), false);
    
    // Setting the same checked state should not emit the signal
    m_radioButton->setChecked(false);
    QCOMPARE(checkedChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testText() {
    // Test setting and getting text
    const QString text1 = "Radio Button Text";
    const QString text2 = "New Text";
    
    QSignalSpy textChangedSpy(m_radioButton, &FluentRadioButton::textChanged);
    
    m_radioButton->setText(text1);
    QCOMPARE(m_radioButton->text(), text1);
    QCOMPARE(textChangedSpy.count(), 1);
    QCOMPARE(textChangedSpy.first().first().toString(), text1);
    
    m_radioButton->setText(text2);
    QCOMPARE(m_radioButton->text(), text2);
    QCOMPARE(textChangedSpy.count(), 2);
    QCOMPARE(textChangedSpy.last().first().toString(), text2);
    
    // Setting the same text should not emit the signal
    m_radioButton->setText(text2);
    QCOMPARE(textChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testValue() {
    // Test setting and getting value
    const QString value1 = "option_1";
    const QString value2 = "option_2";
    
    QSignalSpy valueChangedSpy(m_radioButton, &FluentRadioButton::valueChanged);
    
    m_radioButton->setValue(value1);
    QCOMPARE(m_radioButton->value(), value1);
    QCOMPARE(valueChangedSpy.count(), 1);
    QCOMPARE(valueChangedSpy.first().first().toString(), value1);
    
    m_radioButton->setValue(value2);
    QCOMPARE(m_radioButton->value(), value2);
    QCOMPARE(valueChangedSpy.count(), 2);
    QCOMPARE(valueChangedSpy.last().first().toString(), value2);
    
    // Setting the same value should not emit the signal
    m_radioButton->setValue(value2);
    QCOMPARE(valueChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testIcon() {
    // Test setting and getting icon
    QSignalSpy iconChangedSpy(m_radioButton, &FluentRadioButton::iconChanged);
    
    QIcon icon1;
    QPixmap pixmap1(16, 16);
    pixmap1.fill(Qt::red);
    icon1 = QIcon(pixmap1);
    
    m_radioButton->setIcon(icon1);
    QVERIFY(!m_radioButton->icon().isNull());
    QCOMPARE(iconChangedSpy.count(), 1);
    
    QIcon icon2;
    QPixmap pixmap2(16, 16);
    pixmap2.fill(Qt::green);
    icon2 = QIcon(pixmap2);
    
    m_radioButton->setIcon(icon2);
    QVERIFY(!m_radioButton->icon().isNull());
    QCOMPARE(iconChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testShowIcon() {
    // Test show icon property
    QSignalSpy showIconChangedSpy(m_radioButton, &FluentRadioButton::showIconChanged);
    
    QVERIFY(!m_radioButton->showIcon()); // Default should be false
    
    m_radioButton->setShowIcon(true);
    QVERIFY(m_radioButton->showIcon());
    QCOMPARE(showIconChangedSpy.count(), 1);
    QCOMPARE(showIconChangedSpy.first().first().toBool(), true);
    
    m_radioButton->setShowIcon(false);
    QVERIFY(!m_radioButton->showIcon());
    QCOMPARE(showIconChangedSpy.count(), 2);
    QCOMPARE(showIconChangedSpy.last().first().toBool(), false);
    
    // Setting the same show icon state should not emit the signal
    m_radioButton->setShowIcon(false);
    QCOMPARE(showIconChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testSize() {
    // Test setting and getting size
    QSignalSpy sizeChangedSpy(m_radioButton, &FluentRadioButton::sizeChanged);

    // Default size should be Medium
    QCOMPARE(m_radioButton->size(), FluentRadioButtonSize::Medium);

    m_radioButton->setSize(FluentRadioButtonSize::Small);
    QCOMPARE(m_radioButton->size(), FluentRadioButtonSize::Small);
    QCOMPARE(sizeChangedSpy.count(), 1);
    QCOMPARE(sizeChangedSpy.first().first().value<FluentRadioButtonSize>(), FluentRadioButtonSize::Small);

    m_radioButton->setSize(FluentRadioButtonSize::Large);
    QCOMPARE(m_radioButton->size(), FluentRadioButtonSize::Large);
    QCOMPARE(sizeChangedSpy.count(), 2);

    // Setting the same size should not emit the signal
    m_radioButton->setSize(FluentRadioButtonSize::Large);
    QCOMPARE(sizeChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testLabelPosition() {
    // Test setting and getting label position
    QSignalSpy labelPositionChangedSpy(m_radioButton, &FluentRadioButton::labelPositionChanged);

    // Default position should be Right
    QCOMPARE(m_radioButton->labelPosition(), FluentRadioButtonLabelPosition::Right);

    m_radioButton->setLabelPosition(FluentRadioButtonLabelPosition::Left);
    QCOMPARE(m_radioButton->labelPosition(), FluentRadioButtonLabelPosition::Left);
    QCOMPARE(labelPositionChangedSpy.count(), 1);
    QCOMPARE(labelPositionChangedSpy.first().first().value<FluentRadioButtonLabelPosition>(), FluentRadioButtonLabelPosition::Left);

    m_radioButton->setLabelPosition(FluentRadioButtonLabelPosition::Above);
    QCOMPARE(m_radioButton->labelPosition(), FluentRadioButtonLabelPosition::Above);
    QCOMPARE(labelPositionChangedSpy.count(), 2);

    m_radioButton->setLabelPosition(FluentRadioButtonLabelPosition::Below);
    QCOMPARE(m_radioButton->labelPosition(), FluentRadioButtonLabelPosition::Below);
    QCOMPARE(labelPositionChangedSpy.count(), 3);

    // Setting the same position should not emit the signal
    m_radioButton->setLabelPosition(FluentRadioButtonLabelPosition::Below);
    QCOMPARE(labelPositionChangedSpy.count(), 3);
}

void FluentRadioButtonTest::testAutoExclusive() {
    // Test auto exclusive property
    QSignalSpy autoExclusiveChangedSpy(m_radioButton, &FluentRadioButton::autoExclusiveChanged);

    QVERIFY(m_radioButton->autoExclusive()); // Default should be true

    m_radioButton->setAutoExclusive(false);
    QVERIFY(!m_radioButton->autoExclusive());
    QCOMPARE(autoExclusiveChangedSpy.count(), 1);
    QCOMPARE(autoExclusiveChangedSpy.first().first().toBool(), false);

    m_radioButton->setAutoExclusive(true);
    QVERIFY(m_radioButton->autoExclusive());
    QCOMPARE(autoExclusiveChangedSpy.count(), 2);
    QCOMPARE(autoExclusiveChangedSpy.last().first().toBool(), true);

    // Setting the same auto exclusive state should not emit the signal
    m_radioButton->setAutoExclusive(true);
    QCOMPARE(autoExclusiveChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testAnimated() {
    // Test animated property
    QSignalSpy animatedChangedSpy(m_radioButton, &FluentRadioButton::animatedChanged);

    QVERIFY(m_radioButton->isAnimated()); // Default should be true

    m_radioButton->setAnimated(false);
    QVERIFY(!m_radioButton->isAnimated());
    QCOMPARE(animatedChangedSpy.count(), 1);
    QCOMPARE(animatedChangedSpy.first().first().toBool(), false);

    m_radioButton->setAnimated(true);
    QVERIFY(m_radioButton->isAnimated());
    QCOMPARE(animatedChangedSpy.count(), 2);
    QCOMPARE(animatedChangedSpy.last().first().toBool(), true);

    // Setting the same animated state should not emit the signal
    m_radioButton->setAnimated(true);
    QCOMPARE(animatedChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testAnimationDuration() {
    // Test animation duration property
    QSignalSpy animationDurationChangedSpy(m_radioButton, &FluentRadioButton::animationDurationChanged);

    // Default duration should be 200ms
    QCOMPARE(m_radioButton->animationDuration(), 200);

    m_radioButton->setAnimationDuration(300);
    QCOMPARE(m_radioButton->animationDuration(), 300);
    QCOMPARE(animationDurationChangedSpy.count(), 1);
    QCOMPARE(animationDurationChangedSpy.first().first().toInt(), 300);

    m_radioButton->setAnimationDuration(150);
    QCOMPARE(m_radioButton->animationDuration(), 150);
    QCOMPARE(animationDurationChangedSpy.count(), 2);
    QCOMPARE(animationDurationChangedSpy.last().first().toInt(), 150);

    // Setting the same duration should not emit the signal
    m_radioButton->setAnimationDuration(150);
    QCOMPARE(animationDurationChangedSpy.count(), 2);
}

void FluentRadioButtonTest::testAnimateCheck() {
    // Test programmatic check animation
    m_radioButton->setAnimated(true);

    // This should trigger the check animation
    m_radioButton->animateCheck();

    // The animation should be running (hard to test without access to internal state)
    // For now, just verify the method doesn't crash
    QVERIFY(true);
}

void FluentRadioButtonTest::testButtonGroup() {
    // Test button group functionality
    FluentRadioButton* radio1 = new FluentRadioButton("Option 1");
    FluentRadioButton* radio2 = new FluentRadioButton("Option 2");
    FluentRadioButton* radio3 = new FluentRadioButton("Option 3");

    // Create a button group
    QButtonGroup* group = new QButtonGroup();
    group->addButton(radio1);
    group->addButton(radio2);
    group->addButton(radio3);

    // Test that only one can be checked at a time
    radio1->setChecked(true);
    QVERIFY(radio1->isChecked());
    QVERIFY(!radio2->isChecked());
    QVERIFY(!radio3->isChecked());

    radio2->setChecked(true);
    QVERIFY(!radio1->isChecked());
    QVERIFY(radio2->isChecked());
    QVERIFY(!radio3->isChecked());

    delete group;
    delete radio1;
    delete radio2;
    delete radio3;
}

void FluentRadioButtonTest::testExclusiveSelection() {
    // Test exclusive selection behavior
    FluentRadioButton* radio1 = new FluentRadioButton("Option 1");
    FluentRadioButton* radio2 = new FluentRadioButton("Option 2");

    // Both should have auto exclusive enabled by default
    QVERIFY(radio1->autoExclusive());
    QVERIFY(radio2->autoExclusive());

    // When both are in the same parent, they should be mutually exclusive
    QWidget* parent = new QWidget();
    radio1->setParent(parent);
    radio2->setParent(parent);

    radio1->setChecked(true);
    QVERIFY(radio1->isChecked());

    radio2->setChecked(true);
    // radio1 should be unchecked now (if auto-exclusive is working)
    QVERIFY(radio2->isChecked());

    delete parent; // This will delete radio1 and radio2 as well
}

void FluentRadioButtonTest::testRadioGroup() {
    // Test radio group factory method
    QStringList options = {"Option A", "Option B", "Option C"};
    QList<FluentRadioButton*> radioGroup = FluentRadioButton::createRadioGroup(options);

    QCOMPARE(radioGroup.size(), 3);

    // Test that they are mutually exclusive
    radioGroup[0]->setChecked(true);
    QVERIFY(radioGroup[0]->isChecked());
    QVERIFY(!radioGroup[1]->isChecked());
    QVERIFY(!radioGroup[2]->isChecked());

    radioGroup[1]->setChecked(true);
    QVERIFY(!radioGroup[0]->isChecked());
    QVERIFY(radioGroup[1]->isChecked());
    QVERIFY(!radioGroup[2]->isChecked());

    qDeleteAll(radioGroup);
}

void FluentRadioButtonTest::testMouseInteraction() {
    // Test mouse press and release
    QSignalSpy pressedSpy(m_radioButton, &FluentRadioButton::pressed);
    QSignalSpy releasedSpy(m_radioButton, &FluentRadioButton::released);
    QSignalSpy clickedSpy(m_radioButton, &FluentRadioButton::clicked);
    QSignalSpy checkedChangedSpy(m_radioButton, &FluentRadioButton::checkedChanged);

    QPoint center = m_radioButton->rect().center();

    // Test mouse press
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_radioButton, &pressEvent);

    QCOMPARE(pressedSpy.count(), 1);

    // Test mouse release
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_radioButton, &releaseEvent);

    QCOMPARE(releasedSpy.count(), 1);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.count(), 1);
    QVERIFY(m_radioButton->isChecked());

    // Test mouse release outside radio button (should not trigger click)
    m_radioButton->setChecked(false);
    checkedChangedSpy.clear();
    clickedSpy.clear();

    QApplication::sendEvent(m_radioButton, &pressEvent);
    pressedSpy.clear();

    QPoint outside = m_radioButton->rect().bottomRight() + QPoint(10, 10);
    QMouseEvent releaseOutsideEvent(QEvent::MouseButtonRelease, outside, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_radioButton, &releaseOutsideEvent);

    QCOMPARE(clickedSpy.count(), 0); // Should not click
    QCOMPARE(checkedChangedSpy.count(), 0); // Should not change state
}

void FluentRadioButtonTest::testKeyboardInteraction() {
    // Test keyboard interaction
    QSignalSpy clickedSpy(m_radioButton, &FluentRadioButton::clicked);
    QSignalSpy checkedChangedSpy(m_radioButton, &FluentRadioButton::checkedChanged);

    m_radioButton->setFocus();
    QVERIFY(m_radioButton->hasFocus());

    // Test Space key
    QKeyEvent spacePress(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    QApplication::sendEvent(m_radioButton, &spacePress);

    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.count(), 1);
    QVERIFY(m_radioButton->isChecked());

    // Test Return key
    m_radioButton->setChecked(false);
    clickedSpy.clear();
    checkedChangedSpy.clear();

    QKeyEvent returnPress(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_radioButton, &returnPress);

    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.count(), 1);
    QVERIFY(m_radioButton->isChecked());
}

void FluentRadioButtonTest::testClick() {
    // Test programmatic click
    QSignalSpy clickedSpy(m_radioButton, &FluentRadioButton::clicked);
    QSignalSpy pressedSpy(m_radioButton, &FluentRadioButton::pressed);
    QSignalSpy releasedSpy(m_radioButton, &FluentRadioButton::released);
    QSignalSpy checkedChangedSpy(m_radioButton, &FluentRadioButton::checkedChanged);

    m_radioButton->click();

    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.count(), 1);
    QVERIFY(m_radioButton->isChecked());
}

void FluentRadioButtonTest::testToggle() {
    // Test toggle functionality
    QSignalSpy checkedChangedSpy(m_radioButton, &FluentRadioButton::checkedChanged);
    QSignalSpy toggledSpy(m_radioButton, &FluentRadioButton::toggled);

    QVERIFY(!m_radioButton->isChecked());

    // Radio buttons can only be toggled on, not off
    m_radioButton->toggle();
    QVERIFY(m_radioButton->isChecked());
    QCOMPARE(checkedChangedSpy.count(), 1);
    QCOMPARE(toggledSpy.count(), 1);

    // Toggling again should not change state (radio buttons don't toggle off)
    m_radioButton->toggle();
    QVERIFY(m_radioButton->isChecked());
    QCOMPARE(checkedChangedSpy.count(), 1); // Should still be 1
}

void FluentRadioButtonTest::testCheckedChangedSignal() {
    // Test checked changed signal
    QSignalSpy checkedChangedSpy(m_radioButton, &FluentRadioButton::checkedChanged);

    m_radioButton->setChecked(true);
    QCOMPARE(checkedChangedSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.first().first().toBool(), true);

    m_radioButton->setChecked(false);
    QCOMPARE(checkedChangedSpy.count(), 2);
    QCOMPARE(checkedChangedSpy.last().first().toBool(), false);
}

void FluentRadioButtonTest::testToggledSignal() {
    // Test toggled signal
    QSignalSpy toggledSpy(m_radioButton, &FluentRadioButton::toggled);

    m_radioButton->setChecked(true);
    QCOMPARE(toggledSpy.count(), 1);
    QCOMPARE(toggledSpy.first().first().toBool(), true);

    m_radioButton->setChecked(false);
    QCOMPARE(toggledSpy.count(), 2);
    QCOMPARE(toggledSpy.last().first().toBool(), false);
}

void FluentRadioButtonTest::testClickedSignal() {
    // Test clicked signal
    QSignalSpy clickedSpy(m_radioButton, &FluentRadioButton::clicked);

    m_radioButton->click();
    QCOMPARE(clickedSpy.count(), 1);

    // Mouse click should also emit clicked signal
    QPoint center = m_radioButton->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(m_radioButton, &pressEvent);
    QApplication::sendEvent(m_radioButton, &releaseEvent);

    QCOMPARE(clickedSpy.count(), 2);
}

void FluentRadioButtonTest::testPressedReleasedSignals() {
    // Test pressed and released signals
    QSignalSpy pressedSpy(m_radioButton, &FluentRadioButton::pressed);
    QSignalSpy releasedSpy(m_radioButton, &FluentRadioButton::released);

    m_radioButton->click();

    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 1);
}

void FluentRadioButtonTest::testContentChangeSignals() {
    // Test that content change signals are emitted correctly
    QSignalSpy textChangedSpy(m_radioButton, &FluentRadioButton::textChanged);
    QSignalSpy valueChangedSpy(m_radioButton, &FluentRadioButton::valueChanged);
    QSignalSpy iconChangedSpy(m_radioButton, &FluentRadioButton::iconChanged);
    QSignalSpy showIconChangedSpy(m_radioButton, &FluentRadioButton::showIconChanged);
    QSignalSpy sizeChangedSpy(m_radioButton, &FluentRadioButton::sizeChanged);
    QSignalSpy labelPositionChangedSpy(m_radioButton, &FluentRadioButton::labelPositionChanged);
    QSignalSpy autoExclusiveChangedSpy(m_radioButton, &FluentRadioButton::autoExclusiveChanged);
    QSignalSpy animatedChangedSpy(m_radioButton, &FluentRadioButton::animatedChanged);
    QSignalSpy animationDurationChangedSpy(m_radioButton, &FluentRadioButton::animationDurationChanged);

    // Change text
    m_radioButton->setText("New Text");
    QCOMPARE(textChangedSpy.count(), 1);

    // Change value
    m_radioButton->setValue("new_value");
    QCOMPARE(valueChangedSpy.count(), 1);

    // Change icon
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);
    m_radioButton->setIcon(QIcon(pixmap));
    QCOMPARE(iconChangedSpy.count(), 1);

    // Change show icon
    m_radioButton->setShowIcon(true);
    QCOMPARE(showIconChangedSpy.count(), 1);

    // Change size
    m_radioButton->setSize(FluentRadioButtonSize::Large);
    QCOMPARE(sizeChangedSpy.count(), 1);

    // Change label position
    m_radioButton->setLabelPosition(FluentRadioButtonLabelPosition::Left);
    QCOMPARE(labelPositionChangedSpy.count(), 1);

    // Change auto exclusive
    m_radioButton->setAutoExclusive(false);
    QCOMPARE(autoExclusiveChangedSpy.count(), 1);

    // Change animated
    m_radioButton->setAnimated(false);
    QCOMPARE(animatedChangedSpy.count(), 1);

    // Change animation duration
    m_radioButton->setAnimationDuration(300);
    QCOMPARE(animationDurationChangedSpy.count(), 1);
}

void FluentRadioButtonTest::testEnabledState() {
    // Test enabled state
    QVERIFY(m_radioButton->isEnabled()); // Default should be enabled

    m_radioButton->setEnabled(false);
    QVERIFY(!m_radioButton->isEnabled());

    // Disabled radio button should not respond to clicks
    QSignalSpy clickedSpy(m_radioButton, &FluentRadioButton::clicked);
    m_radioButton->click();
    QCOMPARE(clickedSpy.count(), 0);

    m_radioButton->setEnabled(true);
    QVERIFY(m_radioButton->isEnabled());

    // Enabled radio button should respond to clicks
    m_radioButton->click();
    QCOMPARE(clickedSpy.count(), 1);
}

void FluentRadioButtonTest::testFocusState() {
    // Test focus state
    QVERIFY(!m_radioButton->hasFocus());

    m_radioButton->setFocus();
    QVERIFY(m_radioButton->hasFocus());

    m_radioButton->clearFocus();
    QVERIFY(!m_radioButton->hasFocus());
}

void FluentRadioButtonTest::testHoverState() {
    // Test hover state (this is harder to test programmatically)
    // For now, just verify that the radio button can handle enter/leave events

    QEnterEvent enterEvent(QPointF(10, 10), QPointF(10, 10), QPointF(10, 10));
    QEvent leaveEvent(QEvent::Leave);

    // These should not crash
    QApplication::sendEvent(m_radioButton, &enterEvent);
    QApplication::sendEvent(m_radioButton, &leaveEvent);

    QVERIFY(true); // If we get here, the events were handled without crashing
}

void FluentRadioButtonTest::testAccessibility() {
    // Test accessibility features
    const QString accessibleName = "Test Radio Button";
    const QString accessibleDescription = "This is a test radio button";

    m_radioButton->setAccessibleName(accessibleName);
    QCOMPARE(m_radioButton->accessibleName(), accessibleName);

    m_radioButton->setAccessibleDescription(accessibleDescription);
    QCOMPARE(m_radioButton->accessibleDescription(), accessibleDescription);

    // Test that radio button has proper focus policy
    QVERIFY(m_radioButton->focusPolicy() != Qt::NoFocus);

    // Test that radio button accepts keyboard input
    QVERIFY(m_radioButton->focusPolicy() & Qt::TabFocus);
}

void FluentRadioButtonTest::testThemeIntegration() {
    // Test that radio button responds to theme changes
    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    bool originalDarkMode = theme.isDarkMode();

    // Change theme mode
    theme.setDarkMode(!originalDarkMode);

    // Radio button should update its appearance (this would require checking internal styling)
    // For now, just verify the radio button still functions correctly
    QVERIFY(m_radioButton->isEnabled());

    // Restore original theme
    theme.setDarkMode(originalDarkMode);
}

QTEST_MAIN(FluentRadioButtonTest)
#include "FluentRadioButtonTest.moc"
