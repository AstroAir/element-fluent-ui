// src/Components/FluentContentDialog.cpp
#include "FluentQt/Components/FluentContentDialog.h"
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QScreen>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentContentDialog::FluentContentDialog(QWidget* parent)
    : QDialog(parent),
      m_showAnimation(
          std::make_unique<QPropertyAnimation>(this, "windowOpacity")),
      m_hideAnimation(
          std::make_unique<QPropertyAnimation>(this, "windowOpacity")) {
    setupUI();
    setupAnimations();
    updateTheme();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentContentDialog::updateTheme);
}

void FluentContentDialog::setupUI() {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);

    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(24, 24, 24, 24);
    m_mainLayout->setSpacing(16);

    // Title
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("titleLabel");
    m_mainLayout->addWidget(m_titleLabel);

    // Content
    m_contentLabel = new QLabel(this);
    m_contentLabel->setObjectName("contentLabel");
    m_contentLabel->setWordWrap(true);
    m_mainLayout->addWidget(m_contentLabel);

    // Content widget placeholder
    m_contentWidget = nullptr;

    // Button layout
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(8);
    m_buttonLayout->addStretch();

    // Buttons
    m_primaryButton = new QPushButton(this);
    m_primaryButton->setObjectName("primaryButton");
    connect(m_primaryButton, &QPushButton::clicked, this,
            &FluentContentDialog::onPrimaryButtonClicked);

    m_secondaryButton = new QPushButton(this);
    m_secondaryButton->setObjectName("secondaryButton");
    connect(m_secondaryButton, &QPushButton::clicked, this,
            &FluentContentDialog::onSecondaryButtonClicked);

    m_closeButton = new QPushButton(this);
    m_closeButton->setObjectName("closeButton");
    connect(m_closeButton, &QPushButton::clicked, this,
            &FluentContentDialog::onCloseButtonClicked);

    m_buttonLayout->addWidget(m_primaryButton);
    m_buttonLayout->addWidget(m_secondaryButton);
    m_buttonLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_buttonLayout);

    // Set minimum size
    setMinimumSize(320, 200);
    resize(400, 250);
}

void FluentContentDialog::setupAnimations() {
    m_showAnimation->setDuration(250);
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic);

    m_hideAnimation->setDuration(200);
    m_hideAnimation->setEasingCurve(QEasingCurve::InCubic);
}

void FluentContentDialog::updateTheme() {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    QString styleSheet = QString(R"(
        FluentContentDialog {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
        }
        QLabel#titleLabel {
            font-size: 20px;
            font-weight: 600;
            color: %3;
            margin-bottom: 8px;
        }
        QLabel#contentLabel {
            font-size: 14px;
            color: %4;
            line-height: 1.4;
        }
        QPushButton#primaryButton {
            background-color: %5;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: 600;
            min-width: 80px;
        }
        QPushButton#primaryButton:hover {
            background-color: %6;
        }
        QPushButton#secondaryButton, QPushButton#closeButton {
            background-color: transparent;
            color: %3;
            border: 1px solid %2;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
            min-width: 80px;
        }
        QPushButton#secondaryButton:hover, QPushButton#closeButton:hover {
            background-color: %7;
        }
    )")
                             .arg(palette.neutralLightest.name())
                             .arg(palette.neutralQuaternary.name())
                             .arg(palette.neutralPrimary.name())
                             .arg(palette.neutralSecondary.name())
                             .arg(palette.accent.name())
                             .arg(palette.accent.darker(110).name())
                             .arg(palette.neutralLighter.name());

    setStyleSheet(styleSheet);
}

// Property setters
void FluentContentDialog::setTitle(const QString& title) {
    m_titleLabel->setText(title);
    m_titleLabel->setVisible(!title.isEmpty());
}

QString FluentContentDialog::title() const { return m_titleLabel->text(); }

void FluentContentDialog::setContent(const QString& content) {
    m_contentLabel->setText(content);
    m_contentLabel->setVisible(!content.isEmpty());
}

QString FluentContentDialog::content() const { return m_contentLabel->text(); }

void FluentContentDialog::setPrimaryButtonText(const QString& text) {
    m_primaryButton->setText(text);
    m_primaryButton->setVisible(!text.isEmpty());
}

void FluentContentDialog::setSecondaryButtonText(const QString& text) {
    m_secondaryButton->setText(text);
    m_secondaryButton->setVisible(!text.isEmpty());
}

void FluentContentDialog::setCloseButtonText(const QString& text) {
    m_closeButton->setText(text);
    m_closeButton->setVisible(!text.isEmpty());
}

// Button click handlers
void FluentContentDialog::onPrimaryButtonClicked() {
    m_result = FluentDialogResult::Primary;
    accept();
}

void FluentContentDialog::onSecondaryButtonClicked() {
    m_result = FluentDialogResult::Secondary;
    accept();
}

void FluentContentDialog::onCloseButtonClicked() {
    m_result = FluentDialogResult::Close;
    reject();
}

// Static convenience methods
FluentDialogResult FluentContentDialog::showMessageDialog(
    QWidget* parent, const QString& title, const QString& message,
    const QString& primaryButton, const QString& secondaryButton,
    const QString& closeButton) {
    FluentContentDialog dialog(parent);
    dialog.setTitle(title);
    dialog.setContent(message);
    dialog.setPrimaryButtonText(primaryButton);
    dialog.setSecondaryButtonText(secondaryButton);
    dialog.setCloseButtonText(closeButton);

    dialog.exec();
    return dialog.result();
}

// Event handlers
void FluentContentDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    animateShow();
}

void FluentContentDialog::animateShow() {
    setWindowOpacity(0.0);
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);
    m_showAnimation->start();
}

void FluentContentDialog::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Draw background with shadow
    QRect bgRect = rect().adjusted(4, 4, -4, -4);
    painter.setBrush(QBrush(palette.neutralLightest));
    painter.setPen(QPen(palette.neutralQuaternary, 1));
    painter.drawRoundedRect(bgRect, 8, 8);

    QDialog::paintEvent(event);
}

}  // namespace FluentQt::Components
