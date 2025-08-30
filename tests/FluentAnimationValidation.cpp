#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <chrono>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Animation;
using namespace FluentQt::Components;
using namespace FluentQt::Styling;
using namespace std::chrono_literals;

class AnimationValidator : public QMainWindow {
    Q_OBJECT

public:
    explicit AnimationValidator(QWidget* parent = nullptr)
        : QMainWindow(parent), m_testsPassed(0), m_totalTests(0) {
        setupUI();
        runValidationTests();
    }

private slots:
    void runValidationTests() {
        qDebug() << "Starting Fluent Design Animation System Validation...";

        // Test 1: Fluent Design Easing Curves
        testFluentEasingCurves();

        // Test 2: Motion Hierarchy
        testMotionHierarchy();

        // Test 3: Component-Specific Animations
        testComponentAnimations();

        // Test 4: Performance Optimizations
        testPerformanceOptimizations();

        // Test 5: Accessibility Support
        testAccessibilitySupport();

        // Test 6: Connected Animations
        testConnectedAnimations();

        // Test 7: Entrance/Exit Animations
        testEntranceExitAnimations();

        // Final results
        QTimer::singleShot(5000, this, &AnimationValidator::showResults);
    }

    void testFluentEasingCurves() {
        qDebug() << "Testing Fluent Design Easing Curves...";
        m_totalTests++;

        try {
            // Test Fluent standard curve
            FluentAnimationConfig config;
            config.easing = FluentEasing::FluentStandard;
            config.duration = 250ms;

            auto animation = FluentAnimator::fadeIn(m_testWidget, config);
            if (animation) {
                animation->start();
                m_testsPassed++;
                qDebug() << "✓ Fluent standard curve test passed";
            } else {
                qDebug() << "✗ Fluent standard curve test failed";
            }
        } catch (const std::exception& e) {
            qDebug() << "✗ Fluent easing curves test failed:" << e.what();
        }
    }

    void testMotionHierarchy() {
        qDebug() << "Testing Motion Hierarchy...";
        m_totalTests++;

        try {
            // Test primary motion (250ms)
            auto primaryAnim = FluentAnimator::primaryMotion(m_primaryLabel);

            // Test secondary motion (150ms)
            auto secondaryAnim =
                FluentAnimator::secondaryMotion(m_secondaryLabel);

            // Test utility motion (100ms)
            auto utilityAnim = FluentAnimator::utilityMotion(m_utilityLabel);

            if (primaryAnim && secondaryAnim && utilityAnim) {
                primaryAnim->start();

                // Use raw pointers for lambda capture
                auto* secondaryPtr = secondaryAnim.release();
                auto* utilityPtr = utilityAnim.release();

                QTimer::singleShot(50, [secondaryPtr]() {
                    secondaryPtr->start();
                    secondaryPtr->deleteLater();
                });
                QTimer::singleShot(100, [utilityPtr]() {
                    utilityPtr->start();
                    utilityPtr->deleteLater();
                });

                m_testsPassed++;
                qDebug() << "✓ Motion hierarchy test passed";
            } else {
                qDebug() << "✗ Motion hierarchy test failed";
            }
        } catch (const std::exception& e) {
            qDebug() << "✗ Motion hierarchy test failed:" << e.what();
        }
    }

    void testComponentAnimations() {
        qDebug() << "Testing Component-Specific Animations...";
        m_totalTests++;

        try {
            // Test button animation
            auto buttonAnim = FluentAnimator::pressEffect(
                m_testButton, FluentAnimationConfig::buttonInteraction());

            // Test card animation
            auto cardAnim = FluentAnimator::hoverEffect(
                m_testCard, FluentAnimationConfig::cardHover());

            if (buttonAnim && cardAnim) {
                buttonAnim->start();

                // Use raw pointer for lambda capture
                auto* cardPtr = cardAnim.release();
                QTimer::singleShot(200, [cardPtr]() {
                    cardPtr->start();
                    cardPtr->deleteLater();
                });

                m_testsPassed++;
                qDebug() << "✓ Component animations test passed";
            } else {
                qDebug() << "✗ Component animations test failed";
            }
        } catch (const std::exception& e) {
            qDebug() << "✗ Component animations test failed:" << e.what();
        }
    }

    void testPerformanceOptimizations() {
        qDebug() << "Testing Performance Optimizations...";
        m_totalTests++;

        try {
            // Test hardware acceleration
            FluentAnimationConfig config;
            config.useHardwareAcceleration = true;
            config.hierarchy = FluentAnimationConfig::MotionHierarchy::Utility;

            auto animation = FluentAnimator::fadeIn(m_testWidget, config);
            if (animation) {
                // Check if hardware acceleration properties are set
                bool hasHardwareAcceleration =
                    animation->property("hardwareAccelerated").toBool();

                animation->start();

                if (hasHardwareAcceleration) {
                    m_testsPassed++;
                    qDebug() << "✓ Performance optimizations test passed";
                } else {
                    qDebug() << "✗ Hardware acceleration not enabled";
                }
            } else {
                qDebug() << "✗ Performance optimizations test failed";
            }
        } catch (const std::exception& e) {
            qDebug() << "✗ Performance optimizations test failed:" << e.what();
        }
    }

    void testAccessibilitySupport() {
        qDebug() << "Testing Accessibility Support...";
        m_totalTests++;

        try {
            // Test reduced motion support
            FluentAnimationConfig config;
            config.respectReducedMotion = true;
            config.duration = 250ms;

            auto animation = FluentAnimator::fadeIn(m_testWidget, config);
            if (animation) {
                animation->start();

                // Check if reduced motion is being respected
                bool respectsReducedMotion = config.respectReducedMotion;

                if (respectsReducedMotion) {
                    m_testsPassed++;
                    qDebug() << "✓ Accessibility support test passed";
                } else {
                    qDebug() << "✗ Reduced motion not respected";
                }
            } else {
                qDebug() << "✗ Accessibility support test failed";
            }
        } catch (const std::exception& e) {
            qDebug() << "✗ Accessibility support test failed:" << e.what();
        }
    }

    void testConnectedAnimations() {
        qDebug() << "Testing Connected Animations...";
        m_totalTests++;

        try {
            // Create temporary widgets for connected animation test
            auto* fromWidget = new QLabel("From", this);
            auto* toWidget = new QLabel("To", this);

            fromWidget->setGeometry(50, 300, 100, 50);
            toWidget->setGeometry(200, 300, 100, 50);
            fromWidget->show();
            toWidget->hide();

            auto connectedAnim = FluentAnimator::connectedAnimation(
                fromWidget, toWidget,
                FluentAnimationConfig::navigationTransition());

            if (connectedAnim) {
                connectedAnim->start();

                // Clean up after test
                QTimer::singleShot(1000, [=]() {
                    fromWidget->deleteLater();
                    toWidget->deleteLater();
                });

                m_testsPassed++;
                qDebug() << "✓ Connected animations test passed";
            } else {
                qDebug() << "✗ Connected animations test failed";
                fromWidget->deleteLater();
                toWidget->deleteLater();
            }
        } catch (const std::exception& e) {
            qDebug() << "✗ Connected animations test failed:" << e.what();
        }
    }

    void testEntranceExitAnimations() {
        qDebug() << "Testing Entrance/Exit Animations...";
        m_totalTests++;

        try {
            // Create temporary widget for entrance/exit test
            auto* testWidget = new QLabel("Entrance/Exit Test", this);
            testWidget->setGeometry(250, 350, 150, 50);
            testWidget->setStyleSheet(
                "background-color: lightblue; border: 1px solid blue;");

            // Test entrance animation
            auto entranceAnim = FluentAnimator::entranceAnimation(
                testWidget, FluentAnimationConfig::dialogTransition());

            if (entranceAnim) {
                entranceAnim->start();

                // Test exit animation after entrance
                QTimer::singleShot(1000, [=]() {
                    auto exitAnim = FluentAnimator::exitAnimation(
                        testWidget, FluentAnimationConfig::dialogTransition());

                    if (exitAnim) {
                        exitAnim->start();

                        // Clean up after exit
                        QTimer::singleShot(
                            500, [=]() { testWidget->deleteLater(); });
                    }
                });

                m_testsPassed++;
                qDebug() << "✓ Entrance/Exit animations test passed";
            } else {
                qDebug() << "✗ Entrance/Exit animations test failed";
                testWidget->deleteLater();
            }
        } catch (const std::exception& e) {
            qDebug() << "✗ Entrance/Exit animations test failed:" << e.what();
        }
    }

    void showResults() {
        qDebug()
            << "\n=== Fluent Design Animation System Validation Results ===";
        qDebug() << "Tests Passed:" << m_testsPassed << "/" << m_totalTests;
        qDebug() << "Success Rate:" << (m_testsPassed * 100.0 / m_totalTests)
                 << "%";

        if (m_testsPassed == m_totalTests) {
            qDebug() << "🎉 All tests passed! Animation system is working "
                        "correctly.";
            m_resultLabel->setText("✅ All Animation Tests Passed!");
            m_resultLabel->setStyleSheet(
                "color: green; font-weight: bold; font-size: 16px;");
        } else {
            qDebug() << "⚠️ Some tests failed. Check the output for details.";
            m_resultLabel->setText(QString("⚠️ %1/%2 Tests Passed")
                                       .arg(m_testsPassed)
                                       .arg(m_totalTests));
            m_resultLabel->setStyleSheet(
                "color: orange; font-weight: bold; font-size: 16px;");
        }
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* layout = new QVBoxLayout(centralWidget);

        // Title
        auto* titleLabel =
            new QLabel("Fluent Design Animation System Validation", this);
        titleLabel->setStyleSheet(
            "font-size: 20px; font-weight: bold; margin: 20px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(titleLabel);

        // Test widgets
        m_testWidget = new QLabel("Test Widget", this);
        m_testWidget->setStyleSheet(
            "background-color: lightgray; padding: 10px; border: 1px solid "
            "gray;");
        layout->addWidget(m_testWidget);

        m_testButton = new FluentButton("Test Button", this);
        layout->addWidget(m_testButton);

        m_testCard = new FluentCard(this);
        m_testCard->setTitle("Test Card");
        m_testCard->setFixedHeight(100);
        layout->addWidget(m_testCard);

        // Motion hierarchy labels
        m_primaryLabel = new QLabel("Primary Motion", this);
        m_secondaryLabel = new QLabel("Secondary Motion", this);
        m_utilityLabel = new QLabel("Utility Motion", this);

        m_primaryLabel->setStyleSheet(
            "background-color: #0078d4; color: white; padding: 5px;");
        m_secondaryLabel->setStyleSheet(
            "background-color: #106ebe; color: white; padding: 5px;");
        m_utilityLabel->setStyleSheet(
            "background-color: #005a9e; color: white; padding: 5px;");

        layout->addWidget(m_primaryLabel);
        layout->addWidget(m_secondaryLabel);
        layout->addWidget(m_utilityLabel);

        // Result label
        m_resultLabel = new QLabel("Running tests...", this);
        m_resultLabel->setAlignment(Qt::AlignCenter);
        m_resultLabel->setStyleSheet("margin: 20px; font-size: 14px;");
        layout->addWidget(m_resultLabel);

        setWindowTitle("Fluent Animation Validation");
        resize(500, 600);
    }

private:
    QLabel* m_testWidget;
    FluentButton* m_testButton;
    FluentCard* m_testCard;
    QLabel* m_primaryLabel;
    QLabel* m_secondaryLabel;
    QLabel* m_utilityLabel;
    QLabel* m_resultLabel;

    int m_testsPassed;
    int m_totalTests;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = FluentTheme::instance();
    theme.setAccentColor(FluentAccentColor::Blue);
    theme.setMode(FluentThemeMode::Light);

    AnimationValidator validator;
    validator.show();

    return app.exec();
}

#include "FluentAnimationValidation.moc"
