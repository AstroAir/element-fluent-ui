// src/Components/FluentColorPicker.cpp
#include "FluentQt/Components/FluentColorPicker.h"
#include <QAccessible>
#include <QApplication>
#include <QColorDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <cmath>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentColorPicker::FluentColorPicker(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)),
      m_mainLayout(new QVBoxLayout(this)),
      m_colorPreview(new QWidget(this)),
      m_paletteWidget(new QWidget(this)),
      m_customColorWidget(new QWidget(this)),
      m_hexEdit(new QLineEdit(this)),
      m_rgbWidget(new QWidget(this)),
      m_hsvWidget(new QWidget(this)),
      m_alphaSlider(new QSlider(Qt::Horizontal, this)),
      m_moreColorsButton(new QPushButton(tr("More Colors..."), this)) {
    setupUI();
    setupAnimations();
    setupConnections();
    updateColors();
    updatePreview();
    updateAccessibility();

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentColorPicker::updateColors);
}

FluentColorPicker::FluentColorPicker(const QColor& color, QWidget* parent)
    : FluentColorPicker(parent) {
    setCurrentColor(color);
}

FluentColorPicker::FluentColorPicker(FluentColorPickerMode mode,
                                     QWidget* parent)
    : FluentColorPicker(parent) {
    setMode(mode);
}

FluentColorPicker::~FluentColorPicker() = default;

QColor FluentColorPicker::currentColor() const { return m_currentColor; }

void FluentColorPicker::setCurrentColor(const QColor& color) {
    if (m_currentColor == color)
        return;

    m_currentColor = color;
    updatePreview();
    updateHexEdit();
    updateRgbControls();
    updateHsvControls();
    updateAlphaSlider();

    if (m_animated) {
        animateColorChange();
    }

    emit colorChanged(m_currentColor);
    updateAccessibility();
}

FluentColorPickerMode FluentColorPicker::mode() const { return m_mode; }

void FluentColorPicker::setMode(FluentColorPickerMode mode) {
    if (m_mode == mode)
        return;

    m_mode = mode;
    updateModeVisibility();
    emit modeChanged(m_mode);
}

QStringList FluentColorPicker::customColors() const {
    QStringList colors;
    for (const QColor& color : m_customColors) {
        colors.append(color.name());
    }
    return colors;
}

void FluentColorPicker::setCustomColors(const QStringList& colors) {
    m_customColors.clear();
    for (const QString& colorName : colors) {
        QColor color(colorName);
        if (color.isValid()) {
            m_customColors.append(color);
        }
    }
    updateCustomColorPalette();
    emit customColorsChanged(customColors());
}

bool FluentColorPicker::showAlpha() const { return m_showAlpha; }

void FluentColorPicker::setShowAlpha(bool show) {
    if (m_showAlpha == show)
        return;

    m_showAlpha = show;
    m_alphaSlider->setVisible(m_showAlpha);
    updateAlphaSlider();
    emit showAlphaChanged(m_showAlpha);
}

bool FluentColorPicker::showHex() const { return m_showHex; }

void FluentColorPicker::setShowHex(bool show) {
    if (m_showHex == show)
        return;

    m_showHex = show;
    m_hexEdit->setVisible(m_showHex);
    emit showHexChanged(m_showHex);
}

bool FluentColorPicker::showRgb() const { return m_showRgb; }

void FluentColorPicker::setShowRgb(bool show) {
    if (m_showRgb == show)
        return;

    m_showRgb = show;
    m_rgbWidget->setVisible(m_showRgb);
    emit showRgbChanged(m_showRgb);
}

bool FluentColorPicker::showHsv() const { return m_showHsv; }

void FluentColorPicker::setShowHsv(bool show) {
    if (m_showHsv == show)
        return;

    m_showHsv = show;
    m_hsvWidget->setVisible(m_showHsv);
    emit showHsvChanged(m_showHsv);
}

bool FluentColorPicker::isAnimated() const { return m_animated; }

void FluentColorPicker::setAnimated(bool animated) {
    if (m_animated == animated)
        return;

    m_animated = animated;
    emit animatedChanged(m_animated);
}

QSize FluentColorPicker::paletteSize() const { return m_paletteSize; }

void FluentColorPicker::setPaletteSize(const QSize& size) {
    if (m_paletteSize == size)
        return;

    m_paletteSize = size;
    updateStandardColorPalette();
    emit paletteSizeChanged(m_paletteSize);
}

QSize FluentColorPicker::sizeHint() const { return QSize(300, 400); }

QSize FluentColorPicker::minimumSizeHint() const { return QSize(200, 250); }

FluentColorPicker* FluentColorPicker::createSimplePicker(QWidget* parent) {
    auto* picker = new FluentColorPicker(FluentColorPickerMode::Simple, parent);
    return picker;
}

FluentColorPicker* FluentColorPicker::createAdvancedPicker(QWidget* parent) {
    auto* picker =
        new FluentColorPicker(FluentColorPickerMode::Advanced, parent);
    return picker;
}

FluentColorPicker* FluentColorPicker::createPaletteOnly(QWidget* parent) {
    auto* picker =
        new FluentColorPicker(FluentColorPickerMode::PaletteOnly, parent);
    return picker;
}

void FluentColorPicker::addCustomColor(const QColor& color) {
    if (!color.isValid() || m_customColors.contains(color))
        return;

    m_customColors.append(color);
    updateCustomColorPalette();
    emit customColorsChanged(customColors());
}

void FluentColorPicker::removeCustomColor(const QColor& color) {
    if (m_customColors.removeOne(color)) {
        updateCustomColorPalette();
        emit customColorsChanged(customColors());
    }
}

void FluentColorPicker::clearCustomColors() {
    if (!m_customColors.isEmpty()) {
        m_customColors.clear();
        updateCustomColorPalette();
        emit customColorsChanged(customColors());
    }
}

void FluentColorPicker::showColorDialog() {
    QColorDialog dialog(m_currentColor, this);
    dialog.setOption(QColorDialog::ShowAlphaChannel, m_showAlpha);

    if (dialog.exec() == QDialog::Accepted) {
        setCurrentColor(dialog.currentColor());
    }
}

void FluentColorPicker::setupUI() {
    setLayout(m_mainLayout);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    m_mainLayout->setSpacing(8);

    // Color preview
    setupColorPreview();

    // Standard color palette
    setupStandardColorPalette();

    // Custom color palette
    setupCustomColorPalette();

    // Hex input
    setupHexInput();

    // RGB controls
    setupRgbControls();

    // HSV controls
    setupHsvControls();

    // Alpha slider
    setupAlphaSlider();

    // More colors button
    m_mainLayout->addWidget(m_moreColorsButton);

    updateModeVisibility();
}

void FluentColorPicker::setupColorPreview() {
    m_colorPreview->setFixedHeight(40);
    m_colorPreview->setStyleSheet(
        "border: 1px solid #ccc; border-radius: 4px;");
    m_mainLayout->addWidget(m_colorPreview);
}

void FluentColorPicker::setupStandardColorPalette() {
    auto* layout = new QGridLayout(m_paletteWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    // Create standard color palette
    const QList<QColor> standardColors = {
        QColor(255, 0, 0),     QColor(255, 128, 0),   QColor(255, 255, 0),
        QColor(128, 255, 0),   QColor(0, 255, 0),     QColor(0, 255, 128),
        QColor(0, 255, 255),   QColor(0, 128, 255),   QColor(0, 0, 255),
        QColor(128, 0, 255),   QColor(255, 0, 255),   QColor(255, 0, 128),
        QColor(128, 128, 128), QColor(192, 192, 192), QColor(255, 255, 255),
        QColor(0, 0, 0)};

    const int columns = 8;
    for (int i = 0; i < standardColors.size(); ++i) {
        auto* colorButton = createColorButton(standardColors[i]);
        layout->addWidget(colorButton, i / columns, i % columns);
        m_standardColorButtons.append(colorButton);
    }

    m_mainLayout->addWidget(m_paletteWidget);
}

void FluentColorPicker::setupCustomColorPalette() {
    m_customColorLayout = new QGridLayout(m_customColorWidget);
    m_customColorLayout->setContentsMargins(0, 0, 0, 0);
    m_customColorLayout->setSpacing(2);

    m_mainLayout->addWidget(m_customColorWidget);
    updateCustomColorPalette();
}

void FluentColorPicker::setupHexInput() {
    auto* layout = new QHBoxLayout();
    layout->addWidget(new QLabel(tr("Hex:"), this));

    m_hexEdit->setMaxLength(9);  // #RRGGBBAA
    m_hexEdit->setPlaceholderText("#RRGGBB");

    // Hex validator
    auto* validator = new QRegularExpressionValidator(
        QRegularExpression("^#?[0-9A-Fa-f]{0,8}$"), this);
    m_hexEdit->setValidator(validator);

    layout->addWidget(m_hexEdit);

    auto* hexWidget = new QWidget(this);
    hexWidget->setLayout(layout);
    m_mainLayout->addWidget(hexWidget);
}

void FluentColorPicker::setupRgbControls() {
    auto* layout = new QGridLayout(m_rgbWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    // Red
    layout->addWidget(new QLabel(tr("R:"), this), 0, 0);
    m_redSpinBox = new QSpinBox(this);
    m_redSpinBox->setRange(0, 255);
    layout->addWidget(m_redSpinBox, 0, 1);

    // Green
    layout->addWidget(new QLabel(tr("G:"), this), 1, 0);
    m_greenSpinBox = new QSpinBox(this);
    m_greenSpinBox->setRange(0, 255);
    layout->addWidget(m_greenSpinBox, 1, 1);

    // Blue
    layout->addWidget(new QLabel(tr("B:"), this), 2, 0);
    m_blueSpinBox = new QSpinBox(this);
    m_blueSpinBox->setRange(0, 255);
    layout->addWidget(m_blueSpinBox, 2, 1);

    m_mainLayout->addWidget(m_rgbWidget);
}

void FluentColorPicker::setupHsvControls() {
    auto* layout = new QGridLayout(m_hsvWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    // Hue
    layout->addWidget(new QLabel(tr("H:"), this), 0, 0);
    m_hueSpinBox = new QSpinBox(this);
    m_hueSpinBox->setRange(0, 359);
    m_hueSpinBox->setSuffix("°");
    layout->addWidget(m_hueSpinBox, 0, 1);

    // Saturation
    layout->addWidget(new QLabel(tr("S:"), this), 1, 0);
    m_saturationSpinBox = new QSpinBox(this);
    m_saturationSpinBox->setRange(0, 100);
    m_saturationSpinBox->setSuffix("%");
    layout->addWidget(m_saturationSpinBox, 1, 1);

    // Value
    layout->addWidget(new QLabel(tr("V:"), this), 2, 0);
    m_valueSpinBox = new QSpinBox(this);
    m_valueSpinBox->setRange(0, 100);
    m_valueSpinBox->setSuffix("%");
    layout->addWidget(m_valueSpinBox, 2, 1);

    m_mainLayout->addWidget(m_hsvWidget);
}

void FluentColorPicker::setupAlphaSlider() {
    auto* layout = new QHBoxLayout();
    layout->addWidget(new QLabel(tr("Alpha:"), this));

    m_alphaSlider->setRange(0, 255);
    m_alphaSlider->setValue(255);
    layout->addWidget(m_alphaSlider);

    m_alphaLabel = new QLabel("100%", this);
    m_alphaLabel->setMinimumWidth(40);
    layout->addWidget(m_alphaLabel);

    auto* alphaWidget = new QWidget(this);
    alphaWidget->setLayout(layout);
    m_mainLayout->addWidget(alphaWidget);
}

void FluentColorPicker::setupAnimations() {
    // Color change animation
    m_colorAnimation = new QPropertyAnimation(this, "animatedColor", this);
    m_colorAnimation->setDuration(200);
    m_colorAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_colorAnimation, &QPropertyAnimation::finished, this,
            &FluentColorPicker::onColorAnimationFinished);
}

void FluentColorPicker::setupConnections() {
    // Hex edit
    connect(m_hexEdit, &QLineEdit::textChanged, this,
            &FluentColorPicker::onHexChanged);

    // RGB controls
    connect(m_redSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &FluentColorPicker::onRgbChanged);
    connect(m_greenSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &FluentColorPicker::onRgbChanged);
    connect(m_blueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &FluentColorPicker::onRgbChanged);

    // HSV controls
    connect(m_hueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &FluentColorPicker::onHsvChanged);
    connect(m_saturationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FluentColorPicker::onHsvChanged);
    connect(m_valueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &FluentColorPicker::onHsvChanged);

    // Alpha slider
    connect(m_alphaSlider, &QSlider::valueChanged, this,
            &FluentColorPicker::onAlphaChanged);

    // More colors button
    connect(m_moreColorsButton, &QPushButton::clicked, this,
            &FluentColorPicker::showColorDialog);
}

QPushButton* FluentColorPicker::createColorButton(const QColor& color) {
    auto* button = new QPushButton(this);
    button->setFixedSize(24, 24);
    button->setStyleSheet(QString("QPushButton {"
                                  "    background-color: %1;"
                                  "    border: 1px solid #ccc;"
                                  "    border-radius: 4px;"
                                  "}"
                                  "QPushButton:hover {"
                                  "    border: 2px solid #0078d4;"
                                  "}"
                                  "QPushButton:pressed {"
                                  "    border: 2px solid #005a9e;"
                                  "}")
                              .arg(color.name()));

    connect(button, &QPushButton::clicked, this,
            [this, color]() { setCurrentColor(color); });

    return button;
}

void FluentColorPicker::updateModeVisibility() {
    switch (m_mode) {
        case FluentColorPickerMode::Simple:
            m_paletteWidget->setVisible(true);
            m_customColorWidget->setVisible(false);
            m_hexEdit->setVisible(false);
            m_rgbWidget->setVisible(false);
            m_hsvWidget->setVisible(false);
            m_alphaSlider->parent()->setVisible(false);
            m_moreColorsButton->setVisible(true);
            break;

        case FluentColorPickerMode::Advanced:
            m_paletteWidget->setVisible(true);
            m_customColorWidget->setVisible(true);
            m_hexEdit->setVisible(m_showHex);
            m_rgbWidget->setVisible(m_showRgb);
            m_hsvWidget->setVisible(m_showHsv);
            m_alphaSlider->parent()->setVisible(m_showAlpha);
            m_moreColorsButton->setVisible(true);
            break;

        case FluentColorPickerMode::PaletteOnly:
            m_paletteWidget->setVisible(true);
            m_customColorWidget->setVisible(true);
            m_hexEdit->setVisible(false);
            m_rgbWidget->setVisible(false);
            m_hsvWidget->setVisible(false);
            m_alphaSlider->parent()->setVisible(false);
            m_moreColorsButton->setVisible(false);
            break;
    }
}

void FluentColorPicker::updateStandardColorPalette() {
    // Standard palette is created in setupStandardColorPalette()
    // This method can be used to refresh the palette if needed
}

void FluentColorPicker::updateCustomColorPalette() {
    // Clear existing buttons
    for (auto* button : m_customColorButtons) {
        m_customColorLayout->removeWidget(button);
        button->deleteLater();
    }
    m_customColorButtons.clear();

    // Add custom color buttons
    const int columns = 8;
    for (int i = 0; i < m_customColors.size(); ++i) {
        auto* colorButton = createColorButton(m_customColors[i]);
        m_customColorLayout->addWidget(colorButton, i / columns, i % columns);
        m_customColorButtons.append(colorButton);
    }

    m_customColorWidget->setVisible(!m_customColors.isEmpty());
}

void FluentColorPicker::updatePreview() {
    if (!m_colorPreview)
        return;

    const QString style =
        QString(
            "background-color: %1; border: 1px solid #ccc; border-radius: 4px;")
            .arg(m_currentColor.name());

    m_colorPreview->setStyleSheet(style);
}

void FluentColorPicker::updateHexEdit() {
    if (!m_hexEdit || m_updatingFromHex)
        return;

    m_updatingFromControls = true;

    QString hexText;
    if (m_showAlpha && m_currentColor.alpha() < 255) {
        hexText = QString("#%1%2%3%4")
                      .arg(m_currentColor.red(), 2, 16, QChar('0'))
                      .arg(m_currentColor.green(), 2, 16, QChar('0'))
                      .arg(m_currentColor.blue(), 2, 16, QChar('0'))
                      .arg(m_currentColor.alpha(), 2, 16, QChar('0'));
    } else {
        hexText = m_currentColor.name().toUpper();
    }

    m_hexEdit->setText(hexText);
    m_updatingFromControls = false;
}

void FluentColorPicker::updateRgbControls() {
    if (m_updatingFromRgb)
        return;

    m_updatingFromControls = true;
    m_redSpinBox->setValue(m_currentColor.red());
    m_greenSpinBox->setValue(m_currentColor.green());
    m_blueSpinBox->setValue(m_currentColor.blue());
    m_updatingFromControls = false;
}

void FluentColorPicker::updateHsvControls() {
    if (m_updatingFromHsv)
        return;

    m_updatingFromControls = true;
    m_hueSpinBox->setValue(m_currentColor.hue() == -1 ? 0
                                                      : m_currentColor.hue());
    m_saturationSpinBox->setValue(qRound(m_currentColor.saturationF() * 100));
    m_valueSpinBox->setValue(qRound(m_currentColor.valueF() * 100));
    m_updatingFromControls = false;
}

void FluentColorPicker::updateAlphaSlider() {
    if (m_updatingFromAlpha)
        return;

    m_updatingFromControls = true;
    m_alphaSlider->setValue(m_currentColor.alpha());

    const int percentage = qRound((m_currentColor.alpha() / 255.0) * 100);
    m_alphaLabel->setText(QString("%1%").arg(percentage));

    m_updatingFromControls = false;
}

void FluentColorPicker::onHexChanged(const QString& text) {
    if (m_updatingFromControls)
        return;

    QString cleanText = text;
    if (!cleanText.startsWith('#')) {
        cleanText.prepend('#');
    }

    QColor color(cleanText);
    if (color.isValid()) {
        m_updatingFromHex = true;
        setCurrentColor(color);
        m_updatingFromHex = false;
    }
}

void FluentColorPicker::onRgbChanged() {
    if (m_updatingFromControls)
        return;

    const QColor color(m_redSpinBox->value(), m_greenSpinBox->value(),
                       m_blueSpinBox->value(), m_currentColor.alpha());

    m_updatingFromRgb = true;
    setCurrentColor(color);
    m_updatingFromRgb = false;
}

void FluentColorPicker::onHsvChanged() {
    if (m_updatingFromControls)
        return;

    QColor color;
    color.setHsv(m_hueSpinBox->value(),
                 qRound((m_saturationSpinBox->value() / 100.0) * 255),
                 qRound((m_valueSpinBox->value() / 100.0) * 255),
                 m_currentColor.alpha());

    m_updatingFromHsv = true;
    setCurrentColor(color);
    m_updatingFromHsv = false;
}

void FluentColorPicker::onAlphaChanged(int value) {
    if (m_updatingFromControls)
        return;

    QColor color = m_currentColor;
    color.setAlpha(value);

    m_updatingFromAlpha = true;
    setCurrentColor(color);
    m_updatingFromAlpha = false;
}

void FluentColorPicker::onColorAnimationFinished() {
    m_animatedColor = m_currentColor;
}

void FluentColorPicker::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    // Update widget colors based on theme
    const QColor backgroundColor =
        theme.color(Styling::FluentThemeColor::LayerFillColorDefault);
    const QColor textColor =
        theme.color(Styling::FluentThemeColor::TextFillPrimary);

    setStyleSheet(
        QString("QWidget { background-color: %1; color: %2; }"
                "QLineEdit { background-color: %3; border: 1px solid %4; "
                "border-radius: 4px; padding: 4px; }"
                "QSpinBox { background-color: %3; border: 1px solid %4; "
                "border-radius: 4px; padding: 4px; }"
                "QPushButton { background-color: %5; border: 1px solid %4; "
                "border-radius: 4px; padding: 6px 12px; }"
                "QPushButton:hover { background-color: %6; }"
                "QPushButton:pressed { background-color: %7; }")
            .arg(backgroundColor.name())
            .arg(textColor.name())
            .arg(theme.color(Styling::FluentThemeColor::ControlFillDefault)
                     .name())
            .arg(theme.color(Styling::FluentThemeColor::ControlStrokeDefault)
                     .name())
            .arg(theme.color(Styling::FluentThemeColor::ControlFillSecondary)
                     .name())
            .arg(theme.color(Styling::FluentThemeColor::ControlFillTertiary)
                     .name())
            .arg(theme.color(Styling::FluentThemeColor::ControlFillQuarternary)
                     .name()));
}

void FluentColorPicker::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        const QString accessibleText =
            tr("Color picker. Current color: %1").arg(m_currentColor.name());
        setAccessibleName(accessibleText);
        setAccessibleDescription(accessibleText);

        QAccessibleEvent event(this, QAccessible::ValueChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

void FluentColorPicker::animateColorChange() {
    if (!m_colorAnimation)
        return;

    m_colorAnimation->stop();
    m_colorAnimation->setStartValue(m_animatedColor);
    m_colorAnimation->setEndValue(m_currentColor);
    m_colorAnimation->start();
}

// Property accessors for animations
QColor FluentColorPicker::animatedColor() const { return m_animatedColor; }

void FluentColorPicker::setAnimatedColor(const QColor& color) {
    if (m_animatedColor == color)
        return;
    m_animatedColor = color;
    updatePreview();
}

}  // namespace FluentQt::Components
