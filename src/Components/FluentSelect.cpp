// src/Components/FluentSelect.cpp
#include "FluentQt/Components/FluentSelect.h"
#include "FluentQt/Components/FluentSelectDropdown.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QApplication>
#include <QFontMetrics>
#include <QCompleter>
#include <QRegularExpression>
#include <QDebug>

using namespace FluentQt::Components;
using namespace FluentQt::Core;
using namespace FluentQt::Styling;

FluentSelect::FluentSelect(QWidget* parent)
    : FluentComponent(parent)
{
    setupUI();
    setupDropdown();
    setupAnimations();
    setupModel();
    setupConnections();
    
    // Connect to theme changes
    connect(&FluentTheme::instance(), &FluentTheme::themeChanged,
            this, &FluentSelect::onThemeChanged);
    
    updateColors();
    updateFonts();
    updateAccessibility();
    
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
}

FluentSelect::FluentSelect(FluentSelectMode mode, QWidget* parent)
    : FluentSelect(parent)
{
    m_config.mode = mode;
    updateLayout();
}

FluentSelect::FluentSelect(const FluentSelectConfig& config, QWidget* parent)
    : FluentSelect(parent)
{
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

QVariant FluentSelect::currentData() const {
    return currentData(Qt::UserRole);
}

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
        m_selectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
        
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
    const bool searchModeHasChanged = (m_config.searchMode != config.searchMode);
    const bool sizeHasChanged = (m_config.size != config.size);
    const bool editableHasChanged = (m_config.editable != config.editable);
    const bool clearableHasChanged = (m_config.clearable != config.clearable);
    const bool searchableHasChanged = (m_config.searchable != config.searchable);
    const bool maxVisibleHasChanged = (m_config.maxVisibleItems != config.maxVisibleItems);
    const bool placeholderHasChanged = (m_config.placeholderText != config.placeholderText);

    m_config = config;

    m_sizeHintValid = false;
    updateLayout();
    // updateDropdownSettings(); // Will be implemented later
    updateColors();
    updateFonts();
    updatePlaceholder();
    updateAccessibility();

    // Emit change signals
    if (modeHasChanged) emit modeChanged(config.mode);
    if (searchModeHasChanged) emit searchModeChanged(config.searchMode);
    if (sizeHasChanged) emit sizeChanged(config.size);
    if (editableHasChanged) emit editableChanged(config.editable);
    if (clearableHasChanged) emit clearableChanged(config.clearable);
    if (searchableHasChanged) emit searchableChanged(config.searchable);
    if (maxVisibleHasChanged) emit maxVisibleItemsChanged(config.maxVisibleItems);
    if (placeholderHasChanged) emit placeholderChanged(config.placeholderText);
}

QAbstractItemModel* FluentSelect::model() const {
    return m_selectModel;
}

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
        m_listView->setModel(m_proxyModel ? static_cast<QAbstractItemModel*>(m_proxyModel) : m_selectModel);
    }
    
    // Connect new model
    if (m_selectModel) {
        connect(m_selectModel, &QAbstractItemModel::dataChanged,
                this, &FluentSelect::onModelDataChanged);
        connect(m_selectModel, &QAbstractItemModel::rowsInserted,
                this, &FluentSelect::onModelRowsInserted);
        connect(m_selectModel, &QAbstractItemModel::rowsRemoved,
                this, &FluentSelect::onModelRowsRemoved);
        connect(m_selectModel, &QAbstractItemModel::modelReset,
                this, &FluentSelect::onModelReset);
    }
    
    updateDisplayText();
}

// Item management convenience methods
void FluentSelect::addItem(const QString& text, const QVariant& data) {
    if (m_selectModel) {
        m_selectModel->addItem(FluentSelectItem(text, data));
    }
}

void FluentSelect::addItem(const QIcon& icon, const QString& text, const QVariant& data) {
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

void FluentSelect::insertItem(int index, const QString& text, const QVariant& data) {
    if (m_selectModel) {
        m_selectModel->insertItem(index, FluentSelectItem(text, data));
    }
}

void FluentSelect::insertItem(int index, const QIcon& icon, const QString& text, const QVariant& data) {
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

bool FluentSelect::isEmpty() const {
    return count() == 0;
}

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

int FluentSelect::findData(const QVariant& data, int role, Qt::MatchFlags flags) const {
    Q_UNUSED(role) // For future use
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

QString FluentSelect::searchFilter() const {
    return m_searchText;
}

void FluentSelect::clearSearchFilter() {
    setSearchFilter(QString());
}

void FluentSelect::setCustomSearchFunction(std::function<bool(const FluentSelectItem&, const QString&)> searchFunc) {
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

void FluentSelect::setValidator(std::function<bool(const QVariant&)> validator) {
    m_validator = std::move(validator);
}

void FluentSelect::setValidationErrorMessage(const QString& message) {
    m_validationErrorMessage = message;
}

QSize FluentSelect::sizeHint() const {
    if (!m_sizeHintValid) {
        const_cast<FluentSelect*>(this)->m_cachedSizeHint = calculateSizeHintInternal();
        const_cast<FluentSelect*>(this)->m_sizeHintValid = true;
    }
    return m_cachedSizeHint;
}

QSize FluentSelect::minimumSizeHint() const {
    return QSize(120, getItemHeight());
}

QSize FluentSelect::calculateSizeHintInternal() const {
    const QFontMetrics metrics(getFont());

    int width = 200; // Base width
    int height = getItemHeight();

    // Calculate width based on content
    if (m_selectModel && !m_selectModel->isEmpty()) {
        for (int i = 0; i < m_selectModel->itemCount(); ++i) {
            const FluentSelectItem item = m_selectModel->itemAt(i);
            const int textWidth = metrics.horizontalAdvance(item.text());
            width = qMax(width, textWidth + 60); // Add space for icon and padding
        }
    }

    // Add space for dropdown button
    width += 32;

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
    if (m_config.mode == FluentSelectMode::Single || !m_selectionModel || !m_selectModel) {
        return;
    }

    // Select all items manually since QItemSelectionModel doesn't have selectAll()
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
    if (m_config.mode == FluentSelectMode::Single || !m_selectionModel || !m_selectModel) {
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
    m_displayLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
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
    connect(m_clearButton, &QPushButton::clicked, this, &FluentSelect::onClearButtonClicked);
    m_mainLayout->addWidget(m_clearButton);

    // Dropdown button
    m_dropdownButton = new QPushButton();
    m_dropdownButton->setFixedSize(20, 20);
    m_dropdownButton->setFlat(true);
    m_dropdownButton->setText("▼");
    connect(m_dropdownButton, &QPushButton::clicked, this, &FluentSelect::onDropdownButtonClicked);
    m_mainLayout->addWidget(m_dropdownButton);

    updateLayout();
}

void FluentSelect::setupDropdown() {
    m_dropdown = new FluentSelectDropdown(this);
    m_dropdown->setVisible(false);

    m_listView = m_dropdown->listView();

    // Connect dropdown signals
    connect(m_dropdown, &FluentSelectDropdown::itemActivated,
            this, &FluentSelect::onDropdownItemActivated);
    connect(m_dropdown, &FluentSelectDropdown::itemClicked,
            this, &FluentSelect::onDropdownItemClicked);
    connect(m_dropdown, &FluentSelectDropdown::selectionChanged,
            this, &FluentSelect::onDropdownSelectionChanged);
    connect(m_dropdown, &FluentSelectDropdown::searchTextChanged,
            this, &FluentSelect::onSearchTextChanged);
    connect(m_dropdown, &FluentSelectDropdown::hidden,
            this, &FluentSelect::onDropdownAnimationFinished);
}

void FluentSelect::setupAnimations() {
    // Dropdown show/hide animations
    m_dropdownAnimation = std::make_unique<QPropertyAnimation>(m_dropdown, "geometry");
    m_dropdownAnimation->setDuration(200);
    m_dropdownAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_dropdownAnimation.get(), &QPropertyAnimation::finished,
            this, &FluentSelect::onDropdownAnimationFinished);
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
        connect(m_searchEdit, &QLineEdit::textChanged,
                this, &FluentSelect::onSearchTextChanged);
        connect(m_searchEdit, &QLineEdit::textEdited,
                this, &FluentSelect::onSearchTextEdited);
    }

    // Selection model connections
    if (m_selectionModel) {
        connect(m_selectionModel, &QItemSelectionModel::selectionChanged,
                this, &FluentSelect::onDropdownSelectionChanged);
    }
}

void FluentSelect::updateLayout() {
    if (!m_displayLabel || !m_searchEdit) return;

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
        m_clearButton->setVisible(m_config.clearable && !currentText().isEmpty());
    }

    updateDisplayText();
}

void FluentSelect::updateColors() {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (m_config.autoCalculateColors) {
        m_backgroundColor = palette.neutralLightest;
        m_textColor = palette.neutralPrimary;
        m_borderColor = palette.neutralTertiary;
        m_placeholderColor = palette.neutralSecondary;
        m_focusColor = palette.accent;
    } else {
        if (m_config.customBackgroundColor.isValid()) {
            m_backgroundColor = m_config.customBackgroundColor;
        }
        if (m_config.customTextColor.isValid()) {
            m_textColor = m_config.customTextColor;
        }
        if (m_config.customBorderColor.isValid()) {
            m_borderColor = m_config.customBorderColor;
        }
    }

    // Apply colors to widgets
    const QString styleSheet = QString(
        "QLabel { color: %1; background: transparent; }"
        "QLineEdit { color: %1; background: transparent; border: none; }"
        "QPushButton { color: %2; background: transparent; border: none; }"
    ).arg(m_textColor.name()).arg(m_borderColor.name());

    if (m_displayLabel) m_displayLabel->setStyleSheet(styleSheet);
    if (m_searchEdit) m_searchEdit->setStyleSheet(styleSheet);
    if (m_clearButton) m_clearButton->setStyleSheet(styleSheet);
    if (m_dropdownButton) m_dropdownButton->setStyleSheet(styleSheet);
}

void FluentSelect::updateFonts() {
    const QFont font = getFont();

    if (m_displayLabel) m_displayLabel->setFont(font);
    if (m_searchEdit) m_searchEdit->setFont(font);
}

void FluentSelect::updateDropdownGeometry() {
    if (!m_dropdown) return;

    m_dropdown->setMaxVisibleItems(m_config.maxVisibleItems);
    m_dropdown->setMaxHeight(m_config.maxDropdownHeight);
    m_dropdown->setMinWidth(m_config.minDropdownWidth > 0 ? m_config.minDropdownWidth : width());
    m_dropdown->setMaxWidth(m_config.maxDropdownWidth);
}

void FluentSelect::updateDropdownPosition() {
    if (!m_dropdown) return;

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
        m_displayLabel->setText(text.isEmpty() ? m_config.placeholderText : text);
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
    setAccessibleName(m_config.placeholderText.isEmpty() ? "Select" : m_config.placeholderText);
    setAccessibleDescription(QString("Select dropdown with %1 items").arg(count()));
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
    if (!m_proxyModel) return;

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
            m_proxyModel->setFilterRegularExpression(QRegularExpression("^" + QRegularExpression::escape(m_searchText)));
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
    switch (m_config.size) {
        case FluentSelectSize::Small:
            return 28;
        case FluentSelectSize::Large:
            return 40;
        case FluentSelectSize::Medium:
        default:
            return 32;
    }
}

int FluentSelect::getDropdownHeight() const {
    const int itemHeight = getItemHeight();
    const int maxItems = qMin(m_config.maxVisibleItems, count());
    return maxItems * itemHeight + 8; // Add padding
}

QSize FluentSelect::getDropdownSize() const {
    const int width = qMax(this->width(), m_config.minDropdownWidth);
    const int height = qMin(getDropdownHeight(), m_config.maxDropdownHeight);
    return QSize(width, height);
}

QColor FluentSelect::getBackgroundColor() const {
    return m_backgroundColor;
}

QColor FluentSelect::getTextColor() const {
    return m_textColor;
}

QColor FluentSelect::getBorderColor() const {
    return m_borderColor;
}

QColor FluentSelect::getPlaceholderColor() const {
    return m_placeholderColor;
}

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

// Event handler stubs (to be implemented)
void FluentSelect::onDropdownItemActivated(const QModelIndex& index) {
    Q_UNUSED(index)
    // Implementation will be added later
}

void FluentSelect::onDropdownItemClicked(const QModelIndex& index) {
    Q_UNUSED(index)
    // Implementation will be added later
}

void FluentSelect::onDropdownSelectionChanged() {
    // Implementation will be added later
}

void FluentSelect::onSearchTextChanged(const QString& text) {
    Q_UNUSED(text)
    // Implementation will be added later
}

void FluentSelect::onSearchTextEdited(const QString& text) {
    Q_UNUSED(text)
    // Implementation will be added later
}

void FluentSelect::onClearButtonClicked() {
    clearSelection();
}

void FluentSelect::onDropdownButtonClicked() {
    toggleDropdown();
}

void FluentSelect::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    updateDisplayText();
}

void FluentSelect::onModelRowsInserted(const QModelIndex& parent, int first, int last) {
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    updateDisplayText();
}

void FluentSelect::onModelRowsRemoved(const QModelIndex& parent, int first, int last) {
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    updateDisplayText();
}

void FluentSelect::onModelReset() {
    updateDisplayText();
}

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
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_dropdown && m_dropdown->isVisible()) {
            hideDropdown();
        } else {
            showDropdown();
        }
        event->accept();
        break;
    case Qt::Key_Up:
        if (count() > 0) {
            int current = currentIndex();
            if (current > 0) {
                setCurrentIndex(current - 1);
            }
        }
        event->accept();
        break;
    case Qt::Key_Down:
        if (count() > 0) {
            int current = currentIndex();
            if (current < count() - 1) {
                setCurrentIndex(current + 1);
            }
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
    default:
        FluentComponent::keyPressEvent(event);
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

    const QRect rect = this->rect();
    const auto& theme = FluentTheme::instance();

    // Draw background
    QColor backgroundColor = theme.color("ControlFillColorDefault");
    if (state() == FluentState::Hovered) {
        backgroundColor = theme.color("ControlFillColorSecondary");
    } else if (state() == FluentState::Pressed) {
        backgroundColor = theme.color("ControlFillColorTertiary");
    } else if (state() == FluentState::Focused) {
        backgroundColor = theme.color("ControlFillColorInputActive");
    }

    painter.fillRect(rect, backgroundColor);

    // Draw border
    QPen borderPen(theme.color("ControlStrokeColorDefault"));
    if (state() == FluentState::Focused) {
        borderPen.setColor(theme.color("AccentFillColorDefault"));
        borderPen.setWidth(2);
    }
    painter.setPen(borderPen);
    painter.drawRoundedRect(rect.adjusted(0, 0, -1, -1), 4, 4);

    // Draw text
    QString displayText = currentText();
    if (displayText.isEmpty()) {
        displayText = m_config.placeholderText;
        painter.setPen(theme.color("TextFillColorSecondary"));
    } else {
        painter.setPen(theme.color("TextFillColorPrimary"));
    }

    if (!displayText.isEmpty()) {
        painter.setFont(font());
        QRect textRect = rect.adjusted(12, 0, -32, 0);
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, displayText);
    }

    // Draw dropdown arrow
    QRect arrowRect(rect.right() - 24, rect.top() + (rect.height() - 12) / 2, 12, 12);
    painter.setPen(QPen(theme.color("TextFillColorSecondary"), 2));
    painter.drawLine(arrowRect.left() + 2, arrowRect.top() + 4,
                     arrowRect.center().x(), arrowRect.bottom() - 2);
    painter.drawLine(arrowRect.center().x(), arrowRect.bottom() - 2,
                     arrowRect.right() - 2, arrowRect.top() + 4);
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

void FluentSelect::performStateTransition(Core::FluentState from, Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    updateStateStyle();
}
