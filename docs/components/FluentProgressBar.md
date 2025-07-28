# FluentProgressBar Component

The `FluentProgressBar` component is an enhanced progress indicator that implements Microsoft's Fluent Design principles. It provides smooth animations, multiple styles, and comprehensive customization options for showing task progress.

## Overview

FluentProgressBar offers:
- **Multiple progress styles** (Linear, Circular, Ring)
- **Smooth animations** with customizable easing
- **Indeterminate mode** for unknown progress
- **Custom formatting** for progress text
- **Theme integration** with accent colors
- **Accessibility support** with progress announcements
- **Performance optimized** rendering

## Basic Usage

### Simple Progress Bar

```cpp
#include "FluentQt/Components/FluentProgressBar.h"

// Create a basic progress bar
auto* progressBar = new FluentProgressBar;
progressBar->setMinimum(0);
progressBar->setMaximum(100);
progressBar->setValue(50);

// Add to layout
layout->addWidget(progressBar);
```

### Progress Bar with Label

```cpp
auto* progressBar = new FluentProgressBar;
progressBar->setLabelText("Loading...");
progressBar->setRange(0, 100);
progressBar->setValue(25);

// Show percentage
progressBar->setFormat("%p%");  // Shows "25%"

// Custom format
progressBar->setFormat("%v of %m files");  // Shows "25 of 100 files"
```

## Progress Styles

### Linear Progress Bar

```cpp
auto* linearProgress = new FluentProgressBar;
linearProgress->setProgressStyle(FluentProgressStyle::Linear);
linearProgress->setRange(0, 100);
linearProgress->setValue(60);

// Configure appearance
linearProgress->setHeight(8);           // Thin progress bar
linearProgress->setCornerRadius(4);     // Rounded corners
linearProgress->setShowText(true);      // Show progress text
```

### Circular Progress Bar

```cpp
auto* circularProgress = new FluentProgressBar;
circularProgress->setProgressStyle(FluentProgressStyle::Circular);
circularProgress->setRange(0, 100);
circularProgress->setValue(75);

// Configure appearance
circularProgress->setDiameter(80);      // Size of the circle
circularProgress->setStrokeWidth(6);    // Thickness of the progress ring
circularProgress->setShowText(true);    // Show percentage in center
```

### Ring Progress Bar

```cpp
auto* ringProgress = new FluentProgressBar;
ringProgress->setProgressStyle(FluentProgressStyle::Ring);
ringProgress->setRange(0, 100);
ringProgress->setValue(40);

// Configure appearance
ringProgress->setDiameter(100);
ringProgress->setStrokeWidth(8);
ringProgress->setInnerRadius(30);       // Size of inner circle
```

## Indeterminate Progress

### Indeterminate Linear

```cpp
auto* indeterminateProgress = new FluentProgressBar;
indeterminateProgress->setProgressStyle(FluentProgressStyle::Linear);
indeterminateProgress->setIndeterminate(true);  // Unknown progress
indeterminateProgress->setLabelText("Processing...");

// Start/stop indeterminate animation
indeterminateProgress->startAnimation();
// indeterminateProgress->stopAnimation();
```

### Indeterminate Circular

```cpp
auto* spinnerProgress = new FluentProgressBar;
spinnerProgress->setProgressStyle(FluentProgressStyle::Circular);
spinnerProgress->setIndeterminate(true);
spinnerProgress->setDiameter(40);

// Configure animation
spinnerProgress->setAnimationDuration(1000);    // 1 second rotation
spinnerProgress->setAnimationEasing(FluentEasing::Linear);
```

## Animations

### Smooth Value Changes

```cpp
auto* progressBar = new FluentProgressBar;
progressBar->setRange(0, 100);
progressBar->setValue(0);

// Enable smooth animations
progressBar->setAnimated(true);
progressBar->setAnimationDuration(500);         // 500ms transition
progressBar->setAnimationEasing(FluentEasing::CubicOut);

// Animate to new value
progressBar->animateToValue(75);  // Smoothly animate from current to 75

// Chain animations
connect(progressBar, &FluentProgressBar::animationFinished, [=]() {
    QTimer::singleShot(1000, [=]() {
        progressBar->animateToValue(100);
    });
});
```

### Custom Animation Curves

```cpp
auto* progressBar = new FluentProgressBar;

// Use different easing curves
progressBar->setAnimationEasing(FluentEasing::BounceOut);    // Bouncy animation
progressBar->setAnimationEasing(FluentEasing::ElasticOut);   // Elastic animation
progressBar->setAnimationEasing(FluentEasing::BackOut);      // Back animation

// Custom animation curve
QEasingCurve customCurve(QEasingCurve::BezierSpline);
customCurve.addCubicBezierSegment(QPointF(0.25, 0.1), QPointF(0.25, 1.0), QPointF(1.0, 1.0));
progressBar->setCustomAnimationCurve(customCurve);
```

## Styling and Theming

### Color Customization

```cpp
auto* progressBar = new FluentProgressBar;

// Use theme accent color (default)
progressBar->setUseAccentColor(true);

// Custom colors
progressBar->setProgressColor(QColor(0, 120, 215));         // Blue progress
progressBar->setBackgroundColor(QColor(240, 240, 240));     // Light gray background
progressBar->setTextColor(QColor(50, 50, 50));              // Dark gray text

// Gradient progress
QLinearGradient gradient(0, 0, 1, 0);
gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
gradient.setColorAt(0, QColor(0, 120, 215));
gradient.setColorAt(1, QColor(0, 180, 120));
progressBar->setProgressGradient(gradient);
```

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* progressBar = new FluentProgressBar;

// Automatic theme integration
auto& theme = FluentTheme::instance();
progressBar->setProgressColor(theme.color("accent"));
progressBar->setBackgroundColor(theme.color("surface"));

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    progressBar->setProgressColor(theme.color("accent"));
    progressBar->setBackgroundColor(theme.color("surface"));
});
```

## Progress States

### Different Progress States

```cpp
auto* progressBar = new FluentProgressBar;

// Normal progress (default)
progressBar->setProgressState(FluentProgressState::Normal);
progressBar->setProgressColor(QColor(0, 120, 215));  // Blue

// Success state
progressBar->setProgressState(FluentProgressState::Success);
progressBar->setProgressColor(QColor(16, 124, 16));  // Green

// Warning state
progressBar->setProgressState(FluentProgressState::Warning);
progressBar->setProgressColor(QColor(255, 140, 0));  // Orange

// Error state
progressBar->setProgressState(FluentProgressState::Error);
progressBar->setProgressColor(QColor(196, 43, 28));  // Red

// Paused state
progressBar->setProgressState(FluentProgressState::Paused);
progressBar->setProgressColor(QColor(255, 185, 0));  // Yellow
```

### State-Based Animations

```cpp
auto* progressBar = new FluentProgressBar;

// Different animations for different states
connect(progressBar, &FluentProgressBar::stateChanged, [=](FluentProgressState state) {
    switch (state) {
        case FluentProgressState::Success:
            // Pulse animation for success
            auto* animator = new FluentAnimator(this);
            animator->pulseEffect(progressBar, 3);
            break;
            
        case FluentProgressState::Error:
            // Shake animation for error
            animator->shakeEffect(progressBar);
            break;
            
        case FluentProgressState::Paused:
            // Slow blink for paused
            animator->blinkEffect(progressBar, 2000);
            break;
    }
});
```

## File Upload Example

### Upload Progress with Multiple Files

```cpp
class FileUploadWidget : public QWidget
{
public:
    FileUploadWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
    }
    
    void uploadFiles(const QStringList& filePaths) {
        m_filesToUpload = filePaths;
        m_currentFileIndex = 0;
        m_overallProgress->setMaximum(filePaths.size());
        m_overallProgress->setValue(0);
        
        uploadNextFile();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Overall progress
        m_overallProgress = new FluentProgressBar;
        m_overallProgress->setLabelText("Overall Progress");
        m_overallProgress->setFormat("Uploading file %v of %m");
        
        // Current file progress
        m_fileProgress = new FluentProgressBar;
        m_fileProgress->setLabelText("Current File");
        m_fileProgress->setFormat("%p% - %v KB of %m KB");
        
        // Status label
        m_statusLabel = new QLabel("Ready to upload");
        
        layout->addWidget(m_overallProgress);
        layout->addWidget(m_fileProgress);
        layout->addWidget(m_statusLabel);
    }
    
    void setupConnections() {
        // Connect to upload manager signals
        connect(m_uploadManager, &UploadManager::fileProgress, [=](int bytesUploaded, int totalBytes) {
            m_fileProgress->setMaximum(totalBytes / 1024);  // Convert to KB
            m_fileProgress->setValue(bytesUploaded / 1024);
        });
        
        connect(m_uploadManager, &UploadManager::fileCompleted, [=](const QString& fileName) {
            m_currentFileIndex++;
            m_overallProgress->setValue(m_currentFileIndex);
            
            if (m_currentFileIndex < m_filesToUpload.size()) {
                uploadNextFile();
            } else {
                // All files uploaded
                m_overallProgress->setProgressState(FluentProgressState::Success);
                m_fileProgress->setProgressState(FluentProgressState::Success);
                m_statusLabel->setText("All files uploaded successfully!");
            }
        });
        
        connect(m_uploadManager, &UploadManager::fileError, [=](const QString& fileName, const QString& error) {
            m_overallProgress->setProgressState(FluentProgressState::Error);
            m_fileProgress->setProgressState(FluentProgressState::Error);
            m_statusLabel->setText(QString("Error uploading %1: %2").arg(fileName, error));
        });
    }
    
    void uploadNextFile() {
        if (m_currentFileIndex < m_filesToUpload.size()) {
            QString fileName = m_filesToUpload[m_currentFileIndex];
            m_statusLabel->setText(QString("Uploading: %1").arg(QFileInfo(fileName).fileName()));
            
            m_fileProgress->setValue(0);
            m_fileProgress->setProgressState(FluentProgressState::Normal);
            
            m_uploadManager->uploadFile(fileName);
        }
    }

private:
    FluentProgressBar* m_overallProgress{nullptr};
    FluentProgressBar* m_fileProgress{nullptr};
    QLabel* m_statusLabel{nullptr};
    
    UploadManager* m_uploadManager{new UploadManager(this)};
    QStringList m_filesToUpload;
    int m_currentFileIndex{0};
};
```

## Task Progress Dashboard

### Multiple Progress Indicators

```cpp
class TaskDashboard : public QWidget
{
public:
    TaskDashboard(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        startProgressSimulation();
    }

private:
    void setupUI() {
        auto* layout = new QGridLayout(this);
        
        // CPU Usage (circular)
        m_cpuProgress = new FluentProgressBar;
        m_cpuProgress->setProgressStyle(FluentProgressStyle::Circular);
        m_cpuProgress->setLabelText("CPU Usage");
        m_cpuProgress->setDiameter(100);
        m_cpuProgress->setFormat("%p%");
        
        // Memory Usage (ring)
        m_memoryProgress = new FluentProgressBar;
        m_memoryProgress->setProgressStyle(FluentProgressStyle::Ring);
        m_memoryProgress->setLabelText("Memory Usage");
        m_memoryProgress->setDiameter(100);
        m_memoryProgress->setFormat("%p%");
        
        // Disk Usage (linear)
        m_diskProgress = new FluentProgressBar;
        m_diskProgress->setProgressStyle(FluentProgressStyle::Linear);
        m_diskProgress->setLabelText("Disk Usage");
        m_diskProgress->setFormat("%v GB of %m GB");
        
        // Network Activity (indeterminate)
        m_networkProgress = new FluentProgressBar;
        m_networkProgress->setProgressStyle(FluentProgressStyle::Linear);
        m_networkProgress->setLabelText("Network Activity");
        m_networkProgress->setIndeterminate(true);
        m_networkProgress->setHeight(4);
        
        layout->addWidget(createProgressCard("CPU", m_cpuProgress), 0, 0);
        layout->addWidget(createProgressCard("Memory", m_memoryProgress), 0, 1);
        layout->addWidget(createProgressCard("Disk", m_diskProgress), 1, 0);
        layout->addWidget(createProgressCard("Network", m_networkProgress), 1, 1);
    }
    
    QWidget* createProgressCard(const QString& title, FluentProgressBar* progressBar) {
        auto* card = new FluentCard(title);
        auto* layout = new QVBoxLayout;
        layout->addWidget(progressBar);
        
        auto* contentWidget = new QWidget;
        contentWidget->setLayout(layout);
        card->setContentWidget(contentWidget);
        
        return card;
    }
    
    void startProgressSimulation() {
        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [=]() {
            // Simulate changing values
            m_cpuProgress->setValue(QRandomGenerator::global()->bounded(0, 101));
            m_memoryProgress->setValue(QRandomGenerator::global()->bounded(30, 90));
            m_diskProgress->setValue(QRandomGenerator::global()->bounded(100, 500));
            
            // Update colors based on values
            updateProgressColors();
        });
        timer->start(2000);  // Update every 2 seconds
        
        // Start network activity animation
        m_networkProgress->startAnimation();
    }
    
    void updateProgressColors() {
        // CPU color based on usage
        if (m_cpuProgress->value() > 80) {
            m_cpuProgress->setProgressState(FluentProgressState::Error);
        } else if (m_cpuProgress->value() > 60) {
            m_cpuProgress->setProgressState(FluentProgressState::Warning);
        } else {
            m_cpuProgress->setProgressState(FluentProgressState::Normal);
        }
        
        // Memory color based on usage
        if (m_memoryProgress->value() > 85) {
            m_memoryProgress->setProgressState(FluentProgressState::Error);
        } else if (m_memoryProgress->value() > 70) {
            m_memoryProgress->setProgressState(FluentProgressState::Warning);
        } else {
            m_memoryProgress->setProgressState(FluentProgressState::Normal);
        }
    }

private:
    FluentProgressBar* m_cpuProgress{nullptr};
    FluentProgressBar* m_memoryProgress{nullptr};
    FluentProgressBar* m_diskProgress{nullptr};
    FluentProgressBar* m_networkProgress{nullptr};
};
```

## Accessibility

FluentProgressBar provides comprehensive accessibility support:

```cpp
auto* progressBar = new FluentProgressBar;

// Accessible properties (automatically set)
progressBar->setAccessibleName("File download progress");
progressBar->setAccessibleDescription("Shows the progress of the current file download");

// Progress announcements (automatic)
// - Screen readers announce progress changes
// - Percentage updates are announced at intervals
// - State changes are announced

// Custom accessibility
progressBar->setAccessibleRole(QAccessible::ProgressBar);

// Configure announcement frequency
progressBar->setProgressAnnouncementInterval(10);  // Announce every 10% change
```

## Performance Optimization

### Efficient Updates

```cpp
auto* progressBar = new FluentProgressBar;

// Batch updates for better performance
progressBar->setUpdatesEnabled(false);
progressBar->setMinimum(0);
progressBar->setMaximum(1000);
progressBar->setValue(500);
progressBar->setFormat("Processing %v of %m items");
progressBar->setUpdatesEnabled(true);

// Throttle rapid updates
QTimer* updateTimer = new QTimer(this);
updateTimer->setSingleShot(true);
updateTimer->setInterval(16);  // ~60 FPS

connect(dataProcessor, &DataProcessor::progressChanged, [=](int value) {
    m_pendingValue = value;
    if (!updateTimer->isActive()) {
        updateTimer->start();
    }
});

connect(updateTimer, &QTimer::timeout, [=]() {
    progressBar->setValue(m_pendingValue);
});
```

## Best Practices

### Progress Bar Guidelines
1. **Provide meaningful labels** - Describe what is being processed
2. **Use appropriate styles** - Linear for tasks, circular for status
3. **Show time estimates** when possible
4. **Use indeterminate mode** for unknown duration tasks
5. **Provide cancel options** for long-running operations

### Performance Considerations
- Throttle rapid progress updates
- Use appropriate animation durations
- Disable animations for very frequent updates
- Consider using indeterminate mode for very fast operations

### Accessibility Guidelines
- Provide descriptive labels
- Announce progress changes appropriately
- Support keyboard interaction for associated controls
- Ensure sufficient color contrast
- Test with screen readers

## Signals Reference

```cpp
// Value change signals
void valueChanged(int value);
void progressChanged(double percentage);

// Animation signals
void animationStarted();
void animationFinished();

// State change signals
void stateChanged(FluentProgressState state);
```

## See Also

- [FluentLoadingIndicator](FluentLoadingIndicator.md) - For simple loading states
- [FluentButton](FluentButton.md) - For progress-related actions
- [FluentCard](FluentCard.md) - For organizing progress displays
- [API Reference - FluentProgressBar](../API_REFERENCE.md#fluentprogressbar) - Complete API documentation
