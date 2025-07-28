// examples/CarouselShowcaseExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QRandomGenerator>

// FluentQt Carousel Components
#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentBasicCarousel.h"
#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Components/FluentIndicatorCarousel.h"
#include "FluentQt/Components/FluentTouchCarousel.h"

// FluentQt Core
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class CarouselShowcaseWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit CarouselShowcaseWindow(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        
        setWindowTitle("FluentCarousel Showcase - Element Fluent UI");
        setMinimumSize(1200, 800);
        
        setupUI();
        createCarouselExamples();
        
        // Apply Fluent theme
        auto& theme = FluentQt::Styling::FluentTheme::instance();
        theme.setTheme(FluentQt::Styling::FluentThemeMode::Light);
    }

private slots:
    void onCarouselIndexChanged(int index) {
        auto* carousel = qobject_cast<FluentCarousel*>(sender());
        if (carousel) {
            qDebug() << "Carousel index changed to:" << index;
        }
    }

    void onAutoPlayToggled() {
        if (m_autoCarousel) {
            if (m_autoCarousel->autoPlayActive()) {
                m_autoCarousel->stopAutoPlay();
            } else {
                m_autoCarousel->startAutoPlay();
            }
        }
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QVBoxLayout(centralWidget);
        
        // Title
        auto* titleLabel = new QLabel("FluentCarousel Component Showcase");
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px; color: #323130;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        // Create tab widget for different carousel types
        m_tabWidget = new QTabWidget();
        m_tabWidget->setStyleSheet(R"(
            QTabWidget::pane {
                border: 1px solid #c8c6c4;
                background-color: white;
            }
            QTabBar::tab {
                background-color: #f3f2f1;
                padding: 10px 20px;
                margin-right: 2px;
                border: 1px solid #c8c6c4;
                border-bottom: none;
            }
            QTabBar::tab:selected {
                background-color: white;
                border-bottom: 1px solid white;
            }
            QTabBar::tab:hover {
                background-color: #e1dfdd;
            }
        )");
        
        mainLayout->addWidget(m_tabWidget);
    }

    void createCarouselExamples() {
        createBasicCarouselExample();
        createAutoCarouselExample();
        createIndicatorCarouselExample();
        createTouchCarouselExample();
        createAdvancedExample();
    }

    void createBasicCarouselExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        // Description
        auto* descLabel = new QLabel(
            "FluentBasicCarousel provides simple navigation with previous/next buttons. "
            "Perfect for basic content browsing where users control the navigation."
        );
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("margin: 10px; color: #605e5c;");
        layout->addWidget(descLabel);
        
        // Create basic carousel
        m_basicCarousel = new FluentBasicCarousel(widget);
        m_basicCarousel->setFixedSize(600, 300);
        
        // Configure
        m_basicCarousel->setTransition(FluentCarouselTransition::Slide);
        m_basicCarousel->setTransitionDuration(300);
        m_basicCarousel->setWrapAround(false);
        m_basicCarousel->setShowNavigationButtons(true);
        
        // Add content
        QStringList colors = {"#FF6B6B", "#4ECDC4", "#45B7D1", "#96CEB4", "#FFEAA7", "#DDA0DD"};
        for (int i = 0; i < colors.size(); ++i) {
            auto* item = createColoredItem(QString("Basic Slide %1").arg(i + 1), colors[i]);
            m_basicCarousel->addItem(item);
        }
        
        // Connect signals
        connect(m_basicCarousel, &FluentCarousel::currentIndexChanged,
                this, &CarouselShowcaseWindow::onCarouselIndexChanged);
        
        // Center the carousel
        auto* carouselLayout = new QHBoxLayout();
        carouselLayout->addStretch();
        carouselLayout->addWidget(m_basicCarousel);
        carouselLayout->addStretch();
        layout->addLayout(carouselLayout);
        
        layout->addStretch();
        m_tabWidget->addTab(widget, "Basic Carousel");
    }

    void createAutoCarouselExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        // Description
        auto* descLabel = new QLabel(
            "FluentAutoCarousel automatically advances through items with configurable timing. "
            "Includes play/pause controls and progress indicators. Perfect for slideshows and promotional content."
        );
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("margin: 10px; color: #605e5c;");
        layout->addWidget(descLabel);
        
        // Create auto carousel
        m_autoCarousel = new FluentAutoCarousel(widget);
        m_autoCarousel->setFixedSize(600, 300);
        
        // Configure auto-play
        m_autoCarousel->setAutoPlayInterval(2500);
        m_autoCarousel->setPauseOnHover(true);
        m_autoCarousel->setShowPlayPauseButton(true);
        m_autoCarousel->setShowProgressIndicator(true);
        m_autoCarousel->setTransition(FluentCarouselTransition::Fade);
        m_autoCarousel->setTransitionDuration(500);
        
        // Add image-like content
        QStringList imageData = {
            "Sunset over mountains",
            "Ocean waves at beach", 
            "Forest in autumn",
            "City skyline at night",
            "Desert landscape"
        };
        QStringList imageColors = {"#FF7F50", "#20B2AA", "#DAA520", "#4169E1", "#CD853F"};
        
        for (int i = 0; i < imageData.size(); ++i) {
            auto* item = createImageLikeItem(imageData[i], imageColors[i]);
            m_autoCarousel->addItem(item);
        }
        
        // Start auto-play
        m_autoCarousel->startAutoPlay();
        
        // Control buttons
        auto* controlsLayout = new QHBoxLayout();
        auto* playPauseBtn = new QPushButton("Toggle Auto-Play");
        playPauseBtn->setStyleSheet("padding: 8px 16px; background-color: #0078d4; color: white; border: none; border-radius: 4px;");
        connect(playPauseBtn, &QPushButton::clicked, this, &CarouselShowcaseWindow::onAutoPlayToggled);
        
        controlsLayout->addStretch();
        controlsLayout->addWidget(playPauseBtn);
        controlsLayout->addStretch();
        
        // Center the carousel
        auto* carouselLayout = new QHBoxLayout();
        carouselLayout->addStretch();
        carouselLayout->addWidget(m_autoCarousel);
        carouselLayout->addStretch();
        layout->addLayout(carouselLayout);
        
        layout->addLayout(controlsLayout);
        layout->addStretch();
        m_tabWidget->addTab(widget, "Auto Carousel");
    }

    void createIndicatorCarouselExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        // Description
        auto* descLabel = new QLabel(
            "FluentIndicatorCarousel shows visual indicators for current position and allows direct navigation. "
            "Supports dots, lines, numbers, and thumbnail indicators."
        );
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("margin: 10px; color: #605e5c;");
        layout->addWidget(descLabel);
        
        // Create indicator carousel
        m_indicatorCarousel = new FluentIndicatorCarousel(widget);
        m_indicatorCarousel->setFixedSize(600, 300);
        
        // Configure indicators
        m_indicatorCarousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Dots);
        m_indicatorCarousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Bottom);
        m_indicatorCarousel->setShowIndicators(true);
        m_indicatorCarousel->setTransition(FluentCarouselTransition::Scale);
        m_indicatorCarousel->setTransitionDuration(400);
        
        // Add product-like content
        QStringList products = {"Wireless Headphones", "Smart Watch", "Tablet Device", "Gaming Mouse", "Bluetooth Speaker"};
        QStringList productColors = {"#E74C3C", "#3498DB", "#2ECC71", "#F39C12", "#9B59B6"};
        
        for (int i = 0; i < products.size(); ++i) {
            auto* item = createProductItem(products[i], QString("$%1.99").arg(99 + i * 50), productColors[i]);
            m_indicatorCarousel->addItem(item);
        }
        
        // Center the carousel
        auto* carouselLayout = new QHBoxLayout();
        carouselLayout->addStretch();
        carouselLayout->addWidget(m_indicatorCarousel);
        carouselLayout->addStretch();
        layout->addLayout(carouselLayout);
        
        layout->addStretch();
        m_tabWidget->addTab(widget, "Indicator Carousel");
    }

    void createTouchCarouselExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        // Description
        auto* descLabel = new QLabel(
            "FluentTouchCarousel is optimized for touch devices with swipe gestures and momentum scrolling. "
            "Try dragging the items to navigate (simulated with mouse)."
        );
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("margin: 10px; color: #605e5c;");
        layout->addWidget(descLabel);
        
        // Create touch carousel
        m_touchCarousel = new FluentTouchCarousel(widget);
        m_touchCarousel->setFixedSize(600, 300);
        
        // Configure touch behavior
        FluentCarouselTouchConfig touchConfig;
        touchConfig.swipeThreshold = 50;
        touchConfig.gestureSensitivity = FluentCarouselGestureSensitivity::Medium;
        touchConfig.momentumScrolling = true;
        touchConfig.edgeBehavior = FluentCarouselEdgeBehavior::Resistance;
        m_touchCarousel->setTouchConfiguration(touchConfig);
        
        m_touchCarousel->setTransition(FluentCarouselTransition::Slide);
        m_touchCarousel->setTransitionDuration(250);
        
        // Add touch-optimized content
        QStringList touchItems = {"Swipe Left", "Drag to Navigate", "Touch Friendly", "Momentum Scroll", "Edge Resistance", "Gesture Support"};
        QStringList touchColors = {"#FF6B9D", "#C44569", "#F8B500", "#6C5CE7", "#00B894", "#FDCB6E"};
        
        for (int i = 0; i < touchItems.size(); ++i) {
            auto* item = createTouchItem(touchItems[i], touchColors[i]);
            m_touchCarousel->addItem(item);
        }
        
        // Center the carousel
        auto* carouselLayout = new QHBoxLayout();
        carouselLayout->addStretch();
        carouselLayout->addWidget(m_touchCarousel);
        carouselLayout->addStretch();
        layout->addLayout(carouselLayout);
        
        layout->addStretch();
        m_tabWidget->addTab(widget, "Touch Carousel");
    }

    void createAdvancedExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        // Description
        auto* descLabel = new QLabel(
            "Advanced example combining multiple carousel features and demonstrating different transition effects."
        );
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("margin: 10px; color: #605e5c;");
        layout->addWidget(descLabel);
        
        // Create advanced carousel with multiple transitions
        m_advancedCarousel = new FluentCarousel(widget);
        m_advancedCarousel->setFixedSize(600, 300);
        
        // Configure with advanced settings
        FluentCarouselConfig config;
        config.transition = FluentCarouselTransition::Coverflow;
        config.transitionDuration = 600;
        config.autoPlay = FluentCarouselAutoPlay::PauseOnHover;
        config.autoPlayInterval = 4000;
        config.wrapAround = true;
        config.showNavigationButtons = true;
        config.keyboardNavigation = true;
        
        m_advancedCarousel->setConfiguration(config);
        
        // Add diverse content
        for (int i = 0; i < 8; ++i) {
            auto* item = createAdvancedItem(i);
            m_advancedCarousel->addItem(item);
        }
        
        m_advancedCarousel->startAutoPlay();
        
        // Transition controls
        auto* transitionLayout = new QHBoxLayout();
        auto* transitionLabel = new QLabel("Transition Effect:");
        auto* transitionCombo = new QComboBox();
        transitionCombo->addItems({"Slide", "Fade", "Scale", "Flip", "Cube", "Coverflow"});
        transitionCombo->setCurrentText("Coverflow");
        
        connect(transitionCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
                [this](const QString& text) {
                    FluentCarouselTransition transition = FluentCarouselTransition::Slide;
                    if (text == "Fade") transition = FluentCarouselTransition::Fade;
                    else if (text == "Scale") transition = FluentCarouselTransition::Scale;
                    else if (text == "Flip") transition = FluentCarouselTransition::Flip;
                    else if (text == "Cube") transition = FluentCarouselTransition::Cube;
                    else if (text == "Coverflow") transition = FluentCarouselTransition::Coverflow;
                    
                    m_advancedCarousel->setTransition(transition);
                });
        
        transitionLayout->addStretch();
        transitionLayout->addWidget(transitionLabel);
        transitionLayout->addWidget(transitionCombo);
        transitionLayout->addStretch();
        
        // Center the carousel
        auto* carouselLayout = new QHBoxLayout();
        carouselLayout->addStretch();
        carouselLayout->addWidget(m_advancedCarousel);
        carouselLayout->addStretch();
        layout->addLayout(carouselLayout);
        
        layout->addLayout(transitionLayout);
        layout->addStretch();
        m_tabWidget->addTab(widget, "Advanced Features");
    }

    QWidget* createColoredItem(const QString& text, const QString& color) {
        auto* widget = new QWidget();
        widget->setFixedSize(580, 280);
        widget->setStyleSheet(QString("background-color: %1; border-radius: 8px;").arg(color));
        
        auto* label = new QLabel(text, widget);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
        label->setGeometry(widget->rect());
        
        return widget;
    }

    QWidget* createImageLikeItem(const QString& title, const QString& color) {
        auto* widget = new QWidget();
        widget->setFixedSize(580, 280);
        
        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(0, 0, 0, 0);
        
        // Image area
        auto* imageArea = new QWidget();
        imageArea->setFixedHeight(220);
        imageArea->setStyleSheet(QString("background-color: %1; border-radius: 8px 8px 0 0;").arg(color));
        
        // Title area
        auto* titleArea = new QWidget();
        titleArea->setFixedHeight(60);
        titleArea->setStyleSheet("background-color: white; border-radius: 0 0 8px 8px; border-top: 1px solid #e1dfdd;");
        
        auto* titleLabel = new QLabel(title, titleArea);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("color: #323130; font-size: 16px; font-weight: bold;");
        titleLabel->setGeometry(titleArea->rect());
        
        layout->addWidget(imageArea);
        layout->addWidget(titleArea);
        
        return widget;
    }

    QWidget* createProductItem(const QString& name, const QString& price, const QString& color) {
        auto* widget = new QWidget();
        widget->setFixedSize(580, 280);
        widget->setStyleSheet("background-color: white; border: 1px solid #e1dfdd; border-radius: 8px;");
        
        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(20, 20, 20, 20);
        
        // Product image placeholder
        auto* imageWidget = new QWidget();
        imageWidget->setFixedHeight(160);
        imageWidget->setStyleSheet(QString("background-color: %1; border-radius: 4px;").arg(color));
        
        // Product info
        auto* nameLabel = new QLabel(name);
        nameLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #323130;");
        nameLabel->setAlignment(Qt::AlignCenter);
        
        auto* priceLabel = new QLabel(price);
        priceLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #0078d4;");
        priceLabel->setAlignment(Qt::AlignCenter);
        
        layout->addWidget(imageWidget);
        layout->addWidget(nameLabel);
        layout->addWidget(priceLabel);
        layout->addStretch();
        
        return widget;
    }

    QWidget* createTouchItem(const QString& text, const QString& color) {
        auto* widget = new QWidget();
        widget->setFixedSize(580, 280);
        widget->setStyleSheet(QString("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2); border-radius: 8px;")
                             .arg(color).arg(adjustColor(color, 20)));
        
        auto* label = new QLabel(text, widget);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: white; font-size: 22px; font-weight: bold; text-shadow: 2px 2px 4px rgba(0,0,0,0.3);");
        label->setGeometry(widget->rect());
        
        // Add touch hint
        auto* hintLabel = new QLabel("👆 Drag to navigate", widget);
        hintLabel->setAlignment(Qt::AlignCenter);
        hintLabel->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 14px; margin-top: 40px;");
        hintLabel->setGeometry(0, 200, 580, 30);
        
        return widget;
    }

    QWidget* createAdvancedItem(int index) {
        auto* widget = new QWidget();
        widget->setFixedSize(580, 280);
        
        // Create gradient background
        QString color1 = QString("hsl(%1, 70%, 60%)").arg(index * 45);
        QString color2 = QString("hsl(%1, 70%, 40%)").arg(index * 45 + 20);
        widget->setStyleSheet(QString("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2); border-radius: 12px;")
                             .arg(color1).arg(color2));
        
        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(30, 30, 30, 30);
        
        auto* titleLabel = new QLabel(QString("Advanced Item %1").arg(index + 1));
        titleLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold; text-shadow: 2px 2px 4px rgba(0,0,0,0.3);");
        titleLabel->setAlignment(Qt::AlignCenter);
        
        auto* descLabel = new QLabel("Demonstrating advanced carousel features with smooth transitions and rich content.");
        descLabel->setStyleSheet("color: rgba(255,255,255,0.9); font-size: 14px; text-shadow: 1px 1px 2px rgba(0,0,0,0.3);");
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setWordWrap(true);
        
        layout->addWidget(titleLabel);
        layout->addWidget(descLabel);
        layout->addStretch();
        
        return widget;
    }

    QString adjustColor(const QString& color, int adjustment) {
        // Simple color adjustment for gradients
        QColor c(color);
        int h, s, l;
        c.getHsl(&h, &s, &l);
        l = qBound(0, l + adjustment, 255);
        return QColor::fromHsl(h, s, l).name();
    }

private:
    QTabWidget* m_tabWidget{nullptr};
    FluentBasicCarousel* m_basicCarousel{nullptr};
    FluentAutoCarousel* m_autoCarousel{nullptr};
    FluentIndicatorCarousel* m_indicatorCarousel{nullptr};
    FluentTouchCarousel* m_touchCarousel{nullptr};
    FluentCarousel* m_advancedCarousel{nullptr};
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("FluentCarousel Showcase");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Element Fluent UI");
    
    // Create and show the showcase window
    CarouselShowcaseWindow window;
    window.show();
    
    return app.exec();
}

#include "CarouselShowcaseExample.moc"
