// include/FluentQt/Components/FluentColorPicker.h
#pragma once

#include <QColor>
#include <QColorDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentColorPickerMode {
    Palette,  // Predefined color palette
    Wheel,    // Color wheel picker
    Sliders,  // RGB/HSV sliders
    Compact,  // Compact palette + custom button
    Full      // All modes available
};

enum class FluentColorPickerFormat {
    RGB,   // Red, Green, Blue
    HSV,   // Hue, Saturation, Value
    HSL,   // Hue, Saturation, Lightness
    CMYK,  // Cyan, Magenta, Yellow, Key
    Hex    // Hexadecimal
};

enum class FluentColorPickerSize {
    Small,   // Compact size
    Medium,  // Standard size
    Large    // Extended size
};

class FluentColorWheel;
class FluentColorSlider;
class FluentColorPalette;

class FluentColorPicker : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(
        FluentColorPickerMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(FluentColorPickerFormat format READ format WRITE setFormat NOTIFY
                   formatChanged)
    Q_PROPERTY(FluentColorPickerSize pickerSize READ pickerSize WRITE
                   setPickerSize NOTIFY pickerSizeChanged)
    Q_PROPERTY(bool showAlpha READ showAlpha WRITE setShowAlpha NOTIFY
                   showAlphaChanged)
    Q_PROPERTY(bool showPreview READ showPreview WRITE setShowPreview NOTIFY
                   showPreviewChanged)
    Q_PROPERTY(bool showHistory READ showHistory WRITE setShowHistory NOTIFY
                   showHistoryChanged)
    Q_PROPERTY(bool showEyeDropper READ showEyeDropper WRITE setShowEyeDropper
                   NOTIFY showEyeDropperChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(QStringList customPalette READ customPalette WRITE
                   setCustomPalette NOTIFY customPaletteChanged)

public:
    explicit FluentColorPicker(QWidget* parent = nullptr);
    explicit FluentColorPicker(const QColor& initialColor,
                               QWidget* parent = nullptr);
    explicit FluentColorPicker(FluentColorPickerMode mode,
                               QWidget* parent = nullptr);
    ~FluentColorPicker() override;

    // Color properties
    QColor color() const;
    void setColor(const QColor& color);

    int alpha() const;
    void setAlpha(int alpha);

    // Mode and format
    FluentColorPickerMode mode() const;
    void setMode(FluentColorPickerMode mode);

    FluentColorPickerFormat format() const;
    void setFormat(FluentColorPickerFormat format);

    FluentColorPickerSize pickerSize() const;
    void setPickerSize(FluentColorPickerSize size);

    // Feature toggles
    bool showAlpha() const;
    void setShowAlpha(bool show);

    bool showPreview() const;
    void setShowPreview(bool show);

    bool showHistory() const;
    void setShowHistory(bool show);

    bool showEyeDropper() const;
    void setShowEyeDropper(bool show);

    bool isAnimated() const;
    void setAnimated(bool animated);

    // Palette management
    QStringList customPalette() const;
    void setCustomPalette(const QStringList& colors);
    void addToCustomPalette(const QColor& color);
    void removeFromCustomPalette(const QColor& color);
    void clearCustomPalette();

    // History management
    QList<QColor> colorHistory() const;
    void addToHistory(const QColor& color);
    void clearHistory();
    int maxHistorySize() const;
    void setMaxHistorySize(int size);

    // Utility methods
    QString colorString() const;
    QString colorString(FluentColorPickerFormat format) const;
    bool isValidColorString(const QString& colorStr) const;
    QColor colorFromString(const QString& colorStr) const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Access to sub-components
    FluentColorWheel* colorWheel() const { return m_colorWheel; }
    FluentColorPalette* colorPalette() const { return m_colorPalette; }

    // Static convenience methods
    static QColor getColor(const QColor& initial = Qt::white,
                           QWidget* parent = nullptr,
                           const QString& title = QString(),
                           QColorDialog::ColorDialogOptions options = {});
    static FluentColorPicker* createCompactPicker(QWidget* parent = nullptr);
    static FluentColorPicker* createPalettePicker(QWidget* parent = nullptr);

public slots:
    void showColorDialog();
    void pickColorFromScreen();
    void resetToDefault();
    void copyColorToClipboard();
    void pasteColorFromClipboard();

signals:
    void colorChanged(const QColor& color);
    void modeChanged(FluentColorPickerMode mode);
    void formatChanged(FluentColorPickerFormat format);
    void pickerSizeChanged(FluentColorPickerSize size);
    void showAlphaChanged(bool show);
    void showPreviewChanged(bool show);
    void showHistoryChanged(bool show);
    void showEyeDropperChanged(bool show);
    void animatedChanged(bool animated);
    void customPaletteChanged(const QStringList& colors);

    void colorSelected(const QColor& color);
    void colorHovered(const QColor& color);
    void eyeDropperActivated();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onColorWheelChanged(const QColor& color);
    void onPaletteColorSelected(const QColor& color);
    void onSliderValueChanged();
    void onFormatInputChanged();
    void onPreviewClicked();
    void onEyeDropperClicked();
    void onHistoryColorSelected(const QColor& color);
    void updateColorInputs();
    void updateColorPreview();

private:
    void setupLayout();
    void setupColorWheel();
    void setupColorPalette();
    void setupColorSliders();
    void setupColorInputs();
    void setupPreview();
    void setupHistory();
    void setupEyeDropper();

    void updateLayout();
    void updateSizeMetrics();
    void updateVisibility();

    // Color conversion methods
    void updateFromColor();
    void updateColorFromInputs();
    QColor getColorFromSliders() const;
    void setColorToSliders(const QColor& color);

    // Input validation
    void validateColorInputs();
    void formatColorInputs();

    // Animation methods
    void animateColorChange(const QColor& newColor);
    void animatePreviewUpdate();

private:
    // Core properties
    QColor m_color{Qt::white};
    FluentColorPickerMode m_mode{FluentColorPickerMode::Full};
    FluentColorPickerFormat m_format{FluentColorPickerFormat::RGB};
    FluentColorPickerSize m_pickerSize{FluentColorPickerSize::Medium};

    // Feature flags
    bool m_showAlpha{true};
    bool m_showPreview{true};
    bool m_showHistory{true};
    bool m_showEyeDropper{true};
    bool m_animated{true};

    // Palette and history
    QStringList m_customPalette;
    QList<QColor> m_colorHistory;
    int m_maxHistorySize{20};

    // Layout components
    QVBoxLayout* m_mainLayout{nullptr};
    QHBoxLayout* m_topLayout{nullptr};
    QVBoxLayout* m_controlsLayout{nullptr};
    QGridLayout* m_slidersLayout{nullptr};
    QHBoxLayout* m_inputsLayout{nullptr};
    QHBoxLayout* m_previewLayout{nullptr};

    // Sub-components
    FluentColorWheel* m_colorWheel{nullptr};
    FluentColorPalette* m_colorPalette{nullptr};

    // Sliders (RGB/HSV)
    FluentColorSlider* m_redSlider{nullptr};
    FluentColorSlider* m_greenSlider{nullptr};
    FluentColorSlider* m_blueSlider{nullptr};
    FluentColorSlider* m_alphaSlider{nullptr};
    FluentColorSlider* m_hueSlider{nullptr};
    FluentColorSlider* m_saturationSlider{nullptr};
    FluentColorSlider* m_valueSlider{nullptr};

    // Input fields
    QSpinBox* m_redSpinBox{nullptr};
    QSpinBox* m_greenSpinBox{nullptr};
    QSpinBox* m_blueSpinBox{nullptr};
    QSpinBox* m_alphaSpinBox{nullptr};
    QLineEdit* m_hexLineEdit{nullptr};

    // Labels
    QLabel* m_redLabel{nullptr};
    QLabel* m_greenLabel{nullptr};
    QLabel* m_blueLabel{nullptr};
    QLabel* m_alphaLabel{nullptr};
    QLabel* m_hexLabel{nullptr};

    // Preview and controls
    QWidget* m_previewWidget{nullptr};
    QWidget* m_currentColorWidget{nullptr};
    QWidget* m_newColorWidget{nullptr};
    FluentButton* m_eyeDropperButton{nullptr};
    FluentButton* m_moreButton{nullptr};

    // History
    QWidget* m_historyWidget{nullptr};
    QGridLayout* m_historyLayout{nullptr};
    QList<QWidget*> m_historyItems;

    // State
    bool m_updatingFromColor{false};
    bool m_updatingFromInputs{false};
    QColor m_previousColor;

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_colorAnimation{nullptr};
    QPropertyAnimation* m_previewAnimation{nullptr};

    // Size metrics
    int m_wheelSize{200};
    int m_sliderHeight{20};
    int m_previewSize{40};
    int m_historyItemSize{24};
    int m_spacing{8};
};

// Helper components
class FluentColorWheel : public QWidget {
    Q_OBJECT
public:
    explicit FluentColorWheel(QWidget* parent = nullptr);
    QColor color() const { return m_color; }
    void setColor(const QColor& color);

signals:
    void colorChanged(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QColor m_color{Qt::white};
    QPoint m_center;
    int m_radius{100};
};

class FluentColorSlider : public QSlider {
    Q_OBJECT
public:
    explicit FluentColorSlider(Qt::Orientation orientation,
                               QWidget* parent = nullptr);
    void setColorComponent(FluentColorPickerFormat format, int component);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    FluentColorPickerFormat m_format{FluentColorPickerFormat::RGB};
    int m_component{0};
    QColor m_baseColor{Qt::white};
};

class FluentColorPalette : public QWidget {
    Q_OBJECT
public:
    explicit FluentColorPalette(QWidget* parent = nullptr);
    void setColors(const QStringList& colors);
    void addColor(const QColor& color);

signals:
    void colorSelected(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QList<QColor> m_colors;
    int m_columns{8};
    int m_itemSize{24};
};

}  // namespace FluentQt::Components
