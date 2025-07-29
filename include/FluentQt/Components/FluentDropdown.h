// include/FluentQt/Components/FluentDropdown.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <QVariant>
#include <memory>

namespace FluentQt::Components {

struct FluentDropdownItem {
    QString text;
    QIcon icon;
    QVariant data;
    bool enabled{true};
    bool separator{false};
    
    FluentDropdownItem() = default;
    FluentDropdownItem(const QString& text, const QVariant& data = QVariant())
        : text(text), data(data) {}
    FluentDropdownItem(const QIcon& icon, const QString& text, const QVariant& data = QVariant())
        : text(text), icon(icon), data(data) {}
};

class FluentDropdown : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString currentText READ currentText WRITE setCurrentText NOTIFY currentTextChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QVariant currentData READ currentData NOTIFY currentDataChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)
    Q_PROPERTY(bool dropdownVisible READ isDropdownVisible WRITE setDropdownVisible NOTIFY dropdownVisibilityChanged)
    Q_PROPERTY(int maxVisibleItems READ maxVisibleItems WRITE setMaxVisibleItems NOTIFY maxVisibleItemsChanged)

public:
    explicit FluentDropdown(QWidget* parent = nullptr);
    ~FluentDropdown() override;

    // Content properties
    QString currentText() const;
    void setCurrentText(const QString& text);

    int currentIndex() const;
    void setCurrentIndex(int index);

    QVariant currentData() const;

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    // State properties
    bool isEditable() const;
    void setEditable(bool editable);

    bool isDropdownVisible() const;
    void setDropdownVisible(bool visible);

    int maxVisibleItems() const;
    void setMaxVisibleItems(int count);

    // Item management
    void addItem(const QString& text, const QVariant& data = QVariant());
    void addItem(const QIcon& icon, const QString& text, const QVariant& data = QVariant());
    void addItem(const FluentDropdownItem& item);
    void addSeparator();
    
    void insertItem(int index, const QString& text, const QVariant& data = QVariant());
    void insertItem(int index, const QIcon& icon, const QString& text, const QVariant& data = QVariant());
    void insertItem(int index, const FluentDropdownItem& item);
    void insertSeparator(int index);
    
    void removeItem(int index);
    void clear();
    
    int count() const;
    FluentDropdownItem itemAt(int index) const;
    void setItemAt(int index, const FluentDropdownItem& item);
    
    int findText(const QString& text, Qt::MatchFlags flags = Qt::MatchExactly) const;
    int findData(const QVariant& data) const;

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void showDropdown();
    void hideDropdown();
    void toggleDropdown();

signals:
    void currentTextChanged(const QString& text);
    void currentIndexChanged(int index);
    void currentDataChanged(const QVariant& data);
    void placeholderTextChanged(const QString& text);
    void editableChanged(bool editable);
    void dropdownVisibilityChanged(bool visible);
    void maxVisibleItemsChanged(int count);
    void itemSelected(int index);
    void textEdited(const QString& text);

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from, Core::FluentState to) override;

private slots:
    void onListItemClicked(int index);
    void onTextEdited(const QString& text);
    void onThemeChanged();
    void onDropdownAnimationFinished();

private:
    // Setup methods
    void setupUI();
    void setupDropdown();
    void setupAnimations();
    void updateColors();
    void updateFonts();

    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintChevron(QPainter& painter, const QRect& rect);
    void paintFocusRing(QPainter& painter, const QRect& rect);

    // Animation methods
    void startShowDropdownAnimation();
    void startHideDropdownAnimation();

    // Utility methods
    void updateDisplayText();
    void updateDropdownPosition();
    void updateDropdownSize();
    QRect chevronRect() const;
    QRect textRect() const;
    int calculateDropdownHeight() const;

private:
    // Content
    QList<FluentDropdownItem> m_items;
    int m_currentIndex{-1};
    QString m_placeholderText{"Select item"};
    
    // State
    bool m_editable{false};
    bool m_dropdownVisible{false};
    bool m_pressed{false};
    int m_maxVisibleItems{10};
    
    // UI components
    QHBoxLayout* m_mainLayout{nullptr};
    QLabel* m_displayLabel{nullptr};
    QWidget* m_dropdownContainer{nullptr};
    QListWidget* m_listWidget{nullptr};
    
    // Animation
    std::unique_ptr<QPropertyAnimation> m_dropdownAnimation;
    QGraphicsOpacityEffect* m_dropdownOpacityEffect{nullptr};
    
    // Cached values
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

} // namespace FluentQt::Components
