// examples/OptimizedAnimationExample.cpp
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentContextMenu.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Animation/FluentAdvancedAnimator.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>

using namespace FluentQt;

class OptimizedAnimationDemo : public QWidget {
    Q_OBJECT

public:
    OptimizedAnimationDemo(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupOptimizedAnimations();
        
        // Apply Fluent theme
        auto& theme = Styling::FluentTheme::instance();
        theme.setMode(Styling::FluentThemeMode::System);
    }

private slots:
    void demonstrateMicroInteractions() {
        // Optimized hover effects with proper timing
        Animation::FluentAnimationConfig hoverConfig;
        hoverConfig.duration = std::chrono::milliseconds(150); // Fluent standard
        hoverConfig.easing = Animation::FluentEasing::EaseOutQuad;
        
        auto hoverAnim = Animation::FluentAnimator::hoverEffect(m_primaryButton, hoverConfig);
        hoverAnim->start();
        
        // Ripple effect for tactile feedback
        QTimer::singleShot(200, [this]() {
            auto rippleAnim = Animation::FluentAnimator::rippleEffect(
                m_primaryButton, m_primaryButton->rect().center());
            rippleAnim->start();
        });
    }
    
    void demonstratePerformantTransitions() {
        // Use transform-based animations for better performance
        Animation::FluentAnimationConfig scaleConfig;
        scaleConfig.duration = std::chrono::milliseconds(300);
        scaleConfig.easing = Animation::FluentEasing::BackOut;
        scaleConfig.useHardwareAcceleration = true;
        
        // Scale animation using optimized geometry approach
        auto scaleAnim = Animation::FluentAnimator::scaleIn(m_demoCard, scaleConfig);
        scaleAnim->start();
        
        // Staggered entrance for multiple elements
        QTimer::singleShot(100, [this]() {
            demonstrateStaggeredAnimations();
        });
    }
    
    void demonstrateStaggeredAnimations() {
        QList<QWidget*> buttons = {m_button1, m_button2, m_button3};
        
        for (int i = 0; i < buttons.size(); ++i) {
            QTimer::singleShot(i * 100, [this, buttons, i]() {
                Animation::FluentAnimationConfig slideConfig;
                slideConfig.duration = std::chrono::milliseconds(400);
                slideConfig.easing = Animation::FluentEasing::EaseOutCubic;
                
                auto slideAnim = Animation::FluentAnimator::slideUp(buttons[i], 30, slideConfig);
                slideAnim->start();
            });
        }
    }
    
    void demonstrateReducedMotion() {
        // Check accessibility preferences
        if (Animation::FluentAnimator::shouldRespectReducedMotion()) {
            m_statusLabel->setText("Reduced motion mode - animations disabled");
            return;
        }
        
        // Normal animation flow
        Animation::FluentAnimationConfig config;
        config.respectReducedMotion = true;
        config.duration = std::chrono::milliseconds(200);
        
        auto fadeAnim = Animation::FluentAnimator::fadeIn(m_statusLabel, config);
        fadeAnim->start();
    }

private:
    void setupUI() {
        setWindowTitle("Optimized Animation Demo");
        setMinimumSize(800, 600);
        
        auto* layout = new QVBoxLayout(this);
        
        // Title
        auto* titleLabel = new QLabel("Fluent UI Animation Optimizations");
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px;");
        layout->addWidget(titleLabel);
        
        // Demo card
        m_demoCard = new Components::FluentCard;
        m_demoCard->setTitle("Performance Optimized Animations");
        m_demoCard->setDescription("Demonstrating hardware-accelerated, Fluent Design compliant animations");
        layout->addWidget(m_demoCard);
        
        // Button row
        auto* buttonLayout = new QHBoxLayout;
        
        m_primaryButton = new Components::FluentButton("Micro-interactions");
        m_primaryButton->setButtonStyle(Components::FluentButtonStyle::Primary);
        connect(m_primaryButton, &Components::FluentButton::clicked,
                this, &OptimizedAnimationDemo::demonstrateMicroInteractions);
        buttonLayout->addWidget(m_primaryButton);
        
        auto* transitionButton = new Components::FluentButton("Performant Transitions");
        connect(transitionButton, &Components::FluentButton::clicked,
                this, &OptimizedAnimationDemo::demonstratePerformantTransitions);
        buttonLayout->addWidget(transitionButton);
        
        auto* accessibilityButton = new Components::FluentButton("Reduced Motion");
        connect(accessibilityButton, &Components::FluentButton::clicked,
                this, &OptimizedAnimationDemo::demonstrateReducedMotion);
        buttonLayout->addWidget(accessibilityButton);
        
        layout->addLayout(buttonLayout);
        
        // Stagger demo buttons
        auto* staggerLayout = new QHBoxLayout;
        m_button1 = new Components::FluentButton("Button 1");
        m_button2 = new Components::FluentButton("Button 2");
        m_button3 = new Components::FluentButton("Button 3");
        
        staggerLayout->addWidget(m_button1);
        staggerLayout->addWidget(m_button2);
        staggerLayout->addWidget(m_button3);
        layout->addLayout(staggerLayout);
        
        // Status label
        m_statusLabel = new QLabel("Ready for animation demonstrations");
        m_statusLabel->setStyleSheet("color: #666; margin: 10px;");
        layout->addWidget(m_statusLabel);
        
        layout->addStretch();
    }
    
    void setupOptimizedAnimations() {
        // Enable hardware acceleration for all components
        for (auto* widget : findChildren<QWidget*>()) {
            widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
            widget->setAttribute(Qt::WA_NoSystemBackground, true);
        }
        
        // Setup entrance animations
        QTimer::singleShot(100, [this]() {
            Animation::FluentAnimationConfig entranceConfig;
            entranceConfig.duration = std::chrono::milliseconds(600);
            entranceConfig.easing = Animation::FluentEasing::EaseOutCubic;
            
            auto slideAnim = Animation::FluentAnimator::slideUp(this, 50, entranceConfig);
            slideAnim->start();
        });
    }

private:
    Components::FluentCard* m_demoCard;
    Components::FluentButton* m_primaryButton;
    Components::FluentButton* m_button1;
    Components::FluentButton* m_button2;
    Components::FluentButton* m_button3;
    QLabel* m_statusLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize FluentQt
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
    
    OptimizedAnimationDemo demo;
    demo.show();
    
    return app.exec();
}

#include "OptimizedAnimationExample.moc"
