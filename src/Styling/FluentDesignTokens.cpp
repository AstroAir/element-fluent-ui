// src/Styling/FluentDesignTokens.cpp
#include <QApplication>
#include <QDebug>
#include <QScreen>
#include "FluentQt/Styling/FluentAdvancedTheme.h"

namespace FluentQt::Styling {

// Forward declarations
void validateDesignTokenSystem();
void validateSemanticTokenReferences();
void validateColorAccessibility();
void registerComponentTokenMappings();
void registerButtonTokens(FluentAdvancedThemeManager& manager);
void registerCardTokens(FluentAdvancedThemeManager& manager);
void registerInputTokens(FluentAdvancedThemeManager& manager);
void registerNavigationTokens(FluentAdvancedThemeManager& manager);
void setupTokenChangeNotifications();
FluentTokenType determineTokenType(const QString& tokenName);
double calculateSimpleContrastRatio(const QColor& color1, const QColor& color2);
void recomputeAllTokens(FluentAdvancedThemeManager& manager);

// Microsoft FluentUI Design Token Implementation
class FluentDesignTokenRegistry {
public:
    static void registerAllTokens() {
        registerColorTokens();
        registerTypographyTokens();
        registerSpacingTokens();
        registerShadowTokens();
        registerAnimationTokens();
        registerBorderTokens();
        registerBreakpointTokens();
    }

private:
    static void registerColorTokens() {
        auto& manager = FluentAdvancedThemeManager::instance();

        // Neutral color palette (Microsoft FluentUI specification)
        QList<QPair<QString, QColor>> neutralColors = {
            {"neutral.0", QColor("#ffffff")},
            {"neutral.2", QColor("#fafafa")},
            {"neutral.4", QColor("#f5f5f5")},
            {"neutral.6", QColor("#f0f0f0")},
            {"neutral.8", QColor("#ebebeb")},
            {"neutral.10", QColor("#e1e1e1")},
            {"neutral.12", QColor("#d6d6d6")},
            {"neutral.14", QColor("#cccccc")},
            {"neutral.16", QColor("#c2c2c2")},
            {"neutral.18", QColor("#b8b8b8")},
            {"neutral.20", QColor("#adadad")},
            {"neutral.22", QColor("#a3a3a3")},
            {"neutral.24", QColor("#999999")},
            {"neutral.26", QColor("#8f8f8f")},
            {"neutral.28", QColor("#858585")},
            {"neutral.30", QColor("#7a7a7a")},
            {"neutral.32", QColor("#707070")},
            {"neutral.34", QColor("#666666")},
            {"neutral.36", QColor("#5c5c5c")},
            {"neutral.38", QColor("#525252")},
            {"neutral.40", QColor("#474747")},
            {"neutral.42", QColor("#3d3d3d")},
            {"neutral.44", QColor("#333333")},
            {"neutral.46", QColor("#292929")},
            {"neutral.48", QColor("#1f1f1f")},
            {"neutral.50", QColor("#141414")},
            {"neutral.52", QColor("#0a0a0a")},
            {"neutral.54", QColor("#000000")}};

        for (const auto& color : neutralColors) {
            FluentDesignToken token;
            token.name = color.first;
            token.value = color.second;
            token.type = FluentTokenType::Color;
            token.category = "neutral";
            token.description = QString("Neutral color %1").arg(color.first);
            manager.registerToken(token);
        }

        // Brand color palette (Microsoft Blue)
        QList<QPair<QString, QColor>> brandColors = {
            {"brand.10", QColor("#061724")},  {"brand.20", QColor("#082338")},
            {"brand.30", QColor("#0a2e4a")},  {"brand.40", QColor("#0c3b5e")},
            {"brand.50", QColor("#0e4775")},  {"brand.60", QColor("#0f548c")},
            {"brand.70", QColor("#115ea3")},  {"brand.80", QColor("#106ebe")},
            {"brand.90", QColor("#0f7bd8")},  {"brand.100", QColor("#0d8cf0")},
            {"brand.110", QColor("#2899f5")}, {"brand.120", QColor("#479ef5")},
            {"brand.130", QColor("#62abf5")}, {"brand.140", QColor("#77b7f7")},
            {"brand.150", QColor("#96c6fa")}, {"brand.160", QColor("#b4d6fa")}};

        for (const auto& color : brandColors) {
            FluentDesignToken token;
            token.name = color.first;
            token.value = color.second;
            token.type = FluentTokenType::Color;
            token.category = "brand";
            token.description = QString("Brand color %1").arg(color.first);
            manager.registerToken(token);
        }

        // Semantic color tokens
        QList<QPair<QString, QString>> semanticColors = {
            {"color.background.primary", "neutral.0"},
            {"color.background.secondary", "neutral.2"},
            {"color.background.tertiary", "neutral.4"},
            {"color.background.disabled", "neutral.8"},
            {"color.text.primary", "neutral.54"},
            {"color.text.secondary", "neutral.38"},
            {"color.text.tertiary", "neutral.30"},
            {"color.text.disabled", "neutral.22"},
            {"color.text.accent", "brand.100"},
            {"color.border.primary", "neutral.12"},
            {"color.border.secondary", "neutral.8"},
            {"color.border.focus", "brand.100"},
            {"color.surface.primary", "neutral.0"},
            {"color.surface.secondary", "neutral.2"},
            {"color.surface.tertiary", "neutral.4"}};

        for (const auto& semantic : semanticColors) {
            FluentDesignToken token;
            token.name = semantic.first;
            token.value = semantic.second;
            token.type = FluentTokenType::Color;
            token.category = "semantic";
            token.description =
                QString("Semantic color reference to %1").arg(semantic.second);
            token.isComputed = true;
            token.computeExpression = semantic.second;
            manager.registerToken(token);
        }
    }

    static void registerTypographyTokens() {
        auto& manager = FluentAdvancedThemeManager::instance();

        // Microsoft FluentUI Typography Scale
        QList<QPair<QString, QFont>> typographyTokens = {
            {"typography.caption1", createFont("Segoe UI", 12, QFont::Normal)},
            {"typography.caption2", createFont("Segoe UI", 11, QFont::Normal)},
            {"typography.body1", createFont("Segoe UI", 14, QFont::Normal)},
            {"typography.body1Strong",
             createFont("Segoe UI", 14, QFont::DemiBold)},
            {"typography.body2", createFont("Segoe UI", 13, QFont::Normal)},
            {"typography.subtitle1",
             createFont("Segoe UI", 16, QFont::DemiBold)},
            {"typography.subtitle2",
             createFont("Segoe UI", 14, QFont::DemiBold)},
            {"typography.title1", createFont("Segoe UI", 28, QFont::DemiBold)},
            {"typography.title2", createFont("Segoe UI", 24, QFont::DemiBold)},
            {"typography.title3", createFont("Segoe UI", 20, QFont::DemiBold)},
            {"typography.largeTitle",
             createFont("Segoe UI", 40, QFont::DemiBold)},
            {"typography.display",
             createFont("Segoe UI", 68, QFont::DemiBold)}};

        for (const auto& typo : typographyTokens) {
            FluentDesignToken token;
            token.name = typo.first;
            token.value = typo.second;
            token.type = FluentTokenType::Typography;
            token.category = "typography";
            token.description = QString("Typography token %1").arg(typo.first);
            manager.registerToken(token);
        }

        // Line height tokens
        QList<QPair<QString, double>> lineHeightTokens = {
            {"typography.lineHeight.100", 1.0},
            {"typography.lineHeight.200", 1.125},
            {"typography.lineHeight.300", 1.25},
            {"typography.lineHeight.400", 1.375},
            {"typography.lineHeight.500", 1.5},
            {"typography.lineHeight.600", 1.75}};

        for (const auto& lineHeight : lineHeightTokens) {
            FluentDesignToken token;
            token.name = lineHeight.first;
            token.value = lineHeight.second;
            token.type = FluentTokenType::Typography;
            token.category = "lineHeight";
            token.description =
                QString("Line height ratio %1").arg(lineHeight.second);
            manager.registerToken(token);
        }
    }

    static void registerSpacingTokens() {
        auto& manager = FluentAdvancedThemeManager::instance();

        // Microsoft FluentUI Spacing Scale (4px base unit)
        QList<QPair<QString, int>> spacingTokens = {
            {"spacing.none", 0},   {"spacing.xxs", 2},  {"spacing.xs", 4},
            {"spacing.sNudge", 6}, {"spacing.s", 8},    {"spacing.mNudge", 10},
            {"spacing.m", 12},     {"spacing.l", 16},   {"spacing.xl", 20},
            {"spacing.xxl", 24},   {"spacing.xxxl", 32}};

        for (const auto& spacing : spacingTokens) {
            FluentDesignToken token;
            token.name = spacing.first;
            token.value = spacing.second;
            token.type = FluentTokenType::Spacing;
            token.category = "spacing";
            token.description =
                QString("Spacing value %1px").arg(spacing.second);
            manager.registerToken(token);
        }

        // Component-specific spacing
        QList<QPair<QString, QString>> componentSpacing = {
            {"spacing.button.padding.horizontal", "spacing.m"},
            {"spacing.button.padding.vertical", "spacing.s"},
            {"spacing.card.padding", "spacing.l"},
            {"spacing.dialog.padding", "spacing.xl"},
            {"spacing.list.item.padding", "spacing.s"}};

        for (const auto& compSpacing : componentSpacing) {
            FluentDesignToken token;
            token.name = compSpacing.first;
            token.value = compSpacing.second;
            token.type = FluentTokenType::Spacing;
            token.category = "component";
            token.description = QString("Component spacing reference to %1")
                                    .arg(compSpacing.second);
            token.isComputed = true;
            token.computeExpression = compSpacing.second;
            manager.registerToken(token);
        }
    }

    static void registerShadowTokens() {
        auto& manager = FluentAdvancedThemeManager::instance();

        // Microsoft FluentUI Shadow Tokens
        QList<QPair<QString, QString>> shadowTokens = {
            {"shadow.2",
             "0px 1px 2px rgba(0, 0, 0, 0.14), 0px 0px 2px rgba(0, 0, 0, "
             "0.12)"},
            {"shadow.4",
             "0px 2px 4px rgba(0, 0, 0, 0.14), 0px 0px 2px rgba(0, 0, 0, "
             "0.12)"},
            {"shadow.8",
             "0px 4px 8px rgba(0, 0, 0, 0.14), 0px 0px 2px rgba(0, 0, 0, "
             "0.12)"},
            {"shadow.16",
             "0px 8px 16px rgba(0, 0, 0, 0.14), 0px 0px 2px rgba(0, 0, 0, "
             "0.12)"},
            {"shadow.28",
             "0px 14px 28px rgba(0, 0, 0, 0.24), 0px 0px 8px rgba(0, 0, 0, "
             "0.20)"},
            {"shadow.64",
             "0px 32px 64px rgba(0, 0, 0, 0.24), 0px 0px 8px rgba(0, 0, 0, "
             "0.20)"}};

        for (const auto& shadow : shadowTokens) {
            FluentDesignToken token;
            token.name = shadow.first;
            token.value = shadow.second;
            token.type = FluentTokenType::Shadow;
            token.category = "shadow";
            token.description = QString("Shadow depth %1").arg(shadow.first);
            manager.registerToken(token);
        }
    }

    static void registerAnimationTokens() {
        auto& manager = FluentAdvancedThemeManager::instance();

        // Microsoft FluentUI Animation Tokens
        QList<QPair<QString, int>> durationTokens = {
            {"duration.ultraFast", 50}, {"duration.faster", 100},
            {"duration.fast", 150},     {"duration.normal", 200},
            {"duration.slow", 300},     {"duration.slower", 400},
            {"duration.ultraSlow", 500}};

        for (const auto& duration : durationTokens) {
            FluentDesignToken token;
            token.name = duration.first;
            token.value = duration.second;
            token.type = FluentTokenType::Animation;
            token.category = "duration";
            token.description =
                QString("Animation duration %1ms").arg(duration.second);
            manager.registerToken(token);
        }

        // Easing curves
        QList<QPair<QString, QString>> easingTokens = {
            {"easing.accelerateMax", "cubic-bezier(1,0,1,1)"},
            {"easing.accelerateMid", "cubic-bezier(0.7,0,1,0.5)"},
            {"easing.accelerateMin", "cubic-bezier(0.8,0,0.78,1)"},
            {"easing.decelerateMax", "cubic-bezier(0,0,0,1)"},
            {"easing.decelerateMid", "cubic-bezier(0.1,0.9,0.2,1)"},
            {"easing.decelerateMin", "cubic-bezier(0.33,0,0.1,1)"},
            {"easing.maxEasyEase", "cubic-bezier(0.8,0,0.1,1)"},
            {"easing.easyEase", "cubic-bezier(0.33,0,0.67,1)"},
            {"easing.linear", "cubic-bezier(0,0,1,1)"}};

        for (const auto& easing : easingTokens) {
            FluentDesignToken token;
            token.name = easing.first;
            token.value = easing.second;
            token.type = FluentTokenType::Animation;
            token.category = "easing";
            token.description = QString("Easing curve %1").arg(easing.first);
            manager.registerToken(token);
        }
    }

    static void registerBorderTokens() {
        auto& manager = FluentAdvancedThemeManager::instance();

        // Border radius tokens
        QList<QPair<QString, int>> radiusTokens = {
            {"borderRadius.none", 0},   {"borderRadius.small", 2},
            {"borderRadius.medium", 4}, {"borderRadius.large", 6},
            {"borderRadius.xLarge", 8}, {"borderRadius.circular", 10000}};

        for (const auto& radius : radiusTokens) {
            FluentDesignToken token;
            token.name = radius.first;
            token.value = radius.second;
            token.type = FluentTokenType::Border;
            token.category = "radius";
            token.description =
                QString("Border radius %1px").arg(radius.second);
            manager.registerToken(token);
        }

        // Border width tokens
        QList<QPair<QString, int>> widthTokens = {{"borderWidth.thin", 1},
                                                  {"borderWidth.thick", 2},
                                                  {"borderWidth.thicker", 3},
                                                  {"borderWidth.thickest", 4}};

        for (const auto& width : widthTokens) {
            FluentDesignToken token;
            token.name = width.first;
            token.value = width.second;
            token.type = FluentTokenType::Border;
            token.category = "width";
            token.description = QString("Border width %1px").arg(width.second);
            manager.registerToken(token);
        }
    }

    static void registerBreakpointTokens() {
        auto& manager = FluentAdvancedThemeManager::instance();

        // Responsive breakpoints
        QList<QPair<QString, int>> breakpointTokens = {
            {"breakpoint.small", 480},
            {"breakpoint.medium", 640},
            {"breakpoint.large", 1024},
            {"breakpoint.xLarge", 1366},
            {"breakpoint.xxLarge", 1920}};

        for (const auto& breakpoint : breakpointTokens) {
            FluentDesignToken token;
            token.name = breakpoint.first;
            token.value = breakpoint.second;
            token.type = FluentTokenType::Breakpoint;
            token.category = "breakpoint";
            token.description =
                QString("Responsive breakpoint %1px").arg(breakpoint.second);
            manager.registerToken(token);
        }
    }

    static QFont createFont(const QString& family, int size,
                            QFont::Weight weight) {
        QFont font(family, size);
        font.setWeight(weight);
        return font;
    }
};

// Enhanced design token initialization with validation
void initializeFluentDesignTokens() {
    static bool initialized = false;
    if (!initialized) {
        qDebug() << "Initializing FluentUI Design Token System...";

        // Register all token categories
        FluentDesignTokenRegistry::registerAllTokens();

        // Validate token system integrity
        validateDesignTokenSystem();

        // Register component-specific token mappings
        registerComponentTokenMappings();

        // Setup token change notifications
        setupTokenChangeNotifications();

        initialized = true;
        qDebug() << "FluentUI Design Token System initialized successfully";
    }
}

// Validate the design token system for completeness and consistency
void validateDesignTokenSystem() {
    auto& manager = FluentAdvancedThemeManager::instance();

    // Validate required token categories exist
    QStringList requiredCategories = {"neutral",    "brand",   "semantic",
                                      "typography", "spacing", "shadow",
                                      "animation",  "border",  "breakpoint"};

    for (const QString& category : requiredCategories) {
        QStringList categoryTokens = manager.getTokenNames();
        bool categoryExists = false;
        for (const QString& token : categoryTokens) {
            if (token.startsWith(category + ".")) {
                categoryExists = true;
                break;
            }
        }

        if (!categoryExists) {
            qWarning() << "Missing design token category:" << category;
        }
    }

    // Validate semantic token references
    validateSemanticTokenReferences();

    // Validate color accessibility
    validateColorAccessibility();
}

// Validate that semantic tokens properly reference base tokens
void validateSemanticTokenReferences() {
    auto& manager = FluentAdvancedThemeManager::instance();

    QStringList semanticTokens = {"color.background.primary",
                                  "color.text.primary", "color.border.primary"};

    for (const QString& token : semanticTokens) {
        QVariant resolved = manager.resolveToken(token);
        if (!resolved.isValid()) {
            qWarning() << "Semantic token reference broken:" << token;
        }
    }
}

// Validate color accessibility compliance
void validateColorAccessibility() {
    auto& manager = FluentAdvancedThemeManager::instance();

    // Check common color combinations for WCAG compliance
    QColor background =
        manager.resolveToken("color.background.primary").value<QColor>();
    QColor textPrimary =
        manager.resolveToken("color.text.primary").value<QColor>();

    if (background.isValid() && textPrimary.isValid()) {
        // Calculate contrast ratio (simplified)
        double ratio = calculateSimpleContrastRatio(textPrimary, background);
        if (ratio < 4.5) {
            qWarning() << "Primary text/background combination may not meet "
                          "WCAG AA contrast requirements";
        }
    }
}

// Register component-specific token mappings
void registerComponentTokenMappings() {
    auto& manager = FluentAdvancedThemeManager::instance();

    // Button component tokens
    registerButtonTokens(manager);

    // Card component tokens
    registerCardTokens(manager);

    // Input component tokens
    registerInputTokens(manager);

    // Navigation component tokens
    registerNavigationTokens(manager);
}

// Register button-specific design tokens
void registerButtonTokens(FluentAdvancedThemeManager& manager) {
    // Button size variants
    QList<QPair<QString, QVariant>> buttonTokens = {
        {"button.size.small.height", 24},
        {"button.size.small.padding.horizontal", 8},
        {"button.size.small.padding.vertical", 4},
        {"button.size.small.fontSize", 12},

        {"button.size.medium.height", 32},
        {"button.size.medium.padding.horizontal", 12},
        {"button.size.medium.padding.vertical", 6},
        {"button.size.medium.fontSize", 14},

        {"button.size.large.height", 40},
        {"button.size.large.padding.horizontal", 16},
        {"button.size.large.padding.vertical", 8},
        {"button.size.large.fontSize", 16},

        // Button style tokens
        {"button.borderRadius", "borderRadius.medium"},
        {"button.borderWidth", "borderWidth.thin"},
        {"button.shadow.rest", "shadow.2"},
        {"button.shadow.hover", "shadow.4"},
        {"button.shadow.pressed", "shadow.2"},

        // Button color tokens
        {"button.primary.background.rest", "brand.100"},
        {"button.primary.background.hover", "brand.110"},
        {"button.primary.background.pressed", "brand.90"},
        {"button.primary.text", "neutral.0"},

        {"button.secondary.background.rest", "neutral.0"},
        {"button.secondary.background.hover", "neutral.4"},
        {"button.secondary.background.pressed", "neutral.8"},
        {"button.secondary.text", "neutral.54"},
        {"button.secondary.border", "neutral.12"}};

    for (const auto& token : buttonTokens) {
        FluentDesignToken designToken;
        designToken.name = token.first;
        designToken.value = token.second;
        designToken.type = token.first.contains("color") ||
                                   token.first.contains("background") ||
                                   token.first.contains("text") ||
                                   token.first.contains("border")
                               ? FluentTokenType::Color
                               : FluentTokenType::Size;
        designToken.category = "button";
        designToken.description =
            QString("Button component token: %1").arg(token.first);

        // Mark computed tokens
        if (token.second.toString().contains(".")) {
            designToken.isComputed = true;
            designToken.computeExpression = token.second.toString();
        }

        manager.registerToken(designToken);
    }
}

// Register card-specific design tokens
void registerCardTokens(FluentAdvancedThemeManager& manager) {
    QList<QPair<QString, QVariant>> cardTokens = {
        {"card.padding.small", "spacing.s"},
        {"card.padding.medium", "spacing.m"},
        {"card.padding.large", "spacing.l"},

        {"card.borderRadius", "borderRadius.large"},
        {"card.borderWidth", "borderWidth.thin"},

        {"card.background", "color.surface.primary"},
        {"card.border", "color.border.secondary"},
        {"card.shadow.rest", "shadow.4"},
        {"card.shadow.hover", "shadow.8"},

        {"card.header.fontSize", "typography.subtitle1"},
        {"card.body.fontSize", "typography.body1"},
        {"card.caption.fontSize", "typography.caption1"}};

    for (const auto& token : cardTokens) {
        FluentDesignToken designToken;
        designToken.name = token.first;
        designToken.value = token.second;
        designToken.type = determineTokenType(token.first);
        designToken.category = "card";
        designToken.description =
            QString("Card component token: %1").arg(token.first);
        designToken.isComputed = token.second.toString().contains(".");
        if (designToken.isComputed) {
            designToken.computeExpression = token.second.toString();
        }

        manager.registerToken(designToken);
    }
}

// Register input-specific design tokens
void registerInputTokens(FluentAdvancedThemeManager& manager) {
    QList<QPair<QString, QVariant>> inputTokens = {
        {"input.height.small", 24},
        {"input.height.medium", 32},
        {"input.height.large", 40},

        {"input.padding.horizontal", "spacing.s"},
        {"input.padding.vertical", "spacing.xs"},

        {"input.borderRadius", "borderRadius.medium"},
        {"input.borderWidth.rest", "borderWidth.thin"},
        {"input.borderWidth.focus", "borderWidth.thick"},

        {"input.background.rest", "color.surface.primary"},
        {"input.background.hover", "color.surface.secondary"},
        {"input.background.focus", "color.surface.primary"},
        {"input.background.disabled", "color.background.disabled"},

        {"input.border.rest", "color.border.primary"},
        {"input.border.hover", "color.border.secondary"},
        {"input.border.focus", "color.border.focus"},
        {"input.border.error", "color.status.danger"},

        {"input.text", "color.text.primary"},
        {"input.text.placeholder", "color.text.tertiary"},
        {"input.text.disabled", "color.text.disabled"}};

    for (const auto& token : inputTokens) {
        FluentDesignToken designToken;
        designToken.name = token.first;
        designToken.value = token.second;
        designToken.type = determineTokenType(token.first);
        designToken.category = "input";
        designToken.description =
            QString("Input component token: %1").arg(token.first);
        designToken.isComputed = token.second.toString().contains(".");
        if (designToken.isComputed) {
            designToken.computeExpression = token.second.toString();
        }

        manager.registerToken(designToken);
    }
}

// Register navigation-specific design tokens
void registerNavigationTokens(FluentAdvancedThemeManager& manager) {
    QList<QPair<QString, QVariant>> navigationTokens = {
        {"navigation.height", 48},
        {"navigation.padding.horizontal", "spacing.m"},
        {"navigation.padding.vertical", "spacing.s"},

        {"navigation.background", "color.surface.primary"},
        {"navigation.border", "color.border.primary"},
        {"navigation.shadow", "shadow.2"},

        {"navigation.item.height", 40},
        {"navigation.item.padding.horizontal", "spacing.s"},
        {"navigation.item.borderRadius", "borderRadius.medium"},

        {"navigation.item.background.rest", "transparent"},
        {"navigation.item.background.hover", "color.surface.secondary"},
        {"navigation.item.background.selected", "color.surface.tertiary"},

        {"navigation.item.text.rest", "color.text.primary"},
        {"navigation.item.text.hover", "color.text.primary"},
        {"navigation.item.text.selected", "color.text.accent"},

        {"navigation.breadcrumb.separator", "color.text.tertiary"},
        {"navigation.breadcrumb.fontSize", "typography.body2"}};

    for (const auto& token : navigationTokens) {
        FluentDesignToken designToken;
        designToken.name = token.first;
        designToken.value = token.second;
        designToken.type = determineTokenType(token.first);
        designToken.category = "navigation";
        designToken.description =
            QString("Navigation component token: %1").arg(token.first);
        designToken.isComputed = token.second.toString().contains(".");
        if (designToken.isComputed) {
            designToken.computeExpression = token.second.toString();
        }

        manager.registerToken(designToken);
    }
}

// Setup token change notifications
void setupTokenChangeNotifications() {
    auto& manager = FluentAdvancedThemeManager::instance();

    // Connect to theme changes to update dependent tokens
    QObject::connect(&manager, &FluentAdvancedThemeManager::themeChanged,
                     [&manager](const QString& themeName) {
                         Q_UNUSED(themeName)
                         // Recompute all computed tokens when theme changes
                         recomputeAllTokens(manager);
                     });
}

// Helper functions
FluentTokenType determineTokenType(const QString& tokenName) {
    if (tokenName.contains("color") || tokenName.contains("background") ||
        tokenName.contains("text") || tokenName.contains("border")) {
        return FluentTokenType::Color;
    } else if (tokenName.contains("fontSize") || tokenName.contains("font")) {
        return FluentTokenType::Typography;
    } else if (tokenName.contains("padding") || tokenName.contains("margin") ||
               tokenName.contains("spacing")) {
        return FluentTokenType::Spacing;
    } else if (tokenName.contains("shadow")) {
        return FluentTokenType::Shadow;
    } else if (tokenName.contains("duration") || tokenName.contains("easing")) {
        return FluentTokenType::Animation;
    } else if (tokenName.contains("radius") || tokenName.contains("width")) {
        return FluentTokenType::Border;
    } else {
        return FluentTokenType::Size;
    }
}

double calculateSimpleContrastRatio(const QColor& color1,
                                    const QColor& color2) {
    // Simplified contrast calculation for validation
    double lum1 = (color1.red() * 0.299 + color1.green() * 0.587 +
                   color1.blue() * 0.114) /
                  255.0;
    double lum2 = (color2.red() * 0.299 + color2.green() * 0.587 +
                   color2.blue() * 0.114) /
                  255.0;

    double lighter = qMax(lum1, lum2);
    double darker = qMin(lum1, lum2);

    return (lighter + 0.05) / (darker + 0.05);
}

void recomputeAllTokens(FluentAdvancedThemeManager& manager) {
    // Recompute all computed tokens
    QStringList allTokens = manager.getTokenNames();
    for (const QString& tokenName : allTokens) {
        QVariant resolved = manager.resolveToken(tokenName);
        if (resolved.isValid()) {
            manager.setToken(tokenName, resolved);
        }
    }
}

}  // namespace FluentQt::Styling
