// src/Components/FluentSearchBox.cpp
#include "FluentQt/Components/FluentSearchBox.h"
#include <QApplication>
#include <QCompleter>
#include <QFocusEvent>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QStringListModel>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentSearchBox::FluentSearchBox(QWidget* parent) : QWidget(parent) {
    setupUI();
    setupAnimations();

    // Set default icons
    setSearchIcon(QIcon(":/icons/search.png"));
    setClearIcon(QIcon(":/icons/clear.png"));

    // Setup search timer
    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    connect(m_searchTimer, &QTimer::timeout, this,
            &FluentSearchBox::onSearchTimer);

    // Connect signals
    connect(m_lineEdit, &QLineEdit::textChanged, this,
            &FluentSearchBox::onTextChanged);
    connect(m_clearButton, &QToolButton::clicked, this,
            &FluentSearchBox::onClearClicked);

    // Install event filter for suggestions
    m_suggestionsList->installEventFilter(this);

    // Apply theme
    updateSearchIcon();
    updateClearButtonVisibility();
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
    // Main layout
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Search button
    m_searchButton = new QToolButton(this);
    m_searchButton->setObjectName("searchButton");
    m_searchButton->setFixedSize(32, 32);
    m_mainLayout->addWidget(m_searchButton);

    // Line edit
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setObjectName("searchLineEdit");
    m_mainLayout->addWidget(m_lineEdit);

    // Clear button
    m_clearButton = new QToolButton(this);
    m_clearButton->setObjectName("clearButton");
    m_clearButton->setFixedSize(32, 32);
    m_clearButton->setVisible(false);
    m_mainLayout->addWidget(m_clearButton);

    // Suggestions container (positioned absolutely)
    m_suggestionsContainer = new QWidget(this);
    m_suggestionsContainer->setObjectName("suggestionsContainer");
    m_suggestionsContainer->setWindowFlags(Qt::Popup);
    m_suggestionsContainer->hide();

    auto* suggestionsLayout = new QVBoxLayout(m_suggestionsContainer);
    suggestionsLayout->setContentsMargins(0, 0, 0, 0);

    // Suggestions list
    m_suggestionsList = new QListWidget(m_suggestionsContainer);
    m_suggestionsList->setObjectName("suggestionsList");
    suggestionsLayout->addWidget(m_suggestionsList);

    // Set minimum height
    setMinimumHeight(32);
}

void FluentSearchBox::setupAnimations() {
    // Opacity effect for suggestions
    m_suggestionsOpacity = new QGraphicsOpacityEffect(this);
    m_suggestionsContainer->setGraphicsEffect(m_suggestionsOpacity);

    // Animation for suggestions
    m_suggestionsAnimation =
        new QPropertyAnimation(m_suggestionsOpacity, "opacity", this);
    m_suggestionsAnimation->setDuration(200);
    m_suggestionsAnimation->setEasingCurve(QEasingCurve::OutCubic);

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

}  // namespace FluentQt::Components
