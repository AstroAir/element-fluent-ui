// examples/AdvancedAnimationExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QProgressBar>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>

#include "FluentQt/Animation/FluentAdvancedAnimator.h"
#include "FluentQt/Animation/FluentAnimationPerformanceManager.h"
#include "FluentQt/Animation/FluentGestureAnimations.h"
#include "FluentQt/Animation/FluentAdvancedEasing.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

using namespace FluentQt;

class AnimationDemoCard : public QWidget {
    Q_OBJECT

public:
    explicit AnimationDemoCard(const QString& title, const QString& description, QWidget* parent = nullptr)
        : QWidget(parent), m_title(title), m_description(description) {
        setupUI();
        setupAnimations();
        setupGestures();
    }

public slots:
    void startAnimation() {
        if (m_currentAnimation && m_currentAnimation->state() == QAbstractAnimation::Running) {
            return;
        }
        
        // Create entrance animation
        auto* group = new QParallelAnimationGroup(this);
        
        // Scale animation
        auto* scaleAnimation = new QPropertyAnimation(this, "geometry");
        scaleAnimation->setDuration(500);
        scaleAnimation->setStartValue(QRect(geometry().x(), geometry().y(), 0, 0));
        scaleAnimation->setEndValue(geometry());
        scaleAnimation->setEasingCurve(QEasingCurve::OutBack);
        
        // Opacity animation
        auto* opacityEffect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(opacityEffect);
        auto* opacityAnimation = new QPropertyAnimation(opacityEffect, "opacity");
        opacityAnimation->setDuration(500);
        opacityAnimation->setStartValue(0.0);
        opacityAnimation->setEndValue(1.0);
        opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
        
        group->addAnimation(scaleAnimation);
        group->addAnimation(opacityAnimation);
        
        m_currentAnimation = group;
        group->start(QAbstractAnimation::DeleteWhenStopped);
        
        // Add subtle hover effect
        startHoverEffect();
    }
    
    void setAnimationStyle(Animation::FluentCulturalAnimationStyle style) {
        m_animationStyle = style;
        updateAnimationParameters();
    }
    
    void setAccessibilityMode(Animation::FluentAccessibilityAnimationMode mode) {
        m_accessibilityMode = mode;
        updateAnimationParameters();
    }

protected:
    void enterEvent(QEnterEvent* event) override {
        QWidget::enterEvent(event);
        if (m_hoverAnimation && m_hoverAnimation->state() == QAbstractAnimation::Running) {
            m_hoverAnimation->stop();
        }
        
        // Hover in animation
        m_hoverAnimation = new QPropertyAnimation(this, "styleSheet");
        m_hoverAnimation->setDuration(200);
        m_hoverAnimation->setStartValue(styleSheet());
        m_hoverAnimation->setEndValue(styleSheet() + " QWidget { background-color: rgba(255, 255, 255, 0.1); }");
        m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    void leaveEvent(QEvent* event) override {
        QWidget::leaveEvent(event);
        if (m_hoverAnimation && m_hoverAnimation->state() == QAbstractAnimation::Running) {
            m_hoverAnimation->stop();
        }
        
        // Hover out animation
        m_hoverAnimation = new QPropertyAnimation(this, "styleSheet");
        m_hoverAnimation->setDuration(200);
        m_hoverAnimation->setStartValue(styleSheet());
        m_hoverAnimation->setEndValue(m_originalStyleSheet);
        m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    void mousePressEvent(QMouseEvent* event) override {
        QWidget::mousePressEvent(event);
        
        // Press animation
        auto* pressAnimation = new QPropertyAnimation(this, "geometry");
        pressAnimation->setDuration(100);
        QRect currentGeometry = geometry();
        QRect pressedGeometry = currentGeometry.adjusted(2, 2, -2, -2);
        pressAnimation->setStartValue(currentGeometry);
        pressAnimation->setEndValue(pressedGeometry);
        pressAnimation->setEasingCurve(QEasingCurve::OutCubic);
        pressAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        
        // Ripple effect
        createRippleEffect(event->position().toPoint());
    }
    
    void mouseReleaseEvent(QMouseEvent* event) override {
        QWidget::mouseReleaseEvent(event);
        
        // Release animation
        auto* releaseAnimation = new QPropertyAnimation(this, "geometry");
        releaseAnimation->setDuration(100);
        QRect currentGeometry = geometry();
        QRect originalGeometry = currentGeometry.adjusted(-2, -2, 2, 2);
        releaseAnimation->setStartValue(currentGeometry);
        releaseAnimation->setEndValue(originalGeometry);
        releaseAnimation->setEasingCurve(QEasingCurve::OutBack);
        releaseAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    void setupUI() {
        setFixedSize(280, 180);
        setStyleSheet(
            "QWidget { "
            "  background-color: white; "
            "  border: 1px solid #e0e0e0; "
            "  border-radius: 8px; "
            "  padding: 16px; "
            "}"
        );
        m_originalStyleSheet = styleSheet();
        
        auto* layout = new QVBoxLayout(this);
        
        // Title
        m_titleLabel = new QLabel(m_title, this);
        m_titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #333;");
        m_titleLabel->setWordWrap(true);
        
        // Description
        m_descriptionLabel = new QLabel(m_description, this);
        m_descriptionLabel->setStyleSheet("color: #666; font-size: 14px;");
        m_descriptionLabel->setWordWrap(true);
        
        // Action button
        m_actionButton = new QPushButton("Animate", this);
        m_actionButton->setStyleSheet(
            "QPushButton { "
            "  background-color: #0078d4; "
            "  color: white; "
            "  border: none; "
            "  border-radius: 4px; "
            "  padding: 8px 16px; "
            "  font-weight: bold; "
            "} "
            "QPushButton:hover { "
            "  background-color: #106ebe; "
            "} "
            "QPushButton:pressed { "
            "  background-color: #005a9e; "
            "}"
        );
        
        connect(m_actionButton, &QPushButton::clicked, this, &AnimationDemoCard::startAnimation);
        
        layout->addWidget(m_titleLabel);
        layout->addWidget(m_descriptionLabel);
        layout->addStretch();
        layout->addWidget(m_actionButton);
    }
    
    void setupAnimations() {
        // Initialize animation parameters based on current style and accessibility mode
        updateAnimationParameters();
    }
    
    void setupGestures() {
        // Enable gesture animations
        m_gestureController = new Animation::FluentGestureAnimationController(this, this);
        m_gestureController->enableGesture(Animation::FluentGestureType::Tap, true);
        m_gestureController->enableGesture(Animation::FluentGestureType::Hover, true);
        m_gestureController->enableGesture(Animation::FluentGestureType::Pan, true);
        
        // Configure gesture feedback
        m_gestureController->setGestureFeedback(Animation::FluentGestureType::Tap, Animation::FluentGestureFeedback::Visual);
        m_gestureController->setGestureFeedback(Animation::FluentGestureType::Hover, Animation::FluentGestureFeedback::Visual);
    }
    
    void updateAnimationParameters() {
        // Adapt animation duration based on cultural style
        switch (m_animationStyle) {
            case Animation::FluentCulturalAnimationStyle::Western:
                m_animationDuration = 200;
                break;
            case Animation::FluentCulturalAnimationStyle::Eastern:
                m_animationDuration = 400;
                break;
            case Animation::FluentCulturalAnimationStyle::Minimal:
                m_animationDuration = 150;
                break;
            case Animation::FluentCulturalAnimationStyle::Expressive:
                m_animationDuration = 500;
                break;
            default:
                m_animationDuration = 300;
                break;
        }
        
        // Adapt for accessibility
        switch (m_accessibilityMode) {
            case Animation::FluentAccessibilityAnimationMode::Reduced:
                m_animationDuration = std::min(m_animationDuration, 200);
                break;
            case Animation::FluentAccessibilityAnimationMode::Essential:
                m_animationDuration = 100;
                break;
            case Animation::FluentAccessibilityAnimationMode::None:
                m_animationDuration = 0;
                break;
            default:
                break;
        }
    }
    
    void startHoverEffect() {
        // Add subtle shadow effect
        auto* shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(20);
        shadowEffect->setColor(QColor(0, 0, 0, 50));
        shadowEffect->setOffset(0, 4);
        setGraphicsEffect(shadowEffect);
        
        // Animate shadow
        auto* shadowAnimation = new QPropertyAnimation(shadowEffect, "blurRadius");
        shadowAnimation->setDuration(200);
        shadowAnimation->setStartValue(0);
        shadowAnimation->setEndValue(20);
        shadowAnimation->setEasingCurve(QEasingCurve::OutCubic);
        shadowAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    void createRippleEffect(const QPoint& center) {
        // Create ripple effect widget
        auto* ripple = new QWidget(this);
        ripple->setFixedSize(0, 0);
        ripple->move(center);
        ripple->setStyleSheet("background-color: rgba(255, 255, 255, 0.3); border-radius: 0px;");
        ripple->show();
        
        // Animate ripple expansion
        auto* group = new QParallelAnimationGroup(this);
        
        auto* sizeAnimation = new QPropertyAnimation(ripple, "size");
        sizeAnimation->setDuration(400);
        sizeAnimation->setStartValue(QSize(0, 0));
        sizeAnimation->setEndValue(QSize(200, 200));
        sizeAnimation->setEasingCurve(QEasingCurve::OutCubic);
        
        auto* posAnimation = new QPropertyAnimation(ripple, "pos");
        posAnimation->setDuration(400);
        posAnimation->setStartValue(center);
        posAnimation->setEndValue(center - QPoint(100, 100));
        posAnimation->setEasingCurve(QEasingCurve::OutCubic);
        
        auto* opacityEffect = new QGraphicsOpacityEffect(ripple);
        ripple->setGraphicsEffect(opacityEffect);
        auto* opacityAnimation = new QPropertyAnimation(opacityEffect, "opacity");
        opacityAnimation->setDuration(400);
        opacityAnimation->setStartValue(0.6);
        opacityAnimation->setEndValue(0.0);
        opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
        
        group->addAnimation(sizeAnimation);
        group->addAnimation(posAnimation);
        group->addAnimation(opacityAnimation);
        
        connect(group, &QParallelAnimationGroup::finished, ripple, &QWidget::deleteLater);
        group->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    QString m_title;
    QString m_description;
    QString m_originalStyleSheet;
    QLabel* m_titleLabel;
    QLabel* m_descriptionLabel;
    QPushButton* m_actionButton;
    
    // Animation properties
    Animation::FluentCulturalAnimationStyle m_animationStyle{Animation::FluentCulturalAnimationStyle::Western};
    Animation::FluentAccessibilityAnimationMode m_accessibilityMode{Animation::FluentAccessibilityAnimationMode::Full};
    int m_animationDuration{300};
    
    // Animation objects
    QAbstractAnimation* m_currentAnimation{nullptr};
    QPropertyAnimation* m_hoverAnimation{nullptr};
    Animation::FluentGestureAnimationController* m_gestureController{nullptr};
};

class PerformanceMonitorWidget : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceMonitorWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        startMonitoring();
    }

private slots:
    void updateMetrics() {
        auto& performanceManager = Animation::FluentAnimationPerformanceManager::instance();
        auto metrics = performanceManager.getCurrentMetrics();
        
        m_fpsLabel->setText(QString("FPS: %1").arg(metrics.averageFPS, 0, 'f', 1));
        m_frameTimeLabel->setText(QString("Frame Time: %1ms").arg(metrics.frameTime, 0, 'f', 2));
        m_activeAnimationsLabel->setText(QString("Active: %1").arg(metrics.activeAnimations));
        m_performanceScoreLabel->setText(QString("Score: %1").arg(metrics.performanceScore, 0, 'f', 1));
        
        // Update progress bars
        m_fpsProgressBar->setValue(static_cast<int>(metrics.averageFPS));
        m_performanceProgressBar->setValue(static_cast<int>(metrics.performanceScore));
        
        // Update recommendations
        auto recommendations = performanceManager.getPerformanceRecommendations();
        m_recommendationsText->setText(recommendations.join("\n"));
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Metrics display
        auto* metricsGroup = new QGroupBox("Performance Metrics", this);
        auto* metricsLayout = new QGridLayout(metricsGroup);
        
        m_fpsLabel = new QLabel("FPS: --", this);
        m_frameTimeLabel = new QLabel("Frame Time: --", this);
        m_activeAnimationsLabel = new QLabel("Active: --", this);
        m_performanceScoreLabel = new QLabel("Score: --", this);
        
        m_fpsProgressBar = new QProgressBar(this);
        m_fpsProgressBar->setRange(0, 120);
        m_performanceProgressBar = new QProgressBar(this);
        m_performanceProgressBar->setRange(0, 100);
        
        metricsLayout->addWidget(m_fpsLabel, 0, 0);
        metricsLayout->addWidget(m_fpsProgressBar, 0, 1);
        metricsLayout->addWidget(m_frameTimeLabel, 1, 0);
        metricsLayout->addWidget(m_activeAnimationsLabel, 2, 0);
        metricsLayout->addWidget(m_performanceScoreLabel, 3, 0);
        metricsLayout->addWidget(m_performanceProgressBar, 3, 1);
        
        // Recommendations
        auto* recommendationsGroup = new QGroupBox("Recommendations", this);
        auto* recommendationsLayout = new QVBoxLayout(recommendationsGroup);
        
        m_recommendationsText = new QTextEdit(this);
        m_recommendationsText->setMaximumHeight(100);
        m_recommendationsText->setReadOnly(true);
        
        recommendationsLayout->addWidget(m_recommendationsText);
        
        layout->addWidget(metricsGroup);
        layout->addWidget(recommendationsGroup);
        layout->addStretch();
    }
    
    void startMonitoring() {
        auto& performanceManager = Animation::FluentAnimationPerformanceManager::instance();
        performanceManager.startPerformanceMonitoring();
        
        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &PerformanceMonitorWidget::updateMetrics);
        timer->start(100); // Update every 100ms
    }

private:
    QLabel* m_fpsLabel;
    QLabel* m_frameTimeLabel;
    QLabel* m_activeAnimationsLabel;
    QLabel* m_performanceScoreLabel;
    QProgressBar* m_fpsProgressBar;
    QProgressBar* m_performanceProgressBar;
    QTextEdit* m_recommendationsText;
};

class AnimationControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit AnimationControlPanel(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();
    }

signals:
    void animationStyleChanged(Animation::FluentCulturalAnimationStyle style);
    void accessibilityModeChanged(Animation::FluentAccessibilityAnimationMode mode);
    void performanceLevelChanged(Animation::FluentAnimationPerformance level);

private slots:
    void onStyleChanged() {
        auto style = static_cast<Animation::FluentCulturalAnimationStyle>(m_styleCombo->currentData().toInt());
        emit animationStyleChanged(style);
    }
    
    void onAccessibilityChanged() {
        auto mode = static_cast<Animation::FluentAccessibilityAnimationMode>(m_accessibilityCombo->currentData().toInt());
        emit accessibilityModeChanged(mode);
    }
    
    void onPerformanceChanged() {
        auto level = static_cast<Animation::FluentAnimationPerformance>(m_performanceCombo->currentData().toInt());
        emit performanceLevelChanged(level);
        
        auto& performanceManager = Animation::FluentAnimationPerformanceManager::instance();
        performanceManager.setTargetPerformance(level);
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Animation style
        auto* styleGroup = new QGroupBox("Animation Style", this);
        auto* styleLayout = new QVBoxLayout(styleGroup);
        
        m_styleCombo = new QComboBox(this);
        m_styleCombo->addItem("Western", static_cast<int>(Animation::FluentCulturalAnimationStyle::Western));
        m_styleCombo->addItem("Eastern", static_cast<int>(Animation::FluentCulturalAnimationStyle::Eastern));
        m_styleCombo->addItem("Minimal", static_cast<int>(Animation::FluentCulturalAnimationStyle::Minimal));
        m_styleCombo->addItem("Expressive", static_cast<int>(Animation::FluentCulturalAnimationStyle::Expressive));
        m_styleCombo->addItem("Formal", static_cast<int>(Animation::FluentCulturalAnimationStyle::Formal));
        m_styleCombo->addItem("Playful", static_cast<int>(Animation::FluentCulturalAnimationStyle::Playful));
        
        styleLayout->addWidget(m_styleCombo);
        
        // Accessibility mode
        auto* accessibilityGroup = new QGroupBox("Accessibility", this);
        auto* accessibilityLayout = new QVBoxLayout(accessibilityGroup);
        
        m_accessibilityCombo = new QComboBox(this);
        m_accessibilityCombo->addItem("Full", static_cast<int>(Animation::FluentAccessibilityAnimationMode::Full));
        m_accessibilityCombo->addItem("Reduced", static_cast<int>(Animation::FluentAccessibilityAnimationMode::Reduced));
        m_accessibilityCombo->addItem("Essential", static_cast<int>(Animation::FluentAccessibilityAnimationMode::Essential));
        m_accessibilityCombo->addItem("None", static_cast<int>(Animation::FluentAccessibilityAnimationMode::None));
        
        accessibilityLayout->addWidget(m_accessibilityCombo);
        
        // Performance level
        auto* performanceGroup = new QGroupBox("Performance", this);
        auto* performanceLayout = new QVBoxLayout(performanceGroup);
        
        m_performanceCombo = new QComboBox(this);
        m_performanceCombo->addItem("High", static_cast<int>(Animation::FluentAnimationPerformance::High));
        m_performanceCombo->addItem("Medium", static_cast<int>(Animation::FluentAnimationPerformance::Medium));
        m_performanceCombo->addItem("Low", static_cast<int>(Animation::FluentAnimationPerformance::Low));
        m_performanceCombo->addItem("Adaptive", static_cast<int>(Animation::FluentAnimationPerformance::Adaptive));
        m_performanceCombo->addItem("Power Saver", static_cast<int>(Animation::FluentAnimationPerformance::PowerSaver));
        
        performanceLayout->addWidget(m_performanceCombo);
        
        layout->addWidget(styleGroup);
        layout->addWidget(accessibilityGroup);
        layout->addWidget(performanceGroup);
        layout->addStretch();
    }
    
    void connectSignals() {
        connect(m_styleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &AnimationControlPanel::onStyleChanged);
        connect(m_accessibilityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &AnimationControlPanel::onAccessibilityChanged);
        connect(m_performanceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &AnimationControlPanel::onPerformanceChanged);
    }

private:
    QComboBox* m_styleCombo;
    QComboBox* m_accessibilityCombo;
    QComboBox* m_performanceCombo;
};

class AdvancedAnimationMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdvancedAnimationMainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        connectSignals();
        createAnimationCards();
    }

private slots:
    void onAnimationStyleChanged(Animation::FluentCulturalAnimationStyle style) {
        for (auto* card : m_animationCards) {
            card->setAnimationStyle(style);
        }
    }
    
    void onAccessibilityModeChanged(Animation::FluentAccessibilityAnimationMode mode) {
        for (auto* card : m_animationCards) {
            card->setAccessibilityMode(mode);
        }
    }

private:
    void setupUI() {
        setWindowTitle("FluentQt Advanced Animation Demo");
        setMinimumSize(1200, 800);
        
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QHBoxLayout(centralWidget);
        
        // Left panel - Controls and monitoring
        auto* leftPanel = new QWidget(this);
        leftPanel->setMaximumWidth(300);
        auto* leftLayout = new QVBoxLayout(leftPanel);
        
        m_controlPanel = new AnimationControlPanel(this);
        m_performanceMonitor = new PerformanceMonitorWidget(this);
        
        leftLayout->addWidget(m_controlPanel);
        leftLayout->addWidget(m_performanceMonitor);
        
        // Right panel - Animation demos
        auto* rightPanel = new QWidget(this);
        auto* rightLayout = new QVBoxLayout(rightPanel);
        
        // Animation cards grid
        m_cardsWidget = new QWidget(this);
        m_cardsLayout = new QGridLayout(m_cardsWidget);
        m_cardsLayout->setSpacing(16);
        
        auto* scrollArea = new QScrollArea(this);
        scrollArea->setWidget(m_cardsWidget);
        scrollArea->setWidgetResizable(true);
        
        rightLayout->addWidget(scrollArea);
        
        mainLayout->addWidget(leftPanel);
        mainLayout->addWidget(rightPanel, 1);
        
        // Status bar
        statusBar()->showMessage("Advanced animation system ready");
    }
    
    void connectSignals() {
        connect(m_controlPanel, &AnimationControlPanel::animationStyleChanged,
                this, &AdvancedAnimationMainWindow::onAnimationStyleChanged);
        connect(m_controlPanel, &AnimationControlPanel::accessibilityModeChanged,
                this, &AdvancedAnimationMainWindow::onAccessibilityModeChanged);
    }
    
    void createAnimationCards() {
        QStringList titles = {
            "Entrance Animation", "Gesture Response", "Physics Simulation",
            "Cultural Adaptation", "Accessibility Mode", "Performance Optimization",
            "Easing Curves", "Color Interpolation", "Transform Animation"
        };
        
        QStringList descriptions = {
            "Smooth entrance with scale and opacity",
            "Interactive gesture-based animations",
            "Spring physics and momentum",
            "Culturally-adapted animation timing",
            "Accessibility-aware motion design",
            "Performance-optimized animations",
            "Advanced easing curve demonstrations",
            "Smooth color transitions and gradients",
            "Complex transform animations"
        };
        
        for (int i = 0; i < titles.size(); ++i) {
            auto* card = new AnimationDemoCard(titles[i], descriptions[i], this);
            m_animationCards.append(card);
            
            int row = i / 3;
            int col = i % 3;
            m_cardsLayout->addWidget(card, row, col);
            
            // Stagger the entrance animations
            QTimer::singleShot(i * 100, card, &AnimationDemoCard::startAnimation);
        }
    }

private:
    AnimationControlPanel* m_controlPanel;
    PerformanceMonitorWidget* m_performanceMonitor;
    QWidget* m_cardsWidget;
    QGridLayout* m_cardsLayout;
    QList<AnimationDemoCard*> m_animationCards;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize animation system
    auto& performanceManager = Animation::FluentAnimationPerformanceManager::instance();
    performanceManager.startPerformanceMonitoring();
    performanceManager.enableAdaptivePerformance(true);
    performanceManager.enablePowerSavingMode(false);
    
    AdvancedAnimationMainWindow window;
    window.show();
    
    return app.exec();
}

#include "AdvancedAnimationExample.moc"
