// examples/AdditionalFormControlsExample.cpp
// Demonstration of FluentSlider and FluentToggleSwitch components

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>

// FluentQt includes
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Components/FluentToggleSwitch.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class AdditionalFormControlsWindow : public QMainWindow {
    Q_OBJECT

public:
    AdditionalFormControlsWindow(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        setupUI();
        setupConnections();

        setWindowTitle("FluentQt Additional Form Controls Demo");
        resize(1000, 700);
    }

private slots:
    void onSliderValueChanged(qreal value) {
        auto* slider = qobject_cast<Components::FluentSlider*>(sender());
        if (slider == m_volumeSlider) {
            m_volumeLabel->setText(QString("Volume: %1%").arg(qRound(value)));
        } else if (slider == m_brightnessSlider) {
            m_brightnessLabel->setText(
                QString("Brightness: %1%").arg(qRound(value)));
        } else if (slider == m_temperatureSlider) {
            m_temperatureLabel->setText(
                QString("Temperature: %1°C").arg(value, 0, 'f', 1));
        }
    }

    void onRangeSliderValuesChanged(qreal lower, qreal upper) {
        m_priceRangeLabel->setText(QString("Price Range: $%1 - $%2")
                                       .arg(qRound(lower))
                                       .arg(qRound(upper)));
    }

    void onToggleSwitchChanged(bool checked) {
        auto* toggle = qobject_cast<Components::FluentToggleSwitch*>(sender());

        if (toggle == m_notificationsToggle) {
            m_notificationsStatus->setText(checked ? "Notifications: Enabled"
                                                   : "Notifications: Disabled");
        } else if (toggle == m_darkModeToggle) {
            auto& theme = Styling::FluentTheme::instance();
            theme.setDarkMode(checked);
            m_darkModeStatus->setText(checked ? "Dark Mode: On"
                                              : "Dark Mode: Off");
        } else if (toggle == m_autoSaveToggle) {
            m_autoSaveStatus->setText(checked ? "Auto-save: Enabled"
                                              : "Auto-save: Disabled");
        }
    }

    void onAnimationDemo() {
        // Animate sliders to random values
        m_volumeSlider->animateToValue(qrand() % 101);
        m_brightnessSlider->animateToValue(qrand() % 101);
        m_temperatureSlider->animateToValue(15.0 + (qrand() % 251) / 10.0);

        // Animate range slider
        qreal lower = qrand() % 500;
        qreal upper = lower + (qrand() % (1000 - lower));
        m_priceRangeSlider->animateToValues(lower, upper);
    }

    void onResetValues() {
        m_volumeSlider->setValue(50);
        m_brightnessSlider->setValue(75);
        m_temperatureSlider->setValue(20.0);
        m_priceRangeSlider->setValues(100, 500);

        m_notificationsToggle->setChecked(true);
        m_darkModeToggle->setChecked(false);
        m_autoSaveToggle->setChecked(false);
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        // Title
        auto* titleLabel =
            new QLabel("FluentQt Additional Form Controls", this);
        auto& theme = Styling::FluentTheme::instance();
        titleLabel->setFont(theme.displayFont());
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Create demo sections
        auto* contentLayout = new QHBoxLayout();

        // Left column - Sliders
        setupSlidersSection(contentLayout);

        // Right column - Toggle switches
        setupToggleSwitchesSection(contentLayout);

        mainLayout->addLayout(contentLayout, 1);

        // Control buttons
        setupControlButtons(mainLayout);
    }

    void setupSlidersSection(QHBoxLayout* parentLayout) {
        auto* slidersPanel =
            new Components::FluentPanel("Slider Components", this);
        slidersPanel->setPanelType(Components::FluentPanelType::Card);
        slidersPanel->setElevation(Components::FluentPanelElevation::Medium);

        auto* slidersLayout = slidersPanel->createVerticalLayout();

        // Volume slider (horizontal)
        auto* volumeGroup = new QGroupBox("Volume Control", this);
        auto* volumeLayout = new QVBoxLayout(volumeGroup);

        m_volumeSlider = new Components::FluentSlider(0, 100, this);
        m_volumeSlider->setValue(50);
        m_volumeSlider->setTickPosition(
            Components::FluentSliderTickPosition::Below);
        m_volumeSlider->setTickInterval(25);
        m_volumeSlider->setShowLabels(true);
        m_volumeSlider->setShowTooltip(true);

        // Add custom ticks
        m_volumeSlider->addTick(0, "Mute");
        m_volumeSlider->addTick(25, "Low");
        m_volumeSlider->addTick(50, "Medium");
        m_volumeSlider->addTick(75, "High");
        m_volumeSlider->addTick(100, "Max");

        m_volumeLabel = new QLabel("Volume: 50%", this);

        volumeLayout->addWidget(m_volumeSlider);
        volumeLayout->addWidget(m_volumeLabel);
        slidersLayout->addWidget(volumeGroup);

        // Brightness slider (horizontal)
        auto* brightnessGroup = new QGroupBox("Brightness Control", this);
        auto* brightnessLayout = new QVBoxLayout(brightnessGroup);

        m_brightnessSlider = new Components::FluentSlider(0, 100, this);
        m_brightnessSlider->setValue(75);
        m_brightnessSlider->setTickPosition(
            Components::FluentSliderTickPosition::Above);
        m_brightnessSlider->setTickInterval(20);
        m_brightnessSlider->setSnapToTicks(true);

        m_brightnessLabel = new QLabel("Brightness: 75%", this);

        brightnessLayout->addWidget(m_brightnessSlider);
        brightnessLayout->addWidget(m_brightnessLabel);
        slidersLayout->addWidget(brightnessGroup);

        // Temperature slider (vertical)
        auto* temperatureGroup = new QGroupBox("Temperature Control", this);
        auto* temperatureLayout = new QHBoxLayout(temperatureGroup);

        m_temperatureSlider = new Components::FluentSlider(
            Components::FluentSliderOrientation::Vertical, this);
        m_temperatureSlider->setRange(15.0, 40.0);
        m_temperatureSlider->setValue(20.0);
        m_temperatureSlider->setStep(0.5);
        m_temperatureSlider->setTickPosition(
            Components::FluentSliderTickPosition::Both);
        m_temperatureSlider->setTickInterval(5);
        m_temperatureSlider->setShowLabels(true);
        m_temperatureSlider->setValueFormatter(
            [](qreal value) { return QString("%1°C").arg(value, 0, 'f', 1); });

        m_temperatureLabel = new QLabel("Temperature: 20.0°C", this);

        temperatureLayout->addWidget(m_temperatureSlider);
        temperatureLayout->addWidget(m_temperatureLabel);
        slidersLayout->addWidget(temperatureGroup);

        // Price range slider (range mode)
        auto* priceGroup = new QGroupBox("Price Range", this);
        auto* priceLayout = new QVBoxLayout(priceGroup);

        m_priceRangeSlider = new Components::FluentSlider(0, 1000, this);
        m_priceRangeSlider->setMode(Components::FluentSliderMode::Range);
        m_priceRangeSlider->setValues(100, 500);
        m_priceRangeSlider->setTickPosition(
            Components::FluentSliderTickPosition::Below);
        m_priceRangeSlider->setTickInterval(100);
        m_priceRangeSlider->setValueFormatter(
            [](qreal value) { return QString("$%1").arg(qRound(value)); });

        m_priceRangeLabel = new QLabel("Price Range: $100 - $500", this);

        priceLayout->addWidget(m_priceRangeSlider);
        priceLayout->addWidget(m_priceRangeLabel);
        slidersLayout->addWidget(priceGroup);

        parentLayout->addWidget(slidersPanel);
    }

    void setupToggleSwitchesSection(QHBoxLayout* parentLayout) {
        auto* togglesPanel =
            new Components::FluentPanel("Toggle Switch Components", this);
        togglesPanel->setPanelType(Components::FluentPanelType::Surface);
        togglesPanel->setElevation(Components::FluentPanelElevation::Medium);

        auto* togglesLayout = togglesPanel->createVerticalLayout();

        // Notifications toggle (medium size)
        auto* notificationsGroup = new QGroupBox("Notifications", this);
        auto* notificationsLayout = new QVBoxLayout(notificationsGroup);

        m_notificationsToggle =
            new Components::FluentToggleSwitch("Enable notifications", this);
        m_notificationsToggle->setChecked(true);
        m_notificationsToggle->setSize(
            Components::FluentToggleSwitchSize::Medium);
        m_notificationsToggle->setOnText("On");
        m_notificationsToggle->setOffText("Off");
        m_notificationsToggle->setShowStateText(true);

        m_notificationsStatus = new QLabel("Notifications: Enabled", this);

        notificationsLayout->addWidget(m_notificationsToggle);
        notificationsLayout->addWidget(m_notificationsStatus);
        togglesLayout->addWidget(notificationsGroup);

        // Dark mode toggle (large size)
        auto* darkModeGroup = new QGroupBox("Appearance", this);
        auto* darkModeLayout = new QVBoxLayout(darkModeGroup);

        m_darkModeToggle =
            new Components::FluentToggleSwitch("Dark mode", this);
        m_darkModeToggle->setSize(Components::FluentToggleSwitchSize::Large);
        m_darkModeToggle->setLabelPosition(
            Components::FluentToggleSwitchLabelPosition::Left);

        m_darkModeStatus = new QLabel("Dark Mode: Off", this);

        darkModeLayout->addWidget(m_darkModeToggle);
        darkModeLayout->addWidget(m_darkModeStatus);
        togglesLayout->addWidget(darkModeGroup);

        // Auto-save toggle (small size)
        auto* autoSaveGroup = new QGroupBox("Auto-save", this);
        auto* autoSaveLayout = new QVBoxLayout(autoSaveGroup);

        m_autoSaveToggle =
            new Components::FluentToggleSwitch("Enable auto-save", this);
        m_autoSaveToggle->setSize(Components::FluentToggleSwitchSize::Small);
        m_autoSaveToggle->setLabelPosition(
            Components::FluentToggleSwitchLabelPosition::Above);

        m_autoSaveStatus = new QLabel("Auto-save: Disabled", this);

        autoSaveLayout->addWidget(m_autoSaveToggle);
        autoSaveLayout->addWidget(m_autoSaveStatus);
        togglesLayout->addWidget(autoSaveGroup);

        // Toggle switch sizes demo
        auto* sizesGroup = new QGroupBox("Toggle Switch Sizes", this);
        auto* sizesLayout = new QHBoxLayout(sizesGroup);

        auto* smallToggle = new Components::FluentToggleSwitch("Small", this);
        smallToggle->setSize(Components::FluentToggleSwitchSize::Small);
        smallToggle->setLabelPosition(
            Components::FluentToggleSwitchLabelPosition::Below);

        auto* mediumToggle = new Components::FluentToggleSwitch("Medium", this);
        mediumToggle->setSize(Components::FluentToggleSwitchSize::Medium);
        mediumToggle->setLabelPosition(
            Components::FluentToggleSwitchLabelPosition::Below);
        mediumToggle->setChecked(true);

        auto* largeToggle = new Components::FluentToggleSwitch("Large", this);
        largeToggle->setSize(Components::FluentToggleSwitchSize::Large);
        largeToggle->setLabelPosition(
            Components::FluentToggleSwitchLabelPosition::Below);

        sizesLayout->addWidget(smallToggle);
        sizesLayout->addWidget(mediumToggle);
        sizesLayout->addWidget(largeToggle);
        togglesLayout->addWidget(sizesGroup);

        parentLayout->addWidget(togglesPanel);
    }

    void setupControlButtons(QVBoxLayout* parentLayout) {
        auto* buttonLayout = new QHBoxLayout();

        auto* animateButton = new QPushButton("Animate Values", this);
        auto* resetButton = new QPushButton("Reset All", this);

        connect(animateButton, &QPushButton::clicked, this,
                &AdditionalFormControlsWindow::onAnimationDemo);
        connect(resetButton, &QPushButton::clicked, this,
                &AdditionalFormControlsWindow::onResetValues);

        buttonLayout->addWidget(animateButton);
        buttonLayout->addWidget(resetButton);
        buttonLayout->addStretch();

        parentLayout->addLayout(buttonLayout);
    }

    void setupConnections() {
        // Slider connections
        connect(m_volumeSlider, &Components::FluentSlider::valueChanged, this,
                &AdditionalFormControlsWindow::onSliderValueChanged);
        connect(m_brightnessSlider, &Components::FluentSlider::valueChanged,
                this, &AdditionalFormControlsWindow::onSliderValueChanged);
        connect(m_temperatureSlider, &Components::FluentSlider::valueChanged,
                this, &AdditionalFormControlsWindow::onSliderValueChanged);
        connect(m_priceRangeSlider, &Components::FluentSlider::valuesChanged,
                this,
                &AdditionalFormControlsWindow::onRangeSliderValuesChanged);

        // Toggle switch connections
        connect(m_notificationsToggle,
                &Components::FluentToggleSwitch::checkedChanged, this,
                &AdditionalFormControlsWindow::onToggleSwitchChanged);
        connect(m_darkModeToggle,
                &Components::FluentToggleSwitch::checkedChanged, this,
                &AdditionalFormControlsWindow::onToggleSwitchChanged);
        connect(m_autoSaveToggle,
                &Components::FluentToggleSwitch::checkedChanged, this,
                &AdditionalFormControlsWindow::onToggleSwitchChanged);
    }

private:
    // Sliders
    Components::FluentSlider* m_volumeSlider{nullptr};
    Components::FluentSlider* m_brightnessSlider{nullptr};
    Components::FluentSlider* m_temperatureSlider{nullptr};
    Components::FluentSlider* m_priceRangeSlider{nullptr};

    // Toggle switches
    Components::FluentToggleSwitch* m_notificationsToggle{nullptr};
    Components::FluentToggleSwitch* m_darkModeToggle{nullptr};
    Components::FluentToggleSwitch* m_autoSaveToggle{nullptr};

    // Status labels
    QLabel* m_volumeLabel{nullptr};
    QLabel* m_brightnessLabel{nullptr};
    QLabel* m_temperatureLabel{nullptr};
    QLabel* m_priceRangeLabel{nullptr};
    QLabel* m_notificationsStatus{nullptr};
    QLabel* m_darkModeStatus{nullptr};
    QLabel* m_autoSaveStatus{nullptr};
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    AdditionalFormControlsWindow window;
    window.show();

    return app.exec();
}

#include "AdditionalFormControlsExample.moc"
