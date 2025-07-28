// examples/main.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"

using namespace FluentQt;

class FluentDemo : public QMainWindow {
    Q_OBJECT

public:
    FluentDemo(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupTheme();
        setupAnimations();
    }

private slots:
    void toggleTheme() {
        auto& theme = Styling::FluentTheme::instance();
        auto newMode = (theme.mode() == Styling::FluentThemeMode::Light) 
            ? Styling::FluentThemeMode::Dark 
            : Styling::FluentThemeMode::Light;
        theme.setMode(newMode);
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* layout = new QVBoxLayout(centralWidget);
        
        // Header with theme toggle
        auto* headerLayout = new QHBoxLayout();
        
        auto* titleLabel = new QLabel("FluentQt Demo");
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        
        auto* themeButton = new Components::FluentButton("Toggle Theme", this);
        themeButton->setButtonStyle(Components::FluentButtonStyle::Accent);
        connect(themeButton, &Components::FluentButton::clicked, 
                this, &FluentDemo::toggleTheme);
        
        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(themeButton);
        
        // Button showcase
        auto* buttonCard = new Components::FluentCard("Button Styles", this);
        auto* buttonLayout = new QHBoxLayout();
        
        auto* defaultBtn = new Components::FluentButton("Default", this);
        auto* primaryBtn = new Components::FluentButton("Primary", this);
        primaryBtn->setButtonStyle(Components::FluentButtonStyle::Primary);
        
        auto* accentBtn = new Components::FluentButton("Accent", this);
        accentBtn->setButtonStyle(Components::FluentButtonStyle::Accent);
        
        auto* subtleBtn = new Components::FluentButton("Subtle", this);
        subtleBtn->setButtonStyle(Components::FluentButtonStyle::Subtle);
        
        buttonLayout->addWidget(defaultBtn);
        buttonLayout->addWidget(primaryBtn);
        buttonLayout->addWidget(accentBtn);
        buttonLayout->addWidget(subtleBtn);
        
        auto* buttonContainer = new QWidget();
        buttonContainer->setLayout(buttonLayout);
        buttonCard->setContentWidget(buttonContainer);
        
        layout->addLayout(headerLayout);
        layout->addWidget(buttonCard);
        layout->addStretch();
        
        resize(800, 600);
    }
    
    void setupTheme() {
        auto& theme = Styling::FluentTheme::instance();
        theme.setAccentColor(Styling::FluentAccentColor::Blue);
    }
    
    void setupAnimations() {
        // Add entrance animations for cards
        QTimer::singleShot(100, [this]() {
            auto cards = findChildren<Components::FluentCard*>();
            for (int i = 0; i < cards.size(); ++i) {
                auto* card = cards[i];
                card->hide();
                
                QTimer::singleShot(i * 100, [card]() {
                    card->show();
                    Animation::FluentAnimationConfig config;
                    config.duration = 400ms;
                    auto animation = Animation::FluentAnimator::slideIn(
                        card, QPoint(0, 50), config
                    );
                    animation->start();
                });
            }
        });
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize FluentQt
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
    
    FluentDemo demo;
    demo.show();
    
    return app.exec();
}

#include "main.moc"