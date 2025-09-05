// src/Components/FluentSearchBox.cpp
#include "FluentQt/Components/FluentSearchBox.h"
#include <QAccessible>
#include <QApplication>
#include <QCompleter>
#include <QFocusEvent>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSequentialAnimationGroup>
#include <QStringListModel>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentSearchBox::FluentSearchBox(QWidget* parent) : QWidget(parent) {
    // Initialize state
    m_isLoading = false;
    m_hasError = false;
    m_isDisabled = false;

    setupUI();
    setupAnimations();
    setupAccessibility();
    setupThemeIntegration();

    // Setup search timer with theme-based delay
    auto& theme = Styling::FluentTheme::instance();
    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchDelay = 300;  // Default, can be overridden by theme
    connect(m_searchTimer, &QTimer::timeout, this,
            &FluentSearchBox::onSearchTimer);

    // Connect signals
    connect(m_lineEdit, &QLineEdit::textChanged, this,
            &FluentSearchBox::onTextChanged);
    connect(m_clearButton, &QToolButton::clicked, this,
            &FluentSearchBox::onClearClicked);
    connect(m_searchButton, &QToolButton::clicked, this,
            &FluentSearchBox::onSearchButtonClicked);

    // Install event filter for suggestions
    m_suggestionsList->installEventFilter(this);

    // Apply initial theme
    applyTheme();
    updateClearButtonVisibility();

    // Connect to theme changes
    connect(&theme, &Styling::FluentTheme::themeChanged, this,
            &FluentSearchBox::applyTheme);
}

FluentSearchBox::~FluentSearchBox() = default;

QString FluentSearchBox::text() const { return m_lineEdit->text(); }

void FluentSearchBox::setText(const QString& text) {
    m_lineEdit->setText(text);
}

QString FluentSearchBox::placeholderText() const {
    return m_lineEdit->placeholderText();
}

void FluentSearchBox::setPlaceholderText(const QString& text) {
    m_lineEdit->setPlaceholderText(text);
}

void FluentSearchBox::setShowSuggestions(bool show) {
    if (m_showSuggestions != show) {
        m_showSuggestions = show;
        if (!show) {
            hideSuggestionsList();
        }
    }
}

void FluentSearchBox::setMaxSuggestions(int max) {
    m_maxSuggestions = qMax(1, max);
    updateSuggestions();
}

void FluentSearchBox::addSuggestion(const FluentSearchSuggestion& suggestion) {
    m_suggestions.append(suggestion);
    updateSuggestions();
}

void FluentSearchBox::setSuggestions(
    const QList<FluentSearchSuggestion>& suggestions) {
    m_suggestions = suggestions;
    updateSuggestions();
}

void FluentSearchBox::clearSuggestions() {
    m_suggestions.clear();
    m_filteredSuggestions.clear();
    m_suggestionsList->clear();
    hideSuggestionsList();
}

void FluentSearchBox::setSearchDelay(int delay) {
    m_searchDelay = qMax(0, delay);
}

void FluentSearchBox::setSearchFunction(
    std::function<QList<FluentSearchSuggestion>(const QString&)> func) {
    m_searchFunction = func;
}

void FluentSearchBox::setSearchIcon(const QIcon& icon) {
    m_searchIcon = icon;
    updateSearchIcon();
}

void FluentSearchBox::setClearIcon(const QIcon& icon) {
    m_clearIcon = icon;
    m_clearButton->setIcon(icon);
}

bool FluentSearchBox::isEmpty() const { return m_lineEdit->text().isEmpty(); }

void FluentSearchBox::clear() {
    m_lineEdit->clear();
    hideSuggestionsList();
    emit cleared();
}

void FluentSearchBox::selectAll() { m_lineEdit->selectAll(); }

void FluentSearchBox::focus() { m_lineEdit->setFocus(); }

// Enhanced state management methods
void FluentSearchBox::setLoading(bool loading) {
    if (m_isLoading != loading) {
        m_isLoading = loading;
        updateLoadingState();
        emit loadingStateChanged(loading);
    }
}

void FluentSearchBox::setError(bool hasError, const QString& errorMessage) {
    if (m_hasError != hasError || m_errorMessage != errorMessage) {
        m_hasError = hasError;
        m_errorMessage = errorMessage;
        updateErrorState();
        emit errorStateChanged(hasError, errorMessage);
    }
}

void FluentSearchBox::setDisabled(bool disabled) {
    if (m_isDisabled != disabled) {
        m_isDisabled = disabled;
        updateDisabledState();
        setEnabled(!disabled);
    }
}

// Enhanced accessibility methods
void FluentSearchBox::setAccessibleName(const QString& name) {
    m_accessibleName = name;
    setAccessibleName(name);
    if (m_lineEdit) {
        m_lineEdit->setAccessibleName(name);
    }
}

void FluentSearchBox::setAccessibleDescription(const QString& description) {
    m_accessibleDescription = description;
    setAccessibleDescription(description);
    if (m_lineEdit) {
        m_lineEdit->setAccessibleDescription(description);
    }
}

QString FluentSearchBox::accessibleName() const { return m_accessibleName; }

QString FluentSearchBox::accessibleDescription() const {
    return m_accessibleDescription;
}

void FluentSearchBox::showSuggestionsList() {
    if (!m_showSuggestions || m_filteredSuggestions.isEmpty()) {
        return;
    }

    updateSuggestionsPosition();
    m_suggestionsContainer->show();
    animateSuggestions(true);
    m_suggestionsVisible = true;
}

void FluentSearchBox::hideSuggestionsList() {
    if (m_suggestionsVisible) {
        animateSuggestions(false);
        m_suggestionsVisible = false;
        m_selectedSuggestionIndex = -1;
    }
}

void FluentSearchBox::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateSuggestionsPosition();
}

void FluentSearchBox::keyPressEvent(QKeyEvent* event) {
    if (!m_suggestionsVisible) {
        QWidget::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
        case Qt::Key_Down:
            if (m_selectedSuggestionIndex < m_filteredSuggestions.size() - 1) {
                selectSuggestion(m_selectedSuggestionIndex + 1);
            }
            event->accept();
            break;
        case Qt::Key_Up:
            if (m_selectedSuggestionIndex > 0) {
                selectSuggestion(m_selectedSuggestionIndex - 1);
            }
            event->accept();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (m_selectedSuggestionIndex >= 0 &&
                m_selectedSuggestionIndex < m_filteredSuggestions.size()) {
                applySuggestion(
                    m_filteredSuggestions[m_selectedSuggestionIndex]);
            } else {
                emit searchRequested(m_lineEdit->text());
            }
            event->accept();
            break;
        case Qt::Key_Escape:
            hideSuggestionsList();
            event->accept();
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void FluentSearchBox::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);
    m_lineEdit->setFocus();
    emit focusReceived();
}

void FluentSearchBox::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
    // Delay hiding suggestions to allow for clicks
    QTimer::singleShot(100, this, [this]() {
        if (!m_lineEdit->hasFocus() && !m_suggestionsList->hasFocus()) {
            hideSuggestionsList();
            emit focusLost();
        }
    });
}

bool FluentSearchBox::eventFilter(QObject* object, QEvent* event) {
    if (object == m_suggestionsList) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QListWidgetItem* item =
                m_suggestionsList->itemAt(mouseEvent->pos());
            if (item) {
                int index = m_suggestionsList->row(item);
                if (index >= 0 && index < m_filteredSuggestions.size()) {
                    applySuggestion(m_filteredSuggestions[index]);
                }
            }
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}

void FluentSearchBox::onTextChanged() {
    updateClearButtonVisibility();

    if (m_searchDelay > 0) {
        m_searchTimer->start(m_searchDelay);
    } else {
        onSearchTimer();
    }

    emit textChanged(m_lineEdit->text());
}

void FluentSearchBox::onSearchTimer() {
    updateSuggestions();

    if (!m_lineEdit->text().isEmpty() && !m_filteredSuggestions.isEmpty()) {
        showSuggestionsList();
    } else {
        hideSuggestionsList();
    }
}

void FluentSearchBox::onSuggestionClicked() {
    // Handled in eventFilter
}

void FluentSearchBox::onClearClicked() { clear(); }

void FluentSearchBox::updateSuggestions() {
    m_filteredSuggestions.clear();
    m_suggestionsList->clear();

    QString query = m_lineEdit->text().trimmed();
    if (query.isEmpty()) {
        return;
    }

    // Use custom search function if available
    if (m_searchFunction) {
        m_filteredSuggestions = m_searchFunction(query);
    } else {
        // Default filtering
        for (const auto& suggestion : m_suggestions) {
            if (suggestion.text.contains(query, Qt::CaseInsensitive)) {
                m_filteredSuggestions.append(suggestion);
            }
        }
    }

    // Limit to max suggestions
    if (m_filteredSuggestions.size() > m_maxSuggestions) {
        m_filteredSuggestions = m_filteredSuggestions.mid(0, m_maxSuggestions);
    }

    // Populate suggestions list
    for (const auto& suggestion : m_filteredSuggestions) {
        auto* item = new QListWidgetItem();
        item->setText(suggestion.text);
        if (!suggestion.icon.isNull()) {
            item->setIcon(suggestion.icon);
        }
        if (!suggestion.description.isEmpty()) {
            item->setToolTip(suggestion.description);
        }
        m_suggestionsList->addItem(item);
    }
}

void FluentSearchBox::setupUI() {
    auto& theme = Styling::FluentTheme::instance();

    // Main layout with Fluent UI spacing
    m_mainLayout = new QHBoxLayout(this);
    int padding = theme.paddingValue("small");
    m_mainLayout->setContentsMargins(padding, padding, padding, padding);
    m_mainLayout->setSpacing(theme.spacing("xs"));

    // Search button with theme-based sizing
    m_searchButton = new QToolButton(this);
    m_searchButton->setObjectName("fluentSearchButton");
    int buttonSize = theme.componentHeight("small");
    m_searchButton->setFixedSize(buttonSize, buttonSize);
    m_searchButton->setToolTip(tr("Search"));
    m_mainLayout->addWidget(m_searchButton);

    // Line edit with enhanced styling
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setObjectName("fluentSearchLineEdit");
    m_lineEdit->setFont(theme.bodyFont());
    m_lineEdit->setPlaceholderText(tr("Search..."));
    m_mainLayout->addWidget(m_lineEdit);

    // Clear button with theme-based sizing
    m_clearButton = new QToolButton(this);
    m_clearButton->setObjectName("fluentClearButton");
    m_clearButton->setFixedSize(buttonSize, buttonSize);
    m_clearButton->setVisible(false);
    m_clearButton->setToolTip(tr("Clear search"));
    m_mainLayout->addWidget(m_clearButton);

    // Suggestions container with elevation and modern styling
    m_suggestionsContainer = new QWidget(this);
    m_suggestionsContainer->setObjectName("fluentSuggestionsContainer");
    m_suggestionsContainer->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    m_suggestionsContainer->setAttribute(Qt::WA_TranslucentBackground);
    m_suggestionsContainer->hide();

    // Add drop shadow effect for elevation
    auto* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(theme.elevation("medium"));
    shadowEffect->setColor(theme.color("shadowMedium"));
    shadowEffect->setOffset(0, 2);
    m_suggestionsContainer->setGraphicsEffect(shadowEffect);

    auto* suggestionsLayout = new QVBoxLayout(m_suggestionsContainer);
    int containerPadding = theme.paddingValue("xs");
    suggestionsLayout->setContentsMargins(containerPadding, containerPadding,
                                          containerPadding, containerPadding);

    // Suggestions list with enhanced styling
    m_suggestionsList = new QListWidget(m_suggestionsContainer);
    m_suggestionsList->setObjectName("fluentSuggestionsList");
    m_suggestionsList->setFont(theme.bodyFont());
    m_suggestionsList->setFrameStyle(QFrame::NoFrame);
    m_suggestionsList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_suggestionsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    suggestionsLayout->addWidget(m_suggestionsList);

    // Set minimum height based on theme
    int minHeight = theme.componentHeight("medium");
    setMinimumHeight(minHeight);
    setMaximumHeight(minHeight);
}

void FluentSearchBox::setupAnimations() {
    auto& theme = Styling::FluentTheme::instance();

    // Opacity effect for suggestions
    m_suggestionsOpacity = new QGraphicsOpacityEffect(this);
    // Note: Don't set the effect here as we already set shadow effect

    // Animation for suggestions with theme-based duration
    m_suggestionsAnimation =
        new QPropertyAnimation(m_suggestionsOpacity, "opacity", this);

    // Use theme-based animation duration (fallback to 200ms if not available)
    int duration = 200;  // Default Fluent UI duration for micro-interactions
    if (!theme.isReducedMotionMode()) {
        m_suggestionsAnimation->setDuration(duration);
        m_suggestionsAnimation->setEasingCurve(QEasingCurve::OutCubic);
    } else {
        // Reduced motion: instant or very fast
        m_suggestionsAnimation->setDuration(50);
        m_suggestionsAnimation->setEasingCurve(QEasingCurve::Linear);
    }

    connect(m_suggestionsAnimation, &QPropertyAnimation::finished, this,
            [this]() {
                if (m_suggestionsOpacity->opacity() == 0.0) {
                    m_suggestionsContainer->hide();
                }
            });
}

void FluentSearchBox::updateSuggestionsPosition() {
    if (!m_suggestionsContainer)
        return;

    QPoint globalPos = mapToGlobal(QPoint(0, height()));
    m_suggestionsContainer->move(globalPos);
    m_suggestionsContainer->resize(
        width(), qMin(200, m_filteredSuggestions.size() * 30 + 10));
}

void FluentSearchBox::animateSuggestions(bool show) {
    if (!m_suggestionsAnimation)
        return;

    m_suggestionsAnimation->setStartValue(show ? 0.0 : 1.0);
    m_suggestionsAnimation->setEndValue(show ? 1.0 : 0.0);
    m_suggestionsAnimation->start();
}

void FluentSearchBox::selectSuggestion(int index) {
    if (index < 0 || index >= m_filteredSuggestions.size())
        return;

    m_selectedSuggestionIndex = index;
    m_suggestionsList->setCurrentRow(index);
}

void FluentSearchBox::applySuggestion(
    const FluentSearchSuggestion& suggestion) {
    setText(suggestion.text);
    hideSuggestionsList();

    if (suggestion.onSelected) {
        suggestion.onSelected();
    }

    emit suggestionSelected(suggestion);
    emit searchRequested(suggestion.text);
}

void FluentSearchBox::updateClearButtonVisibility() {
    m_clearButton->setVisible(!m_lineEdit->text().isEmpty());
}

void FluentSearchBox::updateSearchIcon() {
    if (!m_searchIcon.isNull()) {
        m_searchButton->setIcon(m_searchIcon);
    }
}

// FluentCommandPalette Implementation
FluentCommandPalette::FluentCommandPalette(QWidget* parent)
    : FluentSearchBox(parent) {
    setupCommandPalette();
}

void FluentCommandPalette::addCommand(const FluentCommand& command) {
    m_commands.append(command);
}

void FluentCommandPalette::removeCommand(const QString& id) {
    m_commands.removeIf(
        [&id](const FluentCommand& cmd) { return cmd.id == id; });
}

void FluentCommandPalette::clearCommands() { m_commands.clear(); }

void FluentCommandPalette::setCommands(const QList<FluentCommand>& commands) {
    m_commands = commands;
}

void FluentCommandPalette::addCategory(const QString& category,
                                       const QIcon& icon) {
    m_categories[category] = icon;
}

void FluentCommandPalette::removeCategory(const QString& category) {
    m_categories.remove(category);
}

void FluentCommandPalette::executeCommand(const QString& id) {
    for (const auto& command : m_commands) {
        if (command.id == id && command.enabled) {
            if (command.execute) {
                command.execute();
                emit commandExecuted(id);
            }
            break;
        }
    }
}

void FluentCommandPalette::showPalette() {
    show();
    focus();
    emit paletteShown();
}

void FluentCommandPalette::hidePalette() {
    hide();
    emit paletteHidden();
}

void FluentCommandPalette::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hidePalette();
        event->accept();
        return;
    }
    FluentSearchBox::keyPressEvent(event);
}

void FluentCommandPalette::showEvent(QShowEvent* event) {
    FluentSearchBox::showEvent(event);
    onCommandSearch(text());
}

void FluentCommandPalette::hideEvent(QHideEvent* event) {
    FluentSearchBox::hideEvent(event);
    clear();
}

void FluentCommandPalette::onCommandSearch(const QString& query) {
    setSuggestions(searchCommands(query));
}

void FluentCommandPalette::onCommandSelected(
    const FluentSearchSuggestion& suggestion) {
    QString commandId = suggestion.data.value("commandId").toString();
    executeCommand(commandId);
    hidePalette();
}

void FluentCommandPalette::setupCommandPalette() {
    setPlaceholderText("Type a command...");
    setSearchFunction(
        [this](const QString& query) { return searchCommands(query); });

    connect(this, &FluentSearchBox::suggestionSelected, this,
            &FluentCommandPalette::onCommandSelected);
}

QList<FluentSearchSuggestion> FluentCommandPalette::searchCommands(
    const QString& query) {
    QList<FluentSearchSuggestion> suggestions;

    for (const auto& command : m_commands) {
        if (!command.enabled)
            continue;

        bool matches = false;
        if (query.isEmpty()) {
            matches = true;
        } else {
            matches = command.name.contains(query, Qt::CaseInsensitive) ||
                      command.description.contains(query, Qt::CaseInsensitive);

            for (const QString& keyword : command.keywords) {
                if (keyword.contains(query, Qt::CaseInsensitive)) {
                    matches = true;
                    break;
                }
            }
        }

        if (matches) {
            suggestions.append(commandToSuggestion(command));
        }
    }

    return suggestions;
}

FluentSearchSuggestion FluentCommandPalette::commandToSuggestion(
    const FluentCommand& command) {
    FluentSearchSuggestion suggestion;
    suggestion.text = command.name;
    suggestion.description = command.description;
    suggestion.icon = command.icon;
    suggestion.data["commandId"] = command.id;
    suggestion.onSelected = [this, command]() { executeCommand(command.id); };
    return suggestion;
}

// FluentTagInput Implementation
FluentTagInput::FluentTagInput(QWidget* parent) : QWidget(parent) { setupUI(); }

void FluentTagInput::setTags(const QStringList& tags) {
    if (m_tags != tags) {
        clearTags();
        for (const QString& tag : tags) {
            addTag(tag);
        }
    }
}

void FluentTagInput::addTag(const QString& tag) {
    QString trimmedTag = tag.trimmed();
    if (trimmedTag.isEmpty() || !isValidTag(trimmedTag)) {
        return;
    }

    if (!m_allowDuplicates && m_tags.contains(trimmedTag)) {
        return;
    }

    m_tags.append(trimmedTag);
    addTagWidget(trimmedTag);
    updateLayout();

    emit tagAdded(trimmedTag);
    emit tagsChanged(m_tags);
}

void FluentTagInput::removeTag(const QString& tag) {
    int index = m_tags.indexOf(tag);
    if (index >= 0) {
        removeTag(index);
    }
}

void FluentTagInput::removeTag(int index) {
    if (index >= 0 && index < m_tags.size()) {
        QString tag = m_tags.takeAt(index);
        removeTagWidget(index);
        updateLayout();

        emit tagRemoved(tag);
        emit tagsChanged(m_tags);
    }
}

void FluentTagInput::clearTags() {
    m_tags.clear();
    for (QWidget* widget : m_tagWidgets) {
        widget->deleteLater();
    }
    m_tagWidgets.clear();
    updateLayout();
    emit tagsChanged(m_tags);
}

QString FluentTagInput::placeholderText() const {
    return m_input->placeholderText();
}

void FluentTagInput::setPlaceholderText(const QString& text) {
    m_input->setPlaceholderText(text);
}

void FluentTagInput::setAllowDuplicates(bool allow) {
    m_allowDuplicates = allow;
}

void FluentTagInput::setSuggestions(const QStringList& suggestions) {
    m_suggestions = suggestions;
    if (m_completer) {
        m_completer->setModel(new QStringListModel(suggestions, m_completer));
    }
}

void FluentTagInput::setValidator(
    std::function<bool(const QString&)> validator) {
    m_validator = validator;
}

void FluentTagInput::focus() { m_input->setFocus(); }

void FluentTagInput::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateLayout();
}

void FluentTagInput::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Backspace && m_input->text().isEmpty() &&
        !m_tags.isEmpty()) {
        removeTag(m_tags.size() - 1);
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void FluentTagInput::onInputTextChanged() {
    QString text = m_input->text();
    if (text.endsWith(',') || text.endsWith(';') || text.endsWith(' ')) {
        QString tag = text.chopped(1).trimmed();
        if (!tag.isEmpty()) {
            addTag(tag);
            m_input->clear();
        }
    }
}

void FluentTagInput::onTagRemoveClicked() {
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    if (button) {
        int index = m_tagWidgets.indexOf(button->parentWidget());
        if (index >= 0) {
            removeTag(index);
        }
    }
}

void FluentTagInput::setupUI() {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    m_tagsContainer = new QWidget(this);
    layout->addWidget(m_tagsContainer);

    m_input = new QLineEdit(this);
    m_input->setPlaceholderText("Add tag...");
    layout->addWidget(m_input);

    // Setup completer
    m_completer = new QCompleter(this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_input->setCompleter(m_completer);

    connect(m_input, &QLineEdit::textChanged, this,
            &FluentTagInput::onInputTextChanged);
    connect(m_input, &QLineEdit::returnPressed, this, [this]() {
        QString text = m_input->text().trimmed();
        if (!text.isEmpty()) {
            addTag(text);
            m_input->clear();
        }
    });
}

void FluentTagInput::updateLayout() {
    // Simple layout update - in a real implementation, this would handle
    // wrapping
}

void FluentTagInput::addTagWidget(const QString& tag) {
    auto* tagWidget = new QWidget(this);
    auto* tagLayout = new QHBoxLayout(tagWidget);
    tagLayout->setContentsMargins(8, 4, 4, 4);
    tagLayout->setSpacing(4);

    auto* label = new QLabel(tag, tagWidget);
    tagLayout->addWidget(label);

    auto* removeButton = new QToolButton(tagWidget);
    removeButton->setText("×");
    removeButton->setFixedSize(16, 16);
    connect(removeButton, &QToolButton::clicked, this,
            &FluentTagInput::onTagRemoveClicked);
    tagLayout->addWidget(removeButton);

    m_tagWidgets.append(tagWidget);
    tagWidget->show();
}

void FluentTagInput::removeTagWidget(int index) {
    if (index >= 0 && index < m_tagWidgets.size()) {
        QWidget* widget = m_tagWidgets.takeAt(index);
        widget->deleteLater();
    }
}

bool FluentTagInput::isValidTag(const QString& tag) const {
    if (m_validator) {
        return m_validator(tag);
    }
    return !tag.isEmpty();
}

// New optimized methods for Fluent UI compliance

void FluentSearchBox::setupAccessibility() {
    // Set up comprehensive accessibility support
    setAccessibleName(tr("Search Box"));
    setAccessibleDescription(tr("Enter search terms to find items"));

    if (m_lineEdit) {
        m_lineEdit->setAccessibleName(tr("Search Input"));
        m_lineEdit->setAccessibleDescription(tr("Type your search query here"));
    }

    if (m_searchButton) {
        m_searchButton->setAccessibleName(tr("Search"));
        m_searchButton->setAccessibleDescription(tr("Execute search"));
    }

    if (m_clearButton) {
        m_clearButton->setAccessibleName(tr("Clear"));
        m_clearButton->setAccessibleDescription(tr("Clear search text"));
    }

    if (m_suggestionsList) {
        m_suggestionsList->setAccessibleName(tr("Search Suggestions"));
        m_suggestionsList->setAccessibleDescription(
            tr("List of search suggestions"));
    }
}

void FluentSearchBox::setupThemeIntegration() {
    // Set up theme integration and responsive behavior
    auto& theme = Styling::FluentTheme::instance();

    // Apply initial theme-based properties
    setFont(theme.bodyFont());

    // Set up responsive behavior based on theme settings
    if (theme.isHighContrastMode()) {
        // Enhanced contrast for accessibility
        setStyleSheet(QString("FluentSearchBox { border: 2px solid %1; }")
                          .arg(theme.color("borderFocus").name()));
    }
}

void FluentSearchBox::applyTheme() {
    auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Apply theme colors and styling
    QString styleSheet =
        QString(R"(
        FluentSearchBox {
            background-color: %1;
            border: 1px solid %2;
            border-radius: %3px;
        }

        FluentSearchBox:focus {
            border-color: %4;
            box-shadow: 0 0 0 2px %5;
        }

        QLineEdit#fluentSearchLineEdit {
            background-color: transparent;
            border: none;
            color: %6;
            font-size: %7px;
            padding: %8px;
        }

        QLineEdit#fluentSearchLineEdit:focus {
            outline: none;
        }

        QToolButton#fluentSearchButton,
        QToolButton#fluentClearButton {
            background-color: transparent;
            border: none;
            border-radius: %9px;
            color: %10;
        }

        QToolButton#fluentSearchButton:hover,
        QToolButton#fluentClearButton:hover {
            background-color: %11;
        }

        QToolButton#fluentSearchButton:pressed,
        QToolButton#fluentClearButton:pressed {
            background-color: %12;
        }

        QWidget#fluentSuggestionsContainer {
            background-color: %13;
            border: 1px solid %14;
            border-radius: %15px;
        }

        QListWidget#fluentSuggestionsList {
            background-color: transparent;
            border: none;
            outline: none;
        }

        QListWidget#fluentSuggestionsList::item {
            padding: %16px;
            border-bottom: 1px solid %17;
            color: %18;
        }

        QListWidget#fluentSuggestionsList::item:hover {
            background-color: %19;
        }

        QListWidget#fluentSuggestionsList::item:selected {
            background-color: %20;
            color: %21;
        }
    )")
            .arg(palette.surface.name())           // 1: background
            .arg(palette.border.name())            // 2: border
            .arg(theme.borderRadius("medium"))     // 3: border radius
            .arg(palette.borderFocus.name())       // 4: focus border
            .arg(palette.focus.name())             // 5: focus shadow
            .arg(palette.neutralPrimary.name())    // 6: text color
            .arg(theme.bodyFont().pointSize())     // 7: font size
            .arg(theme.paddingValue("small"))      // 8: padding
            .arg(theme.borderRadius("small"))      // 9: button radius
            .arg(palette.neutralSecondary.name())  // 10: button color
            .arg(palette.hover.name())             // 11: button hover
            .arg(palette.pressed.name())           // 12: button pressed
            .arg(palette.surfaceSecondary.name())  // 13: suggestions bg
            .arg(palette.borderSecondary.name())   // 14: suggestions border
            .arg(theme.borderRadius("medium"))     // 15: suggestions radius
            .arg(theme.paddingValue("medium"))     // 16: item padding
            .arg(palette.borderSecondary.name())   // 17: item border
            .arg(palette.neutralPrimary.name())    // 18: item text
            .arg(palette.hover.name())             // 19: item hover
            .arg(palette.selected.name())          // 20: item selected bg
            .arg(palette.neutralLightest.name());  // 21: item selected text

    setStyleSheet(styleSheet);

    // Update component sizes based on theme
    if (m_searchButton && m_clearButton) {
        int buttonSize = theme.componentHeight("small");
        m_searchButton->setFixedSize(buttonSize, buttonSize);
        m_clearButton->setFixedSize(buttonSize, buttonSize);
    }

    // Update fonts
    if (m_lineEdit) {
        m_lineEdit->setFont(theme.bodyFont());
    }
    if (m_suggestionsList) {
        m_suggestionsList->setFont(theme.bodyFont());
    }
}

void FluentSearchBox::updateLoadingState() {
    if (m_isLoading) {
        m_searchButton->setEnabled(false);
        m_searchButton->setToolTip(tr("Searching..."));
        // Could add a loading spinner here
    } else {
        m_searchButton->setEnabled(true);
        m_searchButton->setToolTip(tr("Search"));
    }
}

void FluentSearchBox::updateErrorState() {
    auto& theme = Styling::FluentTheme::instance();

    if (m_hasError) {
        // Apply error styling
        setStyleSheet(styleSheet() + QString(R"(
            FluentSearchBox {
                border-color: %1 !important;
            }
        )")
                                         .arg(theme.color("error").name()));

        setToolTip(m_errorMessage.isEmpty() ? tr("Search error occurred")
                                            : m_errorMessage);
    } else {
        // Remove error styling by reapplying theme
        applyTheme();
        setToolTip(QString());
    }
}

void FluentSearchBox::updateDisabledState() {
    auto& theme = Styling::FluentTheme::instance();

    if (m_isDisabled) {
        // Apply disabled styling
        setStyleSheet(styleSheet() +
                      QString(R"(
            FluentSearchBox {
                background-color: %1 !important;
                color: %2 !important;
            }
        )")
                          .arg(theme.color("disabled").name())
                          .arg(theme.color("neutralTertiary").name()));
    } else {
        // Remove disabled styling by reapplying theme
        applyTheme();
    }
}

void FluentSearchBox::onSearchButtonClicked() {
    if (!m_lineEdit->text().isEmpty()) {
        emit searchRequested(m_lineEdit->text());
    }
}

}  // namespace FluentQt::Components
