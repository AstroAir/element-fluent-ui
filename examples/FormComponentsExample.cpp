// examples/FormComponentsExample.cpp
#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class FormComponentsWindow : public QMainWindow {
    Q_OBJECT

public:
    FormComponentsWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupConnections();

        // Apply Fluent theme
        auto& theme = Styling::FluentTheme::instance();
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                          .arg(theme.color("backgroundPrimary").name()));
    }

private slots:
    void onValidateForm() {
        // Validate all inputs
        m_nameInput->validate();
        m_emailInput->validate();
        m_passwordInput->validate();

        // Check if form is valid
        bool isValid = m_nameInput->isValid() && m_emailInput->isValid() &&
                       m_passwordInput->isValid();

        if (isValid && m_agreeCheckBox->isChecked()) {
            m_statusLabel->setText("✓ Form is valid and ready to submit!");
            m_statusLabel->setStyleSheet("color: green;");
        } else {
            m_statusLabel->setText("✗ Please fix the errors above");
            m_statusLabel->setStyleSheet("color: red;");
        }
    }

    void onThemeToggle() {
        auto& theme = Styling::FluentTheme::instance();
        theme.setDarkMode(!theme.isDarkMode());

        // Update window background
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                          .arg(theme.color("backgroundPrimary").name()));
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(40, 40, 40, 40);

        // Title
        auto* titleLabel = new QLabel("Fluent UI Form Components Demo", this);
        auto& theme = Styling::FluentTheme::instance();
        titleLabel->setFont(theme.titleFont());
        titleLabel->setStyleSheet(
            QString("color: %1;").arg(theme.color("textPrimary").name()));
        mainLayout->addWidget(titleLabel);

        // Form group
        auto* formGroup = new QGroupBox("User Registration Form", this);
        formGroup->setFont(theme.subtitleFont());
        auto* formLayout = new QVBoxLayout(formGroup);
        formLayout->setSpacing(16);

        // Name input
        m_nameInput = new Components::FluentTextInput(this);
        m_nameInput->setLabelText("Full Name");
        m_nameInput->setPlaceholderText("Enter your full name");
        m_nameInput->setRequired(true);
        m_nameInput->setHelperText("This will be displayed on your profile");
        formLayout->addWidget(m_nameInput);

        // Email input
        m_emailInput = new Components::FluentTextInput(this);
        m_emailInput->setLabelText("Email Address");
        m_emailInput->setPlaceholderText("Enter your email address");
        m_emailInput->setInputType(Components::FluentTextInputType::Email);
        m_emailInput->setRequired(true);
        m_emailInput->setHelperText(
            "We'll use this to send you important updates");
        formLayout->addWidget(m_emailInput);

        // Password input
        m_passwordInput = new Components::FluentTextInput(this);
        m_passwordInput->setLabelText("Password");
        m_passwordInput->setPlaceholderText("Enter a secure password");
        m_passwordInput->setInputType(
            Components::FluentTextInputType::Password);
        m_passwordInput->setRequired(true);
        m_passwordInput->setHelperText("Must be at least 8 characters long");
        formLayout->addWidget(m_passwordInput);

        mainLayout->addWidget(formGroup);

        // Checkbox group
        auto* checkboxGroup = new QGroupBox("Preferences", this);
        checkboxGroup->setFont(theme.subtitleFont());
        auto* checkboxLayout = new QVBoxLayout(checkboxGroup);
        checkboxLayout->setSpacing(12);

        // Agreement checkbox
        m_agreeCheckBox = new Components::FluentCheckBox(
            "I agree to the Terms of Service and Privacy Policy", this);
        checkboxLayout->addWidget(m_agreeCheckBox);

        // Newsletter checkbox
        m_newsletterCheckBox =
            new Components::FluentCheckBox("Subscribe to our newsletter", this);
        m_newsletterCheckBox->setChecked(true);
        checkboxLayout->addWidget(m_newsletterCheckBox);

        // Notifications checkbox (tristate)
        m_notificationsCheckBox =
            new Components::FluentCheckBox("Enable notifications", this);
        m_notificationsCheckBox->setTristate(true);
        m_notificationsCheckBox->setCheckState(
            Components::FluentCheckState::PartiallyChecked);
        checkboxLayout->addWidget(m_notificationsCheckBox);

        mainLayout->addWidget(checkboxGroup);

        // Action buttons
        auto* buttonLayout = new QHBoxLayout();

        auto* validateButton = new QPushButton("Validate Form", this);
        validateButton->setMinimumHeight(32);
        buttonLayout->addWidget(validateButton);

        auto* themeButton = new QPushButton("Toggle Theme", this);
        themeButton->setMinimumHeight(32);
        buttonLayout->addWidget(themeButton);

        buttonLayout->addStretch();
        mainLayout->addLayout(buttonLayout);

        // Status label
        m_statusLabel =
            new QLabel("Fill out the form and click 'Validate Form'", this);
        m_statusLabel->setFont(theme.bodyFont());
        m_statusLabel->setStyleSheet(
            QString("color: %1;").arg(theme.color("textSecondary").name()));
        mainLayout->addWidget(m_statusLabel);

        mainLayout->addStretch();

        // Connect buttons
        connect(validateButton, &QPushButton::clicked, this,
                &FormComponentsWindow::onValidateForm);
        connect(themeButton, &QPushButton::clicked, this,
                &FormComponentsWindow::onThemeToggle);

        setWindowTitle("Fluent UI Form Components Example");
        resize(500, 700);
    }

    void setupConnections() {
        // Connect form inputs for real-time validation feedback
        connect(m_nameInput, &Components::FluentTextInput::textChanged, this,
                [this]() { m_statusLabel->setText("Form modified..."); });

        connect(m_emailInput, &Components::FluentTextInput::textChanged, this,
                [this]() { m_statusLabel->setText("Form modified..."); });

        connect(m_passwordInput, &Components::FluentTextInput::textChanged,
                this, [this]() { m_statusLabel->setText("Form modified..."); });

        connect(m_agreeCheckBox, &Components::FluentCheckBox::toggled, this,
                [this]() { m_statusLabel->setText("Form modified..."); });
    }

private:
    Components::FluentTextInput* m_nameInput{nullptr};
    Components::FluentTextInput* m_emailInput{nullptr};
    Components::FluentTextInput* m_passwordInput{nullptr};
    Components::FluentCheckBox* m_agreeCheckBox{nullptr};
    Components::FluentCheckBox* m_newsletterCheckBox{nullptr};
    Components::FluentCheckBox* m_notificationsCheckBox{nullptr};
    QLabel* m_statusLabel{nullptr};
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize Fluent theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(QColor(0, 120, 215));  // Windows blue

    FormComponentsWindow window;
    window.show();

    return app.exec();
}

#include "FormComponentsExample.moc"
