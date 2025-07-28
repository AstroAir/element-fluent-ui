// include/FluentQt/Styling/FluentTheme.h
#pragma once

#include <QObject>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QPalette>
#include <QSettings>
#include <QTimer>
#include <unordered_map>
#include <string_view>

namespace FluentQt::Styling {

enum class FluentThemeMode {
    Light,
    Dark,
    System
};

enum class FluentAccentColor {
    Blue,
    Purple,
    Pink,
    Red,
    Orange,
    Yellow,
    Green,
    Teal
};

struct FluentColorPalette {
    // Primary colors
    QColor accent;
    QColor accentLight1;
    QColor accentLight2; 
    QColor accentLight3;
    QColor accentDark1;
    QColor accentDark2;
    QColor accentDark3;

    // Neutral colors
    QColor neutralLightest;
    QColor neutralLighter;
    QColor neutralLight;
    QColor neutralQuaternaryAlt;
    QColor neutralQuaternary;
    QColor neutralTertiaryAlt;
    QColor neutralTertiary;
    QColor neutralSecondaryAlt;
    QColor neutralSecondary;
    QColor neutralPrimaryAlt;
    QColor neutralPrimary;
    QColor neutralDark;
    QColor neutralDarker;
    QColor neutralDarkest;

    // Semantic colors
    QColor error;
    QColor errorLight;
    QColor warning;
    QColor warningLight;
    QColor success;
    QColor successLight;
    QColor info;
    QColor infoLight;
};

class FluentTheme : public QObject {
    Q_OBJECT
    Q_PROPERTY(FluentThemeMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(FluentAccentColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)

public:
    static FluentTheme& instance();

    // Theme mode
    FluentThemeMode mode() const noexcept { return m_mode; }
    void setMode(FluentThemeMode mode);

    // Accent color
    FluentAccentColor accentColor() const noexcept { return m_accentColor; }
    void setAccentColor(FluentAccentColor color);

    // Color palette access
    const FluentColorPalette& lightPalette() const noexcept { return m_lightPalette; }
    const FluentColorPalette& darkPalette() const noexcept { return m_darkPalette; }
    const FluentColorPalette& currentPalette() const noexcept;

    // Dynamic color access
    QColor color(std::string_view colorName) const;
    QBrush brush(std::string_view brushName) const;
    
    // Typography
    QFont displayFont() const;
    QFont titleLargeFont() const;
    QFont titleFont() const;
    QFont titleSmallFont() const;
    QFont subtitleFont() const;
    QFont bodyLargeFont() const;
    QFont bodyFont() const;
    QFont bodySmallFont() const;
    QFont captionFont() const;
    QFont overlineFont() const;

    // Typography with semantic meaning
    QFont headingFont(int level = 1) const; // H1-H6
    QFont buttonFont() const;
    QFont linkFont() const;
    QFont codeFont() const;

    // Spacing and sizing
    int spacing(std::string_view size) const;
    QSize iconSize(std::string_view size) const;

    // Enhanced spacing system
    int borderRadius(std::string_view size) const;
    int elevation(std::string_view level) const;
    QMargins margins(std::string_view size) const;
    QMargins padding(std::string_view size) const;

    // Layout dimensions
    int componentHeight(std::string_view size) const;
    int componentWidth(std::string_view size) const;
    int strokeWidth(std::string_view weight) const;

    // Custom colors
    void setCustomColor(const QString& name, const QColor& color);
    QColor customColor(const QString& name) const;

    // Accessibility features
    bool isHighContrastMode() const;
    void setHighContrastMode(bool enabled);
    qreal contrastRatio(const QColor& foreground, const QColor& background) const;
    QColor ensureContrast(const QColor& foreground, const QColor& background, qreal minRatio = 4.5) const;

    // Advanced color utilities
    QColor lighten(const QColor& color, qreal amount = 0.1) const;
    QColor darken(const QColor& color, qreal amount = 0.1) const;
    QColor adjustAlpha(const QColor& color, qreal alpha) const;
    QColor blend(const QColor& color1, const QColor& color2, qreal ratio = 0.5) const;

    // Theme management
    void resetToDefaults();
    QString exportTheme() const;
    bool importTheme(const QString& themeData);

signals:
    void modeChanged(FluentThemeMode mode);
    void accentColorChanged(FluentAccentColor color);
    void themeChanged();

private slots:
    void onSystemThemeChanged(Qt::ColorScheme colorScheme);

private:
    FluentTheme();
    void initializePalettes();
    void updateAccentColors();
    void loadSettings();
    void saveSettings();
    void connectToSystemTheme();
    void initializeSpacingAndSizing();
    void initializeTypography();
    void initializeBorderRadius();
    void initializeElevation();
    void updateEffectiveMode();
    void loadCustomColors(QSettings& settings);
    void saveCustomColors(QSettings& settings);
    bool isSystemDarkMode() const;
    void updateSystemAccentColor(const QColor& systemColor);
    FluentAccentColor findClosestAccentColor(const QColor& targetColor) const;

    // Brush creation helpers
    QBrush createAcrylicBrush(const QColor& baseColor) const;
    QBrush createMicaBrush() const;
    QBrush createGradientBrush(const QColor& baseColor) const;

#ifdef Q_OS_WIN
    bool isWindowsDarkMode() const;
    void checkWindowsThemeChanges();
    QColor getWindowsAccentColor() const;
#endif

#ifdef Q_OS_MAC
    bool isMacOSDarkMode() const;
#endif

private:
    FluentThemeMode m_mode{FluentThemeMode::System};
    FluentAccentColor m_accentColor{FluentAccentColor::Blue};
    FluentColorPalette m_lightPalette;
    FluentColorPalette m_darkPalette;

    // Enhanced design tokens
    std::unordered_map<std::string, int> m_spacingMap;
    std::unordered_map<std::string, QSize> m_iconSizeMap;
    std::unordered_map<std::string, int> m_borderRadiusMap;
    std::unordered_map<std::string, int> m_elevationMap;
    std::unordered_map<std::string, QMargins> m_marginsMap;
    std::unordered_map<std::string, QMargins> m_paddingMap;
    std::unordered_map<std::string, int> m_componentHeightMap;
    std::unordered_map<std::string, int> m_componentWidthMap;
    std::unordered_map<std::string, int> m_strokeWidthMap;

    // Typography maps
    std::unordered_map<std::string, QFont> m_fontMap;

    // Custom colors
    std::unordered_map<std::string, QColor> m_customColors;

    // Accessibility
    bool m_highContrastMode{false};

    // System integration
    QTimer* m_systemThemeTimer{nullptr};
    bool m_useSystemAccentColor{false};
};

} // namespace FluentQt::Styling