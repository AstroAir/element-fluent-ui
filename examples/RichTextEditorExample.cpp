// examples/RichTextEditorExample.cpp
#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTextDocumentWriter>
#include <QTextEdit>
#include <QTextStream>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentRichTextEditor.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class RichTextEditorExample : public QMainWindow {
    Q_OBJECT

public:
    RichTextEditorExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupMenuBar();
        setupToolBar();
        setupStatusBar();
        setupConnections();
        setupTheme();
        loadSampleContent();
    }

private slots:
    void newDocument();
    void openDocument();
    void saveDocument();
    void saveAsDocument();
    void exportToPdf();
    void exportToHtml();
    void showFindReplace();
    void showDocumentStats();
    void toggleReadOnly();
    void toggleFormatBar();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void toggleTheme();
    void onTextChanged();
    void onSelectionChanged();
    void onZoomChanged(int percent);
    void updateWordCount();
    void insertSampleTable();
    void insertSampleImage();
    void insertSampleLink();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    void setupTheme();
    void loadSampleContent();
    void createControlPanel();
    void createEditorArea();
    void createPreviewArea();
    void updateDocumentStats();
    void animateStatusUpdate();

private:
    // UI Components
    QWidget* m_centralWidget{nullptr};
    QSplitter* m_mainSplitter{nullptr};
    QSplitter* m_editorSplitter{nullptr};

    // Control Panel
    QGroupBox* m_controlGroup{nullptr};
    Components::FluentButton* m_newButton{nullptr};
    Components::FluentButton* m_openButton{nullptr};
    Components::FluentButton* m_saveButton{nullptr};
    Components::FluentButton* m_exportPdfButton{nullptr};
    Components::FluentButton* m_exportHtmlButton{nullptr};
    Components::FluentButton* m_readOnlyButton{nullptr};
    Components::FluentButton* m_formatBarButton{nullptr};
    Components::FluentButton* m_themeButton{nullptr};

    // Zoom controls
    QSlider* m_zoomSlider{nullptr};
    QLabel* m_zoomLabel{nullptr};
    Components::FluentButton* m_zoomInButton{nullptr};
    Components::FluentButton* m_zoomOutButton{nullptr};
    Components::FluentButton* m_resetZoomButton{nullptr};

    // Insert controls
    Components::FluentButton* m_insertTableButton{nullptr};
    Components::FluentButton* m_insertImageButton{nullptr};
    Components::FluentButton* m_insertLinkButton{nullptr};

    // Editor Area
    Components::FluentPanel* m_editorPanel{nullptr};
    Components::FluentRichTextEditor* m_editor{nullptr};

    // Preview Area
    Components::FluentPanel* m_previewPanel{nullptr};
    QTextEdit* m_htmlPreview{nullptr};

    // Status and Statistics
    QLabel* m_wordCountLabel{nullptr};
    QLabel* m_charCountLabel{nullptr};
    QLabel* m_lineCountLabel{nullptr};
    QLabel* m_statusLabel{nullptr};
    QProgressBar* m_saveProgress{nullptr};

    // Menu and Toolbar
    QMenuBar* m_menuBar{nullptr};
    QToolBar* m_toolBar{nullptr};
    QStatusBar* m_statusBar{nullptr};

    // Data and Animation
    Animation::FluentAnimator* m_animator{nullptr};
    QTimer* m_statsUpdateTimer{nullptr};
    QString m_currentFileName;
    bool m_documentModified{false};
    int m_currentZoom{100};
};

void RichTextEditorExample::setupUI() {
    setWindowTitle("FluentQt Rich Text Editor Example");
    setMinimumSize(1200, 800);
    resize(1400, 900);

    // Create central widget and main layout
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, m_centralWidget);
    auto* mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->addWidget(m_mainSplitter);

    // Create control panel, editor area, and preview area
    createControlPanel();
    createEditorArea();
    createPreviewArea();

    // Set splitter proportions
    m_mainSplitter->setSizes({300, 700, 400});
    m_mainSplitter->setCollapsible(0, false);
    m_mainSplitter->setCollapsible(1, false);
    m_mainSplitter->setCollapsible(2, true);

    // Initialize animator
    m_animator = new Animation::FluentAnimator(this);

    // Setup update timer
    m_statsUpdateTimer = new QTimer(this);
    m_statsUpdateTimer->setSingleShot(true);
    m_statsUpdateTimer->setInterval(
        500);  // Update stats 500ms after text changes
}

void RichTextEditorExample::createControlPanel() {
    m_controlGroup = new QGroupBox("Document Controls", this);
    auto* layout = new QVBoxLayout(m_controlGroup);

    // File operations
    auto* fileGroup = new QGroupBox("File Operations", this);
    auto* fileLayout = new QGridLayout(fileGroup);

    m_newButton = new Components::FluentButton("New", this);
    m_newButton->setIcon(QIcon(":/icons/new.png"));
    fileLayout->addWidget(m_newButton, 0, 0);

    m_openButton = new Components::FluentButton("Open", this);
    m_openButton->setIcon(QIcon(":/icons/open.png"));
    fileLayout->addWidget(m_openButton, 0, 1);

    m_saveButton = new Components::FluentButton("Save", this);
    m_saveButton->setIcon(QIcon(":/icons/save.png"));
    fileLayout->addWidget(m_saveButton, 1, 0);

    m_exportPdfButton = new Components::FluentButton("Export PDF", this);
    m_exportPdfButton->setIcon(QIcon(":/icons/pdf.png"));
    fileLayout->addWidget(m_exportPdfButton, 1, 1);

    m_exportHtmlButton = new Components::FluentButton("Export HTML", this);
    m_exportHtmlButton->setIcon(QIcon(":/icons/html.png"));
    fileLayout->addWidget(m_exportHtmlButton, 2, 0, 1, 2);

    layout->addWidget(fileGroup);

    // View controls
    auto* viewGroup = new QGroupBox("View Controls", this);
    auto* viewLayout = new QGridLayout(viewGroup);

    m_readOnlyButton = new Components::FluentButton("Read Only", this);
    m_readOnlyButton->setCheckable(true);
    viewLayout->addWidget(m_readOnlyButton, 0, 0);

    m_formatBarButton = new Components::FluentButton("Format Bar", this);
    m_formatBarButton->setCheckable(true);
    m_formatBarButton->setChecked(true);
    viewLayout->addWidget(m_formatBarButton, 0, 1);

    m_themeButton = new Components::FluentButton("Dark Theme", this);
    m_themeButton->setCheckable(true);
    viewLayout->addWidget(m_themeButton, 1, 0, 1, 2);

    layout->addWidget(viewGroup);

    // Zoom controls
    auto* zoomGroup = new QGroupBox("Zoom Controls", this);
    auto* zoomLayout = new QVBoxLayout(zoomGroup);

    m_zoomLabel = new QLabel("Zoom: 100%", this);
    m_zoomLabel->setAlignment(Qt::AlignCenter);
    zoomLayout->addWidget(m_zoomLabel);

    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setRange(50, 200);
    m_zoomSlider->setValue(100);
    m_zoomSlider->setTickPosition(QSlider::TicksBelow);
    m_zoomSlider->setTickInterval(25);
    zoomLayout->addWidget(m_zoomSlider);

    auto* zoomButtonLayout = new QHBoxLayout();
    m_zoomInButton = new Components::FluentButton("+", this);
    m_zoomOutButton = new Components::FluentButton("-", this);
    m_resetZoomButton = new Components::FluentButton("Reset", this);

    zoomButtonLayout->addWidget(m_zoomOutButton);
    zoomButtonLayout->addWidget(m_resetZoomButton);
    zoomButtonLayout->addWidget(m_zoomInButton);
    zoomLayout->addLayout(zoomButtonLayout);

    layout->addWidget(zoomGroup);

    // Insert controls
    auto* insertGroup = new QGroupBox("Insert Elements", this);
    auto* insertLayout = new QGridLayout(insertGroup);

    m_insertTableButton = new Components::FluentButton("Table", this);
    m_insertTableButton->setIcon(QIcon(":/icons/table.png"));
    insertLayout->addWidget(m_insertTableButton, 0, 0);

    m_insertImageButton = new Components::FluentButton("Image", this);
    m_insertImageButton->setIcon(QIcon(":/icons/image.png"));
    insertLayout->addWidget(m_insertImageButton, 0, 1);

    m_insertLinkButton = new Components::FluentButton("Link", this);
    m_insertLinkButton->setIcon(QIcon(":/icons/link.png"));
    insertLayout->addWidget(m_insertLinkButton, 1, 0, 1, 2);

    layout->addWidget(insertGroup);

    // Document statistics
    auto* statsGroup = new QGroupBox("Document Statistics", this);
    auto* statsLayout = new QVBoxLayout(statsGroup);

    m_wordCountLabel = new QLabel("Words: 0", this);
    m_charCountLabel = new QLabel("Characters: 0", this);
    m_lineCountLabel = new QLabel("Lines: 0", this);

    statsLayout->addWidget(m_wordCountLabel);
    statsLayout->addWidget(m_charCountLabel);
    statsLayout->addWidget(m_lineCountLabel);

    layout->addWidget(statsGroup);

    layout->addStretch();
    m_mainSplitter->addWidget(m_controlGroup);
}

void RichTextEditorExample::createEditorArea() {
    m_editorPanel = new Components::FluentPanel("Rich Text Editor", this);
    m_editorPanel->setPanelType(Components::FluentPanelType::Card);
    m_editorPanel->setElevation(Components::FluentPanelElevation::Medium);

    auto* layout = m_editorPanel->createVerticalLayout();

    // Create the rich text editor
    m_editor = new Components::FluentRichTextEditor(this);
    m_editor->setShowFormatBar(true);
    m_editor->setMinimumHeight(500);
    layout->addWidget(m_editor);

    m_mainSplitter->addWidget(m_editorPanel);
}

void RichTextEditorExample::createPreviewArea() {
    m_previewPanel = new Components::FluentPanel("HTML Preview", this);
    m_previewPanel->setPanelType(Components::FluentPanelType::Surface);

    auto* layout = m_previewPanel->createVerticalLayout();

    // HTML preview text edit (read-only)
    m_htmlPreview = new QTextEdit(this);
    m_htmlPreview->setReadOnly(true);
    m_htmlPreview->setFont(QFont("Consolas", 10));
    m_htmlPreview->setMinimumHeight(400);
    layout->addWidget(m_htmlPreview);

    m_mainSplitter->addWidget(m_previewPanel);
}

void RichTextEditorExample::setupMenuBar() {
    m_menuBar = menuBar();

    // File menu
    auto* fileMenu = m_menuBar->addMenu("&File");
    fileMenu->addAction("&New", this, &RichTextEditorExample::newDocument,
                        QKeySequence::New);
    fileMenu->addAction("&Open", this, &RichTextEditorExample::openDocument,
                        QKeySequence::Open);
    fileMenu->addAction("&Save", this, &RichTextEditorExample::saveDocument,
                        QKeySequence::Save);
    fileMenu->addAction("Save &As", this,
                        &RichTextEditorExample::saveAsDocument,
                        QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction("Export to &PDF", this,
                        &RichTextEditorExample::exportToPdf);
    fileMenu->addAction("Export to &HTML", this,
                        &RichTextEditorExample::exportToHtml);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

    // Edit menu
    auto* editMenu = m_menuBar->addMenu("&Edit");
    editMenu->addAction("&Undo", m_editor,
                        &Components::FluentRichTextEditor::undo,
                        QKeySequence::Undo);
    editMenu->addAction("&Redo", m_editor,
                        &Components::FluentRichTextEditor::redo,
                        QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("Cu&t", m_editor,
                        &Components::FluentRichTextEditor::cut,
                        QKeySequence::Cut);
    editMenu->addAction("&Copy", m_editor,
                        &Components::FluentRichTextEditor::copy,
                        QKeySequence::Copy);
    editMenu->addAction("&Paste", m_editor,
                        &Components::FluentRichTextEditor::paste,
                        QKeySequence::Paste);
    editMenu->addSeparator();
    editMenu->addAction("Select &All", m_editor,
                        &Components::FluentRichTextEditor::selectAll,
                        QKeySequence::SelectAll);
    editMenu->addAction("&Find/Replace", this,
                        &RichTextEditorExample::showFindReplace,
                        QKeySequence::Find);

    // View menu
    auto* viewMenu = m_menuBar->addMenu("&View");
    viewMenu->addAction("Zoom &In", this, &RichTextEditorExample::zoomIn,
                        QKeySequence::ZoomIn);
    viewMenu->addAction("Zoom &Out", this, &RichTextEditorExample::zoomOut,
                        QKeySequence::ZoomOut);
    viewMenu->addAction("&Reset Zoom", this, &RichTextEditorExample::resetZoom);
    viewMenu->addSeparator();
    viewMenu->addAction("Toggle &Read Only", this,
                        &RichTextEditorExample::toggleReadOnly);
    viewMenu->addAction("Toggle &Format Bar", this,
                        &RichTextEditorExample::toggleFormatBar);
    viewMenu->addAction("Toggle &Theme", this,
                        &RichTextEditorExample::toggleTheme);

    // Insert menu
    auto* insertMenu = m_menuBar->addMenu("&Insert");
    insertMenu->addAction("&Table", this,
                          &RichTextEditorExample::insertSampleTable);
    insertMenu->addAction("&Image", this,
                          &RichTextEditorExample::insertSampleImage);
    insertMenu->addAction("&Link", this,
                          &RichTextEditorExample::insertSampleLink);

    // Tools menu
    auto* toolsMenu = m_menuBar->addMenu("&Tools");
    toolsMenu->addAction("Document &Statistics", this,
                         &RichTextEditorExample::showDocumentStats);
}

void RichTextEditorExample::setupToolBar() {
    m_toolBar = addToolBar("Main Toolbar");
    m_toolBar->setMovable(false);

    // File operations
    m_toolBar->addAction(QIcon(":/icons/new.png"), "New", this,
                         &RichTextEditorExample::newDocument);
    m_toolBar->addAction(QIcon(":/icons/open.png"), "Open", this,
                         &RichTextEditorExample::openDocument);
    m_toolBar->addAction(QIcon(":/icons/save.png"), "Save", this,
                         &RichTextEditorExample::saveDocument);
    m_toolBar->addSeparator();

    // Edit operations
    m_toolBar->addAction(QIcon(":/icons/undo.png"), "Undo", m_editor,
                         &Components::FluentRichTextEditor::undo);
    m_toolBar->addAction(QIcon(":/icons/redo.png"), "Redo", m_editor,
                         &Components::FluentRichTextEditor::redo);
    m_toolBar->addSeparator();

    // Zoom operations
    m_toolBar->addAction(QIcon(":/icons/zoom-in.png"), "Zoom In", this,
                         &RichTextEditorExample::zoomIn);
    m_toolBar->addAction(QIcon(":/icons/zoom-out.png"), "Zoom Out", this,
                         &RichTextEditorExample::zoomOut);
    m_toolBar->addAction(QIcon(":/icons/zoom-reset.png"), "Reset Zoom", this,
                         &RichTextEditorExample::resetZoom);
}

void RichTextEditorExample::setupStatusBar() {
    m_statusBar = statusBar();

    // Status label
    m_statusLabel = new QLabel("Ready", this);
    m_statusBar->addWidget(m_statusLabel);

    // Progress bar for save operations
    m_saveProgress = new QProgressBar(this);
    m_saveProgress->setVisible(false);
    m_saveProgress->setMaximumWidth(200);
    m_statusBar->addPermanentWidget(m_saveProgress);

    // Document statistics in status bar
    m_statusBar->addPermanentWidget(new QLabel(" | ", this));

    auto* wordLabel = new QLabel("Words: 0", this);
    m_statusBar->addPermanentWidget(wordLabel);
    m_wordCountLabel = wordLabel;

    auto* charLabel = new QLabel("Chars: 0", this);
    m_statusBar->addPermanentWidget(charLabel);
    m_charCountLabel = charLabel;
}

void RichTextEditorExample::setupConnections() {
    // File operations
    connect(m_newButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::newDocument);
    connect(m_openButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::openDocument);
    connect(m_saveButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::saveDocument);
    connect(m_exportPdfButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::exportToPdf);
    connect(m_exportHtmlButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::exportToHtml);

    // View controls
    connect(m_readOnlyButton, &Components::FluentButton::toggled, this,
            &RichTextEditorExample::toggleReadOnly);
    connect(m_formatBarButton, &Components::FluentButton::toggled, this,
            &RichTextEditorExample::toggleFormatBar);
    connect(m_themeButton, &Components::FluentButton::toggled, this,
            &RichTextEditorExample::toggleTheme);

    // Zoom controls
    connect(m_zoomSlider, &QSlider::valueChanged, this, [this](int value) {
        m_editor->zoomTo(value);
        m_currentZoom = value;
        m_zoomLabel->setText(QString("Zoom: %1%").arg(value));
    });
    connect(m_zoomInButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::zoomIn);
    connect(m_zoomOutButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::zoomOut);
    connect(m_resetZoomButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::resetZoom);

    // Insert controls
    connect(m_insertTableButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::insertSampleTable);
    connect(m_insertImageButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::insertSampleImage);
    connect(m_insertLinkButton, &Components::FluentButton::clicked, this,
            &RichTextEditorExample::insertSampleLink);

    // Editor events
    connect(m_editor, &Components::FluentRichTextEditor::textChanged, this,
            &RichTextEditorExample::onTextChanged);
    connect(m_editor, &Components::FluentRichTextEditor::selectionChanged, this,
            &RichTextEditorExample::onSelectionChanged);
    connect(m_editor, &Components::FluentRichTextEditor::zoomChanged, this,
            &RichTextEditorExample::onZoomChanged);

    // Stats update timer
    connect(m_statsUpdateTimer, &QTimer::timeout, this,
            &RichTextEditorExample::updateWordCount);
}

void RichTextEditorExample::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    // Theme will be applied automatically to FluentQt components
}

void RichTextEditorExample::loadSampleContent() {
    QString sampleHtml = R"(
        <h1>Welcome to FluentQt Rich Text Editor</h1>
        <p>This is a comprehensive example demonstrating the powerful rich text editing capabilities of FluentQt.</p>

        <h2>Features Demonstrated</h2>
        <ul>
            <li><strong>Rich text formatting</strong> with bold, italic, underline, and strikethrough</li>
            <li><em>Font family and size selection</em></li>
            <li><span style="color: blue;">Text and background colors</span></li>
            <li>Paragraph alignment and indentation</li>
            <li>Lists (bulleted, numbered, and checklists)</li>
            <li>Tables and images</li>
            <li>Hyperlinks and document structure</li>
        </ul>

        <h2>Sample Table</h2>
        <table border="1" cellpadding="4" cellspacing="0">
            <tr>
                <th>Feature</th>
                <th>Status</th>
                <th>Description</th>
            </tr>
            <tr>
                <td>Text Formatting</td>
                <td>✓ Complete</td>
                <td>Full support for character formatting</td>
            </tr>
            <tr>
                <td>Document Export</td>
                <td>✓ Complete</td>
                <td>Export to PDF and HTML formats</td>
            </tr>
            <tr>
                <td>Real-time Preview</td>
                <td>✓ Complete</td>
                <td>Live HTML preview as you type</td>
            </tr>
        </table>

        <h2>Try It Out!</h2>
        <p>Use the formatting toolbar above to modify this text. Try changing fonts, colors, alignment, and adding your own content.</p>

        <blockquote>
            <p><em>"The best way to learn is by doing. Start editing this document and explore all the features!"</em></p>
        </blockquote>

        <p>You can also use the control panel on the left to:</p>
        <ol>
            <li>Create new documents</li>
            <li>Open existing files</li>
            <li>Save your work</li>
            <li>Export to different formats</li>
            <li>Adjust zoom levels</li>
            <li>Insert tables, images, and links</li>
        </ol>
    )";

    m_editor->setHtml(sampleHtml);
    m_htmlPreview->setPlainText(sampleHtml);
    updateWordCount();
}

// Slot implementations
void RichTextEditorExample::newDocument() {
    if (m_documentModified) {
        auto reply = QMessageBox::question(
            this, "New Document",
            "The current document has unsaved changes. Do you want to save it "
            "first?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveDocument();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    m_editor->clear();
    m_currentFileName.clear();
    m_documentModified = false;
    setWindowTitle("FluentQt Rich Text Editor Example - New Document");
    m_statusLabel->setText("New document created");

    // Animate the clear action
    Animation::FluentAnimationConfig config;
    config.duration = std::chrono::milliseconds(300);
    auto fadeAnimation = Animation::FluentAnimator::fadeIn(m_editor, config);
    fadeAnimation->start();
}

void RichTextEditorExample::openDocument() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "Open Document",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "HTML Files (*.html *.htm);;Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();

            if (fileName.endsWith(".html") || fileName.endsWith(".htm")) {
                m_editor->setHtml(content);
            } else {
                m_editor->setPlainText(content);
            }

            m_currentFileName = fileName;
            m_documentModified = false;
            setWindowTitle(QString("FluentQt Rich Text Editor Example - %1")
                               .arg(QFileInfo(fileName).fileName()));
            m_statusLabel->setText(
                QString("Opened: %1").arg(QFileInfo(fileName).fileName()));

            animateStatusUpdate();
        } else {
            QMessageBox::warning(this, "Error",
                                 "Could not open file: " + fileName);
        }
    }
}

void RichTextEditorExample::saveDocument() {
    if (m_currentFileName.isEmpty()) {
        saveAsDocument();
        return;
    }

    QFile file(m_currentFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << m_editor->toHtml();

        m_documentModified = false;
        m_statusLabel->setText(
            QString("Saved: %1").arg(QFileInfo(m_currentFileName).fileName()));
        animateStatusUpdate();
    } else {
        QMessageBox::warning(this, "Error",
                             "Could not save file: " + m_currentFileName);
    }
}

void RichTextEditorExample::saveAsDocument() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save Document",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "HTML Files (*.html);;Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        m_currentFileName = fileName;
        saveDocument();
        setWindowTitle(QString("FluentQt Rich Text Editor Example - %1")
                           .arg(QFileInfo(fileName).fileName()));
    }
}

void RichTextEditorExample::exportToPdf() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "Export to PDF",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "PDF Files (*.pdf)");

    if (!fileName.isEmpty()) {
        if (m_editor->exportToPdf(fileName)) {
            m_statusLabel->setText("Exported to PDF successfully");
            animateStatusUpdate();
        } else {
            QMessageBox::warning(this, "Error", "Could not export to PDF");
        }
    }
}

void RichTextEditorExample::exportToHtml() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "Export to HTML",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "HTML Files (*.html)");

    if (!fileName.isEmpty()) {
        if (m_editor->exportToHtml(fileName)) {
            m_statusLabel->setText("Exported to HTML successfully");
            animateStatusUpdate();
        } else {
            QMessageBox::warning(this, "Error", "Could not export to HTML");
        }
    }
}

void RichTextEditorExample::showFindReplace() {
    // This would show a find/replace dialog
    m_statusLabel->setText(
        "Find/Replace functionality would be implemented here");
}

void RichTextEditorExample::showDocumentStats() {
    QString stats =
        QString(
            "Document Statistics:\n\n"
            "Words: %1\n"
            "Characters: %2\n"
            "Characters (no spaces): %3\n"
            "Lines: %4\n"
            "Paragraphs: %5")
            .arg(m_editor->toPlainText()
                     .split(QRegularExpression("\\s+"), Qt::SkipEmptyParts)
                     .count())
            .arg(m_editor->toPlainText().length())
            .arg(m_editor->toPlainText().remove(' ').length())
            .arg(m_editor->toPlainText().split('\n').count())
            .arg(m_editor->document()->blockCount());

    QMessageBox::information(this, "Document Statistics", stats);
}

void RichTextEditorExample::toggleReadOnly() {
    bool readOnly = m_readOnlyButton->isChecked();
    m_editor->setReadOnly(readOnly);
    m_statusLabel->setText(readOnly ? "Document is now read-only"
                                    : "Document is now editable");
    animateStatusUpdate();
}

void RichTextEditorExample::toggleFormatBar() {
    bool showFormatBar = m_formatBarButton->isChecked();
    m_editor->setShowFormatBar(showFormatBar);
    m_statusLabel->setText(showFormatBar ? "Format bar shown"
                                         : "Format bar hidden");
}

void RichTextEditorExample::zoomIn() {
    int newZoom = qMin(200, m_currentZoom + 10);
    m_zoomSlider->setValue(newZoom);
}

void RichTextEditorExample::zoomOut() {
    int newZoom = qMax(50, m_currentZoom - 10);
    m_zoomSlider->setValue(newZoom);
}

void RichTextEditorExample::resetZoom() { m_zoomSlider->setValue(100); }

void RichTextEditorExample::toggleTheme() {
    auto& theme = Styling::FluentTheme::instance();
    bool isDark = m_themeButton->isChecked();

    // Theme switching would be implemented here
    m_themeButton->setText(isDark ? "Light Theme" : "Dark Theme");
    m_statusLabel->setText(isDark ? "Switched to dark theme"
                                  : "Switched to light theme");
    animateStatusUpdate();
}

void RichTextEditorExample::onTextChanged() {
    m_documentModified = true;

    // Update HTML preview
    m_htmlPreview->setPlainText(m_editor->toHtml());

    // Start timer for stats update (debounced)
    m_statsUpdateTimer->start();
}

void RichTextEditorExample::onSelectionChanged() {
    QString selectedText = m_editor->selectedText();
    if (!selectedText.isEmpty()) {
        m_statusLabel->setText(
            QString("Selected: %1 characters").arg(selectedText.length()));
    } else {
        m_statusLabel->setText("Ready");
    }
}

void RichTextEditorExample::onZoomChanged(int percent) {
    m_currentZoom = percent;
    m_zoomSlider->setValue(percent);
    m_zoomLabel->setText(QString("Zoom: %1%").arg(percent));
}

void RichTextEditorExample::updateWordCount() {
    QString text = m_editor->toPlainText();
    int wordCount =
        text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();
    int charCount = text.length();
    int lineCount = text.split('\n').count();

    m_wordCountLabel->setText(QString("Words: %1").arg(wordCount));
    m_charCountLabel->setText(QString("Chars: %1").arg(charCount));
    m_lineCountLabel->setText(QString("Lines: %1").arg(lineCount));
}

void RichTextEditorExample::insertSampleTable() {
    m_editor->insertTable(3, 4);
    m_statusLabel->setText("Sample table inserted");
    animateStatusUpdate();
}

void RichTextEditorExample::insertSampleImage() {
    // In a real application, this would open a file dialog
    m_statusLabel->setText("Image insertion would be implemented here");
}

void RichTextEditorExample::insertSampleLink() {
    m_editor->insertLink("https://www.example.com", "Example Link");
    m_statusLabel->setText("Sample link inserted");
    animateStatusUpdate();
}

void RichTextEditorExample::updateDocumentStats() { updateWordCount(); }

void RichTextEditorExample::animateStatusUpdate() {
    // Animate the status label to draw attention to updates
    Animation::FluentAnimationConfig config;
    config.duration = std::chrono::milliseconds(200);
    config.easing = Animation::FluentEasing::EaseOutQuad;

    auto scaleAnimation =
        Animation::FluentAnimator::scaleIn(m_statusLabel, config);
    scaleAnimation->start();
}

// Main function
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("FluentQt Rich Text Editor Example");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt Examples");

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    // Theme will be initialized automatically

    RichTextEditorExample window;
    window.show();

    return app.exec();
}

#include "RichTextEditorExample.moc"
