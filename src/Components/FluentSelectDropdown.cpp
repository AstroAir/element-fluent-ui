// src/Components/FluentSelectDropdown.cpp
#include "FluentQt/Components/FluentSelectDropdown.h"
#include "FluentQt/Components/FluentSelect.h"
#include "FluentQt/Components/FluentSelectItem.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QDebug>
#include <QFocusEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QScreen>
#include <QScrollBar>
#include <QShowEvent>

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

FluentSelectDropdown::FluentSelectDropdown(FluentSelect* parent)
    : QWidget(parent), m_select(parent) {
    setupUI();
    setupAnimations();
    setupConnections();

    // Connect to theme changes
    connect(&FluentTheme::instance(), &FluentTheme::themeChanged, this,
            &FluentSelectDropdown::onThemeChanged);

    updateColors();

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setFocusPolicy(Qt::StrongFocus);

    // Install event filter on application to handle clicks outside
    qApp->installEventFilter(this);
}

FluentSelectDropdown::~FluentSelectDropdown() { qApp->removeEventFilter(this); }

void FluentSelectDropdown::showAnimated() {
    if (m_state == FluentDropdownState::Visible ||
        m_state == FluentDropdownState::Showing) {
        return;
    }

    emit aboutToShow();
    m_state = FluentDropdownState::Showing;

    updateGeometry();
    updatePosition();

    startShowAnimation();
}

void FluentSelectDropdown::hideAnimated() {
    if (m_state == FluentDropdownState::Hidden ||
        m_state == FluentDropdownState::Hiding) {
        return;
    }

    emit aboutToHide();
    m_state = FluentDropdownState::Hiding;

    startHideAnimation();
}

void FluentSelectDropdown::setVisible(bool visible) {
    if (visible) {
        showAnimated();
    } else {
        hideAnimated();
    }
}

void FluentSelectDropdown::updateGeometry() {
    if (!m_select)
        return;

    const QRect geometry = calculateGeometry();

    m_targetGeometry = geometry;
    m_targetPosition = geometry.topLeft();

    setGeometry(geometry);
}

void FluentSelectDropdown::updatePosition() {
    if (!m_select)
        return;

    const QPoint position = getOptimalPosition();
    m_targetPosition = position;

    move(position);
}

QSize FluentSelectDropdown::calculateSize() const {
    if (!m_select) {
        return QSize(200, 100);
    }

    const int width = qMax(m_select->width(), m_minWidth);
    const int height = qMin(visibleItemCount() * itemHeight() + 8, m_maxHeight);

    return QSize(width, height);
}

QRect FluentSelectDropdown::calculateGeometry() const {
    const QSize size = calculateSize();
    const QPoint position = getOptimalPosition();

    return QRect(position, size);
}

void FluentSelectDropdown::setModel(QAbstractItemModel* model) {
    if (m_listView) {
        m_listView->setModel(model);
    }
}

QAbstractItemModel* FluentSelectDropdown::model() const {
    return m_listView ? m_listView->model() : nullptr;
}

void FluentSelectDropdown::setSelectionModel(
    QItemSelectionModel* selectionModel) {
    if (m_listView) {
        m_listView->setSelectionModel(selectionModel);
    }
}

QItemSelectionModel* FluentSelectDropdown::selectionModel() const {
    return m_listView ? m_listView->selectionModel() : nullptr;
}

void FluentSelectDropdown::setSearchVisible(bool visible) {
    if (m_searchVisible != visible) {
        m_searchVisible = visible;

        if (m_searchEdit) {
            m_searchEdit->setVisible(visible);
        }

        updateGeometry();
    }
}

void FluentSelectDropdown::setSearchText(const QString& text) {
    if (m_searchEdit) {
        m_searchEdit->setText(text);
    }
}

QString FluentSelectDropdown::searchText() const {
    return m_searchEdit ? m_searchEdit->text() : QString();
}

void FluentSelectDropdown::focusSearch() {
    if (m_searchEdit && m_searchVisible) {
        m_searchEdit->setFocus();
        m_searchEdit->selectAll();
    }
}

void FluentSelectDropdown::clearSearch() {
    if (m_searchEdit) {
        m_searchEdit->clear();
    }
}

void FluentSelectDropdown::setMaxVisibleItems(int max) {
    m_maxVisibleItems = qMax(1, max);
    updateGeometry();
}

void FluentSelectDropdown::setMaxHeight(int height) {
    m_maxHeight = qMax(50, height);
    updateGeometry();
}

void FluentSelectDropdown::setMinWidth(int width) {
    m_minWidth = qMax(0, width);
    updateGeometry();
}

void FluentSelectDropdown::setMaxWidth(int width) {
    m_maxWidth = qMax(0, width);
    updateGeometry();
}

void FluentSelectDropdown::selectNextItem() {
    if (!m_listView || !m_listView->model())
        return;

    const QModelIndex current = m_listView->currentIndex();
    const int rowCount = m_listView->model()->rowCount();

    if (!current.isValid()) {
        // Select first item
        const QModelIndex first = m_listView->model()->index(0, 0);
        if (first.isValid()) {
            m_listView->setCurrentIndex(first);
        }
    } else {
        // Select next item
        const int nextRow = (current.row() + 1) % rowCount;
        const QModelIndex next = m_listView->model()->index(nextRow, 0);
        if (next.isValid()) {
            m_listView->setCurrentIndex(next);
        }
    }

    ensureCurrentItemVisible();
}

void FluentSelectDropdown::selectPreviousItem() {
    if (!m_listView || !m_listView->model())
        return;

    const QModelIndex current = m_listView->currentIndex();
    const int rowCount = m_listView->model()->rowCount();

    if (!current.isValid()) {
        // Select last item
        const QModelIndex last = m_listView->model()->index(rowCount - 1, 0);
        if (last.isValid()) {
            m_listView->setCurrentIndex(last);
        }
    } else {
        // Select previous item
        const int prevRow = (current.row() - 1 + rowCount) % rowCount;
        const QModelIndex prev = m_listView->model()->index(prevRow, 0);
        if (prev.isValid()) {
            m_listView->setCurrentIndex(prev);
        }
    }

    ensureCurrentItemVisible();
}

void FluentSelectDropdown::selectFirstItem() {
    if (!m_listView || !m_listView->model())
        return;

    const QModelIndex first = m_listView->model()->index(0, 0);
    if (first.isValid()) {
        m_listView->setCurrentIndex(first);
        ensureCurrentItemVisible();
    }
}

void FluentSelectDropdown::selectLastItem() {
    if (!m_listView || !m_listView->model())
        return;

    const int rowCount = m_listView->model()->rowCount();
    const QModelIndex last = m_listView->model()->index(rowCount - 1, 0);
    if (last.isValid()) {
        m_listView->setCurrentIndex(last);
        ensureCurrentItemVisible();
    }
}

void FluentSelectDropdown::activateCurrentItem() {
    if (!m_listView)
        return;

    const QModelIndex current = m_listView->currentIndex();
    if (current.isValid()) {
        emit itemActivated(current);
    }
}

int FluentSelectDropdown::itemHeight() const {
    return m_itemDelegate ? m_itemDelegate->itemHeight() : 32;
}

int FluentSelectDropdown::visibleItemCount() const {
    if (!m_listView || !m_listView->model()) {
        return 0;
    }

    const int totalItems = m_listView->model()->rowCount();
    return qMin(totalItems, m_maxVisibleItems);
}

QModelIndex FluentSelectDropdown::currentIndex() const {
    return m_listView ? m_listView->currentIndex() : QModelIndex();
}

void FluentSelectDropdown::setCurrentIndex(const QModelIndex& index) {
    if (m_listView) {
        m_listView->setCurrentIndex(index);
        ensureCurrentItemVisible();
    }
}

qreal FluentSelectDropdown::opacity() const { return m_opacity; }

void FluentSelectDropdown::setOpacity(qreal opacity) {
    if (m_opacity != opacity) {
        m_opacity = opacity;

        if (m_opacityEffect) {
            m_opacityEffect->setOpacity(opacity);
        }
    }
}

// Protected event handlers
void FluentSelectDropdown::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    paintBorder(painter, rect);
}

void FluentSelectDropdown::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Up:
            selectPreviousItem();
            event->accept();
            break;
        case Qt::Key_Down:
            selectNextItem();
            event->accept();
            break;
        case Qt::Key_Home:
            selectFirstItem();
            event->accept();
            break;
        case Qt::Key_End:
            selectLastItem();
            event->accept();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            activateCurrentItem();
            event->accept();
            break;
        case Qt::Key_Escape:
            hideAnimated();
            event->accept();
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void FluentSelectDropdown::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);

    if (m_searchVisible && m_searchEdit) {
        m_searchEdit->setFocus();
    }
}

void FluentSelectDropdown::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);

    // Hide dropdown when focus is lost (unless focus went to a child widget)
    QWidget* focusWidget = QApplication::focusWidget();
    if (!focusWidget || !isAncestorOf(focusWidget)) {
        hideAnimated();
    }
}

void FluentSelectDropdown::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);

    m_state = FluentDropdownState::Visible;
    emit shown();
}

void FluentSelectDropdown::hideEvent(QHideEvent* event) {
    QWidget::hideEvent(event);

    m_state = FluentDropdownState::Hidden;
    emit hidden();
}

void FluentSelectDropdown::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    updateScrollBars();
}

bool FluentSelectDropdown::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        // Hide dropdown if clicked outside
        if (!isAncestorOf(static_cast<QWidget*>(object)) && object != this) {
            hideAnimated();
        }
    }

    return QWidget::eventFilter(object, event);
}

// Private slots
void FluentSelectDropdown::onItemActivated(const QModelIndex& index) {
    emit itemActivated(index);
}

void FluentSelectDropdown::onItemClicked(const QModelIndex& index) {
    emit itemClicked(index);
}

void FluentSelectDropdown::onSelectionChanged() { emit selectionChanged(); }

void FluentSelectDropdown::onSearchTextChanged(const QString& text) {
    emit searchTextChanged(text);
}

void FluentSelectDropdown::onShowAnimationFinished() {
    m_state = FluentDropdownState::Visible;
    emit shown();
}

void FluentSelectDropdown::onHideAnimationFinished() {
    m_state = FluentDropdownState::Hidden;
    hide();
    emit hidden();
}

void FluentSelectDropdown::onThemeChanged() {
    updateColors();
    updateShadow();
    update();
}

// Private implementation methods
void FluentSelectDropdown::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(0);

    // Search edit
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search...");
    m_searchEdit->setVisible(false);
    m_mainLayout->addWidget(m_searchEdit);

    // List view
    m_listView = new QListView();
    m_listView->setFrameStyle(QFrame::NoFrame);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setMouseTracking(true);
    m_mainLayout->addWidget(m_listView);

    // Item delegate
    m_itemDelegate = new FluentSelectItemDelegate(this);
    m_listView->setItemDelegate(m_itemDelegate);

    // Empty label
    m_emptyLabel = new QLabel("No items available");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setVisible(false);
    m_mainLayout->addWidget(m_emptyLabel);
}

void FluentSelectDropdown::setupAnimations() {
    // Opacity effect
    m_opacityEffect = std::make_unique<QGraphicsOpacityEffect>(this);
    setGraphicsEffect(m_opacityEffect.get());

    // Shadow effect
    m_shadowEffect = std::make_unique<QGraphicsDropShadowEffect>(this);
    updateShadow();

    // Show animation
    m_showAnimation =
        std::make_unique<QPropertyAnimation>(m_opacityEffect.get(), "opacity");
    m_showAnimation->setDuration(200);
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);
    connect(m_showAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentSelectDropdown::onShowAnimationFinished);

    // Hide animation
    m_hideAnimation =
        std::make_unique<QPropertyAnimation>(m_opacityEffect.get(), "opacity");
    m_hideAnimation->setDuration(150);
    m_hideAnimation->setEasingCurve(QEasingCurve::InCubic);
    m_hideAnimation->setStartValue(1.0);
    m_hideAnimation->setEndValue(0.0);
    connect(m_hideAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentSelectDropdown::onHideAnimationFinished);
}

void FluentSelectDropdown::setupConnections() {
    // List view connections
    if (m_listView) {
        connect(m_listView, &QListView::activated, this,
                &FluentSelectDropdown::onItemActivated);
        connect(m_listView, &QListView::clicked, this,
                &FluentSelectDropdown::onItemClicked);

        if (auto* selectionModel = m_listView->selectionModel()) {
            connect(selectionModel, &QItemSelectionModel::selectionChanged,
                    this, &FluentSelectDropdown::onSelectionChanged);
        }
    }

    // Search edit connections
    if (m_searchEdit) {
        connect(m_searchEdit, &QLineEdit::textChanged, this,
                &FluentSelectDropdown::onSearchTextChanged);
    }
}

void FluentSelectDropdown::updateColors() {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    m_backgroundColor = palette.neutralLightest;
    m_borderColor = palette.neutralTertiary;
    m_shadowColor = QColor(0, 0, 0, 60);

    // Apply colors to widgets
    const QString styleSheet =
        QString(
            "QListView { background: %1; border: none; }"
            "QLineEdit { background: %1; border: 1px solid %2; border-radius: "
            "4px; padding: 4px; }"
            "QLabel { color: %3; }")
            .arg(m_backgroundColor.name())
            .arg(m_borderColor.name())
            .arg(palette.neutralSecondary.name());

    if (m_listView)
        m_listView->setStyleSheet(styleSheet);
    if (m_searchEdit)
        m_searchEdit->setStyleSheet(styleSheet);
    if (m_emptyLabel)
        m_emptyLabel->setStyleSheet(styleSheet);
}

void FluentSelectDropdown::updateShadow() {
    if (m_shadowEffect) {
        m_shadowEffect->setBlurRadius(16);
        m_shadowEffect->setOffset(0, 4);
        m_shadowEffect->setColor(m_shadowColor);
    }
}

void FluentSelectDropdown::updateScrollBars() {
    if (!m_listView)
        return;

    // Update scroll bar styling
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    const QString scrollBarStyle = QString(
                                       "QScrollBar:vertical {"
                                       "    background: %1;"
                                       "    width: 12px;"
                                       "    border-radius: 6px;"
                                       "}"
                                       "QScrollBar::handle:vertical {"
                                       "    background: %2;"
                                       "    border-radius: 6px;"
                                       "    min-height: 20px;"
                                       "}"
                                       "QScrollBar::handle:vertical:hover {"
                                       "    background: %3;"
                                       "}")
                                       .arg(palette.neutralLighter.name())
                                       .arg(palette.neutralTertiary.name())
                                       .arg(palette.neutralSecondary.name());

    m_listView->verticalScrollBar()->setStyleSheet(scrollBarStyle);
}

void FluentSelectDropdown::paintBackground(QPainter& painter,
                                           const QRect& rect) {
    const int radius = 8;

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, m_backgroundColor);
}

void FluentSelectDropdown::paintBorder(QPainter& painter, const QRect& rect) {
    const int radius = 8;

    QPen pen(m_borderColor, 1);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath path;
    path.addRoundedRect(rect.adjusted(0, 0, -1, -1), radius, radius);
    painter.drawPath(path);
}

QColor FluentSelectDropdown::getBackgroundColor() const {
    return m_backgroundColor;
}

QColor FluentSelectDropdown::getBorderColor() const { return m_borderColor; }

QColor FluentSelectDropdown::getShadowColor() const { return m_shadowColor; }

void FluentSelectDropdown::ensureCurrentItemVisible() {
    if (!m_listView)
        return;

    const QModelIndex current = m_listView->currentIndex();
    if (current.isValid()) {
        m_listView->scrollTo(current, QAbstractItemView::EnsureVisible);
    }
}

void FluentSelectDropdown::scrollToItem(const QModelIndex& index) {
    if (m_listView && index.isValid()) {
        m_listView->scrollTo(index, QAbstractItemView::EnsureVisible);
    }
}

QRect FluentSelectDropdown::getDropdownRect() const { return m_targetGeometry; }

QPoint FluentSelectDropdown::getOptimalPosition() const {
    if (!m_select) {
        return QPoint();
    }

    const QRect selectRect = m_select->geometry();
    const QPoint globalPos = m_select->mapToGlobal(selectRect.bottomLeft());
    const QSize dropdownSize = calculateSize();

    // Get screen geometry
    const QScreen* screen = QApplication::screenAt(globalPos);
    const QRect screenRect = screen ? screen->availableGeometry() : QRect();

    QPoint position = globalPos;

    // Check if dropdown fits below the select
    if (globalPos.y() + dropdownSize.height() > screenRect.bottom()) {
        // Try to position above
        const QPoint abovePos = m_select->mapToGlobal(selectRect.topLeft());
        if (abovePos.y() - dropdownSize.height() >= screenRect.top()) {
            position =
                QPoint(abovePos.x(), abovePos.y() - dropdownSize.height());
        }
    }

    // Ensure dropdown stays within screen bounds horizontally
    if (position.x() + dropdownSize.width() > screenRect.right()) {
        position.setX(screenRect.right() - dropdownSize.width());
    }
    if (position.x() < screenRect.left()) {
        position.setX(screenRect.left());
    }

    return position;
}

bool FluentSelectDropdown::shouldDropUp() const {
    if (!m_select)
        return false;

    const QRect selectRect = m_select->geometry();
    const QPoint globalPos = m_select->mapToGlobal(selectRect.bottomLeft());
    const QSize dropdownSize = calculateSize();

    const QScreen* screen = QApplication::screenAt(globalPos);
    const QRect screenRect = screen ? screen->availableGeometry() : QRect();

    return globalPos.y() + dropdownSize.height() > screenRect.bottom();
}

void FluentSelectDropdown::startShowAnimation() {
    if (m_showAnimation) {
        show();
        m_showAnimation->start();
    } else {
        show();
        onShowAnimationFinished();
    }
}

void FluentSelectDropdown::startHideAnimation() {
    if (m_hideAnimation) {
        m_hideAnimation->start();
    } else {
        hide();
        onHideAnimationFinished();
    }
}
