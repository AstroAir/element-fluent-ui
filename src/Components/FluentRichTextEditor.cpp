// src/Components/FluentRichTextEditor.cpp
#include "FluentQt/Components/FluentRichTextEditor.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPageSetupDialog>
#include <QPaintEvent>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QResizeEvent>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QUrl>
#include <QWheelEvent>

namespace FluentQt::Components {

// FluentTextFormatBar Implementation
FluentTextFormatBar::FluentTextFormatBar(QWidget* parent) : QWidget(parent) {
    setupFontControls();
    setupFormatControls();
    setupAlignmentControls();
    setupListControls();
    setupInsertControls();

    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(8, 4, 8, 4);
    m_layout->setSpacing(4);

    // Add controls to layout
    m_layout->addWidget(m_fontCombo);
    m_layout->addWidget(m_fontSizeSpinBox);
    m_layout->addWidget(createSeparator());

    m_layout->addWidget(m_boldButton);
    m_layout->addWidget(m_italicButton);
    m_layout->addWidget(m_underlineButton);
    m_layout->addWidget(m_strikethroughButton);
    m_layout->addWidget(createSeparator());

    m_layout->addWidget(m_textColorButton);
    m_layout->addWidget(m_backgroundColorButton);
    m_layout->addWidget(createSeparator());

    m_layout->addWidget(m_alignLeftButton);
    m_layout->addWidget(m_alignCenterButton);
    m_layout->addWidget(m_alignRightButton);
    m_layout->addWidget(m_alignJustifyButton);
    m_layout->addWidget(createSeparator());

    m_layout->addWidget(m_listTypeCombo);
    m_layout->addWidget(m_indentButton);
    m_layout->addWidget(m_outdentButton);
    m_layout->addWidget(createSeparator());

    m_layout->addWidget(m_linkButton);
    m_layout->addWidget(m_imageButton);
    m_layout->addWidget(m_tableButton);

    m_layout->addStretch();

    // Style the format bar
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    setStyleSheet(QString(R"(
        FluentTextFormatBar {
            background-color: %1;
            border-bottom: 1px solid %2;
        }
    )")
                      .arg(palette.neutralLighter.name(),
                           palette.neutralQuaternary.name()));
}

void FluentTextFormatBar::updateFromCursor(const QTextCursor& cursor) {
    if (m_updating)
        return;

    m_updating = true;

    QTextCharFormat charFormat = cursor.charFormat();
    QTextBlockFormat blockFormat = cursor.blockFormat();

    updateFontControls(charFormat);
    updateFormatControls(charFormat);
    updateAlignmentControls(blockFormat);
    updateListControls(cursor);

    m_updating = false;
}

void FluentTextFormatBar::setupFontControls() {
    m_fontCombo = new QFontComboBox(this);
    m_fontCombo->setMaximumWidth(150);
    connect(m_fontCombo,
            QOverload<const QString&>::of(&QFontComboBox::currentTextChanged),
            this, &FluentTextFormatBar::onFontFamilyChanged);

    m_fontSizeSpinBox = new QSpinBox(this);
    m_fontSizeSpinBox->setRange(6, 72);
    m_fontSizeSpinBox->setValue(12);
    m_fontSizeSpinBox->setMaximumWidth(60);
    connect(m_fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FluentTextFormatBar::onFontSizeChanged);
}

void FluentTextFormatBar::setupFormatControls() {
    m_boldButton = new FluentButton("B", this);
    m_boldButton->setCheckable(true);
    m_boldButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_boldButton->setFixedSize(32, 32);
    m_boldButton->setFont(
        QFont(font().family(), font().pointSize(), QFont::Bold));
    connect(m_boldButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onBoldClicked);

    m_italicButton = new FluentButton("I", this);
    m_italicButton->setCheckable(true);
    m_italicButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_italicButton->setFixedSize(32, 32);
    m_italicButton->setFont(
        QFont(font().family(), font().pointSize(), QFont::Normal, true));
    connect(m_italicButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onItalicClicked);

    m_underlineButton = new FluentButton("U", this);
    m_underlineButton->setCheckable(true);
    m_underlineButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_underlineButton->setFixedSize(32, 32);
    QFont underlineFont = font();
    underlineFont.setUnderline(true);
    m_underlineButton->setFont(underlineFont);
    connect(m_underlineButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onUnderlineClicked);

    m_strikethroughButton = new FluentButton("S", this);
    m_strikethroughButton->setCheckable(true);
    m_strikethroughButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_strikethroughButton->setFixedSize(32, 32);
    QFont strikeFont = font();
    strikeFont.setStrikeOut(true);
    m_strikethroughButton->setFont(strikeFont);
    connect(m_strikethroughButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onStrikethroughClicked);

    m_textColorButton = new FluentButton("A", this);
    m_textColorButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_textColorButton->setFixedSize(32, 32);
    connect(m_textColorButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onTextColorClicked);

    m_backgroundColorButton = new FluentButton("H", this);
    m_backgroundColorButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_backgroundColorButton->setFixedSize(32, 32);
    connect(m_backgroundColorButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onBackgroundColorClicked);
}

void FluentTextFormatBar::setupAlignmentControls() {
    m_alignLeftButton = new FluentButton("⇤", this);
    m_alignLeftButton->setCheckable(true);
    m_alignLeftButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_alignLeftButton->setFixedSize(32, 32);

    m_alignCenterButton = new FluentButton("⇋", this);
    m_alignCenterButton->setCheckable(true);
    m_alignCenterButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_alignCenterButton->setFixedSize(32, 32);

    m_alignRightButton = new FluentButton("⇥", this);
    m_alignRightButton->setCheckable(true);
    m_alignRightButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_alignRightButton->setFixedSize(32, 32);

    m_alignJustifyButton = new FluentButton("⇿", this);
    m_alignJustifyButton->setCheckable(true);
    m_alignJustifyButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_alignJustifyButton->setFixedSize(32, 32);

    // Connect buttons manually since FluentButton doesn't inherit from
    // QAbstractButton
    connect(m_alignLeftButton, &FluentButton::clicked, this, [this]() {
        m_alignCenterButton->setChecked(false);
        m_alignRightButton->setChecked(false);
        m_alignJustifyButton->setChecked(false);
        m_alignLeftButton->setChecked(true);
        emit alignmentChanged(FluentTextAlignment::Left);
    });

    connect(m_alignCenterButton, &FluentButton::clicked, this, [this]() {
        m_alignLeftButton->setChecked(false);
        m_alignRightButton->setChecked(false);
        m_alignJustifyButton->setChecked(false);
        m_alignCenterButton->setChecked(true);
        emit alignmentChanged(FluentTextAlignment::Center);
    });

    connect(m_alignRightButton, &FluentButton::clicked, this, [this]() {
        m_alignLeftButton->setChecked(false);
        m_alignCenterButton->setChecked(false);
        m_alignJustifyButton->setChecked(false);
        m_alignRightButton->setChecked(true);
        emit alignmentChanged(FluentTextAlignment::Right);
    });

    connect(m_alignJustifyButton, &FluentButton::clicked, this, [this]() {
        m_alignLeftButton->setChecked(false);
        m_alignCenterButton->setChecked(false);
        m_alignRightButton->setChecked(false);
        m_alignJustifyButton->setChecked(true);
        emit alignmentChanged(FluentTextAlignment::Justify);
    });
}

void FluentTextFormatBar::setupListControls() {
    m_listTypeCombo = new FluentComboBox(this);
    m_listTypeCombo->addItems(
        {"None", "Bullet List", "Numbered List", "Check List"});
    m_listTypeCombo->setMaximumWidth(120);
    connect(m_listTypeCombo,
            QOverload<int>::of(&FluentComboBox::currentIndexChanged), this,
            &FluentTextFormatBar::onListTypeChanged);

    m_indentButton = new FluentButton("→", this);
    m_indentButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_indentButton->setFixedSize(32, 32);
    connect(m_indentButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::indentRequested);

    m_outdentButton = new FluentButton("←", this);
    m_outdentButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_outdentButton->setFixedSize(32, 32);
    connect(m_outdentButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::outdentRequested);
}

void FluentTextFormatBar::setupInsertControls() {
    m_linkButton = new FluentButton("🔗", this);
    m_linkButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_linkButton->setFixedSize(32, 32);
    connect(m_linkButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::insertLinkRequested);

    m_imageButton = new FluentButton("🖼", this);
    m_imageButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_imageButton->setFixedSize(32, 32);
    connect(m_imageButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::insertImageRequested);

    m_tableButton = new FluentButton("⊞", this);
    m_tableButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_tableButton->setFixedSize(32, 32);
    connect(m_tableButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::insertTableRequested);
}

QWidget* FluentTextFormatBar::createSeparator() {
    auto* separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setFixedWidth(1);
    separator->setStyleSheet("QFrame { color: #E0E0E0; }");
    return separator;
}

// FluentRichTextEditor Implementation
FluentRichTextEditor::FluentRichTextEditor(QWidget* parent)
    : Core::FluentComponent(parent) {
    setupTextEditor();
    setupFormatBar();
    setupContextMenu();

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this, [this]() {
                // Update styling when theme changes
                update();
            });
}

QString FluentRichTextEditor::toHtml() const { return m_textEdit->toHtml(); }

void FluentRichTextEditor::setHtml(const QString& html) {
    m_textEdit->setHtml(html);
}

QString FluentRichTextEditor::toPlainText() const {
    return m_textEdit->toPlainText();
}

void FluentRichTextEditor::setPlainText(const QString& text) {
    m_textEdit->setPlainText(text);
}

QTextDocument* FluentRichTextEditor::document() const {
    return m_textEdit->document();
}

void FluentRichTextEditor::setDocument(QTextDocument* document) {
    m_textEdit->setDocument(document);
}

bool FluentRichTextEditor::isReadOnly() const {
    return m_textEdit->isReadOnly();
}

void FluentRichTextEditor::setReadOnly(bool readOnly) {
    m_textEdit->setReadOnly(readOnly);
    if (m_formatBar) {
        m_formatBar->setEnabled(!readOnly);
    }
}

bool FluentRichTextEditor::showFormatBar() const { return m_showFormatBar; }

void FluentRichTextEditor::setShowFormatBar(bool show) {
    if (m_showFormatBar != show) {
        m_showFormatBar = show;
        if (m_formatBar) {
            m_formatBar->setVisible(show);
        }
    }
}

QTextCursor FluentRichTextEditor::textCursor() const {
    return m_textEdit->textCursor();
}

void FluentRichTextEditor::setTextCursor(const QTextCursor& cursor) {
    m_textEdit->setTextCursor(cursor);
}

void FluentRichTextEditor::setFontFamily(const QString& family) {
    QTextCharFormat format;
    format.setFontFamilies(QStringList() << family);
    applyCharacterFormat(format);
}

void FluentRichTextEditor::setFontSize(int size) {
    QTextCharFormat format;
    format.setFontPointSize(size);
    applyCharacterFormat(format);
}

void FluentRichTextEditor::setAlignment(FluentTextAlignment alignment) {
    QTextBlockFormat format;
    Qt::Alignment qtAlignment;

    switch (alignment) {
        case FluentTextAlignment::Left:
            qtAlignment = Qt::AlignLeft;
            break;
        case FluentTextAlignment::Center:
            qtAlignment = Qt::AlignCenter;
            break;
        case FluentTextAlignment::Right:
            qtAlignment = Qt::AlignRight;
            break;
        case FluentTextAlignment::Justify:
            qtAlignment = Qt::AlignJustify;
            break;
    }

    format.setAlignment(qtAlignment);
    applyBlockFormat(format);
}

void FluentRichTextEditor::insertLink(const QString& url, const QString& text) {
    QTextCursor cursor = m_textEdit->textCursor();

    QString linkText = text.isEmpty() ? url : text;

    QTextCharFormat linkFormat;
    linkFormat.setAnchor(true);
    linkFormat.setAnchorHref(url);
    linkFormat.setForeground(QColor(0, 0, 255));
    linkFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

    cursor.insertText(linkText, linkFormat);
}

void FluentRichTextEditor::insertImage(const QString& path,
                                       const QString& alt) {
    QTextCursor cursor = m_textEdit->textCursor();

    QTextImageFormat imageFormat;
    imageFormat.setName(path);
    if (!alt.isEmpty()) {
        imageFormat.setProperty(QTextFormat::ImageAltText, alt);
    }

    cursor.insertImage(imageFormat);
}

void FluentRichTextEditor::insertTable(int rows, int cols) {
    QTextCursor cursor = m_textEdit->textCursor();

    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setBorder(1);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(4);

    cursor.insertTable(rows, cols, tableFormat);
}

bool FluentRichTextEditor::exportToPdf(const QString& fileName) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    m_textEdit->document()->print(&printer);
    return true;
}

bool FluentRichTextEditor::exportToHtml(const QString& fileName) {
    QTextDocumentWriter writer(fileName);
    writer.setFormat("HTML");
    return writer.write(m_textEdit->document());
}

void FluentRichTextEditor::zoomIn(int delta) {
    m_zoomPercent += delta * 10;
    m_zoomPercent = qMin(m_zoomPercent, 300);
    updateZoom();
}

void FluentRichTextEditor::zoomOut(int delta) {
    m_zoomPercent -= delta * 10;
    m_zoomPercent = qMax(m_zoomPercent, 25);
    updateZoom();
}

void FluentRichTextEditor::zoomTo(int percent) {
    m_zoomPercent = qBound(25, percent, 300);
    updateZoom();
}

void FluentRichTextEditor::resetZoom() {
    m_zoomPercent = 100;
    updateZoom();
}

void FluentRichTextEditor::setupTextEditor() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_textEdit = new QTextEdit(this);
    m_textEdit->setAcceptRichText(true);
    m_textEdit->setFrameShape(QFrame::NoFrame);

    // Enable drag and drop for images
    m_textEdit->setAcceptDrops(true);

    connect(m_textEdit, &QTextEdit::textChanged, this,
            &FluentRichTextEditor::onTextChanged);
    connect(m_textEdit, &QTextEdit::selectionChanged, this,
            &FluentRichTextEditor::onSelectionChanged);
    connect(m_textEdit, &QTextEdit::cursorPositionChanged, this,
            &FluentRichTextEditor::onCursorPositionChanged);
    connect(m_textEdit, &QTextEdit::undoAvailable, this,
            &FluentRichTextEditor::undoAvailable);
    connect(m_textEdit, &QTextEdit::redoAvailable, this,
            &FluentRichTextEditor::redoAvailable);
    connect(m_textEdit, &QTextEdit::copyAvailable, this,
            &FluentRichTextEditor::copyAvailable);

    m_layout->addWidget(m_textEdit);
}

void FluentRichTextEditor::setupFormatBar() {
    if (m_showFormatBar) {
        m_formatBar = new FluentTextFormatBar(this);

        connect(m_formatBar, &FluentTextFormatBar::fontFamilyChanged, this,
                &FluentRichTextEditor::setFontFamily);
        connect(m_formatBar, &FluentTextFormatBar::fontSizeChanged, this,
                &FluentRichTextEditor::setFontSize);
        connect(m_formatBar, &FluentTextFormatBar::boldToggled, this,
                [this](bool bold) {
                    QTextCharFormat format;
                    format.setFontWeight(bold ? QFont::Bold : QFont::Normal);
                    applyCharacterFormat(format);
                });
        connect(m_formatBar, &FluentTextFormatBar::alignmentChanged, this,
                &FluentRichTextEditor::setAlignment);

        m_layout->insertWidget(0, m_formatBar);
    }
}

void FluentRichTextEditor::applyCharacterFormat(const QTextCharFormat& format) {
    QTextCursor cursor = m_textEdit->textCursor();
    if (cursor.hasSelection()) {
        cursor.mergeCharFormat(format);
    } else {
        cursor.setCharFormat(format);
    }
    m_textEdit->setTextCursor(cursor);
}

void FluentRichTextEditor::updateZoom() {
    QFont font = m_textEdit->font();
    int baseSize = 12;
    int newSize = (baseSize * m_zoomPercent) / 100;
    font.setPointSize(newSize);
    m_textEdit->setFont(font);

    emit zoomChanged(m_zoomPercent);
}

void FluentRichTextEditor::onTextChanged() { emit textChanged(); }

void FluentRichTextEditor::onSelectionChanged() {
    updateFormatBar();
    emit selectionChanged();
}

void FluentRichTextEditor::onCursorPositionChanged() {
    updateFormatBar();
    emit cursorPositionChanged();
}

void FluentRichTextEditor::updateFormatBar() {
    if (m_formatBar) {
        m_formatBar->updateFromCursor(m_textEdit->textCursor());
    }
}

bool FluentRichTextEditor::acceptRichText() const {
    return m_textEdit ? m_textEdit->acceptRichText() : true;
}

bool FluentRichTextEditor::lineWrapMode() const {
    return m_textEdit ? (m_textEdit->lineWrapMode() != QTextEdit::NoWrap)
                      : true;
}

void FluentRichTextEditor::clear() {
    if (m_textEdit) {
        m_textEdit->clear();
    }
}

void FluentRichTextEditor::selectAll() {
    if (m_textEdit) {
        m_textEdit->selectAll();
    }
}

void FluentRichTextEditor::undo() {
    if (m_textEdit) {
        m_textEdit->undo();
    }
}

void FluentRichTextEditor::redo() {
    if (m_textEdit) {
        m_textEdit->redo();
    }
}

void FluentRichTextEditor::cut() {
    if (m_textEdit) {
        m_textEdit->cut();
    }
}

void FluentRichTextEditor::copy() {
    if (m_textEdit) {
        m_textEdit->copy();
    }
}

void FluentRichTextEditor::paste() {
    if (m_textEdit) {
        m_textEdit->paste();
    }
}

void FluentRichTextEditor::onFormatBarActionTriggered() {
    // Handle format bar action triggers
    updateFormatBar();
}

void FluentRichTextEditor::setAcceptRichText(bool accept) {
    if (m_textEdit) {
        m_textEdit->setAcceptRichText(accept);
    }
}

void FluentRichTextEditor::setLineWrapMode(bool wrap) {
    if (m_textEdit) {
        m_textEdit->setLineWrapMode(wrap ? QTextEdit::WidgetWidth
                                         : QTextEdit::NoWrap);
    }
}

void FluentRichTextEditor::applyBlockFormat(const QTextBlockFormat& format) {
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.mergeBlockFormat(format);
    m_textEdit->setTextCursor(cursor);
}

void FluentRichTextEditor::setupContextMenu() {
    // Setup context menu for the text editor
    m_textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_textEdit, &QTextEdit::customContextMenuRequested, this,
            [this](const QPoint& pos) {
                QMenu* menu = m_textEdit->createStandardContextMenu();
                menu->exec(m_textEdit->mapToGlobal(pos));
                menu->deleteLater();
            });
}

// FluentTextFormatBar missing method implementations
void FluentTextFormatBar::onFontFamilyChanged() {
    // Handle font family change
    emit fontFamilyChanged(QString());
}

void FluentTextFormatBar::onFontSizeChanged() {
    // Handle font size change
    emit fontSizeChanged(12);
}

void FluentTextFormatBar::onBoldClicked() {
    // Handle bold toggle
    emit boldToggled(true);
}

void FluentTextFormatBar::onItalicClicked() {
    // Handle italic toggle
    emit italicToggled(true);
}

void FluentTextFormatBar::onUnderlineClicked() {
    // Handle underline toggle
    emit underlineToggled(true);
}

void FluentTextFormatBar::onStrikethroughClicked() {
    // Handle strikethrough toggle
    emit strikethroughToggled(true);
}

void FluentTextFormatBar::onTextColorClicked() {
    // Handle text color change
    emit textColorChanged(QColor());
}

void FluentTextFormatBar::onBackgroundColorClicked() {
    // Handle background color change
    emit backgroundColor(QColor());
}

void FluentTextFormatBar::onAlignmentChanged() {
    // Handle alignment change
    emit alignmentChanged(FluentTextAlignment::Left);
}

void FluentTextFormatBar::onListTypeChanged() {
    // Handle list type change
    emit listTypeChanged(FluentTextListType::None);
}

void FluentTextFormatBar::updateFontControls(const QTextCharFormat& format) {
    Q_UNUSED(format)
    // Update font controls based on format
}

void FluentTextFormatBar::updateFormatControls(const QTextCharFormat& format) {
    Q_UNUSED(format)
    // Update format controls based on format
}

void FluentTextFormatBar::updateAlignmentControls(
    const QTextBlockFormat& format) {
    Q_UNUSED(format)
    // Update alignment controls based on format
}

void FluentTextFormatBar::updateListControls(const QTextCursor& cursor) {
    Q_UNUSED(cursor)
    // Update list controls based on cursor
}

void FluentTextFormatBar::setEnabled(bool enabled) {
    QWidget::setEnabled(enabled);
}

// Missing event handlers for FluentRichTextEditor
void FluentRichTextEditor::wheelEvent(QWheelEvent* event) {
    // Handle wheel events for zooming
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else {
        Core::FluentComponent::wheelEvent(event);
    }
}

void FluentRichTextEditor::keyPressEvent(QKeyEvent* event) {
    // Handle key press events
    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
            case Qt::Key_Plus:
            case Qt::Key_Equal:
                zoomIn();
                event->accept();
                return;
            case Qt::Key_Minus:
                zoomOut();
                event->accept();
                return;
            case Qt::Key_0:
                resetZoom();
                event->accept();
                return;
        }
    }
    Core::FluentComponent::keyPressEvent(event);
}

void FluentRichTextEditor::paintEvent(QPaintEvent* event) {
    Core::FluentComponent::paintEvent(event);
}

void FluentRichTextEditor::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
}

void FluentRichTextEditor::contextMenuEvent(QContextMenuEvent* event) {
    // Forward context menu events to the text edit
    if (m_textEdit) {
        QContextMenuEvent* textEditEvent = new QContextMenuEvent(
            event->reason(), m_textEdit->mapFromGlobal(event->globalPos()),
            event->globalPos(), event->modifiers());
        QApplication::sendEvent(m_textEdit, textEditEvent);
        delete textEditEvent;
    }
    event->accept();
}

}  // namespace FluentQt::Components
