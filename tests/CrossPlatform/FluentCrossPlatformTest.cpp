// tests/CrossPlatform/FluentCrossPlatformTest.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QScreen>
#include <QStyleHints>
#include <QOperatingSystemVersion>
#include <QDir>
#include <QStandardPaths>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Core/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentCrossPlatformTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Platform detection tests
    void testPlatformDetection();
    void testOperatingSystemVersion();
    void testDesktopEnvironment();

    // Display and DPI tests
    void testHighDPISupport();
    void testMultipleScreens();
    void testScreenScaling();
    void testDevicePixelRatio();

    // Font rendering tests
    void testFontRendering();
    void testFontScaling();
    void testSystemFonts();
    void testFontHinting();

    // Theme integration tests
    void testSystemThemeDetection();
    void testDarkModeSupport();
    void testSystemColors();
    void testSystemAccentColors();

    // Input method tests
    void testKeyboardLayouts();
    void testInputMethods();
    void testTouchSupport();
    void testMouseSupport();

    // File system tests
    void testPathHandling();
    void testFilePermissions();
    void testDirectoryStructure();

    // Performance tests
    void testPlatformPerformance();
    void testMemoryUsage();
    void testStartupTime();

    // Accessibility tests
    void testPlatformAccessibility();
    void testScreenReaderSupport();
    void testSystemAccessibilitySettings();

    // Localization tests
    void testSystemLocale();
    void testTextDirection();
    void testNumberFormatting();
    void testDateTimeFormatting();

private:
    QString getPlatformName() const;
    QString getDesktopEnvironment() const;
    bool isHighDPIDisplay() const;
    void testComponentOnPlatform(QWidget* component, const QString& testName);
    void logPlatformInfo();

private:
    QWidget* m_testWidget;
    QString m_platformName;
    QScreen* m_primaryScreen;
};

void FluentCrossPlatformTest::initTestCase() {
    m_platformName = getPlatformName();
    m_primaryScreen = QApplication::primaryScreen();
    
    logPlatformInfo();
    
    // Create test widget
    m_testWidget = new QWidget();
    m_testWidget->setFixedSize(400, 300);
    
    qDebug() << "Cross-platform testing initialized for:" << m_platformName;
}

void FluentCrossPlatformTest::cleanupTestCase() {
    delete m_testWidget;
}

void FluentCrossPlatformTest::init() {
    // Clean up any child widgets
    qDeleteAll(m_testWidget->findChildren<QWidget*>());
}

void FluentCrossPlatformTest::cleanup() {
    // Reset test widget state
}

void FluentCrossPlatformTest::testPlatformDetection() {
    // Test platform detection
    QString platform = getPlatformName();
    QVERIFY(!platform.isEmpty());
    
    qDebug() << "Detected platform:" << platform;
    
    // Verify platform-specific behavior
#ifdef Q_OS_WIN
    QVERIFY(platform.contains("Windows"));
#elif defined(Q_OS_MACOS)
    QVERIFY(platform.contains("macOS"));
#elif defined(Q_OS_LINUX)
    QVERIFY(platform.contains("Linux"));
#endif
}

void FluentCrossPlatformTest::testOperatingSystemVersion() {
    auto osVersion = QOperatingSystemVersion::current();
    
    qDebug() << "OS Version:" << osVersion.name() 
             << osVersion.majorVersion() 
             << osVersion.minorVersion() 
             << osVersion.microVersion();
    
    QVERIFY(osVersion.majorVersion() > 0);
    
    // Test version-specific features
#ifdef Q_OS_WIN
    if (osVersion >= QOperatingSystemVersion::Windows10) {
        qDebug() << "Windows 10+ features available";
        // Test Windows 10+ specific features
    }
#elif defined(Q_OS_MACOS)
    if (osVersion >= QOperatingSystemVersion::MacOSBigSur) {
        qDebug() << "macOS Big Sur+ features available";
        // Test macOS Big Sur+ specific features
    }
#endif
}

void FluentCrossPlatformTest::testHighDPISupport() {
    qreal devicePixelRatio = m_primaryScreen->devicePixelRatio();
    QSize logicalSize = m_primaryScreen->size();
    QSize physicalSize = m_primaryScreen->geometry().size();
    
    qDebug() << "Device pixel ratio:" << devicePixelRatio;
    qDebug() << "Logical size:" << logicalSize;
    qDebug() << "Physical size:" << physicalSize;
    
    QVERIFY(devicePixelRatio > 0);
    QVERIFY(logicalSize.isValid());
    
    // Test high DPI rendering
    if (devicePixelRatio > 1.0) {
        qDebug() << "High DPI display detected";
        
        // Create components and test scaling
        auto* button = new FluentButton("High DPI Test", m_testWidget);
        testComponentOnPlatform(button, "HighDPI_Button");
        
        // Verify that components scale properly
        QSize buttonSize = button->sizeHint();
        QVERIFY(buttonSize.width() > 0);
        QVERIFY(buttonSize.height() > 0);
    }
}

void FluentCrossPlatformTest::testMultipleScreens() {
    QList<QScreen*> screens = QApplication::screens();
    
    qDebug() << "Number of screens:" << screens.count();
    
    for (int i = 0; i < screens.count(); ++i) {
        QScreen* screen = screens[i];
        qDebug() << "Screen" << i << ":"
                 << "Size:" << screen->size()
                 << "DPI:" << screen->logicalDotsPerInch()
                 << "Pixel ratio:" << screen->devicePixelRatio();
        
        QVERIFY(screen->size().isValid());
        QVERIFY(screen->logicalDotsPerInch() > 0);
    }
    
    // Test multi-screen scenarios if available
    if (screens.count() > 1) {
        // Test moving widgets between screens
        m_testWidget->show();
        QTest::qWaitForWindowExposed(m_testWidget);
        
        // Move to second screen
        QRect secondScreenGeometry = screens[1]->geometry();
        m_testWidget->move(secondScreenGeometry.center());
        
        QTest::qWait(100);
        
        // Verify widget is on correct screen
        QScreen* currentScreen = m_testWidget->screen();
        QCOMPARE(currentScreen, screens[1]);
        
        m_testWidget->hide();
    }
}

void FluentCrossPlatformTest::testFontRendering() {
    // Test font rendering across platforms
    QFont systemFont = QApplication::font();
    
    qDebug() << "System font:" << systemFont.family() 
             << "Size:" << systemFont.pointSize()
             << "Weight:" << systemFont.weight();
    
    QVERIFY(!systemFont.family().isEmpty());
    QVERIFY(systemFont.pointSize() > 0);
    
    // Test FluentQt font handling
    auto* button = new FluentButton("Font Test", m_testWidget);
    QFont buttonFont = button->font();
    
    // Verify font is properly set
    QVERIFY(!buttonFont.family().isEmpty());
    
    // Test font metrics
    QFontMetrics metrics(buttonFont);
    int textWidth = metrics.horizontalAdvance("Font Test");
    int textHeight = metrics.height();
    
    QVERIFY(textWidth > 0);
    QVERIFY(textHeight > 0);
    
    qDebug() << "Button font metrics - Width:" << textWidth << "Height:" << textHeight;
}

void FluentCrossPlatformTest::testSystemThemeDetection() {
    // Test system theme detection
    QPalette systemPalette = QApplication::palette();
    
    qDebug() << "System palette:";
    qDebug() << "  Window:" << systemPalette.color(QPalette::Window).name();
    qDebug() << "  WindowText:" << systemPalette.color(QPalette::WindowText).name();
    qDebug() << "  Base:" << systemPalette.color(QPalette::Base).name();
    qDebug() << "  Text:" << systemPalette.color(QPalette::Text).name();
    
    // Test dark mode detection
    QColor windowColor = systemPalette.color(QPalette::Window);
    bool isDarkTheme = windowColor.lightness() < 128;
    
    qDebug() << "Dark theme detected:" << isDarkTheme;
    
    // Test FluentQt theme adaptation
    if (isDarkTheme) {
        FluentTheme::setTheme(FluentTheme::Dark);
    } else {
        FluentTheme::setTheme(FluentTheme::Light);
    }
    
    // Verify theme is applied
    QVERIFY(FluentTheme::currentTheme() != FluentTheme::System); // Should be set to specific theme
}

void FluentCrossPlatformTest::testKeyboardLayouts() {
    // Test keyboard layout handling
    QLocale systemLocale = QLocale::system();
    
    qDebug() << "System locale:" << systemLocale.name();
    qDebug() << "Language:" << QLocale::languageToString(systemLocale.language());
    qDebug() << "Country:" << QLocale::countryToString(systemLocale.country());
    
    // Test text input with different layouts
    auto* textInput = new FluentTextInput(m_testWidget);
    textInput->setPlaceholderText("Test input");
    
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    textInput->setFocus();
    
    // Test basic text input
    QTest::keyClicks(textInput, "Hello World");
    QCOMPARE(textInput->text(), QString("Hello World"));
    
    // Test special characters (platform-dependent)
    textInput->clear();
    QTest::keyClicks(textInput, "Test@123");
    QCOMPARE(textInput->text(), QString("Test@123"));
    
    m_testWidget->hide();
}

void FluentCrossPlatformTest::testPathHandling() {
    // Test file path handling across platforms
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString testPath = tempPath + QDir::separator() + "FluentQt_Test";
    
    qDebug() << "Temp path:" << tempPath;
    qDebug() << "Test path:" << testPath;
    
    QVERIFY(!tempPath.isEmpty());
    QVERIFY(QDir(tempPath).exists());
    
    // Test directory creation
    QDir testDir;
    bool created = testDir.mkpath(testPath);
    QVERIFY(created);
    QVERIFY(QDir(testPath).exists());
    
    // Test file operations
    QString testFile = testPath + QDir::separator() + "test.txt";
    QFile file(testFile);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream out(&file);
    out << "FluentQt cross-platform test";
    file.close();
    
    QVERIFY(QFile::exists(testFile));
    
    // Clean up
    QFile::remove(testFile);
    testDir.rmdir(testPath);
}

void FluentCrossPlatformTest::testPlatformPerformance() {
    // Test performance characteristics on current platform
    const int componentCount = 100;
    
    QElapsedTimer timer;
    timer.start();
    
    // Create components
    QList<QWidget*> components;
    for (int i = 0; i < componentCount; ++i) {
        auto* button = new FluentButton(QString("Button %1").arg(i), m_testWidget);
        components.append(button);
    }
    
    qint64 creationTime = timer.elapsed();
    
    // Test rendering
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    timer.restart();
    for (auto* component : components) {
        component->update();
    }
    QApplication::processEvents();
    
    qint64 renderTime = timer.elapsed();
    
    m_testWidget->hide();
    
    qDebug() << "Platform performance:";
    qDebug() << "  Creation time:" << creationTime << "ms for" << componentCount << "components";
    qDebug() << "  Render time:" << renderTime << "ms";
    
    // Performance assertions (platform-dependent thresholds)
    QVERIFY2(creationTime < 5000, "Component creation should be reasonably fast");
    QVERIFY2(renderTime < 1000, "Rendering should be reasonably fast");
    
    // Clean up
    qDeleteAll(components);
}

void FluentCrossPlatformTest::testSystemLocale() {
    QLocale locale = QLocale::system();
    
    qDebug() << "System locale tests:";
    qDebug() << "  Locale:" << locale.name();
    qDebug() << "  Decimal point:" << locale.decimalPoint();
    qDebug() << "  Group separator:" << locale.groupSeparator();
    qDebug() << "  Currency symbol:" << locale.currencySymbol();
    
    // Test number formatting
    double testNumber = 1234.56;
    QString formattedNumber = locale.toString(testNumber, 'f', 2);
    qDebug() << "  Formatted number:" << formattedNumber;
    
    QVERIFY(!formattedNumber.isEmpty());
    
    // Test date formatting
    QDate testDate = QDate::currentDate();
    QString formattedDate = locale.toString(testDate, QLocale::ShortFormat);
    qDebug() << "  Formatted date:" << formattedDate;
    
    QVERIFY(!formattedDate.isEmpty());
}

QString FluentCrossPlatformTest::getPlatformName() const {
#ifdef Q_OS_WIN
    return "Windows";
#elif defined(Q_OS_MACOS)
    return "macOS";
#elif defined(Q_OS_LINUX)
    return "Linux";
#elif defined(Q_OS_ANDROID)
    return "Android";
#elif defined(Q_OS_IOS)
    return "iOS";
#else
    return "Unknown";
#endif
}

QString FluentCrossPlatformTest::getDesktopEnvironment() const {
#ifdef Q_OS_LINUX
    QString de = qgetenv("XDG_CURRENT_DESKTOP");
    if (de.isEmpty()) {
        de = qgetenv("DESKTOP_SESSION");
    }
    return de.isEmpty() ? "Unknown" : de;
#else
    return "N/A";
#endif
}

void FluentCrossPlatformTest::testComponentOnPlatform(QWidget* component, const QString& testName) {
    Q_UNUSED(testName)
    
    // Basic component tests
    QVERIFY(component != nullptr);
    QVERIFY(component->sizeHint().isValid());
    
    // Test visibility
    component->show();
    QVERIFY(component->isVisible());
    
    // Test size constraints
    QSize minSize = component->minimumSizeHint();
    QSize maxSize = component->maximumSize();
    
    QVERIFY(minSize.isValid());
    QVERIFY(maxSize.isValid() || maxSize == QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
}

void FluentCrossPlatformTest::logPlatformInfo() {
    qDebug() << "=== Platform Information ===";
    qDebug() << "Platform:" << getPlatformName();
    qDebug() << "OS Version:" << QOperatingSystemVersion::current().name();
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Desktop Environment:" << getDesktopEnvironment();
    qDebug() << "Primary Screen DPI:" << m_primaryScreen->logicalDotsPerInch();
    qDebug() << "Device Pixel Ratio:" << m_primaryScreen->devicePixelRatio();
    qDebug() << "Screen Size:" << m_primaryScreen->size();
    qDebug() << "Available Geometry:" << m_primaryScreen->availableGeometry();
    qDebug() << "System Locale:" << QLocale::system().name();
    qDebug() << "============================";
}

QTEST_MAIN(FluentCrossPlatformTest)
#include "FluentCrossPlatformTest.moc"
