# FluentNavigationView Component

The `FluentNavigationView` component provides a modern navigation interface following Microsoft's Fluent Design principles. It offers flexible navigation patterns including sidebar, top navigation, and adaptive layouts.

## Overview

FluentNavigationView offers:
- **Multiple navigation modes** (Sidebar, Top, Adaptive)
- **Hierarchical navigation** with nested menu items
- **Smooth animations** for transitions and state changes
- **Responsive design** that adapts to different screen sizes
- **Icon and text support** for navigation items
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Sidebar Navigation

```cpp
#include "FluentQt/Components/FluentNavigationView.h"

// Create navigation view
auto* navView = new FluentNavigationView;
navView->setNavigationMode(FluentNavigationMode::Sidebar);

// Add navigation items
navView->addNavigationItem("Home", QIcon(":/icons/home"));
navView->addNavigationItem("Documents", QIcon(":/icons/documents"));
navView->addNavigationItem("Pictures", QIcon(":/icons/pictures"));
navView->addNavigationItem("Settings", QIcon(":/icons/settings"));

// Set as main widget
setCentralWidget(navView);

// Handle navigation
connect(navView, &FluentNavigationView::itemSelected, [=](const QString& itemId) {
    qDebug() << "Selected:" << itemId;
    // Navigate to the selected page
});
```

### Navigation with Content Pages

```cpp
auto* navView = new FluentNavigationView;

// Create content pages
auto* homePage = new QWidget;
auto* documentsPage = new QWidget;
auto* settingsPage = new QWidget;

// Add navigation items with associated content
navView->addNavigationItem("home", "Home", QIcon(":/icons/home"), homePage);
navView->addNavigationItem("docs", "Documents", QIcon(":/icons/documents"), documentsPage);
navView->addNavigationItem("settings", "Settings", QIcon(":/icons/settings"), settingsPage);

// Set initial page
navView->setCurrentItem("home");
```

## Navigation Modes

### Sidebar Mode

```cpp
auto* navView = new FluentNavigationView;
navView->setNavigationMode(FluentNavigationMode::Sidebar);

// Configure sidebar
navView->setSidebarWidth(280);
navView->setCollapsible(true);  // Allow sidebar collapse
navView->setAutoCollapse(true); // Auto-collapse on small screens

// Sidebar can be expanded/collapsed
navView->setSidebarExpanded(true);
```

### Top Navigation Mode

```cpp
auto* navView = new FluentNavigationView;
navView->setNavigationMode(FluentNavigationMode::Top);

// Configure top navigation
navView->setTopNavigationHeight(48);
navView->setShowNavigationText(true);  // Show text labels
navView->setShowNavigationIcons(true); // Show icons
```

### Adaptive Mode

```cpp
auto* navView = new FluentNavigationView;
navView->setNavigationMode(FluentNavigationMode::Adaptive);

// Configure adaptive behavior
navView->setAdaptiveThreshold(768);  // Switch to top nav below 768px width
navView->setCompactThreshold(480);   // Use compact mode below 480px width

// The navigation will automatically switch between modes based on window size
```

## Hierarchical Navigation

### Nested Menu Items

```cpp
auto* navView = new FluentNavigationView;

// Add top-level items
auto* homeItem = navView->addNavigationItem("home", "Home", QIcon(":/icons/home"));
auto* projectsItem = navView->addNavigationItem("projects", "Projects", QIcon(":/icons/projects"));

// Add sub-items to Projects
auto* activeProjects = navView->addNavigationSubItem(projectsItem, "active", "Active Projects");
auto* archivedProjects = navView->addNavigationSubItem(projectsItem, "archived", "Archived Projects");
auto* newProject = navView->addNavigationSubItem(projectsItem, "new", "New Project");

// Add nested sub-items
navView->addNavigationSubItem(activeProjects, "project1", "Project Alpha");
navView->addNavigationSubItem(activeProjects, "project2", "Project Beta");
navView->addNavigationSubItem(activeProjects, "project3", "Project Gamma");

// Handle hierarchical navigation
connect(navView, &FluentNavigationView::itemSelected, [=](const QString& itemId) {
    if (itemId == "project1") {
        // Navigate to Project Alpha
        showProjectDetails("alpha");
    } else if (itemId == "project2") {
        // Navigate to Project Beta
        showProjectDetails("beta");
    }
    // ... handle other items
});
```

### Breadcrumb Integration

```cpp
auto* navView = new FluentNavigationView;
navView->setBreadcrumbEnabled(true);

// Breadcrumb automatically updates based on current navigation path
connect(navView, &FluentNavigationView::breadcrumbChanged, [=](const QStringList& path) {
    qDebug() << "Current path:" << path.join(" > ");
});

// Manually set breadcrumb path
navView->setBreadcrumbPath({"Home", "Projects", "Active Projects", "Project Alpha"});
```

## Customization

### Custom Navigation Items

```cpp
// Create custom navigation item widget
auto createCustomItem = [](const QString& title, const QString& subtitle, const QIcon& icon) {
    auto* widget = new QWidget;
    auto* layout = new QHBoxLayout(widget);
    
    auto* iconLabel = new QLabel;
    iconLabel->setPixmap(icon.pixmap(24, 24));
    
    auto* textWidget = new QWidget;
    auto* textLayout = new QVBoxLayout(textWidget);
    textLayout->setContentsMargins(0, 0, 0, 0);
    
    auto* titleLabel = new QLabel(title);
    titleLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    
    auto* subtitleLabel = new QLabel(subtitle);
    subtitleLabel->setFont(QFont("Segoe UI", 8));
    subtitleLabel->setStyleSheet("color: #666;");
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(subtitleLabel);
    
    layout->addWidget(iconLabel);
    layout->addWidget(textWidget);
    layout->addStretch();
    
    return widget;
};

auto* navView = new FluentNavigationView;

// Add custom navigation items
auto* dashboardWidget = createCustomItem("Dashboard", "Overview & Analytics", QIcon(":/icons/dashboard"));
auto* reportsWidget = createCustomItem("Reports", "Data & Insights", QIcon(":/icons/reports"));

navView->addCustomNavigationItem("dashboard", dashboardWidget);
navView->addCustomNavigationItem("reports", reportsWidget);
```

### Header and Footer Areas

```cpp
auto* navView = new FluentNavigationView;

// Add header area (above navigation items)
auto* headerWidget = new QWidget;
auto* headerLayout = new QVBoxLayout(headerWidget);

auto* logoLabel = new QLabel;
logoLabel->setPixmap(QPixmap(":/images/logo.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
logoLabel->setAlignment(Qt::AlignCenter);

auto* appNameLabel = new QLabel("My Application");
appNameLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
appNameLabel->setAlignment(Qt::AlignCenter);

headerLayout->addWidget(logoLabel);
headerLayout->addWidget(appNameLabel);

navView->setHeaderWidget(headerWidget);

// Add footer area (below navigation items)
auto* footerWidget = new QWidget;
auto* footerLayout = new QVBoxLayout(footerWidget);

auto* userWidget = new QWidget;
auto* userLayout = new QHBoxLayout(userWidget);

auto* avatarLabel = new QLabel;
avatarLabel->setPixmap(QPixmap(":/images/avatar.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));

auto* userNameLabel = new QLabel("John Doe");
userNameLabel->setFont(QFont("Segoe UI", 9));

userLayout->addWidget(avatarLabel);
userLayout->addWidget(userNameLabel);
userLayout->addStretch();

footerLayout->addWidget(userWidget);

navView->setFooterWidget(footerWidget);
```

## Animations and Transitions

### Page Transitions

```cpp
auto* navView = new FluentNavigationView;
navView->setPageTransitionEnabled(true);
navView->setPageTransitionDuration(300);
navView->setPageTransitionEasing(FluentEasing::CubicOut);

// Custom transition animations
connect(navView, &FluentNavigationView::pageChanging, [=](const QString& fromPage, const QString& toPage) {
    // Custom transition logic
    auto* animator = new FluentAnimator(this);
    
    // Slide out current page
    auto* currentWidget = navView->currentContentWidget();
    if (currentWidget) {
        animator->slideOutToLeft(currentWidget, 200);
    }
});

connect(navView, &FluentNavigationView::pageChanged, [=](const QString& pageId) {
    // Slide in new page
    auto* animator = new FluentAnimator(this);
    auto* newWidget = navView->currentContentWidget();
    if (newWidget) {
        animator->slideInFromRight(newWidget, 200);
    }
});
```

### Navigation Item Animations

```cpp
auto* navView = new FluentNavigationView;

// Enable hover animations
navView->setItemHoverAnimationEnabled(true);

// Enable selection animations
navView->setItemSelectionAnimationEnabled(true);

// Custom item animations
connect(navView, &FluentNavigationView::itemHovered, [=](const QString& itemId, bool isHovered) {
    auto* item = navView->getNavigationItem(itemId);
    if (item && isHovered) {
        auto* animator = new FluentAnimator(this);
        animator->scaleEffect(item, 1.05, 150);
    }
});
```

## Responsive Design

### Adaptive Layout

```cpp
auto* navView = new FluentNavigationView;
navView->setNavigationMode(FluentNavigationMode::Adaptive);

// Configure responsive breakpoints
navView->setAdaptiveThreshold(768);   // Switch to top nav
navView->setCompactThreshold(480);    // Use compact mode
navView->setMobileThreshold(320);     // Mobile optimizations

// Handle responsive changes
connect(navView, &FluentNavigationView::navigationModeChanged, [=](FluentNavigationMode mode) {
    switch (mode) {
        case FluentNavigationMode::Sidebar:
            qDebug() << "Switched to sidebar navigation";
            break;
        case FluentNavigationMode::Top:
            qDebug() << "Switched to top navigation";
            break;
        case FluentNavigationMode::Compact:
            qDebug() << "Switched to compact navigation";
            break;
    }
});
```

### Mobile Optimizations

```cpp
auto* navView = new FluentNavigationView;

// Enable mobile-specific features
navView->setMobileOptimizationsEnabled(true);
navView->setTouchFriendly(true);
navView->setSwipeGesturesEnabled(true);

// Configure mobile navigation
navView->setMobileNavigationStyle(FluentMobileNavigationStyle::BottomTabs);
navView->setMobileNavigationHeight(56);

// Handle swipe gestures
connect(navView, &FluentNavigationView::swipeDetected, [=](FluentSwipeDirection direction) {
    if (direction == FluentSwipeDirection::Left) {
        navView->navigateToNextItem();
    } else if (direction == FluentSwipeDirection::Right) {
        navView->navigateToPreviousItem();
    }
});
```

## Accessibility

FluentNavigationView provides comprehensive accessibility support:

```cpp
auto* navView = new FluentNavigationView;

// Accessible properties (automatically set)
navView->setAccessibleName("Main navigation");
navView->setAccessibleDescription("Navigate between different sections of the application");

// Keyboard navigation (automatic)
// - Tab to navigate between items
// - Arrow keys for hierarchical navigation
// - Enter/Space to select items
// - Escape to collapse expanded items

// Screen reader support
navView->setAccessibleRole(QAccessible::MenuBar);

// Custom accessibility for navigation items
auto* homeItem = navView->addNavigationItem("home", "Home", QIcon(":/icons/home"));
homeItem->setAccessibleName("Home page");
homeItem->setAccessibleDescription("Navigate to the home page");
```

## Integration Examples

### Complete Application Layout

```cpp
class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupNavigation();
        setupContent();
        setupConnections();
    }

private:
    void setupNavigation() {
        m_navView = new FluentNavigationView;
        m_navView->setNavigationMode(FluentNavigationMode::Adaptive);
        
        // Add navigation items
        m_navView->addNavigationItem("dashboard", "Dashboard", QIcon(":/icons/dashboard"), createDashboardPage());
        m_navView->addNavigationItem("projects", "Projects", QIcon(":/icons/projects"), createProjectsPage());
        m_navView->addNavigationItem("team", "Team", QIcon(":/icons/team"), createTeamPage());
        m_navView->addNavigationItem("settings", "Settings", QIcon(":/icons/settings"), createSettingsPage());
        
        setCentralWidget(m_navView);
    }
    
    void setupContent() {
        // Content pages are created and managed by navigation view
        m_navView->setCurrentItem("dashboard");
    }
    
    void setupConnections() {
        connect(m_navView, &FluentNavigationView::itemSelected, this, &MainWindow::handleNavigation);
    }
    
    void handleNavigation(const QString& itemId) {
        // Handle navigation logic
        if (itemId == "dashboard") {
            updateDashboard();
        } else if (itemId == "projects") {
            loadProjects();
        } else if (itemId == "team") {
            loadTeamMembers();
        } else if (itemId == "settings") {
            loadSettings();
        }
    }
    
    QWidget* createDashboardPage() {
        auto* page = new QWidget;
        auto* layout = new QVBoxLayout(page);
        
        auto* titleLabel = new QLabel("Dashboard");
        titleLabel->setFont(QFont("Segoe UI", 18, QFont::Bold));
        
        auto* metricsWidget = createMetricsWidget();
        auto* chartsWidget = createChartsWidget();
        
        layout->addWidget(titleLabel);
        layout->addWidget(metricsWidget);
        layout->addWidget(chartsWidget);
        layout->addStretch();
        
        return page;
    }
    
    // ... other page creation methods
    
private:
    FluentNavigationView* m_navView{nullptr};
};
```

## Best Practices

### Navigation Design Guidelines
1. **Keep navigation shallow** - Avoid deep hierarchies (max 3 levels)
2. **Use clear labels** - Navigation items should be self-explanatory
3. **Provide visual feedback** - Show current location and state
4. **Consider context** - Group related functionality together
5. **Test on different screen sizes** - Ensure responsive behavior works well

### Performance Considerations
- Lazy load content pages to improve startup time
- Use virtual scrolling for large navigation lists
- Optimize animations for smooth performance
- Cache frequently accessed pages

### Accessibility Guidelines
- Provide keyboard navigation support
- Use semantic markup and ARIA roles
- Ensure sufficient color contrast
- Test with screen readers
- Support high contrast themes

## Signals Reference

```cpp
// Navigation signals
void itemSelected(const QString& itemId);
void itemHovered(const QString& itemId, bool isHovered);
void pageChanged(const QString& pageId);
void pageChanging(const QString& fromPage, const QString& toPage);

// Layout signals
void navigationModeChanged(FluentNavigationMode mode);
void sidebarExpandedChanged(bool expanded);
void breadcrumbChanged(const QStringList& path);

// Gesture signals (mobile)
void swipeDetected(FluentSwipeDirection direction);
```

## See Also

- [FluentTabView](FluentTabView.md) - For tab-based navigation
- [FluentBreadcrumb](FluentBreadcrumb.md) - For breadcrumb navigation
- [FluentCard](FluentCard.md) - For content organization
- [API Reference - FluentNavigationView](../API_REFERENCE.md#fluentnavigationview) - Complete API documentation
