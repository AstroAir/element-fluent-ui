# FluentTabView Component

The `FluentTabView` component is an enhanced tab widget that implements Microsoft's Fluent Design principles. It provides smooth animations, customizable tab styles, and comprehensive tab management features.

## Overview

FluentTabView offers:
- **Multiple tab styles** (Standard, Pivot, Segmented)
- **Tab positioning** (Top, Bottom, Left, Right)
- **Closeable tabs** with confirmation dialogs
- **Drag and drop** tab reordering
- **Overflow handling** with scroll buttons
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Tab View

```cpp
#include "FluentQt/Components/FluentTabView.h"

// Create a basic tab view
auto* tabView = new FluentTabView;

// Add tabs with content
auto* homeWidget = new QWidget;
auto* settingsWidget = new QWidget;
auto* aboutWidget = new QWidget;

tabView->addTab(homeWidget, "Home");
tabView->addTab(settingsWidget, "Settings");
tabView->addTab(aboutWidget, "About");

// Handle tab changes
connect(tabView, &FluentTabView::currentChanged, [=](int index) {
    qDebug() << "Tab changed to:" << tabView->tabText(index);
});

// Add to layout
layout->addWidget(tabView);
```

### Tabs with Icons

```cpp
auto* tabView = new FluentTabView;

// Add tabs with icons
tabView->addTab(homeWidget, QIcon(":/icons/home"), "Home");
tabView->addTab(documentsWidget, QIcon(":/icons/documents"), "Documents");
tabView->addTab(settingsWidget, QIcon(":/icons/settings"), "Settings");

// Set tab icons separately
tabView->setTabIcon(0, QIcon(":/icons/home"));
tabView->setTabIcon(1, QIcon(":/icons/documents"));
```

## Tab Styles

### Standard Tabs

```cpp
auto* tabView = new FluentTabView;
tabView->setTabStyle(FluentTabStyle::Standard);

// Configure standard tab appearance
tabView->setTabShape(FluentTabShape::Rounded);
tabView->setTabHeight(32);
tabView->setTabMinimumWidth(80);
tabView->setTabMaximumWidth(200);
```

### Pivot Tabs

```cpp
auto* tabView = new FluentTabView;
tabView->setTabStyle(FluentTabStyle::Pivot);

// Pivot tabs are typically used for navigation
tabView->setTabPosition(FluentTabPosition::Top);
tabView->setShowTabBorders(false);
tabView->setTabSpacing(20);

// Pivot tabs often don't show content borders
tabView->setContentBorderVisible(false);
```

### Segmented Tabs

```cpp
auto* tabView = new FluentTabView;
tabView->setTabStyle(FluentTabStyle::Segmented);

// Segmented tabs appear as connected buttons
tabView->setTabCornerRadius(4);
tabView->setTabBorderWidth(1);
tabView->setSegmentedSpacing(0);  // No spacing between segments
```

## Tab Positioning

### Different Tab Positions

```cpp
auto* tabView = new FluentTabView;

// Position tabs at different locations
tabView->setTabPosition(FluentTabPosition::Top);     // Default
tabView->setTabPosition(FluentTabPosition::Bottom);
tabView->setTabPosition(FluentTabPosition::Left);
tabView->setTabPosition(FluentTabPosition::Right);

// Configure tab orientation for vertical positions
if (tabView->tabPosition() == FluentTabPosition::Left || 
    tabView->tabPosition() == FluentTabPosition::Right) {
    tabView->setTabTextOrientation(FluentTabTextOrientation::Horizontal);
    tabView->setTabIconPosition(FluentTabIconPosition::Top);
}
```

### Responsive Tab Layout

```cpp
auto* tabView = new FluentTabView;

// Enable responsive behavior
tabView->setResponsiveEnabled(true);
tabView->setResponsiveThreshold(600);  // Switch to bottom tabs below 600px

// Handle responsive changes
connect(tabView, &FluentTabView::responsiveModeChanged, [=](bool isMobile) {
    if (isMobile) {
        tabView->setTabPosition(FluentTabPosition::Bottom);
        tabView->setTabStyle(FluentTabStyle::Segmented);
    } else {
        tabView->setTabPosition(FluentTabPosition::Top);
        tabView->setTabStyle(FluentTabStyle::Standard);
    }
});
```

## Tab Management

### Adding and Removing Tabs

```cpp
auto* tabView = new FluentTabView;

// Add tabs
int homeIndex = tabView->addTab(homeWidget, "Home");
int settingsIndex = tabView->insertTab(1, settingsWidget, "Settings");

// Remove tabs
tabView->removeTab(settingsIndex);
tabView->removeTab(homeWidget);  // Remove by widget

// Clear all tabs
tabView->clear();

// Get tab information
int tabCount = tabView->count();
QWidget* currentWidget = tabView->currentWidget();
int currentIndex = tabView->currentIndex();
```

### Closeable Tabs

```cpp
auto* tabView = new FluentTabView;

// Enable closeable tabs
tabView->setTabsClosable(true);
tabView->setCloseButtonPosition(FluentTabCloseButtonPosition::Right);

// Handle tab close requests
connect(tabView, &FluentTabView::tabCloseRequested, [=](int index) {
    QString tabText = tabView->tabText(index);
    
    // Show confirmation dialog
    auto reply = QMessageBox::question(this, "Close Tab", 
        QString("Are you sure you want to close '%1'?").arg(tabText),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        tabView->removeTab(index);
    }
});

// Prevent specific tabs from being closed
connect(tabView, &FluentTabView::tabCloseRequested, [=](int index) {
    if (tabView->tabText(index) == "Home") {
        // Don't allow closing the Home tab
        return;
    }
    tabView->removeTab(index);
});
```

### Tab Reordering

```cpp
auto* tabView = new FluentTabView;

// Enable drag and drop reordering
tabView->setTabsMovable(true);
tabView->setDragDropMode(FluentTabDragDropMode::InternalMove);

// Handle tab moves
connect(tabView, &FluentTabView::tabMoved, [=](int from, int to) {
    qDebug() << "Tab moved from" << from << "to" << to;
    // Update any associated data structures
});

// Enable drag and drop between different tab views
tabView->setDragDropMode(FluentTabDragDropMode::DragDrop);
connect(tabView, &FluentTabView::tabDropped, [=](int index, FluentTabView* sourceTabView) {
    // Handle tab dropped from another tab view
    QWidget* widget = sourceTabView->widget(index);
    QString text = sourceTabView->tabText(index);
    QIcon icon = sourceTabView->tabIcon(index);
    
    sourceTabView->removeTab(index);
    tabView->addTab(widget, icon, text);
});
```

## Overflow Handling

### Tab Scrolling

```cpp
auto* tabView = new FluentTabView;

// Configure overflow behavior
tabView->setTabOverflowMode(FluentTabOverflowMode::Scroll);
tabView->setScrollButtonsVisible(true);
tabView->setScrollButtonPosition(FluentScrollButtonPosition::Ends);

// Smooth scrolling
tabView->setScrollAnimationEnabled(true);
tabView->setScrollAnimationDuration(250);

// Handle overflow state changes
connect(tabView, &FluentTabView::overflowStateChanged, [=](bool hasOverflow) {
    if (hasOverflow) {
        qDebug() << "Tabs are overflowing - scroll buttons visible";
    } else {
        qDebug() << "All tabs fit - scroll buttons hidden";
    }
});
```

### Tab Menu

```cpp
auto* tabView = new FluentTabView;

// Use dropdown menu for overflow
tabView->setTabOverflowMode(FluentTabOverflowMode::Menu);
tabView->setOverflowMenuButtonVisible(true);

// Customize overflow menu
connect(tabView, &FluentTabView::overflowMenuRequested, [=](QMenu* menu) {
    // Add custom actions to overflow menu
    menu->addSeparator();
    menu->addAction("Close All Tabs", [=]() {
        tabView->clear();
    });
    menu->addAction("Close Other Tabs", [=]() {
        int current = tabView->currentIndex();
        for (int i = tabView->count() - 1; i >= 0; --i) {
            if (i != current) {
                tabView->removeTab(i);
            }
        }
    });
});
```

## Animations

### Tab Transition Animations

```cpp
auto* tabView = new FluentTabView;

// Enable tab change animations
tabView->setAnimated(true);
tabView->setTabChangeAnimationDuration(300);
tabView->setTabChangeAnimationEasing(FluentEasing::CubicOut);

// Different animation types
tabView->setTabChangeAnimation(FluentTabAnimation::Slide);
tabView->setTabChangeAnimation(FluentTabAnimation::Fade);
tabView->setTabChangeAnimation(FluentTabAnimation::Push);

// Custom animations
connect(tabView, &FluentTabView::tabChanging, [=](int from, int to) {
    auto* animator = new FluentAnimator(this);
    
    QWidget* fromWidget = tabView->widget(from);
    QWidget* toWidget = tabView->widget(to);
    
    if (to > from) {
        // Sliding to the right
        animator->slideOutToLeft(fromWidget, 200);
        animator->slideInFromRight(toWidget, 200);
    } else {
        // Sliding to the left
        animator->slideOutToRight(fromWidget, 200);
        animator->slideInFromLeft(toWidget, 200);
    }
});
```

### Tab Hover Effects

```cpp
auto* tabView = new FluentTabView;

// Enable hover animations
tabView->setTabHoverAnimationEnabled(true);
tabView->setTabHoverAnimationDuration(150);

// Custom hover effects
connect(tabView, &FluentTabView::tabHovered, [=](int index, bool isHovered) {
    auto* animator = new FluentAnimator(this);
    QWidget* tabButton = tabView->tabButton(index);
    
    if (isHovered) {
        animator->scaleEffect(tabButton, 1.05, 150);
        animator->colorTransition(tabButton, "background-color", 
                                QColor(240, 240, 240), QColor(230, 230, 230), 150);
    } else {
        animator->scaleEffect(tabButton, 1.0, 150);
        animator->colorTransition(tabButton, "background-color", 
                                QColor(230, 230, 230), QColor(240, 240, 240), 150);
    }
});
```

## Customization

### Custom Tab Buttons

```cpp
auto* tabView = new FluentTabView;

// Create custom tab button
class CustomTabButton : public QWidget
{
public:
    CustomTabButton(const QString& text, const QIcon& icon, QWidget* parent = nullptr)
        : QWidget(parent) {
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(8, 4, 8, 4);
        
        m_iconLabel = new QLabel;
        m_iconLabel->setPixmap(icon.pixmap(16, 16));
        
        m_textLabel = new QLabel(text);
        m_textLabel->setFont(QFont("Segoe UI", 9));
        
        m_closeButton = new QPushButton("×");
        m_closeButton->setFixedSize(16, 16);
        m_closeButton->setStyleSheet("border: none; background: transparent;");
        
        layout->addWidget(m_iconLabel);
        layout->addWidget(m_textLabel);
        layout->addWidget(m_closeButton);
        
        connect(m_closeButton, &QPushButton::clicked, this, &CustomTabButton::closeRequested);
    }
    
signals:
    void closeRequested();

private:
    QLabel* m_iconLabel{nullptr};
    QLabel* m_textLabel{nullptr};
    QPushButton* m_closeButton{nullptr};
};

// Use custom tab button
auto* customButton = new CustomTabButton("Custom Tab", QIcon(":/icons/custom"));
int index = tabView->addTab(contentWidget, "");  // Empty text since button has its own
tabView->setTabButton(index, customButton);

connect(customButton, &CustomTabButton::closeRequested, [=]() {
    tabView->removeTab(index);
});
```

### Tab Styling

```cpp
auto* tabView = new FluentTabView;

// Customize tab appearance
tabView->setTabBackgroundColor(QColor(240, 240, 240));
tabView->setTabSelectedBackgroundColor(QColor(255, 255, 255));
tabView->setTabHoverBackgroundColor(QColor(230, 230, 230));

tabView->setTabTextColor(QColor(50, 50, 50));
tabView->setTabSelectedTextColor(QColor(0, 0, 0));

tabView->setTabBorderColor(QColor(200, 200, 200));
tabView->setTabSelectedBorderColor(QColor(0, 120, 215));

// Custom tab stylesheet
tabView->setStyleSheet(R"(
    FluentTabView::tab {
        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                   stop: 0 #f0f0f0, stop: 1 #e0e0e0);
        border: 1px solid #c0c0c0;
        border-bottom: none;
        border-radius: 4px 4px 0 0;
        padding: 6px 12px;
        margin-right: 2px;
    }
    
    FluentTabView::tab:selected {
        background: white;
        border-color: #0078d4;
        border-bottom: 2px solid #0078d4;
    }
    
    FluentTabView::tab:hover {
        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                   stop: 0 #e8e8e8, stop: 1 #d8d8d8);
    }
)");
```

## Integration Examples

### Document Editor with Tabs

```cpp
class TabbedDocumentEditor : public QMainWindow
{
public:
    TabbedDocumentEditor(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupConnections();
    }
    
    void openDocument(const QString& filePath) {
        // Check if document is already open
        for (int i = 0; i < m_tabView->count(); ++i) {
            auto* editor = qobject_cast<FluentRichTextEditor*>(m_tabView->widget(i));
            if (editor && editor->filePath() == filePath) {
                m_tabView->setCurrentIndex(i);
                return;
            }
        }
        
        // Create new editor for document
        auto* editor = new FluentRichTextEditor;
        editor->loadFromFile(filePath);
        
        QString fileName = QFileInfo(filePath).baseName();
        int index = m_tabView->addTab(editor, fileName);
        m_tabView->setCurrentIndex(index);
        
        // Set up editor connections
        connect(editor, &FluentRichTextEditor::modificationChanged, [=](bool modified) {
            updateTabTitle(editor, modified);
        });
    }

private:
    void setupUI() {
        m_tabView = new FluentTabView;
        m_tabView->setTabsClosable(true);
        m_tabView->setTabsMovable(true);
        m_tabView->setTabStyle(FluentTabStyle::Standard);
        
        setCentralWidget(m_tabView);
        
        // Create initial "Welcome" tab
        auto* welcomeWidget = createWelcomeWidget();
        m_tabView->addTab(welcomeWidget, "Welcome");
    }
    
    void setupConnections() {
        // Handle tab close requests
        connect(m_tabView, &FluentTabView::tabCloseRequested, [=](int index) {
            closeTab(index);
        });
        
        // Handle tab changes
        connect(m_tabView, &FluentTabView::currentChanged, [=](int index) {
            updateMenusAndToolbars(index);
        });
    }
    
    void closeTab(int index) {
        auto* editor = qobject_cast<FluentRichTextEditor*>(m_tabView->widget(index));
        if (editor && editor->isModified()) {
            QString fileName = m_tabView->tabText(index);
            auto reply = QMessageBox::question(this, "Save Changes",
                QString("Save changes to '%1'?").arg(fileName),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            
            if (reply == QMessageBox::Save) {
                if (saveDocument(editor)) {
                    m_tabView->removeTab(index);
                }
            } else if (reply == QMessageBox::Discard) {
                m_tabView->removeTab(index);
            }
            // Cancel - do nothing
        } else {
            m_tabView->removeTab(index);
        }
    }
    
    void updateTabTitle(FluentRichTextEditor* editor, bool modified) {
        int index = m_tabView->indexOf(editor);
        if (index >= 0) {
            QString title = QFileInfo(editor->filePath()).baseName();
            if (modified) title += "*";
            m_tabView->setTabText(index, title);
        }
    }

private:
    FluentTabView* m_tabView{nullptr};
};
```

### Settings Dialog with Tabs

```cpp
class SettingsDialog : public QDialog
{
public:
    SettingsDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setupUI();
        loadSettings();
    }

private:
    void setupUI() {
        setWindowTitle("Settings");
        setFixedSize(600, 400);
        
        auto* layout = new QVBoxLayout(this);
        
        // Tab view with different settings categories
        m_tabView = new FluentTabView;
        m_tabView->setTabStyle(FluentTabStyle::Pivot);
        m_tabView->setTabPosition(FluentTabPosition::Left);
        
        // General settings
        auto* generalWidget = createGeneralSettingsWidget();
        m_tabView->addTab(generalWidget, QIcon(":/icons/general"), "General");
        
        // Appearance settings
        auto* appearanceWidget = createAppearanceSettingsWidget();
        m_tabView->addTab(appearanceWidget, QIcon(":/icons/appearance"), "Appearance");
        
        // Advanced settings
        auto* advancedWidget = createAdvancedSettingsWidget();
        m_tabView->addTab(advancedWidget, QIcon(":/icons/advanced"), "Advanced");
        
        // Buttons
        auto* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch();
        
        auto* okButton = FluentButton::createPrimaryButton("OK");
        auto* cancelButton = new FluentButton("Cancel");
        auto* applyButton = new FluentButton("Apply");
        
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(applyButton);
        
        layout->addWidget(m_tabView);
        layout->addLayout(buttonLayout);
        
        // Connect buttons
        connect(okButton, &FluentButton::clicked, this, &SettingsDialog::accept);
        connect(cancelButton, &FluentButton::clicked, this, &SettingsDialog::reject);
        connect(applyButton, &FluentButton::clicked, this, &SettingsDialog::applySettings);
    }

private:
    FluentTabView* m_tabView{nullptr};
};
```

## Accessibility

FluentTabView provides comprehensive accessibility support:

```cpp
auto* tabView = new FluentTabView;

// Accessible properties (automatically set)
tabView->setAccessibleName("Document tabs");
tabView->setAccessibleDescription("Navigate between open documents");

// Keyboard navigation (automatic)
// - Ctrl+Tab to cycle through tabs
// - Ctrl+Shift+Tab to cycle backwards
// - Ctrl+W to close current tab
// - Arrow keys to navigate tabs when focused

// Screen reader support
tabView->setAccessibleRole(QAccessible::PageTabList);

// Custom accessibility for tab content
connect(tabView, &FluentTabView::currentChanged, [=](int index) {
    QString announcement = QString("Switched to tab %1: %2")
        .arg(index + 1)
        .arg(tabView->tabText(index));
    tabView->announceToScreenReader(announcement);
});
```

## Best Practices

### Tab Design Guidelines
1. **Use clear tab labels** - Make tab purposes obvious
2. **Limit tab count** - Consider overflow handling for many tabs
3. **Provide close options** - Allow users to manage tabs
4. **Show unsaved changes** - Indicate modified content
5. **Support keyboard navigation** - Enable efficient tab switching

### Performance Considerations
- Use lazy loading for tab content
- Implement virtual tabs for very large numbers
- Cache tab content when switching frequently
- Optimize animations for smooth performance

### Accessibility Guidelines
- Support keyboard navigation
- Provide screen reader announcements
- Use sufficient color contrast
- Include meaningful tab labels
- Test with assistive technologies

## Signals Reference

```cpp
// Tab change signals
void currentChanged(int index);
void tabChanged(int index);
void tabChanging(int from, int to);

// Tab management signals
void tabCloseRequested(int index);
void tabMoved(int from, int to);
void tabDropped(int index, FluentTabView* source);

// Interaction signals
void tabHovered(int index, bool isHovered);
void tabDoubleClicked(int index);
```

## See Also

- [FluentNavigationView](FluentNavigationView.md) - For sidebar navigation
- [FluentCard](FluentCard.md) - For organizing tab content
- [FluentButton](FluentButton.md) - For tab close buttons
- [API Reference - FluentTabView](../API_REFERENCE.md#fluenttabview) - Complete API documentation
