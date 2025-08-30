// tests/Components/FluentPasswordBoxTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentPasswordBox.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentPasswordBoxTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Text tests
    void testPassword();
    void testPlaceholderText();
    void testMaxLength();

    // Visibility tests
    void testPasswordVisible();
    void testRevealButton();
    void testTogglePasswordVisibility();

    // Validation tests
    void testPasswordStrength();
    void testPasswordRequirements();
    void testValidation();

    // Appearance tests
    void testPasswordChar();
    void testEchoMode();

    // Interaction tests
    void testTextInput();
    void testKeyboardInteraction();
    void testMouseInteraction();

    // Signal tests
    void testSignalEmission();

    // Security tests
    void testPasswordSecurity();

private:
    FluentPasswordBox* m_passwordBox{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentPasswordBoxTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentPasswordBoxTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentPasswordBoxTest::init() {
    m_passwordBox = new FluentPasswordBox(m_testWidget);
}

void FluentPasswordBoxTest::cleanup() {
    delete m_passwordBox;
    m_passwordBox = nullptr;
}

void FluentPasswordBoxTest::testDefaultConstructor() {
    QVERIFY(m_passwordBox != nullptr);
    QCOMPARE(m_passwordBox->password(), QString());
    QVERIFY(!m_passwordBox->isPasswordVisible());
    QVERIFY(m_passwordBox->isRevealButtonVisible());
    QCOMPARE(m_passwordBox->echoMode(), QLineEdit::Password);
    QCOMPARE(m_passwordBox->passwordChar(), QChar('*'));
}

void FluentPasswordBoxTest::testPassword() {
    QSignalSpy passwordSpy(m_passwordBox, &FluentPasswordBox::passwordChanged);
    
    m_passwordBox->setPassword("secret123");
    QCOMPARE(m_passwordBox->password(), "secret123");
    QCOMPARE(passwordSpy.count(), 1);
    QCOMPARE(passwordSpy.first().first().toString(), "secret123");
    
    // Test empty password
    m_passwordBox->setPassword("");
    QCOMPARE(m_passwordBox->password(), "");
    QCOMPARE(passwordSpy.count(), 2);
    
    // Test password with special characters
    m_passwordBox->setPassword("P@ssw0rd!");
    QCOMPARE(m_passwordBox->password(), "P@ssw0rd!");
    QCOMPARE(passwordSpy.count(), 3);
}

void FluentPasswordBoxTest::testPlaceholderText() {
    QSignalSpy placeholderSpy(m_passwordBox, &FluentPasswordBox::placeholderTextChanged);
    
    m_passwordBox->setPlaceholderText("Enter password");
    QCOMPARE(m_passwordBox->placeholderText(), "Enter password");
    QCOMPARE(placeholderSpy.count(), 1);
    
    // Test empty placeholder
    m_passwordBox->setPlaceholderText("");
    QCOMPARE(m_passwordBox->placeholderText(), "");
    QCOMPARE(placeholderSpy.count(), 2);
}

void FluentPasswordBoxTest::testMaxLength() {
    QSignalSpy maxLengthSpy(m_passwordBox, &FluentPasswordBox::maxLengthChanged);
    
    m_passwordBox->setMaxLength(20);
    QCOMPARE(m_passwordBox->maxLength(), 20);
    QCOMPARE(maxLengthSpy.count(), 1);
    
    // Test password truncation
    m_passwordBox->setPassword("verylongpasswordthatexceedsmaxlength");
    QVERIFY(m_passwordBox->password().length() <= 20);
    
    // Test invalid max length
    m_passwordBox->setMaxLength(-5);
    QVERIFY(m_passwordBox->maxLength() >= 0);
}

void FluentPasswordBoxTest::testPasswordVisible() {
    QSignalSpy visibilitySpy(m_passwordBox, &FluentPasswordBox::passwordVisibleChanged);
    
    m_passwordBox->setPasswordVisible(true);
    QVERIFY(m_passwordBox->isPasswordVisible());
    QCOMPARE(m_passwordBox->echoMode(), QLineEdit::Normal);
    QCOMPARE(visibilitySpy.count(), 1);
    
    m_passwordBox->setPasswordVisible(false);
    QVERIFY(!m_passwordBox->isPasswordVisible());
    QCOMPARE(m_passwordBox->echoMode(), QLineEdit::Password);
    QCOMPARE(visibilitySpy.count(), 2);
}

void FluentPasswordBoxTest::testRevealButton() {
    QSignalSpy revealButtonSpy(m_passwordBox, &FluentPasswordBox::revealButtonVisibleChanged);
    
    m_passwordBox->setRevealButtonVisible(false);
    QVERIFY(!m_passwordBox->isRevealButtonVisible());
    QCOMPARE(revealButtonSpy.count(), 1);
    
    m_passwordBox->setRevealButtonVisible(true);
    QVERIFY(m_passwordBox->isRevealButtonVisible());
    QCOMPARE(revealButtonSpy.count(), 2);
}

void FluentPasswordBoxTest::testTogglePasswordVisibility() {
    QSignalSpy visibilitySpy(m_passwordBox, &FluentPasswordBox::passwordVisibleChanged);
    
    bool initialVisibility = m_passwordBox->isPasswordVisible();
    m_passwordBox->togglePasswordVisibility();
    QCOMPARE(m_passwordBox->isPasswordVisible(), !initialVisibility);
    QCOMPARE(visibilitySpy.count(), 1);
    
    // Toggle again
    m_passwordBox->togglePasswordVisibility();
    QCOMPARE(m_passwordBox->isPasswordVisible(), initialVisibility);
    QCOMPARE(visibilitySpy.count(), 2);
}

void FluentPasswordBoxTest::testPasswordStrength() {
    QSignalSpy strengthSpy(m_passwordBox, &FluentPasswordBox::passwordStrengthChanged);
    
    // Test weak password
    m_passwordBox->setPassword("123");
    FluentPasswordStrength strength = m_passwordBox->passwordStrength();
    QCOMPARE(strength, FluentPasswordStrength::Weak);
    QCOMPARE(strengthSpy.count(), 1);
    
    // Test medium password
    m_passwordBox->setPassword("password123");
    strength = m_passwordBox->passwordStrength();
    QCOMPARE(strength, FluentPasswordStrength::Medium);
    QCOMPARE(strengthSpy.count(), 2);
    
    // Test strong password
    m_passwordBox->setPassword("StrongP@ssw0rd!");
    strength = m_passwordBox->passwordStrength();
    QCOMPARE(strength, FluentPasswordStrength::Strong);
    QCOMPARE(strengthSpy.count(), 3);
    
    // Test empty password
    m_passwordBox->setPassword("");
    strength = m_passwordBox->passwordStrength();
    QCOMPARE(strength, FluentPasswordStrength::None);
    QCOMPARE(strengthSpy.count(), 4);
}

void FluentPasswordBoxTest::testPasswordRequirements() {
    QSignalSpy requirementsSpy(m_passwordBox, &FluentPasswordBox::passwordRequirementsChanged);
    
    FluentPasswordRequirements requirements;
    requirements.minLength = 8;
    requirements.requireUppercase = true;
    requirements.requireLowercase = true;
    requirements.requireNumbers = true;
    requirements.requireSpecialChars = true;
    
    m_passwordBox->setPasswordRequirements(requirements);
    QCOMPARE(m_passwordBox->passwordRequirements().minLength, 8);
    QVERIFY(m_passwordBox->passwordRequirements().requireUppercase);
    QVERIFY(m_passwordBox->passwordRequirements().requireLowercase);
    QVERIFY(m_passwordBox->passwordRequirements().requireNumbers);
    QVERIFY(m_passwordBox->passwordRequirements().requireSpecialChars);
    QCOMPARE(requirementsSpy.count(), 1);
    
    // Test password validation against requirements
    m_passwordBox->setPassword("weak");
    QVERIFY(!m_passwordBox->meetsRequirements());
    
    m_passwordBox->setPassword("StrongP@ssw0rd!");
    QVERIFY(m_passwordBox->meetsRequirements());
}

void FluentPasswordBoxTest::testValidation() {
    // Set up requirements
    FluentPasswordRequirements requirements;
    requirements.minLength = 6;
    requirements.requireUppercase = true;
    requirements.requireNumbers = true;
    m_passwordBox->setPasswordRequirements(requirements);
    
    QSignalSpy validationSpy(m_passwordBox, &FluentPasswordBox::validationStateChanged);
    
    // Test invalid password
    m_passwordBox->setPassword("weak");
    QVERIFY(!m_passwordBox->isValid());
    QCOMPARE(validationSpy.count(), 1);
    
    // Test valid password
    m_passwordBox->setPassword("Strong123");
    QVERIFY(m_passwordBox->isValid());
    QCOMPARE(validationSpy.count(), 2);
    
    // Test validation messages
    m_passwordBox->setPassword("short");
    QStringList validationMessages = m_passwordBox->validationMessages();
    QVERIFY(!validationMessages.isEmpty());
    QVERIFY(validationMessages.contains("Password must be at least 6 characters long"));
}

void FluentPasswordBoxTest::testPasswordChar() {
    QSignalSpy passwordCharSpy(m_passwordBox, &FluentPasswordBox::passwordCharChanged);
    
    m_passwordBox->setPasswordChar(QChar('•'));
    QCOMPARE(m_passwordBox->passwordChar(), QChar('•'));
    QCOMPARE(passwordCharSpy.count(), 1);
    
    // Test null character
    m_passwordBox->setPasswordChar(QChar());
    QVERIFY(!m_passwordBox->passwordChar().isNull()); // Should remain valid
}

void FluentPasswordBoxTest::testEchoMode() {
    QSignalSpy echoModeSpy(m_passwordBox, &FluentPasswordBox::echoModeChanged);
    
    m_passwordBox->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    QCOMPARE(m_passwordBox->echoMode(), QLineEdit::PasswordEchoOnEdit);
    QCOMPARE(echoModeSpy.count(), 1);
    
    m_passwordBox->setEchoMode(QLineEdit::Normal);
    QCOMPARE(m_passwordBox->echoMode(), QLineEdit::Normal);
    QCOMPARE(echoModeSpy.count(), 2);
}

void FluentPasswordBoxTest::testTextInput() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_passwordBox->setFocus();
    
    QSignalSpy passwordSpy(m_passwordBox, &FluentPasswordBox::passwordChanged);
    QSignalSpy textEditedSpy(m_passwordBox, &FluentPasswordBox::textEdited);
    
    // Test typing
    QTest::keyClicks(m_passwordBox, "test123");
    QCOMPARE(m_passwordBox->password(), "test123");
    QVERIFY(passwordSpy.count() >= 1);
    QVERIFY(textEditedSpy.count() >= 1);
    
    // Test backspace
    QTest::keyClick(m_passwordBox, Qt::Key_Backspace);
    QCOMPARE(m_passwordBox->password(), "test12");
    
    // Test clear
    m_passwordBox->clear();
    QCOMPARE(m_passwordBox->password(), "");
}

void FluentPasswordBoxTest::testKeyboardInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_passwordBox->setFocus();
    
    QSignalSpy visibilitySpy(m_passwordBox, &FluentPasswordBox::passwordVisibleChanged);
    
    // Test Ctrl+H to toggle visibility (if supported)
    QTest::keyClick(m_passwordBox, Qt::Key_H, Qt::ControlModifier);
    
    // Test Enter key
    QSignalSpy returnPressedSpy(m_passwordBox, &FluentPasswordBox::returnPressed);
    QTest::keyClick(m_passwordBox, Qt::Key_Return);
    QCOMPARE(returnPressedSpy.count(), 1);
    
    // Test Escape key
    QSignalSpy escapePressedSpy(m_passwordBox, &FluentPasswordBox::escapePressed);
    QTest::keyClick(m_passwordBox, Qt::Key_Escape);
    QCOMPARE(escapePressedSpy.count(), 1);
    
    // Test Tab navigation
    QTest::keyClick(m_passwordBox, Qt::Key_Tab);
}

void FluentPasswordBoxTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_passwordBox->setPassword("secret");
    
    QSignalSpy visibilitySpy(m_passwordBox, &FluentPasswordBox::passwordVisibleChanged);
    
    // Test clicking reveal button (if visible and accessible)
    if (m_passwordBox->isRevealButtonVisible()) {
        // Simulate reveal button click
        m_passwordBox->togglePasswordVisibility();
        QCOMPARE(visibilitySpy.count(), 1);
    }
    
    // Test mouse click to focus
    QTest::mouseClick(m_passwordBox, Qt::LeftButton);
    QVERIFY(m_passwordBox->hasFocus());
    
    // Test double-click to select all
    QTest::mouseDClick(m_passwordBox, Qt::LeftButton);
    QVERIFY(m_passwordBox->hasSelectedText());
}

void FluentPasswordBoxTest::testSignalEmission() {
    QSignalSpy passwordSpy(m_passwordBox, &FluentPasswordBox::passwordChanged);
    QSignalSpy placeholderSpy(m_passwordBox, &FluentPasswordBox::placeholderTextChanged);
    QSignalSpy maxLengthSpy(m_passwordBox, &FluentPasswordBox::maxLengthChanged);
    QSignalSpy visibilitySpy(m_passwordBox, &FluentPasswordBox::passwordVisibleChanged);
    QSignalSpy revealButtonSpy(m_passwordBox, &FluentPasswordBox::revealButtonVisibleChanged);
    QSignalSpy strengthSpy(m_passwordBox, &FluentPasswordBox::passwordStrengthChanged);
    QSignalSpy requirementsSpy(m_passwordBox, &FluentPasswordBox::passwordRequirementsChanged);
    QSignalSpy validationSpy(m_passwordBox, &FluentPasswordBox::validationStateChanged);
    QSignalSpy passwordCharSpy(m_passwordBox, &FluentPasswordBox::passwordCharChanged);
    QSignalSpy echoModeSpy(m_passwordBox, &FluentPasswordBox::echoModeChanged);
    QSignalSpy textEditedSpy(m_passwordBox, &FluentPasswordBox::textEdited);
    QSignalSpy returnPressedSpy(m_passwordBox, &FluentPasswordBox::returnPressed);
    QSignalSpy escapePressedSpy(m_passwordBox, &FluentPasswordBox::escapePressed);
    
    // Test all signals
    m_passwordBox->setPassword("test123");
    m_passwordBox->setPlaceholderText("Enter password");
    m_passwordBox->setMaxLength(50);
    m_passwordBox->setPasswordVisible(true);
    m_passwordBox->setRevealButtonVisible(false);
    
    FluentPasswordRequirements requirements;
    requirements.minLength = 8;
    m_passwordBox->setPasswordRequirements(requirements);
    
    m_passwordBox->setPasswordChar(QChar('•'));
    m_passwordBox->setEchoMode(QLineEdit::Normal);
    
    // Simulate user input
    m_passwordBox->setFocus();
    QTest::keyClicks(m_passwordBox, "abc");
    QTest::keyClick(m_passwordBox, Qt::Key_Return);
    QTest::keyClick(m_passwordBox, Qt::Key_Escape);
    
    QCOMPARE(passwordSpy.count(), 1);
    QCOMPARE(placeholderSpy.count(), 1);
    QCOMPARE(maxLengthSpy.count(), 1);
    QCOMPARE(visibilitySpy.count(), 1);
    QCOMPARE(revealButtonSpy.count(), 1);
    QCOMPARE(strengthSpy.count(), 1);
    QCOMPARE(requirementsSpy.count(), 1);
    QVERIFY(validationSpy.count() >= 1);
    QCOMPARE(passwordCharSpy.count(), 1);
    QCOMPARE(echoModeSpy.count(), 1);
    QVERIFY(textEditedSpy.count() >= 1);
    QCOMPARE(returnPressedSpy.count(), 1);
    QCOMPARE(escapePressedSpy.count(), 1);
}

void FluentPasswordBoxTest::testPasswordSecurity() {
    // Test that password is properly masked
    m_passwordBox->setPassword("secret123");
    m_passwordBox->setPasswordVisible(false);
    
    // Verify that displayed text is masked
    QVERIFY(m_passwordBox->displayText() != "secret123");
    QVERIFY(m_passwordBox->displayText().contains(m_passwordBox->passwordChar()));
    
    // Test that actual password is preserved
    QCOMPARE(m_passwordBox->password(), "secret123");
    
    // Test password clearing
    m_passwordBox->clearPassword();
    QCOMPARE(m_passwordBox->password(), "");
    QCOMPARE(m_passwordBox->displayText(), "");
    
    // Test secure input (no clipboard operations during password mode)
    m_passwordBox->setPassword("sensitive");
    m_passwordBox->setPasswordVisible(false);
    
    // Copy operation should be restricted in password mode
    m_passwordBox->selectAll();
    m_passwordBox->copy();
    // Clipboard should not contain the actual password
    
    // Test undo/redo restrictions in password mode
    m_passwordBox->setPassword("test");
    m_passwordBox->undo(); // Should not reveal previous password
    QVERIFY(m_passwordBox->password().isEmpty() || m_passwordBox->password() == "test");
}

QTEST_MAIN(FluentPasswordBoxTest)
#include "FluentPasswordBoxTest.moc"
