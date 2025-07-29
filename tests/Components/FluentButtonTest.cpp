// tests/Components/FluentButtonTest.cpp
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentButtonTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTextConstructor();
    void testIconTextConstructor();
    void testFactoryMethods();

    // Content tests
    void testText();
    void testIcon();
    void testIconPosition();

    // Style tests
    void testButtonStyle();
    void testButtonSize();
    void testFlat();

    // State tests
    void testEnabled();
    void testFocus();
    void testLoading();
    void testCheckable();
    void testChecked();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardInteraction();
    void testAnimateClick();
    void testToggle();

    // Signal tests
    void testClickedSignal();
    void testPressedSignal();
    void testReleasedSignal();
    void testToggledSignal();
    void testContentChangeSignals();
    void testStateChangeSignals();

    // Accessibility tests
    void testAccessibility();

    // Theme integration tests
    void testThemeIntegration();

private:
    FluentButton* m_button;
};

void FluentButtonTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentButtonTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentButtonTest::init() {
    // Create a fresh button before each test
    m_button = new FluentButton();
    m_button->show();
    QTest::qWaitForWindowExposed(m_button);
}

void FluentButtonTest::cleanup() {
    // Clean up after each test
    delete m_button;
    m_button = nullptr;
}

void FluentButtonTest::testDefaultConstructor() {
    // Test default constructor
    FluentButton* button = new FluentButton();
    
    // Verify default properties
    QVERIFY(button->text().isEmpty());
    QVERIFY(button->icon().isNull());
    QCOMPARE(button->buttonStyle(), FluentButtonStyle::Default);
    QCOMPARE(button->buttonSize(), FluentButtonSize::Medium);
    QCOMPARE(button->iconPosition(), FluentIconPosition::Left);
    QVERIFY(!button->isLoading());
    QVERIFY(!button->isCheckable());
    QVERIFY(!button->isChecked());
    QVERIFY(button->isEnabled());
    
    delete button;
}

void FluentButtonTest::testTextConstructor() {
    // Test constructor with text
    const QString buttonText = "Test Button";
    FluentButton* button = new FluentButton(buttonText);
    
    QCOMPARE(button->text(), buttonText);
    QVERIFY(button->icon().isNull());
    
    delete button;
}

void FluentButtonTest::testIconTextConstructor() {
    // Test constructor with icon and text
    const QString buttonText = "Icon Button";
    QIcon icon(":/icons/test"); // Assuming test icon exists
    
    // If no test icon exists, create a simple pixmap
    if (icon.isNull()) {
        QPixmap pixmap(16, 16);
        pixmap.fill(Qt::red);
        icon = QIcon(pixmap);
    }
    
    FluentButton* button = new FluentButton(icon, buttonText);
    
    QCOMPARE(button->text(), buttonText);
    QVERIFY(!button->icon().isNull());
    
    delete button;
}

void FluentButtonTest::testFactoryMethods() {
    // Test primary button factory method
    FluentButton* primaryButton = FluentButton::createPrimaryButton("Primary");
    QCOMPARE(primaryButton->text(), QString("Primary"));
    QCOMPARE(primaryButton->buttonStyle(), FluentButtonStyle::Primary);
    delete primaryButton;
    
    // Test accent button factory method
    FluentButton* accentButton = FluentButton::createAccentButton("Accent");
    QCOMPARE(accentButton->text(), QString("Accent"));
    QCOMPARE(accentButton->buttonStyle(), FluentButtonStyle::Accent);
    delete accentButton;
    
    // Test icon button factory method
    QIcon icon;
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::blue);
    icon = QIcon(pixmap);
    
    FluentButton* iconButton = FluentButton::createIconButton(icon);
    QVERIFY(!iconButton->icon().isNull());
    QCOMPARE(iconButton->buttonStyle(), FluentButtonStyle::Icon);
    delete iconButton;
}

void FluentButtonTest::testText() {
    // Test setting and getting text
    const QString text1 = "Button Text";
    const QString text2 = "New Text";
    
    QSignalSpy textChangedSpy(m_button, &FluentButton::textChanged);
    
    m_button->setText(text1);
    QCOMPARE(m_button->text(), text1);
    QCOMPARE(textChangedSpy.count(), 1);
    QCOMPARE(textChangedSpy.first().first().toString(), text1);
    
    m_button->setText(text2);
    QCOMPARE(m_button->text(), text2);
    QCOMPARE(textChangedSpy.count(), 2);
    QCOMPARE(textChangedSpy.last().first().toString(), text2);
    
    // Setting the same text should not emit the signal
    m_button->setText(text2);
    QCOMPARE(textChangedSpy.count(), 2);
}

void FluentButtonTest::testIcon() {
    // Test setting and getting icon
    QSignalSpy iconChangedSpy(m_button, &FluentButton::iconChanged);
    
    QIcon icon1;
    QPixmap pixmap1(16, 16);
    pixmap1.fill(Qt::red);
    icon1 = QIcon(pixmap1);
    
    m_button->setIcon(icon1);
    QVERIFY(!m_button->icon().isNull());
    QCOMPARE(iconChangedSpy.count(), 1);
    
    QIcon icon2;
    QPixmap pixmap2(16, 16);
    pixmap2.fill(Qt::green);
    icon2 = QIcon(pixmap2);
    
    m_button->setIcon(icon2);
    QVERIFY(!m_button->icon().isNull());
    QCOMPARE(iconChangedSpy.count(), 2);
}

void FluentButtonTest::testIconPosition() {
    // Test setting and getting icon position
    QSignalSpy positionChangedSpy(m_button, &FluentButton::iconPositionChanged);
    
    // Default position should be Left
    QCOMPARE(m_button->iconPosition(), FluentIconPosition::Left);
    
    m_button->setIconPosition(FluentIconPosition::Right);
    QCOMPARE(m_button->iconPosition(), FluentIconPosition::Right);
    QCOMPARE(positionChangedSpy.count(), 1);
    QCOMPARE(positionChangedSpy.first().first().value<FluentIconPosition>(), FluentIconPosition::Right);
    
    m_button->setIconPosition(FluentIconPosition::Top);
    QCOMPARE(m_button->iconPosition(), FluentIconPosition::Top);
    QCOMPARE(positionChangedSpy.count(), 2);
    
    m_button->setIconPosition(FluentIconPosition::Bottom);
    QCOMPARE(m_button->iconPosition(), FluentIconPosition::Bottom);
    QCOMPARE(positionChangedSpy.count(), 3);
    
    // Setting the same position should not emit the signal
    m_button->setIconPosition(FluentIconPosition::Bottom);
    QCOMPARE(positionChangedSpy.count(), 3);
}

void FluentButtonTest::testButtonStyle() {
    // Test setting and getting button style
    QSignalSpy styleChangedSpy(m_button, &FluentButton::styleChanged);
    
    // Default style should be Default
    QCOMPARE(m_button->buttonStyle(), FluentButtonStyle::Default);
    
    m_button->setButtonStyle(FluentButtonStyle::Primary);
    QCOMPARE(m_button->buttonStyle(), FluentButtonStyle::Primary);
    QCOMPARE(styleChangedSpy.count(), 1);
    QCOMPARE(styleChangedSpy.first().first().value<FluentButtonStyle>(), FluentButtonStyle::Primary);
    
    m_button->setButtonStyle(FluentButtonStyle::Accent);
    QCOMPARE(m_button->buttonStyle(), FluentButtonStyle::Accent);
    QCOMPARE(styleChangedSpy.count(), 2);
    
    m_button->setButtonStyle(FluentButtonStyle::Subtle);
    QCOMPARE(m_button->buttonStyle(), FluentButtonStyle::Subtle);
    QCOMPARE(styleChangedSpy.count(), 3);
    
    m_button->setButtonStyle(FluentButtonStyle::Outline);
    QCOMPARE(m_button->buttonStyle(), FluentButtonStyle::Outline);
    QCOMPARE(styleChangedSpy.count(), 4);
    
    // Setting the same style should not emit the signal
    m_button->setButtonStyle(FluentButtonStyle::Outline);
    QCOMPARE(styleChangedSpy.count(), 4);
}

void FluentButtonTest::testButtonSize() {
    // Test setting and getting button size
    QSignalSpy sizeChangedSpy(m_button, &FluentButton::sizeChanged);

    // Default size should be Medium
    QCOMPARE(m_button->buttonSize(), FluentButtonSize::Medium);

    m_button->setButtonSize(FluentButtonSize::Small);
    QCOMPARE(m_button->buttonSize(), FluentButtonSize::Small);
    QCOMPARE(sizeChangedSpy.count(), 1);
    QCOMPARE(sizeChangedSpy.first().first().value<FluentButtonSize>(), FluentButtonSize::Small);

    m_button->setButtonSize(FluentButtonSize::Large);
    QCOMPARE(m_button->buttonSize(), FluentButtonSize::Large);
    QCOMPARE(sizeChangedSpy.count(), 2);

    m_button->setButtonSize(FluentButtonSize::ExtraLarge);
    QCOMPARE(m_button->buttonSize(), FluentButtonSize::ExtraLarge);
    QCOMPARE(sizeChangedSpy.count(), 3);

    // Setting the same size should not emit the signal
    m_button->setButtonSize(FluentButtonSize::ExtraLarge);
    QCOMPARE(sizeChangedSpy.count(), 3);
}

void FluentButtonTest::testFlat() {
    // Test flat property
    QVERIFY(!m_button->isFlat()); // Default should be false

    m_button->setFlat(true);
    QVERIFY(m_button->isFlat());

    m_button->setFlat(false);
    QVERIFY(!m_button->isFlat());
}

void FluentButtonTest::testEnabled() {
    // Test enabled state
    QVERIFY(m_button->isEnabled()); // Default should be enabled

    m_button->setEnabled(false);
    QVERIFY(!m_button->isEnabled());
    QCOMPARE(m_button->state(), FluentState::Disabled);

    m_button->setEnabled(true);
    QVERIFY(m_button->isEnabled());
    QCOMPARE(m_button->state(), FluentState::Normal);
}

void FluentButtonTest::testFocus() {
    // Test focus state
    QVERIFY(!m_button->hasFocus());
    QCOMPARE(m_button->state(), FluentState::Normal);

    m_button->setFocus();
    QVERIFY(m_button->hasFocus());
    QCOMPARE(m_button->state(), FluentState::Focused);

    m_button->clearFocus();
    QVERIFY(!m_button->hasFocus());
    QCOMPARE(m_button->state(), FluentState::Normal);
}

void FluentButtonTest::testLoading() {
    // Test loading state
    QSignalSpy loadingChangedSpy(m_button, &FluentButton::loadingChanged);

    QVERIFY(!m_button->isLoading()); // Default should be false

    m_button->setLoading(true);
    QVERIFY(m_button->isLoading());
    QCOMPARE(loadingChangedSpy.count(), 1);
    QCOMPARE(loadingChangedSpy.first().first().toBool(), true);

    m_button->setLoading(false);
    QVERIFY(!m_button->isLoading());
    QCOMPARE(loadingChangedSpy.count(), 2);
    QCOMPARE(loadingChangedSpy.last().first().toBool(), false);

    // Setting the same loading state should not emit the signal
    m_button->setLoading(false);
    QCOMPARE(loadingChangedSpy.count(), 2);
}

void FluentButtonTest::testCheckable() {
    // Test checkable property
    QVERIFY(!m_button->isCheckable()); // Default should be false

    m_button->setCheckable(true);
    QVERIFY(m_button->isCheckable());

    m_button->setCheckable(false);
    QVERIFY(!m_button->isCheckable());
}

void FluentButtonTest::testChecked() {
    // Test checked state
    QSignalSpy checkedChangedSpy(m_button, &FluentButton::checkedChanged);

    QVERIFY(!m_button->isChecked()); // Default should be false

    // Make button checkable first
    m_button->setCheckable(true);

    m_button->setChecked(true);
    QVERIFY(m_button->isChecked());
    QCOMPARE(checkedChangedSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.first().first().toBool(), true);

    m_button->setChecked(false);
    QVERIFY(!m_button->isChecked());
    QCOMPARE(checkedChangedSpy.count(), 2);
    QCOMPARE(checkedChangedSpy.last().first().toBool(), false);

    // Setting the same checked state should not emit the signal
    m_button->setChecked(false);
    QCOMPARE(checkedChangedSpy.count(), 2);
}

void FluentButtonTest::testMouseInteraction() {
    // Test mouse press and release
    QSignalSpy pressedSpy(m_button, &FluentButton::pressed);
    QSignalSpy releasedSpy(m_button, &FluentButton::released);
    QSignalSpy clickedSpy(m_button, &FluentButton::clicked);

    QPoint center = m_button->rect().center();

    // Test mouse press
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_button, &pressEvent);

    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(m_button->state(), FluentState::Pressed);

    // Test mouse release
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_button, &releaseEvent);

    QCOMPARE(releasedSpy.count(), 1);
    QCOMPARE(clickedSpy.count(), 1);

    // Test mouse release outside button (should not trigger click)
    QApplication::sendEvent(m_button, &pressEvent);
    pressedSpy.clear();

    QPoint outside = m_button->rect().bottomRight() + QPoint(10, 10);
    QMouseEvent releaseOutsideEvent(QEvent::MouseButtonRelease, outside, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_button, &releaseOutsideEvent);

    QCOMPARE(clickedSpy.count(), 1); // Should still be 1, no new click
}

void FluentButtonTest::testKeyboardInteraction() {
    // Test keyboard interaction
    QSignalSpy pressedSpy(m_button, &FluentButton::pressed);
    QSignalSpy releasedSpy(m_button, &FluentButton::released);
    QSignalSpy clickedSpy(m_button, &FluentButton::clicked);

    m_button->setFocus();
    QVERIFY(m_button->hasFocus());

    // Test Space key
    QKeyEvent spacePress(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    QKeyEvent spaceRelease(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier);

    QApplication::sendEvent(m_button, &spacePress);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(m_button->state(), FluentState::Pressed);

    QApplication::sendEvent(m_button, &spaceRelease);
    QCOMPARE(releasedSpy.count(), 1);
    QCOMPARE(clickedSpy.count(), 1);

    // Test Return key
    pressedSpy.clear();
    releasedSpy.clear();
    clickedSpy.clear();

    QKeyEvent returnPress(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QKeyEvent returnRelease(QEvent::KeyRelease, Qt::Key_Return, Qt::NoModifier);

    QApplication::sendEvent(m_button, &returnPress);
    QCOMPARE(pressedSpy.count(), 1);

    QApplication::sendEvent(m_button, &returnRelease);
    QCOMPARE(releasedSpy.count(), 1);
    QCOMPARE(clickedSpy.count(), 1);
}

void FluentButtonTest::testAnimateClick() {
    // Test programmatic click animation
    QSignalSpy clickedSpy(m_button, &FluentButton::clicked);

    m_button->animateClick();

    // Should trigger clicked signal
    QCOMPARE(clickedSpy.count(), 1);
}

void FluentButtonTest::testToggle() {
    // Test toggle functionality
    QSignalSpy toggledSpy(m_button, &FluentButton::toggled);
    QSignalSpy checkedChangedSpy(m_button, &FluentButton::checkedChanged);

    // Make button checkable
    m_button->setCheckable(true);
    QVERIFY(!m_button->isChecked());

    m_button->toggle();
    QVERIFY(m_button->isChecked());
    QCOMPARE(toggledSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.count(), 1);
    QCOMPARE(toggledSpy.first().first().toBool(), true);

    m_button->toggle();
    QVERIFY(!m_button->isChecked());
    QCOMPARE(toggledSpy.count(), 2);
    QCOMPARE(checkedChangedSpy.count(), 2);
    QCOMPARE(toggledSpy.last().first().toBool(), false);
}

void FluentButtonTest::testClickedSignal() {
    // Test clicked signal with checkable button
    QSignalSpy clickedSpy(m_button, &FluentButton::clicked);

    // Non-checkable button
    m_button->setCheckable(false);
    m_button->animateClick();
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().toBool(), false);

    // Checkable button
    clickedSpy.clear();
    m_button->setCheckable(true);
    m_button->animateClick();
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().toBool(), true); // Should be checked after click
}

void FluentButtonTest::testPressedSignal() {
    // Test pressed signal
    QSignalSpy pressedSpy(m_button, &FluentButton::pressed);

    QPoint center = m_button->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(m_button, &pressEvent);
    QCOMPARE(pressedSpy.count(), 1);
}

void FluentButtonTest::testReleasedSignal() {
    // Test released signal
    QSignalSpy releasedSpy(m_button, &FluentButton::released);

    QPoint center = m_button->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(m_button, &pressEvent);
    QApplication::sendEvent(m_button, &releaseEvent);

    QCOMPARE(releasedSpy.count(), 1);
}

void FluentButtonTest::testToggledSignal() {
    // Test toggled signal
    QSignalSpy toggledSpy(m_button, &FluentButton::toggled);

    m_button->setCheckable(true);

    // Click to toggle
    QPoint center = m_button->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(m_button, &pressEvent);
    QApplication::sendEvent(m_button, &releaseEvent);

    QCOMPARE(toggledSpy.count(), 1);
    QCOMPARE(toggledSpy.first().first().toBool(), true);

    // Click again to toggle back
    QApplication::sendEvent(m_button, &pressEvent);
    QApplication::sendEvent(m_button, &releaseEvent);

    QCOMPARE(toggledSpy.count(), 2);
    QCOMPARE(toggledSpy.last().first().toBool(), false);
}

void FluentButtonTest::testContentChangeSignals() {
    // Test that content change signals are emitted correctly
    QSignalSpy textChangedSpy(m_button, &FluentButton::textChanged);
    QSignalSpy iconChangedSpy(m_button, &FluentButton::iconChanged);
    QSignalSpy styleChangedSpy(m_button, &FluentButton::styleChanged);
    QSignalSpy sizeChangedSpy(m_button, &FluentButton::sizeChanged);
    QSignalSpy iconPositionChangedSpy(m_button, &FluentButton::iconPositionChanged);

    // Change text
    m_button->setText("New Text");
    QCOMPARE(textChangedSpy.count(), 1);

    // Change icon
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);
    m_button->setIcon(QIcon(pixmap));
    QCOMPARE(iconChangedSpy.count(), 1);

    // Change style
    m_button->setButtonStyle(FluentButtonStyle::Primary);
    QCOMPARE(styleChangedSpy.count(), 1);

    // Change size
    m_button->setButtonSize(FluentButtonSize::Large);
    QCOMPARE(sizeChangedSpy.count(), 1);

    // Change icon position
    m_button->setIconPosition(FluentIconPosition::Right);
    QCOMPARE(iconPositionChangedSpy.count(), 1);
}

void FluentButtonTest::testStateChangeSignals() {
    // Test that state change signals are emitted correctly
    QSignalSpy loadingChangedSpy(m_button, &FluentButton::loadingChanged);
    QSignalSpy checkedChangedSpy(m_button, &FluentButton::checkedChanged);

    // Change loading state
    m_button->setLoading(true);
    QCOMPARE(loadingChangedSpy.count(), 1);
    QCOMPARE(loadingChangedSpy.first().first().toBool(), true);

    // Change checked state (need to make checkable first)
    m_button->setCheckable(true);
    m_button->setChecked(true);
    QCOMPARE(checkedChangedSpy.count(), 1);
    QCOMPARE(checkedChangedSpy.first().first().toBool(), true);
}

void FluentButtonTest::testAccessibility() {
    // Test accessibility features
    const QString accessibleName = "Test Button";
    const QString accessibleDescription = "This is a test button";

    m_button->setAccessibleName(accessibleName);
    QCOMPARE(m_button->accessibleName(), accessibleName);

    m_button->setAccessibleDescription(accessibleDescription);
    QCOMPARE(m_button->accessibleDescription(), accessibleDescription);

    // Test that button has proper focus policy
    QVERIFY(m_button->focusPolicy() != Qt::NoFocus);

    // Test that button accepts keyboard input
    QVERIFY(m_button->focusPolicy() & Qt::TabFocus);
}

void FluentButtonTest::testThemeIntegration() {
    // Test that button responds to theme changes
    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    bool originalDarkMode = theme.isDarkMode();

    // Change theme mode
    theme.setDarkMode(!originalDarkMode);

    // Button should update its appearance (this would require checking internal styling)
    // For now, just verify the button still functions correctly
    QVERIFY(m_button->isEnabled());

    // Restore original theme
    theme.setDarkMode(originalDarkMode);
}

QTEST_MAIN(FluentButtonTest)
#include "FluentButtonTest.moc"
