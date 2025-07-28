# FluentLoadingIndicator Component

The `FluentLoadingIndicator` component is an enhanced loading indicator widget that implements Microsoft's Fluent Design principles. It provides smooth animations, multiple visual styles, and comprehensive customization options for showing loading states.

## Overview

FluentLoadingIndicator offers:
- **Multiple indicator styles** (spinner, dots, bars, ring)
- **Smooth animations** with customizable timing
- **Size and color customization** with theme integration
- **Text labels** and progress information
- **Overlay modes** for blocking interactions
- **Theme integration** with automatic styling
- **Accessibility support** with screen reader announcements

## Basic Usage

### Simple Loading Spinner

```cpp
#include "FluentQt/Components/FluentLoadingIndicator.h"

// Create a basic loading indicator
auto* loadingIndicator = new FluentLoadingIndicator(this);
loadingIndicator->setStyle(FluentLoadingStyle::Spinner);
loadingIndicator->setSize(QSize(32, 32));

// Start/stop loading
loadingIndicator->start();
// ... later ...
loadingIndicator->stop();

// Add to layout
layout->addWidget(loadingIndicator);
```

### Loading with Text

```cpp
auto* loadingIndicator = new FluentLoadingIndicator(this);
loadingIndicator->setStyle(FluentLoadingStyle::Dots);
loadingIndicator->setText("Loading...");
loadingIndicator->setTextPosition(FluentLoadingTextPosition::Bottom);

// Update text during loading
loadingIndicator->start();
loadingIndicator->setText("Connecting to server...");
// ... later ...
loadingIndicator->setText("Downloading data...");
```

## Loading Styles

### Spinner Style

```cpp
auto* spinner = new FluentLoadingIndicator(this);
spinner->setStyle(FluentLoadingStyle::Spinner);
spinner->setSize(QSize(24, 24));
spinner->setColor(QColor(0, 120, 215));
spinner->setLineWidth(3);
spinner->setRotationSpeed(1000);  // milliseconds per rotation
```

### Dots Style

```cpp
auto* dots = new FluentLoadingIndicator(this);
dots->setStyle(FluentLoadingStyle::Dots);
dots->setDotCount(5);
dots->setDotSize(8);
dots->setDotSpacing(4);
dots->setAnimationSpeed(500);  // milliseconds per cycle
```

### Bars Style

```cpp
auto* bars = new FluentLoadingIndicator(this);
bars->setStyle(FluentLoadingStyle::Bars);
bars->setBarCount(5);
bars->setBarWidth(4);
bars->setBarHeight(20);
bars->setBarSpacing(2);
bars->setAnimationDelay(100);  // delay between bars
```

### Ring Style

```cpp
auto* ring = new FluentLoadingIndicator(this);
ring->setStyle(FluentLoadingStyle::Ring);
ring->setSize(QSize(40, 40));
ring->setRingWidth(4);
ring->setStartAngle(0);
ring->setSpanAngle(270);  // 3/4 circle
ring->setRotationSpeed(1200);
```

## Customization

### Color and Theming

```cpp
auto* loadingIndicator = new FluentLoadingIndicator(this);

// Use theme accent color (default)
loadingIndicator->setUseAccentColor(true);

// Custom colors
loadingIndicator->setColor(QColor(0, 120, 215));
loadingIndicator->setSecondaryColor(QColor(200, 200, 200));
loadingIndicator->setTextColor(QColor(96, 96, 96));

// Gradient colors
loadingIndicator->setGradientEnabled(true);
loadingIndicator->setGradientStartColor(QColor(0, 120, 215));
loadingIndicator->setGradientEndColor(QColor(0, 180, 255));
```

### Animation Settings

```cpp
auto* loadingIndicator = new FluentLoadingIndicator(this);

// Animation timing
loadingIndicator->setAnimationDuration(1000);
loadingIndicator->setAnimationEasing(FluentEasing::Linear);

// Custom easing for different effects
loadingIndicator->setAnimationEasing(FluentEasing::CubicInOut);
loadingIndicator->setAnimationEasing(FluentEasing::ElasticOut);

// Animation direction
loadingIndicator->setAnimationDirection(FluentAnimationDirection::Clockwise);
loadingIndicator->setAnimationDirection(FluentAnimationDirection::CounterClockwise);
loadingIndicator->setAnimationDirection(FluentAnimationDirection::Alternate);
```

### Text Configuration

```cpp
auto* loadingIndicator = new FluentLoadingIndicator(this);

// Text properties
loadingIndicator->setText("Processing...");
loadingIndicator->setTextFont(QFont("Segoe UI", 9));
loadingIndicator->setTextColor(QColor(96, 96, 96));

// Text positioning
loadingIndicator->setTextPosition(FluentLoadingTextPosition::Bottom);
loadingIndicator->setTextPosition(FluentLoadingTextPosition::Top);
loadingIndicator->setTextPosition(FluentLoadingTextPosition::Right);
loadingIndicator->setTextPosition(FluentLoadingTextPosition::Left);

// Text spacing
loadingIndicator->setTextSpacing(8);
```

## Overlay Mode

### Full Screen Overlay

```cpp
class LoadingOverlay : public QWidget
{
public:
    LoadingOverlay(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    }
    
    void showLoading(const QString& message = "Loading...") {
        m_loadingIndicator->setText(message);
        m_loadingIndicator->start();
        
        if (parentWidget()) {
            resize(parentWidget()->size());
            move(0, 0);
        }
        
        show();
        raise();
    }
    
    void hideLoading() {
        m_loadingIndicator->stop();
        hide();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        
        // Semi-transparent background
        setStyleSheet("background-color: rgba(0, 0, 0, 0.5);");
        
        // Loading indicator
        m_loadingIndicator = new FluentLoadingIndicator;
        m_loadingIndicator->setStyle(FluentLoadingStyle::Spinner);
        m_loadingIndicator->setSize(QSize(48, 48));
        m_loadingIndicator->setColor(QColor(255, 255, 255));
        m_loadingIndicator->setTextColor(QColor(255, 255, 255));
        m_loadingIndicator->setTextPosition(FluentLoadingTextPosition::Bottom);
        
        layout->addWidget(m_loadingIndicator);
    }

private:
    FluentLoadingIndicator* m_loadingIndicator{nullptr};
};

// Usage
auto* overlay = new LoadingOverlay(mainWindow);
overlay->showLoading("Saving document...");
// ... perform operation ...
overlay->hideLoading();
```

### Widget Overlay

```cpp
class WidgetLoadingOverlay : public QWidget
{
public:
    WidgetLoadingOverlay(QWidget* targetWidget) : QWidget(targetWidget), m_targetWidget(targetWidget) {
        setupUI();
        hide();
        
        // Install event filter to handle resize
        targetWidget->installEventFilter(this);
    }
    
    void showLoading(const QString& message = "Loading...") {
        m_loadingIndicator->setText(message);
        m_loadingIndicator->start();
        
        updateGeometry();
        show();
        raise();
    }
    
    void hideLoading() {
        m_loadingIndicator->stop();
        hide();
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (obj == m_targetWidget && event->type() == QEvent::Resize) {
            updateGeometry();
        }
        return QWidget::eventFilter(obj, event);
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        
        // Semi-transparent background
        setStyleSheet("background-color: rgba(255, 255, 255, 0.9);");
        
        // Loading indicator
        m_loadingIndicator = new FluentLoadingIndicator;
        m_loadingIndicator->setStyle(FluentLoadingStyle::Dots);
        m_loadingIndicator->setSize(QSize(32, 32));
        m_loadingIndicator->setTextPosition(FluentLoadingTextPosition::Bottom);
        
        layout->addWidget(m_loadingIndicator);
    }
    
    void updateGeometry() {
        if (m_targetWidget) {
            setGeometry(m_targetWidget->rect());
        }
    }

private:
    FluentLoadingIndicator* m_loadingIndicator{nullptr};
    QWidget* m_targetWidget{nullptr};
};

// Usage
auto* overlay = new WidgetLoadingOverlay(tableWidget);
overlay->showLoading("Loading data...");
// ... load data ...
overlay->hideLoading();
```

## Integration Examples

### File Download Progress

```cpp
class FileDownloadWidget : public QWidget
{
public:
    FileDownloadWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
    }
    
    void startDownload(const QString& url, const QString& filename) {
        m_filename = filename;
        m_downloadButton->setEnabled(false);
        m_cancelButton->setEnabled(true);
        
        // Show loading indicator
        m_loadingIndicator->setText(QString("Downloading %1...").arg(filename));
        m_loadingIndicator->start();
        
        // Start download (pseudo-code)
        m_downloader->download(url, filename);
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // File info
        m_filenameLabel = new QLabel("No file selected");
        m_filenameLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
        
        // Loading indicator
        m_loadingIndicator = new FluentLoadingIndicator;
        m_loadingIndicator->setStyle(FluentLoadingStyle::Bars);
        m_loadingIndicator->setSize(QSize(100, 20));
        m_loadingIndicator->setText("Ready to download");
        m_loadingIndicator->setTextPosition(FluentLoadingTextPosition::Bottom);
        
        // Progress info
        m_progressLabel = new QLabel("0% complete");
        m_speedLabel = new QLabel("0 KB/s");
        
        // Buttons
        auto* buttonLayout = new QHBoxLayout;
        m_downloadButton = new FluentButton("Download");
        m_cancelButton = new FluentButton("Cancel");
        m_cancelButton->setEnabled(false);
        
        buttonLayout->addWidget(m_downloadButton);
        buttonLayout->addWidget(m_cancelButton);
        buttonLayout->addStretch();
        
        layout->addWidget(m_filenameLabel);
        layout->addWidget(m_loadingIndicator);
        layout->addWidget(m_progressLabel);
        layout->addWidget(m_speedLabel);
        layout->addLayout(buttonLayout);
    }
    
    void setupConnections() {
        connect(m_downloadButton, &FluentButton::clicked, [=]() {
            QString filename = QFileDialog::getOpenFileName(this, "Select file to download");
            if (!filename.isEmpty()) {
                startDownload("https://example.com/file", filename);
            }
        });
        
        connect(m_cancelButton, &FluentButton::clicked, [=]() {
            m_downloader->cancel();
            downloadFinished(false);
        });
        
        // Download progress updates
        connect(m_downloader, &FileDownloader::progressChanged, [=](int percentage, qint64 speed) {
            m_progressLabel->setText(QString("%1% complete").arg(percentage));
            m_speedLabel->setText(QString("%1 KB/s").arg(speed / 1024));
            
            // Update loading indicator text
            m_loadingIndicator->setText(QString("Downloading %1... %2%").arg(m_filename).arg(percentage));
        });
        
        connect(m_downloader, &FileDownloader::finished, [=](bool success) {
            downloadFinished(success);
        });
    }
    
    void downloadFinished(bool success) {
        m_loadingIndicator->stop();
        m_downloadButton->setEnabled(true);
        m_cancelButton->setEnabled(false);
        
        if (success) {
            m_loadingIndicator->setText("Download completed");
            m_progressLabel->setText("100% complete");
        } else {
            m_loadingIndicator->setText("Download cancelled");
            m_progressLabel->setText("Cancelled");
        }
    }

private:
    QLabel* m_filenameLabel{nullptr};
    FluentLoadingIndicator* m_loadingIndicator{nullptr};
    QLabel* m_progressLabel{nullptr};
    QLabel* m_speedLabel{nullptr};
    FluentButton* m_downloadButton{nullptr};
    FluentButton* m_cancelButton{nullptr};
    FileDownloader* m_downloader{nullptr};
    QString m_filename;
};
```

### Data Loading States

```cpp
class DataTableWidget : public QWidget
{
public:
    DataTableWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
    }
    
    void loadData() {
        showLoadingState("Loading data...");
        
        // Simulate async data loading
        QTimer::singleShot(2000, [=]() {
            // Simulate data loaded
            hideLoadingState();
            populateTable();
        });
    }
    
    void refreshData() {
        showLoadingState("Refreshing data...");
        
        // Clear existing data
        m_tableWidget->setRowCount(0);
        
        // Load new data
        QTimer::singleShot(1500, [=]() {
            hideLoadingState();
            populateTable();
        });
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Toolbar
        auto* toolbar = new QHBoxLayout;
        m_refreshButton = new FluentButton("Refresh");
        m_loadButton = new FluentButton("Load Data");
        
        toolbar->addWidget(m_refreshButton);
        toolbar->addWidget(m_loadButton);
        toolbar->addStretch();
        
        // Stacked widget for table and loading states
        m_stackedWidget = new QStackedWidget;
        
        // Table widget
        m_tableWidget = new QTableWidget;
        m_tableWidget->setColumnCount(3);
        m_tableWidget->setHorizontalHeaderLabels({"Name", "Value", "Status"});
        
        // Loading widget
        m_loadingWidget = new QWidget;
        auto* loadingLayout = new QVBoxLayout(m_loadingWidget);
        loadingLayout->setAlignment(Qt::AlignCenter);
        
        m_loadingIndicator = new FluentLoadingIndicator;
        m_loadingIndicator->setStyle(FluentLoadingStyle::Spinner);
        m_loadingIndicator->setSize(QSize(48, 48));
        m_loadingIndicator->setText("Loading...");
        m_loadingIndicator->setTextPosition(FluentLoadingTextPosition::Bottom);
        
        loadingLayout->addWidget(m_loadingIndicator);
        
        // Empty state widget
        m_emptyWidget = new QWidget;
        auto* emptyLayout = new QVBoxLayout(m_emptyWidget);
        emptyLayout->setAlignment(Qt::AlignCenter);
        
        auto* emptyIcon = new QLabel;
        emptyIcon->setPixmap(QIcon(":/icons/empty").pixmap(64, 64));
        emptyIcon->setAlignment(Qt::AlignCenter);
        
        auto* emptyLabel = new QLabel("No data available");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #666; font-size: 14px;");
        
        auto* loadDataButton = new FluentButton("Load Data");
        connect(loadDataButton, &FluentButton::clicked, this, &DataTableWidget::loadData);
        
        emptyLayout->addWidget(emptyIcon);
        emptyLayout->addWidget(emptyLabel);
        emptyLayout->addWidget(loadDataButton);
        
        // Add widgets to stack
        m_stackedWidget->addWidget(m_tableWidget);
        m_stackedWidget->addWidget(m_loadingWidget);
        m_stackedWidget->addWidget(m_emptyWidget);
        
        layout->addLayout(toolbar);
        layout->addWidget(m_stackedWidget);
        
        // Show empty state initially
        m_stackedWidget->setCurrentWidget(m_emptyWidget);
    }
    
    void setupConnections() {
        connect(m_refreshButton, &FluentButton::clicked, this, &DataTableWidget::refreshData);
        connect(m_loadButton, &FluentButton::clicked, this, &DataTableWidget::loadData);
    }
    
    void showLoadingState(const QString& message) {
        m_loadingIndicator->setText(message);
        m_loadingIndicator->start();
        m_stackedWidget->setCurrentWidget(m_loadingWidget);
        
        // Disable buttons during loading
        m_refreshButton->setEnabled(false);
        m_loadButton->setEnabled(false);
    }
    
    void hideLoadingState() {
        m_loadingIndicator->stop();
        m_stackedWidget->setCurrentWidget(m_tableWidget);
        
        // Re-enable buttons
        m_refreshButton->setEnabled(true);
        m_loadButton->setEnabled(true);
    }
    
    void populateTable() {
        // Simulate populating table with data
        m_tableWidget->setRowCount(10);
        
        for (int row = 0; row < 10; ++row) {
            m_tableWidget->setItem(row, 0, new QTableWidgetItem(QString("Item %1").arg(row + 1)));
            m_tableWidget->setItem(row, 1, new QTableWidgetItem(QString("Value %1").arg(row + 1)));
            m_tableWidget->setItem(row, 2, new QTableWidgetItem("Active"));
        }
    }

private:
    QStackedWidget* m_stackedWidget{nullptr};
    QTableWidget* m_tableWidget{nullptr};
    QWidget* m_loadingWidget{nullptr};
    QWidget* m_emptyWidget{nullptr};
    FluentLoadingIndicator* m_loadingIndicator{nullptr};
    FluentButton* m_refreshButton{nullptr};
    FluentButton* m_loadButton{nullptr};
};
```

### Login Form with Loading

```cpp
class LoginForm : public QWidget
{
public:
    LoginForm(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        
        // Title
        auto* titleLabel = new QLabel("Sign In");
        titleLabel->setFont(QFont("Segoe UI", 18, QFont::Bold));
        titleLabel->setAlignment(Qt::AlignCenter);
        
        // Form fields
        m_usernameEdit = new FluentTextInput;
        m_usernameEdit->setPlaceholderText("Username");
        
        m_passwordEdit = new FluentTextInput;
        m_passwordEdit->setPlaceholderText("Password");
        m_passwordEdit->setEchoMode(QLineEdit::Password);
        
        // Loading indicator (initially hidden)
        m_loadingIndicator = new FluentLoadingIndicator;
        m_loadingIndicator->setStyle(FluentLoadingStyle::Dots);
        m_loadingIndicator->setSize(QSize(60, 20));
        m_loadingIndicator->setText("Signing in...");
        m_loadingIndicator->setTextPosition(FluentLoadingTextPosition::Right);
        m_loadingIndicator->hide();
        
        // Buttons
        m_loginButton = FluentButton::createPrimaryButton("Sign In");
        m_cancelButton = new FluentButton("Cancel");
        
        auto* buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(m_loginButton);
        buttonLayout->addWidget(m_cancelButton);
        
        layout->addWidget(titleLabel);
        layout->addSpacing(20);
        layout->addWidget(m_usernameEdit);
        layout->addWidget(m_passwordEdit);
        layout->addSpacing(10);
        layout->addWidget(m_loadingIndicator);
        layout->addSpacing(10);
        layout->addLayout(buttonLayout);
    }
    
    void setupConnections() {
        connect(m_loginButton, &FluentButton::clicked, this, &LoginForm::performLogin);
        connect(m_cancelButton, &FluentButton::clicked, this, &LoginForm::cancelLogin);
    }
    
    void performLogin() {
        QString username = m_usernameEdit->text();
        QString password = m_passwordEdit->text();
        
        if (username.isEmpty() || password.isEmpty()) {
            // Show validation error
            return;
        }
        
        // Show loading state
        m_loadingIndicator->show();
        m_loadingIndicator->start();
        m_loginButton->setEnabled(false);
        m_usernameEdit->setEnabled(false);
        m_passwordEdit->setEnabled(false);
        
        // Simulate login process
        QTimer::singleShot(2000, [=]() {
            // Simulate login result
            bool success = (username == "admin" && password == "password");
            loginCompleted(success);
        });
    }
    
    void cancelLogin() {
        m_loadingIndicator->stop();
        m_loadingIndicator->hide();
        m_loginButton->setEnabled(true);
        m_usernameEdit->setEnabled(true);
        m_passwordEdit->setEnabled(true);
    }
    
    void loginCompleted(bool success) {
        m_loadingIndicator->stop();
        m_loadingIndicator->hide();
        m_loginButton->setEnabled(true);
        m_usernameEdit->setEnabled(true);
        m_passwordEdit->setEnabled(true);
        
        if (success) {
            emit loginSuccessful();
        } else {
            // Show error message
            QMessageBox::warning(this, "Login Failed", "Invalid username or password");
        }
    }

signals:
    void loginSuccessful();

private:
    FluentTextInput* m_usernameEdit{nullptr};
    FluentTextInput* m_passwordEdit{nullptr};
    FluentLoadingIndicator* m_loadingIndicator{nullptr};
    FluentButton* m_loginButton{nullptr};
    FluentButton* m_cancelButton{nullptr};
};
```

## Accessibility

FluentLoadingIndicator provides comprehensive accessibility support:

```cpp
auto* loadingIndicator = new FluentLoadingIndicator(this);

// Accessible properties (automatically set)
loadingIndicator->setAccessibleName("Loading indicator");
loadingIndicator->setAccessibleDescription("Content is loading, please wait");

// Screen reader announcements
loadingIndicator->setAccessibleRole(QAccessible::ProgressBar);

// State announcements
connect(loadingIndicator, &FluentLoadingIndicator::started, [=]() {
    loadingIndicator->announceToScreenReader("Loading started");
});

connect(loadingIndicator, &FluentLoadingIndicator::stopped, [=]() {
    loadingIndicator->announceToScreenReader("Loading completed");
});
```

## Best Practices

### Loading Indicator Design Guidelines
1. **Show immediate feedback** - Start loading indicators quickly
2. **Provide context** - Include descriptive text when possible
3. **Use appropriate styles** - Match the loading type to content
4. **Handle long operations** - Show progress for lengthy tasks
5. **Support cancellation** - Allow users to cancel when appropriate

### Performance Considerations
- Use efficient animation algorithms
- Optimize for smooth 60fps animations
- Cache animation frames when possible
- Implement proper cleanup when stopping

### Accessibility Guidelines
- Provide loading announcements
- Support screen reader notifications
- Use sufficient color contrast
- Include meaningful status text
- Test with assistive technologies

## Signals Reference

```cpp
// State signals
void started();
void stopped();
void textChanged(const QString& text);

// Animation signals
void animationStarted();
void animationStopped();
void animationCycleCompleted();
```

## See Also

- [FluentProgressBar](FluentProgressBar.md) - For determinate progress
- [FluentButton](FluentButton.md) - For loading buttons
- [FluentNotification](FluentNotification.md) - For completion notifications
- [API Reference - FluentLoadingIndicator](../API_REFERENCE.md#fluentloadingindicator) - Complete API documentation
