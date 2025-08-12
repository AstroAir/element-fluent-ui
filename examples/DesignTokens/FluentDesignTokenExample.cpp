// examples/DesignTokens/FluentDesignTokenExample.cpp
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

#include "FluentQt/Styling/FluentAdvancedTheme.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"

using namespace FluentQt::Styling;

// Example component using design tokens
class TokenizedButton : public QPushButton {
    Q_OBJECT

public:
    enum ButtonVariant { Primary, Secondary, Accent, Subtle };

    enum ButtonSize { Small, Medium, Large };

    TokenizedButton(const QString& text, ButtonVariant variant = Primary,
                    ButtonSize size = Medium, QWidget* parent = nullptr)
        : QPushButton(text, parent), m_variant(variant), m_size(size) {
        applyDesignTokens();
        setupHoverEffects();
    }

    void setVariant(ButtonVariant variant) {
        if (m_variant != variant) {
            m_variant = variant;
            applyDesignTokens();
        }
    }

    void setButtonSize(ButtonSize size) {
        if (m_size != size) {
            m_size = size;
            applyDesignTokens();
        }
    }

private slots:
    void onTokenChanged(const QString& tokenName, const QVariant& value) {
        Q_UNUSED(tokenName)
        Q_UNUSED(value)
        // Refresh styling when tokens change
        applyDesignTokens();
    }

private:
    void applyDesignTokens() {
        auto& tokenUtils = FluentDesignTokenUtils::instance();

        // Apply size tokens
        QString sizeStr = sizeToString(m_size);
        int height = FLUENT_BUTTON_SIZE(sizeStr, "height");
        int hPadding = FLUENT_BUTTON_SIZE(sizeStr, "padding.horizontal");
        int vPadding = FLUENT_BUTTON_SIZE(sizeStr, "padding.vertical");
        int fontSize = FLUENT_BUTTON_SIZE(sizeStr, "fontSize");

        setMinimumHeight(height);
        setContentsMargins(hPadding, vPadding, hPadding, vPadding);

        // Apply font
        QFont font = this->font();
        font.setPointSize(fontSize);
        setFont(font);

        // Apply variant-specific colors
        applyVariantColors();

        // Apply border radius
        int borderRadius = FLUENT_SIZE("button.borderRadius");
        setStyleSheet(
            QString("QPushButton { border-radius: %1px; }").arg(borderRadius));
    }

    void applyVariantColors() {
        QPalette palette = this->palette();
        QString variantStr = variantToString(m_variant);

        // Get colors from design tokens
        QColor bgRest = FLUENT_BUTTON_COLOR(variantStr, "rest");
        QColor bgHover = FLUENT_BUTTON_COLOR(variantStr, "hover");
        QColor bgPressed = FLUENT_BUTTON_COLOR(variantStr, "pressed");
        QColor textColor =
            FLUENT_COLOR(QString("button.%1.text").arg(variantStr));

        // Apply colors
        palette.setColor(QPalette::Button, bgRest);
        palette.setColor(QPalette::ButtonText, textColor);
        setPalette(palette);

        // Store hover/pressed colors for effects
        setProperty("hoverColor", bgHover);
        setProperty("pressedColor", bgPressed);
    }

    void setupHoverEffects() {
        // Connect to token changes
        connect(&FluentDesignTokenUtils::instance(),
                &FluentDesignTokenUtils::tokenChanged, this,
                &TokenizedButton::onTokenChanged);
    }

    QString variantToString(ButtonVariant variant) const {
        switch (variant) {
            case Primary:
                return "primary";
            case Secondary:
                return "secondary";
            case Accent:
                return "accent";
            case Subtle:
                return "subtle";
            default:
                return "primary";
        }
    }

    QString sizeToString(ButtonSize size) const {
        switch (size) {
            case Small:
                return "small";
            case Medium:
                return "medium";
            case Large:
                return "large";
            default:
                return "medium";
        }
    }

private:
    ButtonVariant m_variant;
    ButtonSize m_size;
};

// Example card component using design tokens
class TokenizedCard : public QFrame {
    Q_OBJECT

public:
    enum CardSize { Small, Medium, Large };

    TokenizedCard(CardSize size = Medium, QWidget* parent = nullptr)
        : QFrame(parent), m_size(size) {
        applyDesignTokens();
        setupLayout();
    }

    void setCardSize(CardSize size) {
        if (m_size != size) {
            m_size = size;
            applyDesignTokens();
        }
    }

private:
    void applyDesignTokens() {
        // Apply background color
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, FLUENT_COLOR("card.background"));
        setPalette(palette);
        setAutoFillBackground(true);

        // Apply border
        setFrameStyle(QFrame::Box);
        setLineWidth(FLUENT_SIZE("card.borderWidth"));

        // Apply border radius (would need custom painting for full effect)
        int borderRadius = FLUENT_SIZE("card.borderRadius");
        setStyleSheet(
            QString("QFrame { border-radius: %1px; }").arg(borderRadius));

        // Apply padding based on size
        QString sizeStr = sizeToString(m_size);
        int padding = FLUENT_SIZE(QString("card.padding.%1").arg(sizeStr));
        setContentsMargins(padding, padding, padding, padding);
    }

    void setupLayout() {
        auto* layout = new QVBoxLayout(this);

        // Card header
        auto* headerLabel = new QLabel("Card Title");
        headerLabel->setFont(FLUENT_FONT("card.header.fontSize"));
        headerLabel->setStyleSheet(
            QString("color: %1;")
                .arg(FLUENT_COLOR("color.text.primary").name()));
        layout->addWidget(headerLabel);

        // Card body
        auto* bodyLabel = new QLabel(
            "This is the card content area. It uses design tokens for "
            "consistent styling.");
        bodyLabel->setFont(FLUENT_FONT("card.body.fontSize"));
        bodyLabel->setStyleSheet(
            QString("color: %1;")
                .arg(FLUENT_COLOR("color.text.secondary").name()));
        bodyLabel->setWordWrap(true);
        layout->addWidget(bodyLabel);

        // Card actions
        auto* actionsLayout = new QHBoxLayout();
        actionsLayout->addStretch();

        auto* actionButton = new TokenizedButton(
            "Action", TokenizedButton::Primary, TokenizedButton::Small);
        actionsLayout->addWidget(actionButton);

        layout->addLayout(actionsLayout);
    }

    QString sizeToString(CardSize size) const {
        switch (size) {
            case Small:
                return "small";
            case Medium:
                return "medium";
            case Large:
                return "large";
            default:
                return "medium";
        }
    }

private:
    CardSize m_size;
};

// Main example window
class DesignTokenExampleWindow : public QMainWindow {
    Q_OBJECT

public:
    DesignTokenExampleWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupTokenDemo();
    }

private slots:
    void onThemeChanged() {
        // Theme changes are automatically handled by the token system
        qDebug() << "Theme changed - components will update automatically";
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);

        // Title
        auto* titleLabel = new QLabel("FluentUI Design Token System Demo");
        titleLabel->setFont(FLUENT_TITLE_FONT(1));
        titleLabel->setStyleSheet(
            QString("color: %1; margin-bottom: %2px;")
                .arg(FLUENT_COLOR("color.text.primary").name())
                .arg(FLUENT_SPACING("l")));
        mainLayout->addWidget(titleLabel);

        // Color palette demo
        setupColorPaletteDemo(mainLayout);

        // Typography demo
        setupTypographyDemo(mainLayout);

        // Component demo
        setupComponentDemo(mainLayout);

        // Spacing demo
        setupSpacingDemo(mainLayout);

        setWindowTitle("FluentUI Design Tokens Example");
        resize(800, 600);
    }

    void setupColorPaletteDemo(QVBoxLayout* mainLayout) {
        auto* colorGroup = new QGroupBox("Color Palette");
        colorGroup->setFont(FLUENT_FONT("typography.subtitle1"));
        auto* colorLayout = new QGridLayout(colorGroup);

        // Brand colors
        for (int i = 0; i < 8; ++i) {
            int shade = 20 + (i * 20);
            QColor color = FLUENT_BRAND_COLOR(shade);

            auto* colorLabel = new QLabel(QString("Brand %1").arg(shade));
            colorLabel->setStyleSheet(
                QString("background-color: %1; color: white; padding: 8px; "
                        "border-radius: 4px;")
                    .arg(color.name()));
            colorLayout->addWidget(colorLabel, 0, i);
        }

        // Neutral colors
        for (int i = 0; i < 8; ++i) {
            int shade = i * 8;
            QColor color = FLUENT_NEUTRAL_COLOR(shade);

            auto* colorLabel = new QLabel(QString("Neutral %1").arg(shade));
            colorLabel->setStyleSheet(
                QString("background-color: %1; color: %2; padding: 8px; "
                        "border-radius: 4px;")
                    .arg(color.name())
                    .arg(shade > 24 ? "white" : "black"));
            colorLayout->addWidget(colorLabel, 1, i);
        }

        mainLayout->addWidget(colorGroup);
    }

    void setupTypographyDemo(QVBoxLayout* mainLayout) {
        auto* typoGroup = new QGroupBox("Typography Scale");
        typoGroup->setFont(FLUENT_FONT("typography.subtitle1"));
        auto* typoLayout = new QVBoxLayout(typoGroup);

        // Typography samples
        QStringList typoSamples = {"typography.display|Display Text",
                                   "typography.title1|Title 1 Text",
                                   "typography.title2|Title 2 Text",
                                   "typography.subtitle1|Subtitle 1 Text",
                                   "typography.body1|Body 1 Text",
                                   "typography.caption1|Caption 1 Text"};

        for (const QString& sample : typoSamples) {
            QStringList parts = sample.split('|');
            auto* label = new QLabel(parts[1]);
            label->setFont(FLUENT_FONT(parts[0]));
            label->setStyleSheet(
                QString("color: %1; margin: 4px 0;")
                    .arg(FLUENT_COLOR("color.text.primary").name()));
            typoLayout->addWidget(label);
        }

        mainLayout->addWidget(typoGroup);
    }

    void setupComponentDemo(QVBoxLayout* mainLayout) {
        auto* componentGroup = new QGroupBox("Component Examples");
        componentGroup->setFont(FLUENT_FONT("typography.subtitle1"));
        auto* componentLayout = new QVBoxLayout(componentGroup);

        // Button variants
        auto* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(
            new TokenizedButton("Primary", TokenizedButton::Primary));
        buttonLayout->addWidget(
            new TokenizedButton("Secondary", TokenizedButton::Secondary));
        buttonLayout->addWidget(
            new TokenizedButton("Accent", TokenizedButton::Accent));
        buttonLayout->addWidget(
            new TokenizedButton("Subtle", TokenizedButton::Subtle));
        buttonLayout->addStretch();
        componentLayout->addLayout(buttonLayout);

        // Button sizes
        auto* sizeLayout = new QHBoxLayout();
        sizeLayout->addWidget(new TokenizedButton(
            "Small", TokenizedButton::Primary, TokenizedButton::Small));
        sizeLayout->addWidget(new TokenizedButton(
            "Medium", TokenizedButton::Primary, TokenizedButton::Medium));
        sizeLayout->addWidget(new TokenizedButton(
            "Large", TokenizedButton::Primary, TokenizedButton::Large));
        sizeLayout->addStretch();
        componentLayout->addLayout(sizeLayout);

        // Cards
        auto* cardLayout = new QHBoxLayout();
        cardLayout->addWidget(new TokenizedCard(TokenizedCard::Small));
        cardLayout->addWidget(new TokenizedCard(TokenizedCard::Medium));
        cardLayout->addStretch();
        componentLayout->addLayout(cardLayout);

        mainLayout->addWidget(componentGroup);
    }

    void setupSpacingDemo(QVBoxLayout* mainLayout) {
        auto* spacingGroup = new QGroupBox("Spacing Scale (4px base unit)");
        spacingGroup->setFont(FLUENT_FONT("typography.subtitle1"));
        auto* spacingLayout = new QVBoxLayout(spacingGroup);

        QStringList spacingSizes = {"xs", "s", "m", "l", "xl", "xxl"};

        for (const QString& size : spacingSizes) {
            int spacing = FLUENT_SPACING(size);

            auto* spacingDemo = new QFrame();
            spacingDemo->setStyleSheet(
                QString("background-color: %1; margin: 2px 0;")
                    .arg(FLUENT_BRAND_COLOR(120).name()));
            spacingDemo->setFixedHeight(spacing);

            auto* spacingLabel =
                new QLabel(QString("spacing.%1 = %2px").arg(size).arg(spacing));
            spacingLabel->setFont(FLUENT_FONT("typography.caption1"));

            spacingLayout->addWidget(spacingLabel);
            spacingLayout->addWidget(spacingDemo);
        }

        mainLayout->addWidget(spacingGroup);
    }

    void setupTokenDemo() {
        // Initialize design tokens
        extern void initializeFluentDesignTokens();
        initializeFluentDesignTokens();

        // Connect to theme changes
        connect(&FluentAdvancedThemeManager::instance(),
                &FluentAdvancedThemeManager::themeChanged, this,
                &DesignTokenExampleWindow::onThemeChanged);

        qDebug() << "Design token demo initialized";
        qDebug()
            << "Available tokens:"
            << FluentDesignTokenUtils::instance().getAvailableTokens().size();
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    DesignTokenExampleWindow window;
    window.show();

    return app.exec();
}

#include "FluentDesignTokenExample.moc"
