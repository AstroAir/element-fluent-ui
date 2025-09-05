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

    // Initialize search timer for debounced search
    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(300);  // 300ms debounce
    connect(m_searchTimer, &QTimer::timeout, this,
            [this]() { filterItems(m_currentSearchText); });

    setupUI();
    setupDropdown();
    setupAnimations();
    setupShadowEffect();
    setupAccessibility();
    setupSearchFunctionality();

    // Apply Fluent Design tokens
    applyFluentDesignTokens();

    updateColors();
    updateFonts();
    updateComponentSize();
    updateComponentStyle();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentDropdown::onThemeChanged);
}

FluentDropdown::~FluentDropdown() = default;

void FluentDropdown::setupUI() {
    FLUENT_PROFILE("FluentDropdown::setupUI");

    const auto& theme = Styling::FluentTheme::instance();

    // Create main layout with theme-based padding
    m_mainLayout = new QHBoxLayout(this);
    const auto padding = getComponentPadding();
    m_mainLayout->setContentsMargins(padding);
    m_mainLayout->setSpacing(theme.spacing("m"));

    // Create display label
    m_displayLabel = new QLabel(this);
    m_displayLabel->setObjectName("FluentDropdown_DisplayLabel");
    m_displayLabel->setText(m_placeholderText);
    m_displayLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_mainLayout->addWidget(m_displayLabel, 1);

    // Create error label (initially hidden)
    m_errorLabel = new QLabel(this);
    m_errorLabel->setObjectName("FluentDropdown_ErrorLabel");
    m_errorLabel->setVisible(false);
    m_errorLabel->setStyleSheet("color: " +
                                theme.currentPalette().error.name());

    // Create loading label (initially hidden)
    m_loadingLabel = new QLabel(this);
    m_loadingLabel->setObjectName("FluentDropdown_LoadingLabel");
    m_loadingLabel->setText("Loading...");
    m_loadingLabel->setVisible(false);

    // Set component height based on size
    updateComponentSize();
}

void FluentDropdown::setupDropdown() {
    FLUENT_PROFILE("FluentDropdown::setupDropdown");

    const auto& theme = Styling::FluentTheme::instance();

    // Create dropdown container with proper styling
    m_dropdownContainer = new QWidget(this);
    m_dropdownContainer->setObjectName("FluentDropdown_Container");
    m_dropdownContainer->setWindowFlags(Qt::Popup);
    m_dropdownContainer->hide();

    // Create dropdown layout with theme-based margins
    auto* dropdownLayout = new QVBoxLayout(m_dropdownContainer);
    dropdownLayout->setContentsMargins(theme.spacing("xs"), theme.spacing("xs"),
                                       theme.spacing("xs"),
                                       theme.spacing("xs"));
    dropdownLayout->setSpacing(0);

    // Add search edit if search is enabled
    if (m_searchEnabled) {
        m_searchEdit = new QLineEdit(m_dropdownContainer);
        m_searchEdit->setObjectName("FluentDropdown_SearchEdit");
        m_searchEdit->setPlaceholderText(m_searchPlaceholder);
        m_searchEdit->setVisible(m_searchEnabled);
        dropdownLayout->addWidget(m_searchEdit);

        // Connect search functionality
        connect(m_searchEdit, &QLineEdit::textChanged, this,
                [this](const QString& text) {
                    m_currentSearchText = text;
                    m_searchTimer->start();  // Debounced search
                    emit searchTextChanged(text);
                });
    }

    // Create list widget with enhanced styling
    m_listWidget = new QListWidget(m_dropdownContainer);
    m_listWidget->setObjectName("FluentDropdown_List");
    m_listWidget->setFrameStyle(QFrame::NoFrame);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Apply theme-based styling to list widget
    const int itemHeight = theme.componentHeight("small");
    m_listWidget->setStyleSheet(
        QString("QListWidget::item { "
                "  height: %1px; "
                "  padding: %2px %3px; "
                "  border: none; "
                "} "
                "QListWidget::item:hover { "
                "  background-color: %4; "
                "} "
                "QListWidget::item:selected { "
                "  background-color: %5; "
                "}")
            .arg(itemHeight)
            .arg(theme.spacing("s"))
            .arg(theme.spacing("m"))
            .arg(theme.currentPalette().hover.name())
            .arg(theme.currentPalette().selected.name()));

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

    const auto& theme = Styling::FluentTheme::instance();
    const QFontMetrics fm(m_displayLabel ? m_displayLabel->font()
                                         : theme.bodyFont());
    int maxWidth = fm.horizontalAdvance(m_placeholderText);

    // Find the widest item text
    for (const auto& item : m_items) {
        if (!item.separator && !item.isGroupHeader) {
            const int itemWidth = fm.horizontalAdvance(item.text);
            maxWidth = std::max(maxWidth, itemWidth);
        }
    }

    // Add padding and chevron space using theme tokens
    const auto padding = getComponentPadding();
    const int chevronSpace =
        theme.iconSize("medium").width() + theme.spacing("m");
    const int totalWidth =
        maxWidth + padding.left() + padding.right() + chevronSpace;

    const int height = getComponentHeight();

    m_cachedSizeHint = QSize(totalWidth, height);
    m_sizeHintValid = true;

    return m_cachedSizeHint;
}

QSize FluentDropdown::minimumSizeHint() const {
    const auto& theme = Styling::FluentTheme::instance();
    const int minWidth = theme.componentWidth("small");
    const int height = getComponentHeight();
    return QSize(minWidth, height);
}

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

    // Handle error state first
    if (m_hasError) {
        backgroundColor = palette.errorLight;
    } else if (m_isLoading) {
        backgroundColor = palette.neutralQuaternaryAlt;
    } else {
        // Apply style-specific background colors
        switch (m_style) {
            case FluentDropdownStyle::Outline:
                switch (state()) {
                    case Core::FluentState::Normal:
                        backgroundColor = palette.surface;
                        break;
                    case Core::FluentState::Hovered:
                        backgroundColor = palette.neutralLighter;
                        break;
                    case Core::FluentState::Pressed:
                        backgroundColor = palette.neutralLight;
                        break;
                    case Core::FluentState::Disabled:
                        backgroundColor = palette.neutralLighter;
                        break;
                    default:
                        backgroundColor = palette.surface;
                }
                break;

            case FluentDropdownStyle::Filled:
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
                        backgroundColor = palette.neutralQuaternaryAlt;
                        break;
                    default:
                        backgroundColor = palette.neutralLighter;
                }
                break;

            case FluentDropdownStyle::Underlined:
                // Underlined style has transparent background
                backgroundColor = Qt::transparent;
                break;
        }
    }

    // Create path with rounded corners (except for underlined style)
    QPainterPath path;
    const int radius =
        (m_style == FluentDropdownStyle::Underlined) ? 0 : getBorderRadius();
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, backgroundColor);
}

void FluentDropdown::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    QColor borderColor;
    int borderWidth = 1;

    // Handle error state first
    if (m_hasError) {
        borderColor = palette.error;
        borderWidth = 2;  // Thicker border for error state
    } else {
        // Apply style and state-specific border colors
        switch (state()) {
            case Core::FluentState::Normal:
                borderColor = (m_style == FluentDropdownStyle::Filled)
                                  ? Qt::transparent
                                  : palette.neutralSecondary;
                break;
            case Core::FluentState::Hovered:
                borderColor = palette.neutralPrimary;
                break;
            case Core::FluentState::Pressed:
            case Core::FluentState::Focused:
                borderColor = palette.accent;
                borderWidth = 2;  // Thicker border for focus
                break;
            case Core::FluentState::Disabled:
                borderColor = palette.neutralTertiaryAlt;
                break;
            default:
                borderColor = palette.neutralSecondary;
        }
    }

    // Handle different styles
    switch (m_style) {
        case FluentDropdownStyle::Outline:
        case FluentDropdownStyle::Filled: {
            if (borderColor != Qt::transparent) {
                QPen borderPen(borderColor, borderWidth);
                painter.setPen(borderPen);

                const int radius = getBorderRadius();
                const QRectF borderRect =
                    QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

                painter.drawRoundedRect(borderRect, radius, radius);
            }
            break;
        }

        case FluentDropdownStyle::Underlined: {
            // Draw only bottom border for underlined style
            QPen borderPen(borderColor, borderWidth);
            painter.setPen(borderPen);

            const int y = rect.bottom() - borderWidth / 2;
            painter.drawLine(rect.left(), y, rect.right(), y);
            break;
        }
    }

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

// FluentDropdownItem factory methods
FluentDropdownItem FluentDropdownItem::createSeparator() {
    FluentDropdownItem item;
    item.separator = true;
    item.text = "";
    item.enabled = false;
    return item;
}

FluentDropdownItem FluentDropdownItem::createGroupHeader(const QString& title) {
    FluentDropdownItem item;
    item.text = title;
    item.isGroupHeader = true;
    item.enabled = false;
    return item;
}

// Enhanced FluentDropdown methods
FluentDropdownSize FluentDropdown::size() const { return m_size; }

void FluentDropdown::setSize(FluentDropdownSize size) {
    if (m_size != size) {
        m_size = size;
        updateComponentSize();
        m_sizeHintValid = false;
        updateGeometry();
        emit sizeChanged(size);
    }
}

FluentDropdownStyle FluentDropdown::style() const { return m_style; }

void FluentDropdown::setStyle(FluentDropdownStyle style) {
    if (m_style != style) {
        m_style = style;
        updateComponentStyle();
        update();
        emit styleChanged(style);
    }
}

bool FluentDropdown::hasError() const { return m_hasError; }

void FluentDropdown::setHasError(bool hasError) {
    if (m_hasError != hasError) {
        m_hasError = hasError;
        updateErrorState();
        updateAccessibility();
        emit errorChanged(hasError);
    }
}

QString FluentDropdown::errorMessage() const { return m_errorMessage; }

void FluentDropdown::setErrorMessage(const QString& message) {
    if (m_errorMessage != message) {
        m_errorMessage = message;
        updateErrorState();
        updateAccessibility();
        emit errorMessageChanged(message);
    }
}

bool FluentDropdown::isLoading() const { return m_isLoading; }

void FluentDropdown::setLoading(bool loading) {
    if (m_isLoading != loading) {
        m_isLoading = loading;
        updateLoadingState();
        updateAccessibility();
        emit loadingChanged(loading);
    }
}

bool FluentDropdown::isSearchEnabled() const { return m_searchEnabled; }

void FluentDropdown::setSearchEnabled(bool enabled) {
    if (m_searchEnabled != enabled) {
        m_searchEnabled = enabled;
        setupSearchFunctionality();
        emit searchEnabledChanged(enabled);
    }
}

QString FluentDropdown::searchPlaceholder() const {
    return m_searchPlaceholder;
}

void FluentDropdown::setSearchPlaceholder(const QString& placeholder) {
    if (m_searchPlaceholder != placeholder) {
        m_searchPlaceholder = placeholder;
        if (m_searchEdit) {
            m_searchEdit->setPlaceholderText(placeholder);
        }
        emit searchPlaceholderChanged(placeholder);
    }
}

bool FluentDropdown::isGroupingEnabled() const { return m_groupingEnabled; }

void FluentDropdown::setGroupingEnabled(bool enabled) {
    if (m_groupingEnabled != enabled) {
        m_groupingEnabled = enabled;
        // Refresh the list to apply grouping
        // TODO: Implement grouping logic
        emit groupingEnabledChanged(enabled);
    }
}

QString FluentDropdown::accessibleName() const { return m_accessibleName; }

void FluentDropdown::setAccessibleName(const QString& name) {
    if (m_accessibleName != name) {
        m_accessibleName = name;
        updateAccessibility();
    }
}

QString FluentDropdown::accessibleDescription() const {
    return m_accessibleDescription;
}

void FluentDropdown::setAccessibleDescription(const QString& description) {
    if (m_accessibleDescription != description) {
        m_accessibleDescription = description;
        updateAccessibility();
    }
}

// Enhanced private methods implementation
void FluentDropdown::setupAccessibility() {
    // Set up comprehensive accessibility support
    setAccessibleName(m_accessibleName.isEmpty() ? "Dropdown"
                                                 : m_accessibleName);
    setAccessibleDescription(m_accessibleDescription.isEmpty()
                                 ? "Select an item from the dropdown list"
                                 : m_accessibleDescription);

    // Set ARIA properties
    setProperty("role", "combobox");
    setProperty("aria-haspopup", "listbox");
    setProperty("aria-expanded", m_dropdownVisible);
    setProperty("aria-required", false);
    setProperty("aria-invalid", m_hasError);

    // Enable keyboard navigation
    setFocusPolicy(Qt::StrongFocus);
    setTabOrder(this, m_listWidget);
}

void FluentDropdown::updateAccessibility() {
    // Update accessibility properties based on current state
    setProperty("aria-expanded", m_dropdownVisible);
    setProperty("aria-invalid", m_hasError);

    if (m_hasError && !m_errorMessage.isEmpty()) {
        setProperty("aria-describedby", "error-message");
        if (m_errorLabel) {
            m_errorLabel->setAccessibleName("Error: " + m_errorMessage);
        }
    }

    if (m_isLoading) {
        setProperty("aria-busy", true);
        setAccessibleDescription("Loading items...");
    } else {
        setProperty("aria-busy", false);
        setAccessibleDescription(m_accessibleDescription.isEmpty()
                                     ? "Select an item from the dropdown list"
                                     : m_accessibleDescription);
    }

    // Update current selection accessibility
    if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
        const QString currentText = m_items[m_currentIndex].text;
        setProperty("aria-activedescendant", currentText);
        setAccessibleName(QString("Dropdown, %1 selected").arg(currentText));
    }

    emit accessibilityChanged();
}

void FluentDropdown::setupShadowEffect() {
    const auto& theme = Styling::FluentTheme::instance();

    // Create shadow effect for dropdown elevation
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(getElevation());
    m_shadowEffect->setColor(theme.currentPalette().shadowMedium);
    m_shadowEffect->setOffset(0, 2);

    if (m_dropdownContainer) {
        m_dropdownContainer->setGraphicsEffect(m_shadowEffect);
    }
}

void FluentDropdown::updateShadowEffect() {
    if (m_shadowEffect) {
        const auto& theme = Styling::FluentTheme::instance();
        m_shadowEffect->setBlurRadius(getElevation());
        m_shadowEffect->setColor(theme.currentPalette().shadowMedium);
    }
}

void FluentDropdown::setupSearchFunctionality() {
    if (m_searchEnabled && m_dropdownContainer && !m_searchEdit) {
        // Create search edit if it doesn't exist
        m_searchEdit = new QLineEdit(m_dropdownContainer);
        m_searchEdit->setObjectName("FluentDropdown_SearchEdit");
        m_searchEdit->setPlaceholderText(m_searchPlaceholder);

        // Insert at the beginning of the dropdown layout
        auto* layout =
            qobject_cast<QVBoxLayout*>(m_dropdownContainer->layout());
        if (layout) {
            layout->insertWidget(0, m_searchEdit);
        }

        // Connect search functionality
        connect(m_searchEdit, &QLineEdit::textChanged, this,
                [this](const QString& text) {
                    m_currentSearchText = text;
                    m_searchTimer->start();  // Debounced search
                    emit searchTextChanged(text);
                });
    } else if (!m_searchEnabled && m_searchEdit) {
        // Hide search edit if search is disabled
        m_searchEdit->setVisible(false);
    }

    if (m_searchEdit) {
        m_searchEdit->setVisible(m_searchEnabled);
    }
}

void FluentDropdown::updateComponentSize() {
    const int height = getComponentHeight();
    setMinimumHeight(height);
    setFixedHeight(height);

    // Update font size based on component size
    const auto& theme = Styling::FluentTheme::instance();
    QFont font;
    switch (m_size) {
        case FluentDropdownSize::Small:
            font = theme.bodySmallFont();
            break;
        case FluentDropdownSize::Medium:
            font = theme.bodyFont();
            break;
        case FluentDropdownSize::Large:
            font = theme.bodyLargeFont();
            break;
    }

    if (m_displayLabel) {
        m_displayLabel->setFont(font);
    }
}

void FluentDropdown::updateComponentStyle() {
    // Update visual style based on m_style
    // This will be handled in paintBackground method
    update();
}

void FluentDropdown::updateErrorState() {
    if (m_errorLabel) {
        m_errorLabel->setVisible(m_hasError && !m_errorMessage.isEmpty());
        m_errorLabel->setText(m_errorMessage);

        const auto& theme = Styling::FluentTheme::instance();
        m_errorLabel->setStyleSheet("color: " +
                                    theme.currentPalette().error.name());
    }

    // Update border color and styling will be handled in paintBorder
    update();
}

void FluentDropdown::updateLoadingState() {
    if (m_loadingLabel) {
        m_loadingLabel->setVisible(m_isLoading);
    }

    // Disable interaction during loading
    setEnabled(!m_isLoading);

    if (m_listWidget) {
        m_listWidget->setEnabled(!m_isLoading);
    }
}

void FluentDropdown::filterItems(const QString& searchText) {
    if (!m_listWidget)
        return;

    int visibleCount = 0;
    const bool hasSearch = !searchText.isEmpty();

    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (!item)
            continue;

        bool visible = true;
        if (hasSearch) {
            const QString itemText = item->text();
            visible = itemText.contains(searchText, Qt::CaseInsensitive);
        }

        item->setHidden(!visible);
        if (visible) {
            visibleCount++;
        }
    }

    emit itemsFiltered(visibleCount);
    updateDropdownSize();
}

void FluentDropdown::applyFluentDesignTokens() {
    const auto& theme = Styling::FluentTheme::instance();

    // Apply corner radius
    setCornerRadius(getBorderRadius());

    // Update cached size hint
    m_sizeHintValid = false;
}

int FluentDropdown::getComponentHeight() const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (m_size) {
        case FluentDropdownSize::Small:
            return theme.componentHeight("small");  // 24px
        case FluentDropdownSize::Medium:
            return theme.componentHeight("medium");  // 32px
        case FluentDropdownSize::Large:
            return theme.componentHeight("large");  // 40px
        default:
            return theme.componentHeight("medium");
    }
}

QMargins FluentDropdown::getComponentPadding() const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (m_size) {
        case FluentDropdownSize::Small:
            return theme.padding("small");  // 4px all around
        case FluentDropdownSize::Medium:
            return theme.padding("medium");  // 8px all around
        case FluentDropdownSize::Large:
            return theme.padding("large");  // 12px all around
        default:
            return theme.padding("medium");
    }
}

int FluentDropdown::getBorderRadius() const {
    const auto& theme = Styling::FluentTheme::instance();
    return theme.borderRadius("input");  // 4px for input components
}

int FluentDropdown::getElevation() const {
    const auto& theme = Styling::FluentTheme::instance();
    return theme.elevation("dropdown");  // 8px shadow blur for dropdowns
}

}  // namespace FluentQt::Components
