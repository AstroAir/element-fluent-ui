// examples/InternationalizationExample.cpp
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Core/FluentCulturalAdaptation.h"
#include "FluentQt/Core/FluentI18n.h"
#include "FluentQt/Core/FluentLocaleFormatting.h"
#include "FluentQt/Core/FluentRTLSupport.h"

using namespace FluentQt;

class LocaleInfoWidget : public QWidget {
    Q_OBJECT

public:
    explicit LocaleInfoWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
    }

    void setLocale(const QLocale& locale) {
        m_currentLocale = locale;
        updateLocaleInfo();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Locale information display
        auto* infoGroup = new QGroupBox("Locale Information", this);
        auto* infoLayout = new QGridLayout(infoGroup);

        m_localeNameLabel = new QLabel(this);
        m_nativeNameLabel = new QLabel(this);
        m_languageLabel = new QLabel(this);
        m_countryLabel = new QLabel(this);
        m_scriptLabel = new QLabel(this);
        m_textDirectionLabel = new QLabel(this);
        m_numberSystemLabel = new QLabel(this);

        infoLayout->addWidget(new QLabel("Locale:", this), 0, 0);
        infoLayout->addWidget(m_localeNameLabel, 0, 1);
        infoLayout->addWidget(new QLabel("Native Name:", this), 1, 0);
        infoLayout->addWidget(m_nativeNameLabel, 1, 1);
        infoLayout->addWidget(new QLabel("Language:", this), 2, 0);
        infoLayout->addWidget(m_languageLabel, 2, 1);
        infoLayout->addWidget(new QLabel("Country:", this), 3, 0);
        infoLayout->addWidget(m_countryLabel, 3, 1);
        infoLayout->addWidget(new QLabel("Script:", this), 4, 0);
        infoLayout->addWidget(m_scriptLabel, 4, 1);
        infoLayout->addWidget(new QLabel("Text Direction:", this), 5, 0);
        infoLayout->addWidget(m_textDirectionLabel, 5, 1);
        infoLayout->addWidget(new QLabel("Number System:", this), 6, 0);
        infoLayout->addWidget(m_numberSystemLabel, 6, 1);

        // Cultural adaptation info
        auto* culturalGroup = new QGroupBox("Cultural Adaptation", this);
        auto* culturalLayout = new QVBoxLayout(culturalGroup);

        m_culturalInfoText = new QTextEdit(this);
        m_culturalInfoText->setMaximumHeight(150);
        m_culturalInfoText->setReadOnly(true);

        culturalLayout->addWidget(m_culturalInfoText);

        layout->addWidget(infoGroup);
        layout->addWidget(culturalGroup);
        layout->addStretch();
    }

    void updateLocaleInfo() {
        auto& rtlManager = Core::FluentRTLSupportManager::instance();
        auto& culturalManager =
            Core::FluentCulturalAdaptationManager::instance();

        // Basic locale information
        m_localeNameLabel->setText(m_currentLocale.name());
        m_nativeNameLabel->setText(m_currentLocale.nativeLanguageName() + " (" +
                                   m_currentLocale.nativeCountryName() + ")");
        m_languageLabel->setText(
            QLocale::languageToString(m_currentLocale.language()));
        m_countryLabel->setText(
            QLocale::countryToString(m_currentLocale.country()));
        m_scriptLabel->setText(
            QLocale::scriptToString(m_currentLocale.script()));

        // Text direction
        bool isRTL = rtlManager.isRTLLocale(m_currentLocale);
        m_textDirectionLabel->setText(isRTL ? "Right-to-Left (RTL)"
                                            : "Left-to-Right (LTR)");

        // Number system
        QString numberSystem = "Western";
        if (m_currentLocale.language() == QLocale::Arabic) {
            numberSystem = "Arabic-Indic";
        } else if (m_currentLocale.language() == QLocale::Persian) {
            numberSystem = "Persian";
        } else if (m_currentLocale.language() == QLocale::Hindi) {
            numberSystem = "Devanagari";
        }
        m_numberSystemLabel->setText(numberSystem);

        // Cultural adaptation information
        QString culturalInfo;

        // Cultural context
        auto culturalConfig =
            culturalManager.getCulturalConfig(m_currentLocale);
        culturalInfo +=
            QString("Cultural Context: %1\n")
                .arg(culturalConfig.context ==
                             Core::FluentCulturalContext::HighContext
                         ? "High Context"
                         : "Low Context");

        // Layout preferences
        culturalInfo +=
            QString("Layout Style: %1\n")
                .arg(culturalConfig.layoutStyle ==
                             Core::FluentCulturalLayoutStyle::Minimal
                         ? "Minimal"
                         : "Rich");

        // Interaction style
        culturalInfo += QString("Interaction Style: %1\n")
                            .arg(culturalConfig.interactionStyle ==
                                         Core::FluentCulturalInteraction::Formal
                                     ? "Formal"
                                     : "Casual");

        // Color preferences
        if (!culturalConfig.preferredColors.isEmpty()) {
            culturalInfo += QString("Preferred Colors: %1 colors defined\n")
                                .arg(culturalConfig.preferredColors.size());
        }

        // Typography preferences
        if (!culturalConfig.preferredFontFamilies.isEmpty()) {
            culturalInfo +=
                QString("Preferred Fonts: %1\n")
                    .arg(culturalConfig.preferredFontFamilies.join(", "));
        }

        culturalInfo += QString("Text Scaling: %1x\n")
                            .arg(culturalConfig.textScalingFactor, 0, 'f', 2);
        culturalInfo += QString("Density Factor: %1x\n")
                            .arg(culturalConfig.densityFactor, 0, 'f', 2);

        m_culturalInfoText->setText(culturalInfo);
    }

private:
    QLocale m_currentLocale;
    QLabel* m_localeNameLabel;
    QLabel* m_nativeNameLabel;
    QLabel* m_languageLabel;
    QLabel* m_countryLabel;
    QLabel* m_scriptLabel;
    QLabel* m_textDirectionLabel;
    QLabel* m_numberSystemLabel;
    QTextEdit* m_culturalInfoText;
};

class FormattingDemoWidget : public QWidget {
    Q_OBJECT

public:
    explicit FormattingDemoWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        updateTimer();
    }

    void setLocale(const QLocale& locale) {
        m_currentLocale = locale;
        updateFormattingExamples();
    }

private slots:
    void updateTimer() {
        updateFormattingExamples();
        QTimer::singleShot(1000, this, &FormattingDemoWidget::updateTimer);
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Number formatting examples
        auto* numberGroup = new QGroupBox("Number Formatting", this);
        auto* numberLayout = new QGridLayout(numberGroup);

        m_integerLabel = new QLabel(this);
        m_decimalLabel = new QLabel(this);
        m_currencyLabel = new QLabel(this);
        m_percentageLabel = new QLabel(this);
        m_compactLabel = new QLabel(this);
        m_ordinalLabel = new QLabel(this);

        numberLayout->addWidget(new QLabel("Integer:", this), 0, 0);
        numberLayout->addWidget(m_integerLabel, 0, 1);
        numberLayout->addWidget(new QLabel("Decimal:", this), 1, 0);
        numberLayout->addWidget(m_decimalLabel, 1, 1);
        numberLayout->addWidget(new QLabel("Currency:", this), 2, 0);
        numberLayout->addWidget(m_currencyLabel, 2, 1);
        numberLayout->addWidget(new QLabel("Percentage:", this), 3, 0);
        numberLayout->addWidget(m_percentageLabel, 3, 1);
        numberLayout->addWidget(new QLabel("Compact:", this), 4, 0);
        numberLayout->addWidget(m_compactLabel, 4, 1);
        numberLayout->addWidget(new QLabel("Ordinal:", this), 5, 0);
        numberLayout->addWidget(m_ordinalLabel, 5, 1);

        // Date/time formatting examples
        auto* dateTimeGroup = new QGroupBox("Date/Time Formatting", this);
        auto* dateTimeLayout = new QGridLayout(dateTimeGroup);

        m_dateLabel = new QLabel(this);
        m_timeLabel = new QLabel(this);
        m_dateTimeLabel = new QLabel(this);
        m_relativeLabel = new QLabel(this);
        m_durationLabel = new QLabel(this);

        dateTimeLayout->addWidget(new QLabel("Date:", this), 0, 0);
        dateTimeLayout->addWidget(m_dateLabel, 0, 1);
        dateTimeLayout->addWidget(new QLabel("Time:", this), 1, 0);
        dateTimeLayout->addWidget(m_timeLabel, 1, 1);
        dateTimeLayout->addWidget(new QLabel("Date/Time:", this), 2, 0);
        dateTimeLayout->addWidget(m_dateTimeLabel, 2, 1);
        dateTimeLayout->addWidget(new QLabel("Relative:", this), 3, 0);
        dateTimeLayout->addWidget(m_relativeLabel, 3, 1);
        dateTimeLayout->addWidget(new QLabel("Duration:", this), 4, 0);
        dateTimeLayout->addWidget(m_durationLabel, 4, 1);

        // Text formatting examples
        auto* textGroup = new QGroupBox("Text Formatting", this);
        auto* textLayout = new QGridLayout(textGroup);

        m_listLabel = new QLabel(this);
        m_nameLabel = new QLabel(this);
        m_addressLabel = new QLabel(this);
        m_phoneLabel = new QLabel(this);

        textLayout->addWidget(new QLabel("List:", this), 0, 0);
        textLayout->addWidget(m_listLabel, 0, 1);
        textLayout->addWidget(new QLabel("Name:", this), 1, 0);
        textLayout->addWidget(m_nameLabel, 1, 1);
        textLayout->addWidget(new QLabel("Address:", this), 2, 0);
        textLayout->addWidget(m_addressLabel, 2, 1);
        textLayout->addWidget(new QLabel("Phone:", this), 3, 0);
        textLayout->addWidget(m_phoneLabel, 3, 1);

        layout->addWidget(numberGroup);
        layout->addWidget(dateTimeGroup);
        layout->addWidget(textGroup);
        layout->addStretch();
    }

    void updateFormattingExamples() {
        auto& formatter = Core::FluentLocaleFormattingManager::instance();

        // Number formatting
        auto intResult = formatter.formatInteger(123456, m_currentLocale);
        m_integerLabel->setText(intResult.formattedText);

        auto decimalResult =
            formatter.formatNumber(123456.789, m_currentLocale);
        m_decimalLabel->setText(decimalResult.formattedText);

        auto currencyResult =
            formatter.formatCurrency(1234.56, "USD", m_currentLocale);
        m_currencyLabel->setText(currencyResult.formattedText);

        auto percentResult =
            formatter.formatPercentage(0.1234, m_currentLocale);
        m_percentageLabel->setText(percentResult.formattedText);

        auto compactResult =
            formatter.formatCompactNumber(1234567, m_currentLocale);
        m_compactLabel->setText(compactResult.formattedText);

        auto ordinalResult = formatter.formatOrdinal(42, m_currentLocale);
        m_ordinalLabel->setText(ordinalResult.formattedText);

        // Date/time formatting
        QDateTime now = QDateTime::currentDateTime();

        auto dateResult = formatter.formatDate(now.date(), m_currentLocale);
        m_dateLabel->setText(dateResult.formattedText);

        auto timeResult = formatter.formatTime(now.time(), m_currentLocale);
        m_timeLabel->setText(timeResult.formattedText);

        auto dateTimeResult = formatter.formatDateTime(now, m_currentLocale);
        m_dateTimeLabel->setText(dateTimeResult.formattedText);

        auto relativeResult = formatter.formatRelativeDateTime(
            now.addSecs(-3600), m_currentLocale);
        m_relativeLabel->setText(relativeResult.formattedText);

        auto durationResult = formatter.formatDuration(
            3661000, m_currentLocale);  // 1 hour, 1 minute, 1 second
        m_durationLabel->setText(durationResult.formattedText);

        // Text formatting
        QStringList items = {"Apple", "Banana", "Cherry", "Date"};
        auto listResult = formatter.formatList(items, m_currentLocale);
        m_listLabel->setText(listResult.formattedText);

        auto nameResult =
            formatter.formatName("John", "Smith", m_currentLocale);
        m_nameLabel->setText(nameResult.formattedText);

        QMap<QString, QString> addressComponents;
        addressComponents["street"] = "123 Main St";
        addressComponents["city"] = "New York";
        addressComponents["region"] = "NY";
        addressComponents["postalCode"] = "10001";
        addressComponents["country"] = "USA";
        auto addressResult =
            formatter.formatAddress(addressComponents, m_currentLocale);
        m_addressLabel->setText(addressResult.formattedText);

        auto phoneResult =
            formatter.formatPhoneNumber("+1-555-123-4567", m_currentLocale);
        m_phoneLabel->setText(phoneResult.formattedText);
    }

private:
    QLocale m_currentLocale;
    QLabel* m_integerLabel;
    QLabel* m_decimalLabel;
    QLabel* m_currencyLabel;
    QLabel* m_percentageLabel;
    QLabel* m_compactLabel;
    QLabel* m_ordinalLabel;
    QLabel* m_dateLabel;
    QLabel* m_timeLabel;
    QLabel* m_dateTimeLabel;
    QLabel* m_relativeLabel;
    QLabel* m_durationLabel;
    QLabel* m_listLabel;
    QLabel* m_nameLabel;
    QLabel* m_addressLabel;
    QLabel* m_phoneLabel;
};

class RTLDemoWidget : public QWidget {
    Q_OBJECT

public:
    explicit RTLDemoWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
    }

    void setLocale(const QLocale& locale) {
        m_currentLocale = locale;
        updateRTLDemo();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // RTL text examples
        auto* textGroup = new QGroupBox("RTL Text Examples", this);
        auto* textLayout = new QVBoxLayout(textGroup);

        m_arabicText = new QLabel("مرحبا بالعالم - Hello World", this);
        m_hebrewText = new QLabel("שלום עולם - Hello World", this);
        m_mixedText =
            new QLabel("English text with عربي and עברית mixed in", this);

        textLayout->addWidget(new QLabel("Arabic:", this));
        textLayout->addWidget(m_arabicText);
        textLayout->addWidget(new QLabel("Hebrew:", this));
        textLayout->addWidget(m_hebrewText);
        textLayout->addWidget(new QLabel("Mixed:", this));
        textLayout->addWidget(m_mixedText);

        // RTL layout demo
        auto* layoutGroup = new QGroupBox("RTL Layout Demo", this);
        auto* layoutDemo = new QHBoxLayout(layoutGroup);

        m_button1 = new QPushButton("Button 1", this);
        m_button2 = new QPushButton("Button 2", this);
        m_button3 = new QPushButton("Button 3", this);

        layoutDemo->addWidget(m_button1);
        layoutDemo->addWidget(m_button2);
        layoutDemo->addWidget(m_button3);

        // RTL controls
        auto* controlsGroup = new QGroupBox("RTL Controls", this);
        auto* controlsLayout = new QVBoxLayout(controlsGroup);

        m_enableRTLCheck = new QCheckBox("Enable RTL Mode", this);
        m_mirrorLayoutCheck = new QCheckBox("Mirror Layout", this);
        m_adaptTextCheck = new QCheckBox("Adapt Text Direction", this);

        connect(m_enableRTLCheck, &QCheckBox::toggled, this,
                &RTLDemoWidget::onRTLToggled);
        connect(m_mirrorLayoutCheck, &QCheckBox::toggled, this,
                &RTLDemoWidget::onMirrorToggled);
        connect(m_adaptTextCheck, &QCheckBox::toggled, this,
                &RTLDemoWidget::onTextAdaptToggled);

        controlsLayout->addWidget(m_enableRTLCheck);
        controlsLayout->addWidget(m_mirrorLayoutCheck);
        controlsLayout->addWidget(m_adaptTextCheck);

        layout->addWidget(textGroup);
        layout->addWidget(layoutGroup);
        layout->addWidget(controlsGroup);
        layout->addStretch();
    }

    void updateRTLDemo() {
        auto& rtlManager = Core::FluentRTLSupportManager::instance();

        bool isRTL = rtlManager.isRTLLocale(m_currentLocale);
        m_enableRTLCheck->setChecked(isRTL);

        if (isRTL) {
            // Apply RTL adaptations
            rtlManager.adaptWidget(this, true);

            // Update text alignment
            m_arabicText->setAlignment(Qt::AlignRight);
            m_hebrewText->setAlignment(Qt::AlignRight);
            m_mixedText->setAlignment(Qt::AlignRight);
        } else {
            // Apply LTR layout
            m_arabicText->setAlignment(Qt::AlignLeft);
            m_hebrewText->setAlignment(Qt::AlignLeft);
            m_mixedText->setAlignment(Qt::AlignLeft);
        }
    }

private slots:
    void onRTLToggled(bool enabled) {
        auto& rtlManager = Core::FluentRTLSupportManager::instance();

        if (enabled) {
            rtlManager.setRTLMode(Core::FluentRTLMode::Forced);
            rtlManager.adaptWidget(this, true);
        } else {
            rtlManager.setRTLMode(Core::FluentRTLMode::Disabled);
        }
    }

    void onMirrorToggled(bool enabled) {
        if (enabled) {
            // Mirror the button layout
            auto* layout =
                qobject_cast<QHBoxLayout*>(m_button1->parentWidget()->layout());
            if (layout) {
                layout->setDirection(QBoxLayout::RightToLeft);
            }
        } else {
            auto* layout =
                qobject_cast<QHBoxLayout*>(m_button1->parentWidget()->layout());
            if (layout) {
                layout->setDirection(QBoxLayout::LeftToRight);
            }
        }
    }

    void onTextAdaptToggled(bool enabled) {
        if (enabled) {
            // Apply text direction based on content
            auto& rtlManager = Core::FluentRTLSupportManager::instance();

            auto arabicDir =
                rtlManager.detectTextDirection(m_arabicText->text());
            auto hebrewDir =
                rtlManager.detectTextDirection(m_hebrewText->text());
            auto mixedDir = rtlManager.detectTextDirection(m_mixedText->text());

            m_arabicText->setAlignment(
                arabicDir == Core::FluentTextDirection::RightToLeft
                    ? Qt::AlignRight
                    : Qt::AlignLeft);
            m_hebrewText->setAlignment(
                hebrewDir == Core::FluentTextDirection::RightToLeft
                    ? Qt::AlignRight
                    : Qt::AlignLeft);
            m_mixedText->setAlignment(
                mixedDir == Core::FluentTextDirection::RightToLeft
                    ? Qt::AlignRight
                    : Qt::AlignLeft);
        }
    }

private:
    QLocale m_currentLocale;
    QLabel* m_arabicText;
    QLabel* m_hebrewText;
    QLabel* m_mixedText;
    QPushButton* m_button1;
    QPushButton* m_button2;
    QPushButton* m_button3;
    QCheckBox* m_enableRTLCheck;
    QCheckBox* m_mirrorLayoutCheck;
    QCheckBox* m_adaptTextCheck;
};

class InternationalizationMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit InternationalizationMainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        setupUI();
        connectSignals();

        // Set initial locale
        setCurrentLocale(QLocale::system());
    }

private slots:
    void onLocaleChanged() {
        QString localeName = m_localeCombo->currentData().toString();
        QLocale locale(localeName);
        setCurrentLocale(locale);
    }

    void onCulturalAdaptationToggled(bool enabled) {
        auto& culturalManager =
            Core::FluentCulturalAdaptationManager::instance();

        if (enabled) {
            culturalManager.adaptWidget(this, m_currentLocale);
        }
    }

private:
    void setupUI() {
        setWindowTitle("FluentQt Internationalization Demo");
        setMinimumSize(1000, 700);

        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);

        // Locale selection
        auto* localeGroup = new QGroupBox("Locale Selection", this);
        auto* localeLayout = new QHBoxLayout(localeGroup);

        m_localeCombo = new QComboBox(this);
        populateLocaleCombo();

        m_culturalAdaptationCheck =
            new QCheckBox("Enable Cultural Adaptation", this);

        localeLayout->addWidget(new QLabel("Locale:", this));
        localeLayout->addWidget(m_localeCombo);
        localeLayout->addStretch();
        localeLayout->addWidget(m_culturalAdaptationCheck);

        // Tab widget for different demos
        auto* tabWidget = new QTabWidget(this);

        m_localeInfoWidget = new LocaleInfoWidget(this);
        m_formattingWidget = new FormattingDemoWidget(this);
        m_rtlWidget = new RTLDemoWidget(this);

        tabWidget->addTab(m_localeInfoWidget, "Locale Info");
        tabWidget->addTab(m_formattingWidget, "Formatting");
        tabWidget->addTab(m_rtlWidget, "RTL Support");

        mainLayout->addWidget(localeGroup);
        mainLayout->addWidget(tabWidget);

        // Status bar
        statusBar()->showMessage("Internationalization system ready");
    }

    void populateLocaleCombo() {
        // Add common locales
        QList<QLocale> locales = {
            QLocale(QLocale::English, QLocale::UnitedStates),
            QLocale(QLocale::English, QLocale::UnitedKingdom),
            QLocale(QLocale::Spanish, QLocale::Spain),
            QLocale(QLocale::French, QLocale::France),
            QLocale(QLocale::German, QLocale::Germany),
            QLocale(QLocale::Italian, QLocale::Italy),
            QLocale(QLocale::Portuguese, QLocale::Brazil),
            QLocale(QLocale::Russian, QLocale::Russia),
            QLocale(QLocale::Chinese, QLocale::China),
            QLocale(QLocale::Japanese, QLocale::Japan),
            QLocale(QLocale::Korean, QLocale::SouthKorea),
            QLocale(QLocale::Arabic, QLocale::SaudiArabia),
            QLocale(QLocale::Hebrew, QLocale::Israel),
            QLocale(QLocale::Hindi, QLocale::India),
            QLocale(QLocale::Thai, QLocale::Thailand)};

        for (const QLocale& locale : locales) {
            QString displayName = QString("%1 (%2)").arg(
                locale.nativeLanguageName(), locale.nativeCountryName());
            m_localeCombo->addItem(displayName, locale.name());
        }
    }

    void connectSignals() {
        connect(m_localeCombo,
                QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                &InternationalizationMainWindow::onLocaleChanged);
        connect(m_culturalAdaptationCheck, &QCheckBox::toggled, this,
                &InternationalizationMainWindow::onCulturalAdaptationToggled);
    }

    void setCurrentLocale(const QLocale& locale) {
        m_currentLocale = locale;

        // Update all demo widgets
        m_localeInfoWidget->setLocale(locale);
        m_formattingWidget->setLocale(locale);
        m_rtlWidget->setLocale(locale);

        // Update status bar
        statusBar()->showMessage(
            QString("Current locale: %1").arg(locale.name()));

        // Update window title with locale
        setWindowTitle(QString("FluentQt Internationalization Demo - %1")
                           .arg(locale.nativeLanguageName()));
    }

private:
    QLocale m_currentLocale;
    QComboBox* m_localeCombo;
    QCheckBox* m_culturalAdaptationCheck;
    LocaleInfoWidget* m_localeInfoWidget;
    FormattingDemoWidget* m_formattingWidget;
    RTLDemoWidget* m_rtlWidget;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize internationalization system
    auto& i18nManager = Core::FluentTranslationManager::instance();
    auto& culturalManager = Core::FluentCulturalAdaptationManager::instance();
    auto& rtlManager = Core::FluentRTLSupportManager::instance();
    auto& formattingManager = Core::FluentLocaleFormattingManager::instance();

    // Set up default configurations
    i18nManager.initialize();
    culturalManager.enableCulturalLearning(true);
    rtlManager.setRTLMode(Core::FluentRTLMode::Automatic);
    formattingManager.enableCaching(true);

    InternationalizationMainWindow window;
    window.show();

    return app.exec();
}

#include "InternationalizationExample.moc"
