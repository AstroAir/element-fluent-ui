// src/Components/FluentSelect.cpp
#include "FluentQt/Components/FluentSelect.h"
#include "FluentQt/Components/FluentSelectDropdown.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QCompleter>
#include <QDebug>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QWheelEvent>

using namespace FluentQt::Components;
using namespace FluentQt::Core;
using namespace FluentQt::Styling;

FluentSelect::FluentSelect(QWidget* parent) : FluentComponent(parent) {
    setupUI();
    setupDropdown();
    setupAnimations();
    setupModel();
    setupConnections();

    // Connect to theme changes
    connect(&FluentTheme::instance(), &FluentTheme::themeChanged, this,
            &FluentSelect::onThemeChanged);

    updateColors();
    updateFonts();
    updateAccessibility();

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
}

FluentSelect::FluentSelect(FluentSelectMode mode, QWidget* parent)
    : FluentSelect(parent) {
    m_config.mode = mode;
    updateLayout();
}

FluentSelect::FluentSelect(const FluentSelectConfig& config, QWidget* parent)
    : FluentSelect(parent) {
    setConfiguration(config);
}

FluentSelect::~FluentSelect() = default;

void FluentSelect::setSelectMode(FluentSelectMode mode) {
    if (m_config.mode != mode) {
        m_config.mode = mode;
        updateLayout();
        updateAccessibility();
        emit modeChanged(mode);
    }
}

void FluentSelect::setSearchMode(FluentSelectSearchMode mode) {
    if (m_config.searchMode != mode) {
        m_config.searchMode = mode;
        // updateDropdown(); // Will be implemented later
        emit searchModeChanged(mode);
    }
}

void FluentSelect::setSelectSize(FluentSelectSize size) {
    if (m_config.size != size) {
        m_config.size = size;
        updateFonts();
        m_sizeHintValid = false;
        updateGeometry();
        emit sizeChanged(size);
    }
}

void FluentSelect::setPlaceholderText(const QString& text) {
    if (m_config.placeholderText != text) {
        m_config.placeholderText = text;
        updatePlaceholder();
        emit placeholderChanged(text);
    }
}

QString FluentSelect::currentText() const {
    if (m_config.editable && m_searchEdit) {
        return m_searchEdit->text();
    }

    const QModelIndex index = currentModelIndex();
    if (index.isValid()) {
        return index.data(Qt::DisplayRole).toString();
    }

    return QString();
}

QVariant FluentSelect::currentData() const { return currentData(Qt::UserRole); }

QVariant FluentSelect::currentData(int role) const {
    const QModelIndex index = currentModelIndex();
    if (index.isValid()) {
        return index.data(role);
    }

    return QVariant();
}

int FluentSelect::currentIndex() const {
    if (!m_selectionModel) {
        return -1;
    }

    const QModelIndex index = m_selectionModel->currentIndex();
    if (m_proxyModel) {
        const QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
        return sourceIndex.row();
    }

    return index.row();
}

void FluentSelect::setCurrentIndex(int index) {
    if (!m_selectModel || index < 0 || index >= m_selectModel->itemCount()) {
        return;
    }

    QModelIndex modelIndex = m_selectModel->index(index, 0);
    if (m_proxyModel) {
        modelIndex = m_proxyModel->mapFromSource(modelIndex);
    }

    setCurrentModelIndex(modelIndex);
}

QModelIndex FluentSelect::currentModelIndex() const {
    if (m_selectionModel) {
        return m_selectionModel->currentIndex();
    }

    return QModelIndex();
}

void FluentSelect::setCurrentModelIndex(const QModelIndex& index) {
    if (m_selectionModel && index != m_selectionModel->currentIndex()) {
        const int oldIndex = currentIndex();
        m_selectionModel->setCurrentIndex(index,
                                          QItemSelectionModel::ClearAndSelect);

        updateDisplayText();

        emit currentIndexChanged(currentIndex(), oldIndex);
        emit currentTextChanged(currentText());
        emit currentDataChanged(currentData());
    }
}

QList<int> FluentSelect::selectedIndexes() const {
    QList<int> indexes;

    if (m_selectionModel) {
        const QModelIndexList selected = m_selectionModel->selectedIndexes();
        for (const QModelIndex& index : selected) {
            QModelIndex sourceIndex = index;
            if (m_proxyModel) {
                sourceIndex = m_proxyModel->mapToSource(index);
            }
            indexes.append(sourceIndex.row());
        }
    }

    return indexes;
}

void FluentSelect::setSelectedIndexes(const QList<int>& indexes) {
    if (!m_selectionModel || m_config.mode == FluentSelectMode::Single) {
        return;
    }

    m_selectionModel->clearSelection();

    for (int index : indexes) {
        if (index >= 0 && index < m_selectModel->itemCount()) {
            QModelIndex modelIndex = m_selectModel->index(index, 0);
            if (m_proxyModel) {
                modelIndex = m_proxyModel->mapFromSource(modelIndex);
            }
            m_selectionModel->select(modelIndex, QItemSelectionModel::Select);
        }
    }

    updateDisplayText();
    emit selectionChanged();
}

QModelIndexList FluentSelect::selectedModelIndexes() const {
    if (m_selectionModel) {
        return m_selectionModel->selectedIndexes();
    }

    return QModelIndexList();
}

void FluentSelect::setSelectedModelIndexes(const QModelIndexList& indexes) {
    if (!m_selectionModel || m_config.mode == FluentSelectMode::Single) {
        return;
    }

    m_selectionModel->clearSelection();

    for (const QModelIndex& index : indexes) {
        m_selectionModel->select(index, QItemSelectionModel::Select);
    }

    updateDisplayText();
    emit selectionChanged();
}

QStringList FluentSelect::selectedTexts() const {
    QStringList texts;

    const QModelIndexList indexes = selectedModelIndexes();
    for (const QModelIndex& index : indexes) {
        texts.append(index.data(Qt::DisplayRole).toString());
    }

    return texts;
}

QVariantList FluentSelect::selectedData() const {
    QVariantList data;

    const QModelIndexList indexes = selectedModelIndexes();
    for (const QModelIndex& index : indexes) {
        data.append(index.data(Qt::UserRole));
    }

    return data;
}

void FluentSelect::setEditable(bool editable) {
    if (m_config.editable != editable) {
        m_config.editable = editable;
        updateLayout();
        emit editableChanged(editable);
    }
}

void FluentSelect::setClearable(bool clearable) {
    if (m_config.clearable != clearable) {
        m_config.clearable = clearable;
        updateLayout();
        emit clearableChanged(clearable);
    }
}

void FluentSelect::setSearchable(bool searchable) {
    if (m_config.searchable != searchable) {
        m_config.searchable = searchable;
        // updateDropdown(); // Will be implemented later
        emit searchableChanged(searchable);
    }
}

void FluentSelect::setDropdownVisible(bool visible) {
    if (m_dropdownVisible != visible) {
        if (visible) {
            showDropdown();
        } else {
            hideDropdown();
        }
    }
}

void FluentSelect::setMaxVisibleItems(int max) {
    if (m_config.maxVisibleItems != max) {
        m_config.maxVisibleItems = qMax(1, max);
        updateDropdownGeometry();
        emit maxVisibleItemsChanged(m_config.maxVisibleItems);
    }
}

void FluentSelect::setConfiguration(const FluentSelectConfig& config) {
    const bool modeHasChanged = (m_config.mode != config.mode);
    const bool searchModeHasChanged =
        (m_config.searchMode != config.searchMode);
    const bool sizeHasChanged = (m_config.size != config.size);
    const bool editableHasChanged = (m_config.editable != config.editable);
    const bool clearableHasChanged = (m_config.clearable != config.clearable);
    const bool searchableHasChanged =
        (m_config.searchable != config.searchable);
    const bool maxVisibleHasChanged =
        (m_config.maxVisibleItems != config.maxVisibleItems);
    const bool placeholderHasChanged =
        (m_config.placeholderText != config.placeholderText);

    m_config = config;

    m_sizeHintValid = false;
    updateLayout();
    // updateDropdownSettings(); // Will be implemented later
    updateColors();
    updateFonts();
    updatePlaceholder();
    updateAccessibility();

    // Emit change signals
    if (modeHasChanged)
        emit modeChanged(config.mode);
    if (searchModeHasChanged)
        emit searchModeChanged(config.searchMode);
    if (sizeHasChanged)
        emit sizeChanged(config.size);
    if (editableHasChanged)
        emit editableChanged(config.editable);
    if (clearableHasChanged)
        emit clearableChanged(config.clearable);
    if (searchableHasChanged)
        emit searchableChanged(config.searchable);
    if (maxVisibleHasChanged)
        emit maxVisibleItemsChanged(config.maxVisibleItems);
    if (placeholderHasChanged)
        emit placeholderChanged(config.placeholderText);
}

QAbstractItemModel* FluentSelect::model() const { return m_selectModel; }

void FluentSelect::setModel(QAbstractItemModel* model) {
    if (model == m_selectModel) {
        return;
    }

    // Disconnect old model
    if (m_selectModel) {
        disconnect(m_selectModel, nullptr, this, nullptr);
    }

    // Set new model
    if (auto* selectModel = qobject_cast<FluentSelectModel*>(model)) {
        m_selectModel = selectModel;
    } else {
        // Create a wrapper model if needed
        // For now, we'll only support FluentSelectModel directly
        qWarning() << "FluentSelect only supports FluentSelectModel directly";
        return;
    }

    // Setup proxy model
    if (m_proxyModel) {
        m_proxyModel->setSourceModel(m_selectModel);
    }

    // Setup list view
    if (m_listView) {
        m_listView->setModel(
            m_proxyModel ? static_cast<QAbstractItemModel*>(m_proxyModel)
                         : m_selectModel);
    }

    // Connect new model
    if (m_selectModel) {
        connect(m_selectModel, &QAbstractItemModel::dataChanged, this,
                &FluentSelect::onModelDataChanged);
        connect(m_selectModel, &QAbstractItemModel::rowsInserted, this,
                &FluentSelect::onModelRowsInserted);
        connect(m_selectModel, &QAbstractItemModel::rowsRemoved, this,
                &FluentSelect::onModelRowsRemoved);
        connect(m_selectModel, &QAbstractItemModel::modelReset, this,
                &FluentSelect::onModelReset);
    }

    updateDisplayText();
}

// Item management convenience methods
void FluentSelect::addItem(const QString& text, const QVariant& data) {
    if (m_selectModel) {
        m_selectModel->addItem(FluentSelectItem(text, data));
    }
}

void FluentSelect::addItem(const QIcon& icon, const QString& text,
                           const QVariant& data) {
    if (m_selectModel) {
        m_selectModel->addItem(FluentSelectItem(icon, text, data));
    }
}

void FluentSelect::addItem(const FluentSelectItem& item) {
    if (m_selectModel) {
        m_selectModel->addItem(item);
    }
}

void FluentSelect::addItems(const QStringList& texts) {
    if (m_selectModel) {
        for (const QString& text : texts) {
            m_selectModel->addItem(FluentSelectItem(text));
        }
    }
}

void FluentSelect::addSeparator() {
    if (m_selectModel) {
        m_selectModel->addItem(FluentSelectItem::createSeparator());
    }
}

void FluentSelect::addGroup(const QString& title) {
    if (m_selectModel) {
        m_selectModel->addGroup(title);
    }
}

void FluentSelect::insertItem(int index, const QString& text,
                              const QVariant& data) {
    if (m_selectModel) {
        m_selectModel->insertItem(index, FluentSelectItem(text, data));
    }
}

void FluentSelect::insertItem(int index, const QIcon& icon, const QString& text,
                              const QVariant& data) {
    if (m_selectModel) {
        m_selectModel->insertItem(index, FluentSelectItem(icon, text, data));
    }
}

void FluentSelect::insertItem(int index, const FluentSelectItem& item) {
    if (m_selectModel) {
        m_selectModel->insertItem(index, item);
    }
}

void FluentSelect::removeItem(int index) {
    if (m_selectModel) {
        m_selectModel->removeItem(index);
    }
}

void FluentSelect::clear() {
    if (m_selectModel) {
        m_selectModel->clear();
    }
}

int FluentSelect::count() const {
    return m_selectModel ? m_selectModel->itemCount() : 0;
}

bool FluentSelect::isEmpty() const { return count() == 0; }

FluentSelectItem FluentSelect::itemAt(int index) const {
    if (m_selectModel) {
        return m_selectModel->itemAt(index);
    }
    return FluentSelectItem();
}

void FluentSelect::setItemAt(int index, const FluentSelectItem& item) {
    if (m_selectModel) {
        m_selectModel->setItemAt(index, item);
    }
}

int FluentSelect::findText(const QString& text, Qt::MatchFlags flags) const {
    if (!m_selectModel) {
        return -1;
    }

    const QList<int> found = m_selectModel->findItems(text, flags);
    return found.isEmpty() ? -1 : found.first();
}

int FluentSelect::findData(const QVariant& data, int role,
                           Qt::MatchFlags flags) const {
    Q_UNUSED(role)  // For future use
    if (!m_selectModel) {
        return -1;
    }

    const QList<int> found = m_selectModel->findItemsByData(data, flags);
    return found.isEmpty() ? -1 : found.first();
}

// Search functionality
void FluentSelect::setSearchFilter(const QString& filter) {
    if (m_searchText != filter) {
        m_searchText = filter;
        applySearchFilter();
        emit searchFilterChanged(filter);
    }
}

QString FluentSelect::searchFilter() const { return m_searchText; }

void FluentSelect::clearSearchFilter() { setSearchFilter(QString()); }

void FluentSelect::setCustomSearchFunction(
    std::function<bool(const FluentSelectItem&, const QString&)> searchFunc) {
    m_customSearchFunction = std::move(searchFunc);
}

// Validation
bool FluentSelect::isValid() const {
    if (!m_validator) {
        return true;
    }

    return m_validator(currentData());
}

QString FluentSelect::validationError() const {
    return isValid() ? QString() : m_validationErrorMessage;
}

void FluentSelect::setValidator(
    std::function<bool(const QVariant&)> validator) {
    m_validator = std::move(validator);
}

void FluentSelect::setValidationErrorMessage(const QString& message) {
    m_validationErrorMessage = message;
}

QSize FluentSelect::sizeHint() const {
    if (!m_sizeHintValid) {
        const_cast<FluentSelect*>(this)->m_cachedSizeHint =
            calculateSizeHintInternal();
        const_cast<FluentSelect*>(this)->m_sizeHintValid = true;
    }
    return m_cachedSizeHint;
}

QSize FluentSelect::minimumSizeHint() const {
    return QSize(120, getItemHeight());
}

QSize FluentSelect::calculateSizeHintInternal() const {
    const auto& theme = FluentTheme::instance();
    const QFontMetrics metrics(getFont());

    // Use theme-based minimum width
    int width = theme.componentWidth("medium");
    int height = getItemHeight();

    // Calculate width based on content with proper spacing
    const int padding = theme.paddingValue("medium");
    const int iconSpace = theme.iconSize("small").width();
    const int buttonSpace = 24;  // Space for dropdown arrow
    const int clearButtonSpace = m_config.clearable ? 20 : 0;

    if (m_selectModel && !m_selectModel->isEmpty()) {
        for (int i = 0; i < m_selectModel->itemCount(); ++i) {
            const FluentSelectItem item = m_selectModel->itemAt(i);
            const int textWidth = metrics.horizontalAdvance(item.text());
            const int itemWidth = textWidth + (padding * 2) + iconSpace +
                                  buttonSpace + clearButtonSpace;
            width = qMax(width, itemWidth);
        }
    }

    // Ensure minimum touch target size for accessibility
    const int minTouchTarget = 44;  // Microsoft accessibility guidelines
    height = qMax(height, minTouchTarget);
    width = qMax(width, minTouchTarget * 3);  // Reasonable minimum width

    return QSize(width, height);
}

// Static factory methods
FluentSelect* FluentSelect::createSingleSelect(QWidget* parent) {
    return new FluentSelect(FluentSelectMode::Single, parent);
}

FluentSelect* FluentSelect::createMultiSelect(QWidget* parent) {
    return new FluentSelect(FluentSelectMode::Multiple, parent);
}

FluentSelect* FluentSelect::createSearchableSelect(QWidget* parent) {
    FluentSelectConfig config;
    config.searchable = true;
    config.searchMode = FluentSelectSearchMode::Contains;
    return new FluentSelect(config, parent);
}

FluentSelect* FluentSelect::createEditableSelect(QWidget* parent) {
    FluentSelectConfig config;
    config.editable = true;
    config.searchable = true;
    return new FluentSelect(config, parent);
}

// Public slots
void FluentSelect::showDropdown() {
    if (m_dropdownVisible || !m_dropdown) {
        return;
    }

    emit dropdownAboutToShow();

    m_dropdownVisible = true;
    updateDropdownGeometry();
    updateDropdownPosition();

    if (m_dropdown) {
        m_dropdown->showAnimated();
    }

    emit dropdownVisibilityChanged(true);
    emit dropdownShown();
}

void FluentSelect::hideDropdown() {
    if (!m_dropdownVisible || !m_dropdown) {
        return;
    }

    emit dropdownAboutToHide();

    m_dropdownVisible = false;

    if (m_dropdown) {
        m_dropdown->hideAnimated();
    }

    emit dropdownVisibilityChanged(false);
    emit dropdownHidden();
}

void FluentSelect::toggleDropdown() {
    if (m_dropdownVisible) {
        hideDropdown();
    } else {
        showDropdown();
    }
}

void FluentSelect::clearSelection() {
    if (m_selectionModel) {
        m_selectionModel->clearSelection();
        updateDisplayText();
        emit selectionChanged();
    }
}

void FluentSelect::selectAll() {
    if (m_config.mode == FluentSelectMode::Single || !m_selectionModel ||
        !m_selectModel) {
        return;
    }

    // Select all items manually since QItemSelectionModel doesn't have
    // selectAll()
    for (int i = 0; i < m_selectModel->itemCount(); ++i) {
        const QModelIndex index = m_selectModel->index(i, 0);
        QModelIndex proxyIndex = index;
        if (m_proxyModel) {
            proxyIndex = m_proxyModel->mapFromSource(index);
        }
        m_selectionModel->select(proxyIndex, QItemSelectionModel::Select);
    }

    updateDisplayText();
    emit selectionChanged();
}

void FluentSelect::invertSelection() {
    if (m_config.mode == FluentSelectMode::Single || !m_selectionModel ||
        !m_selectModel) {
        return;
    }

    const QModelIndexList allIndexes = m_selectionModel->selectedIndexes();
    m_selectionModel->clearSelection();

    for (int i = 0; i < m_selectModel->itemCount(); ++i) {
        const QModelIndex index = m_selectModel->index(i, 0);
        if (!allIndexes.contains(index)) {
            m_selectionModel->select(index, QItemSelectionModel::Select);
        }
    }

    updateDisplayText();
    emit selectionChanged();
}

void FluentSelect::refresh() {
    updateLayout();
    updateColors();
    updateDisplayText();
    update();
}

// Private implementation methods
void FluentSelect::setupUI() {
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(1, 1, 1, 1);
    m_mainLayout->setSpacing(0);

    // Display label (for non-editable mode)
    m_displayLabel = new QLabel();
    m_displayLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_displayLabel->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Preferred);
    m_mainLayout->addWidget(m_displayLabel);

    // Search edit (for editable mode)
    m_searchEdit = new QLineEdit();
    m_searchEdit->setVisible(false);
    m_searchEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mainLayout->addWidget(m_searchEdit);

    // Clear button
    m_clearButton = new QPushButton();
    m_clearButton->setFixedSize(20, 20);
    m_clearButton->setVisible(false);
    m_clearButton->setFlat(true);
    m_clearButton->setText("×");
    connect(m_clearButton, &QPushButton::clicked, this,
            &FluentSelect::onClearButtonClicked);
    m_mainLayout->addWidget(m_clearButton);

    // Dropdown button
    m_dropdownButton = new QPushButton();
    m_dropdownButton->setFixedSize(20, 20);
    m_dropdownButton->setFlat(true);
    m_dropdownButton->setText("▼");
    connect(m_dropdownButton, &QPushButton::clicked, this,
            &FluentSelect::onDropdownButtonClicked);
    m_mainLayout->addWidget(m_dropdownButton);

    updateLayout();
}

void FluentSelect::setupDropdown() {
    m_dropdown = new FluentSelectDropdown(this);
    m_dropdown->setVisible(false);

    m_listView = m_dropdown->listView();

    // Connect dropdown signals
    connect(m_dropdown, &FluentSelectDropdown::itemActivated, this,
            &FluentSelect::onDropdownItemActivated);
    connect(m_dropdown, &FluentSelectDropdown::itemClicked, this,
            &FluentSelect::onDropdownItemClicked);
    connect(m_dropdown, &FluentSelectDropdown::selectionChanged, this,
            &FluentSelect::onDropdownSelectionChanged);
    connect(m_dropdown, &FluentSelectDropdown::searchTextChanged, this,
            &FluentSelect::onSearchTextChanged);
    connect(m_dropdown, &FluentSelectDropdown::hidden, this,
            &FluentSelect::onDropdownAnimationFinished);
}

void FluentSelect::setupAnimations() {
    // Dropdown show/hide animations
    m_dropdownAnimation =
        std::make_unique<QPropertyAnimation>(m_dropdown, "geometry");
    m_dropdownAnimation->setDuration(200);
    m_dropdownAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_dropdownAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentSelect::onDropdownAnimationFinished);
}

void FluentSelect::setupModel() {
    // Create select model
    m_selectModel = new FluentSelectModel(this);

    // Create proxy model for filtering
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_selectModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // Create selection model
    m_selectionModel = new QItemSelectionModel(m_proxyModel, this);

    // Set model to dropdown
    if (m_dropdown) {
        m_dropdown->setModel(m_proxyModel);
        m_dropdown->setSelectionModel(m_selectionModel);
    }
}

void FluentSelect::setupConnections() {
    // Search edit connections
    if (m_searchEdit) {
        connect(m_searchEdit, &QLineEdit::textChanged, this,
                &FluentSelect::onSearchTextChanged);
        connect(m_searchEdit, &QLineEdit::textEdited, this,
                &FluentSelect::onSearchTextEdited);
    }

    // Selection model connections
    if (m_selectionModel) {
        connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this,
                &FluentSelect::onDropdownSelectionChanged);
    }
}

void FluentSelect::updateLayout() {
    if (!m_displayLabel || !m_searchEdit)
        return;

    // Show/hide appropriate widgets based on mode
    if (m_config.editable) {
        m_displayLabel->setVisible(false);
        m_searchEdit->setVisible(true);
        m_searchEdit->setPlaceholderText(m_config.placeholderText);
    } else {
        m_displayLabel->setVisible(true);
        m_searchEdit->setVisible(false);
    }

    // Show/hide clear button
    if (m_clearButton) {
        m_clearButton->setVisible(m_config.clearable &&
                                  !currentText().isEmpty());
    }

    updateDisplayText();
}

void FluentSelect::updateColors() {
    const auto& theme = FluentTheme::instance();

    // Use proper Fluent design tokens instead of hardcoded colors
    if (m_config.autoCalculateColors) {
        m_backgroundColor = theme.color("controlFillDefault");
        m_textColor = theme.color("textPrimary");
        m_borderColor = theme.color("controlStrokeDefault");
        m_placeholderColor = theme.color("textSecondary");
        m_focusColor = theme.color("accent");
    } else {
        // Allow custom colors but ensure they meet accessibility standards
        if (m_config.customBackgroundColor.isValid()) {
            m_backgroundColor =
                theme.ensureContrast(m_config.customBackgroundColor,
                                     theme.color("textPrimary"), 4.5);
        }
        if (m_config.customTextColor.isValid()) {
            m_textColor = theme.ensureContrast(m_config.customTextColor,
                                               m_backgroundColor, 4.5);
        }
        if (m_config.customBorderColor.isValid()) {
            m_borderColor = m_config.customBorderColor;
        }
    }

    // Apply semantic colors to child widgets with proper Fluent styling
    const QString labelStyle = QString(
                                   "QLabel { "
                                   "color: %1; "
                                   "background: transparent; "
                                   "padding: %2px %3px; "
                                   "}")
                                   .arg(m_textColor.name())
                                   .arg(theme.paddingValue("small"))
                                   .arg(theme.paddingValue("medium"));

    const QString editStyle = QString(
                                  "QLineEdit { "
                                  "color: %1; "
                                  "background: transparent; "
                                  "border: none; "
                                  "padding: %2px %3px; "
                                  "selection-background-color: %4; "
                                  "}")
                                  .arg(m_textColor.name())
                                  .arg(theme.paddingValue("small"))
                                  .arg(theme.paddingValue("medium"))
                                  .arg(theme.color("accent").name());

    const QString buttonStyle =
        QString(
            "QPushButton { "
            "color: %1; "
            "background: transparent; "
            "border: none; "
            "border-radius: %2px; "
            "padding: %3px; "
            "}"
            "QPushButton:hover { "
            "background: %4; "
            "}"
            "QPushButton:pressed { "
            "background: %5; "
            "}")
            .arg(theme.color("textSecondary").name())
            .arg(theme.borderRadius("small"))
            .arg(theme.paddingValue("small"))
            .arg(theme.color("controlFillSecondary").name())
            .arg(theme.color("controlFillTertiary").name());

    if (m_displayLabel)
        m_displayLabel->setStyleSheet(labelStyle);
    if (m_searchEdit)
        m_searchEdit->setStyleSheet(editStyle);
    if (m_clearButton)
        m_clearButton->setStyleSheet(buttonStyle);
    if (m_dropdownButton)
        m_dropdownButton->setStyleSheet(buttonStyle);
}

void FluentSelect::updateFonts() {
    const QFont font = getFont();

    if (m_displayLabel)
        m_displayLabel->setFont(font);
    if (m_searchEdit)
        m_searchEdit->setFont(font);
}

void FluentSelect::updateDropdownGeometry() {
    if (!m_dropdown)
        return;

    m_dropdown->setMaxVisibleItems(m_config.maxVisibleItems);
    m_dropdown->setMaxHeight(m_config.maxDropdownHeight);
    m_dropdown->setMinWidth(
        m_config.minDropdownWidth > 0 ? m_config.minDropdownWidth : width());
    m_dropdown->setMaxWidth(m_config.maxDropdownWidth);
}

void FluentSelect::updateDropdownPosition() {
    if (!m_dropdown)
        return;

    m_dropdown->updatePosition();
}

void FluentSelect::updateDisplayText() {
    const QString text = formatDisplayText();
    m_cachedDisplayText = text;

    if (m_config.editable && m_searchEdit) {
        if (!m_searchActive) {
            m_searchEdit->setText(text);
        }
    } else if (m_displayLabel) {
        m_displayLabel->setText(text.isEmpty() ? m_config.placeholderText
                                               : text);
    }
}

void FluentSelect::updatePlaceholder() {
    if (m_config.editable && m_searchEdit) {
        m_searchEdit->setPlaceholderText(m_config.placeholderText);
    } else if (m_displayLabel && currentText().isEmpty()) {
        m_displayLabel->setText(m_config.placeholderText);
    }
}

void FluentSelect::updateAccessibility() {
    // Set proper ARIA role
    setAccessibleName(tr("Select ComboBox"));

    // Set accessible name
    QString accessibleName = m_config.placeholderText.isEmpty()
                                 ? "Select"
                                 : m_config.placeholderText;
    setAccessibleName(accessibleName);

    // Set comprehensive accessible description
    QString description;
    if (m_config.mode == FluentSelectMode::Single) {
        description =
            QString("Single selection dropdown with %1 items").arg(count());
    } else {
        const int selectedCount = selectedIndexes().size();
        description =
            QString("Multiple selection dropdown with %1 items, %2 selected")
                .arg(count())
                .arg(selectedCount);
    }

    if (m_config.searchable) {
        description += ", searchable";
    }
    if (m_config.editable) {
        description += ", editable";
    }
    if (!isEnabled()) {
        description += ", disabled";
    }

    setAccessibleDescription(description);

    // Set state properties
    setProperty("aria-expanded", m_dropdownVisible);
    setProperty("aria-haspopup", "listbox");
    setProperty("aria-autocomplete", m_config.searchable ? "list" : "none");

    // Set current selection for screen readers
    if (m_config.mode == FluentSelectMode::Single) {
        const QString currentText = this->currentText();
        if (!currentText.isEmpty()) {
            setProperty("aria-valuenow", currentIndex());
            setProperty("aria-valuetext", currentText);
        }
    } else {
        const QStringList selectedTexts = this->selectedTexts();
        if (!selectedTexts.isEmpty()) {
            setProperty("aria-valuetext", selectedTexts.join(", "));
        }
    }

    // Set validation state
    if (m_validator && !isValid()) {
        setProperty("aria-invalid", "true");
        setProperty("aria-errormessage", m_validationErrorMessage);
    } else {
        setProperty("aria-invalid", "false");
        setProperty("aria-errormessage", QString());
    }

    // Update child widget accessibility
    if (m_searchEdit) {
        m_searchEdit->setAccessibleName("Search in " + accessibleName);
        m_searchEdit->setAccessibleDescription("Type to search items");
    }

    if (m_clearButton) {
        m_clearButton->setAccessibleName("Clear selection");
        m_clearButton->setAccessibleDescription("Clear current selection");
    }

    if (m_dropdownButton) {
        m_dropdownButton->setAccessibleName("Open dropdown");
        m_dropdownButton->setAccessibleDescription(
            m_dropdownVisible ? "Close dropdown" : "Open dropdown");
    }
}

QString FluentSelect::formatDisplayText() const {
    if (m_config.mode == FluentSelectMode::Single) {
        return currentText();
    } else {
        return formatMultipleSelectionText();
    }
}

QString FluentSelect::formatMultipleSelectionText() const {
    const QStringList selected = selectedTexts();

    if (selected.isEmpty()) {
        return QString();
    } else if (selected.size() == 1) {
        return selected.first();
    } else {
        return QString("%1 items selected").arg(selected.size());
    }
}

void FluentSelect::applySearchFilter() {
    if (!m_proxyModel)
        return;

    if (m_config.searchMode == FluentSelectSearchMode::None) {
        m_proxyModel->setFilterFixedString(QString());
        return;
    }

    if (m_customSearchFunction) {
        // Custom search would require a custom proxy model
        // For now, use standard filtering
    }

    switch (m_config.searchMode) {
        case FluentSelectSearchMode::StartsWith:
            m_proxyModel->setFilterRegularExpression(QRegularExpression(
                "^" + QRegularExpression::escape(m_searchText)));
            break;
        case FluentSelectSearchMode::Contains:
            m_proxyModel->setFilterFixedString(m_searchText);
            break;
        default:
            m_proxyModel->setFilterFixedString(QString());
            break;
    }
}

void FluentSelect::resetSearchFilter() {
    if (m_proxyModel) {
        m_proxyModel->setFilterFixedString(QString());
    }
    m_searchText.clear();
}

int FluentSelect::getItemHeight() const {
    const auto& theme = FluentTheme::instance();

    switch (m_config.size) {
        case FluentSelectSize::Small:
            return theme.componentHeight("small");
        case FluentSelectSize::Large:
            return theme.componentHeight("large");
        case FluentSelectSize::Medium:
        default:
            return theme.componentHeight("medium");
    }
}

int FluentSelect::getDropdownHeight() const {
    const int itemHeight = getItemHeight();
    const int maxItems = qMin(m_config.maxVisibleItems, count());
    return maxItems * itemHeight + 8;  // Add padding
}

QSize FluentSelect::getDropdownSize() const {
    const int width = qMax(this->width(), m_config.minDropdownWidth);
    const int height = qMin(getDropdownHeight(), m_config.maxDropdownHeight);
    return QSize(width, height);
}

QColor FluentSelect::getBackgroundColor() const { return m_backgroundColor; }

QColor FluentSelect::getTextColor() const { return m_textColor; }

QColor FluentSelect::getBorderColor() const { return m_borderColor; }

QColor FluentSelect::getPlaceholderColor() const { return m_placeholderColor; }

QFont FluentSelect::getFont() const {
    const auto& theme = FluentTheme::instance();

    switch (m_config.size) {
        case FluentSelectSize::Small:
            return theme.bodySmallFont();
        case FluentSelectSize::Large:
            return theme.bodyLargeFont();
        case FluentSelectSize::Medium:
        default:
            return theme.bodyFont();
    }
}

// Complete event handler implementations
void FluentSelect::onDropdownItemActivated(const QModelIndex& index) {
    if (!index.isValid() || !m_selectModel) {
        return;
    }

    // Map proxy index to source if needed
    QModelIndex sourceIndex = index;
    if (m_proxyModel) {
        sourceIndex = m_proxyModel->mapToSource(index);
    }

    const FluentSelectItem item = m_selectModel->itemAt(sourceIndex.row());

    // Don't activate separators or groups
    if (!item.isSelectable()) {
        return;
    }

    // Handle selection based on mode
    if (m_config.mode == FluentSelectMode::Single) {
        setCurrentModelIndex(index);
        hideDropdown();
        emit activated(sourceIndex.row());
        emit activated(item.text());
    } else {
        // For multiple selection, toggle the item
        if (m_selectionModel) {
            const bool isSelected = m_selectionModel->isSelected(index);
            if (isSelected) {
                m_selectionModel->select(index, QItemSelectionModel::Deselect);
            } else {
                m_selectionModel->select(index, QItemSelectionModel::Select);
            }
            updateDisplayText();
            emit selectionChanged();
        }
    }

    // Validate selection if validator is set
    if (m_validator && !isValid()) {
        // Revert selection if validation fails
        if (m_config.mode == FluentSelectMode::Single) {
            setCurrentIndex(-1);
        }
        emit validationFailed(m_validationErrorMessage);
    }
}

void FluentSelect::onDropdownItemClicked(const QModelIndex& index) {
    if (!index.isValid()) {
        return;
    }

    // Highlight the item first
    if (m_selectionModel) {
        m_selectionModel->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
        emit highlighted(index.row());

        QModelIndex sourceIndex = index;
        if (m_proxyModel) {
            sourceIndex = m_proxyModel->mapToSource(index);
        }

        if (sourceIndex.isValid() && m_selectModel) {
            const FluentSelectItem item =
                m_selectModel->itemAt(sourceIndex.row());
            emit highlighted(item.text());
        }
    }

    // Then activate the item
    onDropdownItemActivated(index);
}

void FluentSelect::onDropdownSelectionChanged() {
    updateDisplayText();

    // Update clear button visibility
    if (m_clearButton) {
        m_clearButton->setVisible(m_config.clearable &&
                                  !currentText().isEmpty());
    }

    // Emit selection changed signal
    emit selectionChanged();

    // Update accessibility information
    updateAccessibility();

    // Validate current selection
    if (m_validator && !isValid()) {
        setState(FluentState::Disabled);  // Visual indication of invalid state
        emit validationFailed(m_validationErrorMessage);
    } else {
        if (state() == FluentState::Disabled && isEnabled()) {
            setState(FluentState::Normal);
        }
    }
}

void FluentSelect::onSearchTextChanged(const QString& text) {
    m_searchText = text;
    m_searchActive = !text.isEmpty();

    // Apply search filter
    applySearchFilter();

    // Show dropdown if search is active and has results
    if (m_searchActive && m_proxyModel && m_proxyModel->rowCount() > 0) {
        if (!m_dropdownVisible) {
            showDropdown();
        }
    } else if (m_searchActive && m_proxyModel &&
               m_proxyModel->rowCount() == 0) {
        // Show "no results" message in dropdown
        if (!m_dropdownVisible) {
            showDropdown();
        }
    }

    emit searchFilterChanged(text);
}

void FluentSelect::onSearchTextEdited(const QString& text) {
    if (!m_config.editable) {
        return;
    }

    // Handle text editing in editable mode
    m_searchActive = true;
    onSearchTextChanged(text);

    // Try to find exact match and select it
    if (!text.isEmpty()) {
        const int exactMatch = findText(text, Qt::MatchExactly);
        if (exactMatch >= 0) {
            setCurrentIndex(exactMatch);
        }
    }

    emit textEdited(text);
    emit textChanged(text);
}

void FluentSelect::onClearButtonClicked() { clearSelection(); }

void FluentSelect::onDropdownButtonClicked() { toggleDropdown(); }

void FluentSelect::onModelDataChanged(const QModelIndex& topLeft,
                                      const QModelIndex& bottomRight) {
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    updateDisplayText();
}

void FluentSelect::onModelRowsInserted(const QModelIndex& parent, int first,
                                       int last) {
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    updateDisplayText();
}

void FluentSelect::onModelRowsRemoved(const QModelIndex& parent, int first,
                                      int last) {
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    updateDisplayText();
}

void FluentSelect::onModelReset() { updateDisplayText(); }

void FluentSelect::onDropdownAnimationFinished() {
    // Animation finished
}

void FluentSelect::onThemeChanged() {
    updateColors();
    updateFonts();
    m_sizeHintValid = false;
    updateGeometry();
    update();
}

// Helper methods for improved keyboard navigation and accessibility
void FluentSelect::handleDropdownNavigation(int direction) {
    if (!m_selectionModel || !m_proxyModel) {
        return;
    }

    const QModelIndex currentIndex = m_selectionModel->currentIndex();
    int newRow = 0;

    if (currentIndex.isValid()) {
        newRow = currentIndex.row() + direction;
    } else if (direction > 0) {
        newRow = 0;
    } else {
        newRow = m_proxyModel->rowCount() - 1;
    }

    // Clamp to valid range
    newRow = qMax(0, qMin(newRow, m_proxyModel->rowCount() - 1));

    const QModelIndex newIndex = m_proxyModel->index(newRow, 0);
    if (newIndex.isValid()) {
        m_selectionModel->setCurrentIndex(newIndex,
                                          QItemSelectionModel::NoUpdate);

        // Scroll to ensure visibility
        if (m_listView) {
            m_listView->scrollTo(newIndex, QAbstractItemView::EnsureVisible);
        }

        // Emit highlighted signal
        QModelIndex sourceIndex = newIndex;
        if (m_proxyModel) {
            sourceIndex = m_proxyModel->mapToSource(newIndex);
        }

        if (sourceIndex.isValid()) {
            emit highlighted(sourceIndex.row());
            if (m_selectModel) {
                const FluentSelectItem item =
                    m_selectModel->itemAt(sourceIndex.row());
                emit highlighted(item.text());
            }
        }
    }
}

void FluentSelect::handleTextInput(QKeyEvent* event) {
    if (!event || event->text().isEmpty()) {
        return;
    }

    const QString inputChar = event->text().toLower();

    // Implement type-ahead search
    static QString typeAheadBuffer;
    static QTimer typeAheadTimer;

    // Reset buffer after 1 second of inactivity
    if (!typeAheadTimer.isActive()) {
        typeAheadBuffer.clear();
    }

    typeAheadBuffer += inputChar;

    // Find matching item
    if (m_selectModel) {
        for (int i = 0; i < m_selectModel->itemCount(); ++i) {
            const FluentSelectItem item = m_selectModel->itemAt(i);
            if (item.text().toLower().startsWith(typeAheadBuffer)) {
                setCurrentIndex(i);
                emit highlighted(i);
                emit highlighted(item.text());
                break;
            }
        }
    }

    // Set timer to clear buffer
    typeAheadTimer.setSingleShot(true);
    typeAheadTimer.start(1000);

    event->accept();
}

// Event handling methods
bool FluentSelect::eventFilter(QObject* object, QEvent* event) {
    Q_UNUSED(object)
    Q_UNUSED(event)
    return FluentComponent::eventFilter(object, event);
}

void FluentSelect::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_dropdown && m_dropdown->isVisible()) {
            hideDropdown();
        } else {
            showDropdown();
        }
    }
    FluentComponent::mousePressEvent(event);
}

void FluentSelect::mouseReleaseEvent(QMouseEvent* event) {
    FluentComponent::mouseReleaseEvent(event);
}

void FluentSelect::wheelEvent(QWheelEvent* event) {
    if (m_config.mode == FluentSelectMode::Single && count() > 0) {
        int delta = event->angleDelta().y();
        int current = currentIndex();
        if (delta > 0 && current > 0) {
            setCurrentIndex(current - 1);
        } else if (delta < 0 && current < count() - 1) {
            setCurrentIndex(current + 1);
        }
        event->accept();
    } else {
        FluentComponent::wheelEvent(event);
    }
}

void FluentSelect::keyPressEvent(QKeyEvent* event) {
    // Handle different key combinations for comprehensive navigation
    const bool ctrlPressed = event->modifiers() & Qt::ControlModifier;
    const bool shiftPressed = event->modifiers() & Qt::ShiftModifier;

    switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (m_dropdown && m_dropdown->isVisible()) {
                // If dropdown is open, activate current highlighted item
                if (m_selectionModel && m_selectionModel->hasSelection()) {
                    const QModelIndex current =
                        m_selectionModel->currentIndex();
                    if (current.isValid()) {
                        onDropdownItemActivated(current);
                    }
                } else {
                    hideDropdown();
                }
            } else {
                showDropdown();
            }
            event->accept();
            break;

        case Qt::Key_Up:
            if (m_dropdown && m_dropdown->isVisible()) {
                // Navigate in dropdown
                handleDropdownNavigation(-1);
            } else if (count() > 0) {
                // Navigate in closed dropdown
                int current = currentIndex();
                if (current > 0) {
                    setCurrentIndex(current - 1);
                    emit highlighted(current - 1);
                }
            }
            event->accept();
            break;

        case Qt::Key_Down:
            if (m_dropdown && m_dropdown->isVisible()) {
                // Navigate in dropdown
                handleDropdownNavigation(1);
            } else if (count() > 0) {
                // Navigate in closed dropdown or open it
                if (!m_dropdownVisible) {
                    showDropdown();
                } else {
                    int current = currentIndex();
                    if (current < count() - 1) {
                        setCurrentIndex(current + 1);
                        emit highlighted(current + 1);
                    }
                }
            }
            event->accept();
            break;

        case Qt::Key_Home:
            if (count() > 0) {
                setCurrentIndex(0);
                emit highlighted(0);
            }
            event->accept();
            break;

        case Qt::Key_End:
            if (count() > 0) {
                setCurrentIndex(count() - 1);
                emit highlighted(count() - 1);
            }
            event->accept();
            break;

        case Qt::Key_PageUp:
            if (m_dropdown && m_dropdown->isVisible()) {
                handleDropdownNavigation(-m_config.maxVisibleItems);
            } else if (count() > 0) {
                int current = currentIndex();
                int newIndex = qMax(0, current - m_config.maxVisibleItems);
                setCurrentIndex(newIndex);
                emit highlighted(newIndex);
            }
            event->accept();
            break;

        case Qt::Key_PageDown:
            if (m_dropdown && m_dropdown->isVisible()) {
                handleDropdownNavigation(m_config.maxVisibleItems);
            } else if (count() > 0) {
                int current = currentIndex();
                int newIndex =
                    qMin(count() - 1, current + m_config.maxVisibleItems);
                setCurrentIndex(newIndex);
                emit highlighted(newIndex);
            }
            event->accept();
            break;

        case Qt::Key_Escape:
            if (m_dropdown && m_dropdown->isVisible()) {
                hideDropdown();
                event->accept();
            } else {
                FluentComponent::keyPressEvent(event);
            }
            break;

        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            // Allow normal tab navigation
            FluentComponent::keyPressEvent(event);
            break;

        case Qt::Key_A:
            if (ctrlPressed && m_config.mode != FluentSelectMode::Single) {
                selectAll();
                event->accept();
            } else {
                handleTextInput(event);
            }
            break;

        case Qt::Key_Delete:
        case Qt::Key_Backspace:
            if (m_config.clearable) {
                clearSelection();
                event->accept();
            } else {
                FluentComponent::keyPressEvent(event);
            }
            break;

        default:
            // Handle type-ahead search for printable characters
            if (event->text().length() == 1 && event->text().at(0).isPrint()) {
                handleTextInput(event);
            } else {
                FluentComponent::keyPressEvent(event);
            }
            break;
    }
}

void FluentSelect::focusInEvent(QFocusEvent* event) {
    FluentComponent::focusInEvent(event);
    setState(FluentState::Focused);
    update();
}

void FluentSelect::focusOutEvent(QFocusEvent* event) {
    FluentComponent::focusOutEvent(event);
    if (m_dropdown && m_dropdown->isVisible()) {
        hideDropdown();
    }
    setState(FluentState::Normal);
    update();
}

void FluentSelect::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QRect rect = this->rect();
    const auto& theme = FluentTheme::instance();

    // Get proper spacing and sizing from theme
    const int borderRadius = theme.borderRadius("medium");
    const int padding = theme.paddingValue("medium");
    const int smallPadding = theme.paddingValue("small");

    // Draw background with proper state colors
    QColor backgroundColor = theme.color("controlFillDefault");
    if (!isEnabled()) {
        backgroundColor = theme.color("controlFillDisabled");
    } else if (state() == FluentState::Pressed) {
        backgroundColor = theme.color("controlFillTertiary");
    } else if (state() == FluentState::Hovered) {
        backgroundColor = theme.color("controlFillSecondary");
    } else if (state() == FluentState::Focused) {
        backgroundColor = theme.color("controlFillInputActive");
    }

    // Add subtle elevation for the component
    if (isEnabled() && state() != FluentState::Pressed) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 10));  // Subtle shadow
        painter.drawRoundedRect(rect.adjusted(1, 1, 1, 1), borderRadius,
                                borderRadius);
    }

    painter.fillRect(rect, backgroundColor);

    // Draw border with proper focus ring
    QPen borderPen(theme.color("controlStrokeDefault"));
    int borderWidth = 1;

    if (!isEnabled()) {
        borderPen.setColor(theme.color("controlStrokeDisabled"));
    } else if (state() == FluentState::Focused) {
        borderPen.setColor(theme.color("accent"));
        borderWidth = 2;

        // Draw focus ring
        QPen focusRingPen(theme.color("accent"));
        focusRingPen.setWidth(1);
        focusRingPen.setStyle(Qt::DashLine);
        painter.setPen(focusRingPen);
        painter.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), borderRadius + 2,
                                borderRadius + 2);
    }

    borderPen.setWidth(borderWidth);
    painter.setPen(borderPen);
    painter.drawRoundedRect(rect.adjusted(0, 0, -borderWidth, -borderWidth),
                            borderRadius, borderRadius);

    // Draw text with proper typography
    QString displayText = currentText();
    QColor textColor = theme.color("textPrimary");

    if (!isEnabled()) {
        textColor = theme.color("textDisabled");
    } else if (displayText.isEmpty()) {
        displayText = m_config.placeholderText;
        textColor = theme.color("textSecondary");
    }

    if (!displayText.isEmpty()) {
        painter.setPen(textColor);
        painter.setFont(getFont());

        // Use proper text rectangle with theme spacing
        QRect textRect = rect.adjusted(padding, 0, -(padding + 24), 0);

        // Handle text elision for long text
        QFontMetrics fm(getFont());
        const QString elidedText =
            fm.elidedText(displayText, Qt::ElideRight, textRect.width());

        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                         elidedText);
    }

    // Draw dropdown arrow with proper Fluent styling
    const int arrowSize = 8;
    const int arrowPadding = padding;
    QRect arrowRect(rect.right() - arrowPadding - arrowSize,
                    rect.center().y() - arrowSize / 2, arrowSize, arrowSize);

    QColor arrowColor = theme.color("textSecondary");
    if (!isEnabled()) {
        arrowColor = theme.color("textDisabled");
    } else if (state() == FluentState::Hovered ||
               state() == FluentState::Pressed) {
        arrowColor = theme.color("textPrimary");
    }

    painter.setPen(
        QPen(arrowColor, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    // Draw chevron down arrow
    const QPointF arrowPoints[] = {
        QPointF(arrowRect.left() + 2, arrowRect.top() + 3),
        QPointF(arrowRect.center().x(), arrowRect.bottom() - 2),
        QPointF(arrowRect.right() - 2, arrowRect.top() + 3)};

    painter.drawPolyline(arrowPoints, 3);

    // Draw clear button if visible and clearable
    if (m_config.clearable && !currentText().isEmpty() && isEnabled()) {
        const int clearButtonSize = 16;
        QRect clearRect(rect.right() - arrowPadding - arrowSize -
                            clearButtonSize - smallPadding,
                        rect.center().y() - clearButtonSize / 2,
                        clearButtonSize, clearButtonSize);

        painter.setPen(QPen(theme.color("textSecondary"), 1.5, Qt::SolidLine,
                            Qt::RoundCap));

        // Draw X for clear button
        painter.drawLine(clearRect.topLeft() + QPoint(4, 4),
                         clearRect.bottomRight() - QPoint(4, 4));
        painter.drawLine(clearRect.topRight() + QPoint(-4, 4),
                         clearRect.bottomLeft() + QPoint(4, -4));
    }
}

void FluentSelect::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);
    if (m_dropdown) {
        m_dropdown->updateGeometry();
    }
}

void FluentSelect::changeEvent(QEvent* event) {
    FluentComponent::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        updateColors();
        update();
    }
}

void FluentSelect::updateStateStyle() {
    updateColors();
    update();
}

void FluentSelect::performStateTransition(Core::FluentState from,
                                          Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    updateStateStyle();
}
