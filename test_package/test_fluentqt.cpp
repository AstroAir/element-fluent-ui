#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <iostream>

// Include FluentQt headers
#include "FluentQt/FluentQt.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Styling/FluentTheme.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    try {
        // Initialize FluentQt
        if (!FluentQt::initialize(&app)) {
            std::cerr << "Failed to initialize FluentQt" << std::endl;
            return 1;
        }

        // Test basic functionality
        std::cout << "FluentQt Version: " << FluentQt::version().toStdString() << std::endl;
        std::cout << "Build Info: " << FluentQt::buildInfo().toStdString() << std::endl;

        // Test theme system
        auto& theme = FluentQt::Styling::FluentTheme::instance();
        theme.setMode(FluentQt::Styling::FluentThemeMode::Light);
        theme.setAccentColor(FluentQt::Styling::FluentAccentColor::Blue);

        // Test component creation
        auto* widget = new QWidget;
        auto* layout = new QVBoxLayout(widget);

        auto* card = new FluentQt::Components::FluentCard("Test Card");
        card->setSubtitle("Testing FluentQt installation");

        auto* button = FluentQt::Components::FluentButton::createPrimaryButton("Test Button");

        layout->addWidget(card);
        layout->addWidget(button);

        widget->setLayout(layout);
        widget->resize(300, 200);
        widget->show();

        std::cout << "FluentQt test completed successfully!" << std::endl;
        std::cout << "Components created and displayed." << std::endl;

        // Don't actually run the event loop in the test
        // return app.exec();
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}
