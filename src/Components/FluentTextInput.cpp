// src/Components/FluentTextInput.cpp
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QClipboard>
#include <QEnterEvent>
#include <QFocusEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QRegularExpression>
#include <QUrl>

namespace FluentQt::Components {

FluentTextInput::FluentTextInput(QWidget* parent)
    : FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setObjectName("FluentTextInput");
    setFocusPolicy(Qt::StrongFocus);

    setupUI();
    setupAnimations();
    setupConnections();
    updateColors();
    updateFonts();

    // Initialize validation timer
    m_validationTimer = new QTimer(this);
    m_validationTimer->setSingleShot(true);
    m_validationTimer->setInterval(500);  // 500ms delay for validation
    connect(m_validationTimer, &QTimer::timeout, this,
            &FluentTextInput::validate);
}

FluentTextInput::~FluentTextInput() = default;

void FluentTextInput::setupUI() {
    FLUENT_PROFILE("FluentTextInput::setupUI");

    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(4);

    // Create label
    m_label = new QLabel(this);
    m_label->setObjectName("FluentTextInput_Label");
    m_label->hide();  // Initially hidden
    m_mainLayout->addWidget(m_label);

    // Create input layout
    m_inputLayout = new QHBoxLayout();
    m_inputLayout->setContentsMargins(0, 0, 0, 0);
    m_inputLayout->setSpacing(0);

    // Create line edit
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setObjectName("FluentTextInput_LineEdit");
    m_lineEdit->setFrame(false);
    m_inputLayout->addWidget(m_lineEdit);

    // Create clear button
    m_clearButton = new QPushButton(this);
    m_clearButton->setObjectName("FluentTextInput_ClearButton");
    m_clearButton->setText("×");
    m_clearButton->setFixedSize(24, 24);
    m_clearButton->setFlat(true);
    m_clearButton->hide();
    m_inputLayout->addWidget(m_clearButton);

    m_mainLayout->addLayout(m_inputLayout);

    // Create helper text label
    m_helperLabel = new QLabel(this);
    m_helperLabel->setObjectName("FluentTextInput_HelperLabel");
    m_helperLabel->hide();
    m_mainLayout->addWidget(m_helperLabel);

    // Create error text label
    m_errorLabel = new QLabel(this);
    m_errorLabel->setObjectName("FluentTextInput_ErrorLabel");
    m_errorLabel->hide();
    m_mainLayout->addWidget(m_errorLabel);

    // Set minimum height
    setMinimumHeight(32);
}

void FluentTextInput::setupAnimations() {
    FLUENT_PROFILE("FluentTextInput::setupAnimations");

    // Create opacity effects for helper and error labels
    m_helperOpacityEffect = new QGraphicsOpacityEffect(this);
    m_helperOpacityEffect->setOpacity(1.0);
    m_helperLabel->setGraphicsEffect(m_helperOpacityEffect);

    m_errorOpacityEffect = new QGraphicsOpacityEffect(this);
    m_errorOpacityEffect->setOpacity(0.0);
    m_errorLabel->setGraphicsEffect(m_errorOpacityEffect);

    // Create animations
    m_focusAnimation = new QPropertyAnimation(this, "geometry", this);
    m_focusAnimation->setDuration(200);
    m_focusAnimation->setEasingCurve(QEasingCurve::OutCubic);

    m_stateAnimation = new QPropertyAnimation(this, "geometry", this);
    m_stateAnimation->setDuration(150);
    m_stateAnimation->setEasingCurve(QEasingCurve::OutCubic);

    m_validationAnimation =
        new QPropertyAnimation(m_errorOpacityEffect, "opacity", this);
    m_validationAnimation->setDuration(200);
    m_validationAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void FluentTextInput::setupConnections() {
    // Connect line edit signals
    connect(m_lineEdit, &QLineEdit::textChanged, this,
            &FluentTextInput::onTextChanged);
    connect(m_lineEdit, &QLineEdit::textEdited, this,
            &FluentTextInput::onTextEdited);
    connect(m_lineEdit, &QLineEdit::returnPressed, this,
            &FluentTextInput::onReturnPressed);
    connect(m_lineEdit, &QLineEdit::editingFinished, this,
            &FluentTextInput::onEditingFinished);

    // Connect clear button
    connect(m_clearButton, &QPushButton::clicked, this,
            &FluentTextInput::onClearButtonClicked);

    // Connect theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentTextInput::updateAnimations);
}

QString FluentTextInput::text() const { return m_lineEdit->text(); }

void FluentTextInput::setText(const QString& text) {
    if (m_lineEdit->text() != text) {
        m_lineEdit->setText(text);
        updateClearButton();
        emit textChanged(text);
    }
}

QString FluentTextInput::placeholderText() const {
    return m_lineEdit->placeholderText();
}

void FluentTextInput::setPlaceholderText(const QString& text) {
    if (m_lineEdit->placeholderText() != text) {
        m_lineEdit->setPlaceholderText(text);
        emit placeholderTextChanged(text);
    }
}

QString FluentTextInput::labelText() const { return m_label->text(); }

void FluentTextInput::setLabelText(const QString& text) {
    if (m_label->text() != text) {
        m_label->setText(text);
        m_label->setVisible(!text.isEmpty());
        emit labelTextChanged(text);
    }
}

QString FluentTextInput::helperText() const { return m_helperLabel->text(); }

void FluentTextInput::setHelperText(const QString& text) {
    if (m_helperLabel->text() != text) {
        m_helperLabel->setText(text);
        m_helperLabel->setVisible(!text.isEmpty());
        emit helperTextChanged(text);
    }
}

QString FluentTextInput::errorText() const { return m_errorLabel->text(); }

void FluentTextInput::setErrorText(const QString& text) {
    if (m_errorLabel->text() != text) {
        m_errorLabel->setText(text);
        m_errorLabel->setVisible(!text.isEmpty());

        if (!text.isEmpty()) {
            setInputState(FluentTextInputState::Error);
        }

        emit errorTextChanged(text);
        animateValidationChange();
    }
}

FluentTextInputType FluentTextInput::inputType() const { return m_inputType; }

void FluentTextInput::setInputType(FluentTextInputType type) {
    if (m_inputType != type) {
        m_inputType = type;
        updateInputType();
        emit inputTypeChanged(type);
    }
}

FluentTextInputState FluentTextInput::inputState() const {
    return m_inputState;
}

void FluentTextInput::setInputState(FluentTextInputState state) {
    if (m_inputState != state) {
        m_inputState = state;
        updateInputState();
        emit inputStateChanged(state);
        animateStateChange();
    }
}

bool FluentTextInput::isRequired() const { return m_required; }

void FluentTextInput::setRequired(bool required) {
    if (m_required != required) {
        m_required = required;
        emit requiredChanged(required);
    }
}

bool FluentTextInput::isClearButtonVisible() const {
    return m_clearButtonVisible;
}

void FluentTextInput::setClearButtonVisible(bool visible) {
    if (m_clearButtonVisible != visible) {
        m_clearButtonVisible = visible;
        updateClearButton();
        emit clearButtonVisibleChanged(visible);
    }
}

int FluentTextInput::maxLength() const { return m_lineEdit->maxLength(); }

void FluentTextInput::setMaxLength(int length) {
    if (m_lineEdit->maxLength() != length) {
        m_lineEdit->setMaxLength(length);
        emit maxLengthChanged(length);
    }
}

bool FluentTextInput::isReadOnly() const { return m_lineEdit->isReadOnly(); }

void FluentTextInput::setReadOnly(bool readOnly) {
    if (m_lineEdit->isReadOnly() != readOnly) {
        m_lineEdit->setReadOnly(readOnly);
        if (readOnly) {
            setInputState(FluentTextInputState::Disabled);
        }
        emit readOnlyChanged(readOnly);
    }
}

bool FluentTextInput::isValid() const { return m_isValid; }

void FluentTextInput::validate() {
    bool wasValid = m_isValid;
    m_isValid = true;

    const QString currentText = text();

    // Check required validation
    if (m_required && !validateRequired(currentText)) {
        m_isValid = false;
        setErrorText(tr("This field is required"));
        return;
    }

    // Skip other validations if text is empty and not required
    if (currentText.isEmpty()) {
        clearValidation();
        return;
    }

    // Type-specific validation and any explicit validationType override
    switch (m_inputType) {
        case FluentTextInputType::Email:
            if (!validateEmail(currentText)) {
                m_isValid = false;
                setErrorText(tr("Please enter a valid email address"));
            }
            break;
        case FluentTextInputType::Url:
            if (!validateUrl(currentText)) {
                m_isValid = false;
                setErrorText(tr("Please enter a valid URL"));
            }
            break;
        case FluentTextInputType::Number:
            if (!validateNumber(currentText)) {
                m_isValid = false;
                setErrorText(tr("Please enter a valid number"));
            }
            break;
        default:
            break;
    }

    // Optional explicit validation type
    switch (m_validationType) {
        case FluentTextInput::FluentTextInputValidation::Email:
            if (!validateEmail(currentText)) {
                m_isValid = false;
                setErrorText(tr("Please enter a valid email address"));
            }
            break;
        case FluentTextInput::FluentTextInputValidation::URL:
            if (!validateUrl(currentText)) {
                m_isValid = false;
                setErrorText(tr("Please enter a valid URL"));
            }
            break;
        case FluentTextInput::FluentTextInputValidation::Number:
            if (!validateNumber(currentText)) {
                m_isValid = false;
                setErrorText(tr("Please enter a valid number"));
            }
            break;
        case FluentTextInput::FluentTextInputValidation::Custom:
            if (m_customValidator) {
                if (!m_customValidator(currentText)) {
                    m_isValid = false;
                    setErrorText(tr("Invalid value"));
                }
            }
            break;
    }

    if (m_isValid) {
        clearValidation();
    }

    if (wasValid != m_isValid) {
        emit validationChanged(m_isValid);
    }
}

void FluentTextInput::clearValidation() {
    if (!errorText().isEmpty()) {
        setErrorText("");
        setInputState(FluentTextInputState::Normal);
    }
}

// Focus management
void FluentTextInput::setFocus() { m_lineEdit->setFocus(); }

void FluentTextInput::clearFocus() { m_lineEdit->clearFocus(); }

// Selection
void FluentTextInput::selectAll() { m_lineEdit->selectAll(); }

void FluentTextInput::selectText(int start, int length) {
    m_lineEdit->setSelection(start, length);
}

QString FluentTextInput::selectedText() const {
    return m_lineEdit->selectedText();
}

// Slots
void FluentTextInput::clear() {
    m_lineEdit->clear();
    updateClearButton();
}

void FluentTextInput::undo() { m_lineEdit->undo(); }

void FluentTextInput::redo() { m_lineEdit->redo(); }

void FluentTextInput::copy() { m_lineEdit->copy(); }

void FluentTextInput::cut() { m_lineEdit->cut(); }

void FluentTextInput::paste() { m_lineEdit->paste(); }

// Event handlers
void FluentTextInput::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto& theme = Styling::FluentTheme::instance();
    const QRect inputRect = m_lineEdit->geometry().adjusted(-8, -4, 8, 4);

    // Draw background
    QColor backgroundColor;
    QColor borderColor;
    int borderWidth = 1;

    switch (m_inputState) {
        case FluentTextInputState::Normal:
            backgroundColor = theme.color("controlFillDefault");
            borderColor = theme.color("controlStrokeDefault");
            break;
        case FluentTextInputState::Focused:
            backgroundColor = theme.color("controlFillInputActive");
            borderColor = theme.color("accent");
            borderWidth = 2;
            break;
        case FluentTextInputState::Error:
            backgroundColor = theme.color("controlFillDefault");
            borderColor = theme.color("error");
            borderWidth = 2;
            break;
        case FluentTextInputState::Success:
            backgroundColor = theme.color("controlFillDefault");
            borderColor = theme.color("success");
            break;
        case FluentTextInputState::Disabled:
            backgroundColor = theme.color("controlFillDisabled");
            borderColor = theme.color("controlStrokeDisabled");
            break;
    }

    // Draw background with border radius
    const int radius = theme.borderRadius("medium");
    QPainterPath path;
    path.addRoundedRect(inputRect, radius, radius);

    painter.fillPath(path, backgroundColor);

    // Draw border
    QPen borderPen(borderColor, borderWidth);
    painter.setPen(borderPen);
    painter.drawPath(path);
}

void FluentTextInput::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);
    updateGeometry();
}

void FluentTextInput::focusInEvent(QFocusEvent* event) {
    FluentComponent::focusInEvent(event);
    m_lineEdit->setFocus();
    setInputState(FluentTextInputState::Focused);
    animateFocusChange(true);
}

void FluentTextInput::focusOutEvent(QFocusEvent* event) {
    FluentComponent::focusOutEvent(event);
    if (m_inputState == FluentTextInputState::Focused) {
        setInputState(FluentTextInputState::Normal);
    }
    animateFocusChange(false);

    // Trigger validation on focus out
    if (!text().isEmpty() || m_required) {
        m_validationTimer->start();
    }
}

void FluentTextInput::enterEvent(QEnterEvent* event) {
    FluentComponent::enterEvent(event);
    if (m_inputState == FluentTextInputState::Normal) {
        setState(Core::FluentState::Hovered);
    }
}

void FluentTextInput::leaveEvent(QEvent* event) {
    FluentComponent::leaveEvent(event);
    if (state() == Core::FluentState::Hovered) {
        setState(Core::FluentState::Normal);
    }
}

void FluentTextInput::changeEvent(QEvent* event) {
    FluentComponent::changeEvent(event);

    if (event->type() == QEvent::EnabledChange) {
        setInputState(isEnabled() ? FluentTextInputState::Normal
                                  : FluentTextInputState::Disabled);
    }
}

// Private slots
void FluentTextInput::onTextChanged(const QString& text) {
    updateClearButton();
    emit textChanged(text);

    // Clear validation errors when user starts typing
    if (m_inputState == FluentTextInputState::Error) {
        clearValidation();
    }

    // Start validation timer for real-time validation
    if (!text.isEmpty()) {
        m_validationTimer->start();
    }
}

void FluentTextInput::onTextEdited(const QString& text) {
    emit textEdited(text);
}

void FluentTextInput::onReturnPressed() {
    validate();
    emit returnPressed();
}

void FluentTextInput::onEditingFinished() {
    validate();
    emit editingFinished();
}

void FluentTextInput::onClearButtonClicked() {
    clear();
    m_lineEdit->setFocus();
}

void FluentTextInput::updateAnimations() {
    updateColors();
    updateFonts();
    update();
}

// Private helper methods
void FluentTextInput::updateInputType() {
    switch (m_inputType) {
        case FluentTextInputType::Password:
            m_lineEdit->setEchoMode(QLineEdit::Password);
            break;
        case FluentTextInputType::Email:
            m_lineEdit->setEchoMode(QLineEdit::Normal);
            m_lineEdit->setInputMethodHints(Qt::ImhEmailCharactersOnly);
            break;
        case FluentTextInputType::Number:
            m_lineEdit->setEchoMode(QLineEdit::Normal);
            m_lineEdit->setInputMethodHints(Qt::ImhDigitsOnly);
            break;
        case FluentTextInputType::Url:
            m_lineEdit->setEchoMode(QLineEdit::Normal);
            m_lineEdit->setInputMethodHints(Qt::ImhUrlCharactersOnly);
            break;
        case FluentTextInputType::Tel:
            m_lineEdit->setEchoMode(QLineEdit::Normal);
            m_lineEdit->setInputMethodHints(Qt::ImhDialableCharactersOnly);
            break;
        default:
            m_lineEdit->setEchoMode(QLineEdit::Normal);
            m_lineEdit->setInputMethodHints(Qt::ImhNone);
            break;
    }
}

void FluentTextInput::updateInputState() {
    const auto& theme = Styling::FluentTheme::instance();

    switch (m_inputState) {
        case FluentTextInputState::Disabled:
            m_lineEdit->setEnabled(false);
            // Apply disabled styling from theme
            Q_UNUSED(theme);  // Theme styling would be applied here
            break;
        default:
            m_lineEdit->setEnabled(true);
            // Apply normal styling from theme
            break;
    }

    updateColors();
    update();
}

void FluentTextInput::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    // Update label color
    QPalette labelPalette = m_label->palette();
    labelPalette.setColor(QPalette::WindowText, theme.color("textPrimary"));
    m_label->setPalette(labelPalette);

    // Update line edit colors
    QPalette editPalette = m_lineEdit->palette();
    editPalette.setColor(QPalette::Text, theme.color("textPrimary"));
    editPalette.setColor(QPalette::PlaceholderText,
                         theme.color("textSecondary"));
    editPalette.setColor(QPalette::Base, Qt::transparent);
    m_lineEdit->setPalette(editPalette);

    // Update helper text color
    QPalette helperPalette = m_helperLabel->palette();
    helperPalette.setColor(QPalette::WindowText, theme.color("textSecondary"));
    m_helperLabel->setPalette(helperPalette);

    // Update error text color
    QPalette errorPalette = m_errorLabel->palette();
    errorPalette.setColor(QPalette::WindowText, theme.color("error"));
    m_errorLabel->setPalette(errorPalette);

    // Update clear button style
    const QString clearButtonStyle =
        QString(
            "QPushButton {"
            "    background: transparent;"
            "    border: none;"
            "    color: %1;"
            "    font-size: 16px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background: %2;"
            "    border-radius: 12px;"
            "}"
            "QPushButton:pressed {"
            "    background: %3;"
            "}")
            .arg(theme.color("textSecondary").name(),
                 theme.color("controlFillSecondary").name(),
                 theme.color("controlFillTertiary").name());

    m_clearButton->setStyleSheet(clearButtonStyle);
}

void FluentTextInput::updateFonts() {
    const auto& theme = Styling::FluentTheme::instance();

    m_label->setFont(theme.captionFont());
    m_lineEdit->setFont(theme.bodyFont());
    m_helperLabel->setFont(theme.captionFont());
    m_errorLabel->setFont(theme.captionFont());
}

void FluentTextInput::updateGeometry() {
    // This method can be used to adjust layout based on content
    // For now, the layout handles this automatically
}

void FluentTextInput::updateClearButton() {
    const bool shouldShow =
        m_clearButtonVisible && !text().isEmpty() && !isReadOnly();
    m_clearButton->setVisible(shouldShow);
}

void FluentTextInput::animateStateChange() {
    if (m_stateAnimation->state() == QAbstractAnimation::Running) {
        m_stateAnimation->stop();
    }

    m_stateAnimation->setStartValue(geometry());
    m_stateAnimation->setEndValue(geometry());
    m_stateAnimation->start();
}

void FluentTextInput::animateFocusChange(bool focused) {
    Q_UNUSED(focused)

    if (m_focusAnimation->state() == QAbstractAnimation::Running) {
        m_focusAnimation->stop();
    }

    m_focusAnimation->setStartValue(geometry());
    m_focusAnimation->setEndValue(geometry());
    m_focusAnimation->start();
}

void FluentTextInput::animateValidationChange() {
    if (m_validationAnimation->state() == QAbstractAnimation::Running) {
        m_validationAnimation->stop();
    }

    const qreal targetOpacity = errorText().isEmpty() ? 0.0 : 1.0;
    m_validationAnimation->setStartValue(m_errorOpacityEffect->opacity());
    m_validationAnimation->setEndValue(targetOpacity);
    m_validationAnimation->start();
}

// Validation helpers
bool FluentTextInput::validateEmail(const QString& text) const {
    const QRegularExpression emailRegex(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return emailRegex.match(text).hasMatch();
}

bool FluentTextInput::validateUrl(const QString& text) const {
    const QUrl url(text);
    return url.isValid() && !url.scheme().isEmpty();
}

bool FluentTextInput::validateNumber(const QString& text) const {
    bool ok;
    text.toDouble(&ok);
    return ok;
}

bool FluentTextInput::validateRequired(const QString& text) const {
    return !text.trimmed().isEmpty();
}

}  // namespace FluentQt::Components
