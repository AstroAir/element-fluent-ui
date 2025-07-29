#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QKeyEvent>
#include <QTimer>
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Styling/FluentTheme.h"

class AccessibleFormBuilder : public QMainWindow {
    Q_OBJECT
public:
    AccessibleFormBuilder(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupAccessibility();
        connectSignals();
        setWindowTitle("Accessible Form Builder - Element Fluent UI Demo");
        resize(900, 700);
    }

protected:
    void keyPressEvent(QKeyEvent* event) override {
        // Handle global keyboard shortcuts
        if (event->modifiers() & Qt::AltModifier) {
            switch (event->key()) {
            case Qt::Key_1:
                focusPersonalInfo();
                event->accept();
                return;
            case Qt::Key_2:
                focusContactInfo();
                event->accept();
                return;
            case Qt::Key_3:
                focusPreferences();
                event->accept();
                return;
            case Qt::Key_S:
                if (event->modifiers() & Qt::ControlModifier) {
                    submitForm();
                    event->accept();
                    return;
                }
                break;
            }
        }
        
        QMainWindow::keyPressEvent(event);
    }

private slots:
    void validateField(QWidget* field) {
        auto* textInput = qobject_cast<FluentTextInput*>(field);
        if (!textInput) return;
        
        QString value = textInput->text();
        QString fieldName = textInput->accessibleName();
        
        // Email validation
        if (textInput == m_emailInput) {
            QRegularExpression emailRegex("^[\\w\\.-]+@[\\w\\.-]+\\.[a-zA-Z]{2,}$");
            if (value.isEmpty()) {
                setFieldError(textInput, "Email address is required");
            } else if (!emailRegex.match(value).hasMatch()) {
                setFieldError(textInput, "Please enter a valid email address");
            } else {
                setFieldSuccess(textInput, "Valid email address");
            }
        }
        // Phone validation
        else if (textInput == m_phoneInput) {
            QRegularExpression phoneRegex("^[\\+]?[1-9]?[0-9]{7,15}$");
            if (value.isEmpty()) {
                setFieldError(textInput, "Phone number is required");
            } else if (!phoneRegex.match(value.replace(QRegularExpression("[\\s\\-\\(\\)]"), "")).hasMatch()) {
                setFieldError(textInput, "Please enter a valid phone number");
            } else {
                setFieldSuccess(textInput, "Valid phone number");
            }
        }
        // Required field validation
        else if (textInput->property("required").toBool()) {
            if (value.isEmpty()) {
                setFieldError(textInput, QString("%1 is required").arg(fieldName));
            } else {
                setFieldSuccess(textInput, QString("Valid %1").arg(fieldName.toLower()));
            }
        }
        
        updateFormValidation();
    }
    
    void setFieldError(FluentTextInput* field, const QString& message) {
        field->setInputState(FluentTextInputState::Error);
        field->setHelperText(message);
        
        // Announce error to screen readers
        QString announcement = QString("Error in %1: %2").arg(field->accessibleName(), message);
        announceToScreenReader(announcement);
    }
    
    void setFieldSuccess(FluentTextInput* field, const QString& message) {
        field->setInputState(FluentTextInputState::Success);
        field->setHelperText(message);
    }
    
    void updateFormValidation() {
        bool isValid = true;
        QStringList errors;
        
        // Check all required fields
        for (auto* input : {m_firstNameInput, m_lastNameInput, m_emailInput, m_phoneInput}) {
            if (input->inputState() == FluentTextInputState::Error) {
                isValid = false;
                errors.append(input->helperText());
            }
        }
        
        m_submitButton->setEnabled(isValid);
        
        if (isValid) {
            m_submitButton->setButtonStyle(FluentButtonStyle::Primary);
            m_formStatusLabel->setText("Form is ready to submit");
            m_formStatusLabel->setStyleSheet("color: #107c10;");
        } else {
            m_submitButton->setButtonStyle(FluentButtonStyle::Subtle);
            m_formStatusLabel->setText(QString("%1 error(s) found").arg(errors.size()));
            m_formStatusLabel->setStyleSheet("color: #d83b01;");
        }
    }
    
    void submitForm() {
        if (!m_submitButton->isEnabled()) {
            announceToScreenReader("Cannot submit form: Please fix validation errors first");
            return;
        }
        
        m_submitButton->setLoading(true);
        m_submitButton->setText("Submitting...");
        announceToScreenReader("Form submission in progress");
        
        // Simulate form submission
        QTimer::singleShot(2000, [this]() {
            m_submitButton->setLoading(false);
            m_submitButton->setText("Submit Form");
            
            FluentToastGlobal::showSuccess("Success", "Form submitted successfully!");
            announceToScreenReader("Form submitted successfully");
            
            // Reset form
            QTimer::singleShot(1000, [this]() {
                resetForm();
            });
        });
    }
    
    void resetForm() {
        // Clear all inputs
        for (auto* input : {m_firstNameInput, m_lastNameInput, m_emailInput, m_phoneInput, 
                           m_addressInput, m_cityInput, m_zipInput}) {
            input->clear();
            input->setInputState(FluentTextInputState::Normal);
            input->setHelperText("");
        }
        
        // Reset selections
        m_countryCombo->setCurrentIndex(0);
        m_genderGroup->button(0)->setChecked(true);
        
        // Reset checkboxes
        m_newsletterCheck->setChecked(false);
        m_updatesCheck->setChecked(false);
        m_marketingCheck->setChecked(false);
        
        updateFormValidation();
        announceToScreenReader("Form has been reset");
        
        // Focus first field
        m_firstNameInput->setFocus();
    }
    
    void focusPersonalInfo() {
        m_firstNameInput->setFocus();
        announceToScreenReader("Focused on Personal Information section");
    }
    
    void focusContactInfo() {
        m_emailInput->setFocus();
        announceToScreenReader("Focused on Contact Information section");
    }
    
    void focusPreferences() {
        m_newsletterCheck->setFocus();
        announceToScreenReader("Focused on Preferences section");
    }
    
    void toggleHighContrast() {
        auto& theme = FluentTheme::instance();
        bool isHighContrast = theme.isHighContrastMode();
        theme.setHighContrastMode(!isHighContrast);
        
        QString mode = isHighContrast ? "normal" : "high contrast";
        announceToScreenReader(QString("Switched to %1 mode").arg(mode));
        
        m_contrastButton->setText(isHighContrast ? "Enable High Contrast" : "Disable High Contrast");
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget;
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QVBoxLayout(centralWidget);
        
        // Header with accessibility tools
        setupAccessibilityToolbar(mainLayout);
        
        // Scrollable form area
        auto* scrollArea = new QScrollArea;
        auto* formWidget = new QWidget;
        auto* formLayout = new QVBoxLayout(formWidget);
        
        // Form sections
        setupPersonalInfoSection(formLayout);
        setupContactInfoSection(formLayout);
        setupPreferencesSection(formLayout);
        setupFormActions(formLayout);
        
        scrollArea->setWidget(formWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        
        mainLayout->addWidget(scrollArea);
        
        // Status bar
        setupStatusBar(mainLayout);
    }
    
    void setupAccessibilityToolbar(QVBoxLayout* layout) {
        auto* toolbarCard = new FluentCard("Accessibility Tools");
        auto* toolbarLayout = new QHBoxLayout;
        
        auto* titleLabel = new QLabel("Accessible Form Builder");
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
        
        // Keyboard shortcuts info
        auto* shortcutsLabel = new QLabel("Shortcuts: Alt+1 (Personal), Alt+2 (Contact), Alt+3 (Preferences), Ctrl+Alt+S (Submit)");
        shortcutsLabel->setStyleSheet("color: gray; font-size: 12px;");
        
        m_contrastButton = new FluentButton("Enable High Contrast");
        m_contrastButton->setButtonStyle(FluentButtonStyle::Subtle);
        m_contrastButton->setAccessibleName("Toggle High Contrast Mode");
        m_contrastButton->setAccessibleDescription("Switch between normal and high contrast display modes");
        connect(m_contrastButton, &FluentButton::clicked, this, &AccessibleFormBuilder::toggleHighContrast);
        
        auto* helpButton = new FluentButton("Help");
        helpButton->setButtonStyle(FluentButtonStyle::Subtle);
        helpButton->setAccessibleName("Show Help");
        helpButton->setAccessibleDescription("Display keyboard shortcuts and accessibility features");
        
        toolbarLayout->addWidget(titleLabel);
        toolbarLayout->addStretch();
        toolbarLayout->addWidget(m_contrastButton);
        toolbarLayout->addWidget(helpButton);
        
        auto* toolbarWidget = new QWidget;
        toolbarWidget->setLayout(toolbarLayout);
        
        auto* toolbarMainLayout = new QVBoxLayout;
        toolbarMainLayout->addWidget(toolbarWidget);
        toolbarMainLayout->addWidget(shortcutsLabel);
        
        toolbarCard->setContentWidget(new QWidget);
        toolbarCard->contentWidget()->setLayout(toolbarMainLayout);
        
        layout->addWidget(toolbarCard);
    }
    
    void setupPersonalInfoSection(QVBoxLayout* layout) {
        auto* section = new QGroupBox("Personal Information");
        section->setAccessibleName("Personal Information Section");
        auto* sectionLayout = new QGridLayout(section);
        
        // First Name
        auto* firstNameLabel = new QLabel("First Name *");
        m_firstNameInput = new FluentTextInput();
        m_firstNameInput->setAccessibleName("First Name");
        m_firstNameInput->setAccessibleDescription("Enter your first name");
        m_firstNameInput->setProperty("required", true);
        m_firstNameInput->setPlaceholderText("Enter your first name");
        firstNameLabel->setBuddy(m_firstNameInput);
        
        // Last Name
        auto* lastNameLabel = new QLabel("Last Name *");
        m_lastNameInput = new FluentTextInput();
        m_lastNameInput->setAccessibleName("Last Name");
        m_lastNameInput->setAccessibleDescription("Enter your last name");
        m_lastNameInput->setProperty("required", true);
        m_lastNameInput->setPlaceholderText("Enter your last name");
        lastNameLabel->setBuddy(m_lastNameInput);
        
        // Gender (Radio buttons)
        auto* genderLabel = new QLabel("Gender");
        m_genderGroup = new QButtonGroup(this);
        
        auto* maleRadio = new FluentRadioButton("Male");
        auto* femaleRadio = new FluentRadioButton("Female");
        auto* otherRadio = new FluentRadioButton("Other");
        auto* preferNotRadio = new FluentRadioButton("Prefer not to say");
        
        maleRadio->setAccessibleName("Gender Male");
        femaleRadio->setAccessibleName("Gender Female");
        otherRadio->setAccessibleName("Gender Other");
        preferNotRadio->setAccessibleName("Gender Prefer not to say");
        
        m_genderGroup->addButton(maleRadio, 0);
        m_genderGroup->addButton(femaleRadio, 1);
        m_genderGroup->addButton(otherRadio, 2);
        m_genderGroup->addButton(preferNotRadio, 3);
        
        maleRadio->setChecked(true); // Default selection
        
        auto* genderLayout = new QHBoxLayout;
        genderLayout->addWidget(maleRadio);
        genderLayout->addWidget(femaleRadio);
        genderLayout->addWidget(otherRadio);
        genderLayout->addWidget(preferNotRadio);
        
        sectionLayout->addWidget(firstNameLabel, 0, 0);
        sectionLayout->addWidget(m_firstNameInput, 0, 1);
        sectionLayout->addWidget(lastNameLabel, 0, 2);
        sectionLayout->addWidget(m_lastNameInput, 0, 3);
        sectionLayout->addWidget(genderLabel, 1, 0);
        sectionLayout->addLayout(genderLayout, 1, 1, 1, 3);
        
        layout->addWidget(section);
    }
    
    void setupContactInfoSection(QVBoxLayout* layout) {
        auto* section = new QGroupBox("Contact Information");
        section->setAccessibleName("Contact Information Section");
        auto* sectionLayout = new QGridLayout(section);
        
        // Email
        auto* emailLabel = new QLabel("Email Address *");
        m_emailInput = new FluentTextInput();
        m_emailInput->setInputType(FluentTextInputType::Email);
        m_emailInput->setAccessibleName("Email Address");
        m_emailInput->setAccessibleDescription("Enter your email address for account notifications");
        m_emailInput->setProperty("required", true);
        m_emailInput->setPlaceholderText("Enter your email address");
        emailLabel->setBuddy(m_emailInput);
        
        // Phone
        auto* phoneLabel = new QLabel("Phone Number *");
        m_phoneInput = new FluentTextInput();
        m_phoneInput->setInputType(FluentTextInputType::Phone);
        m_phoneInput->setAccessibleName("Phone Number");
        m_phoneInput->setAccessibleDescription("Enter your phone number for contact purposes");
        m_phoneInput->setProperty("required", true);
        m_phoneInput->setPlaceholderText("Enter your phone number");
        phoneLabel->setBuddy(m_phoneInput);
        
        // Address
        auto* addressLabel = new QLabel("Address");
        m_addressInput = new FluentTextInput();
        m_addressInput->setAccessibleName("Street Address");
        m_addressInput->setAccessibleDescription("Enter your street address");
        m_addressInput->setPlaceholderText("Enter your street address");
        addressLabel->setBuddy(m_addressInput);
        
        // City
        auto* cityLabel = new QLabel("City");
        m_cityInput = new FluentTextInput();
        m_cityInput->setAccessibleName("City");
        m_cityInput->setAccessibleDescription("Enter your city");
        m_cityInput->setPlaceholderText("Enter your city");
        cityLabel->setBuddy(m_cityInput);
        
        // Country
        auto* countryLabel = new QLabel("Country");
        m_countryCombo = new FluentComboBox();
        m_countryCombo->setAccessibleName("Country");
        m_countryCombo->setAccessibleDescription("Select your country from the dropdown list");
        m_countryCombo->addItems({"United States", "Canada", "United Kingdom", "Germany", "France", "Australia", "Japan", "Other"});
        countryLabel->setBuddy(m_countryCombo);
        
        // ZIP Code
        auto* zipLabel = new QLabel("ZIP Code");
        m_zipInput = new FluentTextInput();
        m_zipInput->setAccessibleName("ZIP Code");
        m_zipInput->setAccessibleDescription("Enter your postal or ZIP code");
        m_zipInput->setPlaceholderText("Enter ZIP code");
        zipLabel->setBuddy(m_zipInput);
        
        sectionLayout->addWidget(emailLabel, 0, 0);
        sectionLayout->addWidget(m_emailInput, 0, 1);
        sectionLayout->addWidget(phoneLabel, 0, 2);
        sectionLayout->addWidget(m_phoneInput, 0, 3);
        sectionLayout->addWidget(addressLabel, 1, 0);
        sectionLayout->addWidget(m_addressInput, 1, 1, 1, 3);
        sectionLayout->addWidget(cityLabel, 2, 0);
        sectionLayout->addWidget(m_cityInput, 2, 1);
        sectionLayout->addWidget(countryLabel, 2, 2);
        sectionLayout->addWidget(m_countryCombo, 2, 3);
        sectionLayout->addWidget(zipLabel, 3, 0);
        sectionLayout->addWidget(m_zipInput, 3, 1);
        
        layout->addWidget(section);
    }
    
    void setupPreferencesSection(QVBoxLayout* layout) {
        auto* section = new QGroupBox("Preferences");
        section->setAccessibleName("Preferences Section");
        auto* sectionLayout = new QVBoxLayout(section);
        
        auto* prefsLabel = new QLabel("Email Preferences:");
        prefsLabel->setStyleSheet("font-weight: bold;");
        
        m_newsletterCheck = new FluentCheckBox("Subscribe to newsletter");
        m_newsletterCheck->setAccessibleName("Subscribe to newsletter");
        m_newsletterCheck->setAccessibleDescription("Receive our monthly newsletter with updates and news");
        
        m_updatesCheck = new FluentCheckBox("Receive product updates");
        m_updatesCheck->setAccessibleName("Receive product updates");
        m_updatesCheck->setAccessibleDescription("Get notified about new features and improvements");
        
        m_marketingCheck = new FluentCheckBox("Receive marketing emails");
        m_marketingCheck->setAccessibleName("Receive marketing emails");
        m_marketingCheck->setAccessibleDescription("Receive promotional offers and special deals");
        
        sectionLayout->addWidget(prefsLabel);
        sectionLayout->addWidget(m_newsletterCheck);
        sectionLayout->addWidget(m_updatesCheck);
        sectionLayout->addWidget(m_marketingCheck);
        
        layout->addWidget(section);
    }
    
    void setupFormActions(QVBoxLayout* layout) {
        auto* actionsCard = new FluentCard;
        auto* actionsLayout = new QHBoxLayout;
        
        m_submitButton = new FluentButton("Submit Form");
        m_submitButton->setButtonStyle(FluentButtonStyle::Subtle);
        m_submitButton->setAccessibleName("Submit Form");
        m_submitButton->setAccessibleDescription("Submit the completed form");
        m_submitButton->setEnabled(false);
        
        auto* resetButton = new FluentButton("Reset Form");
        resetButton->setButtonStyle(FluentButtonStyle::Subtle);
        resetButton->setAccessibleName("Reset Form");
        resetButton->setAccessibleDescription("Clear all form fields and start over");
        connect(resetButton, &FluentButton::clicked, this, &AccessibleFormBuilder::resetForm);
        
        actionsLayout->addStretch();
        actionsLayout->addWidget(resetButton);
        actionsLayout->addWidget(m_submitButton);
        
        actionsCard->setContentWidget(new QWidget);
        actionsCard->contentWidget()->setLayout(actionsLayout);
        
        layout->addWidget(actionsCard);
    }
    
    void setupStatusBar(QVBoxLayout* layout) {
        auto* statusPanel = new FluentPanel;
        statusPanel->setPanelType(FluentPanelType::Surface);
        statusPanel->setMaximumHeight(30);
        
        auto* statusLayout = statusPanel->createHorizontalLayout();
        
        m_formStatusLabel = new QLabel("Fill out required fields to enable submission");
        m_formStatusLabel->setStyleSheet("color: gray; font-size: 12px;");
        m_formStatusLabel->setAccessibleName("Form Status");
        
        auto* requiredLabel = new QLabel("* Required fields");
        requiredLabel->setStyleSheet("color: gray; font-size: 12px;");
        
        statusLayout->addWidget(m_formStatusLabel);
        statusLayout->addStretch();
        statusLayout->addWidget(requiredLabel);
        
        layout->addWidget(statusPanel);
    }
    
    void setupAccessibility() {
        // Set window properties for accessibility
        setAccessibleName("Accessible Form Builder");
        setAccessibleDescription("A demonstration of accessible form design with Element Fluent UI components");
        
        // Set up tab order for logical navigation
        QWidget::setTabOrder(m_firstNameInput, m_lastNameInput);
        QWidget::setTabOrder(m_lastNameInput, m_genderGroup->button(0));
        QWidget::setTabOrder(m_genderGroup->button(3), m_emailInput);
        QWidget::setTabOrder(m_emailInput, m_phoneInput);
        QWidget::setTabOrder(m_phoneInput, m_addressInput);
        QWidget::setTabOrder(m_addressInput, m_cityInput);
        QWidget::setTabOrder(m_cityInput, m_countryCombo);
        QWidget::setTabOrder(m_countryCombo, m_zipInput);
        QWidget::setTabOrder(m_zipInput, m_newsletterCheck);
        QWidget::setTabOrder(m_newsletterCheck, m_updatesCheck);
        QWidget::setTabOrder(m_updatesCheck, m_marketingCheck);
        QWidget::setTabOrder(m_marketingCheck, m_submitButton);
    }
    
    void connectSignals() {
        // Connect validation for required fields
        for (auto* input : {m_firstNameInput, m_lastNameInput, m_emailInput, m_phoneInput}) {
            connect(input, &FluentTextInput::textChanged, [this, input]() {
                // Debounced validation
                QTimer::singleShot(500, [this, input]() {
                    validateField(input);
                });
            });
        }
        
        connect(m_submitButton, &FluentButton::clicked, this, &AccessibleFormBuilder::submitForm);
        
        // Announce focus changes for screen readers
        for (auto* input : {m_firstNameInput, m_lastNameInput, m_emailInput, m_phoneInput, 
                           m_addressInput, m_cityInput, m_zipInput}) {
            input->installEventFilter(this);
        }
    }
    
    void announceToScreenReader(const QString& message) {
        // This would integrate with platform-specific screen reader APIs
        // For demonstration, we'll use debug output
        qDebug() << "Screen Reader Announcement:" << message;
        
        // In a real implementation, you would use:
        // - Windows: IAccessible or UI Automation
        // - macOS: NSAccessibility
        // - Linux: AT-SPI
    }
    
    // UI Components
    FluentTextInput* m_firstNameInput;
    FluentTextInput* m_lastNameInput;
    FluentTextInput* m_emailInput;
    FluentTextInput* m_phoneInput;
    FluentTextInput* m_addressInput;
    FluentTextInput* m_cityInput;
    FluentTextInput* m_zipInput;
    FluentComboBox* m_countryCombo;
    QButtonGroup* m_genderGroup;
    FluentCheckBox* m_newsletterCheck;
    FluentCheckBox* m_updatesCheck;
    FluentCheckBox* m_marketingCheck;
    FluentButton* m_submitButton;
    FluentButton* m_contrastButton;
    QLabel* m_formStatusLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Enable accessibility features
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    AccessibleFormBuilder form;
    form.show();
    
    return app.exec();
}

#include "main.moc"
