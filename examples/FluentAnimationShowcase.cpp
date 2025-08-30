#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Animation;
using namespace FluentQt::Components;
using namespace FluentQt::Styling;

class FluentAnimationShowcase : public QMainWindow {
    Q_OBJECT

public:
    explicit FluentAnimationShowcase(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        setupUI();
        setupAnimations();
    }

private slots:
    void demonstrateButtonAnimations() {
        // Utility motion for button press
        auto pressAnim = FluentAnimator::utilityMotion(m_demoButton);
        pressAnim->start();
        
        // Button-specific press effect
        auto buttonPressAnim = FluentAnimator::pressEffect(
            m_demoButton, FluentAnimationConfig::buttonInteraction());
        
        QTimer::singleShot(100, [=]() {
            buttonPressAnim->start();
        });
    }
    
    void demonstrateCardAnimations() {
        // Card entrance animation
        auto entranceAnim = FluentAnimator::entranceAnimation(
            m_demoCard, FluentAnimationConfig::cardHover());
        entranceAnim->start();
        
        // Card hover effect
        QTimer::singleShot(500, [=]() {
            auto hoverAnim = FluentAnimator::hoverEffect(
                m_demoCard, FluentAnimationConfig::cardHover());
            hoverAnim->start();
        });
    }
    
    void demonstrateNavigationAnimations() {
        // Create temporary widgets for navigation demo
        auto* fromWidget = new QLabel("From Page", this);
        auto* toWidget = new QLabel("To Page", this);
        
        fromWidget->setGeometry(50, 300, 200, 100);
        toWidget->setGeometry(300, 300, 200, 100);
        fromWidget->setStyleSheet("background-color: lightblue; border: 1px solid blue;");
        toWidget->setStyleSheet("background-color: lightgreen; border: 1px solid green;");
        
        fromWidget->show();
        toWidget->hide();
        
        // Navigation transition
        auto navAnim = FluentAnimator::navigationTransition(
            fromWidget, toWidget, FluentAnimationConfig::navigationTransition());
        navAnim->start();
        
        // Clean up after animation
        QTimer::singleShot(1000, [=]() {
            fromWidget->deleteLater();
            toWidget->deleteLater();
        });
    }
    
    void demonstrateConnectedAnimations() {
        // Create elements for connected animation demo
        auto* sourceElement = new QPushButton("Source", this);
        auto* targetElement = new QPushButton("Target", this);
        
        sourceElement->setGeometry(50, 450, 100, 40);
        targetElement->setGeometry(400, 450, 100, 40);
        
        sourceElement->show();
        targetElement->show();
        
        // Connected element animation
        auto connectedAnim = FluentAnimator::connectedElementAnimation(
            sourceElement, targetElement, this, this,
            FluentAnimationConfig::navigationTransition());
        connectedAnim->start();
        
        // Clean up after animation
        QTimer::singleShot(1500, [=]() {
            sourceElement->deleteLater();
            targetElement->deleteLater();
        });
    }
    
    void demonstrateMotionHierarchy() {
        // Primary motion (250ms)
        auto primaryAnim = FluentAnimator::primaryMotion(m_primaryLabel);
        primaryAnim->start();
        
        // Secondary motion (150ms) - starts slightly after primary
        QTimer::singleShot(50, [=]() {
            auto secondaryAnim = FluentAnimator::secondaryMotion(m_secondaryLabel);
            secondaryAnim->start();
        });
        
        // Utility motion (100ms) - starts after secondary
        QTimer::singleShot(100, [=]() {
            auto utilityAnim = FluentAnimator::utilityMotion(m_utilityLabel);
            utilityAnim->start();
        });
    }
    
    void demonstrateThemeTransition() {
        auto& theme = FluentTheme::instance();
        
        // Toggle theme mode with animated transition
        if (theme.mode() == FluentThemeMode::Light) {
            theme.setMode(FluentThemeMode::Dark);
        } else {
            theme.setMode(FluentThemeMode::Light);
        }
        
        // The theme system automatically handles the coordinated animation
    }
    
    void demonstrateEntranceExitAnimations() {
        // Create a temporary widget for entrance/exit demo
        auto* demoWidget = new QLabel("Entrance/Exit Demo", this);
        demoWidget->setGeometry(250, 200, 200, 50);
        demoWidget->setStyleSheet("background-color: yellow; border: 1px solid orange;");
        demoWidget->setAlignment(Qt::AlignCenter);
        
        // Entrance animation
        auto entranceAnim = FluentAnimator::entranceAnimation(
            demoWidget, FluentAnimationConfig::dialogTransition());
        entranceAnim->start();
        
        // Exit animation after 2 seconds
        QTimer::singleShot(2000, [=]() {
            auto exitAnim = FluentAnimator::exitAnimation(
                demoWidget, FluentAnimationConfig::dialogTransition());
            exitAnim->start();
            
            // Clean up after exit
            QTimer::singleShot(500, [=]() {
                demoWidget->deleteLater();
            });
        });
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QVBoxLayout(centralWidget);
        
        // Title
        auto* titleLabel = new QLabel("Fluent Design Animation Showcase", this);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        // Animation demo buttons
        auto* buttonLayout = new QGridLayout();
        
        auto* buttonAnimBtn = new QPushButton("Button Animations", this);
        auto* cardAnimBtn = new QPushButton("Card Animations", this);
        auto* navAnimBtn = new QPushButton("Navigation Animations", this);
        auto* connectedAnimBtn = new QPushButton("Connected Animations", this);
        auto* hierarchyAnimBtn = new QPushButton("Motion Hierarchy", this);
        auto* themeAnimBtn = new QPushButton("Theme Transition", this);
        auto* entranceExitBtn = new QPushButton("Entrance/Exit", this);
        
        buttonLayout->addWidget(buttonAnimBtn, 0, 0);
        buttonLayout->addWidget(cardAnimBtn, 0, 1);
        buttonLayout->addWidget(navAnimBtn, 0, 2);
        buttonLayout->addWidget(connectedAnimBtn, 1, 0);
        buttonLayout->addWidget(hierarchyAnimBtn, 1, 1);
        buttonLayout->addWidget(themeAnimBtn, 1, 2);
        buttonLayout->addWidget(entranceExitBtn, 2, 0);
        
        mainLayout->addLayout(buttonLayout);
        
        // Demo elements
        auto* demoGroup = new QGroupBox("Demo Elements", this);
        auto* demoLayout = new QHBoxLayout(demoGroup);
        
        m_demoButton = new FluentButton("Demo Button", this);
        m_demoCard = new FluentCard(this);
        m_demoCard->setHeaderTitle("Demo Card");
        m_demoCard->setFixedSize(150, 100);
        
        demoLayout->addWidget(m_demoButton);
        demoLayout->addWidget(m_demoCard);
        
        mainLayout->addWidget(demoGroup);
        
        // Motion hierarchy demo
        auto* hierarchyGroup = new QGroupBox("Motion Hierarchy", this);
        auto* hierarchyLayout = new QVBoxLayout(hierarchyGroup);
        
        m_primaryLabel = new QLabel("Primary Motion (250ms)", this);
        m_secondaryLabel = new QLabel("Secondary Motion (150ms)", this);
        m_utilityLabel = new QLabel("Utility Motion (100ms)", this);
        
        m_primaryLabel->setStyleSheet("background-color: #0078d4; color: white; padding: 10px;");
        m_secondaryLabel->setStyleSheet("background-color: #106ebe; color: white; padding: 10px;");
        m_utilityLabel->setStyleSheet("background-color: #005a9e; color: white; padding: 10px;");
        
        hierarchyLayout->addWidget(m_primaryLabel);
        hierarchyLayout->addWidget(m_secondaryLabel);
        hierarchyLayout->addWidget(m_utilityLabel);
        
        mainLayout->addWidget(hierarchyGroup);
        
        // Connect buttons
        connect(buttonAnimBtn, &QPushButton::clicked, this, &FluentAnimationShowcase::demonstrateButtonAnimations);
        connect(cardAnimBtn, &QPushButton::clicked, this, &FluentAnimationShowcase::demonstrateCardAnimations);
        connect(navAnimBtn, &QPushButton::clicked, this, &FluentAnimationShowcase::demonstrateNavigationAnimations);
        connect(connectedAnimBtn, &QPushButton::clicked, this, &FluentAnimationShowcase::demonstrateConnectedAnimations);
        connect(hierarchyAnimBtn, &QPushButton::clicked, this, &FluentAnimationShowcase::demonstrateMotionHierarchy);
        connect(themeAnimBtn, &QPushButton::clicked, this, &FluentAnimationShowcase::demonstrateThemeTransition);
        connect(entranceExitBtn, &QPushButton::clicked, this, &FluentAnimationShowcase::demonstrateEntranceExitAnimations);
        
        setWindowTitle("Fluent Design Animation Showcase");
        resize(800, 600);
    }
    
    void setupAnimations() {
        // Set up automatic hover effects for demo elements
        connect(m_demoButton, &FluentButton::hovered, [=](bool hovered) {
            if (hovered) {
                auto hoverAnim = FluentAnimator::hoverEffect(
                    m_demoButton, FluentAnimationConfig::buttonInteraction());
                hoverAnim->start();
            }
        });
    }

private:
    FluentButton* m_demoButton;
    FluentCard* m_demoCard;
    QLabel* m_primaryLabel;
    QLabel* m_secondaryLabel;
    QLabel* m_utilityLabel;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Initialize FluentQt theme
    auto& theme = FluentTheme::instance();
    theme.setAccentColor(FluentAccentColor::Blue);
    theme.setMode(FluentThemeMode::Light);
    
    FluentAnimationShowcase showcase;
    showcase.show();
    
    return app.exec();
}

#include "FluentAnimationShowcase.moc"
