// src/Components/FluentRichTextEditor.cpp
#include "../../include/FluentQt/Components/FluentRichTextEditor.h"
#include "../../include/FluentQt/Components/FluentButton.h"
#include "../../include/FluentQt/Components/FluentComboBox.h"
#include "../../include/FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QContextMenuEvent>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFontComboBox>
#include <QFontDialog>
#include <QFrame>
#include <QHBoxLayout>
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
#include <QSpinBox>
#include <QString>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextEdit>
#include <QTextList>
#include <QToolTip>
#include <QUrl>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>

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

    // Apply Fluent Design styling
    applyFluentStyling();

    // Connect to theme changes for dynamic updates
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            [this]() { applyFluentStyling(); });
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
    const auto& theme = Styling::FluentTheme::instance();

    // Font family combo box with proper styling
    m_fontCombo = new QFontComboBox(this);
    m_fontCombo->setMaximumWidth(theme.spacing("combobox.width.large"));
    m_fontCombo->setFont(theme.bodyFont());
    m_fontCombo->setToolTip(
        QCoreApplication::translate("FluentTextFormatBar", "Font Family"));
    m_fontCombo->setAccessibleName(
        QCoreApplication::translate("FluentTextFormatBar", "Font Family"));
    m_fontCombo->setAccessibleDescription(QCoreApplication::translate(
        "FluentTextFormatBar", "Select font family"));
    connect(m_fontCombo,
            QOverload<const QString&>::of(&QFontComboBox::currentTextChanged),
            this, &FluentTextFormatBar::onFontFamilyChanged);

    // Font size spin box with proper styling
    m_fontSizeSpinBox = new QSpinBox(this);
    m_fontSizeSpinBox->setRange(6, 72);
    m_fontSizeSpinBox->setValue(theme.bodyFont().pointSize());
    m_fontSizeSpinBox->setMaximumWidth(theme.spacing("input.width.small"));
    m_fontSizeSpinBox->setFont(theme.bodyFont());
    m_fontSizeSpinBox->setToolTip(tr("Font Size"));
    m_fontSizeSpinBox->setAccessibleName(tr("Font Size"));
    m_fontSizeSpinBox->setAccessibleDescription(tr("Select font size"));
    connect(m_fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FluentTextFormatBar::onFontSizeChanged);
}

void FluentTextFormatBar::setupFormatControls() {
    const auto& theme = Styling::FluentTheme::instance();
    const int buttonSize = theme.spacing("button.height.medium");

    // Bold button with proper accessibility
    m_boldButton = new FluentButton("B", this);
    m_boldButton->setCheckable(true);
    m_boldButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_boldButton->setFixedSize(buttonSize, buttonSize);
    m_boldButton->setFont(QFont(theme.buttonFont().family(),
                                theme.buttonFont().pointSize(), QFont::Bold));
    m_boldButton->setToolTip(tr("Bold (Ctrl+B)"));
    m_boldButton->setAccessibleName(tr("Bold"));
    m_boldButton->setAccessibleDescription(tr("Make text bold"));
    connect(m_boldButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onBoldClicked);

    // Italic button with proper accessibility
    m_italicButton = new FluentButton("I", this);
    m_italicButton->setCheckable(true);
    m_italicButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_italicButton->setFixedSize(buttonSize, buttonSize);
    QFont italicFont = theme.buttonFont();
    italicFont.setItalic(true);
    m_italicButton->setFont(italicFont);
    m_italicButton->setToolTip(tr("Italic (Ctrl+I)"));
    m_italicButton->setAccessibleName(tr("Italic"));
    m_italicButton->setAccessibleDescription(tr("Make text italic"));
    connect(m_italicButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onItalicClicked);

    // Underline button with proper accessibility
    m_underlineButton = new FluentButton("U", this);
    m_underlineButton->setCheckable(true);
    m_underlineButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_underlineButton->setFixedSize(buttonSize, buttonSize);
    QFont underlineFont = theme.buttonFont();
    underlineFont.setUnderline(true);
    m_underlineButton->setFont(underlineFont);
    m_underlineButton->setToolTip(tr("Underline (Ctrl+U)"));
    m_underlineButton->setAccessibleName(tr("Underline"));
    m_underlineButton->setAccessibleDescription(tr("Underline text"));
    connect(m_underlineButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onUnderlineClicked);

    // Strikethrough button with proper accessibility
    m_strikethroughButton = new FluentButton("S", this);
    m_strikethroughButton->setCheckable(true);
    m_strikethroughButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_strikethroughButton->setFixedSize(buttonSize, buttonSize);
    QFont strikeFont = theme.buttonFont();
    strikeFont.setStrikeOut(true);
    m_strikethroughButton->setFont(strikeFont);
    m_strikethroughButton->setToolTip(tr("Strikethrough"));
    m_strikethroughButton->setAccessibleName(tr("Strikethrough"));
    m_strikethroughButton->setAccessibleDescription(tr("Strike through text"));
    connect(m_strikethroughButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onStrikethroughClicked);

    // Text color button with proper accessibility
    m_textColorButton = new FluentButton("A", this);
    m_textColorButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_textColorButton->setFixedSize(buttonSize, buttonSize);
    m_textColorButton->setToolTip(tr("Text Color"));
    m_textColorButton->setAccessibleName(tr("Text Color"));
    m_textColorButton->setAccessibleDescription(tr("Change text color"));
    connect(m_textColorButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onTextColorClicked);

    // Background color button with proper accessibility
    m_backgroundColorButton = new FluentButton("H", this);
    m_backgroundColorButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_backgroundColorButton->setFixedSize(buttonSize, buttonSize);
    m_backgroundColorButton->setToolTip(tr("Highlight Color"));
    m_backgroundColorButton->setAccessibleName(tr("Highlight Color"));
    m_backgroundColorButton->setAccessibleDescription(
        tr("Change text background color"));
    connect(m_backgroundColorButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::onBackgroundColorClicked);
}

void FluentTextFormatBar::setupAlignmentControls() {
    const auto& theme = Styling::FluentTheme::instance();
    const int buttonSize = theme.spacing("button.height.medium");

    // Create alignment buttons with proper Fluent Design styling
    m_alignLeftButton = new FluentButton("⇤", this);
    m_alignLeftButton->setCheckable(true);
    m_alignLeftButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_alignLeftButton->setFixedSize(buttonSize, buttonSize);
    m_alignLeftButton->setToolTip(tr("Align Left (Ctrl+L)"));
    m_alignLeftButton->setAccessibleName(tr("Align Left"));
    m_alignLeftButton->setAccessibleDescription(tr("Align text to the left"));

    m_alignCenterButton = new FluentButton("⇋", this);
    m_alignCenterButton->setCheckable(true);
    m_alignCenterButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_alignCenterButton->setFixedSize(buttonSize, buttonSize);
    m_alignCenterButton->setToolTip(tr("Align Center (Ctrl+E)"));
    m_alignCenterButton->setAccessibleName(tr("Align Center"));
    m_alignCenterButton->setAccessibleDescription(tr("Center align text"));

    m_alignRightButton = new FluentButton("⇥", this);
    m_alignRightButton->setCheckable(true);
    m_alignRightButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_alignRightButton->setFixedSize(buttonSize, buttonSize);
    m_alignRightButton->setToolTip(tr("Align Right (Ctrl+R)"));
    m_alignRightButton->setAccessibleName(tr("Align Right"));
    m_alignRightButton->setAccessibleDescription(tr("Align text to the right"));

    m_alignJustifyButton = new FluentButton("⇿", this);
    m_alignJustifyButton->setCheckable(true);
    m_alignJustifyButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_alignJustifyButton->setFixedSize(buttonSize, buttonSize);
    m_alignJustifyButton->setToolTip(tr("Justify (Ctrl+J)"));
    m_alignJustifyButton->setAccessibleName(tr("Justify"));
    m_alignJustifyButton->setAccessibleDescription(
        tr("Justify text alignment"));

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
    const auto& theme = Styling::FluentTheme::instance();
    const int buttonSize = theme.spacing("button.height.medium");

    // List type combo box with proper styling
    m_listTypeCombo = new FluentComboBox(this);
    m_listTypeCombo->addItems(QStringList{
        tr("None"), tr("Bullet List"), tr("Numbered List"), tr("Check List")});
    m_listTypeCombo->setMaximumWidth(theme.spacing("combobox.width.medium"));
    m_listTypeCombo->setToolTip(tr("List Type"));
    m_listTypeCombo->setAccessibleName(tr("List Type"));
    m_listTypeCombo->setAccessibleDescription(tr("Select list type"));
    connect(m_listTypeCombo,
            QOverload<int>::of(&FluentComboBox::currentIndexChanged), this,
            &FluentTextFormatBar::onListTypeChanged);

    // Indent button with proper accessibility
    m_indentButton = new FluentButton("→", this);
    m_indentButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_indentButton->setFixedSize(buttonSize, buttonSize);
    m_indentButton->setToolTip(tr("Increase Indent (Tab)"));
    m_indentButton->setAccessibleName(tr("Increase Indent"));
    m_indentButton->setAccessibleDescription(tr("Increase text indentation"));
    connect(m_indentButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::indentRequested);

    // Outdent button with proper accessibility
    m_outdentButton = new FluentButton("←", this);
    m_outdentButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_outdentButton->setFixedSize(buttonSize, buttonSize);
    m_outdentButton->setToolTip(tr("Decrease Indent (Shift+Tab)"));
    m_outdentButton->setAccessibleName(tr("Decrease Indent"));
    m_outdentButton->setAccessibleDescription(tr("Decrease text indentation"));
    connect(m_outdentButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::outdentRequested);
}

void FluentTextFormatBar::setupInsertControls() {
    const auto& theme = Styling::FluentTheme::instance();
    const int buttonSize = theme.spacing("button.height.medium");

    // Link button with proper accessibility
    m_linkButton = new FluentButton("🔗", this);
    m_linkButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_linkButton->setFixedSize(buttonSize, buttonSize);
    m_linkButton->setToolTip(tr("Insert Link (Ctrl+K)"));
    m_linkButton->setAccessibleName(tr("Insert Link"));
    m_linkButton->setAccessibleDescription(tr("Insert hyperlink"));
    connect(m_linkButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::insertLinkRequested);

    // Image button with proper accessibility
    m_imageButton = new FluentButton("🖼", this);
    m_imageButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_imageButton->setFixedSize(buttonSize, buttonSize);
    m_imageButton->setToolTip(tr("Insert Image"));
    m_imageButton->setAccessibleName(tr("Insert Image"));
    m_imageButton->setAccessibleDescription(tr("Insert image from file"));
    connect(m_imageButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::insertImageRequested);

    // Table button with proper accessibility
    m_tableButton = new FluentButton("⊞", this);
    m_tableButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_tableButton->setFixedSize(buttonSize, buttonSize);
    m_tableButton->setToolTip(tr("Insert Table"));
    m_tableButton->setAccessibleName(tr("Insert Table"));
    m_tableButton->setAccessibleDescription(tr("Insert table"));
    connect(m_tableButton, &FluentButton::clicked, this,
            &FluentTextFormatBar::insertTableRequested);
}

QWidget* FluentTextFormatBar::createSeparator() {
    auto* separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setFixedWidth(1);

    // Use Fluent Design tokens for separator color
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();
    separator->setStyleSheet(
        QString("QFrame { color: %1; }").arg(palette.neutralQuaternary.name()));
    return separator;
}

void FluentTextFormatBar::applyFluentStyling() {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Apply Fluent Design styling to the format bar
    setStyleSheet(QString(R"(
        FluentTextFormatBar {
            background-color: %1;
            border-bottom: 1px solid %2;
            padding: %3px;
        }
    )")
                      .arg(palette.surface.name())
                      .arg(palette.neutralQuaternary.name())
                      .arg(theme.spacing("xs")));

    // Update separator colors
    const auto separators = findChildren<QFrame*>();
    for (auto* separator : separators) {
        if (separator->frameShape() == QFrame::VLine) {
            separator->setStyleSheet(
                QString("QFrame { color: %1; }")
                    .arg(palette.neutralQuaternary.name()));
        }
    }
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

// FluentTextFormatBar complete method implementations
void FluentTextFormatBar::onFontFamilyChanged() {
    if (m_updating)
        return;

    const QString family = m_fontCombo->currentText();
    emit fontFamilyChanged(family);
}

void FluentTextFormatBar::onFontSizeChanged() {
    if (m_updating)
        return;

    const int size = m_fontSizeSpinBox->value();
    emit fontSizeChanged(size);
}

void FluentTextFormatBar::onBoldClicked() {
    if (m_updating)
        return;

    const bool bold = m_boldButton->isChecked();
    emit boldToggled(bold);
}

void FluentTextFormatBar::onItalicClicked() {
    if (m_updating)
        return;

    const bool italic = m_italicButton->isChecked();
    emit italicToggled(italic);
}

void FluentTextFormatBar::onUnderlineClicked() {
    if (m_updating)
        return;

    const bool underline = m_underlineButton->isChecked();
    emit underlineToggled(underline);
}

void FluentTextFormatBar::onStrikethroughClicked() {
    if (m_updating)
        return;

    const bool strikethrough = m_strikethroughButton->isChecked();
    emit strikethroughToggled(strikethrough);
}

void FluentTextFormatBar::onTextColorClicked() {
    if (m_updating)
        return;

    QColorDialog dialog(this);
    dialog.setWindowTitle(tr("Select Text Color"));
    dialog.setCurrentColor(Qt::black);

    if (dialog.exec() == QDialog::Accepted) {
        const QColor color = dialog.currentColor();
        emit textColorChanged(color);

        // Update button visual indicator
        updateColorButtonIndicator(m_textColorButton, color);
    }
}

void FluentTextFormatBar::onBackgroundColorClicked() {
    if (m_updating)
        return;

    QColorDialog dialog(this);
    dialog.setWindowTitle(tr("Select Background Color"));
    dialog.setCurrentColor(Qt::yellow);

    if (dialog.exec() == QDialog::Accepted) {
        const QColor color = dialog.currentColor();
        emit backgroundColor(color);

        // Update button visual indicator
        updateColorButtonIndicator(m_backgroundColorButton, color);
    }
}

void FluentTextFormatBar::onAlignmentChanged() {
    // This method is called by individual alignment button handlers
    // The actual alignment is determined by which button was clicked
}

void FluentTextFormatBar::onListTypeChanged() {
    if (m_updating)
        return;

    const int index = m_listTypeCombo->currentIndex();
    FluentTextListType listType = static_cast<FluentTextListType>(index);
    emit listTypeChanged(listType);
}

void FluentTextFormatBar::updateFontControls(const QTextCharFormat& format) {
    if (!format.hasProperty(QTextFormat::FontFamilies) &&
        !format.hasProperty(QTextFormat::FontPointSize)) {
        return;
    }

    // Update font family
    if (format.hasProperty(QTextFormat::FontFamilies)) {
        const QStringList fontFamilies = format.fontFamilies().toStringList();
        if (!fontFamilies.isEmpty()) {
            const QString fontFamily = fontFamilies.first();
            if (m_fontCombo->currentText() != fontFamily) {
                m_fontCombo->setCurrentText(fontFamily);
            }
        }
    }

    // Update font size
    if (format.hasProperty(QTextFormat::FontPointSize)) {
        const int fontSize = static_cast<int>(format.fontPointSize());
        if (m_fontSizeSpinBox->value() != fontSize) {
            m_fontSizeSpinBox->setValue(fontSize);
        }
    }
}

void FluentTextFormatBar::updateFormatControls(const QTextCharFormat& format) {
    // Update bold button
    const bool isBold = format.fontWeight() == QFont::Bold;
    if (m_boldButton->isChecked() != isBold) {
        m_boldButton->setChecked(isBold);
    }

    // Update italic button
    const bool isItalic = format.fontItalic();
    if (m_italicButton->isChecked() != isItalic) {
        m_italicButton->setChecked(isItalic);
    }

    // Update underline button
    const bool isUnderline = format.fontUnderline();
    if (m_underlineButton->isChecked() != isUnderline) {
        m_underlineButton->setChecked(isUnderline);
    }

    // Update strikethrough button
    const bool isStrikethrough = format.fontStrikeOut();
    if (m_strikethroughButton->isChecked() != isStrikethrough) {
        m_strikethroughButton->setChecked(isStrikethrough);
    }

    // Update text color indicator
    if (format.hasProperty(QTextFormat::ForegroundBrush)) {
        const QColor textColor = format.foreground().color();
        updateColorButtonIndicator(m_textColorButton, textColor);
    }

    // Update background color indicator
    if (format.hasProperty(QTextFormat::BackgroundBrush)) {
        const QColor bgColor = format.background().color();
        updateColorButtonIndicator(m_backgroundColorButton, bgColor);
    }
}

void FluentTextFormatBar::updateAlignmentControls(
    const QTextBlockFormat& format) {
    const Qt::Alignment alignment = format.alignment();

    // Reset all alignment buttons
    m_alignLeftButton->setChecked(false);
    m_alignCenterButton->setChecked(false);
    m_alignRightButton->setChecked(false);
    m_alignJustifyButton->setChecked(false);

    // Set the appropriate button based on alignment
    if (alignment & Qt::AlignLeft) {
        m_alignLeftButton->setChecked(true);
    } else if (alignment & Qt::AlignCenter) {
        m_alignCenterButton->setChecked(true);
    } else if (alignment & Qt::AlignRight) {
        m_alignRightButton->setChecked(true);
    } else if (alignment & Qt::AlignJustify) {
        m_alignJustifyButton->setChecked(true);
    }
}

void FluentTextFormatBar::updateListControls(const QTextCursor& cursor) {
    QTextList* list = cursor.currentList();

    if (!list) {
        m_listTypeCombo->setCurrentIndex(0);  // None
        return;
    }

    const QTextListFormat::Style style = list->format().style();
    int comboIndex = 0;

    switch (style) {
        case QTextListFormat::ListDisc:
        case QTextListFormat::ListCircle:
        case QTextListFormat::ListSquare:
            comboIndex = 1;  // Bullet List
            break;
        case QTextListFormat::ListDecimal:
        case QTextListFormat::ListLowerAlpha:
        case QTextListFormat::ListUpperAlpha:
        case QTextListFormat::ListLowerRoman:
        case QTextListFormat::ListUpperRoman:
            comboIndex = 2;  // Numbered List
            break;
        default:
            comboIndex = 0;  // None
            break;
    }

    if (m_listTypeCombo->currentIndex() != comboIndex) {
        m_listTypeCombo->setCurrentIndex(comboIndex);
    }
}

void FluentTextFormatBar::updateColorButtonIndicator(FluentButton* button,
                                                     const QColor& color) {
    if (!button)
        return;

    // Create a small color indicator on the button
    const int indicatorSize = 16;
    QPixmap pixmap(indicatorSize, indicatorSize);
    pixmap.fill(color);

    // Add a border to the color indicator
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(0, 0, indicatorSize - 1, indicatorSize - 1);

    // Set the pixmap as button icon (if FluentButton supports icons)
    // This would need to be implemented based on FluentButton's API
    // For now, we'll store the color as a property
    button->setProperty("indicatorColor", color);
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
