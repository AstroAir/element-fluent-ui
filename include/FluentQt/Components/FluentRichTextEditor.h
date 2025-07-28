// include/FluentQt/Components/FluentRichTextEditor.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentComboBox.h"
#include <QTextEdit>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QToolBar>
#include <QFontComboBox>
#include <QSpinBox>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace FluentQt::Components {

enum class FluentTextAlignment {
    Left,
    Center,
    Right,
    Justify
};

enum class FluentTextListType {
    None,
    Bullet,
    Numbered,
    Checklist
};

class FluentTextFormatBar : public QWidget {
    Q_OBJECT

public:
    explicit FluentTextFormatBar(QWidget* parent = nullptr);
    
    void updateFromCursor(const QTextCursor& cursor);
    void setEnabled(bool enabled);

signals:
    void fontFamilyChanged(const QString& family);
    void fontSizeChanged(int size);
    void boldToggled(bool bold);
    void italicToggled(bool italic);
    void underlineToggled(bool underline);
    void strikethroughToggled(bool strikethrough);
    void textColorChanged(const QColor& color);
    void backgroundColor(const QColor& color);
    void alignmentChanged(FluentTextAlignment alignment);
    void listTypeChanged(FluentTextListType listType);
    void indentRequested();
    void outdentRequested();
    void insertLinkRequested();
    void insertImageRequested();
    void insertTableRequested();

private slots:
    void onFontFamilyChanged();
    void onFontSizeChanged();
    void onBoldClicked();
    void onItalicClicked();
    void onUnderlineClicked();
    void onStrikethroughClicked();
    void onTextColorClicked();
    void onBackgroundColorClicked();
    void onAlignmentChanged();
    void onListTypeChanged();

private:
    void setupFontControls();
    void setupFormatControls();
    void setupAlignmentControls();
    void setupListControls();
    void setupInsertControls();
    void updateFontControls(const QTextCharFormat& format);
    void updateFormatControls(const QTextCharFormat& format);
    void updateAlignmentControls(const QTextBlockFormat& format);
    void updateListControls(const QTextCursor& cursor);
    QWidget* createSeparator();

private:
    QHBoxLayout* m_layout;
    
    // Font controls
    QFontComboBox* m_fontCombo;
    QSpinBox* m_fontSizeSpinBox;
    
    // Format controls
    FluentButton* m_boldButton;
    FluentButton* m_italicButton;
    FluentButton* m_underlineButton;
    FluentButton* m_strikethroughButton;
    FluentButton* m_textColorButton;
    FluentButton* m_backgroundColorButton;
    
    // Alignment controls
    FluentButton* m_alignLeftButton;
    FluentButton* m_alignCenterButton;
    FluentButton* m_alignRightButton;
    FluentButton* m_alignJustifyButton;
    
    // List controls
    FluentComboBox* m_listTypeCombo;
    FluentButton* m_indentButton;
    FluentButton* m_outdentButton;
    
    // Insert controls
    FluentButton* m_linkButton;
    FluentButton* m_imageButton;
    FluentButton* m_tableButton;
    
    bool m_updating{false};
};

class FluentRichTextEditor : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString html READ toHtml WRITE setHtml NOTIFY textChanged)
    Q_PROPERTY(QString plainText READ toPlainText WRITE setPlainText NOTIFY textChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool showFormatBar READ showFormatBar WRITE setShowFormatBar)
    Q_PROPERTY(bool acceptRichText READ acceptRichText WRITE setAcceptRichText)
    Q_PROPERTY(bool lineWrapMode READ lineWrapMode WRITE setLineWrapMode)

public:
    explicit FluentRichTextEditor(QWidget* parent = nullptr);
    
    // Content
    QString toHtml() const;
    void setHtml(const QString& html);
    
    QString toPlainText() const;
    void setPlainText(const QString& text);
    
    QTextDocument* document() const;
    void setDocument(QTextDocument* document);
    
    // Properties
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);
    
    bool showFormatBar() const;
    void setShowFormatBar(bool show);
    
    bool acceptRichText() const;
    void setAcceptRichText(bool accept);
    
    bool lineWrapMode() const;
    void setLineWrapMode(bool wrap);
    
    // Cursor and selection
    QTextCursor textCursor() const;
    void setTextCursor(const QTextCursor& cursor);
    
    QString selectedText() const;
    bool hasSelection() const;
    
    // Formatting
    void setFontFamily(const QString& family);
    void setFontSize(int size);
    void setFontWeight(int weight);
    void setFontItalic(bool italic);
    void setFontUnderline(bool underline);
    void setTextColor(const QColor& color);
    void setTextBackgroundColor(const QColor& color);
    
    // Paragraph formatting
    void setAlignment(FluentTextAlignment alignment);
    void setListType(FluentTextListType listType);
    void indent();
    void outdent();
    
    // Content insertion
    void insertLink(const QString& url, const QString& text = QString());
    void insertImage(const QString& path, const QString& alt = QString());
    void insertTable(int rows, int cols);
    void insertHorizontalRule();
    
    // Search and replace
    bool find(const QString& text, QTextDocument::FindFlags flags = QTextDocument::FindFlags());
    bool replace(const QString& find, const QString& replace, QTextDocument::FindFlags flags = QTextDocument::FindFlags());
    int replaceAll(const QString& find, const QString& replace, QTextDocument::FindFlags flags = QTextDocument::FindFlags());
    
    // Undo/Redo
    bool canUndo() const;
    bool canRedo() const;
    
    // Export
    bool exportToPdf(const QString& fileName);
    bool exportToHtml(const QString& fileName);
    
    // Zoom
    void zoomIn(int delta = 1);
    void zoomOut(int delta = 1);
    void zoomTo(int percent);
    int zoomPercent() const;

public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void clear();
    void resetZoom();

signals:
    void textChanged();
    void selectionChanged();
    void cursorPositionChanged();
    void undoAvailable(bool available);
    void redoAvailable(bool available);
    void copyAvailable(bool available);
    void zoomChanged(int percent);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void onTextChanged();
    void onSelectionChanged();
    void onCursorPositionChanged();
    void onFormatBarActionTriggered();
    void updateFormatBar();

private:
    void setupTextEditor();
    void setupFormatBar();
    void setupContextMenu();
    void applyCharacterFormat(const QTextCharFormat& format);
    void applyBlockFormat(const QTextBlockFormat& format);
    void insertImageDialog();
    void insertLinkDialog();
    void insertTableDialog();
    void showFindReplaceDialog();
    
    QTextCharFormat currentCharFormat() const;
    QTextBlockFormat currentBlockFormat() const;
    FluentTextAlignment currentAlignment() const;
    FluentTextListType currentListType() const;
    void updateZoom();

private:
    QVBoxLayout* m_layout;
    FluentTextFormatBar* m_formatBar;
    QTextEdit* m_textEdit;
    
    bool m_showFormatBar{true};
    int m_zoomPercent{100};
    
    // Dialogs
    QDialog* m_findDialog{nullptr};
    QDialog* m_linkDialog{nullptr};
    QDialog* m_imageDialog{nullptr};
    QDialog* m_tableDialog{nullptr};
};

} // namespace FluentQt::Components