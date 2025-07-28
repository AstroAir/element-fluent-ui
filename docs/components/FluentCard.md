# FluentCard Component

The `FluentCard` component is a versatile container that implements Microsoft's Fluent Design card pattern. It provides elevation, shadows, and structured content areas for organizing information in an appealing and accessible way.

## Overview

FluentCard offers:
- **Structured layout** with header, content, and footer sections
- **Elevation and shadows** following Fluent Design principles
- **Interactive states** with hover and click animations
- **Flexible content** support for any Qt widgets
- **Theme integration** with automatic color adaptation
- **Accessibility support** with proper ARIA roles

## Basic Usage

### Simple Card

```cpp
#include "FluentQt/Components/FluentCard.h"

// Create a basic card
auto* card = new FluentCard("Card Title");
card->setContent("This is the card content area.");

// Add to layout
layout->addWidget(card);
```

### Card with Custom Content

```cpp
auto* card = new FluentCard("User Profile");

// Create custom content widget
auto* contentWidget = new QWidget;
auto* contentLayout = new QVBoxLayout(contentWidget);

auto* nameLabel = new QLabel("John Doe");
nameLabel->setFont(QFont("Segoe UI", 14, QFont::Bold));

auto* emailLabel = new QLabel("john.doe@example.com");
emailLabel->setStyleSheet("color: #666;");

auto* editButton = new FluentButton("Edit Profile");
editButton->setButtonStyle(FluentButtonStyle::Primary);

contentLayout->addWidget(nameLabel);
contentLayout->addWidget(emailLabel);
contentLayout->addWidget(editButton);

// Set the custom content
card->setContentWidget(contentWidget);
```

## Card Structure

### Header, Content, and Footer

```cpp
auto* card = new FluentCard();

// Header with title and subtitle
card->setTitle("Project Status");
card->setSubtitle("Last updated 2 hours ago");

// Header actions (optional)
auto* headerActions = new QWidget;
auto* actionsLayout = new QHBoxLayout(headerActions);
actionsLayout->addWidget(new FluentButton("Refresh"));
actionsLayout->addWidget(new FluentButton("Settings"));
card->setHeaderActions(headerActions);

// Main content
auto* contentWidget = new QWidget;
auto* contentLayout = new QVBoxLayout(contentWidget);

auto* progressBar = new FluentProgressBar;
progressBar->setValue(75);
progressBar->setFormat("75% Complete");

auto* statusLabel = new QLabel("3 of 4 tasks completed");

contentLayout->addWidget(progressBar);
contentLayout->addWidget(statusLabel);
card->setContentWidget(contentWidget);

// Footer with actions
auto* footerWidget = new QWidget;
auto* footerLayout = new QHBoxLayout(footerWidget);
footerLayout->addStretch();
footerLayout->addWidget(new FluentButton("View Details"));
footerLayout->addWidget(FluentButton::createPrimaryButton("Continue"));
card->setFooterWidget(footerWidget);
```

## Card Types and Styling

### Elevation Levels

```cpp
auto* card = new FluentCard("Elevated Card");

// Set elevation level (0-5)
card->setElevation(0);  // Flat, no shadow
card->setElevation(1);  // Subtle shadow
card->setElevation(2);  // Default elevation
card->setElevation(3);  // Medium elevation
card->setElevation(4);  // High elevation
card->setElevation(5);  // Maximum elevation
```

### Interactive Cards

```cpp
auto* clickableCard = new FluentCard("Clickable Card");
clickableCard->setClickable(true);  // Enable click interactions

// Handle card clicks
connect(clickableCard, &FluentCard::clicked, [=]() {
    qDebug() << "Card was clicked!";
    // Navigate to detail view, expand content, etc.
});

// Hover effects are automatically enabled for clickable cards
connect(clickableCard, &FluentCard::hovered, [=](bool isHovered) {
    if (isHovered) {
        // Custom hover animation
        auto* animator = new FluentAnimator(this);
        animator->scaleEffect(clickableCard, 1.02, 200);
    }
});
```

### Card Variants

```cpp
// Outlined card (border instead of shadow)
auto* outlinedCard = new FluentCard("Outlined Card");
outlinedCard->setVariant(FluentCardVariant::Outlined);

// Filled card (background color)
auto* filledCard = new FluentCard("Filled Card");
filledCard->setVariant(FluentCardVariant::Filled);

// Subtle card (minimal styling)
auto* subtleCard = new FluentCard("Subtle Card");
subtleCard->setVariant(FluentCardVariant::Subtle);
```

## Animations

FluentCard includes smooth animations for state transitions:

```cpp
auto* card = new FluentCard("Animated Card");

// Enable/disable animations
card->setAnimated(true);   // Default: enabled

// Custom entrance animation
auto* animator = new FluentAnimator(this);
animator->slideInFromBottom(card, 300);

// Animate content changes
connect(updateButton, &FluentButton::clicked, [=]() {
    // Fade out old content
    auto* fadeOut = animator->fadeOut(card->contentWidget(), 150);
    
    connect(fadeOut, &QPropertyAnimation::finished, [=]() {
        // Update content
        updateCardContent(card);
        
        // Fade in new content
        animator->fadeIn(card->contentWidget(), 150);
    });
    
    fadeOut->start();
});
```

## Layout Patterns

### Card Grid Layout

```cpp
auto* gridWidget = new QWidget;
auto* gridLayout = new QGridLayout(gridWidget);
gridLayout->setSpacing(16);

// Create a grid of cards
for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 4; ++col) {
        auto* card = new FluentCard(QString("Card %1,%2").arg(row).arg(col));
        card->setContent(QString("Content for card at position %1,%2").arg(row).arg(col));
        card->setFixedSize(200, 150);
        
        gridLayout->addWidget(card, row, col);
    }
}
```

### Card List Layout

```cpp
auto* scrollArea = new QScrollArea;
auto* listWidget = new QWidget;
auto* listLayout = new QVBoxLayout(listWidget);

// Create a vertical list of cards
QStringList items = {"Task 1", "Task 2", "Task 3", "Task 4", "Task 5"};
for (const QString& item : items) {
    auto* card = new FluentCard(item);
    
    // Add content specific to each item
    auto* contentWidget = new QWidget;
    auto* contentLayout = new QHBoxLayout(contentWidget);
    
    auto* descriptionLabel = new QLabel(QString("Description for %1").arg(item));
    auto* actionButton = new FluentButton("Action");
    
    contentLayout->addWidget(descriptionLabel);
    contentLayout->addStretch();
    contentLayout->addWidget(actionButton);
    
    card->setContentWidget(contentWidget);
    card->setClickable(true);
    
    listLayout->addWidget(card);
}

listLayout->addStretch();
scrollArea->setWidget(listWidget);
scrollArea->setWidgetResizable(true);
```

## Accessibility

FluentCard provides comprehensive accessibility support:

```cpp
auto* card = new FluentCard("Accessible Card");

// Accessible properties (automatically set)
card->setAccessibleName("User profile card");
card->setAccessibleDescription("Contains user information and profile actions");

// For clickable cards
if (card->isClickable()) {
    card->setAccessibleRole(QAccessible::Button);
    card->setAccessibleDescription("Click to view detailed user profile");
}

// Keyboard navigation (automatic for clickable cards)
// - Tab to focus the card
// - Enter/Space to activate
// - Arrow keys for navigation between cards
```

## Theming Integration

FluentCard automatically adapts to theme changes:

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* card = new FluentCard("Themed Card");

// Card automatically uses theme colors
auto& theme = FluentTheme::instance();

// Custom theme integration (if needed)
connect(&theme, &FluentTheme::themeChanged, [=]() {
    // Update custom styling based on theme
    if (theme.isDarkMode()) {
        card->setStyleSheet("FluentCard { border: 1px solid #333; }");
    } else {
        card->setStyleSheet("FluentCard { border: 1px solid #e0e0e0; }");
    }
});
```

## Advanced Usage

### Dynamic Content Updates

```cpp
auto* newsCard = new FluentCard("Latest News");

// Function to update card content
auto updateNews = [=](const QJsonObject& newsData) {
    auto* contentWidget = new QWidget;
    auto* layout = new QVBoxLayout(contentWidget);
    
    // Update title and subtitle
    newsCard->setTitle(newsData["title"].toString());
    newsCard->setSubtitle(newsData["publishedAt"].toString());
    
    // Update content
    auto* summaryLabel = new QLabel(newsData["summary"].toString());
    summaryLabel->setWordWrap(true);
    
    auto* readMoreButton = new FluentButton("Read More");
    connect(readMoreButton, &FluentButton::clicked, [=]() {
        QDesktopServices::openUrl(QUrl(newsData["url"].toString()));
    });
    
    layout->addWidget(summaryLabel);
    layout->addWidget(readMoreButton);
    
    // Animate content change
    auto* animator = new FluentAnimator(this);
    auto* fadeOut = animator->fadeOut(newsCard, 200);
    
    connect(fadeOut, &QPropertyAnimation::finished, [=]() {
        newsCard->setContentWidget(contentWidget);
        animator->fadeIn(newsCard, 200);
    });
    
    fadeOut->start();
};

// Update news every 5 minutes
auto* timer = new QTimer(this);
connect(timer, &QTimer::timeout, [=]() {
    // Fetch latest news and update
    fetchLatestNews(updateNews);
});
timer->start(300000); // 5 minutes
```

### Card with Loading State

```cpp
auto* dataCard = new FluentCard("Data Overview");

// Show loading state
auto showLoading = [=]() {
    auto* loadingWidget = new QWidget;
    auto* layout = new QVBoxLayout(loadingWidget);
    
    auto* loadingIndicator = new FluentLoadingIndicator;
    auto* loadingLabel = new QLabel("Loading data...");
    loadingLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(loadingIndicator);
    layout->addWidget(loadingLabel);
    
    dataCard->setContentWidget(loadingWidget);
};

// Show data
auto showData = [=](const QJsonObject& data) {
    auto* dataWidget = new QWidget;
    auto* layout = new QVBoxLayout(dataWidget);
    
    // Create data visualization
    auto* chartWidget = new FluentChartWidget;
    chartWidget->setData(data);
    
    layout->addWidget(chartWidget);
    
    dataCard->setContentWidget(dataWidget);
};

// Load data asynchronously
showLoading();
loadDataAsync([=](const QJsonObject& data) {
    showData(data);
});
```

## Best Practices

### Card Design Guidelines
1. **Keep content focused** - Each card should represent a single concept or task
2. **Use consistent sizing** - Maintain uniform card dimensions in grids
3. **Provide clear actions** - Make primary actions obvious and accessible
4. **Consider information hierarchy** - Use title, subtitle, and content appropriately
5. **Optimize for scanning** - Users should quickly understand card content

### Performance Considerations
- Use appropriate elevation levels (higher elevation = more rendering cost)
- Limit the number of animated cards visible simultaneously
- Consider virtualization for large lists of cards
- Cache complex content widgets when possible

### Accessibility Guidelines
- Provide meaningful titles and descriptions
- Ensure sufficient color contrast
- Support keyboard navigation
- Test with screen readers
- Use semantic markup for card content

## Common Patterns

### Dashboard Cards

```cpp
// Metric card pattern
auto createMetricCard = [](const QString& title, const QString& value, const QString& change) {
    auto* card = new FluentCard(title);
    
    auto* contentWidget = new QWidget;
    auto* layout = new QVBoxLayout(contentWidget);
    
    auto* valueLabel = new QLabel(value);
    valueLabel->setFont(QFont("Segoe UI", 24, QFont::Bold));
    
    auto* changeLabel = new QLabel(change);
    changeLabel->setStyleSheet(change.startsWith("+") ? "color: green;" : "color: red;");
    
    layout->addWidget(valueLabel);
    layout->addWidget(changeLabel);
    
    card->setContentWidget(contentWidget);
    card->setFixedSize(200, 120);
    
    return card;
};

// Usage
auto* salesCard = createMetricCard("Sales", "$12,345", "+5.2%");
auto* usersCard = createMetricCard("Users", "1,234", "+12.1%");
auto* revenueCard = createMetricCard("Revenue", "$45,678", "-2.3%");
```

## Signals Reference

```cpp
// Interaction signals
void clicked();
void doubleClicked();
void hovered(bool isHovered);

// Content change signals
void titleChanged(const QString& title);
void contentChanged();
```

## See Also

- [FluentPanel](FluentPanel.md) - For general container components
- [FluentButton](FluentButton.md) - For card action buttons
- [FluentNavigationView](FluentNavigationView.md) - For navigation with cards
- [API Reference - FluentCard](../API_REFERENCE.md#fluentcard) - Complete API documentation
