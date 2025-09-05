// src/Components/FluentSpinBox.cpp
#include "FluentQt/Components/FluentSpinBox.h"
#include <QAccessible>
#include <QApplication>
#include <QDoubleValidator>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLocale>
#include <QMouseEvent>
#include <QPainter>
#include <QRegularExpressionValidator>
#include <QTimer>
#include <QValidator>
#include <QWheelEvent>
#include <cmath>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentSpinBox::FluentSpinBox(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)),
      m_accelerationTimer(new QTimer(this)),
      m_lineEdit(new QLineEdit(this)) {
    setupLineEdit();
    setupAnimations();
    setupValidation();
    updateSizeMetrics();
    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(m_lineEdit);
    setAttribute(Qt::WA_Hover, true);

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentSpinBox::updateColors);

    connect(m_accelerationTimer, &QTimer::timeout, this,
            &FluentSpinBox::onAccelerationTimer);

    updateColors();
    updateDisplayText();
    updateAccessibility();
}

FluentSpinBox::FluentSpinBox(FluentSpinBoxType type, QWidget* parent)
    : FluentSpinBox(parent) {
    setSpinBoxType(type);
}

FluentSpinBox::FluentSpinBox(double minimum, double maximum, QWidget* parent)
    : FluentSpinBox(parent) {
    setRange(minimum, maximum);
}

FluentSpinBox::~FluentSpinBox() = default;

double FluentSpinBox::value() const { return m_value; }

void FluentSpinBox::setValue(double value) {
    const double boundedValue = boundValue(value);
    if (qFuzzyCompare(m_value, boundedValue))
        return;

    m_value = boundedValue;
    updateDisplayText();

    if (m_animated) {
        animateValueChange();
    }

    emit valueChanged(m_value);
    updateAccessibility();
}

double FluentSpinBox::minimum() const { return m_minimum; }

void FluentSpinBox::setMinimum(double minimum) {
    if (qFuzzyCompare(m_minimum, minimum))
        return;

    m_minimum = minimum;
    if (m_value < m_minimum) {
        setValue(m_minimum);
    }
    updateValidation();
    emit minimumChanged(m_minimum);
}

double FluentSpinBox::maximum() const { return m_maximum; }

void FluentSpinBox::setMaximum(double maximum) {
    if (qFuzzyCompare(m_maximum, maximum))
        return;

    m_maximum = maximum;
    if (m_value > m_maximum) {
        setValue(m_maximum);
    }
    updateValidation();
    emit maximumChanged(m_maximum);
}

void FluentSpinBox::setRange(double minimum, double maximum) {
    if (minimum > maximum) {
        qSwap(minimum, maximum);
    }

    setMinimum(minimum);
    setMaximum(maximum);
}

double FluentSpinBox::singleStep() const { return m_singleStep; }

void FluentSpinBox::setSingleStep(double step) {
    if (qFuzzyCompare(m_singleStep, step) || step <= 0.0)
        return;

    m_singleStep = step;
    emit singleStepChanged(m_singleStep);
}

int FluentSpinBox::decimals() const { return m_decimals; }

void FluentSpinBox::setDecimals(int decimals) {
    if (m_decimals == decimals)
        return;

    m_decimals = qBound(0, decimals, 10);
    updateValidation();
    updateDisplayText();
    emit decimalsChanged(m_decimals);
}

FluentSpinBoxType FluentSpinBox::spinBoxType() const { return m_spinBoxType; }

void FluentSpinBox::setSpinBoxType(FluentSpinBoxType type) {
    if (m_spinBoxType == type)
        return;

    m_spinBoxType = type;
    updateValidation();
    updateDisplayText();
    updateSizeMetrics();
    emit spinBoxTypeChanged(m_spinBoxType);
}

FluentSpinBoxButtonLayout FluentSpinBox::buttonLayout() const {
    return m_buttonLayout;
}

void FluentSpinBox::setButtonLayout(FluentSpinBoxButtonLayout layout) {
    if (m_buttonLayout == layout)
        return;

    m_buttonLayout = layout;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit buttonLayoutChanged(m_buttonLayout);
}

QString FluentSpinBox::prefix() const { return m_prefix; }

void FluentSpinBox::setPrefix(const QString& prefix) {
    if (m_prefix == prefix)
        return;

    m_prefix = prefix;
    updateDisplayText();
    emit prefixChanged(m_prefix);
}

QString FluentSpinBox::suffix() const { return m_suffix; }

void FluentSpinBox::setSuffix(const QString& suffix) {
    if (m_suffix == suffix)
        return;

    m_suffix = suffix;
    updateDisplayText();
    emit suffixChanged(m_suffix);
}

QString FluentSpinBox::placeholderText() const { return m_placeholderText; }

void FluentSpinBox::setPlaceholderText(const QString& text) {
    if (m_placeholderText == text)
        return;

    m_placeholderText = text;
    m_lineEdit->setPlaceholderText(text);
    emit placeholderTextChanged(m_placeholderText);
}

FluentSpinBoxSize FluentSpinBox::spinBoxSize() const { return m_spinBoxSize; }

void FluentSpinBox::setSpinBoxSize(FluentSpinBoxSize size) {
    if (m_spinBoxSize == size)
        return;

    m_spinBoxSize = size;
    updateSizeMetrics();
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit spinBoxSizeChanged(m_spinBoxSize);
}

FluentSpinBoxComplexity FluentSpinBox::complexity() const {
    return m_complexity;
}

void FluentSpinBox::setComplexity(FluentSpinBoxComplexity complexity) {
    if (m_complexity == complexity)
        return;

    m_complexity = complexity;
    // Complexity affects which features are available
    if (m_complexity == FluentSpinBoxComplexity::Simple) {
        setAnimated(false);
        setAccelerated(false);
    }
    emit complexityChanged(m_complexity);
}

bool FluentSpinBox::wrapping() const { return m_wrapping; }

void FluentSpinBox::setWrapping(bool wrapping) {
    if (m_wrapping == wrapping)
        return;

    m_wrapping = wrapping;
    emit wrappingChanged(m_wrapping);
}

bool FluentSpinBox::isReadOnly() const { return m_readOnly; }

void FluentSpinBox::setReadOnly(bool readOnly) {
    if (m_readOnly == readOnly)
        return;

    m_readOnly = readOnly;
    m_lineEdit->setReadOnly(readOnly);
    updateColors();
    emit readOnlyChanged(m_readOnly);
}

QString FluentSpinBox::text() const { return m_lineEdit->text(); }

bool FluentSpinBox::showButtons() const { return m_showButtons; }

void FluentSpinBox::setShowButtons(bool show) {
    if (m_showButtons == show)
        return;

    m_showButtons = show;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit showButtonsChanged(m_showButtons);
}

bool FluentSpinBox::accelerated() const { return m_accelerated; }

void FluentSpinBox::setAccelerated(bool accelerated) {
    if (m_accelerated == accelerated)
        return;

    m_accelerated = accelerated;
    emit acceleratedChanged(m_accelerated);
}

bool FluentSpinBox::isAnimated() const { return m_animated; }

void FluentSpinBox::setAnimated(bool animated) {
    if (m_animated == animated)
        return;

    m_animated = animated;
    emit animatedChanged(m_animated);
}

QString FluentSpinBox::displayText() const { return formatValue(m_value); }

QString FluentSpinBox::cleanText() const {
    QString text = m_lineEdit->text();

    // Remove prefix and suffix
    if (!m_prefix.isEmpty() && text.startsWith(m_prefix)) {
        text = text.mid(m_prefix.length());
    }
    if (!m_suffix.isEmpty() && text.endsWith(m_suffix)) {
        text = text.left(text.length() - m_suffix.length());
    }

    // Remove currency symbols and percentage signs for specific types
    if (m_spinBoxType == FluentSpinBoxType::Currency) {
        text = text.remove(QRegularExpression("[^0-9.,+-]"));
    } else if (m_spinBoxType == FluentSpinBoxType::Percentage) {
        text = text.remove('%').trimmed();
    }

    return text.trimmed();
}

bool FluentSpinBox::isValid() const { return isValidValue(m_value); }

bool FluentSpinBox::isValidValue(double value) const {
    return value >= m_minimum && value <= m_maximum;
}

QValidator::State FluentSpinBox::validate(QString& input, int& pos) const {
    if (m_lineEdit->validator()) {
        return m_lineEdit->validator()->validate(input, pos);
    }
    return QValidator::Acceptable;
}

void FluentSpinBox::fixup(QString& input) const {
    const double value = parseValue(input);
    const double boundedValue = boundValue(value);
    input = formatValue(boundedValue);
}

double FluentSpinBox::boundValue(double value) const {
    if (m_wrapping) {
        if (value > m_maximum) {
            const double range = m_maximum - m_minimum + m_singleStep;
            value = m_minimum + std::fmod(value - m_minimum, range);
        } else if (value < m_minimum) {
            const double range = m_maximum - m_minimum + m_singleStep;
            value = m_maximum - std::fmod(m_minimum - value, range);
        }
    } else {
        value = qBound(m_minimum, value, m_maximum);
    }

    return value;
}

QRect FluentSpinBox::upButtonRect() const {
    if (m_layoutDirty) {
        const_cast<FluentSpinBox*>(this)->updateLayout();
    }
    return m_upButtonRect;
}

QRect FluentSpinBox::downButtonRect() const {
    if (m_layoutDirty) {
        const_cast<FluentSpinBox*>(this)->updateLayout();
    }
    return m_downButtonRect;
}

QRect FluentSpinBox::lineEditRect() const {
    if (m_layoutDirty) {
        const_cast<FluentSpinBox*>(this)->updateLayout();
    }
    return m_lineEditRect;
}

QSize FluentSpinBox::sizeHint() const {
    const QFontMetrics fm(font());
    const QString sampleText = formatValue(m_maximum) + "W";  // Add extra width
    const int textWidth = fm.horizontalAdvance(sampleText);
    const int textHeight = fm.height();

    int width = textWidth + m_contentMargins.left() + m_contentMargins.right();
    int height =
        textHeight + m_contentMargins.top() + m_contentMargins.bottom();

    if (m_showButtons) {
        if (m_buttonLayout == FluentSpinBoxButtonLayout::Vertical ||
            m_buttonLayout == FluentSpinBoxButtonLayout::Embedded) {
            width += m_buttonSize;
        } else if (m_buttonLayout == FluentSpinBoxButtonLayout::Horizontal ||
                   m_buttonLayout == FluentSpinBoxButtonLayout::Sides) {
            width += m_buttonSize * 2 + m_spacing;
        }

        height = qMax(height, m_buttonSize);
    }

    return QSize(width, height);
}

QSize FluentSpinBox::minimumSizeHint() const {
    const QFontMetrics fm(font());
    const int minWidth = fm.horizontalAdvance("000") + m_contentMargins.left() +
                         m_contentMargins.right();
    const int minHeight =
        fm.height() + m_contentMargins.top() + m_contentMargins.bottom();

    return QSize(minWidth, minHeight);
}

FluentSpinBox* FluentSpinBox::createIntegerSpinBox(int minimum, int maximum,
                                                   QWidget* parent) {
    auto* spinBox = new FluentSpinBox(parent);
    spinBox->setSpinBoxType(FluentSpinBoxType::Integer);
    spinBox->setRange(minimum, maximum);
    spinBox->setDecimals(0);
    return spinBox;
}

FluentSpinBox* FluentSpinBox::createDoubleSpinBox(double minimum,
                                                  double maximum, int decimals,
                                                  QWidget* parent) {
    auto* spinBox = new FluentSpinBox(parent);
    spinBox->setSpinBoxType(FluentSpinBoxType::Double);
    spinBox->setRange(minimum, maximum);
    spinBox->setDecimals(decimals);
    return spinBox;
}

FluentSpinBox* FluentSpinBox::createCurrencySpinBox(double minimum,
                                                    double maximum,
                                                    QWidget* parent) {
    auto* spinBox = new FluentSpinBox(parent);
    spinBox->setSpinBoxType(FluentSpinBoxType::Currency);
    spinBox->setRange(minimum, maximum);
    spinBox->setDecimals(2);
    return spinBox;
}

FluentSpinBox* FluentSpinBox::createPercentageSpinBox(QWidget* parent) {
    auto* spinBox = new FluentSpinBox(parent);
    spinBox->setSpinBoxType(FluentSpinBoxType::Percentage);
    spinBox->setRange(0.0, 100.0);
    spinBox->setDecimals(1);
    spinBox->setSuffix("%");
    return spinBox;
}

FluentSpinBox* FluentSpinBox::createSimple(FluentSpinBoxType type,
                                           QWidget* parent) {
    auto* spinBox = new FluentSpinBox(parent);
    spinBox->setSpinBoxType(type);
    spinBox->setComplexity(FluentSpinBoxComplexity::Simple);
    spinBox->setSpinBoxSize(FluentSpinBoxSize::Medium);
    return spinBox;
}

FluentSpinBox* FluentSpinBox::createSimpleInteger(int minimum, int maximum,
                                                  QWidget* parent) {
    auto* spinBox = createSimple(FluentSpinBoxType::Integer, parent);
    spinBox->setRange(minimum, maximum);
    spinBox->setDecimals(0);
    return spinBox;
}

FluentSpinBox* FluentSpinBox::createSimpleDouble(double minimum, double maximum,
                                                 int decimals,
                                                 QWidget* parent) {
    auto* spinBox = createSimple(FluentSpinBoxType::Double, parent);
    spinBox->setRange(minimum, maximum);
    spinBox->setDecimals(decimals);
    return spinBox;
}

void FluentSpinBox::stepUp() {
    if (!isEnabled() || m_readOnly)
        return;

    const double newValue = m_value + m_singleStep;
    setValue(newValue);
    // Note: stepUpRequested signal would be declared in header if needed
}

void FluentSpinBox::stepDown() {
    if (!isEnabled() || m_readOnly)
        return;

    const double newValue = m_value - m_singleStep;
    setValue(newValue);
    // Note: stepDownRequested signal would be declared in header if needed
}

void FluentSpinBox::stepBy(int steps) {
    if (!isEnabled() || m_readOnly)
        return;

    const double newValue = m_value + (steps * m_singleStep);
    setValue(newValue);
}

void FluentSpinBox::animateValue(double targetValue) {
    if (!m_animated || m_complexity == FluentSpinBoxComplexity::Simple) {
        setValue(targetValue);
        return;
    }

    if (m_valueAnimation) {
        m_valueAnimation->stop();
        m_valueAnimation->setStartValue(m_value);
        m_valueAnimation->setEndValue(targetValue);
        m_valueAnimation->start();
    }
}

void FluentSpinBox::selectAll() { m_lineEdit->selectAll(); }

void FluentSpinBox::clear() { setValue(m_minimum); }

void FluentSpinBox::setupLineEdit() {
    m_lineEdit->setFrame(false);
    m_lineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    connect(m_lineEdit, &QLineEdit::textChanged, this,
            &FluentSpinBox::onLineEditTextChanged);
    connect(m_lineEdit, &QLineEdit::editingFinished, this,
            &FluentSpinBox::onLineEditEditingFinished);
    connect(m_lineEdit, &QLineEdit::returnPressed, this,
            &FluentSpinBox::onLineEditReturnPressed);
}

void FluentSpinBox::setupAnimations() {
    // Use Fluent UI animation durations and easing curves
    const auto& theme = Styling::FluentTheme::instance();

    // Value change animation - smooth transition for value updates
    m_valueAnimation = new QPropertyAnimation(this, "animatedValue", this);
    m_valueAnimation->setDuration(250);  // Fluent UI standard duration
    m_valueAnimation->setEasingCurve(
        QEasingCurve::OutCubic);  // Fluent UI standard easing
    connect(m_valueAnimation, &QPropertyAnimation::finished, this,
            &FluentSpinBox::onValueAnimationFinished);

    // Button press animation - subtle feedback
    m_buttonAnimation = new QPropertyAnimation(this, "buttonScale", this);
    m_buttonAnimation->setDuration(150);  // Quick feedback
    m_buttonAnimation->setEasingCurve(
        QEasingCurve::OutQuart);  // Fluent UI button easing

    // Focus animation - smooth focus transition
    m_focusAnimation = new QPropertyAnimation(this, "focusOpacity", this);
    m_focusAnimation->setDuration(200);  // Fluent UI focus duration
    m_focusAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void FluentSpinBox::setupValidation() { updateValidation(); }

void FluentSpinBox::updateLayout() {
    if (!m_layoutDirty)
        return;

    const QRect rect = this->rect();
    const int buttonWidth = m_showButtons ? m_buttonSize : 0;
    const int buttonHeight = m_buttonSize;

    // Calculate line edit rect
    QRect lineEditRect =
        rect.adjusted(m_contentMargins.left(), m_contentMargins.top(),
                      -m_contentMargins.right(), -m_contentMargins.bottom());

    // Adjust for buttons based on layout
    if (m_showButtons) {
        switch (m_buttonLayout) {
            case FluentSpinBoxButtonLayout::Vertical:
                lineEditRect.adjust(0, 0, -buttonWidth, 0);
                m_upButtonRect = QRect(lineEditRect.right(), lineEditRect.top(),
                                       buttonWidth, lineEditRect.height() / 2);
                m_downButtonRect = QRect(
                    lineEditRect.right(), m_upButtonRect.bottom(), buttonWidth,
                    lineEditRect.height() - m_upButtonRect.height());
                break;

            case FluentSpinBoxButtonLayout::Horizontal:
                lineEditRect.adjust(0, 0, -(buttonWidth * 2 + m_spacing), 0);
                m_downButtonRect =
                    QRect(lineEditRect.right() + m_spacing,
                          lineEditRect.center().y() - buttonHeight / 2,
                          buttonWidth, buttonHeight);
                m_upButtonRect =
                    QRect(m_downButtonRect.right() + m_spacing,
                          lineEditRect.center().y() - buttonHeight / 2,
                          buttonWidth, buttonHeight);
                break;

            case FluentSpinBoxButtonLayout::Sides:
                lineEditRect.adjust(buttonWidth + m_spacing, 0,
                                    -(buttonWidth + m_spacing), 0);
                m_downButtonRect =
                    QRect(rect.left() + m_contentMargins.left(),
                          lineEditRect.center().y() - buttonHeight / 2,
                          buttonWidth, buttonHeight);
                m_upButtonRect =
                    QRect(lineEditRect.right() + m_spacing,
                          lineEditRect.center().y() - buttonHeight / 2,
                          buttonWidth, buttonHeight);
                break;

            case FluentSpinBoxButtonLayout::Embedded:
                const int embeddedButtonSize = lineEditRect.height() - 4;
                const int embeddedSpacing = 2;
                m_upButtonRect = QRect(
                    lineEditRect.right() - embeddedButtonSize - embeddedSpacing,
                    lineEditRect.top() + embeddedSpacing,
                    embeddedButtonSize / 2, embeddedButtonSize);
                m_downButtonRect =
                    QRect(m_upButtonRect.left() - embeddedButtonSize / 2,
                          lineEditRect.top() + embeddedSpacing,
                          embeddedButtonSize / 2, embeddedButtonSize);
                lineEditRect.adjust(
                    0, 0, -(embeddedButtonSize + embeddedSpacing * 2), 0);
                break;
        }
    } else {
        m_upButtonRect = QRect();
        m_downButtonRect = QRect();
    }

    m_lineEditRect = lineEditRect;
    m_lineEdit->setGeometry(lineEditRect);

    m_layoutDirty = false;
}

void FluentSpinBox::updateSizeMetrics() {
    const auto& theme = Styling::FluentTheme::instance();

    // Use Fluent UI design tokens for sizing based on spinBoxSize
    int baseButtonSize;
    int baseSpacing;
    QMargins baseMargins;

    switch (m_spinBoxSize) {
        case FluentSpinBoxSize::Small:
            baseButtonSize = theme.componentHeight("small");
            baseSpacing = theme.spacing("s");  // 4px
            baseMargins = theme.margins("small");
            break;
        case FluentSpinBoxSize::Medium:
            baseButtonSize = theme.componentHeight("medium");
            baseSpacing = theme.spacing("m");  // 8px
            baseMargins = theme.margins("medium");
            break;
        case FluentSpinBoxSize::Large:
            baseButtonSize = theme.componentHeight("large");
            baseSpacing = theme.spacing("l");  // 12px
            baseMargins = theme.margins("large");
            break;
    }

    // Adjust for spinBoxType if needed
    switch (m_spinBoxType) {
        case FluentSpinBoxType::Integer:
        case FluentSpinBoxType::Double:
            m_buttonSize = baseButtonSize;
            m_spacing = baseSpacing;
            break;
        case FluentSpinBoxType::Currency:
        case FluentSpinBoxType::Percentage:
            // Currency and percentage need slightly larger buttons for better
            // usability
            m_buttonSize = baseButtonSize + theme.spacing("xs");  // +2px
            m_spacing = baseSpacing + theme.spacing("xs");        // +2px
            break;
    }

    m_contentMargins = baseMargins;
}

void FluentSpinBox::updateValidation() {
    delete m_lineEdit->validator();

    QValidator* validator = nullptr;

    switch (m_spinBoxType) {
        case FluentSpinBoxType::Integer:
            validator = new QIntValidator(static_cast<int>(m_minimum),
                                          static_cast<int>(m_maximum), this);
            break;

        case FluentSpinBoxType::Double:
        case FluentSpinBoxType::Currency:
        case FluentSpinBoxType::Percentage:
            auto* doubleValidator =
                new QDoubleValidator(m_minimum, m_maximum, m_decimals, this);
            doubleValidator->setNotation(QDoubleValidator::StandardNotation);
            validator = doubleValidator;
            break;
    }

    m_lineEdit->setValidator(validator);
}

void FluentSpinBox::updateDisplayText() {
    const QString text = formatValue(m_value);
    if (m_lineEdit->text() != text) {
        m_lineEdit->setText(text);
    }
}

QString FluentSpinBox::formatValue(double value) const {
    QString text;

    switch (m_spinBoxType) {
        case FluentSpinBoxType::Integer:
            text = QString::number(static_cast<int>(value));
            break;

        case FluentSpinBoxType::Double:
            text = QString::number(value, 'f', m_decimals);
            break;

        case FluentSpinBoxType::Currency: {
            QLocale locale;
            text = locale.toCurrencyString(value);
            break;
        }

        case FluentSpinBoxType::Percentage:
            text = QString::number(value, 'f', m_decimals);
            break;
    }

    return m_prefix + text + m_suffix;
}

double FluentSpinBox::parseValue(const QString& text) const {
    QString cleanedText = text;

    // Remove prefix and suffix
    if (!m_prefix.isEmpty() && cleanedText.startsWith(m_prefix)) {
        cleanedText = cleanedText.mid(m_prefix.length());
    }
    if (!m_suffix.isEmpty() && cleanedText.endsWith(m_suffix)) {
        cleanedText =
            cleanedText.left(cleanedText.length() - m_suffix.length());
    }

    cleanedText = cleanedText.trimmed();

    bool ok = false;
    double value = 0.0;

    switch (m_spinBoxType) {
        case FluentSpinBoxType::Integer:
            value = cleanedText.toInt(&ok);
            break;

        case FluentSpinBoxType::Double:
        case FluentSpinBoxType::Percentage:
            value = cleanedText.toDouble(&ok);
            break;

        case FluentSpinBoxType::Currency: {
            QLocale locale;
            value = locale.toDouble(cleanedText, &ok);
            if (!ok) {
                // Try parsing without currency symbols
                QString numericText = cleanedText;
                numericText.remove(QRegularExpression("[^0-9.,+-]"));
                value = numericText.toDouble(&ok);
            }
            break;
        }
    }

    return ok ? value : m_value;
}

void FluentSpinBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_layoutDirty) {
        updateLayout();
    }

    // Paint background
    paintBackground(&painter);

    // Paint buttons
    if (m_showButtons) {
        paintButtons(&painter);
    }

    // Paint focus indicator
    if (hasFocus()) {
        paintFocusIndicator(&painter);
    }
}

void FluentSpinBox::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isEnabled() && !m_readOnly) {
        if (m_showButtons) {
            if (m_upButtonRect.contains(event->pos())) {
                m_upButtonPressed = true;
                startButtonPressAnimation(true);
                stepUp();
                startAcceleration(true);
                update();
                return;
            } else if (m_downButtonRect.contains(event->pos())) {
                m_downButtonPressed = true;
                startButtonPressAnimation(false);
                stepDown();
                startAcceleration(false);
                update();
                return;
            }
        }
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentSpinBox::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_upButtonPressed = false;
        m_downButtonPressed = false;
        stopAcceleration();
        update();
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentSpinBox::wheelEvent(QWheelEvent* event) {
    if (!isEnabled() || m_readOnly) {
        event->ignore();
        return;
    }

    if (hasFocus() || m_lineEdit->hasFocus()) {
        const int delta = event->angleDelta().y();
        if (delta > 0) {
            stepUp();
        } else if (delta < 0) {
            stepDown();
        }
        event->accept();
    } else {
        event->ignore();
    }
}

void FluentSpinBox::keyPressEvent(QKeyEvent* event) {
    if (!isEnabled() || m_readOnly) {
        Core::FluentComponent::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
        case Qt::Key_Up:
            stepUp();
            event->accept();
            return;

        case Qt::Key_Down:
            stepDown();
            event->accept();
            return;

        case Qt::Key_PageUp:
            setValue(m_value + m_singleStep * 10);
            event->accept();
            return;

        case Qt::Key_PageDown:
            setValue(m_value - m_singleStep * 10);
            event->accept();
            return;

        case Qt::Key_Home:
            setValue(m_minimum);
            event->accept();
            return;

        case Qt::Key_End:
            setValue(m_maximum);
            event->accept();
            return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentSpinBox::focusInEvent(QFocusEvent* event) {
    if (m_animated) {
        startFocusAnimation(true);
    }
    updateColors();
    update();
    Core::FluentComponent::focusInEvent(event);
}

void FluentSpinBox::focusOutEvent(QFocusEvent* event) {
    if (m_animated) {
        startFocusAnimation(false);
    }
    updateColors();
    update();
    Core::FluentComponent::focusOutEvent(event);
}

void FluentSpinBox::resizeEvent(QResizeEvent* event) {
    m_layoutDirty = true;
    Core::FluentComponent::resizeEvent(event);
}

void FluentSpinBox::changeEvent(QEvent* event) {
    if (event->type() == QEvent::EnabledChange ||
        event->type() == QEvent::FontChange ||
        event->type() == QEvent::StyleChange) {
        updateColors();
        updateDisplayText();
        update();
    }
    Core::FluentComponent::changeEvent(event);
}

void FluentSpinBox::onLineEditTextChanged(const QString& text) {
    if (m_updatingFromValue)
        return;

    emit textChanged(text);

    const double newValue = parseValue(text);
    if (!qFuzzyCompare(newValue, m_value)) {
        m_updatingFromLineEdit = true;
        setValue(newValue);
        m_updatingFromLineEdit = false;
    }
}

void FluentSpinBox::onLineEditEditingFinished() {
    updateDisplayText();
    emit editingFinished();
}

void FluentSpinBox::onLineEditReturnPressed() {
    updateDisplayText();
    emit returnPressed();
}

void FluentSpinBox::onAccelerationTimer() {
    if (m_accelerationDirection > 0) {
        stepUp();
    } else if (m_accelerationDirection < 0) {
        stepDown();
    }

    // Increase acceleration
    if (m_accelerated && m_accelerationTimer->interval() > 50) {
        m_accelerationTimer->setInterval(
            qMax(50, m_accelerationTimer->interval() - 10));
    }
}

void FluentSpinBox::onValueAnimationFinished() { m_animatedValue = m_value; }

void FluentSpinBox::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    if (isEnabled() && !m_readOnly) {
        if (hasFocus()) {
            m_backgroundColor =
                theme.color(Styling::FluentThemeColor::ControlFillInputActive);
            m_borderColor =
                theme.color(Styling::FluentThemeColor::AccentFillDefault);
            m_textColor =
                theme.color(Styling::FluentThemeColor::TextFillPrimary);
        } else {
            m_backgroundColor =
                theme.color(Styling::FluentThemeColor::ControlFillDefault);
            m_borderColor =
                theme.color(Styling::FluentThemeColor::ControlStrokeDefault);
            m_textColor =
                theme.color(Styling::FluentThemeColor::TextFillPrimary);
        }
        m_buttonColor =
            theme.color(Styling::FluentThemeColor::ControlFillSecondary);
        m_buttonHoverColor =
            theme.color(Styling::FluentThemeColor::ControlFillTertiary);
    } else {
        m_backgroundColor =
            theme.color(Styling::FluentThemeColor::ControlFillDisabled);
        m_borderColor =
            theme.color(Styling::FluentThemeColor::ControlStrokeDisabled);
        m_textColor = theme.color(Styling::FluentThemeColor::TextFillDisabled);
        m_buttonColor =
            theme.color(Styling::FluentThemeColor::ControlFillDisabled);
        m_buttonHoverColor = m_buttonColor;
    }

    // Update line edit colors
    QPalette palette = m_lineEdit->palette();
    palette.setColor(QPalette::Base, m_backgroundColor);
    palette.setColor(QPalette::Text, m_textColor);
    m_lineEdit->setPalette(palette);

    update();
}

void FluentSpinBox::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        // Set proper accessibility properties
        setAccessibleName(QString("Spin box, value %1").arg(m_value));
        setAccessibleDescription(
            QString("Spin box with range %1 to %2, step %3")
                .arg(m_minimum)
                .arg(m_maximum)
                .arg(m_singleStep));

        // Update accessibility role
        if (m_spinBoxType == FluentSpinBoxType::Integer) {
            setProperty("accessibleRole", "spinbox");
        } else {
            setProperty("accessibleRole", "spinbox");
        }

        // Notify screen readers of value changes
        QAccessibleEvent event(this, QAccessible::ValueChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

void FluentSpinBox::paintBackground(QPainter* painter) {
    const QRect rect = this->rect();
    const auto& theme = Styling::FluentTheme::instance();
    const int radius = theme.borderRadius("input");

    painter->save();

    // Draw background
    painter->setPen(QPen(m_borderColor, 1));
    painter->setBrush(m_backgroundColor);
    painter->drawRoundedRect(rect, radius, radius);

    painter->restore();
}

void FluentSpinBox::paintButtons(QPainter* painter) {
    if (!m_showButtons)
        return;

    painter->save();

    // Paint up button
    if (!m_upButtonRect.isEmpty()) {
        paintButton(painter, m_upButtonRect, true, m_upButtonPressed);
    }

    // Paint down button
    if (!m_downButtonRect.isEmpty()) {
        paintButton(painter, m_downButtonRect, false, m_downButtonPressed);
    }

    painter->restore();
}

void FluentSpinBox::paintButton(QPainter* painter, const QRect& rect, bool isUp,
                                bool isPressed) {
    const auto& theme = Styling::FluentTheme::instance();
    const int buttonRadius =
        theme.borderRadius("small");  // Smaller radius for buttons

    // Draw button background
    QColor buttonColor = isPressed ? m_buttonHoverColor : m_buttonColor;
    painter->setPen(Qt::NoPen);
    painter->setBrush(buttonColor);
    painter->drawRoundedRect(rect, buttonRadius, buttonRadius);

    // Draw arrow with proper Fluent UI styling
    const int arrowSize = qMin(rect.width(), rect.height()) / 3;
    const QPoint center = rect.center();

    QPen arrowPen(m_textColor, 2);
    arrowPen.setCapStyle(Qt::RoundCap);
    arrowPen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(arrowPen);

    if (isUp) {
        // Up arrow - Fluent UI chevron style
        QPolygon arrow;
        arrow << QPoint(center.x(), center.y() - arrowSize / 2)
              << QPoint(center.x() - arrowSize / 2, center.y() + arrowSize / 2)
              << QPoint(center.x() + arrowSize / 2, center.y() + arrowSize / 2);
        painter->drawPolyline(arrow);
    } else {
        // Down arrow - Fluent UI chevron style
        QPolygon arrow;
        arrow << QPoint(center.x() - arrowSize / 2, center.y() - arrowSize / 2)
              << QPoint(center.x(), center.y() + arrowSize / 2)
              << QPoint(center.x() + arrowSize / 2, center.y() - arrowSize / 2);
        painter->drawPolyline(arrow);
    }
}

void FluentSpinBox::paintFocusIndicator(QPainter* painter) {
    if (!hasFocus())
        return;

    painter->save();

    const auto& theme = Styling::FluentTheme::instance();
    const QColor focusColor =
        theme.color(Styling::FluentThemeColor::AccentFillDefault);
    const int radius = theme.borderRadius("input");

    QPen focusPen(focusColor, 2);
    painter->setPen(focusPen);
    painter->setBrush(Qt::NoBrush);

    const QRect focusRect = rect().adjusted(1, 1, -1, -1);
    painter->drawRoundedRect(focusRect, radius, radius);

    painter->restore();
}

void FluentSpinBox::animateValueChange() {
    if (!m_valueAnimation)
        return;

    m_valueAnimation->stop();
    m_valueAnimation->setStartValue(m_animatedValue);
    m_valueAnimation->setEndValue(m_value);
    m_valueAnimation->start();
}

void FluentSpinBox::startButtonPressAnimation(bool isUp) {
    if (!m_buttonAnimation)
        return;

    m_buttonAnimation->stop();
    m_buttonAnimation->setStartValue(m_buttonScale);
    m_buttonAnimation->setEndValue(0.95);
    m_buttonAnimation->start();
}

void FluentSpinBox::startFocusAnimation(bool focusIn) {
    if (!m_focusAnimation)
        return;

    m_focusAnimation->stop();
    m_focusAnimation->setStartValue(m_focusOpacity);
    m_focusAnimation->setEndValue(focusIn ? 1.0 : 0.0);
    m_focusAnimation->start();
}

void FluentSpinBox::startAcceleration(bool up) {
    if (!m_accelerated)
        return;

    m_accelerationDirection = up ? 1 : -1;
    m_accelerationTimer->start(300);  // Start with 300ms interval
}

void FluentSpinBox::stopAcceleration() {
    m_accelerationDirection = 0;
    m_accelerationTimer->stop();
}

// Property accessors for animations
double FluentSpinBox::animatedValue() const { return m_animatedValue; }

void FluentSpinBox::setAnimatedValue(double value) {
    if (qFuzzyCompare(m_animatedValue, value))
        return;
    m_animatedValue = value;
    if (!m_updatingFromLineEdit) {
        m_updatingFromValue = true;
        updateDisplayText();
        m_updatingFromValue = false;
    }
}

qreal FluentSpinBox::buttonScale() const { return m_buttonScale; }

void FluentSpinBox::setButtonScale(qreal scale) {
    if (qFuzzyCompare(m_buttonScale, scale))
        return;
    m_buttonScale = scale;
    update();
}

qreal FluentSpinBox::focusOpacity() const { return m_focusOpacity; }

void FluentSpinBox::setFocusOpacity(qreal opacity) {
    if (qFuzzyCompare(m_focusOpacity, opacity))
        return;
    m_focusOpacity = opacity;
    update();
}

}  // namespace FluentQt::Components
