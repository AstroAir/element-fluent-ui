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

    const auto& theme = Styling::FluentTheme::instance();

    // Create main layout with Fluent UI spacing
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(theme.spacing("s"));

    // Create label
    m_label = new QLabel(this);
    m_label->setObjectName("FluentTextInput_Label");
    m_label->hide();  // Initially hidden
    m_mainLayout->addWidget(m_label);

    // Create input layout with proper padding
    m_inputLayout = new QHBoxLayout();
    const QMargins inputPadding = theme.padding("medium");
    m_inputLayout->setContentsMargins(inputPadding.left(), inputPadding.top(),
                                      inputPadding.right(),
                                      inputPadding.bottom());
    m_inputLayout->setSpacing(theme.spacing("xs"));

    // Create prefix label
    m_prefixLabel = new QLabel(this);
    m_prefixLabel->setObjectName("FluentTextInput_PrefixLabel");
    m_prefixLabel->hide();
    m_inputLayout->addWidget(m_prefixLabel);

    // Create line edit
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setObjectName("FluentTextInput_LineEdit");
    m_lineEdit->setFrame(false);
    m_inputLayout->addWidget(m_lineEdit, 1);  // Give it stretch

    // Create suffix label
    m_suffixLabel = new QLabel(this);
    m_suffixLabel->setObjectName("FluentTextInput_SuffixLabel");
    m_suffixLabel->hide();
    m_inputLayout->addWidget(m_suffixLabel);

    // Create password toggle button
    m_passwordToggleButton = new QPushButton(this);
    m_passwordToggleButton->setObjectName(
        "FluentTextInput_PasswordToggleButton");
    m_passwordToggleButton->setText("👁");
    const QSize iconSize = theme.iconSize("medium");
    m_passwordToggleButton->setFixedSize(iconSize.width() + 8,
                                         iconSize.height() + 8);
    m_passwordToggleButton->setFlat(true);
    m_passwordToggleButton->hide();
    m_inputLayout->addWidget(m_passwordToggleButton);

    // Create clear button with theme-based sizing
    m_clearButton = new QPushButton(this);
    m_clearButton->setObjectName("FluentTextInput_ClearButton");
    m_clearButton->setText("×");
    m_clearButton->setFixedSize(iconSize.width() + 8, iconSize.height() + 8);
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

    // Create character counter label
    m_characterCounterLabel = new QLabel(this);
    m_characterCounterLabel->setObjectName(
        "FluentTextInput_CharacterCounterLabel");
    m_characterCounterLabel->setAlignment(Qt::AlignRight);
    m_characterCounterLabel->hide();
    m_mainLayout->addWidget(m_characterCounterLabel);

    // Set minimum height using theme tokens
    const int componentHeight = theme.componentHeight("medium");
    setMinimumHeight(componentHeight);

    // Setup accessibility
    setupAccessibility();
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

    // Connect password toggle button
    connect(m_passwordToggleButton, &QPushButton::clicked, this, [this]() {
        bool isVisible = m_lineEdit->echoMode() == QLineEdit::Normal;
        m_lineEdit->setEchoMode(isVisible ? QLineEdit::Password
                                          : QLineEdit::Normal);
        m_passwordToggleButton->setText(isVisible ? "👁" : "🙈");
    });

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

// Enhanced Fluent UI property implementations
bool FluentTextInput::isCharacterCounterVisible() const {
    return m_characterCounterVisible;
}

void FluentTextInput::setCharacterCounterVisible(bool visible) {
    if (m_characterCounterVisible != visible) {
        m_characterCounterVisible = visible;
        updateCharacterCounter();
        emit characterCounterVisibleChanged(visible);
    }
}

QString FluentTextInput::prefixText() const { return m_prefixText; }

void FluentTextInput::setPrefixText(const QString& text) {
    if (m_prefixText != text) {
        m_prefixText = text;
        updatePrefixSuffix();
        emit prefixTextChanged(text);
    }
}

QString FluentTextInput::suffixText() const { return m_suffixText; }

void FluentTextInput::setSuffixText(const QString& text) {
    if (m_suffixText != text) {
        m_suffixText = text;
        updatePrefixSuffix();
        emit suffixTextChanged(text);
    }
}

bool FluentTextInput::showPasswordToggle() const {
    return m_showPasswordToggle;
}

void FluentTextInput::setShowPasswordToggle(bool show) {
    if (m_showPasswordToggle != show) {
        m_showPasswordToggle = show;
        updatePasswordToggleButton();
        emit showPasswordToggleChanged(show);
    }
}

QString FluentTextInput::inputMask() const { return m_inputMask; }

void FluentTextInput::setInputMask(const QString& mask) {
    if (m_inputMask != mask) {
        m_inputMask = mask;
        m_lineEdit->setInputMask(mask);
        emit inputMaskChanged(mask);
    }
}

void FluentTextInput::validate() {
    bool wasValid = m_isValid;
    m_isValid = true;
    bool hasWarning = false;

    const QString currentText = text();

    // Check required validation
    if (m_required && !validateRequired(currentText)) {
        m_isValid = false;
        const QString errorMsg = m_customErrorMessage.isEmpty()
                                     ? tr("This field is required")
                                     : m_customErrorMessage;
        setErrorText(errorMsg);
        return;
    }

    // Skip other validations if text is empty and not required
    if (currentText.isEmpty()) {
        clearValidation();
        return;
    }

    // Check character count for warning
    if (maxLength() > 0 && currentText.length() > maxLength() * 0.9) {
        hasWarning = true;
    }

    // Run custom validation rules first
    for (const auto& rule : m_customValidationRules) {
        if (!rule.second.first(currentText)) {
            m_isValid = false;
            setErrorText(rule.second.second);
            return;
        }
    }

    // Type-specific validation
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
                    const QString errorMsg = m_customErrorMessage.isEmpty()
                                                 ? tr("Invalid value")
                                                 : m_customErrorMessage;
                    setErrorText(errorMsg);
                }
            }
            break;
    }

    if (m_isValid) {
        clearValidation();
        // Set warning state if applicable
        if (hasWarning) {
            setInputState(FluentTextInputState::Warning);
        }
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

// Enhanced validation methods
void FluentTextInput::setValidationMode(FluentValidationMode mode) {
    m_validationMode = mode;
}

void FluentTextInput::setCustomErrorMessage(const QString& message) {
    m_customErrorMessage = message;
}

void FluentTextInput::addCustomValidationRule(
    const QString& name, std::function<bool(const QString&)> rule,
    const QString& errorMessage) {
    m_customValidationRules[name] = std::make_pair(rule, errorMessage);
}

void FluentTextInput::clearCustomValidationRules() {
    m_customValidationRules.clear();
}

// Accessibility methods
void FluentTextInput::setAccessibleName(const QString& name) {
    m_accessibleName = name;
    setAccessibleName(name);
    setupAccessibility();
}

void FluentTextInput::setAccessibleDescription(const QString& description) {
    m_accessibleDescription = description;
    setAccessibleDescription(description);
    setupAccessibility();
}

QString FluentTextInput::accessibleName() const { return m_accessibleName; }

QString FluentTextInput::accessibleDescription() const {
    return m_accessibleDescription;
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
            borderWidth = theme.strokeWidth("thick");
            break;
        case FluentTextInputState::Error:
            backgroundColor = theme.color("controlFillDefault");
            borderColor = theme.color("error");
            borderWidth = theme.strokeWidth("thick");
            break;
        case FluentTextInputState::Warning:
            backgroundColor = theme.color("controlFillDefault");
            borderColor = theme.color("warning");
            borderWidth = theme.strokeWidth("thick");
            break;
        case FluentTextInputState::Success:
            backgroundColor = theme.color("controlFillDefault");
            borderColor = theme.color("success");
            borderWidth = theme.strokeWidth("medium");
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

    // Trigger validation based on validation mode
    if (m_validationMode == FluentValidationMode::OnBlur &&
        (!text().isEmpty() || m_required)) {
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
    updatePasswordToggleButton();
    updateCharacterCounter();
    emit textChanged(text);

    // Clear validation errors when user starts typing
    if (m_inputState == FluentTextInputState::Error ||
        m_inputState == FluentTextInputState::Warning) {
        clearValidation();
    }

    // Start validation timer based on validation mode
    if (m_validationMode == FluentValidationMode::RealTime && !text.isEmpty()) {
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

void FluentTextInput::updatePasswordToggleButton() {
    const bool shouldShow = m_showPasswordToggle &&
                            m_inputType == FluentTextInputType::Password &&
                            !isReadOnly();
    m_passwordToggleButton->setVisible(shouldShow);
}

void FluentTextInput::updateCharacterCounter() {
    if (m_characterCounterVisible && maxLength() > 0) {
        const int currentLength = text().length();
        const int maxLen = maxLength();
        const QString counterText =
            QString("%1/%2").arg(currentLength).arg(maxLen);
        m_characterCounterLabel->setText(counterText);
        m_characterCounterLabel->setVisible(true);

        // Change color based on character count
        const auto& theme = Styling::FluentTheme::instance();
        QPalette palette = m_characterCounterLabel->palette();
        if (currentLength > maxLen * 0.9) {
            palette.setColor(QPalette::WindowText, theme.color("warning"));
        } else if (currentLength == maxLen) {
            palette.setColor(QPalette::WindowText, theme.color("error"));
        } else {
            palette.setColor(QPalette::WindowText,
                             theme.color("textSecondary"));
        }
        m_characterCounterLabel->setPalette(palette);
    } else {
        m_characterCounterLabel->setVisible(false);
    }
}

void FluentTextInput::updatePrefixSuffix() {
    m_prefixLabel->setText(m_prefixText);
    m_prefixLabel->setVisible(!m_prefixText.isEmpty());

    m_suffixLabel->setText(m_suffixText);
    m_suffixLabel->setVisible(!m_suffixText.isEmpty());
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

void FluentTextInput::setupAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    // Set up accessibility properties for the main component
    setAccessibleName(m_accessibleName.isEmpty() ? "Text Input"
                                                 : m_accessibleName);
    setAccessibleDescription(m_accessibleDescription.isEmpty()
                                 ? "Enter text in this input field"
                                 : m_accessibleDescription);

    // Set up line edit accessibility
    m_lineEdit->setAccessibleName(accessibleName());
    m_lineEdit->setAccessibleDescription(accessibleDescription());

    // Set up button accessibility
    m_clearButton->setAccessibleName("Clear text");
    m_clearButton->setAccessibleDescription(
        "Clear all text from the input field");

    m_passwordToggleButton->setAccessibleName("Toggle password visibility");
    m_passwordToggleButton->setAccessibleDescription(
        "Show or hide password characters");

    // Set up label accessibility
    if (m_label && !m_label->text().isEmpty()) {
        m_lineEdit->setAccessibleName(m_label->text());
    }

    // Set ARIA-like properties
    setProperty("role", "textbox");
    setProperty("aria-required", m_required);
    setProperty("aria-invalid", !m_isValid);
    setProperty("aria-multiline", false);

    // Set up keyboard navigation
    setFocusPolicy(Qt::StrongFocus);
    setTabOrder(this, m_lineEdit);
    if (m_clearButton->isVisible()) {
        setTabOrder(m_lineEdit, m_clearButton);
    }
    if (m_passwordToggleButton->isVisible()) {
        setTabOrder(m_lineEdit, m_passwordToggleButton);
    }
#endif
}

}  // namespace FluentQt::Components
