// include/FluentQt/Accessibility/FluentScreenReaderManager.h
#pragma once

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QMutex>
#include <QQueue>
#include <QAccessible>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace FluentQt::Accessibility {

// Screen reader types
enum class FluentScreenReaderType {
    Unknown,
    NVDA,           // NVDA (Windows)
    JAWS,           // JAWS (Windows)
    WindowsNarrator, // Windows Narrator
    VoiceOver,      // VoiceOver (macOS)
    Orca,           // Orca (Linux)
    TalkBack,       // TalkBack (Android)
    Generic         // Generic screen reader
};

// Announcement priority levels
enum class FluentAnnouncementPriority {
    Low,            // Background information
    Medium,         // Standard announcements
    High,           // Important information
    Critical,       // Critical alerts
    Interrupt       // Interrupt current speech
};

// Live region types (ARIA live regions)
enum class FluentLiveRegionType {
    Off,            // No announcements
    Polite,         // Announce when user is idle
    Assertive,      // Announce immediately
    Atomic          // Announce entire region content
};

// ARIA roles for enhanced screen reader support
enum class FluentAriaRole {
    // Landmark roles
    Banner,
    Complementary,
    ContentInfo,
    Form,
    Main,
    Navigation,
    Region,
    Search,
    
    // Widget roles
    Alert,
    AlertDialog,
    Application,
    Article,
    Button,
    Cell,
    CheckBox,
    ColumnHeader,
    ComboBox,
    Dialog,
    Document,
    Feed,
    Figure,
    Grid,
    GridCell,
    Group,
    Heading,
    Img,
    Link,
    List,
    ListBox,
    ListItem,
    Log,
    Marquee,
    Menu,
    MenuBar,
    MenuItem,
    MenuItemCheckBox,
    MenuItemRadio,
    Option,
    Presentation,
    ProgressBar,
    Radio,
    RadioGroup,
    Row,
    RowGroup,
    RowHeader,
    ScrollBar,
    Separator,
    Slider,
    SpinButton,
    Status,
    Switch,
    Tab,
    Table,
    TabList,
    TabPanel,
    TextBox,
    Timer,
    ToolBar,
    ToolTip,
    Tree,
    TreeGrid,
    TreeItem
};

// ARIA properties and states
struct FluentAriaProperties {
    QString label;
    QString labelledBy;
    QString describedBy;
    QString controls;
    QString owns;
    QString flowTo;
    QString details;
    QString errorMessage;
    QString keyShortcuts;
    QString roleDescription;
    
    // States
    bool expanded{false};
    bool selected{false};
    bool checked{false};
    bool pressed{false};
    bool hidden{false};
    bool disabled{false};
    bool invalid{false};
    bool required{false};
    bool readOnly{false};
    bool multiSelectable{false};
    bool modal{false};
    bool busy{false};
    bool grabbed{false};
    bool dropEffect{false};
    
    // Values
    QString valueText;
    double valueNow{0.0};
    double valueMin{0.0};
    double valueMax{100.0};
    int posInSet{0};
    int setSize{0};
    int level{0};
    int colIndex{0};
    int rowIndex{0};
    int colSpan{1};
    int rowSpan{1};
    
    // Live region properties
    FluentLiveRegionType live{FluentLiveRegionType::Off};
    bool atomic{false};
    QString relevant{"additions text"};
};

// Announcement configuration
struct FluentAnnouncement {
    QString message;
    FluentAnnouncementPriority priority{FluentAnnouncementPriority::Medium};
    FluentLiveRegionType liveType{FluentLiveRegionType::Polite};
    QWidget* source{nullptr};
    std::chrono::steady_clock::time_point timestamp;
    bool interrupt{false};
    QString context;
    int retryCount{0};
};

// Screen reader detection result
struct FluentScreenReaderInfo {
    FluentScreenReaderType type{FluentScreenReaderType::Unknown};
    QString name;
    QString version;
    bool isActive{false};
    bool supportsAria{true};
    bool supportsLiveRegions{true};
    bool supportsLandmarks{true};
    QStringList supportedFeatures;
};

// Comprehensive screen reader support manager
class FluentScreenReaderManager : public QObject {
    Q_OBJECT

public:
    static FluentScreenReaderManager& instance();
    
    // Screen reader detection and configuration
    FluentScreenReaderInfo detectScreenReader();
    FluentScreenReaderType getActiveScreenReader() const { return m_activeScreenReader; }
    bool isScreenReaderActive() const { return m_screenReaderActive; }
    void setScreenReaderActive(bool active);
    
    // Announcement system
    void announce(const QString& message, 
                 FluentAnnouncementPriority priority = FluentAnnouncementPriority::Medium,
                 FluentLiveRegionType liveType = FluentLiveRegionType::Polite);
    void announceFromWidget(QWidget* widget, const QString& message,
                           FluentAnnouncementPriority priority = FluentAnnouncementPriority::Medium);
    void interruptAndAnnounce(const QString& message);
    void clearAnnouncementQueue();
    
    // ARIA support
    void setAriaRole(QWidget* widget, FluentAriaRole role);
    void setAriaProperties(QWidget* widget, const FluentAriaProperties& properties);
    void setAriaLabel(QWidget* widget, const QString& label);
    void setAriaDescription(QWidget* widget, const QString& description);
    void setAriaLabelledBy(QWidget* widget, const QString& elementId);
    void setAriaDescribedBy(QWidget* widget, const QString& elementId);
    void setAriaControls(QWidget* widget, const QString& controlsId);
    void setAriaExpanded(QWidget* widget, bool expanded);
    void setAriaSelected(QWidget* widget, bool selected);
    void setAriaChecked(QWidget* widget, bool checked);
    void setAriaPressed(QWidget* widget, bool pressed);
    void setAriaHidden(QWidget* widget, bool hidden);
    void setAriaDisabled(QWidget* widget, bool disabled);
    void setAriaInvalid(QWidget* widget, bool invalid);
    void setAriaRequired(QWidget* widget, bool required);
    void setAriaValue(QWidget* widget, double value, double min = 0.0, double max = 100.0);
    void setAriaValueText(QWidget* widget, const QString& valueText);
    void setAriaLevel(QWidget* widget, int level);
    void setAriaPosition(QWidget* widget, int posInSet, int setSize);
    
    // Live regions
    void setLiveRegion(QWidget* widget, FluentLiveRegionType type, bool atomic = false);
    void updateLiveRegion(QWidget* widget, const QString& content);
    void announceLiveRegionChange(QWidget* widget, const QString& change);
    
    // Landmark management
    void setLandmark(QWidget* widget, FluentAriaRole landmarkRole, const QString& label = "");
    void removeLandmark(QWidget* widget);
    QList<QWidget*> getLandmarks() const;
    void validateLandmarkStructure(QWidget* rootWidget);
    
    // Heading structure
    void setHeading(QWidget* widget, int level, const QString& text);
    void validateHeadingStructure(QWidget* rootWidget);
    QList<QWidget*> getHeadings() const;
    bool hasProperHeadingHierarchy(QWidget* rootWidget);
    
    // Form accessibility
    void associateLabel(QWidget* label, QWidget* control);
    void setFieldDescription(QWidget* field, const QString& description);
    void setFieldError(QWidget* field, const QString& error);
    void clearFieldError(QWidget* field);
    void announceFormValidation(QWidget* form, const QStringList& errors);
    
    // Status and alert management
    void announceStatus(const QString& status, bool polite = true);
    void announceAlert(const QString& alert);
    void announceError(const QString& error, QWidget* source = nullptr);
    void announceSuccess(const QString& success);
    void announceProgress(int percentage, const QString& description = "");
    
    // Navigation announcements
    void announcePageChange(const QString& pageTitle);
    void announceRegionEntered(const QString& regionName, FluentAriaRole role);
    void announceRegionExited(const QString& regionName);
    void announceFocusChange(QWidget* newFocus, QWidget* oldFocus);
    void announceSelectionChange(QWidget* widget, const QString& selection);
    
    // Content change announcements
    void announceContentAdded(QWidget* container, const QString& content);
    void announceContentRemoved(QWidget* container, const QString& content);
    void announceContentChanged(QWidget* widget, const QString& oldContent, const QString& newContent);
    void announceStateChange(QWidget* widget, const QString& property, const QString& newValue);
    
    // Screen reader specific optimizations
    void optimizeForScreenReader(FluentScreenReaderType type);
    void setScreenReaderSpecificProperty(QWidget* widget, const QString& property, const QString& value);
    QString getScreenReaderSpecificText(const QString& text, FluentScreenReaderType type);
    
    // Accessibility testing
    QStringList validateScreenReaderCompatibility(QWidget* rootWidget);
    bool hasProperAriaLabels(QWidget* widget);
    bool hasProperLandmarks(QWidget* rootWidget);
    bool hasProperHeadingStructure(QWidget* rootWidget);
    QStringList findMissingLabels(QWidget* rootWidget);
    QStringList findOrphanedDescriptions(QWidget* rootWidget);
    
    // Configuration
    void setAnnouncementDelay(int milliseconds) { m_announcementDelay = milliseconds; }
    void setMaxAnnouncementLength(int characters) { m_maxAnnouncementLength = characters; }
    void setAnnouncementRate(int wordsPerMinute) { m_announcementRate = wordsPerMinute; }
    void enableVerboseMode(bool enabled) { m_verboseMode = enabled; }
    void enableContextualAnnouncements(bool enabled) { m_contextualAnnouncements = enabled; }

signals:
    void screenReaderDetected(FluentScreenReaderType type, const QString& name);
    void screenReaderActivated(bool active);
    void announcementQueued(const FluentAnnouncement& announcement);
    void announcementDelivered(const FluentAnnouncement& announcement);
    void ariaPropertyChanged(QWidget* widget, const QString& property, const QString& value);
    void liveRegionUpdated(QWidget* widget, const QString& content);
    void landmarkAdded(QWidget* widget, FluentAriaRole role, const QString& label);
    void headingStructureChanged(QWidget* rootWidget);
    void screenReaderCompatibilityIssue(QWidget* widget, const QString& issue);

private slots:
    void processAnnouncementQueue();
    void onWidgetDestroyed(QObject* widget);
    void onSystemAccessibilityChanged();

private:
    FluentScreenReaderManager();
    
    // Screen reader detection helpers
    FluentScreenReaderType detectWindowsScreenReader();
    FluentScreenReaderType detectMacScreenReader();
    FluentScreenReaderType detectLinuxScreenReader();
    bool isProcessRunning(const QString& processName);
    
    // Announcement helpers
    void queueAnnouncement(const FluentAnnouncement& announcement);
    void deliverAnnouncement(const FluentAnnouncement& announcement);
    QString formatAnnouncementForScreenReader(const QString& message, FluentScreenReaderType type);
    bool shouldAnnounce(const FluentAnnouncement& announcement);
    
    // ARIA helpers
    QString ariaRoleToString(FluentAriaRole role);
    void updateAccessibleInterface(QWidget* widget);
    void notifyScreenReaderOfChange(QWidget* widget, const QString& property);
    
    // Validation helpers
    void validateWidget(QWidget* widget, QStringList& issues);
    bool hasAccessibleName(QWidget* widget);
    bool hasAccessibleDescription(QWidget* widget);
    bool isProperlyLabeled(QWidget* widget);

private:
    FluentScreenReaderType m_activeScreenReader{FluentScreenReaderType::Unknown};
    bool m_screenReaderActive{false};
    FluentScreenReaderInfo m_screenReaderInfo;
    
    // Announcement system
    QQueue<FluentAnnouncement> m_announcementQueue;
    QTimer* m_announcementTimer;
    int m_announcementDelay{100};
    int m_maxAnnouncementLength{500};
    int m_announcementRate{200}; // words per minute
    bool m_verboseMode{false};
    bool m_contextualAnnouncements{true};
    
    // Widget registry
    std::unordered_map<QWidget*, FluentAriaProperties> m_ariaProperties;
    std::unordered_map<QWidget*, FluentAriaRole> m_ariaRoles;
    std::unordered_map<QWidget*, FluentLiveRegionType> m_liveRegions;
    QList<QWidget*> m_landmarks;
    QList<QWidget*> m_headings;
    
    // Thread safety
    mutable QMutex m_mutex;
    
    // Performance tracking
    std::chrono::steady_clock::time_point m_lastAnnouncement;
    int m_announcementCount{0};
};

} // namespace FluentQt::Accessibility
