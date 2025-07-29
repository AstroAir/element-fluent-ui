# FluentPanel Component

The `FluentPanel` component is a versatile container that implements Microsoft's Fluent Design principles. It provides multiple visual styles, elevation effects, and layout management capabilities for organizing content in modern applications.

## Overview

FluentPanel offers:
- **8 panel types** (Card, Surface, Acrylic, Mica, Sidebar, Content, Dialog, Flyout)
- **4 elevation levels** (None, Low, Medium, High)
- **Flexible corner radius** options
- **Collapsible functionality** with smooth animations
- **Built-in layout helpers** for common patterns
- **Theme integration** with automatic color adaptation
- **Accessibility support** for screen readers

## Basic Usage

### Simple Panel

```cpp
#include "FluentQt/Components/FluentPanel.h"

// Create a basic card panel
auto* panel = new FluentPanel();
panel->setPanelType(FluentPanelType::Card);
panel->setElevation(FluentPanelElevation::Medium);

// Add content using layout helpers
auto* layout = panel->createVerticalLayout();
layout->addWidget(new QLabel("Panel content"));
layout->addWidget(new QPushButton("Action"));
```

### Panel with Title

```cpp
auto* panel = new FluentPanel("Settings");
panel->setPanelType(FluentPanelType::Card);
panel->setCornerRadius(FluentPanelCornerRadius::Medium);

auto* layout = panel->createVerticalLayout();
layout->addWidget(new QLabel("Configure your preferences"));
```

## Panel Types

### Card Panel
Elevated container with shadow, perfect for content cards.

```cpp
auto* card = new FluentPanel("User Profile");
card->setPanelType(FluentPanelType::Card);
card->setElevation(FluentPanelElevation::Medium);
card->setCornerRadius(FluentPanelCornerRadius::Medium);

auto* layout = card->createVerticalLayout();
layout->addWidget(new QLabel("John Doe"));
layout->addWidget(new QLabel("Software Developer"));
```

### Surface Panel
Flat panel with subtle border, ideal for form sections.

```cpp
auto* surface = new FluentPanel("Contact Information");
surface->setPanelType(FluentPanelType::Surface);
surface->setBorderWidth(1);

auto* layout = surface->createVerticalLayout();
layout->addWidget(emailInput);
layout->addWidget(phoneInput);
```

### Acrylic Panel
Translucent panel with blur effect (where supported).

```cpp
auto* acrylic = new FluentPanel();
acrylic->setPanelType(FluentPanelType::Acrylic);
acrylic->setElevation(FluentPanelElevation::High);

// Acrylic panels work best over complex backgrounds
auto* layout = acrylic->createVerticalLayout();
layout->addWidget(new QLabel("Overlay Content"));
```

### Mica Panel
Modern material effect panel.

```cpp
auto* mica = new FluentPanel("Navigation");
mica->setPanelType(FluentPanelType::Mica);
mica->setCornerRadius(FluentPanelCornerRadius::Large);
```

### Sidebar Panel
Optimized for navigation sidebars.

```cpp
auto* sidebar = new FluentPanel();
sidebar->setPanelType(FluentPanelType::Sidebar);
sidebar->setElevation(FluentPanelElevation::Low);

auto* layout = sidebar->createVerticalLayout();
layout->addWidget(new QLabel("Navigation"));
// Add navigation items...
```

### Content Panel
Main content area panel.

```cpp
auto* content = new FluentPanel();
content->setPanelType(FluentPanelType::Content);
content->setPadding(QMargins(24, 24, 24, 24));

auto* layout = content->createVerticalLayout();
// Add main content...
```

### Dialog Panel
Modal dialog container.

```cpp
auto* dialog = new FluentPanel("Confirm Action");
dialog->setPanelType(FluentPanelType::Dialog);
dialog->setElevation(FluentPanelElevation::High);
dialog->setCornerRadius(FluentPanelCornerRadius::Medium);

auto* layout = dialog->createVerticalLayout();
layout->addWidget(new QLabel("Are you sure you want to delete this item?"));

auto* buttonLayout = dialog->createHorizontalLayout();
buttonLayout->addWidget(cancelButton);
buttonLayout->addWidget(deleteButton);
layout->addLayout(buttonLayout);
```

### Flyout Panel
Popup/dropdown container.

```cpp
auto* flyout = new FluentPanel();
flyout->setPanelType(FluentPanelType::Flyout);
flyout->setElevation(FluentPanelElevation::High);
flyout->setCornerRadius(FluentPanelCornerRadius::Small);

auto* layout = flyout->createVerticalLayout();
layout->addWidget(new QLabel("Quick Actions"));
// Add flyout content...
```

## Elevation and Shadows

### Elevation Levels

```cpp
auto* panel = new FluentPanel("Elevated Panel");

// No elevation (flat)
panel->setElevation(FluentPanelElevation::None);

// Low elevation (subtle shadow)
panel->setElevation(FluentPanelElevation::Low);

// Medium elevation (standard shadow)
panel->setElevation(FluentPanelElevation::Medium);

// High elevation (prominent shadow)
panel->setElevation(FluentPanelElevation::High);
```

### Dynamic Elevation

```cpp
auto* interactivePanel = new FluentPanel("Interactive Card");
interactivePanel->setElevation(FluentPanelElevation::Low);

// Increase elevation on hover
connect(interactivePanel, &FluentPanel::hovered, [=]() {
    interactivePanel->setElevation(FluentPanelElevation::Medium);
});

connect(interactivePanel, &FluentPanel::left, [=]() {
    interactivePanel->setElevation(FluentPanelElevation::Low);
});
```

## Corner Radius and Styling

### Corner Radius Options

```cpp
auto* panel = new FluentPanel("Styled Panel");

// No rounded corners
panel->setCornerRadius(FluentPanelCornerRadius::None);

// Small radius (4px)
panel->setCornerRadius(FluentPanelCornerRadius::Small);

// Medium radius (8px) - default
panel->setCornerRadius(FluentPanelCornerRadius::Medium);

// Large radius (16px)
panel->setCornerRadius(FluentPanelCornerRadius::Large);
```

### Custom Colors

```cpp
auto* panel = new FluentPanel("Custom Panel");

// Custom background color
panel->setBackgroundColor(QColor(240, 248, 255));  // Light blue

// Custom border
panel->setBorderColor(QColor(70, 130, 180));       // Steel blue
panel->setBorderWidth(2);

// Custom padding
panel->setPadding(QMargins(20, 16, 20, 16));
```

## Collapsible Panels

### Basic Collapsible Panel

```cpp
auto* collapsiblePanel = new FluentPanel("Advanced Settings");
collapsiblePanel->setCollapsible(true);
collapsiblePanel->setCollapsed(true);  // Start collapsed

auto* layout = collapsiblePanel->createVerticalLayout();
layout->addWidget(new QLabel("These are advanced options"));
layout->addWidget(advancedSettingsWidget);

// Handle collapse state changes
connect(collapsiblePanel, &FluentPanel::collapsedChanged, [](bool collapsed) {
    qDebug() << "Panel is now" << (collapsed ? "collapsed" : "expanded");
});
```

### Programmatic Control

```cpp
auto* panel = new FluentPanel("Expandable Content");
panel->setCollapsible(true);

// Control collapse state
auto* toggleButton = new FluentButton("Toggle Panel");
connect(toggleButton, &FluentButton::clicked, [=]() {
    panel->setCollapsed(!panel->isCollapsed());
});

// Animate expansion/collapse
panel->setAnimated(true);
```

## Layout Helpers

### Vertical Layout

```cpp
auto* panel = new FluentPanel("Vertical Layout");
auto* layout = panel->createVerticalLayout();

layout->addWidget(new QLabel("Item 1"));
layout->addWidget(new QLabel("Item 2"));
layout->addWidget(new QLabel("Item 3"));
layout->addStretch();  // Push content to top
```

### Horizontal Layout

```cpp
auto* panel = new FluentPanel("Horizontal Layout");
auto* layout = panel->createHorizontalLayout();

layout->addWidget(new QLabel("Left"));
layout->addStretch();
layout->addWidget(new QLabel("Right"));
```

### Grid Layout

```cpp
auto* panel = new FluentPanel("Grid Layout");
auto* layout = panel->createGridLayout();

layout->addWidget(new QLabel("Name:"), 0, 0);
layout->addWidget(nameInput, 0, 1);
layout->addWidget(new QLabel("Email:"), 1, 0);
layout->addWidget(emailInput, 1, 1);
```

### Stacked Layout

```cpp
auto* panel = new FluentPanel("Stacked Content");
auto* layout = panel->createStackedLayout();

layout->addWidget(page1Widget);
layout->addWidget(page2Widget);
layout->addWidget(page3Widget);

// Switch between pages
layout->setCurrentIndex(0);
```

## Advanced Usage

### Nested Panels

```cpp
auto* outerPanel = new FluentPanel("Main Container");
outerPanel->setPanelType(FluentPanelType::Content);

auto* outerLayout = outerPanel->createVerticalLayout();

// Nested card panels
auto* card1 = new FluentPanel("Section 1");
card1->setPanelType(FluentPanelType::Card);
card1->setElevation(FluentPanelElevation::Low);

auto* card2 = new FluentPanel("Section 2");
card2->setPanelType(FluentPanelType::Card);
card2->setElevation(FluentPanelElevation::Low);

outerLayout->addWidget(card1);
outerLayout->addWidget(card2);
```

### Responsive Panels

```cpp
auto* responsivePanel = new FluentPanel("Responsive Content");

// Adjust panel based on size
auto updatePanelStyle = [=]() {
    QSize size = responsivePanel->size();
    
    if (size.width() < 400) {
        // Mobile-like layout
        responsivePanel->setPadding(QMargins(12, 12, 12, 12));
        responsivePanel->setCornerRadius(FluentPanelCornerRadius::Small);
    } else {
        // Desktop layout
        responsivePanel->setPadding(QMargins(24, 24, 24, 24));
        responsivePanel->setCornerRadius(FluentPanelCornerRadius::Medium);
    }
};

connect(responsivePanel, &FluentPanel::resized, updatePanelStyle);
```

### Panel with Actions

```cpp
auto* actionPanel = new FluentPanel("Document");
actionPanel->setPanelType(FluentPanelType::Card);

auto* layout = actionPanel->createVerticalLayout();

// Content
layout->addWidget(new QLabel("Document Title"));
layout->addWidget(new QLabel("Document description..."));

// Action buttons
auto* actionLayout = actionPanel->createHorizontalLayout();
actionLayout->addStretch();
actionLayout->addWidget(new FluentButton("Edit"));
actionLayout->addWidget(new FluentButton("Delete"));

layout->addLayout(actionLayout);
```

## Theming and Styling

### Automatic Theme Adaptation

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* panel = new FluentPanel("Themed Panel");

// Panel automatically adapts to theme changes
auto& theme = FluentTheme::instance();

// Manual theme integration (if needed)
connect(&theme, &FluentTheme::themeChanged, [=]() {
    // Update custom styling if needed
    panel->setBackgroundColor(theme.color("surface"));
    panel->setBorderColor(theme.color("border"));
});
```

### Custom Styling

```cpp
auto* styledPanel = new FluentPanel("Custom Styled");

// Apply custom stylesheet
styledPanel->setStyleSheet(R"(
    FluentPanel {
        background-color: qlineargradient(
            x1: 0, y1: 0, x2: 0, y2: 1,
            stop: 0 #f0f8ff,
            stop: 1 #e6f3ff
        );
        border: 2px solid #4682b4;
        border-radius: 12px;
    }
)");
```

## Accessibility

FluentPanel provides accessibility support:

```cpp
auto* panel = new FluentPanel("Accessible Panel");

// Accessible properties
panel->setAccessibleName("User Settings Panel");
panel->setAccessibleDescription("Contains user preference controls");

// Collapsible panel accessibility
if (panel->isCollapsible()) {
    panel->setAccessibleDescription(
        panel->isCollapsed() ? "Collapsed settings panel" : "Expanded settings panel"
    );
}

// Keyboard navigation (automatic)
// - Tab navigation works within panel content
// - Collapsible panels respond to Enter/Space
```

## Best Practices

### Panel Hierarchy
1. **Content Panel** - Main application content area
2. **Card Panels** - Individual content sections
3. **Surface Panels** - Form sections and grouped controls
4. **Sidebar Panels** - Navigation and secondary content

### Elevation Guidelines
- **None** - Flat surfaces, backgrounds
- **Low** - Subtle separation, hover states
- **Medium** - Standard cards, important content
- **High** - Modals, flyouts, critical content

### Layout Recommendations
- Use appropriate padding for content density
- Maintain consistent spacing between panels
- Consider responsive behavior for different screen sizes
- Group related content logically

### Performance Tips
- Avoid excessive nesting of panels
- Use appropriate panel types for content
- Consider lazy loading for collapsible content
- Optimize animations for smooth performance

## Common Patterns

### Settings Panel

```cpp
auto* settingsPanel = new FluentPanel("Application Settings");
settingsPanel->setPanelType(FluentPanelType::Card);
settingsPanel->setCollapsible(true);

auto* layout = settingsPanel->createVerticalLayout();

// Settings sections
auto* generalSection = new FluentPanel("General");
generalSection->setPanelType(FluentPanelType::Surface);
// Add general settings...

auto* advancedSection = new FluentPanel("Advanced");
advancedSection->setPanelType(FluentPanelType::Surface);
advancedSection->setCollapsible(true);
advancedSection->setCollapsed(true);
// Add advanced settings...

layout->addWidget(generalSection);
layout->addWidget(advancedSection);
```

### Dashboard Layout

```cpp
auto* dashboard = new FluentPanel();
dashboard->setPanelType(FluentPanelType::Content);

auto* layout = dashboard->createGridLayout();

// Dashboard cards
auto* statsCard = new FluentPanel("Statistics");
statsCard->setPanelType(FluentPanelType::Card);
statsCard->setElevation(FluentPanelElevation::Low);

auto* chartCard = new FluentPanel("Performance");
chartCard->setPanelType(FluentPanelType::Card);
chartCard->setElevation(FluentPanelElevation::Low);

auto* activityCard = new FluentPanel("Recent Activity");
activityCard->setPanelType(FluentPanelType::Card);
activityCard->setElevation(FluentPanelElevation::Low);

layout->addWidget(statsCard, 0, 0);
layout->addWidget(chartCard, 0, 1);
layout->addWidget(activityCard, 1, 0, 1, 2);  // Span 2 columns
```

## Signals Reference

```cpp
// Title and state changes
void titleChanged(const QString& title);
void collapsedChanged(bool collapsed);

// Interaction signals
void clicked();
void hovered();
void left();
void resized();
```

## Standalone Examples Collection

### Example 1: Modern Dashboard Layout

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QTimer>
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Styling/FluentTheme.h"

class ModernDashboard : public QMainWindow {
    Q_OBJECT
public:
    ModernDashboard(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupDataUpdates();
        connectSignals();
    }

private slots:
    void updateMetrics() {
        // Simulate real-time data updates
        static int counter = 0;
        counter++;

        // Update statistics
        m_usersLabel->setText(QString::number(1250 + (counter * 3)));
        m_revenueLabel->setText(QString("$%1K").arg(45.2 + (counter * 0.1), 0, 'f', 1));
        m_ordersLabel->setText(QString::number(89 + (counter % 10)));

        // Update progress bars
        m_cpuProgress->setValue((counter * 2) % 100);
        m_memoryProgress->setValue((counter * 3) % 100);
        m_diskProgress->setValue((counter) % 100);

        // Update chart data
        updateChartData();
    }

    void updateChartData() {
        // Add new data point to performance chart
        static QLineSeries* series = qobject_cast<QLineSeries*>(m_chart->series().first());
        if (series) {
            qreal x = series->count();
            qreal y = 50 + 30 * qSin(x * 0.1) + (QRandomGenerator::global()->bounded(20) - 10);
            series->append(x, y);

            // Keep only last 50 points
            if (series->count() > 50) {
                series->remove(0);
                // Shift x values
                for (int i = 0; i < series->count(); ++i) {
                    QPointF point = series->at(i);
                    series->replace(i, QPointF(i, point.y()));
                }
            }
        }
    }

    void toggleTheme() {
        auto& theme = FluentTheme::instance();
        auto newMode = (theme.mode() == FluentThemeMode::Light)
            ? FluentThemeMode::Dark
            : FluentThemeMode::Light;
        theme.setMode(newMode);

        // Update panel appearances for theme
        updatePanelThemes();
    }

    void refreshData() {
        m_refreshButton->setLoading(true);
        m_refreshButton->setText("Refreshing...");

        // Simulate data refresh
        QTimer::singleShot(2000, [this]() {
            m_refreshButton->setLoading(false);
            m_refreshButton->setText("Refresh");
            updateMetrics();
        });
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);

        // Header panel
        auto* headerPanel = new FluentPanel();
        headerPanel->setPanelType(FluentPanelType::Surface);
        headerPanel->setElevation(FluentPanelElevation::Low);

        auto* headerLayout = headerPanel->createHorizontalLayout();
        auto* titleLabel = new QLabel("Analytics Dashboard");
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");

        m_refreshButton = new FluentButton("Refresh");
        m_refreshButton->setButtonStyle(FluentButtonStyle::Subtle);

        auto* themeButton = new FluentButton("Toggle Theme");
        themeButton->setButtonStyle(FluentButtonStyle::Subtle);
        connect(themeButton, &FluentButton::clicked, this, &ModernDashboard::toggleTheme);

        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(m_refreshButton);
        headerLayout->addWidget(themeButton);

        // Metrics row
        auto* metricsLayout = new QHBoxLayout;

        // Users metric
        auto* usersPanel = createMetricPanel("Active Users", "1,250", "#0078d4");
        m_usersLabel = usersPanel->findChild<QLabel*>("valueLabel");

        // Revenue metric
        auto* revenuePanel = createMetricPanel("Revenue", "$45.2K", "#107c10");
        m_revenueLabel = revenuePanel->findChild<QLabel*>("valueLabel");

        // Orders metric
        auto* ordersPanel = createMetricPanel("Orders", "89", "#d83b01");
        m_ordersLabel = ordersPanel->findChild<QLabel*>("valueLabel");

        metricsLayout->addWidget(usersPanel);
        metricsLayout->addWidget(revenuePanel);
        metricsLayout->addWidget(ordersPanel);

        // Content row
        auto* contentLayout = new QHBoxLayout;

        // Performance chart panel
        auto* chartPanel = new FluentPanel("Performance Metrics");
        chartPanel->setPanelType(FluentPanelType::Card);
        chartPanel->setElevation(FluentPanelElevation::Medium);
        chartPanel->setMinimumSize(400, 300);

        m_chart = createPerformanceChart();
        auto* chartView = new QChartView(m_chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        auto* chartLayout = chartPanel->createVerticalLayout();
        chartLayout->addWidget(chartView);

        // System status panel
        auto* statusPanel = createSystemStatusPanel();

        contentLayout->addWidget(chartPanel, 2);
        contentLayout->addWidget(statusPanel, 1);

        // Activity panel
        auto* activityPanel = createActivityPanel();

        mainLayout->addWidget(headerPanel);
        mainLayout->addLayout(metricsLayout);
        mainLayout->addLayout(contentLayout);
        mainLayout->addWidget(activityPanel);
    }

    FluentPanel* createMetricPanel(const QString& title, const QString& value, const QString& color) {
        auto* panel = new FluentPanel();
        panel->setPanelType(FluentPanelType::Card);
        panel->setElevation(FluentPanelElevation::Low);
        panel->setMinimumSize(200, 120);

        auto* layout = panel->createVerticalLayout();

        auto* titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("color: gray; font-size: 14px;");

        auto* valueLabel = new QLabel(value);
        valueLabel->setObjectName("valueLabel");
        valueLabel->setStyleSheet(QString("color: %1; font-size: 32px; font-weight: bold;").arg(color));

        auto* changeLabel = new QLabel("↗ +12.5%");
        changeLabel->setStyleSheet("color: #107c10; font-size: 12px;");

        layout->addWidget(titleLabel);
        layout->addWidget(valueLabel);
        layout->addWidget(changeLabel);
        layout->addStretch();

        return panel;
    }

    QChart* createPerformanceChart() {
        auto* chart = new QChart();
        chart->setTitle("Performance Over Time");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        auto* series = new QLineSeries();
        series->setName("Response Time (ms)");

        // Initialize with some data
        for (int i = 0; i < 20; ++i) {
            qreal y = 50 + 30 * qSin(i * 0.1) + (QRandomGenerator::global()->bounded(20) - 10);
            series->append(i, y);
        }

        chart->addSeries(series);
        chart->createDefaultAxes();
        chart->legend()->setVisible(false);

        return chart;
    }

    FluentPanel* createSystemStatusPanel() {
        auto* panel = new FluentPanel("System Status");
        panel->setPanelType(FluentPanelType::Card);
        panel->setElevation(FluentPanelElevation::Medium);
        panel->setMinimumSize(250, 300);

        auto* layout = panel->createVerticalLayout();

        // CPU usage
        auto* cpuLayout = new QHBoxLayout;
        cpuLayout->addWidget(new QLabel("CPU:"));
        m_cpuProgress = new QProgressBar;
        m_cpuProgress->setValue(45);
        m_cpuProgress->setStyleSheet("QProgressBar::chunk { background-color: #0078d4; }");
        cpuLayout->addWidget(m_cpuProgress);

        // Memory usage
        auto* memoryLayout = new QHBoxLayout;
        memoryLayout->addWidget(new QLabel("Memory:"));
        m_memoryProgress = new QProgressBar;
        m_memoryProgress->setValue(67);
        m_memoryProgress->setStyleSheet("QProgressBar::chunk { background-color: #107c10; }");
        memoryLayout->addWidget(m_memoryProgress);

        // Disk usage
        auto* diskLayout = new QHBoxLayout;
        diskLayout->addWidget(new QLabel("Disk:"));
        m_diskProgress = new QProgressBar;
        m_diskProgress->setValue(23);
        m_diskProgress->setStyleSheet("QProgressBar::chunk { background-color: #d83b01; }");
        diskLayout->addWidget(m_diskProgress);

        layout->addLayout(cpuLayout);
        layout->addLayout(memoryLayout);
        layout->addLayout(diskLayout);
        layout->addStretch();

        return panel;
    }

    FluentPanel* createActivityPanel() {
        auto* panel = new FluentPanel("Recent Activity");
        panel->setPanelType(FluentPanelType::Card);
        panel->setElevation(FluentPanelElevation::Low);
        panel->setMaximumHeight(200);

        auto* layout = panel->createVerticalLayout();

        QStringList activities = {
            "User john.doe@company.com logged in",
            "New order #12345 received",
            "System backup completed successfully",
            "Payment processed for order #12344",
            "User jane.smith@company.com updated profile"
        };

        for (const QString& activity : activities) {
            auto* activityLayout = new QHBoxLayout;
            auto* timeLabel = new QLabel("2 min ago");
            timeLabel->setStyleSheet("color: gray; font-size: 12px;");
            timeLabel->setMinimumWidth(80);

            auto* activityLabel = new QLabel(activity);
            activityLabel->setStyleSheet("font-size: 14px;");

            activityLayout->addWidget(timeLabel);
            activityLayout->addWidget(activityLabel);
            activityLayout->addStretch();

            layout->addLayout(activityLayout);
        }

        return panel;
    }

    void setupDataUpdates() {
        // Update metrics every 3 seconds
        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ModernDashboard::updateMetrics);
        timer->start(3000);
    }

    void connectSignals() {
        connect(m_refreshButton, &FluentButton::clicked, this, &ModernDashboard::refreshData);
    }

    void updatePanelThemes() {
        // Panels automatically update with theme changes
        // This method can be used for custom theme adjustments
    }

    FluentButton* m_refreshButton;
    QLabel* m_usersLabel;
    QLabel* m_revenueLabel;
    QLabel* m_ordersLabel;
    QChart* m_chart;
    QProgressBar* m_cpuProgress;
    QProgressBar* m_memoryProgress;
    QProgressBar* m_diskProgress;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ModernDashboard dashboard;
    dashboard.resize(1200, 800);
    dashboard.show();

    return app.exec();
}

#include "modern_dashboard.moc"
```

## See Also

- [FluentCard](FluentCard.md) - For specialized card containers
- [FluentNavigationView](FluentNavigationView.md) - For navigation panels
- [FluentTheme](../styling/FluentTheme.md) - For theming and styling
- [Layout Guide](../guides/layout-patterns.md) - Best practices for layouts
