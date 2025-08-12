#pragma once

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>
#include <memory>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Core {

/**
 * @brief Error boundary component for handling loading failures and component
 * errors
 *
 * Inspired by ElaWidgetTools' error handling patterns, this component provides
 * graceful error handling with fallback UI and recovery mechanisms.
 */
class FluentErrorBoundary : public FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage
                   NOTIFY errorMessageChanged)
    Q_PROPERTY(bool hasError READ hasError NOTIFY hasErrorChanged)
    Q_PROPERTY(bool showRetryButton READ showRetryButton WRITE
                   setShowRetryButton NOTIFY showRetryButtonChanged)

public:
    enum class ErrorType {
        LoadingTimeout,
        NetworkError,
        ComponentError,
        ValidationError,
        UnknownError
    };
    Q_ENUM(ErrorType)

    explicit FluentErrorBoundary(QWidget* parent = nullptr);
    ~FluentErrorBoundary() override;

    // Error state management
    QString errorMessage() const { return m_errorMessage; }
    void setErrorMessage(const QString& message);

    bool hasError() const { return m_hasError; }
    void setHasError(bool hasError);

    bool showRetryButton() const { return m_showRetryButton; }
    void setShowRetryButton(bool show);

    // Content management
    void setContent(QWidget* content);
    QWidget* content() const { return m_content; }

    // Error handling
    void catchError(const QString& message,
                    ErrorType type = ErrorType::UnknownError);
    void clearError();
    void setRetryCallback(std::function<void()> callback);

    // ElaWidgetTools-inspired timeout handling
    void setLoadingTimeout(int timeoutMs);
    void startLoadingTimeout();
    void stopLoadingTimeout();

signals:
    void errorMessageChanged(const QString& message);
    void hasErrorChanged(bool hasError);
    void showRetryButtonChanged(bool show);
    void errorOccurred(const QString& message, ErrorType type);
    void retryRequested();
    void errorCleared();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onLoadingTimeout();
    void onRetryClicked();

private:
    void setupUI();
    void updateLayout();
    void showErrorUI();
    void showContentUI();
    void createErrorWidget();

    // State
    QString m_errorMessage;
    bool m_hasError{false};
    bool m_showRetryButton{true};
    ErrorType m_currentErrorType{ErrorType::UnknownError};

    // Content
    QWidget* m_content{nullptr};
    QWidget* m_errorWidget{nullptr};
    QVBoxLayout* m_layout{nullptr};

    // Error UI components
    QLabel* m_errorIcon{nullptr};
    QLabel* m_errorLabel{nullptr};
    QPushButton* m_retryButton{nullptr};

    // Timeout handling
    QTimer* m_loadingTimer{nullptr};
    int m_loadingTimeoutMs{30000};  // 30 seconds default

    // Retry callback
    std::function<void()> m_retryCallback;
};

/**
 * @brief Loading state manager with error boundary integration
 *
 * Provides comprehensive loading state management with timeout handling,
 * error boundaries, and fallback mechanisms.
 */
class FluentLoadingStateManager : public QObject {
    Q_OBJECT

public:
    enum class LoadingState { Idle, Loading, Success, Error, Timeout };
    Q_ENUM(LoadingState)

    explicit FluentLoadingStateManager(QObject* parent = nullptr);
    ~FluentLoadingStateManager() override;

    // State management
    LoadingState currentState() const { return m_currentState; }
    void setState(LoadingState state);

    // Timeout configuration
    void setDefaultTimeout(int timeoutMs) { m_defaultTimeoutMs = timeoutMs; }
    int defaultTimeout() const { return m_defaultTimeoutMs; }

    // Error boundary integration
    void setErrorBoundary(FluentErrorBoundary* boundary);
    FluentErrorBoundary* errorBoundary() const { return m_errorBoundary; }

    // Loading operations
    void startLoading(int timeoutMs = -1);
    void finishLoading();
    void failLoading(const QString& errorMessage,
                     FluentErrorBoundary::ErrorType type =
                         FluentErrorBoundary::ErrorType::UnknownError);

signals:
    void stateChanged(LoadingState state);
    void loadingStarted();
    void loadingFinished();
    void loadingFailed(const QString& message,
                       FluentErrorBoundary::ErrorType type);
    void loadingTimeout();

private slots:
    void onLoadingTimeout();

private:
    LoadingState m_currentState{LoadingState::Idle};
    FluentErrorBoundary* m_errorBoundary{nullptr};
    QTimer* m_timeoutTimer{nullptr};
    int m_defaultTimeoutMs{30000};
};

}  // namespace FluentQt::Core
