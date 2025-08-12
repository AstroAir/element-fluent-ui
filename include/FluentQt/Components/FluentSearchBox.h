#pragma once

#include <QCompleter>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPropertyAnimation>
#include <QStringListModel>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>

namespace FluentQt::Components {

/**
 * @brief Search suggestion item
 */
struct FluentSearchSuggestion {
    QString text;                      // Suggestion text
    QString description;               // Optional description
    QIcon icon;                        // Optional icon
    QVariantMap data;                  // Custom data
    std::function<void()> onSelected;  // Selection callback
};

/**
 * @brief Modern search box component with suggestions and animations
 */
class FluentSearchBox : public QWidget {
    Q_OBJECT
    Q_PROPERTY(
        QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(
        bool showSuggestions READ showSuggestions WRITE setShowSuggestions)
    Q_PROPERTY(int maxSuggestions READ maxSuggestions WRITE setMaxSuggestions)
    Q_PROPERTY(int searchDelay READ searchDelay WRITE setSearchDelay)

public:
    explicit FluentSearchBox(QWidget* parent = nullptr);
    ~FluentSearchBox() override;

    // Text properties
    QString text() const;
    void setText(const QString& text);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    // Suggestions
    bool showSuggestions() const { return m_showSuggestions; }
    void setShowSuggestions(bool show);

    int maxSuggestions() const { return m_maxSuggestions; }
    void setMaxSuggestions(int max);

    void addSuggestion(const FluentSearchSuggestion& suggestion);
    void setSuggestions(const QList<FluentSearchSuggestion>& suggestions);
    void clearSuggestions();

    // Search behavior
    int searchDelay() const { return m_searchDelay; }
    void setSearchDelay(int delay);

    void setSearchFunction(
        std::function<QList<FluentSearchSuggestion>(const QString&)> func);

    // Appearance
    void setSearchIcon(const QIcon& icon);
    void setClearIcon(const QIcon& icon);

    // State
    bool isEmpty() const;
    void clear();
    void selectAll();

public slots:
    void focus();
    void showSuggestionsList();
    void hideSuggestionsList();

signals:
    void textChanged(const QString& text);
    void searchRequested(const QString& text);
    void suggestionSelected(const FluentSearchSuggestion& suggestion);
    void cleared();
    void focusReceived();
    void focusLost();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;

private slots:
    void onTextChanged();
    void onSearchTimer();
    void onSuggestionClicked();
    void onClearClicked();
    void updateSuggestions();

private:
    void setupUI();
    void setupAnimations();
    void updateSuggestionsPosition();
    void animateSuggestions(bool show);
    void selectSuggestion(int index);
    void applySuggestion(const FluentSearchSuggestion& suggestion);
    void updateClearButtonVisibility();
    void updateSearchIcon();

    // UI components
    QHBoxLayout* m_mainLayout = nullptr;
    QLineEdit* m_lineEdit = nullptr;
    QToolButton* m_searchButton = nullptr;
    QToolButton* m_clearButton = nullptr;

    // Suggestions
    QWidget* m_suggestionsContainer = nullptr;
    QListWidget* m_suggestionsList = nullptr;
    QGraphicsOpacityEffect* m_suggestionsOpacity = nullptr;
    QPropertyAnimation* m_suggestionsAnimation = nullptr;

    // Data
    QList<FluentSearchSuggestion> m_suggestions;
    QList<FluentSearchSuggestion> m_filteredSuggestions;
    int m_selectedSuggestionIndex = -1;

    // Configuration
    bool m_showSuggestions = true;
    int m_maxSuggestions = 10;
    int m_searchDelay = 300;

    // Search function
    std::function<QList<FluentSearchSuggestion>(const QString&)>
        m_searchFunction;

    // Timers
    QTimer* m_searchTimer = nullptr;

    // Icons
    QIcon m_searchIcon;
    QIcon m_clearIcon;

    // State
    bool m_suggestionsVisible = false;
};

/**
 * @brief Enhanced command palette component
 */
class FluentCommandPalette : public FluentSearchBox {
    Q_OBJECT

public:
    struct FluentCommand {
        QString id;                     // Unique identifier
        QString name;                   // Display name
        QString description;            // Description
        QIcon icon;                     // Command icon
        QStringList keywords;           // Search keywords
        QKeySequence shortcut;          // Keyboard shortcut
        std::function<void()> execute;  // Command execution
        bool enabled = true;            // Whether command is enabled
        QString category;               // Command category
    };

    explicit FluentCommandPalette(QWidget* parent = nullptr);

    // Command management
    void addCommand(const FluentCommand& command);
    void removeCommand(const QString& id);
    void clearCommands();

    void setCommands(const QList<FluentCommand>& commands);
    QList<FluentCommand> commands() const { return m_commands; }

    // Categories
    void addCategory(const QString& category, const QIcon& icon = QIcon());
    void removeCategory(const QString& category);

    // Execution
    void executeCommand(const QString& id);

public slots:
    void showPalette();
    void hidePalette();

signals:
    void commandExecuted(const QString& id);
    void paletteShown();
    void paletteHidden();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private slots:
    void onCommandSearch(const QString& query);
    void onCommandSelected(const FluentSearchSuggestion& suggestion);

private:
    void setupCommandPalette();
    QList<FluentSearchSuggestion> searchCommands(const QString& query);
    FluentSearchSuggestion commandToSuggestion(const FluentCommand& command);

    QList<FluentCommand> m_commands;
    QHash<QString, QIcon> m_categories;
};

/**
 * @brief Tag input component for multiple selections
 */
class FluentTagInput : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(
        QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(
        bool allowDuplicates READ allowDuplicates WRITE setAllowDuplicates)

public:
    explicit FluentTagInput(QWidget* parent = nullptr);

    // Tags management
    QStringList tags() const { return m_tags; }
    void setTags(const QStringList& tags);

    void addTag(const QString& tag);
    void removeTag(const QString& tag);
    void removeTag(int index);
    void clearTags();

    // Configuration
    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    bool allowDuplicates() const { return m_allowDuplicates; }
    void setAllowDuplicates(bool allow);

    void setSuggestions(const QStringList& suggestions);
    void setValidator(std::function<bool(const QString&)> validator);

public slots:
    void focus();

signals:
    void tagsChanged(const QStringList& tags);
    void tagAdded(const QString& tag);
    void tagRemoved(const QString& tag);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onInputTextChanged();
    void onTagRemoveClicked();

private:
    void setupUI();
    void updateLayout();
    void addTagWidget(const QString& tag);
    void removeTagWidget(int index);
    bool isValidTag(const QString& tag) const;

    // UI components
    QWidget* m_tagsContainer = nullptr;
    QLineEdit* m_input = nullptr;
    QCompleter* m_completer = nullptr;

    // Data
    QStringList m_tags;
    QStringList m_suggestions;
    QList<QWidget*> m_tagWidgets;

    // Configuration
    bool m_allowDuplicates = false;
    std::function<bool(const QString&)> m_validator;
};

}  // namespace FluentQt::Components
