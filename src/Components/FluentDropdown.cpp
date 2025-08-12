// src/Components/FluentDropdown.cpp
#include "FluentQt/Components/FluentDropdown.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QScrollBar>
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

namespace FluentQt::Components {

FluentDropdown::FluentDropdown(QWidget* parent)
    : Core::FluentComponent(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setObjectName("FluentDropdown");

    setupUI();
    setupDropdown();
    setupAnimations();
    updateColors();
    updateFonts();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentDropdown::onThemeChanged);
}

FluentDropdown::~FluentDropdown() = default;

void FluentDropdown::setupUI() {
    FLUENT_PROFILE("FluentDropdown::setupUI");

    // Create main layout
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 4, 8, 4);
    m_mainLayout->setSpacing(8);

    // Create display label
    m_displayLabel = new QLabel(this);
    m_displayLabel->setObjectName("FluentDropdown_DisplayLabel");
    m_displayLabel->setText(m_placeholderText);
    m_displayLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_mainLayout->addWidget(m_displayLabel, 1);

    // Set minimum height
    setMinimumHeight(32);
    setFixedHeight(32);
}

void FluentDropdown::setupDropdown() {
    FLUENT_PROFILE("FluentDropdown::setupDropdown");

    // Create dropdown container
    m_dropdownContainer = new QWidget(this);
    m_dropdownContainer->setObjectName("FluentDropdown_Container");
    m_dropdownContainer->setWindowFlags(Qt::Popup);
    m_dropdownContainer->hide();

    // Create dropdown layout
    auto* dropdownLayout = new QVBoxLayout(m_dropdownContainer);
    dropdownLayout->setContentsMargins(1, 1, 1, 1);
    dropdownLayout->setSpacing(0);

    // Create list widget
    m_listWidget = new QListWidget(m_dropdownContainer);
    m_listWidget->setObjectName("FluentDropdown_List");
    m_listWidget->setFrameStyle(QFrame::NoFrame);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    dropdownLayout->addWidget(m_listWidget);

    // Connect list signals
    connect(m_listWidget, &QListWidget::itemClicked,
            [this](QListWidgetItem* item) {
                const int index = m_listWidget->row(item);
                onListItemClicked(index);
            });
}

void FluentDropdown::setupAnimations() {
    FLUENT_PROFILE("FluentDropdown::setupAnimations");

    // Create opacity effect for dropdown
    m_dropdownOpacityEffect = new QGraphicsOpacityEffect(this);
    m_dropdownOpacityEffect->setOpacity(0.0);
    m_dropdownContainer->setGraphicsEffect(m_dropdownOpacityEffect);

    // Create dropdown animation
    m_dropdownAnimation = std::make_unique<QPropertyAnimation>(
        m_dropdownOpacityEffect, "opacity");
    m_dropdownAnimation->setDuration(200);
    m_dropdownAnimation->setEasingCurve(QEasingCurve::OutQuad);

    connect(m_dropdownAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentDropdown::onDropdownAnimationFinished);
}

QString FluentDropdown::currentText() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
        return m_items[m_currentIndex].text;
    }
    return QString();
}

void FluentDropdown::setCurrentText(const QString& text) {
    const int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

int FluentDropdown::currentIndex() const { return m_currentIndex; }

void FluentDropdown::setCurrentIndex(int index) {
    if (index != m_currentIndex && index >= -1 && index < m_items.size()) {
        m_currentIndex = index;
        updateDisplayText();
        m_sizeHintValid = false;
        updateGeometry();

        emit currentIndexChanged(index);
        emit currentTextChanged(currentText());
        emit currentDataChanged(currentData());
        emit itemSelected(index);
    }
}

QVariant FluentDropdown::currentData() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
        return m_items[m_currentIndex].data;
    }
    return QVariant();
}

QString FluentDropdown::placeholderText() const { return m_placeholderText; }

void FluentDropdown::setPlaceholderText(const QString& text) {
    if (m_placeholderText != text) {
        m_placeholderText = text;
        updateDisplayText();
        emit placeholderTextChanged(text);
    }
}

bool FluentDropdown::isEditable() const { return m_editable; }

void FluentDropdown::setEditable(bool editable) {
    if (m_editable != editable) {
        m_editable = editable;
        // TODO: Implement editable functionality
        emit editableChanged(editable);
    }
}

bool FluentDropdown::isDropdownVisible() const { return m_dropdownVisible; }

void FluentDropdown::setDropdownVisible(bool visible) {
    if (m_dropdownVisible != visible) {
        m_dropdownVisible = visible;

        if (visible) {
            startShowDropdownAnimation();
        } else {
            startHideDropdownAnimation();
        }

        emit dropdownVisibilityChanged(visible);
    }
}

int FluentDropdown::maxVisibleItems() const { return m_maxVisibleItems; }

void FluentDropdown::setMaxVisibleItems(int count) {
    if (m_maxVisibleItems != count && count > 0) {
        m_maxVisibleItems = count;
        updateDropdownSize();
        emit maxVisibleItemsChanged(count);
    }
}

void FluentDropdown::addItem(const QString& text, const QVariant& data) {
    addItem(FluentDropdownItem(text, data));
}

void FluentDropdown::addItem(const QIcon& icon, const QString& text,
                             const QVariant& data) {
    addItem(FluentDropdownItem(icon, text, data));
}

void FluentDropdown::addItem(const FluentDropdownItem& item) {
    m_items.append(item);

    // Add to list widget
    auto* listItem = new QListWidgetItem();
    listItem->setText(item.text);
    listItem->setIcon(item.icon);
    listItem->setData(Qt::UserRole, item.data);

    if (item.separator) {
        listItem->setFlags(Qt::NoItemFlags);
        listItem->setSizeHint(QSize(0, 1));
    } else {
        listItem->setFlags(item.enabled
                               ? Qt::ItemIsEnabled | Qt::ItemIsSelectable
                               : Qt::NoItemFlags);
    }

    m_listWidget->addItem(listItem);

    m_sizeHintValid = false;
    updateGeometry();
    updateDropdownSize();
}

void FluentDropdown::addSeparator() {
    FluentDropdownItem separator;
    separator.separator = true;
    addItem(separator);
}

void FluentDropdown::insertItem(int index, const QString& text,
                                const QVariant& data) {
    insertItem(index, FluentDropdownItem(text, data));
}

void FluentDropdown::insertItem(int index, const QIcon& icon,
                                const QString& text, const QVariant& data) {
    insertItem(index, FluentDropdownItem(icon, text, data));
}

void FluentDropdown::insertItem(int index, const FluentDropdownItem& item) {
    if (index < 0 || index > m_items.size()) {
        addItem(item);
        return;
    }

    m_items.insert(index, item);

    // Insert to list widget
    auto* listItem = new QListWidgetItem();
    listItem->setText(item.text);
    listItem->setIcon(item.icon);
    listItem->setData(Qt::UserRole, item.data);

    if (item.separator) {
        listItem->setFlags(Qt::NoItemFlags);
        listItem->setSizeHint(QSize(0, 1));
    } else {
        listItem->setFlags(item.enabled
                               ? Qt::ItemIsEnabled | Qt::ItemIsSelectable
                               : Qt::NoItemFlags);
    }

    m_listWidget->insertItem(index, listItem);

    // Adjust current index if necessary
    if (m_currentIndex >= index) {
        m_currentIndex++;
    }

    m_sizeHintValid = false;
    updateGeometry();
    updateDropdownSize();
}

void FluentDropdown::insertSeparator(int index) {
    FluentDropdownItem separator;
    separator.separator = true;
    insertItem(index, separator);
}

void FluentDropdown::removeItem(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);

        // Remove from list widget
        delete m_listWidget->takeItem(index);

        // Adjust current index
        if (m_currentIndex == index) {
            m_currentIndex = -1;
            updateDisplayText();
        } else if (m_currentIndex > index) {
            m_currentIndex--;
        }

        m_sizeHintValid = false;
        updateGeometry();
        updateDropdownSize();
    }
}

void FluentDropdown::clear() {
    m_items.clear();
    m_listWidget->clear();
    m_currentIndex = -1;
    updateDisplayText();

    m_sizeHintValid = false;
    updateGeometry();
    updateDropdownSize();
}

int FluentDropdown::count() const { return m_items.size(); }

FluentDropdownItem FluentDropdown::itemAt(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items[index];
    }
    return FluentDropdownItem();
}

void FluentDropdown::setItemAt(int index, const FluentDropdownItem& item) {
    if (index >= 0 && index < m_items.size()) {
        m_items[index] = item;

        // Update list widget item
        auto* listItem = m_listWidget->item(index);
        if (listItem) {
            listItem->setText(item.text);
            listItem->setIcon(item.icon);
            listItem->setData(Qt::UserRole, item.data);
            listItem->setFlags(item.enabled
                                   ? Qt::ItemIsEnabled | Qt::ItemIsSelectable
                                   : Qt::NoItemFlags);
        }

        if (index == m_currentIndex) {
            updateDisplayText();
        }

        m_sizeHintValid = false;
        updateGeometry();
    }
}

int FluentDropdown::findText(const QString& text, Qt::MatchFlags flags) const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (flags & Qt::MatchExactly) {
            if (m_items[i].text == text)
                return i;
        } else if (flags & Qt::MatchContains) {
            if (m_items[i].text.contains(text, Qt::CaseInsensitive))
                return i;
        } else if (flags & Qt::MatchStartsWith) {
            if (m_items[i].text.startsWith(text, Qt::CaseInsensitive))
                return i;
        }
    }
    return -1;
}

int FluentDropdown::findData(const QVariant& data) const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].data == data)
            return i;
    }
    return -1;
}

QSize FluentDropdown::sizeHint() const {
    FLUENT_PROFILE("FluentDropdown::sizeHint");

    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }

    const QFontMetrics fm(m_displayLabel->font());
    int maxWidth = fm.horizontalAdvance(m_placeholderText);

    // Find the widest item text
    for (const auto& item : m_items) {
        if (!item.separator) {
            const int itemWidth = fm.horizontalAdvance(item.text);
            maxWidth = std::max(maxWidth, itemWidth);
        }
    }

    // Add padding and chevron space
    const int totalWidth =
        maxWidth + 40;  // 16 padding + 16 chevron + 8 spacing

    m_cachedSizeHint = QSize(totalWidth, 32);
    m_sizeHintValid = true;

    return m_cachedSizeHint;
}

QSize FluentDropdown::minimumSizeHint() const { return QSize(100, 32); }

void FluentDropdown::showDropdown() { setDropdownVisible(true); }

void FluentDropdown::hideDropdown() { setDropdownVisible(false); }

void FluentDropdown::toggleDropdown() {
    setDropdownVisible(!m_dropdownVisible);
}

void FluentDropdown::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentDropdown::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    paintBorder(painter, rect);

    // Paint chevron
    paintChevron(painter, chevronRect());

    // Paint focus ring
    if (hasFocus()) {
        paintFocusRing(painter, rect);
    }
}

void FluentDropdown::paintBackground(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    QColor backgroundColor;
    switch (state()) {
        case Core::FluentState::Normal:
            backgroundColor = palette.neutralLighter;
            break;
        case Core::FluentState::Hovered:
            backgroundColor = palette.neutralLight;
            break;
        case Core::FluentState::Pressed:
            backgroundColor = palette.neutralQuaternaryAlt;
            break;
        case Core::FluentState::Disabled:
            backgroundColor = palette.neutralLighter;
            break;
        default:
            backgroundColor = palette.neutralLighter;
    }

    // Create path with rounded corners
    QPainterPath path;
    const int radius = cornerRadius();
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, backgroundColor);
}

void FluentDropdown::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    QColor borderColor;
    switch (state()) {
        case Core::FluentState::Normal:
            borderColor = palette.neutralSecondary;
            break;
        case Core::FluentState::Hovered:
            borderColor = palette.neutralPrimary;
            break;
        case Core::FluentState::Pressed:
        case Core::FluentState::Focused:
            borderColor = palette.accent;
            break;
        case Core::FluentState::Disabled:
            borderColor = palette.neutralTertiaryAlt;
            break;
        default:
            borderColor = palette.neutralSecondary;
    }

    QPen borderPen(borderColor, 1);
    painter.setPen(borderPen);

    const int radius = cornerRadius();
    const QRectF borderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

    painter.drawRoundedRect(borderRect, radius, radius);

    painter.restore();
}

void FluentDropdown::paintChevron(QPainter& painter, const QRect& rect) {
    painter.save();

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    QPen pen(palette.neutralSecondary, 2);
    painter.setPen(pen);

    const QPointF center = rect.center();
    const qreal size = 4;

    // Rotate based on dropdown state
    painter.translate(center);
    if (m_dropdownVisible) {
        painter.rotate(180);
    }

    // Draw chevron down arrow
    QPainterPath chevron;
    chevron.moveTo(-size, -size / 2);
    chevron.lineTo(0, size / 2);
    chevron.lineTo(size, -size / 2);

    painter.drawPath(chevron);

    painter.restore();
}

void FluentDropdown::paintFocusRing(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    QPen focusPen(palette.accent, 2);
    painter.setPen(focusPen);
    painter.setBrush(Qt::NoBrush);

    const int radius = cornerRadius() + 2;
    const QRect focusRect = rect.adjusted(-2, -2, 2, 2);

    painter.drawRoundedRect(focusRect, radius, radius);

    painter.restore();
}

void FluentDropdown::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        setState(Core::FluentState::Pressed);
        toggleDropdown();
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentDropdown::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (!event->isAutoRepeat()) {
                toggleDropdown();
            }
            event->accept();
            return;

        case Qt::Key_Escape:
            if (m_dropdownVisible) {
                hideDropdown();
                event->accept();
                return;
            }
            break;

        case Qt::Key_Up:
            if (m_dropdownVisible) {
                // Navigate up in dropdown
                const int newIndex = std::max(0, m_currentIndex - 1);
                setCurrentIndex(newIndex);
            } else {
                // Navigate items when closed
                const int newIndex = std::max(0, m_currentIndex - 1);
                setCurrentIndex(newIndex);
            }
            event->accept();
            return;

        case Qt::Key_Down:
            if (m_dropdownVisible) {
                // Navigate down in dropdown
                const int newIndex = std::min(
                    static_cast<int>(m_items.size()) - 1, m_currentIndex + 1);
                setCurrentIndex(newIndex);
            } else {
                // Navigate items when closed
                const int newIndex = std::min(
                    static_cast<int>(m_items.size()) - 1, m_currentIndex + 1);
                setCurrentIndex(newIndex);
            }
            event->accept();
            return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentDropdown::focusInEvent(QFocusEvent* event) {
    setState(Core::FluentState::Focused);
    update();
    Core::FluentComponent::focusInEvent(event);
}

void FluentDropdown::focusOutEvent(QFocusEvent* event) {
    setState(underMouse() ? Core::FluentState::Hovered
                          : Core::FluentState::Normal);

    // Hide dropdown when losing focus (unless clicking on dropdown)
    if (m_dropdownVisible && event->reason() != Qt::PopupFocusReason) {
        hideDropdown();
    }

    update();
    Core::FluentComponent::focusOutEvent(event);
}

void FluentDropdown::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
    m_sizeHintValid = false;
    updateDropdownPosition();
}

void FluentDropdown::wheelEvent(QWheelEvent* event) {
    if (!m_dropdownVisible) {
        // Navigate items with mouse wheel when dropdown is closed
        const int delta = event->angleDelta().y();
        if (delta > 0 && m_currentIndex > 0) {
            setCurrentIndex(m_currentIndex - 1);
        } else if (delta < 0 && m_currentIndex < m_items.size() - 1) {
            setCurrentIndex(m_currentIndex + 1);
        }
        event->accept();
    } else {
        Core::FluentComponent::wheelEvent(event);
    }
}

void FluentDropdown::updateStateStyle() { update(); }

void FluentDropdown::performStateTransition(Core::FluentState from,
                                            Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    update();
}

void FluentDropdown::onListItemClicked(int index) {
    if (index >= 0 && index < m_items.size() && !m_items[index].separator) {
        setCurrentIndex(index);
        hideDropdown();
    }
}

void FluentDropdown::onTextEdited(const QString& text) {
    emit textEdited(text);
}

void FluentDropdown::onThemeChanged() {
    updateColors();
    updateFonts();
    update();
}

void FluentDropdown::onDropdownAnimationFinished() {
    if (!m_dropdownVisible) {
        m_dropdownContainer->hide();
    }
}

void FluentDropdown::updateColors() {
    // Colors are handled in paintEvent
}

void FluentDropdown::updateFonts() {
    const auto& theme = Styling::FluentTheme::instance();
    m_displayLabel->setFont(theme.bodyFont());
}

void FluentDropdown::startShowDropdownAnimation() {
    if (!isAnimated()) {
        m_dropdownOpacityEffect->setOpacity(1.0);
        m_dropdownContainer->show();
        updateDropdownPosition();
        return;
    }

    updateDropdownPosition();
    updateDropdownSize();
    m_dropdownContainer->show();

    m_dropdownAnimation->setStartValue(0.0);
    m_dropdownAnimation->setEndValue(1.0);
    m_dropdownAnimation->start();
}

void FluentDropdown::startHideDropdownAnimation() {
    if (!isAnimated()) {
        m_dropdownOpacityEffect->setOpacity(0.0);
        m_dropdownContainer->hide();
        return;
    }

    m_dropdownAnimation->setStartValue(1.0);
    m_dropdownAnimation->setEndValue(0.0);
    m_dropdownAnimation->start();
}

void FluentDropdown::updateDisplayText() {
    if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
        m_displayLabel->setText(m_items[m_currentIndex].text);
    } else {
        m_displayLabel->setText(m_placeholderText);
    }
}

void FluentDropdown::updateDropdownPosition() {
    if (!m_dropdownContainer)
        return;

    const QPoint globalPos = mapToGlobal(QPoint(0, height()));
    m_dropdownContainer->move(globalPos);
}

void FluentDropdown::updateDropdownSize() {
    if (!m_dropdownContainer)
        return;

    const int dropdownHeight = calculateDropdownHeight();

    m_dropdownContainer->setFixedSize(width(), dropdownHeight);
    m_listWidget->setFixedHeight(dropdownHeight - 2);  // Account for border
}

QRect FluentDropdown::chevronRect() const {
    const int chevronSize = 16;
    const int margin = 8;
    return QRect(width() - margin - chevronSize, (height() - chevronSize) / 2,
                 chevronSize, chevronSize);
}

QRect FluentDropdown::textRect() const {
    const int chevronWidth = 24;
    return rect().adjusted(8, 0, -chevronWidth, 0);
}

int FluentDropdown::calculateDropdownHeight() const {
    const int itemHeight = 32;
    const int visibleItems =
        std::min(m_maxVisibleItems, static_cast<int>(m_items.size()));
    const int borderHeight = 2;
    return (visibleItems * itemHeight) + borderHeight;
}

}  // namespace FluentQt::Components
