// src/Styling/FluentTheme.cpp
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QApplication>
#include <QSettings>
#include <QTimer>
#include <QStyleHints>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace FluentQt::Styling {

FluentTheme& FluentTheme::instance() {
    static FluentTheme instance;
    return instance;
}

FluentTheme::FluentTheme() {
    initializePalettes();
    
    // Load saved settings
    loadSettings();
    
    // Connect to system theme changes
    connectToSystemTheme();
    
    // Initialize spacing and sizing maps
    initializeSpacingAndSizing();

    // Initialize typography system
    initializeTypography();

    // Initialize border radius system
    initializeBorderRadius();

    // Initialize elevation system
    initializeElevation();

    qDebug() << "FluentTheme initialized with mode:" << static_cast<int>(m_mode);
}

void FluentTheme::setMode(FluentThemeMode mode) {
    if (m_mode != mode) {
        const FluentThemeMode oldMode = m_mode;
        m_mode = mode;
        
        // Update effective mode based on system if needed
        updateEffectiveMode();
        
        // Save setting
        saveSettings();
        
        emit modeChanged(mode);
        emit themeChanged();
        
        qDebug() << "Theme mode changed from" << static_cast<int>(oldMode) 
                 << "to" << static_cast<int>(mode);
    }
}

void FluentTheme::setAccentColor(FluentAccentColor color) {
    if (m_accentColor != color) {
        m_accentColor = color;
        
        // Regenerate palettes with new accent color
        updateAccentColors();
        
        // Save setting
        saveSettings();
        
        emit accentColorChanged(color);
        emit themeChanged();
        
        qDebug() << "Accent color changed to" << static_cast<int>(color);
    }
}

const FluentColorPalette& FluentTheme::currentPalette() const {
    // Handle system mode
    if (m_mode == FluentThemeMode::System) {
        return isSystemDarkMode() ? m_darkPalette : m_lightPalette;
    }
    
    return (m_mode == FluentThemeMode::Dark) ? m_darkPalette : m_lightPalette;
}

QColor FluentTheme::color(std::string_view colorName) const {
    FLUENT_PROFILE("FluentTheme::color");
    
    const auto& palette = currentPalette();
    
    // Map color names to palette colors
    static const std::unordered_map<std::string_view, std::function<QColor(const FluentColorPalette&)>> colorMap = {
        // Accent colors
        {"accent", [](const auto& p) { return p.accent; }},
        {"accentLight1", [](const auto& p) { return p.accentLight1; }},
        {"accentLight2", [](const auto& p) { return p.accentLight2; }},
        {"accentLight3", [](const auto& p) { return p.accentLight3; }},
        {"accentDark1", [](const auto& p) { return p.accentDark1; }},
        {"accentDark2", [](const auto& p) { return p.accentDark2; }},
        {"accentDark3", [](const auto& p) { return p.accentDark3; }},
        
        // Neutral colors
        {"neutralPrimary", [](const auto& p) { return p.neutralPrimary; }},
        {"neutralSecondary", [](const auto& p) { return p.neutralSecondary; }},
        {"neutralTertiary", [](const auto& p) { return p.neutralTertiary; }},
        {"neutralQuaternary", [](const auto& p) { return p.neutralQuaternary; }},
        {"neutralLight", [](const auto& p) { return p.neutralLight; }},
        {"neutralLighter", [](const auto& p) { return p.neutralLighter; }},
        {"neutralLightest", [](const auto& p) { return p.neutralLightest; }},
        {"neutralDark", [](const auto& p) { return p.neutralDark; }},
        {"neutralDarker", [](const auto& p) { return p.neutralDarker; }},
        {"neutralDarkest", [](const auto& p) { return p.neutralDarkest; }},
        
        // Semantic colors
        {"error", [](const auto& p) { return p.error; }},
        {"errorLight", [](const auto& p) { return p.errorLight; }},
        {"warning", [](const auto& p) { return p.warning; }},
        {"warningLight", [](const auto& p) { return p.warningLight; }},
        {"success", [](const auto& p) { return p.success; }},
        {"successLight", [](const auto& p) { return p.successLight; }},
        {"info", [](const auto& p) { return p.info; }},
        {"infoLight", [](const auto& p) { return p.infoLight; }},
        
        // Surface colors
        {"surface", [](const auto& p) { return p.neutralLightest; }},
        {"surfaceSecondary", [](const auto& p) { return p.neutralLighter; }},
        {"surfaceTertiary", [](const auto& p) { return p.neutralLight; }},
        
        // Text colors
        {"textPrimary", [](const auto& p) { return p.neutralPrimary; }},
        {"textSecondary", [](const auto& p) { return p.neutralSecondary; }},
        {"textTertiary", [](const auto& p) { return p.neutralTertiary; }},
        {"textDisabled", [](const auto& p) { return p.neutralTertiaryAlt; }},
        
        // Border colors
        {"border", [](const auto& p) { return p.neutralQuaternaryAlt; }},
        {"borderStrong", [](const auto& p) { return p.neutralQuaternary; }},
        {"borderSubtle", [](const auto& p) { return p.neutralTertiaryAlt; }}
    };
    
    const auto it = colorMap.find(colorName);
    if (it != colorMap.end()) {
        return it->second(palette);
    }
    
    qWarning() << "Unknown color name:" << colorName.data();
    return palette.neutralPrimary; // Fallback
}

QBrush FluentTheme::brush(std::string_view brushName) const {
    const QColor brushColor = color(brushName);
    
    // Create special brushes for certain names
    if (brushName == "acrylic") {
        return createAcrylicBrush(brushColor);
    } else if (brushName == "mica") {
        return createMicaBrush();
    } else if (brushName.ends_with("Gradient")) {
        return createGradientBrush(brushColor);
    }
    
    return QBrush(brushColor);
}

QFont FluentTheme::displayFont() const {
    const auto it = m_fontMap.find("display");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::titleLargeFont() const {
    const auto it = m_fontMap.find("titleLarge");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::titleFont() const {
    const auto it = m_fontMap.find("title");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::titleSmallFont() const {
    const auto it = m_fontMap.find("titleSmall");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::subtitleFont() const {
    const auto it = m_fontMap.find("subtitle");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::bodyLargeFont() const {
    const auto it = m_fontMap.find("bodyLarge");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::bodyFont() const {
    const auto it = m_fontMap.find("body");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::bodySmallFont() const {
    const auto it = m_fontMap.find("bodySmall");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::captionFont() const {
    const auto it = m_fontMap.find("caption");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::overlineFont() const {
    const auto it = m_fontMap.find("overline");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

QFont FluentTheme::headingFont(int level) const {
    const QString key = QString("heading%1").arg(qBound(1, level, 6));
    const auto it = m_fontMap.find(key.toStdString());
    return (it != m_fontMap.end()) ? it->second : titleFont();
}

QFont FluentTheme::buttonFont() const {
    const auto it = m_fontMap.find("button");
    return (it != m_fontMap.end()) ? it->second : bodyFont();
}

QFont FluentTheme::linkFont() const {
    const auto it = m_fontMap.find("link");
    return (it != m_fontMap.end()) ? it->second : bodyFont();
}

QFont FluentTheme::codeFont() const {
    const auto it = m_fontMap.find("code");
    return (it != m_fontMap.end()) ? it->second : QApplication::font();
}

int FluentTheme::spacing(std::string_view size) const {
    const auto it = m_spacingMap.find(std::string(size));
    if (it != m_spacingMap.end()) {
        return it->second;
    }
    
    qWarning() << "Unknown spacing size:" << size.data();
    return 8; // Default spacing
}

QSize FluentTheme::iconSize(std::string_view size) const {
    const auto it = m_iconSizeMap.find(std::string(size));
    if (it != m_iconSizeMap.end()) {
        return it->second;
    }

    qWarning() << "Unknown icon size:" << size.data();
    return QSize(16, 16); // Default icon size
}

int FluentTheme::borderRadius(std::string_view size) const {
    const auto it = m_borderRadiusMap.find(std::string(size));
    if (it != m_borderRadiusMap.end()) {
        return it->second;
    }

    qWarning() << "Unknown border radius size:" << size.data();
    return 4; // Default border radius
}

int FluentTheme::elevation(std::string_view level) const {
    const auto it = m_elevationMap.find(std::string(level));
    if (it != m_elevationMap.end()) {
        return it->second;
    }

    qWarning() << "Unknown elevation level:" << level.data();
    return 2; // Default elevation
}

QMargins FluentTheme::margins(std::string_view size) const {
    const auto it = m_marginsMap.find(std::string(size));
    if (it != m_marginsMap.end()) {
        return it->second;
    }

    qWarning() << "Unknown margins size:" << size.data();
    return QMargins(8, 8, 8, 8); // Default margins
}

QMargins FluentTheme::padding(std::string_view size) const {
    const auto it = m_paddingMap.find(std::string(size));
    if (it != m_paddingMap.end()) {
        return it->second;
    }

    qWarning() << "Unknown padding size:" << size.data();
    return QMargins(8, 8, 8, 8); // Default padding
}

int FluentTheme::componentHeight(std::string_view size) const {
    const auto it = m_componentHeightMap.find(std::string(size));
    if (it != m_componentHeightMap.end()) {
        return it->second;
    }

    qWarning() << "Unknown component height size:" << size.data();
    return 32; // Default component height
}

int FluentTheme::componentWidth(std::string_view size) const {
    const auto it = m_componentWidthMap.find(std::string(size));
    if (it != m_componentWidthMap.end()) {
        return it->second;
    }

    qWarning() << "Unknown component width size:" << size.data();
    return 120; // Default component width
}

int FluentTheme::strokeWidth(std::string_view weight) const {
    const auto it = m_strokeWidthMap.find(std::string(weight));
    if (it != m_strokeWidthMap.end()) {
        return it->second;
    }

    qWarning() << "Unknown stroke width weight:" << weight.data();
    return 1; // Default stroke width
}

void FluentTheme::initializePalettes() {
    FLUENT_PROFILE("FluentTheme::initializePalettes");
    
    // Initialize light palette
    m_lightPalette = FluentColorPalette{
        // Accent colors (Blue by default)
        .accent = QColor("#0078d4"),
        .accentLight1 = QColor("#106ebe"),
        .accentLight2 = QColor("#2b88d8"),
        .accentLight3 = QColor("#71afe5"),
        .accentDark1 = QColor("#005a9e"),
        .accentDark2 = QColor("#004578"),
        .accentDark3 = QColor("#003966"),
        
        // Neutral colors
        .neutralLightest = QColor("#ffffff"),
        .neutralLighter = QColor("#f3f2f1"),
        .neutralLight = QColor("#edebe9"),
        .neutralQuaternaryAlt = QColor("#e1dfdd"),
        .neutralQuaternary = QColor("#d2d0ce"),
        .neutralTertiaryAlt = QColor("#c8c6c4"),
        .neutralTertiary = QColor("#a19f9d"),
        .neutralSecondaryAlt = QColor("#8a8886"),
        .neutralSecondary = QColor("#605e5c"),
        .neutralPrimaryAlt = QColor("#3b3a39"),
        .neutralPrimary = QColor("#323130"),
        .neutralDark = QColor("#201f1e"),
        .neutralDarker = QColor("#106ebe"),
        .neutralDarkest = QColor("#11100f"),
        
        // Semantic colors
        .error = QColor("#d13438"),
        .errorLight = QColor("#f3d6d7"),
        .warning = QColor("#ff8c00"),
        .warningLight = QColor("#fff4ce"),
        .success = QColor("#107c10"),
        .successLight = QColor("#dff6dd"),
        .info = QColor("#0078d4"),
        .infoLight = QColor("#deecf9")
    };
    
    // Initialize dark palette
    m_darkPalette = FluentColorPalette{
        // Accent colors (Blue by default)
        .accent = QColor("#60cdff"),
        .accentLight1 = QColor("#4fc3f7"),
        .accentLight2 = QColor("#29b6f6"),
        .accentLight3 = QColor("#039be5"),
        .accentDark1 = QColor("#0086c7"),
        .accentDark2 = QColor("#005ba1"),
        .accentDark3 = QColor("#004578"),
        
        // Neutral colors (inverted from light)
        .neutralLightest = QColor("#1f1f1f"),
        .neutralLighter = QColor("#2d2d2d"),
        .neutralLight = QColor("#3d3d3d"),
        .neutralQuaternaryAlt = QColor("#484644"),
        .neutralQuaternary = QColor("#605e5c"),
        .neutralTertiaryAlt = QColor("#8a8886"),
        .neutralTertiary = QColor("#a19f9d"),
        .neutralSecondaryAlt = QColor("#c8c6c4"),
        .neutralSecondary = QColor("#d2d0ce"),
        .neutralPrimaryAlt = QColor("#e1dfdd"),
        .neutralPrimary = QColor("#ffffff"),
        .neutralDark = QColor("#f3f2f1"),
        .neutralDarker = QColor("#faf9f8"),
        .neutralDarkest = QColor("#fdfdfc"),
        
        // Semantic colors (adjusted for dark theme)
        .error = QColor("#ff6b70"),
        .errorLight = QColor("#442726"),
        .warning = QColor("#ffaa44"),
        .warningLight = QColor("#433519"),
        .success = QColor("#6bb700"),
        .successLight = QColor("#393d1b"),
        .info = QColor("#60cdff"),
        .infoLight = QColor("#203047")
    };
    
    // Update accent colors based on current setting
    updateAccentColors();
}

void FluentTheme::updateAccentColors() {
    FLUENT_PROFILE("FluentTheme::updateAccentColors");
    
    // Base colors for different accent options
    struct AccentColorSet {
        QColor base;
        QColor light1, light2, light3;
        QColor dark1, dark2, dark3;
    };
    
    static const std::unordered_map<FluentAccentColor, AccentColorSet> accentColors = {
        {FluentAccentColor::Blue, {
            QColor("#0078d4"),
            QColor("#106ebe"), QColor("#2b88d8"), QColor("#71afe5"),
            QColor("#005a9e"), QColor("#004578"), QColor("#003966")
        }},
        {FluentAccentColor::Purple, {
            QColor("#8764b8"),
            QColor("#9373c0"), QColor("#b392ca"), QColor("#d7c5ec"),
            QColor("#744da9"), QColor("#5c2e91"), QColor("#401b6c")
        }},
        {FluentAccentColor::Pink, {
            QColor("#e3008c"),
            QColor("#e61e99"), QColor("#ea4ead"), QColor("#f389d2"),
            QColor("#c239b3"), QColor("#9c2e91"), QColor("#77086f")
        }},
        {FluentAccentColor::Red, {
            QColor("#d13438"),
            QColor("#d7494c"), QColor("#dc5e62"), QColor("#e8979a"),
            QColor("#b01e24"), QColor("#8e1317"), QColor("#6e0811")
        }},
        {FluentAccentColor::Orange, {
            QColor("#ff8c00"),
            QColor("#ff9a1a"), QColor("#ffaa3d"), QColor("#ffc970"),
            QColor("#d97700"), QColor("#b25d00"), QColor("#8a4400")
        }},
        {FluentAccentColor::Yellow, {
            QColor("#ffb900"),
            QColor("#ffc41a"), QColor("#ffd03d"), QColor("#ffe270"),
            QColor("#d9a200"), QColor("#b28500"), QColor("#8a6600")
        }},
        {FluentAccentColor::Green, {
            QColor("#107c10"),
            QColor("#218c21"), QColor("#359b35"), QColor("#6bb700"),
            QColor("#0e6e0e"), QColor("#0b5a0b"), QColor("#084508")
        }},
        {FluentAccentColor::Teal, {
            QColor("#008272"),
            QColor("#1a9586"), QColor("#3da89e"), QColor("#6dc4ba"),
            QColor("#00685c"), QColor("#004d42"), QColor("#003328")
        }}
    };
    
    const auto it = accentColors.find(m_accentColor);
    if (it != accentColors.end()) {
        const AccentColorSet& colorSet = it->second;
        
        // Update light palette
        m_lightPalette.accent = colorSet.base;
        m_lightPalette.accentLight1 = colorSet.light1;
        m_lightPalette.accentLight2 = colorSet.light2;
        m_lightPalette.accentLight3 = colorSet.light3;
        m_lightPalette.accentDark1 = colorSet.dark1;
        m_lightPalette.accentDark2 = colorSet.dark2;
        m_lightPalette.accentDark3 = colorSet.dark3;
        
        // For dark palette, use lighter variants as base
        m_darkPalette.accent = colorSet.light2;
        m_darkPalette.accentLight1 = colorSet.light1;
        m_darkPalette.accentLight2 = colorSet.base;
        m_darkPalette.accentLight3 = colorSet.dark1;
        m_darkPalette.accentDark1 = colorSet.dark1;
        m_darkPalette.accentDark2 = colorSet.dark2;
        m_darkPalette.accentDark3 = colorSet.dark3;
        
        // Update info colors to match accent
        m_lightPalette.info = colorSet.base;
        m_lightPalette.infoLight = colorSet.light3;
        m_darkPalette.info = colorSet.light2;
        m_darkPalette.infoLight = colorSet.dark3;
    }
}

void FluentTheme::initializeSpacingAndSizing() {
    // Fluent Design spacing scale
    m_spacingMap = {
        {"none", 0},
        {"xs", 2},
        {"s", 4},
        {"m", 8},
        {"l", 12},
        {"xl", 16},
        {"xxl", 20},
        {"xxxl", 24},
        {"huge", 32},
        
        // Named spacing
        {"compact", 4},
        {"default", 8},
        {"comfortable", 12},
        {"spacious", 16}
    };
    
    // Fluent Design icon sizes
    m_iconSizeMap = {
        {"tiny", QSize(10, 10)},
        {"small", QSize(12, 12)},
        {"medium", QSize(16, 16)},
        {"large", QSize(20, 20)},
        {"xlarge", QSize(24, 24)},
        {"xxlarge", QSize(32, 32)},
        {"huge", QSize(48, 48)},
        
        // Context-specific sizes
        {"button", QSize(16, 16)},
        {"toolbar", QSize(16, 16)},
        {"menu", QSize(16, 16)},
        {"listitem", QSize(20, 20)},
        {"header", QSize(24, 24)}
    };
}

void FluentTheme::initializeTypography() {
    QFont baseFont = QApplication::font();

    // Microsoft Fluent Design typography scale
    m_fontMap = {
        // Display fonts
        {"display", [&]() {
            QFont font = baseFont;
            font.setPointSize(68);
            font.setWeight(QFont::Light);
            return font;
        }()},

        // Title fonts
        {"titleLarge", [&]() {
            QFont font = baseFont;
            font.setPointSize(40);
            font.setWeight(QFont::DemiBold);
            return font;
        }()},

        {"title", [&]() {
            QFont font = baseFont;
            font.setPointSize(28);
            font.setWeight(QFont::DemiBold);
            return font;
        }()},

        {"titleSmall", [&]() {
            QFont font = baseFont;
            font.setPointSize(20);
            font.setWeight(QFont::DemiBold);
            return font;
        }()},

        // Subtitle
        {"subtitle", [&]() {
            QFont font = baseFont;
            font.setPointSize(16);
            font.setWeight(QFont::DemiBold);
            return font;
        }()},

        // Body fonts
        {"bodyLarge", [&]() {
            QFont font = baseFont;
            font.setPointSize(18);
            font.setWeight(QFont::Normal);
            return font;
        }()},

        {"body", [&]() {
            QFont font = baseFont;
            font.setPointSize(14);
            font.setWeight(QFont::Normal);
            return font;
        }()},

        {"bodySmall", [&]() {
            QFont font = baseFont;
            font.setPointSize(12);
            font.setWeight(QFont::Normal);
            return font;
        }()},

        // Caption and overline
        {"caption", [&]() {
            QFont font = baseFont;
            font.setPointSize(12);
            font.setWeight(QFont::Normal);
            return font;
        }()},

        {"overline", [&]() {
            QFont font = baseFont;
            font.setPointSize(12);
            font.setWeight(QFont::DemiBold);
            font.setCapitalization(QFont::AllUppercase);
            return font;
        }()},

        // Semantic fonts
        {"button", [&]() {
            QFont font = baseFont;
            font.setPointSize(14);
            font.setWeight(QFont::Medium);
            return font;
        }()},

        {"link", [&]() {
            QFont font = baseFont;
            font.setPointSize(14);
            font.setWeight(QFont::Normal);
            font.setUnderline(true);
            return font;
        }()},

        {"code", [&]() {
            QFont font("Consolas", 14);
            if (!font.exactMatch()) {
                font = QFont("Courier New", 14);
            }
            return font;
        }()}
    };

    // Add heading fonts (H1-H6)
    const std::vector<std::pair<int, int>> headingSizes = {
        {1, 32}, {2, 28}, {3, 24}, {4, 20}, {5, 18}, {6, 16}
    };

    for (const auto& [level, size] : headingSizes) {
        QFont headingFont = baseFont;
        headingFont.setPointSize(size);
        headingFont.setWeight(QFont::DemiBold);
        m_fontMap[QString("heading%1").arg(level).toStdString()] = headingFont;
    }
}

void FluentTheme::initializeBorderRadius() {
    // Fluent Design border radius scale
    m_borderRadiusMap = {
        {"none", 0},
        {"small", 2},
        {"medium", 4},
        {"large", 8},
        {"xlarge", 12},
        {"circular", 9999}, // Fully rounded

        // Component-specific radii
        {"button", 4},
        {"card", 8},
        {"dialog", 8},
        {"input", 4},
        {"tooltip", 4}
    };

    // Initialize margins and padding maps
    m_marginsMap = {
        {"none", QMargins(0, 0, 0, 0)},
        {"xs", QMargins(2, 2, 2, 2)},
        {"small", QMargins(4, 4, 4, 4)},
        {"medium", QMargins(8, 8, 8, 8)},
        {"large", QMargins(12, 12, 12, 12)},
        {"xlarge", QMargins(16, 16, 16, 16)},
        {"xxlarge", QMargins(20, 20, 20, 20)},

        // Asymmetric margins
        {"compact", QMargins(4, 2, 4, 2)},
        {"comfortable", QMargins(8, 4, 8, 4)},
        {"spacious", QMargins(12, 8, 12, 8)}
    };

    m_paddingMap = {
        {"none", QMargins(0, 0, 0, 0)},
        {"xs", QMargins(2, 2, 2, 2)},
        {"small", QMargins(4, 4, 4, 4)},
        {"medium", QMargins(8, 8, 8, 8)},
        {"large", QMargins(12, 12, 12, 12)},
        {"xlarge", QMargins(16, 16, 16, 16)},
        {"xxlarge", QMargins(20, 20, 20, 20)},

        // Component-specific padding
        {"button", QMargins(12, 6, 12, 6)},
        {"input", QMargins(8, 6, 8, 6)},
        {"card", QMargins(16, 16, 16, 16)},
        {"dialog", QMargins(24, 24, 24, 24)}
    };

    // Component dimensions
    m_componentHeightMap = {
        {"compact", 24},
        {"small", 28},
        {"medium", 32},
        {"large", 40},
        {"xlarge", 48},

        // Component-specific heights
        {"button", 32},
        {"input", 32},
        {"combobox", 32},
        {"slider", 32},
        {"listitem", 40},
        {"menuitem", 32},
        {"toolbar", 40}
    };

    m_componentWidthMap = {
        {"small", 80},
        {"medium", 120},
        {"large", 160},
        {"xlarge", 200},
        {"xxlarge", 240},

        // Component-specific widths
        {"button", 120},
        {"input", 200},
        {"combobox", 160},
        {"dialog", 400},
        {"sidebar", 280}
    };

    m_strokeWidthMap = {
        {"hairline", 1},
        {"thin", 1},
        {"medium", 2},
        {"thick", 3},
        {"bold", 4},

        // Component-specific strokes
        {"border", 1},
        {"focus", 2},
        {"divider", 1}
    };
}

void FluentTheme::initializeElevation() {
    // Fluent Design elevation system (shadow blur radius)
    m_elevationMap = {
        {"none", 0},
        {"low", 2},
        {"medium", 4},
        {"high", 8},
        {"highest", 16},

        // Component-specific elevations
        {"tooltip", 8},
        {"dropdown", 8},
        {"dialog", 16},
        {"flyout", 8},
        {"card", 2},
        {"button", 2},
        {"menu", 8}
    };
}

void FluentTheme::loadSettings() {
    QSettings settings("FluentQt", "Theme");
    
    // Load theme mode
    const int modeInt = settings.value("mode", static_cast<int>(FluentThemeMode::System)).toInt();
    m_mode = static_cast<FluentThemeMode>(modeInt);
    
    // Load accent color
    const int accentInt = settings.value("accentColor", static_cast<int>(FluentAccentColor::Blue)).toInt();
    m_accentColor = static_cast<FluentAccentColor>(accentInt);
    
    // Load custom colors if any
    loadCustomColors(settings);
}

void FluentTheme::saveSettings() {
    QSettings settings("FluentQt", "Theme");
    
    settings.setValue("mode", static_cast<int>(m_mode));
    settings.setValue("accentColor", static_cast<int>(m_accentColor));
    
    // Save custom colors if any
    saveCustomColors(settings);
    
    settings.sync();
}

void FluentTheme::loadCustomColors(QSettings& settings) {
    settings.beginGroup("CustomColors");
    
    // Load any custom color overrides
    const QStringList keys = settings.childKeys();
    for (const QString& key : keys) {
        const QColor color = settings.value(key).value<QColor>();
        if (color.isValid()) {
            // Store custom colors for later use
            m_customColors[key.toStdString()] = color;
        }
    }
    
    settings.endGroup();
}

void FluentTheme::saveCustomColors(QSettings& settings) {
    settings.beginGroup("CustomColors");
    
    for (const auto& [key, color] : m_customColors) {
        settings.setValue(QString::fromStdString(key), color);
    }
    
    settings.endGroup();
}

void FluentTheme::connectToSystemTheme() {
    // Connect to Qt's style hints for system theme changes
    connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged,
            this, &FluentTheme::onSystemThemeChanged);
    
#ifdef Q_OS_WIN
    // On Windows, also monitor for accent color changes
    m_systemThemeTimer = new QTimer(this);
    m_systemThemeTimer->setInterval(1000); // Check every second
    connect(m_systemThemeTimer, &QTimer::timeout,
            this, &FluentTheme::checkWindowsThemeChanges);
    m_systemThemeTimer->start();
#endif

    // Initial update
    updateEffectiveMode();
}

void FluentTheme::onSystemThemeChanged(Qt::ColorScheme colorScheme) {
    Q_UNUSED(colorScheme)
    
    if (m_mode == FluentThemeMode::System) {
        updateEffectiveMode();
        emit themeChanged();
    }
}

bool FluentTheme::isSystemDarkMode() const {
    // Check Qt's color scheme first
    const Qt::ColorScheme scheme = QApplication::styleHints()->colorScheme();
    if (scheme == Qt::ColorScheme::Dark) {
        return true;
    } else if (scheme == Qt::ColorScheme::Light) {
        return false;
    }
    
    // Fallback to platform-specific detection
#ifdef Q_OS_WIN
    return isWindowsDarkMode();
#elif defined(Q_OS_MAC)
    return isMacOSDarkMode();
#else
    // Linux/Unix fallback - check environment variables
    const QString theme = qgetenv("GTK_THEME");
    return theme.contains("dark", Qt::CaseInsensitive);
#endif
}

#ifdef Q_OS_WIN
bool FluentTheme::isWindowsDarkMode() const {
    QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 
                      QSettings::NativeFormat);
    return registry.value("AppsUseLightTheme", 1).toInt() == 0;
}

void FluentTheme::checkWindowsThemeChanges() {
    static bool lastDarkMode = isWindowsDarkMode();
    static QColor lastAccentColor = getWindowsAccentColor();
    
    const bool currentDarkMode = isWindowsDarkMode();
    const QColor currentAccentColor = getWindowsAccentColor();
    
    bool changed = false;
    
    if (currentDarkMode != lastDarkMode) {
        lastDarkMode = currentDarkMode;
        changed = true;
    }
    
    if (currentAccentColor != lastAccentColor) {
        lastAccentColor = currentAccentColor;
        // Update accent color if using system colors
        if (m_useSystemAccentColor) {
            updateSystemAccentColor(currentAccentColor);
            changed = true;
        }
    }
    
    if (changed && m_mode == FluentThemeMode::System) {
        emit themeChanged();
    }
}

QColor FluentTheme::getWindowsAccentColor() const {
    QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\DWM", 
                      QSettings::NativeFormat);
    
    bool ok;
    const quint32 colorValue = registry.value("AccentColor", 0xFF0078D4).toUInt(&ok);
    
    if (ok) {
        // Windows stores ABGR format
        const quint8 r = (colorValue >> 0) & 0xFF;
        const quint8 g = (colorValue >> 8) & 0xFF;
        const quint8 b = (colorValue >> 16) & 0xFF;
        const quint8 a = (colorValue >> 24) & 0xFF;
        
        return QColor(r, g, b, a);
    }
    
    return QColor("#0078d4"); // Fallback to default blue
}
#endif

#ifdef Q_OS_MAC
bool FluentTheme::isMacOSDarkMode() const {
    // This would require proper Objective-C integration
    // For now, use a simplified check
    const QString appearance = qgetenv("APPEARANCE");
    return appearance.contains("Dark", Qt::CaseInsensitive);
}
#endif

void FluentTheme::updateEffectiveMode() {
    // This method handles the logic for system mode
    if (m_mode == FluentThemeMode::System) {
        // The currentPalette() method will handle the actual mode detection
        qDebug() << "System theme mode detected as:" << (isSystemDarkMode() ? "Dark" : "Light");
    }
}

void FluentTheme::updateSystemAccentColor(const QColor& systemColor) {
    // Find the closest FluentAccentColor to the system color
    const auto closestAccent = findClosestAccentColor(systemColor);
    if (closestAccent != m_accentColor) {
        m_accentColor = closestAccent;
        updateAccentColors();
        emit accentColorChanged(m_accentColor);
    }
}

FluentAccentColor FluentTheme::findClosestAccentColor(const QColor& targetColor) const {
    // Simple color distance calculation
    const auto colorDistance = [](const QColor& c1, const QColor& c2) {
        const int dr = c1.red() - c2.red();
        const int dg = c1.green() - c2.green();
        const int db = c1.blue() - c2.blue();
        return dr * dr + dg * dg + db * db;
    };
    
    const std::vector<std::pair<FluentAccentColor, QColor>> accentOptions = {
        {FluentAccentColor::Blue, QColor("#0078d4")},
        {FluentAccentColor::Purple, QColor("#8764b8")},
        {FluentAccentColor::Pink, QColor("#e3008c")},
        {FluentAccentColor::Red, QColor("#d13438")},
        {FluentAccentColor::Orange, QColor("#ff8c00")},
        {FluentAccentColor::Yellow, QColor("#ffb900")},
        {FluentAccentColor::Green, QColor("#107c10")},
        {FluentAccentColor::Teal, QColor("#008272")}
    };
    
    FluentAccentColor closest = FluentAccentColor::Blue;
    int minDistance = std::numeric_limits<int>::max();
    
    for (const auto& [accent, color] : accentOptions) {
        const int distance = colorDistance(targetColor, color);
        if (distance < minDistance) {
            minDistance = distance;
            closest = accent;
        }
    }
    
    return closest;
}

// Accessibility features
bool FluentTheme::isHighContrastMode() const {
    return m_highContrastMode;
}

void FluentTheme::setHighContrastMode(bool enabled) {
    if (m_highContrastMode != enabled) {
        m_highContrastMode = enabled;
        emit themeChanged();
    }
}

qreal FluentTheme::contrastRatio(const QColor& foreground, const QColor& background) const {
    // Calculate relative luminance
    auto relativeLuminance = [](const QColor& color) {
        auto sRGBToLinear = [](qreal c) {
            return (c <= 0.03928) ? c / 12.92 : qPow((c + 0.055) / 1.055, 2.4);
        };

        qreal r = sRGBToLinear(color.redF());
        qreal g = sRGBToLinear(color.greenF());
        qreal b = sRGBToLinear(color.blueF());

        return 0.2126 * r + 0.7152 * g + 0.0722 * b;
    };

    qreal l1 = relativeLuminance(foreground);
    qreal l2 = relativeLuminance(background);

    // Ensure l1 is the lighter color
    if (l1 < l2) {
        std::swap(l1, l2);
    }

    return (l1 + 0.05) / (l2 + 0.05);
}

QColor FluentTheme::ensureContrast(const QColor& foreground, const QColor& background, qreal minRatio) const {
    qreal currentRatio = contrastRatio(foreground, background);

    if (currentRatio >= minRatio) {
        return foreground;
    }

    // Adjust foreground color to meet contrast requirements
    QColor adjusted = foreground;
    qreal factor = 0.1;

    // Try darkening first
    while (contrastRatio(adjusted, background) < minRatio && adjusted.lightnessF() > 0.1) {
        adjusted = darken(adjusted, factor);
        factor += 0.1;
    }

    // If darkening didn't work, try lightening
    if (contrastRatio(adjusted, background) < minRatio) {
        adjusted = foreground;
        factor = 0.1;
        while (contrastRatio(adjusted, background) < minRatio && adjusted.lightnessF() < 0.9) {
            adjusted = lighten(adjusted, factor);
            factor += 0.1;
        }
    }

    return adjusted;
}

// Advanced color utilities
QColor FluentTheme::lighten(const QColor& color, qreal amount) const {
    QColor result = color;
    qreal lightness = result.lightnessF();
    result.setHslF(result.hslHueF(), result.hslSaturationF(), qMin(1.0, lightness + amount));
    return result;
}

QColor FluentTheme::darken(const QColor& color, qreal amount) const {
    QColor result = color;
    qreal lightness = result.lightnessF();
    result.setHslF(result.hslHueF(), result.hslSaturationF(), qMax(0.0, lightness - amount));
    return result;
}

QColor FluentTheme::adjustAlpha(const QColor& color, qreal alpha) const {
    QColor result = color;
    result.setAlphaF(qBound(0.0, alpha, 1.0));
    return result;
}

QColor FluentTheme::blend(const QColor& color1, const QColor& color2, qreal ratio) const {
    ratio = qBound(0.0, ratio, 1.0);

    qreal r = color1.redF() * (1.0 - ratio) + color2.redF() * ratio;
    qreal g = color1.greenF() * (1.0 - ratio) + color2.greenF() * ratio;
    qreal b = color1.blueF() * (1.0 - ratio) + color2.blueF() * ratio;
    qreal a = color1.alphaF() * (1.0 - ratio) + color2.alphaF() * ratio;

    return QColor::fromRgbF(r, g, b, a);
}

// Brush creation helpers
QBrush FluentTheme::createAcrylicBrush(const QColor& baseColor) const {
    // Create a semi-transparent brush that simulates acrylic effect
    QColor acrylicColor = baseColor;
    acrylicColor.setAlphaF(0.8);
    
    // In a full implementation, this would create a more complex brush
    // with noise texture and blur effects
    return QBrush(acrylicColor);
}

QBrush FluentTheme::createMicaBrush() const {
    // Mica is a more subtle material effect in Windows 11
    const auto& palette = currentPalette();
    QColor micaColor = palette.neutralLighter;
    micaColor.setAlphaF(0.7);
    
    return QBrush(micaColor);
}

QBrush FluentTheme::createGradientBrush(const QColor& baseColor) const {
    QLinearGradient gradient(0, 0, 0, 100);
    gradient.setColorAt(0, baseColor.lighter(110));
    gradient.setColorAt(1, baseColor.darker(110));
    
    return QBrush(gradient);
}

// Public API extensions
void FluentTheme::setCustomColor(const QString& name, const QColor& color) {
    m_customColors[name.toStdString()] = color;
    saveSettings();
    emit themeChanged();
}

QColor FluentTheme::customColor(const QString& name) const {
    const auto it = m_customColors.find(name.toStdString());
    return (it != m_customColors.end()) ? it->second : QColor();
}

void FluentTheme::resetToDefaults() {
    m_mode = FluentThemeMode::System;
    m_accentColor = FluentAccentColor::Blue;
    m_customColors.clear();
    
    initializePalettes();
    saveSettings();
    
    emit modeChanged(m_mode);
    emit accentColorChanged(m_accentColor);
    emit themeChanged();
}

QString FluentTheme::exportTheme() const {
    QJsonObject themeJson;
    themeJson["mode"] = static_cast<int>(m_mode);
    themeJson["accentColor"] = static_cast<int>(m_accentColor);
    
    // Export custom colors
    QJsonObject customColorsJson;
    for (const auto& [name, color] : m_customColors) {
        customColorsJson[QString::fromStdString(name)] = color.name();
    }
    themeJson["customColors"] = customColorsJson;
    
    QJsonDocument doc(themeJson);
    return doc.toJson(QJsonDocument::Compact);
}

bool FluentTheme::importTheme(const QString& themeData) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(themeData.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse theme JSON:" << error.errorString();
        return false;
    }
    
    const QJsonObject themeJson = doc.object();
    
    // Import basic settings
    if (themeJson.contains("mode")) {
        m_mode = static_cast<FluentThemeMode>(themeJson["mode"].toInt());
    }
    
    if (themeJson.contains("accentColor")) {
        m_accentColor = static_cast<FluentAccentColor>(themeJson["accentColor"].toInt());
    }
    
    // Import custom colors
    if (themeJson.contains("customColors")) {
        const QJsonObject customColorsJson = themeJson["customColors"].toObject();
        for (auto it = customColorsJson.begin(); it != customColorsJson.end(); ++it) {
            const QColor color(it.value().toString());
            if (color.isValid()) {
                m_customColors[it.key().toStdString()] = color;
            }
        }
    }
    
    // Regenerate palettes and save
    updateAccentColors();
    saveSettings();
    
    emit modeChanged(m_mode);
    emit accentColorChanged(m_accentColor);
    emit themeChanged();
    
    return true;
}

} // namespace FluentQt::Styling