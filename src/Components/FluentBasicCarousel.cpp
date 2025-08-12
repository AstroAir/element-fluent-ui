// src/Components/FluentBasicCarousel.cpp
#include "FluentQt/Components/FluentBasicCarousel.h"
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QVBoxLayout>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentBasicCarousel::FluentBasicCarousel(QWidget* parent)
    : FluentCarousel(parent) {
    initializeBasicCarousel();
}

FluentBasicCarousel::FluentBasicCarousel(const FluentCarouselConfig& config,
                                         QWidget* parent)
    : FluentCarousel(config, parent) {
    initializeBasicCarousel();
}

void FluentBasicCarousel::initializeBasicCarousel() {
    // Set default configuration for basic carousel
    FluentCarouselConfig config = this->config();
    config.navigationStyle = FluentCarouselNavigationStyle::Arrows;
    config.showNavigation = true;
    config.showIndicators = false;
    config.autoPlay = FluentCarouselAutoPlay::None;
    setConfig(config);

    createNavigationButtons();
    setupKeyboardNavigation();

    // Connect signals
    connect(this, &FluentCarousel::currentIndexChanged, this,
            &FluentBasicCarousel::onCurrentIndexChanged);
    connect(this, &FluentCarousel::itemCountChanged, this,
            &FluentBasicCarousel::onItemCountChanged);

    updateNavigationLayout();
    updateButtonStates();
}

void FluentBasicCarousel::createNavigationButtons() {
    // Create navigation container
    m_navigationContainer = new QWidget(this);
    m_navigationContainer->setObjectName("FluentBasicCarousel_Navigation");

    // Create navigation layout
    m_navigationLayout = new QHBoxLayout(m_navigationContainer);
    m_navigationLayout->setContentsMargins(8, 8, 8, 8);
    m_navigationLayout->setSpacing(8);

    // Create previous button
    m_previousButton = new FluentButton(m_navigationContainer);
    m_previousButton->setObjectName("FluentBasicCarousel_PreviousButton");
    m_previousButton->setText(m_previousButtonText);
    m_previousButton->setButtonStyle(m_navigationButtonStyle);
    m_previousButton->setIcon(QIcon(":/icons/chevron-left"));
    m_previousButton->setAccessibleName("Previous item");
    m_previousButton->setAccessibleDescription(
        "Navigate to the previous carousel item");

    // Create next button
    m_nextButton = new FluentButton(m_navigationContainer);
    m_nextButton->setObjectName("FluentBasicCarousel_NextButton");
    m_nextButton->setText(m_nextButtonText);
    m_nextButton->setButtonStyle(m_navigationButtonStyle);
    m_nextButton->setIcon(QIcon(":/icons/chevron-right"));
    m_nextButton->setAccessibleName("Next item");
    m_nextButton->setAccessibleDescription(
        "Navigate to the next carousel item");

    // Add buttons to layout
    m_navigationLayout->addWidget(m_previousButton);
    m_navigationLayout->addStretch();
    m_navigationLayout->addWidget(m_nextButton);

    // Connect button signals
    connect(m_previousButton, &FluentButton::clicked, this,
            &FluentBasicCarousel::onPreviousButtonClicked);
    connect(m_nextButton, &FluentButton::clicked, this,
            &FluentBasicCarousel::onNextButtonClicked);

    // Add navigation container to main layout
    if (auto* mainLayout = qobject_cast<QHBoxLayout*>(layout())) {
        mainLayout->addWidget(m_navigationContainer);
    }
}

void FluentBasicCarousel::updateNavigationLayout() {
    if (!m_navigationContainer)
        return;

    // Position navigation container based on orientation
    const FluentCarouselConfig& config = this->config();

    if (config.orientation == FluentCarouselOrientation::Vertical) {
        // For vertical carousel, place buttons on the sides
        m_navigationLayout->setDirection(QBoxLayout::LeftToRight);
        m_navigationContainer->setFixedHeight(60);
        m_navigationContainer->setMaximumWidth(QWIDGETSIZE_MAX);
    } else {
        // For horizontal carousel, place buttons below
        m_navigationLayout->setDirection(QBoxLayout::LeftToRight);
        m_navigationContainer->setFixedHeight(60);
        m_navigationContainer->setMaximumWidth(QWIDGETSIZE_MAX);
    }

    // Update button icons based on orientation
    if (config.orientation == FluentCarouselOrientation::Vertical) {
        m_previousButton->setIcon(QIcon(":/icons/chevron-up"));
        m_nextButton->setIcon(QIcon(":/icons/chevron-down"));
    } else {
        m_previousButton->setIcon(QIcon(":/icons/chevron-left"));
        m_nextButton->setIcon(QIcon(":/icons/chevron-right"));
    }

    updateButtonStates();
}

void FluentBasicCarousel::updateButtonStates() {
    if (!m_previousButton || !m_nextButton)
        return;

    bool canGoPrev = canGoToPrevious();
    bool canGoNext = canGoToNext();

    m_previousButton->setEnabled(canGoPrev && m_navigationButtonsEnabled);
    m_nextButton->setEnabled(canGoNext && m_navigationButtonsEnabled);

    // Update accessibility
    m_previousButton->setAccessibleDescription(
        canGoPrev ? "Navigate to the previous carousel item"
                  : "No previous item available");
    m_nextButton->setAccessibleDescription(
        canGoNext ? "Navigate to the next carousel item"
                  : "No next item available");
}

void FluentBasicCarousel::updateButtonStyles() {
    if (!m_previousButton || !m_nextButton)
        return;

    m_previousButton->setButtonStyle(m_navigationButtonStyle);
    m_nextButton->setButtonStyle(m_navigationButtonStyle);
}

void FluentBasicCarousel::setupKeyboardNavigation() {
    setFocusPolicy(Qt::StrongFocus);
    setTabOrder(m_previousButton, m_nextButton);
}

void FluentBasicCarousel::updateAccessibilityInfo() {
    FluentCarousel::updateAccessibilityInfo();

    // Update button accessibility
    updateButtonStates();
}

void FluentBasicCarousel::setShowNavigationButtons(bool show) {
    if (m_showNavigationButtons != show) {
        m_showNavigationButtons = show;

        if (m_navigationContainer) {
            m_navigationContainer->setVisible(show);
        }

        emit navigationButtonsVisibilityChanged(show);
    }
}

void FluentBasicCarousel::setPreviousButtonText(const QString& text) {
    if (m_previousButtonText != text) {
        m_previousButtonText = text;

        if (m_previousButton) {
            m_previousButton->setText(text);
        }

        emit previousButtonTextChanged(text);
    }
}

void FluentBasicCarousel::setNextButtonText(const QString& text) {
    if (m_nextButtonText != text) {
        m_nextButtonText = text;

        if (m_nextButton) {
            m_nextButton->setText(text);
        }

        emit nextButtonTextChanged(text);
    }
}

void FluentBasicCarousel::setNavigationButtonStyle(FluentButtonStyle style) {
    if (m_navigationButtonStyle != style) {
        m_navigationButtonStyle = style;
        updateButtonStyles();
        emit navigationButtonStyleChanged(style);
    }
}

void FluentBasicCarousel::setNavigationButtonsEnabled(bool enabled) {
    if (m_navigationButtonsEnabled != enabled) {
        m_navigationButtonsEnabled = enabled;
        updateButtonStates();
    }
}

void FluentBasicCarousel::updateNavigationButtonStates() {
    updateButtonStates();
}

// Factory methods
FluentBasicCarousel* FluentBasicCarousel::createHorizontal(QWidget* parent) {
    FluentCarouselConfig config;
    config.orientation = FluentCarouselOrientation::Horizontal;
    config.navigationStyle = FluentCarouselNavigationStyle::Arrows;
    return new FluentBasicCarousel(config, parent);
}

FluentBasicCarousel* FluentBasicCarousel::createVertical(QWidget* parent) {
    FluentCarouselConfig config;
    config.orientation = FluentCarouselOrientation::Vertical;
    config.navigationStyle = FluentCarouselNavigationStyle::Arrows;
    return new FluentBasicCarousel(config, parent);
}

FluentBasicCarousel* FluentBasicCarousel::createMinimal(QWidget* parent) {
    FluentCarouselConfig config;
    config.navigationStyle = FluentCarouselNavigationStyle::Arrows;
    config.showIndicators = false;
    config.enableKeyboard = true;
    config.contentMargins = QMargins(4, 4, 4, 4);
    return new FluentBasicCarousel(config, parent);
}

// Event handling
void FluentBasicCarousel::keyPressEvent(QKeyEvent* event) {
    bool handled = true;

    switch (event->key()) {
        case Qt::Key_Tab:
            // Handle tab navigation between buttons
            if (event->modifiers() & Qt::ShiftModifier) {
                if (m_nextButton && m_nextButton->hasFocus()) {
                    m_previousButton->setFocus();
                } else {
                    setFocus();
                }
            } else {
                if (m_previousButton && m_previousButton->hasFocus()) {
                    m_nextButton->setFocus();
                } else if (hasFocus()) {
                    m_previousButton->setFocus();
                } else {
                    handled = false;
                }
            }
            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Activate focused button
            if (m_previousButton && m_previousButton->hasFocus()) {
                m_previousButton->animateClick();
            } else if (m_nextButton && m_nextButton->hasFocus()) {
                m_nextButton->animateClick();
            } else {
                handled = false;
            }
            break;

        default:
            handled = false;
            break;
    }

    if (!handled) {
        FluentCarousel::keyPressEvent(event);
    }
}

void FluentBasicCarousel::focusInEvent(QFocusEvent* event) {
    FluentCarousel::focusInEvent(event);

    // If no button has focus, focus the previous button
    if (!m_previousButton->hasFocus() && !m_nextButton->hasFocus()) {
        m_previousButton->setFocus();
    }
}

void FluentBasicCarousel::focusOutEvent(QFocusEvent* event) {
    FluentCarousel::focusOutEvent(event);
}

void FluentBasicCarousel::resizeEvent(QResizeEvent* event) {
    FluentCarousel::resizeEvent(event);
    updateNavigationLayout();
}

void FluentBasicCarousel::changeEvent(QEvent* event) {
    FluentCarousel::changeEvent(event);

    if (event->type() == QEvent::EnabledChange) {
        updateButtonStates();
    }
}

// Private slots
void FluentBasicCarousel::onPreviousButtonClicked() {
    goToPrevious();
    emit navigationButtonClicked(-1);
}

void FluentBasicCarousel::onNextButtonClicked() {
    goToNext();
    emit navigationButtonClicked(1);
}

void FluentBasicCarousel::onCurrentIndexChanged(int index) {
    Q_UNUSED(index)
    updateButtonStates();
}

void FluentBasicCarousel::onItemCountChanged(int count) {
    Q_UNUSED(count)
    updateButtonStates();
}

// FluentCarouselNavigation implementation
FluentCarouselNavigation::FluentCarouselNavigation(QWidget* parent)
    : QWidget(parent), m_layout(new QHBoxLayout(this)) {
    initializeNavigation();
}

void FluentCarouselNavigation::initializeNavigation() {
    m_previousButton = new FluentButton(this);
    m_nextButton = new FluentButton(this);

    m_previousButton->setText("Previous");
    m_nextButton->setText("Next");

    m_layout->addWidget(m_previousButton);
    m_layout->addWidget(m_nextButton);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_previousButton, &FluentButton::clicked, this,
            &FluentCarouselNavigation::previousClicked);
    connect(m_nextButton, &FluentButton::clicked, this,
            &FluentCarouselNavigation::nextClicked);
}

void FluentCarouselNavigation::setPreviousButtonText(const QString& text) {
    if (m_previousButton) {
        m_previousButton->setText(text);
    }
}

void FluentCarouselNavigation::setNextButtonText(const QString& text) {
    if (m_nextButton) {
        m_nextButton->setText(text);
    }
}

void FluentCarouselNavigation::setButtonStyle(FluentButtonStyle style) {
    if (m_previousButton) {
        m_previousButton->setButtonStyle(style);
    }
    if (m_nextButton) {
        m_nextButton->setButtonStyle(style);
    }
}

void FluentCarouselNavigation::setButtonsEnabled(bool enabled) {
    if (m_previousButton) {
        m_previousButton->setEnabled(enabled);
    }
    if (m_nextButton) {
        m_nextButton->setEnabled(enabled);
    }
}

void FluentCarouselNavigation::updateButtonStates(bool canGoPrevious,
                                                  bool canGoNext) {
    if (m_previousButton) {
        m_previousButton->setEnabled(canGoPrevious);
    }
    if (m_nextButton) {
        m_nextButton->setEnabled(canGoNext);
    }
}

void FluentCarouselNavigation::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    QWidget::paintEvent(event);
}

void FluentCarouselNavigation::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateLayout();
}

void FluentCarouselNavigation::updateLayout() {
    if (m_layout) {
        m_layout->setGeometry(rect());
    }
}

}  // namespace FluentQt::Components
