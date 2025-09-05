// examples/FluentTooltip_Enhanced_Usage.cpp
// Demonstrates the enhanced FluentTooltip component with Fluent Design compliance

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Components/FluentButton.h"

using namespace FluentQt::Components;

class TooltipDemoWindow : public QMainWindow {
    Q_OBJECT

public:
    TooltipDemoWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupTooltips();
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QVBoxLayout(centralWidget);
        
        // Title
        auto* titleLabel = new QLabel("FluentTooltip Enhanced Demo", this);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 20px;");
        mainLayout->addWidget(titleLabel);
        
        // Basic tooltips section
        auto* basicSection = createBasicTooltipsSection();
        mainLayout->addWidget(basicSection);
        
        // Advanced tooltips section
        auto* advancedSection = createAdvancedTooltipsSection();
        mainLayout->addWidget(advancedSection);
        
        // Accessibility section
        auto* accessibilitySection = createAccessibilitySection();
        mainLayout->addWidget(accessibilitySection);
        
        setWindowTitle("FluentTooltip Enhanced Demo");
        resize(800, 600);
    }
    
    QWidget* createBasicTooltipsSection() {
        auto* section = new QWidget(this);
        auto* layout = new QVBoxLayout(section);
        
        auto* sectionTitle = new QLabel("Basic Tooltips with Fluent Design", section);
        sectionTitle->setStyleSheet("font-weight: bold; margin: 10px 0;");
        layout->addWidget(sectionTitle);
        
        auto* buttonLayout = new QHBoxLayout();
        
        // Simple tooltip
        m_simpleButton = new QPushButton("Simple Tooltip", section);
        buttonLayout->addWidget(m_simpleButton);
        
        // Title + text tooltip
        m_richButton = new QPushButton("Rich Tooltip", section);
        buttonLayout->addWidget(m_richButton);
        
        // Themed tooltip
        m_themedButton = new QPushButton("Themed Tooltip", section);
        buttonLayout->addWidget(m_themedButton);
        
        layout->addLayout(buttonLayout);
        return section;
    }
    
    QWidget* createAdvancedTooltipsSection() {
        auto* section = new QWidget(this);
        auto* layout = new QVBoxLayout(section);
        
        auto* sectionTitle = new QLabel("Advanced Positioning & Animation", section);
        sectionTitle->setStyleSheet("font-weight: bold; margin: 10px 0;");
        layout->addWidget(sectionTitle);
        
        auto* buttonGrid = new QHBoxLayout();
        
        // Position buttons
        m_topButton = new QPushButton("Top", section);
        m_bottomButton = new QPushButton("Bottom", section);
        m_leftButton = new QPushButton("Left", section);
        m_rightButton = new QPushButton("Right", section);
        m_autoButton = new QPushButton("Auto Position", section);
        
        buttonGrid->addWidget(m_topButton);
        buttonGrid->addWidget(m_bottomButton);
        buttonGrid->addWidget(m_leftButton);
        buttonGrid->addWidget(m_rightButton);
        buttonGrid->addWidget(m_autoButton);
        
        layout->addLayout(buttonGrid);
        return section;
    }
    
    QWidget* createAccessibilitySection() {
        auto* section = new QWidget(this);
        auto* layout = new QVBoxLayout(section);
        
        auto* sectionTitle = new QLabel("Accessibility Features", section);
        sectionTitle->setStyleSheet("font-weight: bold; margin: 10px 0;");
        layout->addWidget(sectionTitle);
        
        auto* infoLabel = new QLabel(
            "• Press Tab to navigate between buttons\n"
            "• Press Escape to close tooltips\n"
            "• Screen reader compatible\n"
            "• Respects reduced motion preferences", section);
        infoLabel->setStyleSheet("margin: 10px; padding: 10px; background: #f0f0f0; border-radius: 4px;");
        layout->addWidget(infoLabel);
        
        m_accessibilityButton = new QPushButton("Accessible Tooltip", section);
        layout->addWidget(m_accessibilityButton);
        
        return section;
    }
    
    void setupTooltips() {
        // Simple tooltip with Fluent design tokens
        auto* simpleTooltip = new FluentTooltip("This is a simple tooltip using Fluent Design tokens", this);
        simpleTooltip->setTheme(FluentTooltipTheme::Auto);  // Adapts to system theme
        connect(m_simpleButton, &QPushButton::entered, [=]() {
            simpleTooltip->showFor(m_simpleButton, FluentTooltipPosition::Top);
        });
        
        // Rich tooltip with title and text
        auto* richTooltip = new FluentTooltip(this);
        richTooltip->setTitle("Rich Tooltip");
        richTooltip->setText("This tooltip demonstrates the enhanced typography system with proper Fluent fonts and spacing.");
        richTooltip->setTheme(FluentTooltipTheme::Auto);
        connect(m_richButton, &QPushButton::entered, [=]() {
            richTooltip->showFor(m_richButton, FluentTooltipPosition::Bottom);
        });
        
        // Themed tooltip
        auto* themedTooltip = new FluentTooltip("Dark themed tooltip with Fluent colors", this);
        themedTooltip->setTheme(FluentTooltipTheme::Dark);
        connect(m_themedButton, &QPushButton::entered, [=]() {
            themedTooltip->showFor(m_themedButton, FluentTooltipPosition::Right);
        });
        
        // Position demonstration tooltips
        setupPositionTooltips();
        
        // Accessibility tooltip
        auto* accessibilityTooltip = new FluentTooltip(this);
        accessibilityTooltip->setTitle("Accessible Tooltip");
        accessibilityTooltip->setText("This tooltip supports keyboard navigation, screen readers, and respects reduced motion preferences.");
        accessibilityTooltip->setPersistent(true);  // Stays visible for accessibility
        connect(m_accessibilityButton, &QPushButton::clicked, [=]() {
            accessibilityTooltip->showFor(m_accessibilityButton, FluentTooltipPosition::Auto);
        });
    }
    
    void setupPositionTooltips() {
        // Top position
        auto* topTooltip = new FluentTooltip("Positioned at the top with Fluent animations", this);
        connect(m_topButton, &QPushButton::entered, [=]() {
            topTooltip->showFor(m_topButton, FluentTooltipPosition::Top);
        });
        
        // Bottom position
        auto* bottomTooltip = new FluentTooltip("Positioned at the bottom with utility motion", this);
        connect(m_bottomButton, &QPushButton::entered, [=]() {
            bottomTooltip->showFor(m_bottomButton, FluentTooltipPosition::Bottom);
        });
        
        // Left position
        auto* leftTooltip = new FluentTooltip("Left positioned with FluentSubtle easing", this);
        connect(m_leftButton, &QPushButton::entered, [=]() {
            leftTooltip->showFor(m_leftButton, FluentTooltipPosition::Left);
        });
        
        // Right position
        auto* rightTooltip = new FluentTooltip("Right positioned with proper arrow rendering", this);
        connect(m_rightButton, &QPushButton::entered, [=]() {
            rightTooltip->showFor(m_rightButton, FluentTooltipPosition::Right);
        });
        
        // Auto position
        auto* autoTooltip = new FluentTooltip("Auto-positioned based on screen boundaries and optimal placement", this);
        connect(m_autoButton, &QPushButton::entered, [=]() {
            autoTooltip->showFor(m_autoButton, FluentTooltipPosition::Auto);
        });
    }

private:
    // Basic tooltips
    QPushButton* m_simpleButton;
    QPushButton* m_richButton;
    QPushButton* m_themedButton;
    
    // Position tooltips
    QPushButton* m_topButton;
    QPushButton* m_bottomButton;
    QPushButton* m_leftButton;
    QPushButton* m_rightButton;
    QPushButton* m_autoButton;
    
    // Accessibility
    QPushButton* m_accessibilityButton;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Enable high DPI support
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    TooltipDemoWindow window;
    window.show();
    
    return app.exec();
}

#include "FluentTooltip_Enhanced_Usage.moc"
