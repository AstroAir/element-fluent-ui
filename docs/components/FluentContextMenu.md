# FluentContextMenu Component

The `FluentContextMenu` component is an enhanced context menu widget that implements Microsoft's Fluent Design principles. It provides smooth animations, rich content support, and comprehensive customization options for right-click menus.

## Overview

FluentContextMenu offers:
- **Rich menu items** with icons, descriptions, and shortcuts
- **Smooth animations** for show/hide transitions
- **Submenu support** with cascading menus
- **Custom widgets** as menu items
- **Separator styles** and grouping
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Context Menu

```cpp
#include "FluentQt/Components/FluentContextMenu.h"

// Create a basic context menu
auto* contextMenu = new FluentContextMenu(this);

// Add menu items
contextMenu->addAction("Cut", [=]() {
    // Cut action
});

contextMenu->addAction("Copy", [=]() {
    // Copy action
});

contextMenu->addAction("Paste", [=]() {
    // Paste action
});

// Show context menu on right-click
widget->setContextMenuPolicy(Qt::CustomContextMenu);
connect(widget, &QWidget::customContextMenuRequested, [=](const QPoint& pos) {
    contextMenu->exec(widget->mapToGlobal(pos));
});
```

### Context Menu with Icons

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Add actions with icons
auto* cutAction = contextMenu->addAction(QIcon(":/icons/cut"), "Cut");
auto* copyAction = contextMenu->addAction(QIcon(":/icons/copy"), "Copy");
auto* pasteAction = contextMenu->addAction(QIcon(":/icons/paste"), "Paste");

// Set keyboard shortcuts
cutAction->setShortcut(QKeySequence::Cut);
copyAction->setShortcut(QKeySequence::Copy);
pasteAction->setShortcut(QKeySequence::Paste);

// Connect actions
connect(cutAction, &QAction::triggered, this, &MyWidget::cut);
connect(copyAction, &QAction::triggered, this, &MyWidget::copy);
connect(pasteAction, &QAction::triggered, this, &MyWidget::paste);
```

## Rich Menu Items

### Menu Items with Descriptions

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Add items with descriptions
auto* newAction = contextMenu->addAction(QIcon(":/icons/new"), "New Document");
newAction->setDescription("Create a new document");

auto* openAction = contextMenu->addAction(QIcon(":/icons/open"), "Open");
openAction->setDescription("Open an existing document");

auto* saveAction = contextMenu->addAction(QIcon(":/icons/save"), "Save");
saveAction->setDescription("Save the current document");
saveAction->setShortcut(QKeySequence::Save);

// Enable rich display
contextMenu->setShowDescriptions(true);
contextMenu->setShowShortcuts(true);
```

### Custom Menu Items

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Create custom menu item widget
class CustomMenuItem : public QWidget
{
public:
    CustomMenuItem(const QString& title, const QString& subtitle, 
                   const QIcon& icon, QWidget* parent = nullptr)
        : QWidget(parent) {
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(8, 4, 8, 4);
        
        // Icon
        auto* iconLabel = new QLabel;
        iconLabel->setPixmap(icon.pixmap(24, 24));
        
        // Text content
        auto* textWidget = new QWidget;
        auto* textLayout = new QVBoxLayout(textWidget);
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(1);
        
        auto* titleLabel = new QLabel(title);
        titleLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
        
        auto* subtitleLabel = new QLabel(subtitle);
        subtitleLabel->setFont(QFont("Segoe UI", 8));
        subtitleLabel->setStyleSheet("color: #666;");
        
        textLayout->addWidget(titleLabel);
        textLayout->addWidget(subtitleLabel);
        
        layout->addWidget(iconLabel);
        layout->addWidget(textWidget);
        layout->addStretch();
    }
};

// Add custom widget as menu item
auto* customItem = new CustomMenuItem("Recent Files", 
    "Access recently opened documents", QIcon(":/icons/recent"));
contextMenu->addWidget(customItem);

// Handle custom item clicks
customItem->installEventFilter(this);
```

## Submenus

### Creating Submenus

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Main menu items
contextMenu->addAction(QIcon(":/icons/new"), "New");
contextMenu->addAction(QIcon(":/icons/open"), "Open");

// Create submenu
auto* recentMenu = contextMenu->addMenu(QIcon(":/icons/recent"), "Recent Files");

// Add items to submenu
recentMenu->addAction("Document1.txt");
recentMenu->addAction("Document2.txt");
recentMenu->addAction("Document3.txt");
recentMenu->addSeparator();
recentMenu->addAction("Clear Recent Files");

// Nested submenus
auto* templatesMenu = contextMenu->addMenu(QIcon(":/icons/template"), "New from Template");
auto* documentTemplates = templatesMenu->addMenu("Documents");
documentTemplates->addAction("Blank Document");
documentTemplates->addAction("Letter Template");
documentTemplates->addAction("Report Template");

auto* presentationTemplates = templatesMenu->addMenu("Presentations");
presentationTemplates->addAction("Blank Presentation");
presentationTemplates->addAction("Business Template");
```

### Dynamic Submenus

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Create dynamic submenu
auto* recentMenu = contextMenu->addMenu(QIcon(":/icons/recent"), "Recent Files");

// Populate submenu dynamically when shown
connect(recentMenu, &FluentContextMenu::aboutToShow, [=]() {
    recentMenu->clear();
    
    QStringList recentFiles = getRecentFiles();
    if (recentFiles.isEmpty()) {
        auto* noFilesAction = recentMenu->addAction("No recent files");
        noFilesAction->setEnabled(false);
    } else {
        for (const QString& file : recentFiles) {
            QString fileName = QFileInfo(file).baseName();
            auto* action = recentMenu->addAction(QIcon(":/icons/document"), fileName);
            action->setData(file);  // Store full path
            
            connect(action, &QAction::triggered, [=]() {
                openFile(action->data().toString());
            });
        }
        
        recentMenu->addSeparator();
        recentMenu->addAction("Clear Recent Files", [=]() {
            clearRecentFiles();
        });
    }
});
```

## Separators and Grouping

### Different Separator Styles

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Standard separator
contextMenu->addAction("Action 1");
contextMenu->addSeparator();
contextMenu->addAction("Action 2");

// Custom separator with text
contextMenu->addSeparatorWithText("Edit Actions");
contextMenu->addAction("Cut");
contextMenu->addAction("Copy");
contextMenu->addAction("Paste");

// Themed separator
contextMenu->addThemedSeparator();
contextMenu->addAction("Delete");

// Invisible separator (spacing)
contextMenu->addSpacing(10);
contextMenu->addAction("Properties");
```

### Grouped Menu Items

```cpp
auto* contextMenu = new FluentContextMenu(this);

// File operations group
contextMenu->beginGroup("File Operations");
contextMenu->addAction(QIcon(":/icons/new"), "New");
contextMenu->addAction(QIcon(":/icons/open"), "Open");
contextMenu->addAction(QIcon(":/icons/save"), "Save");
contextMenu->endGroup();

// Edit operations group
contextMenu->beginGroup("Edit Operations");
contextMenu->addAction(QIcon(":/icons/cut"), "Cut");
contextMenu->addAction(QIcon(":/icons/copy"), "Copy");
contextMenu->addAction(QIcon(":/icons/paste"), "Paste");
contextMenu->endGroup();

// View operations group
contextMenu->beginGroup("View Options");
auto* zoomInAction = contextMenu->addAction(QIcon(":/icons/zoom-in"), "Zoom In");
auto* zoomOutAction = contextMenu->addAction(QIcon(":/icons/zoom-out"), "Zoom Out");
auto* fitToWindowAction = contextMenu->addAction(QIcon(":/icons/fit"), "Fit to Window");
contextMenu->endGroup();
```

## Animations

### Show/Hide Animations

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Enable smooth animations
contextMenu->setAnimated(true);
contextMenu->setShowAnimationDuration(200);
contextMenu->setHideAnimationDuration(150);
contextMenu->setAnimationEasing(FluentEasing::CubicOut);

// Different animation types
contextMenu->setShowAnimation(FluentMenuAnimation::Fade);
contextMenu->setShowAnimation(FluentMenuAnimation::Scale);
contextMenu->setShowAnimation(FluentMenuAnimation::Slide);

// Custom animations
connect(contextMenu, &FluentContextMenu::aboutToShow, [=]() {
    auto* animator = new FluentAnimator(this);
    animator->fadeIn(contextMenu, 200);
    animator->scaleEffect(contextMenu, 0.9, 1.0, 200);
});
```

### Item Hover Effects

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Enable hover animations
contextMenu->setItemHoverAnimationEnabled(true);
contextMenu->setItemHoverAnimationDuration(150);

// Custom hover effects
connect(contextMenu, &FluentContextMenu::itemHovered, [=](QAction* action, bool isHovered) {
    auto* animator = new FluentAnimator(this);
    QWidget* itemWidget = contextMenu->actionWidget(action);
    
    if (itemWidget && isHovered) {
        animator->colorTransition(itemWidget, "background-color",
                                QColor(240, 240, 240), QColor(230, 240, 250), 150);
    }
});
```

## Integration Examples

### File Explorer Context Menu

```cpp
class FileExplorerContextMenu : public FluentContextMenu
{
public:
    FileExplorerContextMenu(const QFileInfo& fileInfo, QWidget* parent = nullptr)
        : FluentContextMenu(parent), m_fileInfo(fileInfo) {
        setupMenu();
    }

private:
    void setupMenu() {
        if (m_fileInfo.isFile()) {
            setupFileMenu();
        } else if (m_fileInfo.isDir()) {
            setupDirectoryMenu();
        }
        
        addSeparator();
        setupCommonActions();
    }
    
    void setupFileMenu() {
        // File-specific actions
        auto* openAction = addAction(QIcon(":/icons/open"), "Open");
        connect(openAction, &QAction::triggered, [=]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(m_fileInfo.absoluteFilePath()));
        });
        
        // Open with submenu
        auto* openWithMenu = addMenu(QIcon(":/icons/open-with"), "Open with");
        populateOpenWithMenu(openWithMenu);
        
        addSeparator();
        
        // Edit actions
        if (isTextFile(m_fileInfo)) {
            auto* editAction = addAction(QIcon(":/icons/edit"), "Edit");
            connect(editAction, &QAction::triggered, [=]() {
                openInTextEditor(m_fileInfo.absoluteFilePath());
            });
        }
        
        if (isImageFile(m_fileInfo)) {
            auto* viewAction = addAction(QIcon(":/icons/image"), "View");
            connect(viewAction, &QAction::triggered, [=]() {
                openInImageViewer(m_fileInfo.absoluteFilePath());
            });
        }
    }
    
    void setupDirectoryMenu() {
        // Directory-specific actions
        auto* openAction = addAction(QIcon(":/icons/folder-open"), "Open");
        connect(openAction, &QAction::triggered, [=]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(m_fileInfo.absoluteFilePath()));
        });
        
        auto* openInTerminalAction = addAction(QIcon(":/icons/terminal"), "Open in Terminal");
        connect(openInTerminalAction, &QAction::triggered, [=]() {
            openInTerminal(m_fileInfo.absoluteFilePath());
        });
        
        addSeparator();
        
        // New items submenu
        auto* newMenu = addMenu(QIcon(":/icons/new"), "New");
        newMenu->addAction(QIcon(":/icons/folder"), "Folder", [=]() {
            createNewFolder(m_fileInfo.absoluteFilePath());
        });
        newMenu->addAction(QIcon(":/icons/document"), "Text Document", [=]() {
            createNewTextFile(m_fileInfo.absoluteFilePath());
        });
    }
    
    void setupCommonActions() {
        // Common actions for both files and directories
        auto* cutAction = addAction(QIcon(":/icons/cut"), "Cut");
        cutAction->setShortcut(QKeySequence::Cut);
        connect(cutAction, &QAction::triggered, [=]() {
            cutToClipboard(m_fileInfo.absoluteFilePath());
        });
        
        auto* copyAction = addAction(QIcon(":/icons/copy"), "Copy");
        copyAction->setShortcut(QKeySequence::Copy);
        connect(copyAction, &QAction::triggered, [=]() {
            copyToClipboard(m_fileInfo.absoluteFilePath());
        });
        
        auto* deleteAction = addAction(QIcon(":/icons/delete"), "Delete");
        deleteAction->setShortcut(QKeySequence::Delete);
        connect(deleteAction, &QAction::triggered, [=]() {
            deleteFile(m_fileInfo.absoluteFilePath());
        });
        
        addSeparator();
        
        auto* renameAction = addAction(QIcon(":/icons/rename"), "Rename");
        renameAction->setShortcut(Qt::Key_F2);
        connect(renameAction, &QAction::triggered, [=]() {
            renameFile(m_fileInfo.absoluteFilePath());
        });
        
        addSeparator();
        
        auto* propertiesAction = addAction(QIcon(":/icons/properties"), "Properties");
        connect(propertiesAction, &QAction::triggered, [=]() {
            showProperties(m_fileInfo.absoluteFilePath());
        });
    }
    
    void populateOpenWithMenu(FluentContextMenu* menu) {
        // Get associated applications
        QStringList applications = getAssociatedApplications(m_fileInfo.suffix());
        
        for (const QString& app : applications) {
            QIcon appIcon = getApplicationIcon(app);
            auto* action = menu->addAction(appIcon, app);
            connect(action, &QAction::triggered, [=]() {
                openWithApplication(m_fileInfo.absoluteFilePath(), app);
            });
        }
        
        menu->addSeparator();
        menu->addAction("Choose default program...", [=]() {
            chooseDefaultProgram(m_fileInfo.absoluteFilePath());
        });
    }

private:
    QFileInfo m_fileInfo;
};

// Usage
void MyFileExplorer::showContextMenu(const QPoint& position, const QFileInfo& fileInfo)
{
    auto* contextMenu = new FileExplorerContextMenu(fileInfo, this);
    contextMenu->exec(mapToGlobal(position));
}
```

### Text Editor Context Menu

```cpp
class TextEditorContextMenu : public FluentContextMenu
{
public:
    TextEditorContextMenu(QTextEdit* textEdit, QWidget* parent = nullptr)
        : FluentContextMenu(parent), m_textEdit(textEdit) {
        setupMenu();
    }

private:
    void setupMenu() {
        bool hasSelection = m_textEdit->textCursor().hasSelection();
        bool canPaste = !QApplication::clipboard()->text().isEmpty();
        bool canUndo = m_textEdit->document()->isUndoAvailable();
        bool canRedo = m_textEdit->document()->isRedoAvailable();
        
        // Undo/Redo
        auto* undoAction = addAction(QIcon(":/icons/undo"), "Undo");
        undoAction->setShortcut(QKeySequence::Undo);
        undoAction->setEnabled(canUndo);
        connect(undoAction, &QAction::triggered, m_textEdit, &QTextEdit::undo);
        
        auto* redoAction = addAction(QIcon(":/icons/redo"), "Redo");
        redoAction->setShortcut(QKeySequence::Redo);
        redoAction->setEnabled(canRedo);
        connect(redoAction, &QAction::triggered, m_textEdit, &QTextEdit::redo);
        
        addSeparator();
        
        // Cut/Copy/Paste
        auto* cutAction = addAction(QIcon(":/icons/cut"), "Cut");
        cutAction->setShortcut(QKeySequence::Cut);
        cutAction->setEnabled(hasSelection);
        connect(cutAction, &QAction::triggered, m_textEdit, &QTextEdit::cut);
        
        auto* copyAction = addAction(QIcon(":/icons/copy"), "Copy");
        copyAction->setShortcut(QKeySequence::Copy);
        copyAction->setEnabled(hasSelection);
        connect(copyAction, &QAction::triggered, m_textEdit, &QTextEdit::copy);
        
        auto* pasteAction = addAction(QIcon(":/icons/paste"), "Paste");
        pasteAction->setShortcut(QKeySequence::Paste);
        pasteAction->setEnabled(canPaste);
        connect(pasteAction, &QAction::triggered, m_textEdit, &QTextEdit::paste);
        
        addSeparator();
        
        // Select All
        auto* selectAllAction = addAction(QIcon(":/icons/select-all"), "Select All");
        selectAllAction->setShortcut(QKeySequence::SelectAll);
        connect(selectAllAction, &QAction::triggered, m_textEdit, &QTextEdit::selectAll);
        
        addSeparator();
        
        // Find/Replace
        auto* findAction = addAction(QIcon(":/icons/find"), "Find");
        findAction->setShortcut(QKeySequence::Find);
        connect(findAction, &QAction::triggered, this, &TextEditorContextMenu::showFindDialog);
        
        if (hasSelection) {
            QString selectedText = m_textEdit->textCursor().selectedText();
            if (selectedText.length() < 50) {  // Don't show for very long selections
                auto* findSelectionAction = addAction(QIcon(":/icons/find"), 
                    QString("Find '%1'").arg(selectedText));
                connect(findSelectionAction, &QAction::triggered, [=]() {
                    findText(selectedText);
                });
            }
        }
        
        addSeparator();
        
        // Formatting (if rich text)
        if (m_textEdit->acceptRichText()) {
            setupFormattingMenu();
        }
        
        // Insert submenu
        auto* insertMenu = addMenu(QIcon(":/icons/insert"), "Insert");
        insertMenu->addAction("Date/Time", [=]() {
            insertDateTime();
        });
        insertMenu->addAction("Special Character...", [=]() {
            showSpecialCharacterDialog();
        });
    }
    
    void setupFormattingMenu() {
        auto* formatMenu = addMenu(QIcon(":/icons/format"), "Format");
        
        QTextCursor cursor = m_textEdit->textCursor();
        QTextCharFormat format = cursor.charFormat();
        
        // Bold
        auto* boldAction = formatMenu->addAction(QIcon(":/icons/bold"), "Bold");
        boldAction->setCheckable(true);
        boldAction->setChecked(format.fontWeight() == QFont::Bold);
        boldAction->setShortcut(QKeySequence::Bold);
        connect(boldAction, &QAction::triggered, [=](bool checked) {
            QTextCharFormat fmt;
            fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
            m_textEdit->textCursor().mergeCharFormat(fmt);
        });
        
        // Italic
        auto* italicAction = formatMenu->addAction(QIcon(":/icons/italic"), "Italic");
        italicAction->setCheckable(true);
        italicAction->setChecked(format.fontItalic());
        italicAction->setShortcut(QKeySequence::Italic);
        connect(italicAction, &QAction::triggered, [=](bool checked) {
            QTextCharFormat fmt;
            fmt.setFontItalic(checked);
            m_textEdit->textCursor().mergeCharFormat(fmt);
        });
        
        // Underline
        auto* underlineAction = formatMenu->addAction(QIcon(":/icons/underline"), "Underline");
        underlineAction->setCheckable(true);
        underlineAction->setChecked(format.fontUnderline());
        underlineAction->setShortcut(QKeySequence::Underline);
        connect(underlineAction, &QAction::triggered, [=](bool checked) {
            QTextCharFormat fmt;
            fmt.setFontUnderline(checked);
            m_textEdit->textCursor().mergeCharFormat(fmt);
        });
    }

private:
    QTextEdit* m_textEdit;
};
```

### Application Menu Bar Integration

```cpp
class ApplicationMenuBar : public QMenuBar
{
public:
    ApplicationMenuBar(QWidget* parent = nullptr) : QMenuBar(parent) {
        setupMenus();
    }

private:
    void setupMenus() {
        // File menu
        auto* fileMenu = new FluentContextMenu("File", this);
        
        fileMenu->addAction(QIcon(":/icons/new"), "New", QKeySequence::New);
        fileMenu->addAction(QIcon(":/icons/open"), "Open", QKeySequence::Open);
        
        // Recent files submenu
        m_recentFilesMenu = fileMenu->addMenu(QIcon(":/icons/recent"), "Recent Files");
        updateRecentFilesMenu();
        
        fileMenu->addSeparator();
        fileMenu->addAction(QIcon(":/icons/save"), "Save", QKeySequence::Save);
        fileMenu->addAction(QIcon(":/icons/save-as"), "Save As", QKeySequence::SaveAs);
        fileMenu->addSeparator();
        fileMenu->addAction(QIcon(":/icons/exit"), "Exit", QKeySequence::Quit);
        
        addMenu(fileMenu);
        
        // Edit menu
        auto* editMenu = new FluentContextMenu("Edit", this);
        editMenu->addAction(QIcon(":/icons/undo"), "Undo", QKeySequence::Undo);
        editMenu->addAction(QIcon(":/icons/redo"), "Redo", QKeySequence::Redo);
        editMenu->addSeparator();
        editMenu->addAction(QIcon(":/icons/cut"), "Cut", QKeySequence::Cut);
        editMenu->addAction(QIcon(":/icons/copy"), "Copy", QKeySequence::Copy);
        editMenu->addAction(QIcon(":/icons/paste"), "Paste", QKeySequence::Paste);
        
        addMenu(editMenu);
        
        // View menu
        auto* viewMenu = new FluentContextMenu("View", this);
        
        auto* zoomMenu = viewMenu->addMenu(QIcon(":/icons/zoom"), "Zoom");
        zoomMenu->addAction("Zoom In", QKeySequence::ZoomIn);
        zoomMenu->addAction("Zoom Out", QKeySequence::ZoomOut);
        zoomMenu->addAction("Actual Size", [=]() { setZoom(100); });
        zoomMenu->addSeparator();
        zoomMenu->addAction("Fit to Window");
        
        viewMenu->addSeparator();
        
        auto* toolbarsMenu = viewMenu->addMenu(QIcon(":/icons/toolbar"), "Toolbars");
        auto* mainToolbarAction = toolbarsMenu->addAction("Main Toolbar");
        mainToolbarAction->setCheckable(true);
        mainToolbarAction->setChecked(true);
        
        auto* formatToolbarAction = toolbarsMenu->addAction("Format Toolbar");
        formatToolbarAction->setCheckable(true);
        formatToolbarAction->setChecked(false);
        
        addMenu(viewMenu);
    }
    
    void updateRecentFilesMenu() {
        m_recentFilesMenu->clear();
        
        QStringList recentFiles = getRecentFiles();
        if (recentFiles.isEmpty()) {
            auto* noFilesAction = m_recentFilesMenu->addAction("No recent files");
            noFilesAction->setEnabled(false);
        } else {
            for (int i = 0; i < recentFiles.size() && i < 10; ++i) {
                QString file = recentFiles[i];
                QString fileName = QFileInfo(file).baseName();
                QString actionText = QString("&%1 %2").arg(i + 1).arg(fileName);
                
                auto* action = m_recentFilesMenu->addAction(QIcon(":/icons/document"), actionText);
                action->setData(file);
                
                connect(action, &QAction::triggered, [=]() {
                    openFile(action->data().toString());
                });
            }
            
            m_recentFilesMenu->addSeparator();
            m_recentFilesMenu->addAction("Clear Recent Files", [=]() {
                clearRecentFiles();
                updateRecentFilesMenu();
            });
        }
    }

private:
    FluentContextMenu* m_recentFilesMenu{nullptr};
};
```

## Accessibility

FluentContextMenu provides comprehensive accessibility support:

```cpp
auto* contextMenu = new FluentContextMenu(this);

// Accessible properties (automatically set)
contextMenu->setAccessibleName("Context menu");
contextMenu->setAccessibleDescription("Right-click menu with available actions");

// Keyboard navigation (automatic)
// - Arrow keys to navigate menu items
// - Enter to activate items
// - Escape to close menu
// - Alt+Letter for mnemonics

// Screen reader support
contextMenu->setAccessibleRole(QAccessible::PopupMenu);

// Action announcements
connect(contextMenu, &FluentContextMenu::actionTriggered, [=](QAction* action) {
    QString announcement = QString("Activated %1").arg(action->text());
    contextMenu->announceToScreenReader(announcement);
});
```

## Best Practices

### Context Menu Design Guidelines
1. **Keep menus concise** - Limit items to most relevant actions
2. **Use logical grouping** - Group related actions with separators
3. **Provide keyboard shortcuts** - Show shortcuts for common actions
4. **Use clear icons** - Help users identify actions quickly
5. **Support keyboard navigation** - Ensure accessibility

### Performance Considerations
- Use lazy loading for dynamic menu content
- Cache menu items when possible
- Optimize animations for smooth interaction
- Implement efficient submenu population

### Accessibility Guidelines
- Support keyboard navigation
- Provide action announcements
- Use sufficient color contrast
- Include meaningful action descriptions
- Test with assistive technologies

## Signals Reference

```cpp
// Menu signals
void aboutToShow();
void aboutToHide();
void triggered(QAction* action);
void hovered(QAction* action);

// Animation signals
void showAnimationStarted();
void showAnimationFinished();
void hideAnimationStarted();
void hideAnimationFinished();

// Interaction signals
void itemHovered(QAction* action, bool isHovered);
void submenuOpened(FluentContextMenu* submenu);
void submenuClosed(FluentContextMenu* submenu);
```

## See Also

- [FluentButton](FluentButton.md) - For menu action buttons
- [FluentNavigationView](FluentNavigationView.md) - For main navigation
- [FluentTooltip](FluentTooltip.md) - For action descriptions
- [API Reference - FluentContextMenu](../API_REFERENCE.md#fluentcontextmenu) - Complete API documentation
