# FluentRichTextEditor Component

The `FluentRichTextEditor` component is a comprehensive rich text editing widget that implements Microsoft's Fluent Design principles. It provides a full-featured text editor with formatting toolbar, document management, and extensible plugin system.

## Overview

FluentRichTextEditor offers:
- **Rich text formatting** with comprehensive toolbar
- **Document management** with save/load functionality
- **Plugin system** for extensible features
- **Collaborative editing** support
- **Spell checking** and grammar checking
- **Theme integration** with automatic styling
- **Accessibility support** with screen reader compatibility

## Basic Usage

### Simple Rich Text Editor

```cpp
#include "FluentQt/Components/FluentRichTextEditor.h"

// Create a basic rich text editor
auto* editor = new FluentRichTextEditor;
editor->setPlaceholderText("Start typing your document...");

// Set initial content
editor->setHtml("<h1>Welcome</h1><p>This is a <b>rich text</b> editor.</p>");

// Handle content changes
connect(editor, &FluentRichTextEditor::contentChanged, [=]() {
    qDebug() << "Content changed";
    // Auto-save or mark as modified
});

// Add to layout
layout->addWidget(editor);
```

### Editor with Toolbar

```cpp
auto* editor = new FluentRichTextEditor;
editor->setToolbarVisible(true);
editor->setToolbarPosition(FluentRichTextToolbarPosition::Top);

// Configure toolbar buttons
editor->setToolbarButtons({
    FluentRichTextButton::Bold,
    FluentRichTextButton::Italic,
    FluentRichTextButton::Underline,
    FluentRichTextButton::Separator,
    FluentRichTextButton::FontFamily,
    FluentRichTextButton::FontSize,
    FluentRichTextButton::FontColor,
    FluentRichTextButton::Separator,
    FluentRichTextButton::AlignLeft,
    FluentRichTextButton::AlignCenter,
    FluentRichTextButton::AlignRight,
    FluentRichTextButton::AlignJustify,
    FluentRichTextButton::Separator,
    FluentRichTextButton::BulletList,
    FluentRichTextButton::NumberedList,
    FluentRichTextButton::Separator,
    FluentRichTextButton::InsertLink,
    FluentRichTextButton::InsertImage,
    FluentRichTextButton::InsertTable
});
```

## Text Formatting

### Basic Formatting

```cpp
auto* editor = new FluentRichTextEditor;

// Apply formatting programmatically
editor->setBold(true);
editor->setItalic(true);
editor->setUnderline(true);
editor->setStrikethrough(true);

// Font formatting
editor->setFontFamily("Arial");
editor->setFontSize(14);
editor->setFontColor(QColor(0, 120, 215));
editor->setBackgroundColor(QColor(255, 255, 0));

// Get current formatting
bool isBold = editor->isBold();
QString fontFamily = editor->fontFamily();
int fontSize = editor->fontSize();
```

### Paragraph Formatting

```cpp
auto* editor = new FluentRichTextEditor;

// Text alignment
editor->setAlignment(Qt::AlignLeft);
editor->setAlignment(Qt::AlignCenter);
editor->setAlignment(Qt::AlignRight);
editor->setAlignment(Qt::AlignJustify);

// Line spacing
editor->setLineSpacing(FluentLineSpacing::Single);
editor->setLineSpacing(FluentLineSpacing::OneAndHalf);
editor->setLineSpacing(FluentLineSpacing::Double);
editor->setCustomLineSpacing(1.2);

// Indentation
editor->increaseIndent();
editor->decreaseIndent();
editor->setIndentSize(20);  // pixels

// Paragraph spacing
editor->setParagraphSpacing(10);  // pixels between paragraphs
```

### Lists and Structure

```cpp
auto* editor = new FluentRichTextEditor;

// Create lists
editor->insertBulletList();
editor->insertNumberedList();
editor->insertCheckboxList();

// Custom list styles
editor->setBulletStyle(FluentBulletStyle::Disc);
editor->setBulletStyle(FluentBulletStyle::Circle);
editor->setBulletStyle(FluentBulletStyle::Square);

editor->setNumberStyle(FluentNumberStyle::Decimal);
editor->setNumberStyle(FluentNumberStyle::LowerAlpha);
editor->setNumberStyle(FluentNumberStyle::UpperRoman);

// Headings
editor->insertHeading(1, "Chapter Title");
editor->insertHeading(2, "Section Title");
editor->insertHeading(3, "Subsection Title");

// Horizontal rule
editor->insertHorizontalRule();
```

## Advanced Features

### Tables

```cpp
auto* editor = new FluentRichTextEditor;

// Insert table
editor->insertTable(3, 4);  // 3 rows, 4 columns

// Table manipulation
editor->insertTableRow();
editor->insertTableColumn();
editor->deleteTableRow();
editor->deleteTableColumn();
editor->mergeTableCells();
editor->splitTableCell();

// Table formatting
editor->setTableBorderWidth(1);
editor->setTableBorderColor(QColor(200, 200, 200));
editor->setTableCellPadding(5);
editor->setTableCellSpacing(0);

// Handle table events
connect(editor, &FluentRichTextEditor::tableClicked, [=](int row, int column) {
    qDebug() << "Table cell clicked:" << row << column;
});
```

### Images and Media

```cpp
auto* editor = new FluentRichTextEditor;

// Insert image
editor->insertImage(":/images/photo.jpg");
editor->insertImageFromUrl("https://example.com/image.png");

// Insert image with properties
FluentImageProperties imageProps;
imageProps.width = 300;
imageProps.height = 200;
imageProps.alignment = Qt::AlignCenter;
imageProps.altText = "Sample image";
imageProps.title = "Image title";
editor->insertImage(":/images/photo.jpg", imageProps);

// Handle image events
connect(editor, &FluentRichTextEditor::imageClicked, [=](const QString& imagePath) {
    qDebug() << "Image clicked:" << imagePath;
    // Show image properties dialog
});

// Insert other media
editor->insertVideo("video.mp4");
editor->insertAudio("audio.mp3");
editor->insertEmbed("<iframe src='...'></iframe>");
```

### Links and References

```cpp
auto* editor = new FluentRichTextEditor;

// Insert links
editor->insertLink("https://example.com", "Example Website");
editor->insertEmailLink("user@example.com", "Contact Us");
editor->insertInternalLink("#section1", "Go to Section 1");

// Link formatting
editor->setLinkColor(QColor(0, 120, 215));
editor->setLinkUnderline(true);
editor->setVisitedLinkColor(QColor(128, 0, 128));

// Handle link events
connect(editor, &FluentRichTextEditor::linkClicked, [=](const QString& url) {
    qDebug() << "Link clicked:" << url;
    // Open link or handle internal navigation
});

// Bookmarks and anchors
editor->insertBookmark("chapter1");
editor->insertAnchor("section1", "Section 1");
```

## Document Management

### Save and Load

```cpp
auto* editor = new FluentRichTextEditor;

// Save document
QString html = editor->toHtml();
QString plainText = editor->toPlainText();
QByteArray rtf = editor->toRtf();
QByteArray docx = editor->toDocx();

// Load document
editor->setHtml(html);
editor->setPlainText(plainText);
editor->loadFromFile("document.html");
editor->loadFromFile("document.rtf");
editor->loadFromFile("document.docx");

// Auto-save functionality
auto* autoSaveTimer = new QTimer(this);
connect(autoSaveTimer, &QTimer::timeout, [=]() {
    if (editor->isModified()) {
        saveDocument();
        editor->setModified(false);
    }
});
autoSaveTimer->start(30000);  // Auto-save every 30 seconds
```

### Document Properties

```cpp
auto* editor = new FluentRichTextEditor;

// Set document properties
FluentDocumentProperties props;
props.title = "My Document";
props.author = "John Doe";
props.subject = "Important Document";
props.keywords = {"keyword1", "keyword2"};
props.comments = "Document comments";
props.createdDate = QDateTime::currentDateTime();
props.modifiedDate = QDateTime::currentDateTime();

editor->setDocumentProperties(props);

// Get document statistics
FluentDocumentStats stats = editor->documentStatistics();
qDebug() << "Words:" << stats.wordCount;
qDebug() << "Characters:" << stats.characterCount;
qDebug() << "Paragraphs:" << stats.paragraphCount;
qDebug() << "Pages:" << stats.pageCount;
```

### Version Control

```cpp
auto* editor = new FluentRichTextEditor;

// Enable version tracking
editor->setVersionTrackingEnabled(true);

// Create version snapshot
editor->createVersionSnapshot("Initial draft");
editor->createVersionSnapshot("After review");

// Get version history
QList<FluentDocumentVersion> versions = editor->versionHistory();
for (const auto& version : versions) {
    qDebug() << version.name << version.timestamp;
}

// Restore version
editor->restoreVersion("Initial draft");

// Compare versions
QString diff = editor->compareVersions("Initial draft", "After review");
```

## Collaborative Features

### Track Changes

```cpp
auto* editor = new FluentRichTextEditor;

// Enable track changes
editor->setTrackChangesEnabled(true);
editor->setCurrentUser("John Doe");

// Configure change tracking
editor->setInsertionColor(QColor(0, 120, 0));
editor->setDeletionColor(QColor(200, 0, 0));
editor->setShowChanges(true);

// Handle change events
connect(editor, &FluentRichTextEditor::changeAdded, [=](const FluentTextChange& change) {
    qDebug() << "Change added by" << change.author << ":" << change.text;
});

// Accept/reject changes
editor->acceptChange(changeId);
editor->rejectChange(changeId);
editor->acceptAllChanges();
editor->rejectAllChanges();
```

### Comments and Annotations

```cpp
auto* editor = new FluentRichTextEditor;

// Enable comments
editor->setCommentsEnabled(true);

// Add comment
FluentComment comment;
comment.author = "Jane Smith";
comment.text = "This needs clarification";
comment.timestamp = QDateTime::currentDateTime();
comment.position = editor->textCursor().position();

editor->addComment(comment);

// Handle comment events
connect(editor, &FluentRichTextEditor::commentClicked, [=](const FluentComment& comment) {
    // Show comment dialog
    showCommentDialog(comment);
});

// Reply to comment
editor->replyToComment(commentId, "Thanks for the feedback!");

// Resolve comment
editor->resolveComment(commentId);
```

## Customization

### Custom Toolbar

```cpp
auto* editor = new FluentRichTextEditor;

// Create custom toolbar
auto* customToolbar = new FluentRichTextToolbar;

// Add standard buttons
customToolbar->addButton(FluentRichTextButton::Bold);
customToolbar->addButton(FluentRichTextButton::Italic);
customToolbar->addSeparator();

// Add custom button
auto* customButton = new FluentButton("Custom Action");
customButton->setIcon(QIcon(":/icons/custom"));
connect(customButton, &FluentButton::clicked, [=]() {
    // Custom action
    editor->insertText("[CUSTOM]");
});
customToolbar->addCustomButton(customButton);

// Add dropdown
auto* styleDropdown = new FluentComboBox;
styleDropdown->addItems({"Normal", "Heading 1", "Heading 2", "Quote"});
connect(styleDropdown, &FluentComboBox::currentTextChanged, [=](const QString& style) {
    applyStyle(editor, style);
});
customToolbar->addCustomWidget(styleDropdown);

editor->setCustomToolbar(customToolbar);
```

### Plugins and Extensions

```cpp
// Create custom plugin
class CustomPlugin : public FluentRichTextPlugin
{
public:
    QString name() const override { return "Custom Plugin"; }
    QString version() const override { return "1.0"; }
    
    void initialize(FluentRichTextEditor* editor) override {
        // Plugin initialization
        m_editor = editor;
        
        // Add custom menu items
        auto* menu = editor->contextMenu();
        menu->addSeparator();
        menu->addAction("Custom Action", this, &CustomPlugin::customAction);
    }
    
    void customAction() {
        // Custom plugin action
        m_editor->insertText("[PLUGIN ACTION]");
    }

private:
    FluentRichTextEditor* m_editor{nullptr};
};

// Register plugin
auto* editor = new FluentRichTextEditor;
editor->registerPlugin(new CustomPlugin);

// Load plugins from directory
editor->loadPluginsFromDirectory("plugins/");
```

### Spell Checking

```cpp
auto* editor = new FluentRichTextEditor;

// Enable spell checking
editor->setSpellCheckEnabled(true);
editor->setSpellCheckLanguage("en_US");

// Configure spell checker
editor->setSpellCheckUnderlineColor(QColor(255, 0, 0));
editor->setSpellCheckUnderlineStyle(FluentUnderlineStyle::Wavy);

// Custom dictionary
editor->addWordToDictionary("FluentQt");
editor->removeWordFromDictionary("oldword");

// Handle spell check events
connect(editor, &FluentRichTextEditor::misspelledWordClicked, [=](const QString& word, const QStringList& suggestions) {
    // Show spell check context menu
    showSpellCheckMenu(word, suggestions);
});

// Grammar checking
editor->setGrammarCheckEnabled(true);
editor->setGrammarCheckColor(QColor(0, 0, 255));
```

## Integration Examples

### Document Editor Application

```cpp
class DocumentEditor : public QMainWindow
{
public:
    DocumentEditor(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupMenus();
        setupConnections();
    }

private:
    void setupUI() {
        m_editor = new FluentRichTextEditor;
        m_editor->setToolbarVisible(true);
        setCentralWidget(m_editor);
        
        // Status bar
        m_statusBar = statusBar();
        m_wordCountLabel = new QLabel("Words: 0");
        m_statusBar->addPermanentWidget(m_wordCountLabel);
        
        // Document outline
        m_outlineDock = new QDockWidget("Document Outline", this);
        m_outlineTree = new QTreeWidget;
        m_outlineDock->setWidget(m_outlineTree);
        addDockWidget(Qt::LeftDockWidgetArea, m_outlineDock);
    }
    
    void setupMenus() {
        // File menu
        auto* fileMenu = menuBar()->addMenu("File");
        fileMenu->addAction("New", this, &DocumentEditor::newDocument, QKeySequence::New);
        fileMenu->addAction("Open", this, &DocumentEditor::openDocument, QKeySequence::Open);
        fileMenu->addAction("Save", this, &DocumentEditor::saveDocument, QKeySequence::Save);
        fileMenu->addAction("Save As", this, &DocumentEditor::saveAsDocument, QKeySequence::SaveAs);
        fileMenu->addSeparator();
        fileMenu->addAction("Export to PDF", this, &DocumentEditor::exportToPdf);
        
        // Edit menu
        auto* editMenu = menuBar()->addMenu("Edit");
        editMenu->addAction("Undo", m_editor, &FluentRichTextEditor::undo, QKeySequence::Undo);
        editMenu->addAction("Redo", m_editor, &FluentRichTextEditor::redo, QKeySequence::Redo);
        editMenu->addSeparator();
        editMenu->addAction("Find", this, &DocumentEditor::showFindDialog, QKeySequence::Find);
        editMenu->addAction("Replace", this, &DocumentEditor::showReplaceDialog, QKeySequence::Replace);
        
        // View menu
        auto* viewMenu = menuBar()->addMenu("View");
        viewMenu->addAction("Zoom In", m_editor, &FluentRichTextEditor::zoomIn, QKeySequence::ZoomIn);
        viewMenu->addAction("Zoom Out", m_editor, &FluentRichTextEditor::zoomOut, QKeySequence::ZoomOut);
        viewMenu->addAction("Reset Zoom", m_editor, &FluentRichTextEditor::resetZoom);
        viewMenu->addSeparator();
        viewMenu->addAction("Full Screen", this, &DocumentEditor::toggleFullScreen, QKeySequence::FullScreen);
    }
    
    void setupConnections() {
        // Update word count
        connect(m_editor, &FluentRichTextEditor::contentChanged, [=]() {
            auto stats = m_editor->documentStatistics();
            m_wordCountLabel->setText(QString("Words: %1").arg(stats.wordCount));
            updateDocumentOutline();
        });
        
        // Update window title
        connect(m_editor, &FluentRichTextEditor::modificationChanged, [=](bool modified) {
            QString title = m_currentFileName.isEmpty() ? "Untitled" : QFileInfo(m_currentFileName).baseName();
            if (modified) title += "*";
            setWindowTitle(title + " - Document Editor");
        });
    }
    
    void updateDocumentOutline() {
        m_outlineTree->clear();
        
        // Parse document for headings
        QTextDocument* doc = m_editor->document();
        QTextBlock block = doc->begin();
        
        while (block.isValid()) {
            QTextBlockFormat format = block.blockFormat();
            if (format.hasProperty(QTextFormat::BlockTraits)) {
                int headingLevel = format.headingLevel();
                if (headingLevel > 0) {
                    auto* item = new QTreeWidgetItem;
                    item->setText(0, block.text());
                    item->setData(0, Qt::UserRole, block.position());
                    
                    // Add to appropriate parent based on heading level
                    addToOutline(item, headingLevel);
                }
            }
            block = block.next();
        }
    }

private:
    FluentRichTextEditor* m_editor{nullptr};
    QStatusBar* m_statusBar{nullptr};
    QLabel* m_wordCountLabel{nullptr};
    QDockWidget* m_outlineDock{nullptr};
    QTreeWidget* m_outlineTree{nullptr};
    QString m_currentFileName;
};
```

## Accessibility

FluentRichTextEditor provides comprehensive accessibility support:

```cpp
auto* editor = new FluentRichTextEditor;

// Accessible properties (automatically set)
editor->setAccessibleName("Rich text editor");
editor->setAccessibleDescription("Edit formatted text documents");

// Keyboard shortcuts (automatic)
// - Ctrl+B for bold, Ctrl+I for italic, etc.
// - Standard text editing shortcuts
// - Tab navigation through toolbar

// Screen reader support
editor->setAccessibleRole(QAccessible::EditableText);

// Custom accessibility announcements
connect(editor, &FluentRichTextEditor::formattingChanged, [=]() {
    QString announcement = "Formatting changed to ";
    if (editor->isBold()) announcement += "bold ";
    if (editor->isItalic()) announcement += "italic ";
    editor->announceToScreenReader(announcement);
});
```

## Best Practices

### Editor Design Guidelines
1. **Provide clear formatting feedback** - Show current formatting state
2. **Use consistent keyboard shortcuts** - Follow standard conventions
3. **Enable auto-save** - Prevent data loss
4. **Organize toolbar logically** - Group related functions
5. **Support undo/redo** - Allow easy mistake correction

### Performance Considerations
- Use lazy loading for large documents
- Implement virtual scrolling for very long documents
- Cache formatted content when possible
- Optimize spell checking for real-time performance

### Accessibility Guidelines
- Support keyboard navigation for all features
- Provide screen reader announcements for formatting changes
- Ensure sufficient color contrast
- Include alternative text for images
- Test with assistive technologies

## Signals Reference

```cpp
// Content signals
void contentChanged();
void textChanged();
void modificationChanged(bool modified);

// Formatting signals
void formattingChanged();
void fontChanged(const QFont& font);
void colorChanged(const QColor& color);

// Selection signals
void selectionChanged();
void cursorPositionChanged();

// Document signals
void documentLoaded();
void documentSaved();
```

## See Also

- [FluentTextInput](FluentTextInput.md) - For simple text input
- [FluentButton](FluentButton.md) - For toolbar buttons
- [FluentComboBox](FluentComboBox.md) - For formatting dropdowns
- [API Reference - FluentRichTextEditor](../API_REFERENCE.md#fluentrichtexteditor) - Complete API documentation
