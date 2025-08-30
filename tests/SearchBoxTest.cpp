// tests/SearchBoxTest.cpp
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include <QDebug>

#include "FluentQt/Components/FluentSearchBox.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

class SearchBoxTestWidget : public QWidget {
    Q_OBJECT

public:
    explicit SearchBoxTestWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupSampleData();
        connectSignals();
        
        // Set window properties
        setWindowTitle("FluentSearchBox Test");
        setMinimumSize(900, 800);
        
        // Apply Fluent theme
        FluentTheme::instance().setMode(FluentThemeMode::Light);
    }

private slots:
    void onSearchTextChanged(const QString& text) {
        m_statusLabel->setText(QString("Search text changed: '%1'").arg(text));
        m_logEdit->append(QString("Text changed: %1").arg(text));
    }
    
    void onSearchRequested(const QString& text) {
        m_statusLabel->setText(QString("Search requested: '%1'").arg(text));
        m_logEdit->append(QString("Search requested: %1").arg(text));
        
        // Simulate search results
        m_resultsList->clear();
        for (int i = 0; i < 5; ++i) {
            m_resultsList->addItem(QString("Result %1 for '%2'").arg(i + 1).arg(text));
        }
    }
    
    void onSuggestionSelected(const FluentSearchSuggestion& suggestion) {
        m_statusLabel->setText(QString("Suggestion selected: '%1'").arg(suggestion.text));
        m_logEdit->append(QString("Suggestion selected: %1 - %2").arg(suggestion.text).arg(suggestion.description));
    }
    
    void onSearchCleared() {
        m_statusLabel->setText("Search cleared");
        m_logEdit->append("Search cleared");
        m_resultsList->clear();
    }
    
    void onSearchFocusReceived() {
        m_statusLabel->setText("Search box focused");
        m_logEdit->append("Search box focused");
    }
    
    void onSearchFocusLost() {
        m_statusLabel->setText("Search box lost focus");
        m_logEdit->append("Search box lost focus");
    }
    
    void onThemeToggle() {
        auto& theme = FluentTheme::instance();
        auto currentMode = theme.mode();
        auto newMode = (currentMode == FluentThemeMode::Light) 
                      ? FluentThemeMode::Dark 
                      : FluentThemeMode::Light;
        theme.setMode(newMode);
        
        m_themeButton->setText(newMode == FluentThemeMode::Dark 
                              ? "Switch to Light Theme" 
                              : "Switch to Dark Theme");
    }
    
    void onShowSuggestionsToggle(bool show) {
        m_searchBox->setShowSuggestions(show);
        m_commandPalette->setShowSuggestions(show);
        m_statusLabel->setText(show ? "Suggestions enabled" : "Suggestions disabled");
    }
    
    void onMaxSuggestionsChanged(int max) {
        m_searchBox->setMaxSuggestions(max);
        m_commandPalette->setMaxSuggestions(max);
        m_statusLabel->setText(QString("Max suggestions set to: %1").arg(max));
    }
    
    void onSearchDelayChanged(int delay) {
        m_searchBox->setSearchDelay(delay);
        m_commandPalette->setSearchDelay(delay);
        m_statusLabel->setText(QString("Search delay set to: %1ms").arg(delay));
    }
    
    void onAddSuggestion() {
        static int counter = 1;
        FluentSearchSuggestion suggestion;
        suggestion.text = QString("Custom Suggestion %1").arg(counter);
        suggestion.description = QString("This is custom suggestion number %1").arg(counter);
        suggestion.onSelected = [this, counter]() {
            m_logEdit->append(QString("Custom suggestion %1 selected!").arg(counter));
        };
        
        m_searchBox->addSuggestion(suggestion);
        m_statusLabel->setText(QString("Added custom suggestion %1").arg(counter));
        counter++;
    }
    
    void onClearSuggestions() {
        m_searchBox->clearSuggestions();
        m_commandPalette->clearCommands();
        m_statusLabel->setText("All suggestions cleared");
        setupSampleData(); // Re-add sample data
    }
    
    void onShowCommandPalette() {
        m_commandPalette->showPalette();
    }
    
    void onCommandExecuted(const QString& id) {
        m_statusLabel->setText(QString("Command executed: %1").arg(id));
        m_logEdit->append(QString("Command executed: %1").arg(id));
    }
    
    void onTagsChanged(const QStringList& tags) {
        m_statusLabel->setText(QString("Tags: %1").arg(tags.join(", ")));
        m_logEdit->append(QString("Tags changed: %1").arg(tags.join(", ")));
    }

private:
    void setupUI() {
        auto* mainLayout = new QVBoxLayout(this);
        
        // Title
        auto* titleLabel = new QLabel("FluentSearchBox Component Test");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
        mainLayout->addWidget(titleLabel);
        
        // Status label
        m_statusLabel = new QLabel("Use search components to see interactions");
        m_statusLabel->setStyleSheet("color: blue; margin: 5px;");
        mainLayout->addWidget(m_statusLabel);
        
        // Control panel
        setupControlPanel(mainLayout);
        
        // Search components demonstration
        setupSearchDemonstrations(mainLayout);
        
        // Results and log
        setupResultsAndLog(mainLayout);
        
        mainLayout->addStretch();
    }
    
    void setupControlPanel(QVBoxLayout* mainLayout) {
        auto* controlGroup = new QGroupBox("Controls");
        auto* controlLayout = new QVBoxLayout(controlGroup);
        
        // First row of controls
        auto* row1Layout = new QHBoxLayout();
        
        m_themeButton = new QPushButton("Switch to Dark Theme");
        row1Layout->addWidget(m_themeButton);
        
        row1Layout->addSpacing(20);
        
        // Show suggestions control
        m_showSuggestionsCheckBox = new QCheckBox("Show Suggestions");
        m_showSuggestionsCheckBox->setChecked(true);
        row1Layout->addWidget(m_showSuggestionsCheckBox);
        
        row1Layout->addSpacing(20);
        
        // Max suggestions control
        row1Layout->addWidget(new QLabel("Max Suggestions:"));
        m_maxSuggestionsSpinBox = new QSpinBox();
        m_maxSuggestionsSpinBox->setRange(1, 20);
        m_maxSuggestionsSpinBox->setValue(10);
        row1Layout->addWidget(m_maxSuggestionsSpinBox);
        
        row1Layout->addStretch();
        controlLayout->addLayout(row1Layout);
        
        // Second row of controls
        auto* row2Layout = new QHBoxLayout();
        
        // Search delay control
        row2Layout->addWidget(new QLabel("Search Delay (ms):"));
        m_searchDelaySpinBox = new QSpinBox();
        m_searchDelaySpinBox->setRange(0, 2000);
        m_searchDelaySpinBox->setValue(300);
        m_searchDelaySpinBox->setSingleStep(100);
        row2Layout->addWidget(m_searchDelaySpinBox);
        
        row2Layout->addSpacing(20);
        
        // Action buttons
        m_addSuggestionButton = new QPushButton("Add Custom Suggestion");
        row2Layout->addWidget(m_addSuggestionButton);
        
        m_clearSuggestionsButton = new QPushButton("Clear Suggestions");
        row2Layout->addWidget(m_clearSuggestionsButton);
        
        m_showPaletteButton = new QPushButton("Show Command Palette");
        row2Layout->addWidget(m_showPaletteButton);
        
        row2Layout->addStretch();
        controlLayout->addLayout(row2Layout);
        
        mainLayout->addWidget(controlGroup);
    }
    
    void setupSearchDemonstrations(QVBoxLayout* mainLayout) {
        // Basic Search Box Group
        auto* searchGroup = new QGroupBox("Search Components");
        auto* searchLayout = new QVBoxLayout(searchGroup);
        
        // Basic search box
        searchLayout->addWidget(new QLabel("Basic Search Box:"));
        m_searchBox = new FluentSearchBox(this);
        m_searchBox->setPlaceholderText("Search for anything...");
        searchLayout->addWidget(m_searchBox);
        
        searchLayout->addSpacing(10);
        
        // Command palette (hidden by default)
        m_commandPalette = new FluentCommandPalette(this);
        m_commandPalette->setPlaceholderText("Type a command...");
        m_commandPalette->hide();
        
        // Tag input
        searchLayout->addWidget(new QLabel("Tag Input:"));
        m_tagInput = new FluentTagInput(this);
        m_tagInput->setPlaceholderText("Add tags...");
        m_tagInput->setTags({"example", "tag", "input"});
        searchLayout->addWidget(m_tagInput);
        
        mainLayout->addWidget(searchGroup);
    }
    
    void setupResultsAndLog(QVBoxLayout* mainLayout) {
        auto* bottomLayout = new QHBoxLayout();
        
        // Results list
        auto* resultsGroup = new QGroupBox("Search Results");
        auto* resultsLayout = new QVBoxLayout(resultsGroup);
        
        m_resultsList = new QListWidget(this);
        m_resultsList->setMaximumHeight(150);
        resultsLayout->addWidget(m_resultsList);
        
        bottomLayout->addWidget(resultsGroup);
        
        // Log
        auto* logGroup = new QGroupBox("Event Log");
        auto* logLayout = new QVBoxLayout(logGroup);
        
        m_logEdit = new QTextEdit(this);
        m_logEdit->setMaximumHeight(150);
        m_logEdit->setReadOnly(true);
        logLayout->addWidget(m_logEdit);
        
        auto* clearLogButton = new QPushButton("Clear Log");
        connect(clearLogButton, &QPushButton::clicked, m_logEdit, &QTextEdit::clear);
        logLayout->addWidget(clearLogButton);
        
        bottomLayout->addWidget(logGroup);
        
        mainLayout->addLayout(bottomLayout);
    }
    
    void setupSampleData() {
        // Add sample suggestions to search box
        QList<FluentSearchSuggestion> suggestions;
        
        FluentSearchSuggestion suggestion1;
        suggestion1.text = "Apple";
        suggestion1.description = "A red or green fruit";
        suggestion1.onSelected = [this]() { m_logEdit->append("Apple selected!"); };
        suggestions.append(suggestion1);
        
        FluentSearchSuggestion suggestion2;
        suggestion2.text = "Application";
        suggestion2.description = "Software program";
        suggestion2.onSelected = [this]() { m_logEdit->append("Application selected!"); };
        suggestions.append(suggestion2);
        
        FluentSearchSuggestion suggestion3;
        suggestion3.text = "Banana";
        suggestion3.description = "A yellow fruit";
        suggestion3.onSelected = [this]() { m_logEdit->append("Banana selected!"); };
        suggestions.append(suggestion3);
        
        FluentSearchSuggestion suggestion4;
        suggestion4.text = "Book";
        suggestion4.description = "Reading material";
        suggestion4.onSelected = [this]() { m_logEdit->append("Book selected!"); };
        suggestions.append(suggestion4);
        
        FluentSearchSuggestion suggestion5;
        suggestion5.text = "Computer";
        suggestion5.description = "Electronic device";
        suggestion5.onSelected = [this]() { m_logEdit->append("Computer selected!"); };
        suggestions.append(suggestion5);
        
        m_searchBox->setSuggestions(suggestions);
        
        // Add sample commands to command palette
        FluentCommandPalette::FluentCommand cmd1;
        cmd1.id = "new_file";
        cmd1.name = "New File";
        cmd1.description = "Create a new file";
        cmd1.keywords = {"create", "new", "file"};
        cmd1.execute = [this]() { m_logEdit->append("New file created!"); };
        m_commandPalette->addCommand(cmd1);
        
        FluentCommandPalette::FluentCommand cmd2;
        cmd2.id = "open_file";
        cmd2.name = "Open File";
        cmd2.description = "Open an existing file";
        cmd2.keywords = {"open", "load", "file"};
        cmd2.execute = [this]() { m_logEdit->append("File opened!"); };
        m_commandPalette->addCommand(cmd2);
        
        FluentCommandPalette::FluentCommand cmd3;
        cmd3.id = "save_file";
        cmd3.name = "Save File";
        cmd3.description = "Save the current file";
        cmd3.keywords = {"save", "store", "file"};
        cmd3.execute = [this]() { m_logEdit->append("File saved!"); };
        m_commandPalette->addCommand(cmd3);
        
        FluentCommandPalette::FluentCommand cmd4;
        cmd4.id = "settings";
        cmd4.name = "Settings";
        cmd4.description = "Open application settings";
        cmd4.keywords = {"settings", "preferences", "config"};
        cmd4.execute = [this]() { m_logEdit->append("Settings opened!"); };
        m_commandPalette->addCommand(cmd4);
        
        // Set tag suggestions
        m_tagInput->setSuggestions({"programming", "design", "testing", "documentation", "bug", "feature", "enhancement"});
    }
    
    void connectSignals() {
        // Search box signals
        connect(m_searchBox, &FluentSearchBox::textChanged,
                this, &SearchBoxTestWidget::onSearchTextChanged);
        connect(m_searchBox, &FluentSearchBox::searchRequested,
                this, &SearchBoxTestWidget::onSearchRequested);
        connect(m_searchBox, &FluentSearchBox::suggestionSelected,
                this, &SearchBoxTestWidget::onSuggestionSelected);
        connect(m_searchBox, &FluentSearchBox::cleared,
                this, &SearchBoxTestWidget::onSearchCleared);
        connect(m_searchBox, &FluentSearchBox::focusReceived,
                this, &SearchBoxTestWidget::onSearchFocusReceived);
        connect(m_searchBox, &FluentSearchBox::focusLost,
                this, &SearchBoxTestWidget::onSearchFocusLost);
        
        // Command palette signals
        connect(m_commandPalette, &FluentCommandPalette::commandExecuted,
                this, &SearchBoxTestWidget::onCommandExecuted);
        
        // Tag input signals
        connect(m_tagInput, &FluentTagInput::tagsChanged,
                this, &SearchBoxTestWidget::onTagsChanged);
        
        // Control signals
        connect(m_themeButton, &QPushButton::clicked,
                this, &SearchBoxTestWidget::onThemeToggle);
        connect(m_showSuggestionsCheckBox, &QCheckBox::toggled,
                this, &SearchBoxTestWidget::onShowSuggestionsToggle);
        connect(m_maxSuggestionsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &SearchBoxTestWidget::onMaxSuggestionsChanged);
        connect(m_searchDelaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &SearchBoxTestWidget::onSearchDelayChanged);
        connect(m_addSuggestionButton, &QPushButton::clicked,
                this, &SearchBoxTestWidget::onAddSuggestion);
        connect(m_clearSuggestionsButton, &QPushButton::clicked,
                this, &SearchBoxTestWidget::onClearSuggestions);
        connect(m_showPaletteButton, &QPushButton::clicked,
                this, &SearchBoxTestWidget::onShowCommandPalette);
    }

private:
    // UI components
    QLabel* m_statusLabel{nullptr};
    QPushButton* m_themeButton{nullptr};
    QCheckBox* m_showSuggestionsCheckBox{nullptr};
    QSpinBox* m_maxSuggestionsSpinBox{nullptr};
    QSpinBox* m_searchDelaySpinBox{nullptr};
    QPushButton* m_addSuggestionButton{nullptr};
    QPushButton* m_clearSuggestionsButton{nullptr};
    QPushButton* m_showPaletteButton{nullptr};
    
    // Search components
    FluentSearchBox* m_searchBox{nullptr};
    FluentCommandPalette* m_commandPalette{nullptr};
    FluentTagInput* m_tagInput{nullptr};
    
    // Results and log
    QListWidget* m_resultsList{nullptr};
    QTextEdit* m_logEdit{nullptr};
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    SearchBoxTestWidget window;
    window.show();
    
    return app.exec();
}

#include "SearchBoxTest.moc"
