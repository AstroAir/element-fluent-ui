// src/Core/FluentErrorBoundary.cpp
#include "FluentQt/Core/FluentErrorBoundary.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QStyle>
#include <QVBoxLayout>

namespace FluentQt::Core {

FluentErrorBoundary::FluentErrorBoundary(QWidget* parent)
    : FluentComponent(parent) {
    setObjectName("FluentErrorBoundary");
    setupUI();

    // Initialize loading timeout timer
    m_loadingTimer = new QTimer(this);
    m_loadingTimer->setSingleShot(true);
    connect(m_loadingTimer, &QTimer::timeout, this,
            &FluentErrorBoundary::onLoadingTimeout);
}

FluentErrorBoundary::~FluentErrorBoundary() = default;

void FluentErrorBoundary::setupUI() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    createErrorWidget();
    showContentUI();  // Start with content visible
}

void FluentErrorBoundary::createErrorWidget() {
    m_errorWidget = new QWidget(this);
    m_errorWidget->setObjectName("FluentErrorWidget");
    m_errorWidget->hide();

    auto* errorLayout = new QVBoxLayout(m_errorWidget);
    errorLayout->setAlignment(Qt::AlignCenter);
    errorLayout->setSpacing(16);

    // Error icon
    m_errorIcon = new QLabel(m_errorWidget);
    m_errorIcon->setAlignment(Qt::AlignCenter);
    m_errorIcon->setFixedSize(48, 48);
    m_errorIcon->setStyleSheet("font-size: 32px; color: #d13438;");
    m_errorIcon->setText("⚠");

    // Error message
    m_errorLabel = new QLabel(m_errorWidget);
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setStyleSheet("color: #605e5c; font-size: 14px;");

    // Retry button
    m_retryButton = new QPushButton("Retry", m_errorWidget);
    m_retryButton->setObjectName("FluentRetryButton");
    m_retryButton->setFixedSize(100, 32);
    connect(m_retryButton, &QPushButton::clicked, this,
            &FluentErrorBoundary::onRetryClicked);

    errorLayout->addWidget(m_errorIcon);
    errorLayout->addWidget(m_errorLabel);
    errorLayout->addWidget(m_retryButton, 0, Qt::AlignCenter);

    m_layout->addWidget(m_errorWidget);
}

void FluentErrorBoundary::setErrorMessage(const QString& message) {
    if (m_errorMessage != message) {
        m_errorMessage = message;
        if (m_errorLabel) {
            m_errorLabel->setText(message);
        }
        emit errorMessageChanged(message);
    }
}

void FluentErrorBoundary::setHasError(bool hasError) {
    if (m_hasError != hasError) {
        m_hasError = hasError;

        if (hasError) {
            showErrorUI();
        } else {
            showContentUI();
        }

        emit hasErrorChanged(hasError);
    }
}

void FluentErrorBoundary::setShowRetryButton(bool show) {
    if (m_showRetryButton != show) {
        m_showRetryButton = show;
        if (m_retryButton) {
            m_retryButton->setVisible(show);
        }
        emit showRetryButtonChanged(show);
    }
}

void FluentErrorBoundary::setContent(QWidget* content) {
    if (m_content) {
        m_layout->removeWidget(m_content);
        m_content->setParent(nullptr);
    }

    m_content = content;
    if (m_content) {
        m_content->setParent(this);
        m_layout->insertWidget(0, m_content);

        if (!m_hasError) {
            showContentUI();
        }
    }
}

void FluentErrorBoundary::catchError(const QString& message, ErrorType type) {
    m_currentErrorType = type;
    setErrorMessage(message);
    setHasError(true);

    // Update error icon based on type
    if (m_errorIcon) {
        switch (type) {
            case ErrorType::LoadingTimeout:
                m_errorIcon->setText("⏱");
                break;
            case ErrorType::NetworkError:
                m_errorIcon->setText("🌐");
                break;
            case ErrorType::ComponentError:
                m_errorIcon->setText("⚠");
                break;
            case ErrorType::ValidationError:
                m_errorIcon->setText("❌");
                break;
            default:
                m_errorIcon->setText("⚠");
                break;
        }
    }

    emit errorOccurred(message, type);
}

void FluentErrorBoundary::clearError() {
    setHasError(false);
    setErrorMessage("");
    m_currentErrorType = ErrorType::UnknownError;
    emit errorCleared();
}

void FluentErrorBoundary::setRetryCallback(std::function<void()> callback) {
    m_retryCallback = std::move(callback);
}

void FluentErrorBoundary::setLoadingTimeout(int timeoutMs) {
    m_loadingTimeoutMs = timeoutMs;
}

void FluentErrorBoundary::startLoadingTimeout() {
    if (m_loadingTimeoutMs > 0) {
        m_loadingTimer->start(m_loadingTimeoutMs);
    }
}

void FluentErrorBoundary::stopLoadingTimeout() { m_loadingTimer->stop(); }

void FluentErrorBoundary::showErrorUI() {
    if (m_content) {
        m_content->hide();
    }
    if (m_errorWidget) {
        m_errorWidget->show();
    }
}

void FluentErrorBoundary::showContentUI() {
    if (m_errorWidget) {
        m_errorWidget->hide();
    }
    if (m_content) {
        m_content->show();
    }
}

void FluentErrorBoundary::onLoadingTimeout() {
    catchError("Loading timed out. Please try again.",
               ErrorType::LoadingTimeout);
}

void FluentErrorBoundary::onRetryClicked() {
    clearError();
    emit retryRequested();

    if (m_retryCallback) {
        m_retryCallback();
    }
}

void FluentErrorBoundary::updateLayout() {
    if (m_layout) {
        m_layout->update();
    }
}

void FluentErrorBoundary::paintEvent(QPaintEvent* event) {
    FluentComponent::paintEvent(event);
}

void FluentErrorBoundary::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);
    updateLayout();
}

// FluentLoadingStateManager implementation
FluentLoadingStateManager::FluentLoadingStateManager(QObject* parent)
    : QObject(parent) {
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this,
            &FluentLoadingStateManager::onLoadingTimeout);
}

FluentLoadingStateManager::~FluentLoadingStateManager() = default;

void FluentLoadingStateManager::setState(LoadingState state) {
    if (m_currentState != state) {
        m_currentState = state;
        emit stateChanged(state);
    }
}

void FluentLoadingStateManager::setErrorBoundary(
    FluentErrorBoundary* boundary) {
    m_errorBoundary = boundary;
}

void FluentLoadingStateManager::startLoading(int timeoutMs) {
    setState(LoadingState::Loading);

    int timeout = (timeoutMs > 0) ? timeoutMs : m_defaultTimeoutMs;
    if (timeout > 0) {
        m_timeoutTimer->start(timeout);
    }

    if (m_errorBoundary) {
        m_errorBoundary->clearError();
        m_errorBoundary->startLoadingTimeout();
    }

    emit loadingStarted();
}

void FluentLoadingStateManager::finishLoading() {
    m_timeoutTimer->stop();
    setState(LoadingState::Success);

    if (m_errorBoundary) {
        m_errorBoundary->stopLoadingTimeout();
        m_errorBoundary->clearError();
    }

    emit loadingFinished();
}

void FluentLoadingStateManager::failLoading(
    const QString& errorMessage, FluentErrorBoundary::ErrorType type) {
    m_timeoutTimer->stop();
    setState(LoadingState::Error);

    if (m_errorBoundary) {
        m_errorBoundary->stopLoadingTimeout();
        m_errorBoundary->catchError(errorMessage, type);
    }

    emit loadingFailed(errorMessage, type);
}

void FluentLoadingStateManager::onLoadingTimeout() {
    setState(LoadingState::Timeout);

    if (m_errorBoundary) {
        m_errorBoundary->catchError(
            "Operation timed out",
            FluentErrorBoundary::ErrorType::LoadingTimeout);
    }

    emit loadingTimeout();
}

}  // namespace FluentQt::Core
