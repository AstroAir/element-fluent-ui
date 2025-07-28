// examples/ContextMenuInteractionExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QSplitter>
#include <QTabWidget>
#include <QShortcut>
#include <QKeySequence>
#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>

#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentContextMenu.h"
#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Components/FluentTextInput.h"
#include <QTime>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPushButton>

using namespace FluentQt;

/**
 * @brief Comprehensive demonstration of FluentQt context menus and user interactions
 * 
 * This example showcases:
 * - Advanced FluentContextMenu with submenus, separators, and custom items
 * - Rich tooltip system with smart positioning and custom content
 * - Comprehensive keyboard shortcuts and navigation patterns
 * - Drag-and-drop interactions with visual feedback
 * - Advanced user interaction patterns and gestures
 * - Context-sensitive menus based on selection and state
 * - Custom menu items with icons, shortcuts, and checkable states
 * - Multi-level menu hierarchies and cascading behavior
 */
class ContextMenuInteractionExample : public QMainWindow {
    Q_OBJECT

public:
    ContextMenuInteractionExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupContextMenus();
        setupTooltips();
        setupKeyboardShortcuts();
        setupDragDrop();
        setupConnections();
        populateWithSampleData();
    }

private slots:
    // Context menu actions
    void onNewItem();
    void onEditItem();
    void onDeleteItem();
    void onCopyItem();
    void onPasteItem();
    void onCutItem();
    void onSelectAll();
    void onProperties();
    void onRefresh();
    
    // File operations
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onExportData();
    void onImportData();
    
    // Edit operations
    void onUndo();
    void onRedo();
    void onFind();
    void onReplace();
    
    // View operations
    void onZoomIn();
    void onZoomOut();
    void onResetZoom();
    void onToggleFullscreen();
    
    // Custom interactions
    void onItemDoubleClicked();
    void onSelectionChanged();
    void showCustomTooltip(const QPoint& position, const QString& content);
    void hideCustomTooltip();

private:
    void setupUI();
    void setupContextMenus();
    void setupTooltips();
    void setupKeyboardShortcuts();
    void setupDragDrop();
    void setupConnections();
    void populateWithSampleData();
    
    void createMainContextMenu();
    void createListContextMenu();
    void createTreeContextMenu();
    void createTextContextMenu();
    void createTableContextMenu();
    
    void showContextMenuAt(const QPoint& position, QWidget* widget);
    void updateMenuStates();
    void logInteraction(const QString& interaction);

private:
    // UI Components
    QWidget* m_centralWidget{nullptr};
    QSplitter* m_mainSplitter{nullptr};
    QTabWidget* m_contentTabs{nullptr};
    
    // Content widgets
    QListWidget* m_listWidget{nullptr};
    Components::FluentTreeView* m_treeView{nullptr};
    QTableWidget* m_tableWidget{nullptr};
    QTextEdit* m_textEdit{nullptr};
    Components::FluentTextInput* m_fluentTextInput{nullptr};
    
    // Context menus
    Components::FluentContextMenu* m_mainContextMenu{nullptr};
    Components::FluentContextMenu* m_listContextMenu{nullptr};
    Components::FluentContextMenu* m_treeContextMenu{nullptr};
    Components::FluentContextMenu* m_textContextMenu{nullptr};
    Components::FluentContextMenu* m_tableContextMenu{nullptr};
    
    // Submenus
    Components::FluentContextMenu* m_fileSubmenu{nullptr};
    Components::FluentContextMenu* m_editSubmenu{nullptr};
    Components::FluentContextMenu* m_viewSubmenu{nullptr};
    Components::FluentContextMenu* m_toolsSubmenu{nullptr};
    
    // Tooltips
    Components::FluentTooltip* m_customTooltip{nullptr};
    
    // Information display
    QTextEdit* m_interactionLog{nullptr};
    QLabel* m_statusLabel{nullptr};
    
    // Keyboard shortcuts
    QList<QShortcut*> m_shortcuts;
    
    // State
    QClipboard* m_clipboard{nullptr};
    QString m_copiedData;
    QWidget* m_lastContextWidget{nullptr};
    bool m_dragInProgress{false};
};

void ContextMenuInteractionExample::setupUI() {
    setWindowTitle("FluentQt Context Menu & Interaction Showcase");
    setMinimumSize(1000, 700);
    
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    auto* mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->addWidget(m_mainSplitter);
    
    // Create content tabs (left side)
    m_contentTabs = new QTabWidget(this);
    m_contentTabs->setMinimumWidth(600);
    
    // List widget tab
    m_listWidget = new QListWidget(this);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_listWidget->setDefaultDropAction(Qt::MoveAction);
    m_contentTabs->addTab(m_listWidget, "List View");
    
    // Tree view tab
    m_treeView = new Components::FluentTreeView(this);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setDragDropEnabled(true);
    m_contentTabs->addTab(m_treeView, "Tree View");
    
    // Table widget tab
    m_tableWidget = new QTableWidget(5, 4, this);
    m_tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableWidget->setHorizontalHeaderLabels({"Name", "Type", "Size", "Modified"});
    m_tableWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_contentTabs->addTab(m_tableWidget, "Table View");
    
    // Text edit tab
    m_textEdit = new QTextEdit(this);
    m_textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    m_textEdit->setAcceptDrops(true);
    m_contentTabs->addTab(m_textEdit, "Text Editor");
    
    // Fluent text input tab
    m_fluentTextInput = new Components::FluentTextInput(this);
    m_fluentTextInput->setContextMenuPolicy(Qt::CustomContextMenu);
    m_fluentTextInput->setPlaceholderText("FluentTextInput with custom context menu...");
    m_contentTabs->addTab(m_fluentTextInput, "Fluent Text");
    
    m_mainSplitter->addWidget(m_contentTabs);
    
    // Create information panel (right side)
    auto* infoWidget = new QWidget(this);
    infoWidget->setMaximumWidth(300);
    auto* infoLayout = new QVBoxLayout(infoWidget);
    
    // Interaction log
    auto* logGroup = new QGroupBox("Interaction Log", this);
    auto* logLayout = new QVBoxLayout(logGroup);
    
    m_interactionLog = new QTextEdit(this);
    m_interactionLog->setMaximumHeight(200);
    m_interactionLog->setReadOnly(true);
    logLayout->addWidget(m_interactionLog);
    
    infoLayout->addWidget(logGroup);
    
    // Instructions
    auto* instructionsGroup = new QGroupBox("Instructions", this);
    auto* instructionsLayout = new QVBoxLayout(instructionsGroup);
    
    auto* instructionsText = new QLabel(
        "• Right-click on any widget for context menu\n"
        "• Use keyboard shortcuts (see Help menu)\n"
        "• Drag and drop items between widgets\n"
        "• Hover over items for tooltips\n"
        "• Double-click items for quick actions\n"
        "• Try different selection states\n"
        "• Explore submenu hierarchies", this);
    instructionsText->setWordWrap(true);
    instructionsLayout->addWidget(instructionsText);
    
    infoLayout->addWidget(instructionsGroup);
    
    // Status
    m_statusLabel = new QLabel("Ready - Right-click for context menu", this);
    m_statusLabel->setStyleSheet("padding: 8px; background-color: #f0f0f0; border-radius: 4px;");
    infoLayout->addWidget(m_statusLabel);
    
    infoLayout->addStretch();
    
    m_mainSplitter->addWidget(infoWidget);
    
    // Set splitter proportions
    m_mainSplitter->setSizes({700, 300});
    
    // Initialize clipboard
    m_clipboard = QApplication::clipboard();
}

void ContextMenuInteractionExample::setupContextMenus() {
    createMainContextMenu();
    createListContextMenu();
    createTreeContextMenu();
    createTextContextMenu();
    createTableContextMenu();
}

void ContextMenuInteractionExample::createMainContextMenu() {
    m_mainContextMenu = new Components::FluentContextMenu(this);
    m_mainContextMenu->setTitle("Main Menu");

    // File submenu
    m_fileSubmenu = m_mainContextMenu->addFluentSubmenu("File", QIcon(":/icons/file.png"));
    m_fileSubmenu->addFluentAction("New", QIcon(":/icons/new.png"), [this]() { onNewFile(); });
    m_fileSubmenu->addFluentAction("Open...", QIcon(":/icons/open.png"), [this]() { onOpenFile(); });
    m_fileSubmenu->addFluentAction("Save", QIcon(":/icons/save.png"), [this]() { onSaveFile(); });
    m_fileSubmenu->addSeparator();
    m_fileSubmenu->addFluentAction("Export...", QIcon(":/icons/export.png"), [this]() { onExportData(); });
    m_fileSubmenu->addFluentAction("Import...", QIcon(":/icons/import.png"), [this]() { onImportData(); });

    // Edit submenu
    m_editSubmenu = m_mainContextMenu->addFluentSubmenu("Edit", QIcon(":/icons/edit.png"));
    m_editSubmenu->addFluentAction("Undo", QIcon(":/icons/undo.png"), [this]() { onUndo(); });
    m_editSubmenu->addFluentAction("Redo", QIcon(":/icons/redo.png"), [this]() { onRedo(); });
    m_editSubmenu->addSeparator();
    m_editSubmenu->addFluentAction("Cut", QIcon(":/icons/cut.png"), [this]() { onCutItem(); });
    m_editSubmenu->addFluentAction("Copy", QIcon(":/icons/copy.png"), [this]() { onCopyItem(); });
    m_editSubmenu->addFluentAction("Paste", QIcon(":/icons/paste.png"), [this]() { onPasteItem(); });
    m_editSubmenu->addSeparator();
    m_editSubmenu->addFluentAction("Select All", QIcon(":/icons/select-all.png"), [this]() { onSelectAll(); });
    m_editSubmenu->addFluentAction("Find...", QIcon(":/icons/find.png"), [this]() { onFind(); });
    m_editSubmenu->addFluentAction("Replace...", QIcon(":/icons/replace.png"), [this]() { onReplace(); });

    // View submenu
    m_viewSubmenu = m_mainContextMenu->addFluentSubmenu("View", QIcon(":/icons/view.png"));
    m_viewSubmenu->addFluentAction("Zoom In", QIcon(":/icons/zoom-in.png"), [this]() { onZoomIn(); });
    m_viewSubmenu->addFluentAction("Zoom Out", QIcon(":/icons/zoom-out.png"), [this]() { onZoomOut(); });
    m_viewSubmenu->addFluentAction("Reset Zoom", QIcon(":/icons/zoom-reset.png"), [this]() { onResetZoom(); });
    m_viewSubmenu->addSeparator();
    auto* fullscreenAction = m_viewSubmenu->addToggleAction("Fullscreen", QIcon(":/icons/fullscreen.png"));
    connect(fullscreenAction->action(), &QAction::triggered, this, &ContextMenuInteractionExample::onToggleFullscreen);

    // Tools submenu
    m_toolsSubmenu = m_mainContextMenu->addFluentSubmenu("Tools", QIcon(":/icons/tools.png"));
    m_toolsSubmenu->addFluentAction("Refresh", QIcon(":/icons/refresh.png"), [this]() { onRefresh(); });
    m_toolsSubmenu->addFluentAction("Properties", QIcon(":/icons/properties.png"), [this]() { onProperties(); });
}

void ContextMenuInteractionExample::createListContextMenu() {
    m_listContextMenu = new Components::FluentContextMenu(this);
    m_listContextMenu->setTitle("List Actions");

    // Item operations
    m_listContextMenu->addFluentAction("New Item", QIcon(":/icons/add.png"), [this]() { onNewItem(); });
    m_listContextMenu->addFluentAction("Edit Item", QIcon(":/icons/edit.png"), [this]() { onEditItem(); });
    m_listContextMenu->addFluentAction("Delete Item", QIcon(":/icons/delete.png"), [this]() { onDeleteItem(); });

    m_listContextMenu->addSeparator();

    // Clipboard operations
    m_listContextMenu->addFluentAction("Cut", QIcon(":/icons/cut.png"), [this]() { onCutItem(); });
    m_listContextMenu->addFluentAction("Copy", QIcon(":/icons/copy.png"), [this]() { onCopyItem(); });
    m_listContextMenu->addFluentAction("Paste", QIcon(":/icons/paste.png"), [this]() { onPasteItem(); });

    m_listContextMenu->addSeparator();

    // Selection operations
    m_listContextMenu->addFluentAction("Select All", QIcon(":/icons/select-all.png"), [this]() { onSelectAll(); });

    m_listContextMenu->addSeparator("Advanced");

    // Advanced options
    auto* sortSubmenu = m_listContextMenu->addFluentSubmenu("Sort", QIcon(":/icons/sort.png"));
    sortSubmenu->addFluentAction("Sort A-Z", QIcon(":/icons/sort-asc.png"), [this]() {
        m_listWidget->sortItems(Qt::AscendingOrder);
        logInteraction("List sorted A-Z");
    });
    sortSubmenu->addFluentAction("Sort Z-A", QIcon(":/icons/sort-desc.png"), [this]() {
        m_listWidget->sortItems(Qt::DescendingOrder);
        logInteraction("List sorted Z-A");
    });

    auto* viewSubmenu = m_listContextMenu->addFluentSubmenu("View", QIcon(":/icons/view.png"));
    auto* iconViewAction = viewSubmenu->addToggleAction("Icon View", QIcon(":/icons/icon-view.png"), true);
    auto* listViewAction = viewSubmenu->addToggleAction("List View", QIcon(":/icons/list-view.png"), false);

    // Make view actions mutually exclusive
    connect(iconViewAction->action(), &QAction::triggered, [listViewAction](bool checked) {
        if (checked) listViewAction->setChecked(false);
    });
    connect(listViewAction->action(), &QAction::triggered, [iconViewAction](bool checked) {
        if (checked) iconViewAction->setChecked(false);
    });

    m_listContextMenu->addFluentAction("Properties", QIcon(":/icons/properties.png"), [this]() { onProperties(); });
}

void ContextMenuInteractionExample::createTreeContextMenu() {
    m_treeContextMenu = new Components::FluentContextMenu(this);
    m_treeContextMenu->setTitle("Tree Actions");

    // Node operations
    m_treeContextMenu->addFluentAction("Add Child", QIcon(":/icons/add-child.png"), [this]() {
        // Add child to selected item
        logInteraction("Add child node requested");
    });
    m_treeContextMenu->addFluentAction("Add Sibling", QIcon(":/icons/add-sibling.png"), [this]() {
        // Add sibling to selected item
        logInteraction("Add sibling node requested");
    });
    m_treeContextMenu->addFluentAction("Delete Node", QIcon(":/icons/delete.png"), [this]() { onDeleteItem(); });

    m_treeContextMenu->addSeparator();

    // Expansion operations
    auto* expandSubmenu = m_treeContextMenu->addFluentSubmenu("Expand", QIcon(":/icons/expand.png"));
    expandSubmenu->addFluentAction("Expand All", QIcon(":/icons/expand-all.png"), [this]() {
        m_treeView->expandAll();
        logInteraction("Tree expanded all");
    });
    expandSubmenu->addFluentAction("Collapse All", QIcon(":/icons/collapse-all.png"), [this]() {
        m_treeView->collapseAll();
        logInteraction("Tree collapsed all");
    });
    expandSubmenu->addFluentAction("Expand Children", QIcon(":/icons/expand-children.png"), [this]() {
        // Expand children of selected item
        logInteraction("Expand children requested");
    });

    m_treeContextMenu->addSeparator();

    // Standard operations
    m_treeContextMenu->addFluentAction("Cut", QIcon(":/icons/cut.png"), [this]() { onCutItem(); });
    m_treeContextMenu->addFluentAction("Copy", QIcon(":/icons/copy.png"), [this]() { onCopyItem(); });
    m_treeContextMenu->addFluentAction("Paste", QIcon(":/icons/paste.png"), [this]() { onPasteItem(); });

    m_treeContextMenu->addSeparator();
    m_treeContextMenu->addFluentAction("Properties", QIcon(":/icons/properties.png"), [this]() { onProperties(); });
}

void ContextMenuInteractionExample::createTextContextMenu() {
    m_textContextMenu = new Components::FluentContextMenu(this);
    m_textContextMenu->setTitle("Text Actions");

    // Text operations
    m_textContextMenu->addFluentAction("Undo", QIcon(":/icons/undo.png"), [this]() { onUndo(); });
    m_textContextMenu->addFluentAction("Redo", QIcon(":/icons/redo.png"), [this]() { onRedo(); });

    m_textContextMenu->addSeparator();

    m_textContextMenu->addFluentAction("Cut", QIcon(":/icons/cut.png"), [this]() { onCutItem(); });
    m_textContextMenu->addFluentAction("Copy", QIcon(":/icons/copy.png"), [this]() { onCopyItem(); });
    m_textContextMenu->addFluentAction("Paste", QIcon(":/icons/paste.png"), [this]() { onPasteItem(); });

    m_textContextMenu->addSeparator();

    m_textContextMenu->addFluentAction("Select All", QIcon(":/icons/select-all.png"), [this]() { onSelectAll(); });
    m_textContextMenu->addFluentAction("Find...", QIcon(":/icons/find.png"), [this]() { onFind(); });
    m_textContextMenu->addFluentAction("Replace...", QIcon(":/icons/replace.png"), [this]() { onReplace(); });

    m_textContextMenu->addSeparator("Formatting");

    // Formatting submenu
    auto* formatSubmenu = m_textContextMenu->addFluentSubmenu("Format", QIcon(":/icons/format.png"));
    auto* boldAction = formatSubmenu->addToggleAction("Bold", QIcon(":/icons/bold.png"));
    auto* italicAction = formatSubmenu->addToggleAction("Italic", QIcon(":/icons/italic.png"));
    auto* underlineAction = formatSubmenu->addToggleAction("Underline", QIcon(":/icons/underline.png"));

    connect(boldAction->action(), &QAction::triggered, [this](bool checked) {
        QTextCursor cursor = m_textEdit->textCursor();
        QTextCharFormat format = cursor.charFormat();
        format.setFontWeight(checked ? QFont::Bold : QFont::Normal);
        cursor.setCharFormat(format);
        logInteraction(QString("Text formatting: Bold %1").arg(checked ? "enabled" : "disabled"));
    });
}

void ContextMenuInteractionExample::createTableContextMenu() {
    m_tableContextMenu = new Components::FluentContextMenu(this);
    m_tableContextMenu->setTitle("Table Actions");

    // Row operations
    auto* rowSubmenu = m_tableContextMenu->addFluentSubmenu("Row", QIcon(":/icons/row.png"));
    rowSubmenu->addFluentAction("Insert Row Above", QIcon(":/icons/insert-row-above.png"), [this]() {
        int currentRow = m_tableWidget->currentRow();
        if (currentRow >= 0) {
            m_tableWidget->insertRow(currentRow);
            logInteraction(QString("Row inserted above row %1").arg(currentRow));
        }
    });
    rowSubmenu->addFluentAction("Insert Row Below", QIcon(":/icons/insert-row-below.png"), [this]() {
        int currentRow = m_tableWidget->currentRow();
        if (currentRow >= 0) {
            m_tableWidget->insertRow(currentRow + 1);
            logInteraction(QString("Row inserted below row %1").arg(currentRow));
        }
    });
    rowSubmenu->addFluentAction("Delete Row", QIcon(":/icons/delete-row.png"), [this]() {
        int currentRow = m_tableWidget->currentRow();
        if (currentRow >= 0) {
            m_tableWidget->removeRow(currentRow);
            logInteraction(QString("Row %1 deleted").arg(currentRow));
        }
    });

    // Column operations
    auto* columnSubmenu = m_tableContextMenu->addFluentSubmenu("Column", QIcon(":/icons/column.png"));
    columnSubmenu->addFluentAction("Insert Column Left", QIcon(":/icons/insert-column-left.png"), [this]() {
        int currentColumn = m_tableWidget->currentColumn();
        if (currentColumn >= 0) {
            m_tableWidget->insertColumn(currentColumn);
            logInteraction(QString("Column inserted left of column %1").arg(currentColumn));
        }
    });
    columnSubmenu->addFluentAction("Insert Column Right", QIcon(":/icons/insert-column-right.png"), [this]() {
        int currentColumn = m_tableWidget->currentColumn();
        if (currentColumn >= 0) {
            m_tableWidget->insertColumn(currentColumn + 1);
            logInteraction(QString("Column inserted right of column %1").arg(currentColumn));
        }
    });
    columnSubmenu->addFluentAction("Delete Column", QIcon(":/icons/delete-column.png"), [this]() {
        int currentColumn = m_tableWidget->currentColumn();
        if (currentColumn >= 0) {
            m_tableWidget->removeColumn(currentColumn);
            logInteraction(QString("Column %1 deleted").arg(currentColumn));
        }
    });

    m_tableContextMenu->addSeparator();

    // Standard operations
    m_tableContextMenu->addFluentAction("Cut", QIcon(":/icons/cut.png"), [this]() { onCutItem(); });
    m_tableContextMenu->addFluentAction("Copy", QIcon(":/icons/copy.png"), [this]() { onCopyItem(); });
    m_tableContextMenu->addFluentAction("Paste", QIcon(":/icons/paste.png"), [this]() { onPasteItem(); });

    m_tableContextMenu->addSeparator();

    // Table-specific operations
    auto* sortSubmenu = m_tableContextMenu->addFluentSubmenu("Sort", QIcon(":/icons/sort.png"));
    sortSubmenu->addFluentAction("Sort by Column (Ascending)", QIcon(":/icons/sort-asc.png"), [this]() {
        int currentColumn = m_tableWidget->currentColumn();
        if (currentColumn >= 0) {
            m_tableWidget->sortItems(currentColumn, Qt::AscendingOrder);
            logInteraction(QString("Table sorted by column %1 (ascending)").arg(currentColumn));
        }
    });
    sortSubmenu->addFluentAction("Sort by Column (Descending)", QIcon(":/icons/sort-desc.png"), [this]() {
        int currentColumn = m_tableWidget->currentColumn();
        if (currentColumn >= 0) {
            m_tableWidget->sortItems(currentColumn, Qt::DescendingOrder);
            logInteraction(QString("Table sorted by column %1 (descending)").arg(currentColumn));
        }
    });

    m_tableContextMenu->addFluentAction("Properties", QIcon(":/icons/properties.png"), [this]() { onProperties(); });
}

void ContextMenuInteractionExample::setupTooltips() {
    // Create custom tooltip
    m_customTooltip = new Components::FluentTooltip(this);

    // Set up tooltips for various widgets
    m_listWidget->setToolTip("Right-click for context menu\nDrag items to reorder");
    m_treeView->setToolTip("Expandable tree structure\nRight-click for node operations");
    m_tableWidget->setToolTip("Editable table\nRight-click for row/column operations");
    m_textEdit->setToolTip("Rich text editor\nRight-click for formatting options");
    m_fluentTextInput->setToolTip("Fluent text input\nSupports validation and custom styling");

    // Set up rich tooltips for tabs
    for (int i = 0; i < m_contentTabs->count(); ++i) {
        QString tabText = m_contentTabs->tabText(i);
        QString tooltipText;

        switch (i) {
            case 0: // List View
                tooltipText = "<b>List View</b><br/>• Right-click for context menu<br/>• Drag to reorder items<br/>• Double-click to edit";
                break;
            case 1: // Tree View
                tooltipText = "<b>Tree View</b><br/>• Expandable hierarchy<br/>• Drag-and-drop support<br/>• Context menu for node operations";
                break;
            case 2: // Table View
                tooltipText = "<b>Table View</b><br/>• Sortable columns<br/>• Row/column operations<br/>• Cell editing support";
                break;
            case 3: // Text Editor
                tooltipText = "<b>Text Editor</b><br/>• Rich text formatting<br/>• Find/replace functionality<br/>• Undo/redo support";
                break;
            case 4: // Fluent Text
                tooltipText = "<b>Fluent Text Input</b><br/>• Modern input styling<br/>• Validation support<br/>• Accessibility features";
                break;
        }

        m_contentTabs->setTabToolTip(i, tooltipText);
    }
}

void ContextMenuInteractionExample::setupKeyboardShortcuts() {
    // File operations
    m_shortcuts.append(new QShortcut(QKeySequence::New, this, [this]() { onNewFile(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::Open, this, [this]() { onOpenFile(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::Save, this, [this]() { onSaveFile(); }));

    // Edit operations
    m_shortcuts.append(new QShortcut(QKeySequence::Undo, this, [this]() { onUndo(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::Redo, this, [this]() { onRedo(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::Cut, this, [this]() { onCutItem(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::Copy, this, [this]() { onCopyItem(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::Paste, this, [this]() { onPasteItem(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::SelectAll, this, [this]() { onSelectAll(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::Find, this, [this]() { onFind(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::Replace, this, [this]() { onReplace(); }));

    // View operations
    m_shortcuts.append(new QShortcut(QKeySequence::ZoomIn, this, [this]() { onZoomIn(); }));
    m_shortcuts.append(new QShortcut(QKeySequence::ZoomOut, this, [this]() { onZoomOut(); }));
    m_shortcuts.append(new QShortcut(QKeySequence("Ctrl+0"), this, [this]() { onResetZoom(); }));
    m_shortcuts.append(new QShortcut(QKeySequence("F11"), this, [this]() { onToggleFullscreen(); }));

    // Custom shortcuts
    m_shortcuts.append(new QShortcut(QKeySequence("F5"), this, [this]() { onRefresh(); }));
    m_shortcuts.append(new QShortcut(QKeySequence("Alt+Return"), this, [this]() { onProperties(); }));
    m_shortcuts.append(new QShortcut(QKeySequence("Delete"), this, [this]() { onDeleteItem(); }));
    m_shortcuts.append(new QShortcut(QKeySequence("Insert"), this, [this]() { onNewItem(); }));

    // Context menu shortcut
    m_shortcuts.append(new QShortcut(QKeySequence("Shift+F10"), this, [this]() {
        QWidget* focusWidget = QApplication::focusWidget();
        if (focusWidget) {
            QPoint pos = focusWidget->mapToGlobal(focusWidget->rect().center());
            showContextMenuAt(pos, focusWidget);
        }
    }));
}

void ContextMenuInteractionExample::setupDragDrop() {
    // Enable drag and drop for all relevant widgets
    m_listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_listWidget->setDefaultDropAction(Qt::MoveAction);
    m_listWidget->setDragEnabled(true);
    m_listWidget->setAcceptDrops(true);

    m_treeView->setDragDropEnabled(true);

    m_tableWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_tableWidget->setDefaultDropAction(Qt::MoveAction);
    m_tableWidget->setDragEnabled(true);
    m_tableWidget->setAcceptDrops(true);

    m_textEdit->setAcceptDrops(true);

    // Install event filters for custom drag-drop feedback
    m_listWidget->installEventFilter(this);
    m_treeView->installEventFilter(this);
    m_tableWidget->installEventFilter(this);
    m_textEdit->installEventFilter(this);
}

void ContextMenuInteractionExample::setupConnections() {
    // Context menu connections
    connect(m_listWidget, &QListWidget::customContextMenuRequested,
            this, [this](const QPoint& pos) {
                m_lastContextWidget = m_listWidget;
                updateMenuStates();
                m_listContextMenu->popup(m_listWidget->mapToGlobal(pos));
                logInteraction("List context menu opened");
            });

    connect(m_treeView, &Components::FluentTreeView::customContextMenuRequested,
            this, [this](const QPoint& pos) {
                m_lastContextWidget = m_treeView;
                updateMenuStates();
                m_treeContextMenu->popup(m_treeView->mapToGlobal(pos));
                logInteraction("Tree context menu opened");
            });

    connect(m_tableWidget, &QTableWidget::customContextMenuRequested,
            this, [this](const QPoint& pos) {
                m_lastContextWidget = m_tableWidget;
                updateMenuStates();
                m_tableContextMenu->popup(m_tableWidget->mapToGlobal(pos));
                logInteraction("Table context menu opened");
            });

    connect(m_textEdit, &QTextEdit::customContextMenuRequested,
            this, [this](const QPoint& pos) {
                m_lastContextWidget = m_textEdit;
                updateMenuStates();
                m_textContextMenu->popup(m_textEdit->mapToGlobal(pos));
                logInteraction("Text editor context menu opened");
            });

    connect(m_fluentTextInput, &Components::FluentTextInput::customContextMenuRequested,
            this, [this](const QPoint& pos) {
                m_lastContextWidget = m_fluentTextInput;
                updateMenuStates();
                m_textContextMenu->popup(m_fluentTextInput->mapToGlobal(pos));
                logInteraction("Fluent text input context menu opened");
            });

    // Double-click connections
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &ContextMenuInteractionExample::onItemDoubleClicked);
    connect(m_treeView, &Components::FluentTreeView::itemDoubleClicked, this, &ContextMenuInteractionExample::onItemDoubleClicked);
    connect(m_tableWidget, &QTableWidget::itemDoubleClicked, this, &ContextMenuInteractionExample::onItemDoubleClicked);

    // Selection change connections
    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &ContextMenuInteractionExample::onSelectionChanged);
    connect(m_treeView, &Components::FluentTreeView::itemSelectionChanged, this, &ContextMenuInteractionExample::onSelectionChanged);
    connect(m_tableWidget, &QTableWidget::itemSelectionChanged, this, &ContextMenuInteractionExample::onSelectionChanged);

    // Tab change connection
    connect(m_contentTabs, &QTabWidget::currentChanged, this, [this](int index) {
        QString tabName = m_contentTabs->tabText(index);
        logInteraction(QString("Switched to tab: %1").arg(tabName));
        m_statusLabel->setText(QString("Active tab: %1").arg(tabName));
    });
}

void ContextMenuInteractionExample::populateWithSampleData() {
    // Populate list widget
    QStringList listItems = {
        "Document 1.pdf", "Image.jpg", "Spreadsheet.xlsx",
        "Presentation.pptx", "Archive.zip", "Video.mp4",
        "Audio.mp3", "Code.cpp", "Data.json", "Config.xml"
    };

    for (const QString& item : listItems) {
        auto* listItem = new QListWidgetItem(item);
        listItem->setToolTip(QString("Right-click for options\nDouble-click to open: %1").arg(item));
        m_listWidget->addItem(listItem);
    }

    // Populate tree view with sample hierarchy
    // Set up columns
    std::vector<Components::FluentTreeColumn> columns = {
        {"Name", 200, true, true, Qt::AlignLeft | Qt::AlignVCenter},
        {"Type", 120, true, true, Qt::AlignLeft | Qt::AlignVCenter},
        {"Size", 100, true, true, Qt::AlignRight | Qt::AlignVCenter}
    };
    m_treeView->setColumns(columns);

    // Documents folder
    auto* documentsItem = m_treeView->addTopLevelItem("Documents");
    documentsItem->setText(1, "Folder");
    documentsItem->setText(2, "");
    documentsItem->setToolTip("Documents folder - Right-click for options");

    auto* reportItem = m_treeView->addChildItem(documentsItem, "Report.docx");
    reportItem->setText(1, "Document");
    reportItem->setText(2, "2.1 MB");

    auto* notesItem = m_treeView->addChildItem(documentsItem, "Notes.txt");
    notesItem->setText(1, "Text");
    notesItem->setText(2, "15 KB");

    // Images folder
    auto* imagesItem = m_treeView->addTopLevelItem("Images");
    imagesItem->setText(1, "Folder");
    imagesItem->setText(2, "");
    imagesItem->setToolTip("Images folder - Right-click for options");

    auto* photo1Item = m_treeView->addChildItem(imagesItem, "Photo1.jpg");
    photo1Item->setText(1, "Image");
    photo1Item->setText(2, "3.2 MB");

    auto* screenshotItem = m_treeView->addChildItem(imagesItem, "Screenshot.png");
    screenshotItem->setText(1, "Image");
    screenshotItem->setText(2, "1.8 MB");

    // Projects folder
    auto* projectsItem = m_treeView->addTopLevelItem("Projects");
    projectsItem->setText(1, "Folder");
    projectsItem->setText(2, "");
    projectsItem->setToolTip("Projects folder - Right-click for options");

    auto* project1Item = m_treeView->addChildItem(projectsItem, "Project Alpha");
    project1Item->setText(1, "Project");
    project1Item->setText(2, "");

    auto* mainCppItem = m_treeView->addChildItem(project1Item, "main.cpp");
    mainCppItem->setText(1, "C++ Source");
    mainCppItem->setText(2, "8.5 KB");

    auto* headerItem = m_treeView->addChildItem(project1Item, "header.h");
    headerItem->setText(1, "C++ Header");
    headerItem->setText(2, "2.1 KB");

    m_treeView->expandAll();

    // Populate table widget
    QStringList tableData[] = {
        {"file1.txt", "Text File", "1.2 KB", "2024-01-15 10:30"},
        {"image.png", "PNG Image", "256 KB", "2024-01-14 15:45"},
        {"data.csv", "CSV File", "45 KB", "2024-01-13 09:15"},
        {"video.mp4", "Video File", "12.5 MB", "2024-01-12 14:20"},
        {"archive.zip", "Archive", "2.8 MB", "2024-01-11 11:10"}
    };

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 4; ++col) {
            auto* item = new QTableWidgetItem(tableData[row][col]);
            item->setToolTip(QString("Cell (%1, %2)\nRight-click for options").arg(row + 1).arg(col + 1));
            m_tableWidget->setItem(row, col, item);
        }
    }

    // Set sample text in text editor
    m_textEdit->setHtml(
        "<h2>Sample Rich Text Document</h2>"
        "<p>This is a <b>rich text editor</b> with <i>formatting capabilities</i>. "
        "You can <u>underline text</u>, change <span style='color: blue;'>colors</span>, "
        "and create <a href='#'>hyperlinks</a>.</p>"
        "<ul>"
        "<li>Right-click for context menu</li>"
        "<li>Use keyboard shortcuts for quick actions</li>"
        "<li>Drag and drop text to rearrange</li>"
        "</ul>"
        "<p>Try selecting text and using the formatting options from the context menu!</p>"
    );

    // Set sample text in fluent text input
    m_fluentTextInput->setText("Sample text with FluentQt styling");

    // Log initial state
    logInteraction("Application initialized with sample data");
    m_statusLabel->setText("Ready - Right-click for context menu");
}

// Context menu action implementations
void ContextMenuInteractionExample::onNewItem() {
    if (m_lastContextWidget == m_listWidget) {
        QString text = QString("New Item %1").arg(m_listWidget->count() + 1);
        auto* item = new QListWidgetItem(text);
        item->setToolTip(QString("Right-click for options\nDouble-click to edit: %1").arg(text));
        m_listWidget->addItem(item);
        logInteraction(QString("New list item created: %1").arg(text));
    } else if (m_lastContextWidget == m_treeView) {
        auto selectedItems = m_treeView->selectedItems();
        if (!selectedItems.isEmpty()) {
            QString text = QString("New Child %1").arg(selectedItems.first()->childCount() + 1);
            auto* newItem = m_treeView->addChildItem(selectedItems.first(), text);
            newItem->setText(1, "Item");
            newItem->setText(2, "0 KB");
            logInteraction(QString("New tree item created: %1").arg(text));
        } else {
            QString text = QString("New Root %1").arg(m_treeView->columnCount() + 1);
            auto* newItem = m_treeView->addTopLevelItem(text);
            newItem->setText(1, "Item");
            newItem->setText(2, "0 KB");
            logInteraction(QString("New root tree item created: %1").arg(text));
        }
    }
    m_statusLabel->setText("New item created");
}

void ContextMenuInteractionExample::onEditItem() {
    if (m_lastContextWidget == m_listWidget) {
        auto* currentItem = m_listWidget->currentItem();
        if (currentItem) {
            m_listWidget->editItem(currentItem);
            logInteraction(QString("Editing list item: %1").arg(currentItem->text()));
        }
    } else if (m_lastContextWidget == m_tableWidget) {
        auto* currentItem = m_tableWidget->currentItem();
        if (currentItem) {
            m_tableWidget->editItem(currentItem);
            logInteraction(QString("Editing table cell: (%1, %2)").arg(currentItem->row() + 1).arg(currentItem->column() + 1));
        }
    }
    m_statusLabel->setText("Item editing started");
}

void ContextMenuInteractionExample::onDeleteItem() {
    if (m_lastContextWidget == m_listWidget) {
        auto* currentItem = m_listWidget->currentItem();
        if (currentItem) {
            QString itemText = currentItem->text();
            delete currentItem;
            logInteraction(QString("Deleted list item: %1").arg(itemText));
        }
    } else if (m_lastContextWidget == m_treeView) {
        auto* currentItem = m_treeView->currentItem();
        if (currentItem) {
            QString itemText = currentItem->text(0);
            m_treeView->removeItem(currentItem);
            logInteraction(QString("Deleted tree item: %1").arg(itemText));
        }
    } else if (m_lastContextWidget == m_tableWidget) {
        int currentRow = m_tableWidget->currentRow();
        if (currentRow >= 0) {
            m_tableWidget->removeRow(currentRow);
            logInteraction(QString("Deleted table row: %1").arg(currentRow + 1));
        }
    }
    m_statusLabel->setText("Item deleted");
}

void ContextMenuInteractionExample::onCopyItem() {
    QString textToCopy;

    if (m_lastContextWidget == m_listWidget) {
        auto* currentItem = m_listWidget->currentItem();
        if (currentItem) {
            textToCopy = currentItem->text();
        }
    } else if (m_lastContextWidget == m_treeView) {
        auto* currentItem = m_treeView->currentItem();
        if (currentItem) {
            textToCopy = QString("%1\t%2\t%3").arg(currentItem->text(0), currentItem->text(1), currentItem->text(2));
        }
    } else if (m_lastContextWidget == m_tableWidget) {
        auto* currentItem = m_tableWidget->currentItem();
        if (currentItem) {
            textToCopy = currentItem->text();
        }
    } else if (m_lastContextWidget == m_textEdit) {
        textToCopy = m_textEdit->textCursor().selectedText();
        if (textToCopy.isEmpty()) {
            textToCopy = m_textEdit->toPlainText();
        }
    } else if (m_lastContextWidget == m_fluentTextInput) {
        textToCopy = m_fluentTextInput->selectedText();
        if (textToCopy.isEmpty()) {
            textToCopy = m_fluentTextInput->text();
        }
    }

    if (!textToCopy.isEmpty()) {
        m_clipboard->setText(textToCopy);
        m_copiedData = textToCopy;
        logInteraction(QString("Copied to clipboard: %1").arg(textToCopy.left(50) + (textToCopy.length() > 50 ? "..." : "")));
        m_statusLabel->setText("Content copied to clipboard");
    }
}

void ContextMenuInteractionExample::onPasteItem() {
    QString clipboardText = m_clipboard->text();

    if (clipboardText.isEmpty()) {
        logInteraction("Paste failed: clipboard is empty");
        m_statusLabel->setText("Nothing to paste");
        return;
    }

    if (m_lastContextWidget == m_listWidget) {
        auto* item = new QListWidgetItem(clipboardText);
        item->setToolTip(QString("Pasted item: %1").arg(clipboardText));
        m_listWidget->addItem(item);
    } else if (m_lastContextWidget == m_textEdit) {
        m_textEdit->insertPlainText(clipboardText);
    } else if (m_lastContextWidget == m_fluentTextInput) {
        QString currentText = m_fluentTextInput->text();
        m_fluentTextInput->setText(currentText + clipboardText);
    }

    logInteraction(QString("Pasted from clipboard: %1").arg(clipboardText.left(50) + (clipboardText.length() > 50 ? "..." : "")));
    m_statusLabel->setText("Content pasted");
}

void ContextMenuInteractionExample::onCutItem() {
    onCopyItem(); // First copy

    if (m_lastContextWidget == m_textEdit) {
        m_textEdit->textCursor().removeSelectedText();
    } else if (m_lastContextWidget == m_fluentTextInput) {
        // For text input, we'll clear selected text or all text
        if (!m_fluentTextInput->selectedText().isEmpty()) {
            QString text = m_fluentTextInput->text();
            // This is a simplified cut - in a real implementation you'd handle selection properly
            m_fluentTextInput->clear();
        }
    } else {
        // For other widgets, cut is the same as delete after copy
        onDeleteItem();
    }

    logInteraction("Cut operation completed");
    m_statusLabel->setText("Content cut to clipboard");
}

void ContextMenuInteractionExample::onSelectAll() {
    if (m_lastContextWidget == m_listWidget) {
        m_listWidget->selectAll();
    } else if (m_lastContextWidget == m_tableWidget) {
        m_tableWidget->selectAll();
    } else if (m_lastContextWidget == m_textEdit) {
        m_textEdit->selectAll();
    } else if (m_lastContextWidget == m_fluentTextInput) {
        m_fluentTextInput->selectAll();
    }

    logInteraction("Select all executed");
    m_statusLabel->setText("All content selected");
}

void ContextMenuInteractionExample::onProperties() {
    QString properties;

    if (m_lastContextWidget == m_listWidget) {
        auto* currentItem = m_listWidget->currentItem();
        if (currentItem) {
            properties = QString("List Item Properties:\nText: %1\nRow: %2\nSelected: %3")
                        .arg(currentItem->text())
                        .arg(m_listWidget->row(currentItem) + 1)
                        .arg(currentItem->isSelected() ? "Yes" : "No");
        }
    } else if (m_lastContextWidget == m_treeView) {
        auto* currentItem = m_treeView->currentItem();
        if (currentItem) {
            properties = QString("Tree Item Properties:\nName: %1\nType: %2\nSize: %3\nChildren: %4")
                        .arg(currentItem->text(0))
                        .arg(currentItem->text(1))
                        .arg(currentItem->text(2))
                        .arg(currentItem->childCount());
        }
    } else if (m_lastContextWidget == m_tableWidget) {
        auto* currentItem = m_tableWidget->currentItem();
        if (currentItem) {
            properties = QString("Table Cell Properties:\nText: %1\nRow: %2\nColumn: %3\nSelected: %4")
                        .arg(currentItem->text())
                        .arg(currentItem->row() + 1)
                        .arg(currentItem->column() + 1)
                        .arg(currentItem->isSelected() ? "Yes" : "No");
        }
    }

    if (!properties.isEmpty()) {
        QMessageBox::information(this, "Properties", properties);
        logInteraction("Properties dialog shown");
    }
    m_statusLabel->setText("Properties displayed");
}

void ContextMenuInteractionExample::onRefresh() {
    // Refresh the current widget's content
    if (m_lastContextWidget == m_listWidget) {
        m_listWidget->update();
    } else if (m_lastContextWidget == m_treeView) {
        m_treeView->update();
    } else if (m_lastContextWidget == m_tableWidget) {
        m_tableWidget->update();
    } else if (m_lastContextWidget == m_textEdit) {
        m_textEdit->update();
    }

    logInteraction("Widget refreshed");
    m_statusLabel->setText("Content refreshed");
}

// File operations
void ContextMenuInteractionExample::onNewFile() {
    logInteraction("New file requested");
    m_statusLabel->setText("New file operation");
}

void ContextMenuInteractionExample::onOpenFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "All Files (*.*)");
    if (!fileName.isEmpty()) {
        logInteraction(QString("File open requested: %1").arg(fileName));
        m_statusLabel->setText(QString("File selected: %1").arg(QFileInfo(fileName).fileName()));
    }
}

void ContextMenuInteractionExample::onSaveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "All Files (*.*)");
    if (!fileName.isEmpty()) {
        logInteraction(QString("File save requested: %1").arg(fileName));
        m_statusLabel->setText(QString("Save location: %1").arg(QFileInfo(fileName).fileName()));
    }
}

void ContextMenuInteractionExample::onExportData() {
    logInteraction("Export data requested");
    m_statusLabel->setText("Export operation");
}

void ContextMenuInteractionExample::onImportData() {
    logInteraction("Import data requested");
    m_statusLabel->setText("Import operation");
}

// Edit operations
void ContextMenuInteractionExample::onUndo() {
    if (m_lastContextWidget == m_textEdit) {
        m_textEdit->undo();
    }
    logInteraction("Undo operation");
    m_statusLabel->setText("Undo executed");
}

void ContextMenuInteractionExample::onRedo() {
    if (m_lastContextWidget == m_textEdit) {
        m_textEdit->redo();
    }
    logInteraction("Redo operation");
    m_statusLabel->setText("Redo executed");
}

void ContextMenuInteractionExample::onFind() {
    logInteraction("Find dialog requested");
    m_statusLabel->setText("Find operation");
}

void ContextMenuInteractionExample::onReplace() {
    logInteraction("Replace dialog requested");
    m_statusLabel->setText("Replace operation");
}

// View operations
void ContextMenuInteractionExample::onZoomIn() {
    if (m_lastContextWidget == m_textEdit) {
        m_textEdit->zoomIn();
    }
    logInteraction("Zoom in");
    m_statusLabel->setText("Zoomed in");
}

void ContextMenuInteractionExample::onZoomOut() {
    if (m_lastContextWidget == m_textEdit) {
        m_textEdit->zoomOut();
    }
    logInteraction("Zoom out");
    m_statusLabel->setText("Zoomed out");
}

void ContextMenuInteractionExample::onResetZoom() {
    logInteraction("Zoom reset");
    m_statusLabel->setText("Zoom reset to 100%");
}

void ContextMenuInteractionExample::onToggleFullscreen() {
    if (isFullScreen()) {
        showNormal();
        logInteraction("Exited fullscreen mode");
        m_statusLabel->setText("Windowed mode");
    } else {
        showFullScreen();
        logInteraction("Entered fullscreen mode");
        m_statusLabel->setText("Fullscreen mode");
    }
}

// Custom interactions
void ContextMenuInteractionExample::onItemDoubleClicked() {
    logInteraction("Item double-clicked");
    m_statusLabel->setText("Item activated by double-click");
}

void ContextMenuInteractionExample::onSelectionChanged() {
    QString selectionInfo;

    if (sender() == m_listWidget) {
        int selectedCount = m_listWidget->selectedItems().count();
        selectionInfo = QString("List selection: %1 items").arg(selectedCount);
    } else if (sender() == m_treeView) {
        int selectedCount = m_treeView->selectedItems().count();
        selectionInfo = QString("Tree selection: %1 items").arg(selectedCount);
    } else if (sender() == m_tableWidget) {
        int selectedCount = m_tableWidget->selectedItems().count();
        selectionInfo = QString("Table selection: %1 cells").arg(selectedCount);
    }

    if (!selectionInfo.isEmpty()) {
        logInteraction(selectionInfo);
        m_statusLabel->setText(selectionInfo);
    }
}

void ContextMenuInteractionExample::showCustomTooltip(const QPoint& position, const QString& content) {
    if (m_customTooltip) {
        m_customTooltip->showTooltip(content, position);
        logInteraction("Custom tooltip shown");
    }
}

void ContextMenuInteractionExample::hideCustomTooltip() {
    if (m_customTooltip) {
        m_customTooltip->hide();
    }
}

// Helper methods
void ContextMenuInteractionExample::showContextMenuAt(const QPoint& position, QWidget* widget) {
    m_lastContextWidget = widget;
    updateMenuStates();

    if (widget == m_listWidget) {
        m_listContextMenu->popup(position);
    } else if (widget == m_treeView) {
        m_treeContextMenu->popup(position);
    } else if (widget == m_tableWidget) {
        m_tableContextMenu->popup(position);
    } else if (widget == m_textEdit || widget == m_fluentTextInput) {
        m_textContextMenu->popup(position);
    } else {
        m_mainContextMenu->popup(position);
    }

    logInteraction("Context menu shown via keyboard shortcut");
}

void ContextMenuInteractionExample::updateMenuStates() {
    // Update menu item states based on current context
    // This would typically enable/disable menu items based on selection state,
    // clipboard content, etc.

    bool hasSelection = false;
    bool hasClipboardContent = !m_clipboard->text().isEmpty();

    if (m_lastContextWidget == m_listWidget) {
        hasSelection = !m_listWidget->selectedItems().isEmpty();
    } else if (m_lastContextWidget == m_treeView) {
        hasSelection = !m_treeView->selectedItems().isEmpty();
    } else if (m_lastContextWidget == m_tableWidget) {
        hasSelection = !m_tableWidget->selectedItems().isEmpty();
    } else if (m_lastContextWidget == m_textEdit) {
        hasSelection = m_textEdit->textCursor().hasSelection();
    } else if (m_lastContextWidget == m_fluentTextInput) {
        hasSelection = !m_fluentTextInput->selectedText().isEmpty();
    }

    // In a real implementation, you would enable/disable specific menu actions here
    // based on hasSelection, hasClipboardContent, and other state variables
}

void ContextMenuInteractionExample::logInteraction(const QString& interaction) {
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, interaction);

    m_interactionLog->append(logEntry);

    // Auto-scroll to bottom
    QTextCursor cursor = m_interactionLog->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_interactionLog->setTextCursor(cursor);
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("FluentQt Context Menu Example");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt Examples");

    ContextMenuInteractionExample window;
    window.show();

    return app.exec();
}

#include "ContextMenuInteractionExample.moc"
