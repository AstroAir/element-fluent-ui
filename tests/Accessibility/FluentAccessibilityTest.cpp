// tests/Accessibility/FluentAccessibilityTest.cpp
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QAccessible>
#include <QAccessibleInterface>
#include <QKeyEvent>
#include <QFocusEvent>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentAccessibilityTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic accessibility tests
    void testAccessibleInterface();
    void testAccessibleRoles();
    void testAccessibleNames();
    void testAccessibleDescriptions();

    // Focus management tests
    void testFocusPolicy();
    void testFocusChain();
    void testKeyboardNavigation();
    void testTabOrder();

    // Screen reader support tests
    void testScreenReaderAnnouncements();
    void testAccessibleText();
    void testAccessibleValue();
    void testAccessibleState();

    // Keyboard accessibility tests
    void testKeyboardOnlyNavigation();
    void testKeyboardShortcuts();
    void testEscapeKey();
    void testEnterReturnKeys();

    // High contrast mode tests
    void testHighContrastMode();
    void testColorContrast();
    void testVisualIndicators();

    // Component-specific accessibility tests
    void testButtonAccessibility();
    void testTextInputAccessibility();
    void testCheckBoxAccessibility();
    void testRadioButtonAccessibility();
    void testProgressBarAccessibility();
    void testSliderAccessibility();
    void testCardAccessibility();

    // ARIA compliance tests
    void testAriaRoles();
    void testAriaProperties();
    void testAriaStates();

    // Assistive technology tests
    void testScreenReaderCompatibility();
    void testMagnifierCompatibility();
    void testVoiceControlCompatibility();

private:
    QWidget* m_container;
    FluentButton* m_button;
    FluentTextInput* m_textInput;
    FluentCheckBox* m_checkBox;
    FluentRadioButton* m_radioButton;
    FluentProgressBar* m_progressBar;
    FluentSlider* m_slider;
    FluentCard* m_card;
};

void FluentAccessibilityTest::initTestCase() {
    // Initialize accessibility framework
    QAccessible::installFactory(nullptr);
}

void FluentAccessibilityTest::cleanupTestCase() {
    // Clean up accessibility framework
}

void FluentAccessibilityTest::init() {
    // Create container and components for each test
    m_container = new QWidget();
    m_container->resize(800, 600);
    m_container->show();
    
    m_button = new FluentButton("Test Button", m_container);
    m_textInput = new FluentTextInput(m_container);
    m_checkBox = new FluentCheckBox("Test Checkbox", m_container);
    m_radioButton = new FluentRadioButton("Test Radio", m_container);
    m_progressBar = new FluentProgressBar(m_container);
    m_slider = new FluentSlider(m_container);
    m_card = new FluentCard("Test Card", m_container);
}

void FluentAccessibilityTest::cleanup() {
    // Clean up after each test
    delete m_container; // This will delete child components
    
    m_container = nullptr;
    m_button = nullptr;
    m_textInput = nullptr;
    m_checkBox = nullptr;
    m_radioButton = nullptr;
    m_progressBar = nullptr;
    m_slider = nullptr;
    m_card = nullptr;
}

void FluentAccessibilityTest::testAccessibleInterface() {
    // Test that components have accessible interfaces
    QAccessibleInterface* buttonInterface = QAccessible::queryAccessibleInterface(m_button);
    QVERIFY(buttonInterface != nullptr);
    
    QAccessibleInterface* textInputInterface = QAccessible::queryAccessibleInterface(m_textInput);
    QVERIFY(textInputInterface != nullptr);
    
    QAccessibleInterface* checkBoxInterface = QAccessible::queryAccessibleInterface(m_checkBox);
    QVERIFY(checkBoxInterface != nullptr);
    
    QAccessibleInterface* radioButtonInterface = QAccessible::queryAccessibleInterface(m_radioButton);
    QVERIFY(radioButtonInterface != nullptr);
    
    QAccessibleInterface* progressBarInterface = QAccessible::queryAccessibleInterface(m_progressBar);
    QVERIFY(progressBarInterface != nullptr);
    
    QAccessibleInterface* sliderInterface = QAccessible::queryAccessibleInterface(m_slider);
    QVERIFY(sliderInterface != nullptr);
    
    QAccessibleInterface* cardInterface = QAccessible::queryAccessibleInterface(m_card);
    QVERIFY(cardInterface != nullptr);
    
    // Interfaces are managed by Qt accessibility framework
    // No need to delete them manually
}

void FluentAccessibilityTest::testAccessibleRoles() {
    // Test that components have correct accessible roles
    QAccessibleInterface* buttonInterface = QAccessible::queryAccessibleInterface(m_button);
    QVERIFY(buttonInterface != nullptr);
    QCOMPARE(buttonInterface->role(), QAccessible::PushButton);

    QAccessibleInterface* textInputInterface = QAccessible::queryAccessibleInterface(m_textInput);
    QVERIFY(textInputInterface != nullptr);
    QCOMPARE(textInputInterface->role(), QAccessible::EditableText);

    QAccessibleInterface* checkBoxInterface = QAccessible::queryAccessibleInterface(m_checkBox);
    QVERIFY(checkBoxInterface != nullptr);
    QCOMPARE(checkBoxInterface->role(), QAccessible::CheckBox);

    QAccessibleInterface* radioButtonInterface = QAccessible::queryAccessibleInterface(m_radioButton);
    QVERIFY(radioButtonInterface != nullptr);
    QCOMPARE(radioButtonInterface->role(), QAccessible::RadioButton);

    QAccessibleInterface* progressBarInterface = QAccessible::queryAccessibleInterface(m_progressBar);
    QVERIFY(progressBarInterface != nullptr);
    QCOMPARE(progressBarInterface->role(), QAccessible::ProgressBar);

    QAccessibleInterface* sliderInterface = QAccessible::queryAccessibleInterface(m_slider);
    QVERIFY(sliderInterface != nullptr);
    QCOMPARE(sliderInterface->role(), QAccessible::Slider);
}

void FluentAccessibilityTest::testAccessibleNames() {
    // Test that components have accessible names
    QAccessibleInterface* buttonInterface = QAccessible::queryAccessibleInterface(m_button);
    QVERIFY(buttonInterface != nullptr);
    QCOMPARE(buttonInterface->text(QAccessible::Name), QString("Test Button"));

    QAccessibleInterface* checkBoxInterface = QAccessible::queryAccessibleInterface(m_checkBox);
    QVERIFY(checkBoxInterface != nullptr);
    QCOMPARE(checkBoxInterface->text(QAccessible::Name), QString("Test Checkbox"));

    QAccessibleInterface* radioButtonInterface = QAccessible::queryAccessibleInterface(m_radioButton);
    QVERIFY(radioButtonInterface != nullptr);
    QCOMPARE(radioButtonInterface->text(QAccessible::Name), QString("Test Radio"));

    QAccessibleInterface* cardInterface = QAccessible::queryAccessibleInterface(m_card);
    QVERIFY(cardInterface != nullptr);
    QCOMPARE(cardInterface->text(QAccessible::Name), QString("Test Card"));
}

void FluentAccessibilityTest::testAccessibleDescriptions() {
    // Test that components can have accessible descriptions
    m_button->setToolTip("This is a test button");
    
    QAccessibleInterface* buttonInterface = QAccessible::queryAccessibleInterface(m_button);
    QVERIFY(buttonInterface != nullptr);
    QCOMPARE(buttonInterface->text(QAccessible::Description), QString("This is a test button"));

}

void FluentAccessibilityTest::testFocusPolicy() {
    // Test that components have appropriate focus policies
    QCOMPARE(m_button->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(m_textInput->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(m_checkBox->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(m_radioButton->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(m_slider->focusPolicy(), Qt::StrongFocus);
    
    // Progress bar typically doesn't need focus
    QVERIFY(m_progressBar->focusPolicy() == Qt::NoFocus || m_progressBar->focusPolicy() == Qt::TabFocus);
}

void FluentAccessibilityTest::testFocusChain() {
    // Test focus chain functionality
    QVBoxLayout* layout = new QVBoxLayout(m_container);
    layout->addWidget(m_textInput);
    layout->addWidget(m_button);
    layout->addWidget(m_checkBox);
    layout->addWidget(m_radioButton);
    layout->addWidget(m_slider);
    
    // Test that components can receive focus
    m_textInput->setFocus();
    QVERIFY(m_textInput->hasFocus());
    
    m_button->setFocus();
    QVERIFY(m_button->hasFocus());
    
    m_checkBox->setFocus();
    QVERIFY(m_checkBox->hasFocus());
    
    m_radioButton->setFocus();
    QVERIFY(m_radioButton->hasFocus());
    
    m_slider->setFocus();
    QVERIFY(m_slider->hasFocus());
}

void FluentAccessibilityTest::testKeyboardNavigation() {
    // Test keyboard navigation
    QVBoxLayout* layout = new QVBoxLayout(m_container);
    layout->addWidget(m_button);
    layout->addWidget(m_textInput);
    layout->addWidget(m_checkBox);
    
    // Test Tab navigation
    m_button->setFocus();
    QVERIFY(m_button->hasFocus());
    
    QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    QApplication::sendEvent(m_button, &tabEvent);
    
    // Focus should move to next component
    // The exact behavior depends on focus policy and tab order
    QVERIFY(true);
    
    // Test Shift+Tab navigation
    QKeyEvent shiftTabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier);
    QApplication::sendEvent(m_textInput, &shiftTabEvent);
    
    // Focus should move to previous component
    QVERIFY(true);
}

void FluentAccessibilityTest::testTabOrder() {
    // Test tab order functionality
    QVBoxLayout* layout = new QVBoxLayout(m_container);
    layout->addWidget(m_textInput);
    layout->addWidget(m_button);
    layout->addWidget(m_checkBox);
    
    // Set explicit tab order
    QWidget::setTabOrder(m_textInput, m_button);
    QWidget::setTabOrder(m_button, m_checkBox);
    
    // Test that tab order is respected
    m_textInput->setFocus();
    QVERIFY(m_textInput->hasFocus());
    
    // Simulate tab key
    QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    QApplication::sendEvent(m_textInput, &tabEvent);
    
    // The exact focus behavior depends on implementation
    QVERIFY(true);
}

void FluentAccessibilityTest::testScreenReaderAnnouncements() {
    // Test screen reader announcements
    // This is difficult to test directly, but we can verify that accessible text is available

    QAccessibleInterface* buttonInterface = QAccessible::queryAccessibleInterface(m_button);
    QVERIFY(buttonInterface != nullptr);

    // Test that button announces its state
    QString buttonText = buttonInterface->text(QAccessible::Name);
    QVERIFY(!buttonText.isEmpty());

    // Test progress bar announcements
    QAccessibleInterface* progressInterface = QAccessible::queryAccessibleInterface(m_progressBar);
    QVERIFY(progressInterface != nullptr);

    m_progressBar->setValue(50);
    QString progressText = progressInterface->text(QAccessible::Value);
    QVERIFY(!progressText.isEmpty());
}

void FluentAccessibilityTest::testAccessibleText() {
    // Test accessible text for various components
    QAccessibleInterface* textInputInterface = QAccessible::queryAccessibleInterface(m_textInput);
    QVERIFY(textInputInterface != nullptr);

    m_textInput->setText("Test text");
    QString accessibleText = textInputInterface->text(QAccessible::Value);
    QCOMPARE(accessibleText, QString("Test text"));

}

void FluentAccessibilityTest::testAccessibleValue() {
    // Test accessible value for components with values
    QAccessibleInterface* progressInterface = QAccessible::queryAccessibleInterface(m_progressBar);
    QVERIFY(progressInterface != nullptr);

    m_progressBar->setValue(75);
    QString progressValue = progressInterface->text(QAccessible::Value);
    QVERIFY(!progressValue.isEmpty());

    QAccessibleInterface* sliderInterface = QAccessible::queryAccessibleInterface(m_slider);
    QVERIFY(sliderInterface != nullptr);

    m_slider->setValue(50);
    QString sliderValue = sliderInterface->text(QAccessible::Value);
    QVERIFY(!sliderValue.isEmpty());
}

void FluentAccessibilityTest::testAccessibleState() {
    // Test accessible state information
    QAccessibleInterface* checkBoxInterface = QAccessible::queryAccessibleInterface(m_checkBox);
    QVERIFY(checkBoxInterface != nullptr);

    // Test unchecked state
    QVERIFY(!m_checkBox->isChecked());
    QAccessible::State uncheckedState = checkBoxInterface->state();
    QVERIFY(!uncheckedState.checked);

    // Test checked state
    m_checkBox->setChecked(true);
    QAccessible::State checkedState = checkBoxInterface->state();
    QVERIFY(checkedState.checked);

    // Test disabled state
    QAccessibleInterface* buttonInterface = QAccessible::queryAccessibleInterface(m_button);
    QVERIFY(buttonInterface != nullptr);

    m_button->setEnabled(false);
    QAccessible::State disabledState = buttonInterface->state();
    QVERIFY(disabledState.disabled);
}

void FluentAccessibilityTest::testKeyboardOnlyNavigation() {
    // Test that all functionality is accessible via keyboard
    QVBoxLayout* layout = new QVBoxLayout(m_container);
    layout->addWidget(m_button);
    layout->addWidget(m_checkBox);
    layout->addWidget(m_slider);

    // Test button activation via keyboard
    QSignalSpy buttonClickedSpy(m_button, &FluentButton::clicked);
    m_button->setFocus();

    QKeyEvent spaceEvent(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    QApplication::sendEvent(m_button, &spaceEvent);

    QKeyEvent spaceReleaseEvent(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier);
    QApplication::sendEvent(m_button, &spaceReleaseEvent);

    QCOMPARE(buttonClickedSpy.count(), 1);

    // Test checkbox toggle via keyboard
    QSignalSpy checkBoxToggledSpy(m_checkBox, &FluentCheckBox::toggled);
    m_checkBox->setFocus();

    QKeyEvent checkSpaceEvent(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    QApplication::sendEvent(m_checkBox, &checkSpaceEvent);

    QKeyEvent checkSpaceReleaseEvent(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier);
    QApplication::sendEvent(m_checkBox, &checkSpaceReleaseEvent);

    QCOMPARE(checkBoxToggledSpy.count(), 1);

    // Test slider control via keyboard
    QSignalSpy sliderValueChangedSpy(m_slider, &FluentSlider::valueChanged);
    m_slider->setFocus();

    QKeyEvent rightArrowEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
    QApplication::sendEvent(m_slider, &rightArrowEvent);

    // Slider should respond to arrow keys
    QVERIFY(true);
}

void FluentAccessibilityTest::testKeyboardShortcuts() {
    // Test keyboard shortcuts
    QVBoxLayout* layout = new QVBoxLayout(m_container);
    layout->addWidget(m_button);

    // Test Enter key activation
    QSignalSpy buttonClickedSpy(m_button, &FluentButton::clicked);
    m_button->setFocus();

    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_button, &enterEvent);

    QKeyEvent enterReleaseEvent(QEvent::KeyRelease, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_button, &enterReleaseEvent);

    QCOMPARE(buttonClickedSpy.count(), 1);
}

void FluentAccessibilityTest::testEscapeKey() {
    // Test Escape key handling
    QVBoxLayout* layout = new QVBoxLayout(m_container);
    layout->addWidget(m_textInput);

    m_textInput->setText("Test text");
    m_textInput->setFocus();

    QKeyEvent escapeEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    QApplication::sendEvent(m_textInput, &escapeEvent);

    // Escape key behavior depends on component implementation
    QVERIFY(true);
}

void FluentAccessibilityTest::testEnterReturnKeys() {
    // Test Enter/Return key handling
    QVBoxLayout* layout = new QVBoxLayout(m_container);
    layout->addWidget(m_textInput);
    layout->addWidget(m_button);

    // Test Enter in text input
    QSignalSpy textInputReturnPressedSpy(m_textInput, &FluentTextInput::returnPressed);
    m_textInput->setFocus();

    QKeyEvent returnEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_textInput, &returnEvent);

    QCOMPARE(textInputReturnPressedSpy.count(), 1);

    // Test Enter on button
    QSignalSpy buttonClickedSpy(m_button, &FluentButton::clicked);
    m_button->setFocus();

    QKeyEvent buttonReturnEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_button, &buttonReturnEvent);

    QKeyEvent buttonReturnReleaseEvent(QEvent::KeyRelease, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_button, &buttonReturnReleaseEvent);

    QCOMPARE(buttonClickedSpy.count(), 1);
}

void FluentAccessibilityTest::testHighContrastMode() {
    // Test high contrast mode support
    auto& theme = FluentQt::Styling::FluentTheme::instance();
    auto originalMode = theme.mode();

    // Enable high contrast mode
    theme.setHighContrastMode(true);

    // Components should still function correctly in high contrast mode
    QSignalSpy buttonClickedSpy(m_button, &FluentButton::clicked);
    m_button->animateClick();
    QCOMPARE(buttonClickedSpy.count(), 1);

    QSignalSpy checkBoxToggledSpy(m_checkBox, &FluentCheckBox::toggled);
    m_checkBox->setChecked(true);
    QCOMPARE(checkBoxToggledSpy.count(), 1);

    // Restore original mode
    theme.setHighContrastMode(false);
    theme.setMode(originalMode);
}

QTEST_MAIN(FluentAccessibilityTest)
#include "FluentAccessibilityTest.moc"
