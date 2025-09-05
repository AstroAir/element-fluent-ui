# FluentCircularProgress Component

The `FluentCircularProgress` component is a modern circular progress indicator that implements Microsoft's Fluent Design principles. It provides smooth animations, multiple visual styles, and comprehensive accessibility features for displaying progress and loading states.

## Overview

FluentCircularProgress offers:
- **Multiple visual styles** (Ring, Filled, Segmented)
- **Determinate and indeterminate modes** with smooth animations
- **Customizable colors** with theme integration
- **Smooth motion** using Fluent Design motion tokens
- **Accessibility support** with ARIA attributes and screen reader compatibility
- **High DPI support** with crisp rendering
- **Performance optimized** painting and animations

## Basic Usage

### Simple Progress Ring

```cpp
#include "FluentQt/Components/FluentCircularProgress.h"

// Create a basic circular progress
auto* progress = new FluentCircularProgress();
progress->setProgress(0.75);  // 75% complete
progress->setStyle(FluentCircularProgress::ProgressStyle::Ring);

// Add to layout
parentLayout->addWidget(progress);
```

### Indeterminate Progress

```cpp
// Create indeterminate (spinning) progress indicator
auto* loadingProgress = new FluentCircularProgress();
loadingProgress->startIndeterminate();

// Stop indeterminate mode and set specific progress
loadingProgress->stopIndeterminate();
loadingProgress->setProgress(0.5);
```

### Styled Progress Indicators

```cpp
// Ring style (default)
auto* ringProgress = new FluentCircularProgress();
ringProgress->setStyle(FluentCircularProgress::ProgressStyle::Ring);
ringProgress->setStrokeWidth(4);

// Filled style
auto* filledProgress = new FluentCircularProgress();
filledProgress->setStyle(FluentCircularProgress::ProgressStyle::Filled);

// Segmented style
auto* segmentedProgress = new FluentCircularProgress();
segmentedProgress->setStyle(FluentCircularProgress::ProgressStyle::Segmented);
```

## Configuration Examples

### Color Customization

```cpp
auto* progress = new FluentCircularProgress();

// Set custom colors
progress->setActiveColor(QColor("#0078D4"));    // Blue active color
progress->setInactiveColor(QColor("#E1E1E1"));  // Light gray inactive

// Use theme colors (automatic)
progress->setActiveColor(QColor());  // Reset to theme default
```

### Size and Stroke Configuration

```cpp
auto* progress = new FluentCircularProgress();

// Configure stroke width (affects ring thickness)
progress->setStrokeWidth(6);  // Thicker ring

// Size is controlled by widget size
progress->setFixedSize(100, 100);  // 100x100 pixel progress ring

// Minimum size for readability
progress->setMinimumSize(32, 32);
```

### Animation Settings

```cpp
auto* progress = new FluentCircularProgress();

// Configure animations
progress->setAnimated(true);
progress->setAnimationDuration(300);  // 300ms transition

// Disable animations for performance
progress->setAnimated(false);
```

## State Examples

### Progress Updates

```cpp
auto* progress = new FluentCircularProgress();

// Set progress (0.0 to 1.0)
progress->setProgress(0.0);   // 0%
progress->setProgress(0.25);  // 25%
progress->setProgress(0.5);   // 50%
progress->setProgress(1.0);   // 100%

// Animated progress updates
progress->setAnimated(true);
progress->setProgress(0.8);  // Smoothly animates to 80%
```

### Indeterminate States

```cpp
auto* progress = new FluentCircularProgress();

// Start indeterminate animation
progress->startIndeterminate();

// Check if indeterminate
if (progress->isIndeterminate()) {
    qDebug() << "Progress is in indeterminate mode";
}

// Stop and return to determinate mode
progress->stopIndeterminate();
progress->setProgress(0.0);  // Reset to 0%
```

### Reset and Control

```cpp
auto* progress = new FluentCircularProgress();

// Reset to zero
progress->reset();  // Sets progress to 0.0

// Complete progress
progress->setProgress(1.0);

// Pulse effect for attention
progress->startIndeterminate();
QTimer::singleShot(1000, progress, &FluentCircularProgress::stopIndeterminate);
```

## Event Handling Examples

### Progress Change Events

```cpp
auto* progress = new FluentCircularProgress();

// Handle progress changes
connect(progress, &FluentCircularProgress::progressChanged, 
        [=](qreal value) {
    qDebug() << "Progress changed to:" << (value * 100) << "%";
    
    // Update associated label
    progressLabel->setText(QString("%1%").arg(qRound(value * 100)));
});
```

### Indeterminate State Events

```cpp
connect(progress, &FluentCircularProgress::indeterminateStarted, [=]() {
    qDebug() << "Indeterminate animation started";
    statusLabel->setText("Loading...");
});

connect(progress, &FluentCircularProgress::indeterminateStopped, [=]() {
    qDebug() << "Indeterminate animation stopped";
    statusLabel->setText("Ready");
});
```

### Style and Color Events

```cpp
connect(progress, &FluentCircularProgress::styleChanged, 
        [=](FluentCircularProgress::ProgressStyle style) {
    QString styleName;
    switch (style) {
        case FluentCircularProgress::ProgressStyle::Ring: 
            styleName = "Ring"; break;
        case FluentCircularProgress::ProgressStyle::Filled: 
            styleName = "Filled"; break;
        case FluentCircularProgress::ProgressStyle::Segmented: 
            styleName = "Segmented"; break;
    }
    qDebug() << "Style changed to:" << styleName;
});
```

## Advanced Usage

### File Upload Progress

```cpp
class FileUploadWidget : public QWidget {
    Q_OBJECT
    
public:
    FileUploadWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();
    }
    
private slots:
    void startUpload() {
        m_progress->reset();
        m_progress->setAnimated(true);
        
        // Simulate upload progress
        m_uploadTimer->start(100);  // Update every 100ms
    }
    
    void updateProgress() {
        static qreal progress = 0.0;
        progress += 0.02;  // 2% increment
        
        if (progress >= 1.0) {
            progress = 1.0;
            m_uploadTimer->stop();
            m_statusLabel->setText("Upload Complete!");
        }
        
        m_progress->setProgress(progress);
        m_statusLabel->setText(QString("Uploading... %1%")
                              .arg(qRound(progress * 100)));
    }
    
private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        m_progress = new FluentCircularProgress();
        m_progress->setFixedSize(80, 80);
        
        m_statusLabel = new QLabel("Ready to upload");
        m_uploadButton = new QPushButton("Start Upload");
        
        layout->addWidget(m_progress, 0, Qt::AlignCenter);
        layout->addWidget(m_statusLabel);
        layout->addWidget(m_uploadButton);
        
        m_uploadTimer = new QTimer(this);
    }
    
    void connectSignals() {
        connect(m_uploadButton, &QPushButton::clicked, 
                this, &FileUploadWidget::startUpload);
        connect(m_uploadTimer, &QTimer::timeout, 
                this, &FileUploadWidget::updateProgress);
    }
    
    FluentCircularProgress* m_progress;
    QLabel* m_statusLabel;
    QPushButton* m_uploadButton;
    QTimer* m_uploadTimer;
};
```

### Multi-Stage Progress

```cpp
class MultiStageProgress : public QWidget {
    Q_OBJECT
    
public:
    enum Stage { Initializing, Processing, Finalizing, Complete };
    
    MultiStageProgress(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setStage(Initializing);
    }
    
    void setStage(Stage stage) {
        m_currentStage = stage;
        updateDisplay();
    }
    
private:
    void updateDisplay() {
        switch (m_currentStage) {
            case Initializing:
                m_progress->startIndeterminate();
                m_stageLabel->setText("Initializing...");
                break;
            case Processing:
                m_progress->stopIndeterminate();
                m_progress->setProgress(0.0);
                m_stageLabel->setText("Processing...");
                break;
            case Finalizing:
                m_progress->setProgress(0.8);
                m_stageLabel->setText("Finalizing...");
                break;
            case Complete:
                m_progress->setProgress(1.0);
                m_stageLabel->setText("Complete!");
                break;
        }
    }
    
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        m_progress = new FluentCircularProgress();
        m_progress->setFixedSize(60, 60);
        m_progress->setActiveColor(QColor("#107C10"));  // Green
        
        m_stageLabel = new QLabel();
        
        layout->addWidget(m_progress, 0, Qt::AlignCenter);
        layout->addWidget(m_stageLabel, 0, Qt::AlignCenter);
    }
    
    FluentCircularProgress* m_progress;
    QLabel* m_stageLabel;
    Stage m_currentStage;
};
```

### Custom Progress Styles

```cpp
class CustomStyledProgress : public FluentCircularProgress {
    Q_OBJECT
    
public:
    CustomStyledProgress(QWidget* parent = nullptr) 
        : FluentCircularProgress(parent) {
        setupCustomStyle();
    }
    
private:
    void setupCustomStyle() {
        // Gradient colors
        setActiveColor(QColor("#FF6B6B"));    // Red-pink
        setInactiveColor(QColor("#F0F0F0"));  // Light gray
        
        // Thicker stroke for visibility
        setStrokeWidth(8);
        
        // Slower, smoother animations
        setAnimationDuration(500);
        
        // Custom size
        setFixedSize(120, 120);
    }
    
protected:
    void paintEvent(QPaintEvent* event) override {
        // Call base implementation
        FluentCircularProgress::paintEvent(event);
        
        // Add custom overlay (e.g., percentage text)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw percentage in center
        if (!isIndeterminate()) {
            QString percentText = QString("%1%").arg(qRound(progress() * 100));
            QFont font = painter.font();
            font.setPointSize(12);
            font.setBold(true);
            painter.setFont(font);
            painter.setPen(activeColor());
            painter.drawText(rect(), Qt::AlignCenter, percentText);
        }
    }
};
```

## Accessibility Examples

### Screen Reader Support

```cpp
auto* progress = new FluentCircularProgress();

// Set accessible properties
progress->setAccessibleName("File upload progress");
progress->setAccessibleDescription("Shows the progress of file upload operation");

// Update accessibility when progress changes
connect(progress, &FluentCircularProgress::progressChanged, 
        [=](qreal value) {
    QString description = QString("Upload progress: %1 percent complete")
                         .arg(qRound(value * 100));
    progress->setAccessibleDescription(description);
});

// Handle indeterminate state
connect(progress, &FluentCircularProgress::indeterminateStarted, [=]() {
    progress->setAccessibleDescription("Loading, please wait");
});
```

### Keyboard Navigation

```cpp
// Progress indicators typically don't need keyboard focus
// But if needed for custom interactions:
progress->setFocusPolicy(Qt::TabFocus);

// Custom keyboard handling
progress->installEventFilter(this);

bool eventFilter(QObject* obj, QEvent* event) override {
    if (obj == progress && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Space) {
            // Toggle indeterminate mode
            if (progress->isIndeterminate()) {
                progress->stopIndeterminate();
            } else {
                progress->startIndeterminate();
            }
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
```

## Theming Examples

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* progress = new FluentCircularProgress();

// Use theme colors automatically
auto& theme = FluentTheme::instance();
QColor accentColor = theme.color("accent.primary");
progress->setActiveColor(accentColor);

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    QColor newAccentColor = theme.color("accent.primary");
    progress->setActiveColor(newAccentColor);
});
```

### Custom Color Schemes

```cpp
// Success theme (green)
auto* successProgress = new FluentCircularProgress();
successProgress->setActiveColor(QColor("#107C10"));
successProgress->setInactiveColor(QColor("#E1F5E1"));

// Warning theme (amber)
auto* warningProgress = new FluentCircularProgress();
warningProgress->setActiveColor(QColor("#FF8C00"));
warningProgress->setInactiveColor(QColor("#FFF4E6"));

// Error theme (red)
auto* errorProgress = new FluentCircularProgress();
errorProgress->setActiveColor(QColor("#D13438"));
errorProgress->setInactiveColor(QColor("#FDE7E9"));
```

## Real-World Scenarios

### Loading Screen

```cpp
void showLoadingScreen() {
    auto* loadingWidget = new QWidget;
    loadingWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    loadingWidget->setAttribute(Qt::WA_TranslucentBackground);
    
    auto* layout = new QVBoxLayout(loadingWidget);
    
    auto* progress = new FluentCircularProgress();
    progress->setFixedSize(80, 80);
    progress->startIndeterminate();
    
    auto* label = new QLabel("Loading application...");
    label->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(progress, 0, Qt::AlignCenter);
    layout->addWidget(label);
    
    loadingWidget->show();
}
```

### Dashboard Progress Cards

```cpp
void createProgressCard(const QString& title, qreal progress) {
    auto* card = new QWidget;
    card->setFixedSize(200, 150);
    
    auto* layout = new QVBoxLayout(card);
    
    auto* titleLabel = new QLabel(title);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    auto* progressRing = new FluentCircularProgress();
    progressRing->setFixedSize(60, 60);
    progressRing->setProgress(progress);
    progressRing->setAnimated(true);
    
    auto* percentLabel = new QLabel(QString("%1%").arg(qRound(progress * 100)));
    percentLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(titleLabel);
    layout->addWidget(progressRing, 0, Qt::AlignCenter);
    layout->addWidget(percentLabel);
    
    dashboardLayout->addWidget(card);
}
```

## API Reference

### Enums

```cpp
enum class ProgressStyle { Ring, Filled, Segmented };
```

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `progress` | `qreal` | Current progress value (0.0 to 1.0) |
| `activeColor` | `QColor` | Color of the progress indicator |
| `inactiveColor` | `QColor` | Color of the background track |
| `strokeWidth` | `int` | Width of the progress ring stroke |
| `animated` | `bool` | Whether to animate progress changes |
| `animationDuration` | `int` | Duration of animations in milliseconds |
| `style` | `ProgressStyle` | Visual style of the progress indicator |

### Methods

| Method | Description |
|--------|-------------|
| `setProgress(qreal)` | Set progress value (0.0 to 1.0) |
| `startIndeterminate()` | Start indeterminate animation |
| `stopIndeterminate()` | Stop indeterminate animation |
| `reset()` | Reset progress to 0.0 |
| `isIndeterminate()` | Check if in indeterminate mode |

### Signals

| Signal | Description |
|--------|-------------|
| `progressChanged(qreal)` | Emitted when progress value changes |
| `activeColorChanged(QColor)` | Emitted when active color changes |
| `inactiveColorChanged(QColor)` | Emitted when inactive color changes |
| `styleChanged(ProgressStyle)` | Emitted when style changes |
| `indeterminateStarted()` | Emitted when indeterminate mode starts |
| `indeterminateStopped()` | Emitted when indeterminate mode stops |

## Best Practices

### Performance Guidelines

- Use appropriate animation durations (200-500ms)
- Disable animations for better performance on low-end devices
- Use indeterminate mode sparingly to avoid distraction
- Consider using smaller sizes for embedded progress indicators

### Accessibility Guidelines

- Always provide accessible names and descriptions
- Update accessibility info when progress changes
- Use sufficient color contrast for visibility
- Provide alternative text descriptions for screen readers

### Design Guidelines

- Use consistent sizing across your application
- Choose appropriate colors for different contexts (success, warning, error)
- Provide visual feedback for long-running operations
- Consider showing percentage text for determinate progress

## Related Components

- **[FluentProgressBar](FluentProgressBar.md)** - For linear progress indication
- **[FluentLoadingIndicator](FluentLoadingIndicator.md)** - For simple loading states
- **[FluentDotProgress](FluentDotProgress.md)** - For minimal progress indication
- **[FluentButton](FluentButton.md)** - For buttons with progress states
