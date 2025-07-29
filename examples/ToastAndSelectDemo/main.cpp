// examples/ToastAndSelectDemo/main.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QTimer>
#include <QProgressBar>
#include <QDebug>

#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentToastManager.h"
#include "FluentQt/Components/FluentSelect.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

class ToastAndSelectDemo : public QMainWindow {
    Q_OBJECT

public:
    ToastAndSelectDemo(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupToastManager();
        setupConnections();
        
        setWindowTitle("FluentQt - Toast and Select Demo");
        resize(800, 600);
        
        // Apply Fluent theme
        FluentTheme::instance().setMode(FluentThemeMode::Light);
    }

private slots:
    void showBasicToast() {
        FluentToastType type = static_cast<FluentToastType>(m_toastTypeSelect->currentData().toInt());
        QString title = m_toastTitleEdit->text().isEmpty() ? "Sample Toast" : m_toastTitleEdit->text();
        QString message = m_toastMessageEdit->text().isEmpty() ? "This is a sample toast message." : m_toastMessageEdit->text();
        
        switch (type) {
            case FluentToastType::Info:
                FluentToastGlobal::showInfo(title, message);
                break;
            case FluentToastType::Success:
                FluentToastGlobal::showSuccess(title, message);
                break;
            case FluentToastType::Warning:
                FluentToastGlobal::showWarning(title, message);
                break;
            case FluentToastType::Error:
                FluentToastGlobal::showError(title, message);
                break;
            case FluentToastType::Custom:
                FluentToastGlobal::showCustom(QIcon(":/icons/star.png"), title, message);
                break;
        }
    }
    
    void showActionToast() {
        auto* toast = FluentToast::createInfo("File Download", "Download completed successfully.");
        
        toast->addAction("Open", [=]() {
            FluentToastGlobal::showSuccess("Action", "File opened!");
        });
        
        toast->addAction("Show in Folder", [=]() {
            FluentToastGlobal::showInfo("Action", "Showing file in folder...");
        }, false);
        
        toast->show();
    }
    
    void showProgressToast() {
        auto* progressToast = FluentToast::createInfo("Processing", "Please wait...");
        progressToast->setShowProgress(true);
        progressToast->setProgressRange(0, 100);
        progressToast->setPersistent(true);
        progressToast->show();
        
        // Simulate progress
        auto* timer = new QTimer(this);
        int progress = 0;
        connect(timer, &QTimer::timeout, [=]() mutable {
            progress += 10;
            progressToast->setProgress(progress);
            
            if (progress >= 100) {
                timer->stop();
                progressToast->setTitle("Complete");
                progressToast->setMessage("Processing finished!");
                progressToast->setPersistent(false);
                progressToast->setDuration(3000);
                timer->deleteLater();
            }
        });
        timer->start(200);
    }
    
    void showPersistentToast() {
        auto* persistentToast = FluentToast::createWarning("Important Notice", "This message requires your attention.");
        persistentToast->setPersistent(true);
        persistentToast->setClosable(true);
        
        persistentToast->addAction("Acknowledge", [=]() {
            FluentToastGlobal::showSuccess("Acknowledged", "Thank you for your attention.");
            persistentToast->dismiss();
        });
        
        persistentToast->show();
    }
    
    void onToastPositionChanged() {
        FluentToastPosition position = static_cast<FluentToastPosition>(m_positionSelect->currentData().toInt());
        FluentToastManager::instance().setDefaultPosition(position);
    }
    
    void onMaxVisibleChanged(int value) {
        FluentToastManager::instance().setMaxVisible(value);
    }
    
    void hideAllToasts() {
        FluentToastManager::instance().hideAll();
    }
    
    void onCountryChanged() {
        QString country = m_countrySelect->currentText();
        if (!country.isEmpty()) {
            FluentToastGlobal::showInfo("Country Selected", QString("You selected: %1").arg(country));
        }
    }
    
    void onLanguagesChanged() {
        QStringList languages = m_languageSelect->selectedTexts();
        if (!languages.isEmpty()) {
            QString message = QString("Selected languages: %1").arg(languages.join(", "));
            FluentToastGlobal::showSuccess("Languages Updated", message);
        }
    }
    
    void onSkillsChanged() {
        QStringList skills = m_skillsSelect->selectedTexts();
        m_selectedSkillsLabel->setText(QString("Selected: %1").arg(skills.join(", ")));
    }
    
    void addCustomSkill() {
        QString skill = m_customSkillEdit->text().trimmed();
        if (!skill.isEmpty() && m_skillsSelect->findText(skill) == -1) {
            m_skillsSelect->addItem(skill);
            m_customSkillEdit->clear();
            FluentToastGlobal::showSuccess("Skill Added", QString("Added '%1' to skills list.").arg(skill));
        }
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget();
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QHBoxLayout(centralWidget);
        
        // Left side - Toast demos
        auto* toastGroup = new QGroupBox("Toast Notifications");
        auto* toastLayout = new QVBoxLayout(toastGroup);
        
        // Toast configuration
        auto* configLayout = new QGridLayout();
        
        configLayout->addWidget(new QLabel("Type:"), 0, 0);
        m_toastTypeSelect = new FluentSelect();
        m_toastTypeSelect->addItem("Info", static_cast<int>(FluentToastType::Info));
        m_toastTypeSelect->addItem("Success", static_cast<int>(FluentToastType::Success));
        m_toastTypeSelect->addItem("Warning", static_cast<int>(FluentToastType::Warning));
        m_toastTypeSelect->addItem("Error", static_cast<int>(FluentToastType::Error));
        m_toastTypeSelect->addItem("Custom", static_cast<int>(FluentToastType::Custom));
        configLayout->addWidget(m_toastTypeSelect, 0, 1);
        
        configLayout->addWidget(new QLabel("Title:"), 1, 0);
        m_toastTitleEdit = new QLineEdit();
        m_toastTitleEdit->setPlaceholderText("Toast title...");
        configLayout->addWidget(m_toastTitleEdit, 1, 1);
        
        configLayout->addWidget(new QLabel("Message:"), 2, 0);
        m_toastMessageEdit = new QLineEdit();
        m_toastMessageEdit->setPlaceholderText("Toast message...");
        configLayout->addWidget(m_toastMessageEdit, 2, 1);
        
        configLayout->addWidget(new QLabel("Position:"), 3, 0);
        m_positionSelect = new FluentSelect();
        m_positionSelect->addItem("Top Left", static_cast<int>(FluentToastPosition::TopLeft));
        m_positionSelect->addItem("Top Center", static_cast<int>(FluentToastPosition::TopCenter));
        m_positionSelect->addItem("Top Right", static_cast<int>(FluentToastPosition::TopRight));
        m_positionSelect->addItem("Bottom Left", static_cast<int>(FluentToastPosition::BottomLeft));
        m_positionSelect->addItem("Bottom Center", static_cast<int>(FluentToastPosition::BottomCenter));
        m_positionSelect->addItem("Bottom Right", static_cast<int>(FluentToastPosition::BottomRight));
        m_positionSelect->addItem("Center", static_cast<int>(FluentToastPosition::Center));
        m_positionSelect->setCurrentIndex(2); // Top Right
        configLayout->addWidget(m_positionSelect, 3, 1);
        
        configLayout->addWidget(new QLabel("Max Visible:"), 4, 0);
        m_maxVisibleSpin = new QSpinBox();
        m_maxVisibleSpin->setRange(1, 10);
        m_maxVisibleSpin->setValue(5);
        configLayout->addWidget(m_maxVisibleSpin, 4, 1);
        
        toastLayout->addLayout(configLayout);
        
        // Toast demo buttons
        auto* basicToastBtn = FluentButton::createPrimaryButton("Show Basic Toast");
        auto* actionToastBtn = new FluentButton("Show Action Toast");
        auto* progressToastBtn = new FluentButton("Show Progress Toast");
        auto* persistentToastBtn = new FluentButton("Show Persistent Toast");
        auto* hideAllBtn = new FluentButton("Hide All Toasts");
        hideAllBtn->setButtonStyle(FluentButtonStyle::Outline);
        
        toastLayout->addWidget(basicToastBtn);
        toastLayout->addWidget(actionToastBtn);
        toastLayout->addWidget(progressToastBtn);
        toastLayout->addWidget(persistentToastBtn);
        toastLayout->addWidget(hideAllBtn);
        toastLayout->addStretch();
        
        // Right side - Select demos
        auto* selectGroup = new QGroupBox("Select Components");
        auto* selectLayout = new QVBoxLayout(selectGroup);
        
        // Country selection (single)
        selectLayout->addWidget(new QLabel("Country (Single Selection):"));
        m_countrySelect = new FluentSelect();
        m_countrySelect->setPlaceholderText("Select your country...");
        m_countrySelect->addItems({"United States", "Canada", "United Kingdom", "Germany", "France", "Japan", "Australia"});
        selectLayout->addWidget(m_countrySelect);
        
        // Language selection (multiple)
        selectLayout->addWidget(new QLabel("Languages (Multiple Selection):"));
        m_languageSelect = new FluentSelect(FluentSelectMode::Multiple);
        m_languageSelect->setPlaceholderText("Select languages...");
        m_languageSelect->addItems({"English", "Spanish", "French", "German", "Chinese", "Japanese", "Korean", "Arabic"});
        selectLayout->addWidget(m_languageSelect);
        
        // Skills selection (searchable)
        selectLayout->addWidget(new QLabel("Skills (Searchable):"));
        m_skillsSelect = FluentSelect::createSearchableSelect();
        m_skillsSelect->setPlaceholderText("Search and select skills...");
        m_skillsSelect->addItems({"C++", "Python", "JavaScript", "Java", "C#", "Go", "Rust", "Swift", "Kotlin", "TypeScript"});
        selectLayout->addWidget(m_skillsSelect);
        
        m_selectedSkillsLabel = new QLabel("Selected: None");
        m_selectedSkillsLabel->setWordWrap(true);
        selectLayout->addWidget(m_selectedSkillsLabel);
        
        // Custom skill input
        auto* customSkillLayout = new QHBoxLayout();
        m_customSkillEdit = new QLineEdit();
        m_customSkillEdit->setPlaceholderText("Add custom skill...");
        auto* addSkillBtn = new FluentButton("Add");
        addSkillBtn->setButtonStyle(FluentButtonStyle::Accent);
        customSkillLayout->addWidget(m_customSkillEdit);
        customSkillLayout->addWidget(addSkillBtn);
        selectLayout->addLayout(customSkillLayout);
        
        selectLayout->addStretch();
        
        // Add groups to main layout
        mainLayout->addWidget(toastGroup);
        mainLayout->addWidget(selectGroup);
        
        // Store button references for connections
        m_basicToastBtn = basicToastBtn;
        m_actionToastBtn = actionToastBtn;
        m_progressToastBtn = progressToastBtn;
        m_persistentToastBtn = persistentToastBtn;
        m_hideAllBtn = hideAllBtn;
        m_addSkillBtn = addSkillBtn;
    }
    
    void setupToastManager() {
        auto& manager = FluentToastManager::instance();
        manager.setParentWidget(this);
        manager.setDefaultPosition(FluentToastPosition::TopRight);
        manager.setMaxVisible(5);
        manager.setStackSpacing(8);
        manager.setScreenMargin(16);
    }
    
    void setupConnections() {
        // Toast buttons
        connect(m_basicToastBtn, &FluentButton::clicked, this, &ToastAndSelectDemo::showBasicToast);
        connect(m_actionToastBtn, &FluentButton::clicked, this, &ToastAndSelectDemo::showActionToast);
        connect(m_progressToastBtn, &FluentButton::clicked, this, &ToastAndSelectDemo::showProgressToast);
        connect(m_persistentToastBtn, &FluentButton::clicked, this, &ToastAndSelectDemo::showPersistentToast);
        connect(m_hideAllBtn, &FluentButton::clicked, this, &ToastAndSelectDemo::hideAllToasts);
        
        // Toast configuration
        connect(m_positionSelect, QOverload<int>::of(&FluentSelect::currentIndexChanged), 
                this, &ToastAndSelectDemo::onToastPositionChanged);
        connect(m_maxVisibleSpin, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &ToastAndSelectDemo::onMaxVisibleChanged);
        
        // Select components
        connect(m_countrySelect, &FluentSelect::currentTextChanged, this, &ToastAndSelectDemo::onCountryChanged);
        connect(m_languageSelect, &FluentSelect::selectionChanged, this, &ToastAndSelectDemo::onLanguagesChanged);
        connect(m_skillsSelect, &FluentSelect::selectionChanged, this, &ToastAndSelectDemo::onSkillsChanged);
        
        // Custom skill
        connect(m_addSkillBtn, &FluentButton::clicked, this, &ToastAndSelectDemo::addCustomSkill);
        connect(m_customSkillEdit, &QLineEdit::returnPressed, this, &ToastAndSelectDemo::addCustomSkill);
    }

private:
    // Toast controls
    FluentSelect* m_toastTypeSelect;
    QLineEdit* m_toastTitleEdit;
    QLineEdit* m_toastMessageEdit;
    FluentSelect* m_positionSelect;
    QSpinBox* m_maxVisibleSpin;
    
    FluentButton* m_basicToastBtn;
    FluentButton* m_actionToastBtn;
    FluentButton* m_progressToastBtn;
    FluentButton* m_persistentToastBtn;
    FluentButton* m_hideAllBtn;
    
    // Select controls
    FluentSelect* m_countrySelect;
    FluentSelect* m_languageSelect;
    FluentSelect* m_skillsSelect;
    QLineEdit* m_customSkillEdit;
    FluentButton* m_addSkillBtn;
    QLabel* m_selectedSkillsLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    ToastAndSelectDemo demo;
    demo.show();
    
    return app.exec();
}

#include "main.moc"
