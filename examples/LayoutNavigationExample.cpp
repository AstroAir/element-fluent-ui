// examples/LayoutNavigationExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QListWidget>
#include <QTreeWidget>
#include <QScrollArea>
#include <QGroupBox>
#include <QTabWidget>

#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentBreadcrumb.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class LayoutNavigationWindow : public QMainWindow {
    Q_OBJECT

public:
    LayoutNavigationWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupConnections();
        
        // Apply Fluent theme
        auto& theme = Styling::FluentTheme::instance();
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                     .arg(theme.color("backgroundPrimary").name()));
    }

private slots:
    void onPanelTypeChanged(int index) {
        auto type = static_cast<Components::FluentPanelType>(index);
        m_demoPanel->setPanelType(type);
    }
    
    void onElevationChanged(int index) {
        auto elevation = static_cast<Components::FluentPanelElevation>(index);
        m_demoPanel->setElevation(elevation);
    }
    
    void onCornerRadiusChanged(int index) {
        auto radius = static_cast<Components::FluentPanelCornerRadius>(index);
        m_demoPanel->setCornerRadius(radius);
    }
    
    void onPaddingChanged(int value) {
        m_demoPanel->setPadding(value);
    }
    
    void onCollapsibleToggled(bool checked) {
        m_demoPanel->setCollapsible(checked);
    }
    
    void onAnimatedToggled(bool checked) {
        m_demoPanel->setAnimated(checked);
    }
    
    void onBreadcrumbStyleChanged(int index) {
        auto style = static_cast<Components::FluentBreadcrumbStyle>(index);
        m_breadcrumb->setStyle(style);
    }
    
    void onBreadcrumbSeparatorChanged(int index) {
        auto separator = static_cast<Components::FluentBreadcrumbSeparator>(index);
        m_breadcrumb->setSeparator(separator);
    }
    
    void onBreadcrumbItemClicked(int index) {
        m_statusLabel->setText(QString("Navigated to: %1").arg(m_breadcrumb->itemText(index)));
    }
    
    void onAddBreadcrumbItem() {
        static int counter = 1;
        m_breadcrumb->addItem(QString("Item %1").arg(counter++));
    }
    
    void onRemoveBreadcrumbItem() {
        if (m_breadcrumb->itemCount() > 0) {
            m_breadcrumb->removeItem(m_breadcrumb->itemCount() - 1);
        }
    }
    
    void onThemeToggle() {
        auto& theme = Styling::FluentTheme::instance();
        theme.setDarkMode(!theme.isDarkMode());
        
        // Update window background
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                     .arg(theme.color("backgroundPrimary").name()));
    }
    
    void onCreateNewPanel() {
        static int panelCounter = 1;
        
        auto* newPanel = new Components::FluentPanel(QString("Dynamic Panel %1").arg(panelCounter++), this);
        newPanel->setCollapsible(true);
        
        auto* layout = newPanel->createVerticalLayout();
        layout->addWidget(new QLabel("This is a dynamically created panel", this));
        layout->addWidget(new QPushButton("Button in panel", this));
        layout->addWidget(new QTextEdit("Some content...", this));
        
        m_dynamicPanelsLayout->addWidget(newPanel);
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        
        // Title
        auto* titleLabel = new QLabel("Fluent UI Layout & Navigation Components Demo", this);
        auto& theme = Styling::FluentTheme::instance();
        titleLabel->setFont(theme.titleFont());
        titleLabel->setStyleSheet(QString("color: %1;").arg(theme.color("textPrimary").name()));
        mainLayout->addWidget(titleLabel);
        
        // Breadcrumb section
        setupBreadcrumbSection(mainLayout);
        
        // Main content area with splitter
        auto* splitter = new QSplitter(Qt::Horizontal, this);
        
        // Left panel - Controls
        setupControlsPanel(splitter);
        
        // Right panel - Demo area
        setupDemoArea(splitter);
        
        splitter->setStretchFactor(0, 0); // Controls don't stretch
        splitter->setStretchFactor(1, 1); // Demo area stretches
        splitter->setSizes({300, 500});
        
        mainLayout->addWidget(splitter, 1);
        
        // Status bar
        m_statusLabel = new QLabel("Ready", this);
        mainLayout->addWidget(m_statusLabel);
        
        setWindowTitle("Fluent UI Layout & Navigation Example");
        resize(1000, 700);
    }
    
    void setupBreadcrumbSection(QVBoxLayout* mainLayout) {
        auto* breadcrumbGroup = new QGroupBox("Breadcrumb Navigation", this);
        auto& theme = Styling::FluentTheme::instance();
        breadcrumbGroup->setFont(theme.subtitleFont());
        auto* breadcrumbLayout = new QVBoxLayout(breadcrumbGroup);
        
        // Breadcrumb component
        m_breadcrumb = new Components::FluentBreadcrumb(this);
        m_breadcrumb->addItem(QIcon(":/icons/home"), "Home");
        m_breadcrumb->addItem("Documents");
        m_breadcrumb->addItem("Projects");
        m_breadcrumb->addItem("FluentQt");
        m_breadcrumb->addItem("Examples");
        m_breadcrumb->setCurrentIndex(4);
        
        breadcrumbLayout->addWidget(m_breadcrumb);
        
        // Breadcrumb controls
        auto* breadcrumbControls = new QHBoxLayout();
        
        breadcrumbControls->addWidget(new QLabel("Style:", this));
        m_breadcrumbStyleCombo = new QComboBox(this);
        m_breadcrumbStyleCombo->addItems({"Default", "Compact", "Pills", "Underline"});
        breadcrumbControls->addWidget(m_breadcrumbStyleCombo);
        
        breadcrumbControls->addWidget(new QLabel("Separator:", this));
        m_breadcrumbSeparatorCombo = new QComboBox(this);
        m_breadcrumbSeparatorCombo->addItems({"Chevron", "Slash", "Backslash", "Pipe", "Arrow", "Dot"});
        breadcrumbControls->addWidget(m_breadcrumbSeparatorCombo);
        
        auto* addItemButton = new QPushButton("Add Item", this);
        breadcrumbControls->addWidget(addItemButton);
        
        auto* removeItemButton = new QPushButton("Remove Item", this);
        breadcrumbControls->addWidget(removeItemButton);
        
        breadcrumbControls->addStretch();
        breadcrumbLayout->addLayout(breadcrumbControls);
        
        mainLayout->addWidget(breadcrumbGroup);
        
        // Connect breadcrumb controls
        connect(m_breadcrumbStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &LayoutNavigationWindow::onBreadcrumbStyleChanged);
        connect(m_breadcrumbSeparatorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &LayoutNavigationWindow::onBreadcrumbSeparatorChanged);
        connect(m_breadcrumb, &Components::FluentBreadcrumb::itemClicked,
                this, &LayoutNavigationWindow::onBreadcrumbItemClicked);
        connect(addItemButton, &QPushButton::clicked, this, &LayoutNavigationWindow::onAddBreadcrumbItem);
        connect(removeItemButton, &QPushButton::clicked, this, &LayoutNavigationWindow::onRemoveBreadcrumbItem);
    }
    
    void setupControlsPanel(QSplitter* splitter) {
        auto* controlsPanel = new Components::FluentPanel("Panel Controls", this);
        controlsPanel->setCollapsible(true);
        controlsPanel->setMinimumWidth(280);
        controlsPanel->setMaximumWidth(350);
        
        auto* controlsLayout = controlsPanel->createVerticalLayout();
        controlsLayout->setSpacing(16);
        
        // Panel type
        auto* typeGroup = new QGroupBox("Panel Type", this);
        auto* typeLayout = new QVBoxLayout(typeGroup);
        m_panelTypeCombo = new QComboBox(this);
        m_panelTypeCombo->addItems({"Card", "Surface", "Acrylic", "Mica", "Sidebar", "Content", "Dialog", "Flyout"});
        m_panelTypeCombo->setCurrentIndex(0); // Card
        typeLayout->addWidget(m_panelTypeCombo);
        controlsLayout->addWidget(typeGroup);
        
        // Elevation
        auto* elevationGroup = new QGroupBox("Elevation", this);
        auto* elevationLayout = new QVBoxLayout(elevationGroup);
        m_elevationCombo = new QComboBox(this);
        m_elevationCombo->addItems({"None", "Low", "Medium", "High", "Very High"});
        m_elevationCombo->setCurrentIndex(2); // Medium
        elevationLayout->addWidget(m_elevationCombo);
        controlsLayout->addWidget(elevationGroup);
        
        // Corner radius
        auto* radiusGroup = new QGroupBox("Corner Radius", this);
        auto* radiusLayout = new QVBoxLayout(radiusGroup);
        m_cornerRadiusCombo = new QComboBox(this);
        m_cornerRadiusCombo->addItems({"None", "Small", "Medium", "Large", "Extra Large"});
        m_cornerRadiusCombo->setCurrentIndex(2); // Medium
        radiusLayout->addWidget(m_cornerRadiusCombo);
        controlsLayout->addWidget(radiusGroup);
        
        // Padding
        auto* paddingGroup = new QGroupBox("Padding", this);
        auto* paddingLayout = new QVBoxLayout(paddingGroup);
        m_paddingSpinBox = new QSpinBox(this);
        m_paddingSpinBox->setRange(0, 50);
        m_paddingSpinBox->setValue(16);
        m_paddingSpinBox->setSuffix("px");
        paddingLayout->addWidget(m_paddingSpinBox);
        controlsLayout->addWidget(paddingGroup);
        
        // Options
        auto* optionsGroup = new QGroupBox("Options", this);
        auto* optionsLayout = new QVBoxLayout(optionsGroup);
        
        m_collapsibleCheckBox = new QCheckBox("Collapsible", this);
        optionsLayout->addWidget(m_collapsibleCheckBox);
        
        m_animatedCheckBox = new QCheckBox("Animated", this);
        m_animatedCheckBox->setChecked(true);
        optionsLayout->addWidget(m_animatedCheckBox);
        
        controlsLayout->addWidget(optionsGroup);
        
        // Actions
        auto* actionsGroup = new QGroupBox("Actions", this);
        auto* actionsLayout = new QVBoxLayout(actionsGroup);
        
        auto* createPanelButton = new QPushButton("Create New Panel", this);
        actionsLayout->addWidget(createPanelButton);
        
        auto* themeButton = new QPushButton("Toggle Theme", this);
        actionsLayout->addWidget(themeButton);
        
        controlsLayout->addWidget(actionsGroup);
        
        controlsLayout->addStretch();
        
        splitter->addWidget(controlsPanel);
        
        // Connect controls
        connect(m_panelTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &LayoutNavigationWindow::onPanelTypeChanged);
        connect(m_elevationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &LayoutNavigationWindow::onElevationChanged);
        connect(m_cornerRadiusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &LayoutNavigationWindow::onCornerRadiusChanged);
        connect(m_paddingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &LayoutNavigationWindow::onPaddingChanged);
        connect(m_collapsibleCheckBox, &QCheckBox::toggled, this, &LayoutNavigationWindow::onCollapsibleToggled);
        connect(m_animatedCheckBox, &QCheckBox::toggled, this, &LayoutNavigationWindow::onAnimatedToggled);
        connect(createPanelButton, &QPushButton::clicked, this, &LayoutNavigationWindow::onCreateNewPanel);
        connect(themeButton, &QPushButton::clicked, this, &LayoutNavigationWindow::onThemeToggle);
    }
    
    void setupDemoArea(QSplitter* splitter) {
        auto* demoArea = new QWidget(this);
        auto* demoLayout = new QVBoxLayout(demoArea);
        demoLayout->setSpacing(16);
        
        // Demo panel
        m_demoPanel = new Components::FluentPanel("Demo Panel", this);
        m_demoPanel->setMinimumHeight(200);
        
        auto* panelLayout = m_demoPanel->createVerticalLayout();
        panelLayout->addWidget(new QLabel("This is a demo panel that you can configure using the controls on the left.", this));
        
        auto* tabWidget = new QTabWidget(this);
        tabWidget->addTab(new QTextEdit("Content for tab 1...", this), "Tab 1");
        tabWidget->addTab(new QListWidget(this), "Tab 2");
        tabWidget->addTab(new QTreeWidget(this), "Tab 3");
        
        panelLayout->addWidget(tabWidget);
        
        auto* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(new QPushButton("Action 1", this));
        buttonLayout->addWidget(new QPushButton("Action 2", this));
        buttonLayout->addWidget(new QPushButton("Action 3", this));
        buttonLayout->addStretch();
        
        panelLayout->addLayout(buttonLayout);
        
        demoLayout->addWidget(m_demoPanel);
        
        // Dynamic panels area
        auto* dynamicGroup = new QGroupBox("Dynamic Panels", this);
        m_dynamicPanelsLayout = new QVBoxLayout(dynamicGroup);
        m_dynamicPanelsLayout->addStretch();
        
        auto* scrollArea = new QScrollArea(this);
        scrollArea->setWidget(dynamicGroup);
        scrollArea->setWidgetResizable(true);
        scrollArea->setMinimumHeight(200);
        
        demoLayout->addWidget(scrollArea);
        
        splitter->addWidget(demoArea);
    }
    
    void setupConnections() {
        // Additional connections can be added here
    }

private:
    // Breadcrumb
    Components::FluentBreadcrumb* m_breadcrumb{nullptr};
    QComboBox* m_breadcrumbStyleCombo{nullptr};
    QComboBox* m_breadcrumbSeparatorCombo{nullptr};
    
    // Demo panel
    Components::FluentPanel* m_demoPanel{nullptr};
    
    // Controls
    QComboBox* m_panelTypeCombo{nullptr};
    QComboBox* m_elevationCombo{nullptr};
    QComboBox* m_cornerRadiusCombo{nullptr};
    QSpinBox* m_paddingSpinBox{nullptr};
    QCheckBox* m_collapsibleCheckBox{nullptr};
    QCheckBox* m_animatedCheckBox{nullptr};
    
    // Dynamic content
    QVBoxLayout* m_dynamicPanelsLayout{nullptr};
    
    // Status
    QLabel* m_statusLabel{nullptr};
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Initialize Fluent theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(QColor(0, 120, 215)); // Windows blue
    
    LayoutNavigationWindow window;
    window.show();
    
    return app.exec();
}

#include "LayoutNavigationExample.moc"
