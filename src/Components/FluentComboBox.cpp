// src/Components/FluentComboBox.cpp
#include "FluentQt/Components/FluentComboBox.h"
#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QStyleOption>
#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <QWheelEvent>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentComboBox::FluentComboBox(QWidget* parent)
    : FluentComboBox(FluentComboBoxStyle::Standard, parent) {}

FluentComboBox::FluentComboBox(FluentComboBoxStyle style, QWidget* parent)
    : Core::FluentComponent(parent),
      m_style(style),
      m_model(new QStandardItemModel(this)),
      m_filteredModel(new QStandardItemModel(this)),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)),
      m_searchTimer(new QTimer(this)) {
    setupUI();
    setupDropdown();
    setupAnimations();
    updateColors();

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);

    // Connect theme changes
    auto& theme = Styling::FluentTheme::instance();
    connect(&theme, &Styling::FluentTheme::themeChanged, this,
            &FluentComboBox::updateColors);
}

FluentComboBox::~FluentComboBox() = default;

void FluentComboBox::setupUI() {
    setMinimumHeight(32);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Setup search timer
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(300);  // 300ms delay for search
    connect(m_searchTimer, &QTimer::timeout, this,
            &FluentComboBox::filterItems);

    if (m_style == FluentComboBoxStyle::Editable) {
        m_searchEdit = new QLineEdit(this);
        m_searchEdit->setPlaceholderText(m_placeholderText);
        m_searchEdit->hide();  // Initially hidden

        connect(m_searchEdit, &QLineEdit::textChanged, this,
                &FluentComboBox::onSearchTextChanged);
    }
}

void FluentComboBox::setupDropdown() {
    m_dropdown = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
    m_dropdown->setAttribute(Qt::WA_TranslucentBackground);
    m_dropdown->setWindowModality(Qt::NonModal);

    auto* dropdownLayout = new QVBoxLayout(m_dropdown);
    dropdownLayout->setContentsMargins(8, 8, 8, 8);
    dropdownLayout->setSpacing(0);

    m_listView = new QListView(m_dropdown);
    m_listView->setModel(m_filteredModel);
    m_listView->setSelectionMode(m_multiSelectEnabled
                                     ? QAbstractItemView::MultiSelection
                                     : QAbstractItemView::SingleSelection);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setFrameStyle(QFrame::NoFrame);
    m_listView->setUniformItemSizes(true);
    m_listView->setMouseTracking(true);

    connect(m_listView, &QListView::clicked, this,
            &FluentComboBox::onDropdownItemClicked);

    dropdownLayout->addWidget(m_listView);

    m_dropdown->hide();
}

void FluentComboBox::setupAnimations() {
    m_dropdownAnimation = new QPropertyAnimation(m_dropdown, "geometry", this);
    m_dropdownAnimation->setDuration(200);
    m_dropdownAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_dropdownAnimation, &QPropertyAnimation::finished, this,
            &FluentComboBox::onDropdownAnimationFinished);
}

// Style and behavior
FluentComboBoxStyle FluentComboBox::style() const { return m_style; }

void FluentComboBox::setStyle(FluentComboBoxStyle style) {
    if (m_style != style) {
        m_style = style;

        if (style == FluentComboBoxStyle::Editable && !m_searchEdit) {
            m_searchEdit = new QLineEdit(this);
            m_searchEdit->setPlaceholderText(m_placeholderText);
            connect(m_searchEdit, &QLineEdit::textChanged, this,
                    &FluentComboBox::onSearchTextChanged);
        } else if (style != FluentComboBoxStyle::Editable && m_searchEdit) {
            m_searchEdit->deleteLater();
            m_searchEdit = nullptr;
        }

        updateLayout();
        emit styleChanged(style);
    }
}

QString FluentComboBox::placeholderText() const { return m_placeholderText; }

void FluentComboBox::setPlaceholderText(const QString& text) {
    if (m_placeholderText != text) {
        m_placeholderText = text;
        if (m_searchEdit) {
            m_searchEdit->setPlaceholderText(text);
        }
        update();
        emit placeholderTextChanged(text);
    }
}

bool FluentComboBox::isSearchEnabled() const { return m_searchEnabled; }

void FluentComboBox::setSearchEnabled(bool enabled) {
    if (m_searchEnabled != enabled) {
        m_searchEnabled = enabled;
        emit searchEnabledChanged(enabled);
    }
}

bool FluentComboBox::isMultiSelectEnabled() const {
    return m_multiSelectEnabled;
}

void FluentComboBox::setMultiSelectEnabled(bool enabled) {
    if (m_multiSelectEnabled != enabled) {
        m_multiSelectEnabled = enabled;

        if (m_listView) {
            m_listView->setSelectionMode(
                enabled ? QAbstractItemView::MultiSelection
                        : QAbstractItemView::SingleSelection);
        }

        emit multiSelectEnabledChanged(enabled);
    }
}

// Current selection
int FluentComboBox::currentIndex() const { return m_currentIndex; }

void FluentComboBox::setCurrentIndex(int index) {
    if (index >= 0 && index < m_model->rowCount() && m_currentIndex != index) {
        m_currentIndex = index;
        updateCurrentItem();
        update();
        emit currentIndexChanged(index);
        emit currentTextChanged(currentText());
        emit currentDataChanged(currentData());
    }
}

QString FluentComboBox::currentText() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_model->rowCount()) {
        return m_model->item(m_currentIndex)->text();
    }
    return QString();
}

void FluentComboBox::setCurrentText(const QString& text) {
    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

QVariant FluentComboBox::currentData() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_model->rowCount()) {
        return m_model->item(m_currentIndex)->data(Qt::UserRole);
    }
    return QVariant();
}

QList<int> FluentComboBox::selectedIndexes() const { return m_selectedIndexes; }

void FluentComboBox::setSelectedIndexes(const QList<int>& indexes) {
    m_selectedIndexes = indexes;
    updateSelection();
    emit selectionChanged();
}

QStringList FluentComboBox::selectedTexts() const {
    QStringList texts;
    for (int index : m_selectedIndexes) {
        if (index >= 0 && index < m_model->rowCount()) {
            texts.append(m_model->item(index)->text());
        }
    }
    return texts;
}

QList<QVariant> FluentComboBox::selectedData() const {
    QList<QVariant> data;
    for (int index : m_selectedIndexes) {
        if (index >= 0 && index < m_model->rowCount()) {
            data.append(m_model->item(index)->data(Qt::UserRole));
        }
    }
    return data;
}

// Items management
int FluentComboBox::count() const { return m_model->rowCount(); }

void FluentComboBox::addItem(const QString& text) { addItem(text, QVariant()); }

void FluentComboBox::addItem(const QString& text, const QVariant& data) {
    addItem(text, QIcon(), data);
}

void FluentComboBox::addItem(const QString& text, const QIcon& icon,
                             const QVariant& data) {
    FluentComboBoxItem item(text, icon, data);
    addItem(item);
}

void FluentComboBox::addItem(const FluentComboBoxItem& item) {
    auto* standardItem = new QStandardItem(item.icon, item.text);
    standardItem->setData(item.data, Qt::UserRole);
    standardItem->setEnabled(item.enabled);

    if (item.separator) {
        standardItem->setData(true, Qt::UserRole + 1);  // Mark as separator
    }

    m_model->appendRow(standardItem);
    updateModel();
}

void FluentComboBox::addItems(const QStringList& texts) {
    for (const QString& text : texts) {
        addItem(text);
    }
}

void FluentComboBox::addItems(const QList<FluentComboBoxItem>& items) {
    for (const FluentComboBoxItem& item : items) {
        addItem(item);
    }
}

void FluentComboBox::insertItem(int index, const QString& text) {
    insertItem(index, text, QVariant());
}

void FluentComboBox::insertItem(int index, const QString& text,
                                const QVariant& data) {
    insertItem(index, text, QIcon(), data);
}

void FluentComboBox::insertItem(int index, const QString& text,
                                const QIcon& icon, const QVariant& data) {
    FluentComboBoxItem item(text, icon, data);
    insertItem(index, item);
}

void FluentComboBox::insertItem(int index, const FluentComboBoxItem& item) {
    auto* standardItem = new QStandardItem(item.icon, item.text);
    standardItem->setData(item.data, Qt::UserRole);
    standardItem->setEnabled(item.enabled);

    if (item.separator) {
        standardItem->setData(true, Qt::UserRole + 1);  // Mark as separator
    }

    m_model->insertRow(index, standardItem);

    // Adjust current index if needed
    if (m_currentIndex >= index) {
        m_currentIndex++;
    }

    updateModel();
}

void FluentComboBox::removeItem(int index) {
    if (index >= 0 && index < m_model->rowCount()) {
        m_model->removeRow(index);

        // Adjust current index
        if (m_currentIndex == index) {
            m_currentIndex = -1;
        } else if (m_currentIndex > index) {
            m_currentIndex--;
        }

        updateModel();
    }
}

void FluentComboBox::clear() {
    m_model->clear();
    m_currentIndex = -1;
    m_selectedIndexes.clear();
    updateModel();
}

QString FluentComboBox::itemText(int index) const {
    if (index >= 0 && index < m_model->rowCount()) {
        return m_model->item(index)->text();
    }
    return QString();
}

void FluentComboBox::setItemText(int index, const QString& text) {
    if (index >= 0 && index < m_model->rowCount()) {
        m_model->item(index)->setText(text);
        updateModel();
    }
}

QIcon FluentComboBox::itemIcon(int index) const {
    if (index >= 0 && index < m_model->rowCount()) {
        return m_model->item(index)->icon();
    }
    return QIcon();
}

void FluentComboBox::setItemIcon(int index, const QIcon& icon) {
    if (index >= 0 && index < m_model->rowCount()) {
        m_model->item(index)->setIcon(icon);
        updateModel();
    }
}

QVariant FluentComboBox::itemData(int index) const {
    if (index >= 0 && index < m_model->rowCount()) {
        return m_model->item(index)->data(Qt::UserRole);
    }
    return QVariant();
}

void FluentComboBox::setItemData(int index, const QVariant& data) {
    if (index >= 0 && index < m_model->rowCount()) {
        m_model->item(index)->setData(data, Qt::UserRole);
        updateModel();
    }
}

bool FluentComboBox::isItemEnabled(int index) const {
    if (index >= 0 && index < m_model->rowCount()) {
        return m_model->item(index)->isEnabled();
    }
    return false;
}

void FluentComboBox::setItemEnabled(int index, bool enabled) {
    if (index >= 0 && index < m_model->rowCount()) {
        m_model->item(index)->setEnabled(enabled);
        updateModel();
    }
}

void FluentComboBox::addSeparator() {
    FluentComboBoxItem separator;
    separator.separator = true;
    addItem(separator);
}

void FluentComboBox::insertSeparator(int index) {
    FluentComboBoxItem separator;
    separator.separator = true;
    insertItem(index, separator);
}

// Model support
QAbstractItemModel* FluentComboBox::model() const { return m_model; }

void FluentComboBox::setModel(QAbstractItemModel* model) {
    if (m_model != model) {
        if (m_model && m_model->parent() == this) {
            m_model->deleteLater();
        }

        m_model = qobject_cast<QStandardItemModel*>(model);
        if (!m_model) {
            m_model = new QStandardItemModel(this);
        }

        updateModel();
    }
}

// Dropdown configuration
int FluentComboBox::maxVisibleItems() const { return m_maxVisibleItems; }

void FluentComboBox::setMaxVisibleItems(int count) {
    if (m_maxVisibleItems != count) {
        m_maxVisibleItems = count;
        updateDropdownSize();
        emit maxVisibleItemsChanged(count);
    }
}

FluentComboBoxDropDirection FluentComboBox::dropDirection() const {
    return m_dropDirection;
}

void FluentComboBox::setDropDirection(FluentComboBoxDropDirection direction) {
    if (m_dropDirection != direction) {
        m_dropDirection = direction;
        emit dropDirectionChanged(direction);
    }
}

bool FluentComboBox::isAnimated() const { return m_animated; }

void FluentComboBox::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

// Search functionality
QString FluentComboBox::searchText() const { return m_searchText; }

void FluentComboBox::setSearchText(const QString& text) {
    if (m_searchText != text) {
        m_searchText = text;
        if (m_searchEdit) {
            m_searchEdit->setText(text);
        }
        m_searchTimer->start();
        emit searchTextChanged(text);
    }
}

void FluentComboBox::clearSearch() { setSearchText(QString()); }

bool FluentComboBox::isCaseSensitiveSearch() const {
    return m_caseSensitiveSearch;
}

void FluentComboBox::setCaseSensitiveSearch(bool sensitive) {
    if (m_caseSensitiveSearch != sensitive) {
        m_caseSensitiveSearch = sensitive;
        if (!m_searchText.isEmpty()) {
            filterItems();
        }
    }
}

// Utility methods
int FluentComboBox::findText(const QString& text, Qt::MatchFlags flags) const {
    for (int i = 0; i < m_model->rowCount(); ++i) {
        QString itemText = m_model->item(i)->text();

        if (flags & Qt::MatchExactly) {
            if (itemText == text)
                return i;
        } else if (flags & Qt::MatchStartsWith) {
            if (itemText.startsWith(text, flags & Qt::MatchCaseSensitive
                                              ? Qt::CaseSensitive
                                              : Qt::CaseInsensitive)) {
                return i;
            }
        } else if (flags & Qt::MatchContains) {
            if (itemText.contains(text, flags & Qt::MatchCaseSensitive
                                            ? Qt::CaseSensitive
                                            : Qt::CaseInsensitive)) {
                return i;
            }
        }
    }
    return -1;
}

int FluentComboBox::findData(const QVariant& data) const {
    for (int i = 0; i < m_model->rowCount(); ++i) {
        if (m_model->item(i)->data(Qt::UserRole) == data) {
            return i;
        }
    }
    return -1;
}

bool FluentComboBox::isDropdownVisible() const { return m_dropdownVisible; }

void FluentComboBox::showDropdown() {
    if (!m_dropdownVisible && m_model->rowCount() > 0) {
        m_dropdownVisible = true;
        positionDropdown();

        if (m_animated) {
            animateDropdown(true);
        } else {
            m_dropdown->show();
        }
    }
}

void FluentComboBox::hideDropdown() {
    if (m_dropdownVisible) {
        m_dropdownVisible = false;

        if (m_animated) {
            animateDropdown(false);
        } else {
            m_dropdown->hide();
        }
    }
}

// Size hints
QSize FluentComboBox::sizeHint() const {
    QFontMetrics fm(font());
    int maxWidth = 0;

    // Calculate width based on items
    for (int i = 0; i < m_model->rowCount(); ++i) {
        QString text = m_model->item(i)->text();
        maxWidth = qMax(maxWidth, fm.horizontalAdvance(text));
    }

    // Add padding and arrow space
    maxWidth += 60;  // 20px padding + 40px arrow area

    return QSize(qMax(maxWidth, 120), 32);
}

QSize FluentComboBox::minimumSizeHint() const { return QSize(80, 32); }

// Public slots
void FluentComboBox::showPopup() { showDropdown(); }

void FluentComboBox::hidePopup() { hideDropdown(); }

// Protected methods - Event handling
void FluentComboBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto& theme = Styling::FluentTheme::instance();

    // Draw background
    QRect bgRect = rect().adjusted(1, 1, -1, -1);
    QColor bgColor = theme.color("controlFillColorDefault");

    if (!isEnabled()) {
        bgColor = theme.color("controlFillColorDisabled");
    } else if (m_dropdownVisible) {
        bgColor = theme.color("controlFillColorInputActive");
    } else if (m_hovered) {
        bgColor = theme.color("controlFillColorSecondary");
    }

    painter.setBrush(bgColor);
    painter.setPen(QPen(theme.color("controlStrokeColorDefault"), 1));
    painter.drawRoundedRect(bgRect, 4, 4);

    // Draw content area
    QRect contentRect = bgRect.adjusted(12, 0, -32, 0);

    // Draw current text or placeholder
    painter.setPen(theme.color("textFillColorPrimary"));
    painter.setFont(font());

    QString displayText = currentText();
    if (displayText.isEmpty() && !m_placeholderText.isEmpty()) {
        painter.setPen(theme.color("textFillColorSecondary"));
        displayText = m_placeholderText;
    }

    if (m_multiSelectEnabled && m_selectedIndexes.size() > 1) {
        displayText =
            QString("%1 items selected").arg(m_selectedIndexes.size());
    }

    painter.drawText(contentRect, Qt::AlignLeft | Qt::AlignVCenter,
                     painter.fontMetrics().elidedText(
                         displayText, Qt::ElideRight, contentRect.width()));

    // Draw dropdown arrow
    QRect arrowRect(width() - 28, (height() - 16) / 2, 16, 16);
    painter.setPen(QPen(theme.color("textFillColorSecondary"), 2));
    painter.setBrush(Qt::NoBrush);

    // Draw chevron down
    QPolygon arrow;
    arrow << QPoint(arrowRect.left() + 4, arrowRect.top() + 6)
          << QPoint(arrowRect.center().x(), arrowRect.bottom() - 6)
          << QPoint(arrowRect.right() - 4, arrowRect.top() + 6);

    painter.drawPolyline(arrow);
}

void FluentComboBox::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_dropdownVisible) {
            hideDropdown();
        } else {
            showDropdown();
        }
        event->accept();
    } else {
        Core::FluentComponent::mousePressEvent(event);
    }
}

void FluentComboBox::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (!m_dropdownVisible) {
                showDropdown();
            }
            event->accept();
            break;

        case Qt::Key_Escape:
            if (m_dropdownVisible) {
                hideDropdown();
            }
            event->accept();
            break;

        case Qt::Key_Up:
            if (m_currentIndex > 0) {
                setCurrentIndex(m_currentIndex - 1);
            }
            event->accept();
            break;

        case Qt::Key_Down:
            if (m_currentIndex < count() - 1) {
                setCurrentIndex(m_currentIndex + 1);
            }
            event->accept();
            break;

        default:
            Core::FluentComponent::keyPressEvent(event);
            break;
    }
}

void FluentComboBox::wheelEvent(QWheelEvent* event) {
    if (!m_dropdownVisible) {
        int delta = event->angleDelta().y();
        if (delta > 0 && m_currentIndex > 0) {
            setCurrentIndex(m_currentIndex - 1);
        } else if (delta < 0 && m_currentIndex < count() - 1) {
            setCurrentIndex(m_currentIndex + 1);
        }
        event->accept();
    } else {
        Core::FluentComponent::wheelEvent(event);
    }
}

void FluentComboBox::focusInEvent(QFocusEvent* event) {
    Core::FluentComponent::focusInEvent(event);
    update();
}

void FluentComboBox::focusOutEvent(QFocusEvent* event) {
    Core::FluentComponent::focusOutEvent(event);
    if (m_dropdownVisible) {
        hideDropdown();
    }
    update();
}

void FluentComboBox::enterEvent(QEnterEvent* event) {
    m_hovered = true;
    update();
    Core::FluentComponent::enterEvent(event);
}

void FluentComboBox::leaveEvent(QEvent* event) {
    m_hovered = false;
    update();
    Core::FluentComponent::leaveEvent(event);
}

void FluentComboBox::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
    updateLayout();
}

// Private slots
void FluentComboBox::onDropdownItemClicked(const QModelIndex& index) {
    if (!index.isValid())
        return;

    int sourceIndex = index.row();

    if (m_multiSelectEnabled) {
        if (m_selectedIndexes.contains(sourceIndex)) {
            m_selectedIndexes.removeAll(sourceIndex);
        } else {
            m_selectedIndexes.append(sourceIndex);
        }
        updateSelection();
        emit selectionChanged();
    } else {
        setCurrentIndex(sourceIndex);
        hideDropdown();
        emit activated(sourceIndex);
    }
}

void FluentComboBox::onSearchTextChanged(const QString& text) {
    setSearchText(text);
}

void FluentComboBox::onDropdownAnimationFinished() {
    if (!m_dropdownVisible) {
        m_dropdown->hide();
    }
}

// Private utility methods
void FluentComboBox::updateColors() {
    update();
    if (m_dropdown) {
        m_dropdown->update();
    }
}

void FluentComboBox::updateLayout() {
    if (m_searchEdit) {
        QRect editRect = rect().adjusted(12, 4, -32, -4);
        m_searchEdit->setGeometry(editRect);
    }
}

void FluentComboBox::updateModel() {
    filterItems();
    updateDropdownSize();
}

void FluentComboBox::updateCurrentItem() {
    // Update display and emit signals as needed
    update();
}

void FluentComboBox::updateSelection() {
    if (m_listView) {
        QItemSelectionModel* selectionModel = m_listView->selectionModel();
        selectionModel->clear();

        for (int index : m_selectedIndexes) {
            QModelIndex modelIndex = m_filteredModel->index(index, 0);
            if (modelIndex.isValid()) {
                selectionModel->select(modelIndex, QItemSelectionModel::Select);
            }
        }
    }
    update();
}

void FluentComboBox::filterItems() {
    m_filteredModel->clear();

    for (int i = 0; i < m_model->rowCount(); ++i) {
        QStandardItem* item = m_model->item(i);
        bool matches = true;

        if (!m_searchText.isEmpty()) {
            Qt::CaseSensitivity cs =
                m_caseSensitiveSearch ? Qt::CaseSensitive : Qt::CaseInsensitive;
            matches = item->text().contains(m_searchText, cs);
        }

        if (matches) {
            auto* filteredItem = new QStandardItem(item->text());
            filteredItem->setIcon(item->icon());
            filteredItem->setData(item->data(Qt::UserRole), Qt::UserRole);
            filteredItem->setEnabled(item->isEnabled());
            m_filteredModel->appendRow(filteredItem);
        }
    }

    updateDropdownSize();
}

void FluentComboBox::positionDropdown() {
    if (!m_dropdown)
        return;

    QPoint globalPos = mapToGlobal(QPoint(0, 0));
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();

    int dropdownHeight = calculateDropdownHeight();
    QRect dropdownRect(globalPos.x(), globalPos.y() + height(), width(),
                       dropdownHeight);

    // Check if dropdown fits below
    if (m_dropDirection == FluentComboBoxDropDirection::Auto) {
        if (dropdownRect.bottom() > screenGeometry.bottom()) {
            // Position above if there's more space
            if (globalPos.y() - dropdownHeight > screenGeometry.top()) {
                dropdownRect.moveTop(globalPos.y() - dropdownHeight);
            }
        }
    } else if (m_dropDirection == FluentComboBoxDropDirection::Up) {
        dropdownRect.moveTop(globalPos.y() - dropdownHeight);
    }

    // Ensure dropdown stays within screen bounds
    if (dropdownRect.right() > screenGeometry.right()) {
        dropdownRect.moveRight(screenGeometry.right());
    }
    if (dropdownRect.left() < screenGeometry.left()) {
        dropdownRect.moveLeft(screenGeometry.left());
    }

    m_dropdown->setGeometry(dropdownRect);
}

void FluentComboBox::animateDropdown(bool show) {
    if (!m_dropdownAnimation)
        return;

    QRect finalGeometry = m_dropdown->geometry();

    if (show) {
        QRect startGeometry = finalGeometry;
        startGeometry.setHeight(0);
        startGeometry.moveTop(finalGeometry.top() + finalGeometry.height() / 2);

        m_dropdown->setGeometry(startGeometry);
        m_dropdown->show();

        m_dropdownAnimation->setStartValue(startGeometry);
        m_dropdownAnimation->setEndValue(finalGeometry);
    } else {
        QRect endGeometry = finalGeometry;
        endGeometry.setHeight(0);
        endGeometry.moveTop(finalGeometry.top() + finalGeometry.height() / 2);

        m_dropdownAnimation->setStartValue(finalGeometry);
        m_dropdownAnimation->setEndValue(endGeometry);
    }

    m_dropdownAnimation->start();
}

void FluentComboBox::updateDropdownSize() {
    if (!m_dropdown || !m_listView)
        return;

    int itemHeight = 32;
    int visibleItems = qMin(m_maxVisibleItems, m_filteredModel->rowCount());
    int dropdownHeight = visibleItems * itemHeight + 16;  // 16px for padding

    m_dropdown->setFixedHeight(dropdownHeight);
    m_listView->setFixedHeight(dropdownHeight - 16);
}

int FluentComboBox::calculateDropdownHeight() const {
    int itemHeight = 32;
    int visibleItems = qMin(m_maxVisibleItems, m_filteredModel->rowCount());
    return visibleItems * itemHeight + 16;  // 16px for padding
}

}  // namespace FluentQt::Components
