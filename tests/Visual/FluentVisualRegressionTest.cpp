// tests/Visual/FluentVisualRegressionTest.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <QStandardPaths>
#include <QScreen>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Core/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentVisualRegressionTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Component visual tests
    void testFluentButtonVisual();
    void testFluentButtonStates();
    void testFluentTextInputVisual();
    void testFluentCardVisual();
    void testFluentProgressBarVisual();

    // Theme visual tests
    void testLightThemeVisual();
    void testDarkThemeVisual();
    void testHighContrastThemeVisual();

    // Layout visual tests
    void testComplexLayoutVisual();
    void testResponsiveLayoutVisual();

    // Animation visual tests
    void testAnimationFrames();
    void testTransitionStates();

    // Cross-platform visual tests
    void testPlatformConsistency();

private:
    QPixmap captureWidget(QWidget* widget, const QString& testName = "");
    bool comparePixmaps(const QPixmap& actual, const QPixmap& expected, double threshold = 0.95);
    void saveReferenceImage(const QPixmap& pixmap, const QString& testName);
    QPixmap loadReferenceImage(const QString& testName);
    QString getTestDataPath() const;
    void setupTestWidget(QWidget* widget);

private:
    QWidget* m_testWidget;
    QString m_testDataPath;
    QScreen* m_primaryScreen;
    double m_devicePixelRatio;
};

void FluentVisualRegressionTest::initTestCase() {
    // Set up test environment
    m_testDataPath = getTestDataPath();
    QDir().mkpath(m_testDataPath);
    
    m_primaryScreen = QApplication::primaryScreen();
    m_devicePixelRatio = m_primaryScreen->devicePixelRatio();
    
    // Initialize FluentQt theme system
    FluentTheme::initialize();
    FluentTheme::setTheme(FluentTheme::Light);
    
    // Create test widget
    m_testWidget = new QWidget();
    m_testWidget->setFixedSize(400, 300);
    setupTestWidget(m_testWidget);
}

void FluentVisualRegressionTest::cleanupTestCase() {
    delete m_testWidget;
}

void FluentVisualRegressionTest::init() {
    // Reset theme to default before each test
    FluentTheme::setTheme(FluentTheme::Light);
    m_testWidget->setStyleSheet("");
}

void FluentVisualRegressionTest::cleanup() {
    // Clean up any child widgets
    qDeleteAll(m_testWidget->findChildren<QWidget*>());
}

void FluentVisualRegressionTest::testFluentButtonVisual() {
    // Test basic button appearance
    auto* button = new FluentButton("Test Button", m_testWidget);
    button->setGeometry(50, 50, 120, 32);
    
    QPixmap actual = captureWidget(m_testWidget, "button_basic");
    QPixmap expected = loadReferenceImage("button_basic");
    
    if (expected.isNull()) {
        // First run - save reference image
        saveReferenceImage(actual, "button_basic");
        QSKIP("Reference image saved for button_basic");
    }
    
    QVERIFY2(comparePixmaps(actual, expected), "Button visual regression detected");
}

void FluentVisualRegressionTest::testFluentButtonStates() {
    auto* button = new FluentButton("Hover Button", m_testWidget);
    button->setGeometry(50, 50, 120, 32);
    
    // Test normal state
    QPixmap normalState = captureWidget(m_testWidget, "button_normal");
    
    // Test hover state
    QHoverEvent hoverEnter(QEvent::HoverEnter, QPointF(60, 60), QPointF(60, 60));
    QApplication::sendEvent(button, &hoverEnter);
    button->update();
    QTest::qWait(100); // Allow animation to complete
    
    QPixmap hoverState = captureWidget(m_testWidget, "button_hover");
    
    // Test pressed state
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(60, 60), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(button, &pressEvent);
    button->update();
    QTest::qWait(50);
    
    QPixmap pressedState = captureWidget(m_testWidget, "button_pressed");
    
    // Verify states are different
    QVERIFY2(!comparePixmaps(normalState, hoverState, 0.99), "Button hover state should be visually different");
    QVERIFY2(!comparePixmaps(hoverState, pressedState, 0.99), "Button pressed state should be visually different");
    
    // Compare with reference images
    QPixmap expectedNormal = loadReferenceImage("button_normal");
    QPixmap expectedHover = loadReferenceImage("button_hover");
    QPixmap expectedPressed = loadReferenceImage("button_pressed");
    
    if (expectedNormal.isNull()) {
        saveReferenceImage(normalState, "button_normal");
        saveReferenceImage(hoverState, "button_hover");
        saveReferenceImage(pressedState, "button_pressed");
        QSKIP("Reference images saved for button states");
    }
    
    QVERIFY2(comparePixmaps(normalState, expectedNormal), "Button normal state visual regression");
    QVERIFY2(comparePixmaps(hoverState, expectedHover), "Button hover state visual regression");
    QVERIFY2(comparePixmaps(pressedState, expectedPressed), "Button pressed state visual regression");
}

void FluentVisualRegressionTest::testFluentTextInputVisual() {
    auto* textInput = new FluentTextInput(m_testWidget);
    textInput->setGeometry(50, 50, 200, 32);
    textInput->setPlaceholderText("Enter text...");
    
    // Test empty state
    QPixmap emptyState = captureWidget(m_testWidget, "textinput_empty");
    
    // Test with text
    textInput->setText("Sample text");
    QPixmap withTextState = captureWidget(m_testWidget, "textinput_text");
    
    // Test focused state
    textInput->setFocus();
    QPixmap focusedState = captureWidget(m_testWidget, "textinput_focused");
    
    // Compare with references
    QPixmap expectedEmpty = loadReferenceImage("textinput_empty");
    QPixmap expectedText = loadReferenceImage("textinput_text");
    QPixmap expectedFocused = loadReferenceImage("textinput_focused");
    
    if (expectedEmpty.isNull()) {
        saveReferenceImage(emptyState, "textinput_empty");
        saveReferenceImage(withTextState, "textinput_text");
        saveReferenceImage(focusedState, "textinput_focused");
        QSKIP("Reference images saved for text input states");
    }
    
    QVERIFY2(comparePixmaps(emptyState, expectedEmpty), "TextInput empty state visual regression");
    QVERIFY2(comparePixmaps(withTextState, expectedText), "TextInput with text visual regression");
    QVERIFY2(comparePixmaps(focusedState, expectedFocused), "TextInput focused state visual regression");
}

void FluentVisualRegressionTest::testFluentCardVisual() {
    auto* card = new FluentCard(m_testWidget);
    card->setGeometry(20, 20, 200, 150);
    card->setTitle("Test Card");
    card->setContent("This is a test card with some content to verify visual appearance.");
    
    QPixmap actual = captureWidget(m_testWidget, "card_basic");
    QPixmap expected = loadReferenceImage("card_basic");
    
    if (expected.isNull()) {
        saveReferenceImage(actual, "card_basic");
        QSKIP("Reference image saved for card_basic");
    }
    
    QVERIFY2(comparePixmaps(actual, expected), "Card visual regression detected");
}

void FluentVisualRegressionTest::testLightThemeVisual() {
    FluentTheme::setTheme(FluentTheme::Light);
    
    // Create a layout with multiple components
    auto* button = new FluentButton("Light Theme", m_testWidget);
    auto* textInput = new FluentTextInput(m_testWidget);
    auto* card = new FluentCard(m_testWidget);
    
    button->setGeometry(20, 20, 100, 32);
    textInput->setGeometry(20, 60, 150, 32);
    card->setGeometry(20, 100, 180, 100);
    
    textInput->setPlaceholderText("Light theme input");
    card->setTitle("Light Card");
    card->setContent("Light theme content");
    
    QPixmap actual = captureWidget(m_testWidget, "theme_light");
    QPixmap expected = loadReferenceImage("theme_light");
    
    if (expected.isNull()) {
        saveReferenceImage(actual, "theme_light");
        QSKIP("Reference image saved for light theme");
    }
    
    QVERIFY2(comparePixmaps(actual, expected), "Light theme visual regression detected");
}

void FluentVisualRegressionTest::testDarkThemeVisual() {
    FluentTheme::setTheme(FluentTheme::Dark);
    
    // Create the same layout as light theme test
    auto* button = new FluentButton("Dark Theme", m_testWidget);
    auto* textInput = new FluentTextInput(m_testWidget);
    auto* card = new FluentCard(m_testWidget);
    
    button->setGeometry(20, 20, 100, 32);
    textInput->setGeometry(20, 60, 150, 32);
    card->setGeometry(20, 100, 180, 100);
    
    textInput->setPlaceholderText("Dark theme input");
    card->setTitle("Dark Card");
    card->setContent("Dark theme content");
    
    QPixmap actual = captureWidget(m_testWidget, "theme_dark");
    QPixmap expected = loadReferenceImage("theme_dark");
    
    if (expected.isNull()) {
        saveReferenceImage(actual, "theme_dark");
        QSKIP("Reference image saved for dark theme");
    }
    
    QVERIFY2(comparePixmaps(actual, expected), "Dark theme visual regression detected");
}

void FluentVisualRegressionTest::testComplexLayoutVisual() {
    // Create a complex layout with nested components
    auto* mainCard = new FluentCard(m_testWidget);
    mainCard->setGeometry(10, 10, 380, 280);
    mainCard->setTitle("Complex Layout Test");
    
    auto* button1 = new FluentButton("Button 1", mainCard);
    auto* button2 = new FluentButton("Button 2", mainCard);
    auto* textInput = new FluentTextInput(mainCard);
    auto* progressBar = new FluentProgressBar(mainCard);
    
    button1->setGeometry(20, 40, 80, 32);
    button2->setGeometry(110, 40, 80, 32);
    textInput->setGeometry(20, 80, 200, 32);
    progressBar->setGeometry(20, 120, 200, 8);
    
    textInput->setText("Complex layout input");
    progressBar->setValue(65);
    
    QPixmap actual = captureWidget(m_testWidget, "layout_complex");
    QPixmap expected = loadReferenceImage("layout_complex");
    
    if (expected.isNull()) {
        saveReferenceImage(actual, "layout_complex");
        QSKIP("Reference image saved for complex layout");
    }
    
    QVERIFY2(comparePixmaps(actual, expected), "Complex layout visual regression detected");
}

QPixmap FluentVisualRegressionTest::captureWidget(QWidget* widget, const QString& testName) {
    Q_UNUSED(testName)
    
    // Ensure widget is visible and updated
    widget->show();
    QTest::qWaitForWindowExposed(widget);
    widget->repaint();
    QApplication::processEvents();
    
    // Capture the widget
    QPixmap pixmap = widget->grab();
    
    // Scale for device pixel ratio consistency
    if (m_devicePixelRatio > 1.0) {
        pixmap.setDevicePixelRatio(m_devicePixelRatio);
    }
    
    return pixmap;
}

bool FluentVisualRegressionTest::comparePixmaps(const QPixmap& actual, const QPixmap& expected, double threshold) {
    if (actual.size() != expected.size()) {
        return false;
    }
    
    QImage actualImage = actual.toImage();
    QImage expectedImage = expected.toImage();
    
    if (actualImage.format() != expectedImage.format()) {
        actualImage = actualImage.convertToFormat(QImage::Format_ARGB32);
        expectedImage = expectedImage.convertToFormat(QImage::Format_ARGB32);
    }
    
    int totalPixels = actualImage.width() * actualImage.height();
    int matchingPixels = 0;
    
    for (int y = 0; y < actualImage.height(); ++y) {
        for (int x = 0; x < actualImage.width(); ++x) {
            QRgb actualPixel = actualImage.pixel(x, y);
            QRgb expectedPixel = expectedImage.pixel(x, y);
            
            // Allow small differences in color values (anti-aliasing, etc.)
            int rDiff = qAbs(qRed(actualPixel) - qRed(expectedPixel));
            int gDiff = qAbs(qGreen(actualPixel) - qGreen(expectedPixel));
            int bDiff = qAbs(qBlue(actualPixel) - qBlue(expectedPixel));
            int aDiff = qAbs(qAlpha(actualPixel) - qAlpha(expectedPixel));
            
            if (rDiff <= 2 && gDiff <= 2 && bDiff <= 2 && aDiff <= 2) {
                matchingPixels++;
            }
        }
    }
    
    double similarity = static_cast<double>(matchingPixels) / totalPixels;
    return similarity >= threshold;
}

void FluentVisualRegressionTest::saveReferenceImage(const QPixmap& pixmap, const QString& testName) {
    QString filename = QString("%1/%2_reference.png").arg(m_testDataPath, testName);
    pixmap.save(filename, "PNG");
}

QPixmap FluentVisualRegressionTest::loadReferenceImage(const QString& testName) {
    QString filename = QString("%1/%2_reference.png").arg(m_testDataPath, testName);
    return QPixmap(filename);
}

QString FluentVisualRegressionTest::getTestDataPath() const {
    QString path = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    return path + "/FluentQt_VisualTests";
}

void FluentVisualRegressionTest::setupTestWidget(QWidget* widget) {
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->setStyleSheet("background-color: white;");
}

QTEST_MAIN(FluentVisualRegressionTest)
#include "FluentVisualRegressionTest.moc"
