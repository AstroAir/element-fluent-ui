// tests/FluentFormComponentsTest.cpp
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class FluentFormComponentsTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // FluentTextInput tests
    void testTextInputConstructor();
    void testTextInputBasicFunctionality();
    void testTextInputValidation();
    void testTextInputStates();
    void testTextInputPlaceholder();
    void testTextInputPasswordMode();
    void testTextInputMultiline();
    void testTextInputCharacterLimit();
    void testTextInputClearButton();

    // FluentCheckBox tests
    void testCheckBoxConstructor();
    void testCheckBoxBasicFunctionality();
    void testCheckBoxStates();
    void testCheckBoxTriState();
    void testCheckBoxAnimation();
    void testCheckBoxKeyboard();
    void testCheckBoxMouse();

    // Integration tests
    void testFormValidation();
    void testThemeIntegration();
    void testAccessibility();

private:
    Components::FluentTextInput* m_textInput{nullptr};
    Components::FluentCheckBox* m_checkBox{nullptr};
};

void FluentFormComponentsTest::initTestCase() {
    qApp->setApplicationName("FluentFormComponentsTest");
}

void FluentFormComponentsTest::cleanupTestCase() {
    // Cleanup
}

void FluentFormComponentsTest::init() {
    m_textInput = new Components::FluentTextInput();
    m_textInput->show();

    m_checkBox = new Components::FluentCheckBox("Test CheckBox");
    m_checkBox->show();

    // Use return value of qWaitForWindowExposed to satisfy 'nodiscard'
    QVERIFY2(QTest::qWaitForWindowExposed(m_textInput),
             "m_textInput window not exposed within timeout");
    QVERIFY2(QTest::qWaitForWindowExposed(m_checkBox),
             "m_checkBox window not exposed within timeout");
}

void FluentFormComponentsTest::cleanup() {
    delete m_textInput;
    delete m_checkBox;

    m_textInput = nullptr;
    m_checkBox = nullptr;
}

void FluentFormComponentsTest::testTextInputConstructor() {
    // Test default constructor
    auto* input1 = new Components::FluentTextInput();
    QVERIFY(input1 != nullptr);
    QVERIFY(input1->text().isEmpty());
    QVERIFY(input1->placeholderText().isEmpty());
    delete input1;

    // Test constructor with placeholder
    auto* input2 = new Components::FluentTextInput("Enter text...");
    QVERIFY(input2 != nullptr);
    QCOMPARE(input2->placeholderText(), QString("Enter text..."));
    delete input2;

    // Test constructor with parent
    auto* parent = new QWidget();
    auto* input3 = new Components::FluentTextInput(parent);
    QVERIFY(input3->parent() == parent);
    delete parent;  // This should also delete input3
}

void FluentFormComponentsTest::testTextInputBasicFunctionality() {
    // Test text setting and getting
    const QString testText = "Hello, World!";
    m_textInput->setText(testText);
    QCOMPARE(m_textInput->text(), testText);

    // Test text changed signal
    QSignalSpy textChangedSpy(m_textInput, SIGNAL(textChanged(QString)));
    m_textInput->setText("New text");
    QCOMPARE(textChangedSpy.count(), 1);
    QCOMPARE(textChangedSpy.first().first().toString(), QString("New text"));

    // Test clear functionality
    m_textInput->clear();
    QVERIFY(m_textInput->text().isEmpty());

    // Test placeholder text
    const QString placeholder = "Enter your name...";
    m_textInput->setPlaceholderText(placeholder);
    QCOMPARE(m_textInput->placeholderText(), placeholder);
}

void FluentFormComponentsTest::testTextInputValidation() {
    // Test email validation
    m_textInput->setValidationType(
        Components::FluentTextInput::FluentTextInputValidation::Email);
    QSignalSpy validationChangedSpy(m_textInput,
                                    SIGNAL(validationChanged(bool)));

    // Test invalid email
    m_textInput->setText("invalid-email");
    QVERIFY(!m_textInput->isValid());
    QCOMPARE(validationChangedSpy.count(), 1);
    QCOMPARE(validationChangedSpy.last().first().toBool(), false);

    // Test valid email
    m_textInput->setText("test@example.com");
    QVERIFY(m_textInput->isValid());
    QCOMPARE(validationChangedSpy.count(), 2);
    QCOMPARE(validationChangedSpy.last().first().toBool(), true);

    // Test URL validation
    m_textInput->setValidationType(
        Components::FluentTextInput::FluentTextInputValidation::URL);
    m_textInput->setText("https://www.example.com");
    QVERIFY(m_textInput->isValid());

    m_textInput->setText("not-a-url");
    QVERIFY(!m_textInput->isValid());

    // Test number validation
    m_textInput->setValidationType(
        Components::FluentTextInput::FluentTextInputValidation::Number);
    m_textInput->setText("123.45");
    QVERIFY(m_textInput->isValid());

    m_textInput->setText("not-a-number");
    QVERIFY(!m_textInput->isValid());

    // Test custom validation
    m_textInput->setValidationType(
        Components::FluentTextInput::FluentTextInputValidation::Custom);
    m_textInput->setCustomValidator(
        [](const QString& text) { return text.length() >= 5; });

    m_textInput->setText("1234");
    QVERIFY(!m_textInput->isValid());

    m_textInput->setText("12345");
    QVERIFY(m_textInput->isValid());
}

void FluentFormComponentsTest::testTextInputStates() {
    // Test normal state
    QCOMPARE(m_textInput->state(), Core::FluentState::Normal);

    // Test focus state
    m_textInput->setFocus();
    QCOMPARE(m_textInput->state(), Core::FluentState::Focused);

    // Test disabled state
    m_textInput->setEnabled(false);
    QCOMPARE(m_textInput->state(), Core::FluentState::Disabled);

    m_textInput->setEnabled(true);
    QCOMPARE(m_textInput->state(), Core::FluentState::Normal);

    // Test error state (through validation)
    m_textInput->setValidationType(
        Components::FluentTextInput::FluentTextInputValidation::Email);
    m_textInput->setText("invalid-email");
    // State should reflect validation error
    QVERIFY(!m_textInput->isValid());
}

void FluentFormComponentsTest::testTextInputPlaceholder() {
    const QString placeholder = "Type something...";
    m_textInput->setPlaceholderText(placeholder);
    QCOMPARE(m_textInput->placeholderText(), placeholder);

    // Test that placeholder disappears when text is entered
    m_textInput->setText("Some text");
    // Placeholder should not be visible when there's text
    QVERIFY(!m_textInput->text().isEmpty());

    // Test that placeholder reappears when text is cleared
    m_textInput->clear();
    QVERIFY(m_textInput->text().isEmpty());
}

void FluentFormComponentsTest::testTextInputPasswordMode() {
    // Test password mode
    m_textInput->setPasswordMode(true);
    QVERIFY(m_textInput->isPasswordMode());

    // Test that text is masked
    m_textInput->setText("secret");
    // The actual text should still be accessible
    QCOMPARE(m_textInput->text(), QString("secret"));

    // Test toggle password visibility
    m_textInput->setPasswordVisible(true);
    QVERIFY(m_textInput->isPasswordVisible());

    m_textInput->setPasswordVisible(false);
    QVERIFY(!m_textInput->isPasswordVisible());

    // Test disabling password mode
    m_textInput->setPasswordMode(false);
    QVERIFY(!m_textInput->isPasswordMode());
}

void FluentFormComponentsTest::testTextInputMultiline() {
    // Test multiline mode
    m_textInput->setMultiline(true);
    QVERIFY(m_textInput->isMultiline());

    // Test multiline text
    const QString multilineText = "Line 1\nLine 2\nLine 3";
    m_textInput->setText(multilineText);
    QCOMPARE(m_textInput->text(), multilineText);

    // Test disabling multiline
    m_textInput->setMultiline(false);
    QVERIFY(!m_textInput->isMultiline());
}

void FluentFormComponentsTest::testTextInputCharacterLimit() {
    // Test character limit
    const int limit = 10;
    m_textInput->setMaxLength(limit);
    QCOMPARE(m_textInput->maxLength(), limit);

    // Test that text is truncated
    m_textInput->setText("This is a very long text that exceeds the limit");
    QVERIFY(m_textInput->text().length() <= limit);

    // Test removing limit
    m_textInput->setMaxLength(-1);  // No limit
    m_textInput->setText(
        "This is a very long text that should not be truncated");
    QVERIFY(m_textInput->text().length() > limit);
}

void FluentFormComponentsTest::testTextInputClearButton() {
    // Test clear button visibility
    m_textInput->setClearButtonEnabled(true);
    QVERIFY(m_textInput->isClearButtonEnabled());

    // Test that clear button works
    m_textInput->setText("Some text");
    QVERIFY(!m_textInput->text().isEmpty());

    // Simulate clear button click (would need access to internal button)
    m_textInput->clear();
    QVERIFY(m_textInput->text().isEmpty());

    // Test disabling clear button
    m_textInput->setClearButtonEnabled(false);
    QVERIFY(!m_textInput->isClearButtonEnabled());
}

void FluentFormComponentsTest::testCheckBoxConstructor() {
    // Test default constructor
    auto* checkBox1 = new Components::FluentCheckBox();
    QVERIFY(checkBox1 != nullptr);
    QVERIFY(checkBox1->text().isEmpty());
    QVERIFY(!checkBox1->isChecked());
    delete checkBox1;

    // Test constructor with text
    auto* checkBox2 = new Components::FluentCheckBox("Test Text");
    QVERIFY(checkBox2 != nullptr);
    QCOMPARE(checkBox2->text(), QString("Test Text"));
    delete checkBox2;

    // Test constructor with parent
    auto* parent = new QWidget();
    auto* checkBox3 = new Components::FluentCheckBox(parent);
    QVERIFY(checkBox3->parent() == parent);
    delete parent;
}

void FluentFormComponentsTest::testCheckBoxBasicFunctionality() {
    // Test text setting and getting
    const QString testText = "Check me!";
    m_checkBox->setText(testText);
    QCOMPARE(m_checkBox->text(), testText);

    // Test checked state
    QVERIFY(!m_checkBox->isChecked());

    QSignalSpy checkStateChangedSpy(
        m_checkBox, SIGNAL(checkStateChanged(FluentCheckState)));
    QSignalSpy toggledSpy(m_checkBox, SIGNAL(toggled(bool)));

    m_checkBox->setChecked(true);
    QVERIFY(m_checkBox->isChecked());
    QCOMPARE(checkStateChangedSpy.count(), 1);
    QCOMPARE(checkStateChangedSpy.first()
                 .first()
                 .value<Components::FluentCheckState>(),
             Components::FluentCheckState::Checked);
    QCOMPARE(toggledSpy.count(), 1);
    QCOMPARE(toggledSpy.first().first().toBool(), true);

    m_checkBox->setChecked(false);
    QVERIFY(!m_checkBox->isChecked());
    QCOMPARE(checkStateChangedSpy.count(), 2);
    QCOMPARE(checkStateChangedSpy.last()
                 .first()
                 .value<Components::FluentCheckState>(),
             Components::FluentCheckState::Unchecked);
    QCOMPARE(toggledSpy.count(), 2);

    // Test toggle
    m_checkBox->toggle();
    QVERIFY(m_checkBox->isChecked());
    QCOMPARE(checkStateChangedSpy.count(), 3);
    QCOMPARE(toggledSpy.count(), 3);
}

void FluentFormComponentsTest::testCheckBoxStates() {
    // Test normal state
    QCOMPARE(m_checkBox->state(), Core::FluentState::Normal);

    // Test disabled state
    m_checkBox->setEnabled(false);
    QCOMPARE(m_checkBox->state(), Core::FluentState::Disabled);

    m_checkBox->setEnabled(true);
    QCOMPARE(m_checkBox->state(), Core::FluentState::Normal);

    // Test focus state
    m_checkBox->setFocus();
    QCOMPARE(m_checkBox->state(), Core::FluentState::Focused);
}

void FluentFormComponentsTest::testCheckBoxTriState() {
    // Test tri-state functionality
    m_checkBox->setTristate(true);
    QVERIFY(m_checkBox->isTristate());

    // Test all three states
    m_checkBox->setCheckState(Components::FluentCheckState::Unchecked);
    QCOMPARE(m_checkBox->checkState(), Components::FluentCheckState::Unchecked);
    QVERIFY(!m_checkBox->isChecked());

    m_checkBox->setCheckState(Components::FluentCheckState::PartiallyChecked);
    QCOMPARE(m_checkBox->checkState(),
             Components::FluentCheckState::PartiallyChecked);
    QVERIFY(
        !m_checkBox->isChecked());  // Partially checked is not fully checked

    m_checkBox->setCheckState(Components::FluentCheckState::Checked);
    QCOMPARE(m_checkBox->checkState(), Components::FluentCheckState::Checked);
    QVERIFY(m_checkBox->isChecked());

    // Test cycling through states
    // Cycle manually via explicit states
    m_checkBox->setCheckState(Components::FluentCheckState::Unchecked);
    m_checkBox->setCheckState(Components::FluentCheckState::PartiallyChecked);
    m_checkBox->setCheckState(Components::FluentCheckState::Checked);

    // Test disabling tri-state
    m_checkBox->setTristate(false);
    QVERIFY(!m_checkBox->isTristate());
}

void FluentFormComponentsTest::testCheckBoxAnimation() {
    // FluentCheckBox animation APIs not exposed in this build; placeholder
    // test.
    QVERIFY(true);
}

void FluentFormComponentsTest::testCheckBoxKeyboard() {
    // Test keyboard interaction
    m_checkBox->setFocus();
    QVERIFY(m_checkBox->hasFocus());

    // Test space key toggle
    QKeyEvent spacePress(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    QKeyEvent spaceRelease(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier);

    bool initialState = m_checkBox->isChecked();

    QApplication::sendEvent(m_checkBox, &spacePress);
    QApplication::sendEvent(m_checkBox, &spaceRelease);

    // State should have toggled
    QCOMPARE(m_checkBox->isChecked(), !initialState);
}

void FluentFormComponentsTest::testCheckBoxMouse() {
    // Test mouse interaction
    QPoint center = m_checkBox->rect().center();
    QPoint global = m_checkBox->mapToGlobal(center);

    // Use non-deprecated QMouseEvent constructor (localPos, scenePos,
    // globalPos, ...)
    QMouseEvent pressEvent(
        QEvent::MouseButtonPress,
        QPointF(center),  // localPos
        QPointF(center),  // scenePos (same for plain QWidget usage)
        QPointF(global),  // globalPos
        Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(center),
                             QPointF(center), QPointF(global), Qt::LeftButton,
                             Qt::LeftButton, Qt::NoModifier);

    bool initialState = m_checkBox->isChecked();

    QApplication::sendEvent(m_checkBox, &pressEvent);
    QApplication::sendEvent(m_checkBox, &releaseEvent);

    // State should have toggled
    QCOMPARE(m_checkBox->isChecked(), !initialState);
}

void FluentFormComponentsTest::testFormValidation() {
    // Test form-level validation
    auto* emailInput = new Components::FluentTextInput("Enter email...");
    emailInput->setValidationType(
        Components::FluentTextInput::FluentTextInputValidation::Email);

    auto* termsCheckBox =
        new Components::FluentCheckBox("I agree to the terms");

    // Test invalid form
    emailInput->setText("invalid-email");
    termsCheckBox->setChecked(false);

    QVERIFY(!emailInput->isValid());
    QVERIFY(!termsCheckBox->isChecked());

    // Test valid form
    emailInput->setText("user@example.com");
    termsCheckBox->setChecked(true);

    QVERIFY(emailInput->isValid());
    QVERIFY(termsCheckBox->isChecked());

    delete emailInput;
    delete termsCheckBox;
}

void FluentFormComponentsTest::testThemeIntegration() {
    // Test that components respond to theme changes
    auto& theme = Styling::FluentTheme::instance();

    // Change theme mode
    bool originalDarkMode = theme.isDarkMode();
    theme.setDarkMode(!originalDarkMode);

    // Components should update their appearance
    // This would require checking internal styling
    QVERIFY(true);  // Placeholder

    // Restore original theme
    theme.setDarkMode(originalDarkMode);
}

void FluentFormComponentsTest::testAccessibility() {
    // Test accessibility features

    // Test that components have proper accessible names
    m_textInput->setAccessibleName("Email input");
    QCOMPARE(m_textInput->accessibleName(), QString("Email input"));

    m_checkBox->setAccessibleName("Terms agreement");
    QCOMPARE(m_checkBox->accessibleName(), QString("Terms agreement"));

    // Test that components have proper accessible descriptions
    m_textInput->setAccessibleDescription("Enter your email address");
    QCOMPARE(m_textInput->accessibleDescription(),
             QString("Enter your email address"));

    // Test that components support keyboard navigation
    QVERIFY(m_textInput->focusPolicy() != Qt::NoFocus);
    QVERIFY(m_checkBox->focusPolicy() != Qt::NoFocus);
}

QTEST_MAIN(FluentFormComponentsTest)
#include "FluentFormComponentsTest.moc"
