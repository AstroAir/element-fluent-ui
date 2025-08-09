// examples/AccessibilityEnhancementExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSlider>
#include <QProgressBar>
#include <QGroupBox>
#include <QTabWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QMenuBar>
#include <QStatusBar>
#include <QTimer>
#include <QMessageBox>

#include "FluentQt/Accessibility/FluentAccessibilityManager.h"
#include "FluentQt/Accessibility/FluentScreenReaderManager.h"
#include "FluentQt/Accessibility/FluentKeyboardNavigationManager.h"
#include "FluentQt/Accessibility/FluentWcagChecker.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

using namespace FluentQt;

class AccessibilityDemoWidget : public QWidget {
    Q_OBJECT

public:
    explicit AccessibilityDemoWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupAccessibility();
        connectSignals();
    }

private slots:
    void runAccessibilityCheck() {
        auto& manager = Accessibility::FluentAccessibilityManager::instance();
        auto issues = manager.validateWcag21Compliance(this);
        
        m_resultsText->clear();
        m_resultsText->append(QString("Found %1 accessibility issues:").arg(issues.size()));
        
        for (const auto& issue : issues) {
            QString severity;
            switch (issue.severity) {
                case 5: severity = "CRITICAL"; break;
                case 4: severity = "HIGH"; break;
                case 3: severity = "MEDIUM"; break;
                case 2: severity = "LOW"; break;
                default: severity = "INFO"; break;
            }
            
            m_resultsText->append(QString("[%1] %2: %3")
                                 .arg(severity)
                                 .arg(issue.widgetName)
                                 .arg(issue.description));
            m_resultsText->append(QString("  Suggestion: %1").arg(issue.suggestion));
            m_resultsText->append(QString("  WCAG: %1").arg(issue.wcagCriterion));
            m_resultsText->append("");
        }
        
        // Announce results to screen reader
        auto& screenReader = Accessibility::FluentScreenReaderManager::instance();
        screenReader.announce(QString("Accessibility check completed. Found %1 issues.").arg(issues.size()),
                             Accessibility::FluentAnnouncementPriority::High);
    }
    
    void applyAccessibilityFixes() {
        auto& manager = Accessibility::FluentAccessibilityManager::instance();
        auto issues = manager.validateWcag21Compliance(this);
        
        int fixedCount = 0;
        for (const auto& issue : issues) {
            if (issue.isAutomaticallyFixable) {
                // Find the widget and apply the fix
                QWidget* widget = findChild<QWidget*>(issue.widgetName);
                if (widget) {
                    manager.applyAccessibilityFix(widget, issue);
                    fixedCount++;
                }
            }
        }
        
        m_resultsText->append(QString("Applied %1 automatic fixes.").arg(fixedCount));
        
        // Announce to screen reader
        auto& screenReader = Accessibility::FluentScreenReaderManager::instance();
        screenReader.announce(QString("Applied %1 accessibility fixes.").arg(fixedCount),
                             Accessibility::FluentAnnouncementPriority::Medium);
    }
    
    void toggleHighContrast() {
        auto& manager = Accessibility::FluentAccessibilityManager::instance();
        bool enabled = !manager.isHighContrastModeEnabled();
        manager.setHighContrastMode(enabled);
        
        m_highContrastBtn->setText(enabled ? "Disable High Contrast" : "Enable High Contrast");
        
        // Update UI colors for high contrast
        if (enabled) {
            setStyleSheet("QWidget { background-color: black; color: white; } "
                         "QPushButton { background-color: #333; border: 2px solid white; } "
                         "QLineEdit { background-color: #222; border: 2px solid white; }");
        } else {
            setStyleSheet("");
        }
    }
    
    void toggleScreenReaderMode() {
        auto& screenReader = Accessibility::FluentScreenReaderManager::instance();
        bool enabled = !screenReader.isScreenReaderActive();
        screenReader.setScreenReaderActive(enabled);
        
        m_screenReaderBtn->setText(enabled ? "Disable Screen Reader Mode" : "Enable Screen Reader Mode");
        
        if (enabled) {
            screenReader.announce("Screen reader mode enabled. All interactions will be announced.",
                                 Accessibility::FluentAnnouncementPriority::High);
        }
    }
    
    void onFormFieldChanged() {
        auto& screenReader = Accessibility::FluentScreenReaderManager::instance();
        
        QWidget* sender = qobject_cast<QWidget*>(this->sender());
        if (sender) {
            QString announcement;
            if (auto* lineEdit = qobject_cast<QLineEdit*>(sender)) {
                announcement = QString("Field %1 changed to: %2")
                              .arg(lineEdit->accessibleName())
                              .arg(lineEdit->text());
            } else if (auto* comboBox = qobject_cast<QComboBox*>(sender)) {
                announcement = QString("Selection changed to: %1")
                              .arg(comboBox->currentText());
            } else if (auto* checkBox = qobject_cast<QCheckBox*>(sender)) {
                announcement = QString("Checkbox %1 %2")
                              .arg(checkBox->text())
                              .arg(checkBox->isChecked() ? "checked" : "unchecked");
            }
            
            if (!announcement.isEmpty()) {
                screenReader.announce(announcement, Accessibility::FluentAnnouncementPriority::Medium);
            }
        }
    }

private:
    void setupUI() {
        setWindowTitle("Accessibility Enhancement Demo");
        setMinimumSize(900, 700);
        
        auto* layout = new QVBoxLayout(this);
        
        // Create menu bar
        createMenuBar();
        
        // Main content area
        auto* splitter = new QSplitter(Qt::Horizontal, this);
        
        // Left panel: Form controls
        auto* leftPanel = createFormPanel();
        
        // Right panel: Results and controls
        auto* rightPanel = createControlPanel();
        
        splitter->addWidget(leftPanel);
        splitter->addWidget(rightPanel);
        splitter->setSizes({400, 500});
        
        layout->addWidget(splitter);
        
        // Status bar
        createStatusBar();
    }
    
    void createMenuBar() {
        // This would typically be done in the main window
        // For demo purposes, we'll create a simple menu structure
    }
    
    QWidget* createFormPanel() {
        auto* panel = new QGroupBox("Sample Form", this);
        panel->setObjectName("FormPanel");
        auto* layout = new QGridLayout(panel);
        
        // Name field
        auto* nameLabel = new QLabel("Name:", this);
        m_nameEdit = new QLineEdit(this);
        m_nameEdit->setObjectName("NameField");
        m_nameEdit->setAccessibleName("Full Name");
        m_nameEdit->setAccessibleDescription("Enter your full name");
        nameLabel->setBuddy(m_nameEdit);
        
        // Email field
        auto* emailLabel = new QLabel("Email:", this);
        m_emailEdit = new QLineEdit(this);
        m_emailEdit->setObjectName("EmailField");
        m_emailEdit->setAccessibleName("Email Address");
        m_emailEdit->setAccessibleDescription("Enter a valid email address");
        emailLabel->setBuddy(m_emailEdit);
        
        // Country selection
        auto* countryLabel = new QLabel("Country:", this);
        m_countryCombo = new QComboBox(this);
        m_countryCombo->setObjectName("CountryField");
        m_countryCombo->setAccessibleName("Country");
        m_countryCombo->addItems({"United States", "Canada", "United Kingdom", "Australia", "Germany", "France"});
        countryLabel->setBuddy(m_countryCombo);
        
        // Newsletter checkbox
        m_newsletterCheck = new QCheckBox("Subscribe to newsletter", this);
        m_newsletterCheck->setObjectName("NewsletterCheck");
        m_newsletterCheck->setAccessibleDescription("Receive monthly updates and news");
        
        // Age slider
        auto* ageLabel = new QLabel("Age:", this);
        m_ageSlider = new QSlider(Qt::Horizontal, this);
        m_ageSlider->setObjectName("AgeSlider");
        m_ageSlider->setAccessibleName("Age");
        m_ageSlider->setRange(18, 100);
        m_ageSlider->setValue(25);
        m_ageSlider->setAccessibleDescription("Select your age using the slider");
        
        auto* ageValueLabel = new QLabel("25", this);
        connect(m_ageSlider, &QSlider::valueChanged, ageValueLabel, 
                [ageValueLabel](int value) { ageValueLabel->setText(QString::number(value)); });
        
        // Comments
        auto* commentsLabel = new QLabel("Comments:", this);
        m_commentsEdit = new QTextEdit(this);
        m_commentsEdit->setObjectName("CommentsField");
        m_commentsEdit->setAccessibleName("Additional Comments");
        m_commentsEdit->setAccessibleDescription("Enter any additional comments or feedback");
        m_commentsEdit->setMaximumHeight(100);
        commentsLabel->setBuddy(m_commentsEdit);
        
        // Submit button
        m_submitBtn = new QPushButton("Submit Form", this);
        m_submitBtn->setObjectName("SubmitButton");
        m_submitBtn->setAccessibleDescription("Submit the form with entered information");
        
        // Layout
        layout->addWidget(nameLabel, 0, 0);
        layout->addWidget(m_nameEdit, 0, 1);
        layout->addWidget(emailLabel, 1, 0);
        layout->addWidget(m_emailEdit, 1, 1);
        layout->addWidget(countryLabel, 2, 0);
        layout->addWidget(m_countryCombo, 2, 1);
        layout->addWidget(m_newsletterCheck, 3, 0, 1, 2);
        layout->addWidget(ageLabel, 4, 0);
        auto* ageLayout = new QHBoxLayout();
        ageLayout->addWidget(m_ageSlider);
        ageLayout->addWidget(ageValueLabel);
        layout->addLayout(ageLayout, 4, 1);
        layout->addWidget(commentsLabel, 5, 0);
        layout->addWidget(m_commentsEdit, 5, 1);
        layout->addWidget(m_submitBtn, 6, 0, 1, 2);
        
        return panel;
    }
    
    QWidget* createControlPanel() {
        auto* panel = new QGroupBox("Accessibility Controls", this);
        auto* layout = new QVBoxLayout(panel);
        
        // Accessibility check button
        auto* checkBtn = new QPushButton("Run Accessibility Check", this);
        checkBtn->setAccessibleDescription("Perform WCAG 2.1 compliance check on the form");
        connect(checkBtn, &QPushButton::clicked, this, &AccessibilityDemoWidget::runAccessibilityCheck);
        
        // Auto-fix button
        auto* fixBtn = new QPushButton("Apply Automatic Fixes", this);
        fixBtn->setAccessibleDescription("Automatically fix accessibility issues where possible");
        connect(fixBtn, &QPushButton::clicked, this, &AccessibilityDemoWidget::applyAccessibilityFixes);
        
        // High contrast toggle
        m_highContrastBtn = new QPushButton("Enable High Contrast", this);
        m_highContrastBtn->setAccessibleDescription("Toggle high contrast mode for better visibility");
        connect(m_highContrastBtn, &QPushButton::clicked, this, &AccessibilityDemoWidget::toggleHighContrast);
        
        // Screen reader mode toggle
        m_screenReaderBtn = new QPushButton("Enable Screen Reader Mode", this);
        m_screenReaderBtn->setAccessibleDescription("Toggle screen reader announcements");
        connect(m_screenReaderBtn, &QPushButton::clicked, this, &AccessibilityDemoWidget::toggleScreenReaderMode);
        
        // Results area
        auto* resultsLabel = new QLabel("Accessibility Check Results:", this);
        m_resultsText = new QTextEdit(this);
        m_resultsText->setObjectName("ResultsArea");
        m_resultsText->setAccessibleName("Accessibility Check Results");
        m_resultsText->setAccessibleDescription("Results from accessibility compliance checking");
        m_resultsText->setReadOnly(true);
        resultsLabel->setBuddy(m_resultsText);
        
        layout->addWidget(checkBtn);
        layout->addWidget(fixBtn);
        layout->addWidget(m_highContrastBtn);
        layout->addWidget(m_screenReaderBtn);
        layout->addWidget(resultsLabel);
        layout->addWidget(m_resultsText);
        
        return panel;
    }
    
    void createStatusBar() {
        // This would typically be done in the main window
    }
    
    void setupAccessibility() {
        // Set up ARIA landmarks
        auto& screenReader = Accessibility::FluentScreenReaderManager::instance();
        
        // Set main content area as main landmark
        screenReader.setLandmark(this, Accessibility::FluentAriaRole::Main, "Main Content");
        
        // Set form as form landmark
        auto* formPanel = findChild<QGroupBox*>("FormPanel");
        if (formPanel) {
            screenReader.setLandmark(formPanel, Accessibility::FluentAriaRole::Form, "Contact Form");
        }
        
        // Set up live regions for dynamic content
        screenReader.setLiveRegion(m_resultsText, Accessibility::FluentLiveRegionType::Polite, true);
        
        // Set up keyboard navigation
        auto& keyboard = Accessibility::FluentKeyboardNavigationManager::instance();
        
        // Register all interactive widgets for keyboard navigation
        keyboard.registerNavigableWidget(m_nameEdit, 1);
        keyboard.registerNavigableWidget(m_emailEdit, 2);
        keyboard.registerNavigableWidget(m_countryCombo, 3);
        keyboard.registerNavigableWidget(m_newsletterCheck, 4);
        keyboard.registerNavigableWidget(m_ageSlider, 5);
        keyboard.registerNavigableWidget(m_commentsEdit, 6);
        keyboard.registerNavigableWidget(m_submitBtn, 7);
        
        // Add skip links
        keyboard.addSkipLink("Skip to form", findChild<QGroupBox*>("FormPanel"), QKeySequence("Alt+F"));
        keyboard.addSkipLink("Skip to results", m_resultsText, QKeySequence("Alt+R"));
    }
    
    void connectSignals() {
        // Connect form field changes for screen reader announcements
        connect(m_nameEdit, &QLineEdit::textChanged, this, &AccessibilityDemoWidget::onFormFieldChanged);
        connect(m_emailEdit, &QLineEdit::textChanged, this, &AccessibilityDemoWidget::onFormFieldChanged);
        connect(m_countryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &AccessibilityDemoWidget::onFormFieldChanged);
        connect(m_newsletterCheck, &QCheckBox::toggled, this, &AccessibilityDemoWidget::onFormFieldChanged);
        
        // Connect accessibility manager signals
        auto& manager = Accessibility::FluentAccessibilityManager::instance();
        connect(&manager, &Accessibility::FluentAccessibilityManager::accessibilityIssueFound,
                this, [this](const Accessibility::AccessibilityIssue& issue) {
                    qDebug() << "Accessibility issue found:" << issue.description;
                });
    }

private:
    // Form controls
    QLineEdit* m_nameEdit;
    QLineEdit* m_emailEdit;
    QComboBox* m_countryCombo;
    QCheckBox* m_newsletterCheck;
    QSlider* m_ageSlider;
    QTextEdit* m_commentsEdit;
    QPushButton* m_submitBtn;
    
    // Control panel
    QPushButton* m_highContrastBtn;
    QPushButton* m_screenReaderBtn;
    QTextEdit* m_resultsText;
};

class AccessibilityMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AccessibilityMainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupAccessibility();
    }

private:
    void setupUI() {
        setWindowTitle("FluentQt Accessibility Enhancement Demo");
        setMinimumSize(1000, 800);
        
        auto* centralWidget = new AccessibilityDemoWidget(this);
        setCentralWidget(centralWidget);
        
        // Create menu bar
        auto* fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("&New", this, &QWidget::close, QKeySequence::New);
        fileMenu->addAction("&Open", this, &QWidget::close, QKeySequence::Open);
        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);
        
        auto* accessibilityMenu = menuBar()->addMenu("&Accessibility");
        accessibilityMenu->addAction("&Check Compliance", [this]() {
            // Trigger accessibility check
        }, QKeySequence("Ctrl+Shift+A"));
        accessibilityMenu->addAction("&High Contrast", [this]() {
            // Toggle high contrast
        }, QKeySequence("Ctrl+Shift+H"));
        
        // Status bar
        statusBar()->showMessage("Ready - Press F1 for accessibility help");
    }
    
    void setupAccessibility() {
        // Set window role and properties
        auto& screenReader = Accessibility::FluentScreenReaderManager::instance();
        screenReader.setAriaRole(this, Accessibility::FluentAriaRole::Application);
        screenReader.setAriaLabel(this, "FluentQt Accessibility Demo Application");
        
        // Announce application startup
        QTimer::singleShot(1000, [&screenReader]() {
            screenReader.announce("FluentQt Accessibility Demo loaded. Use Tab to navigate, F1 for help.",
                                 Accessibility::FluentAnnouncementPriority::High);
        });
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize accessibility
    FluentQt::Accessibility::initializeAccessibility();
    
    AccessibilityMainWindow window;
    window.show();
    
    return app.exec();
}

#include "AccessibilityEnhancementExample.moc"
