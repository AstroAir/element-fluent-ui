// include/FluentQt/Accessibility/FluentAccessibleAnimator.h
#ifndef FLUENTACCESSIBLEANIMATOR_H
#define FLUENTACCESSIBLEANIMATOR_H

#include <QAudioOutput>
#include <QColor>
#include <QFont>
#include <QMediaPlayer>
#include <QObject>
#include <QPropertyAnimation>
#include <QSettings>
#include <QSoundEffect>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "FluentQt/Animation/FluentAnimator.h"

namespace FluentQt::Accessibility {

// Vestibular disorder considerations
enum class FluentVestibularSafety {
    Safe,     // No vestibular triggers
    Caution,  // Minimal triggers, safe for most users
    Warning,  // May cause discomfort for sensitive users
    Unsafe    // High risk for vestibular disorders
};

// Motion sensitivity levels
enum class FluentMotionSensitivity {
    None,      // No motion reduction
    Minimal,   // Slight reduction in motion
    Moderate,  // Significant motion reduction
    Severe,    // Maximum motion reduction
    NoMotion   // All motion disabled
};

// Audio cue types for animations
enum class FluentAudioCue {
    None,
    Subtle,      // Soft, non-intrusive sounds
    Standard,    // Normal audio feedback
    Enhanced,    // Rich audio descriptions
    Descriptive  // Full audio descriptions of visual changes
};

// High contrast animation modes
enum class FluentContrastMode {
    Standard,   // Normal contrast
    High,       // High contrast colors
    ExtraHigh,  // Maximum contrast
    Custom      // User-defined contrast settings
};

// Accessibility configuration
struct FluentAccessibilityConfig {
    // Motion settings
    FluentMotionSensitivity motionSensitivity{FluentMotionSensitivity::None};
    bool respectSystemReducedMotion{true};
    bool enableVestibularSafety{true};
    FluentVestibularSafety maxVestibularLevel{FluentVestibularSafety::Caution};

    // Audio settings
    FluentAudioCue audioCueLevel{FluentAudioCue::None};
    bool enableSpatialAudio{false};
    float audioVolume{0.5f};
    bool enableHapticFeedback{false};

    // Visual settings
    FluentContrastMode contrastMode{FluentContrastMode::Standard};
    bool enableHighContrastAnimations{false};
    bool enableFocusIndicators{true};
    bool enableAnimationDescriptions{false};

    // Timing settings
    float animationSpeedMultiplier{1.0f};
    int minimumAnimationDuration{100};   // ms
    int maximumAnimationDuration{2000};  // ms
    bool enablePauseOnFocus{false};

    // Cognitive accessibility
    bool enableSimplifiedAnimations{false};
    bool enableAnimationPreviews{false};
    bool enableStepByStepMode{false};

    // Load from system settings
    static FluentAccessibilityConfig fromSystemSettings();

    // Predefined configurations
    static FluentAccessibilityConfig vestibularSafe();
    static FluentAccessibilityConfig highContrast();
    static FluentAccessibilityConfig audioDescribed();
    static FluentAccessibilityConfig cognitiveAccessible();
};

// Accessible animation wrapper
class FluentAccessibleAnimator : public QObject {
    Q_OBJECT

public:
    explicit FluentAccessibleAnimator(QObject* parent = nullptr);
    ~FluentAccessibleAnimator() = default;

    // Configuration
    void setAccessibilityConfig(const FluentAccessibilityConfig& config);
    FluentAccessibilityConfig accessibilityConfig() const { return m_config; }

    // Accessible animation creation
    std::unique_ptr<QPropertyAnimation> createAccessibleAnimation(
        QObject* target, const QByteArray& property, const QVariant& from,
        const QVariant& to,
        const Animation::FluentAnimationConfig& config = {});

    // Vestibular-safe animations
    std::unique_ptr<QPropertyAnimation> vestibularSafeFade(
        QWidget* target, const Animation::FluentAnimationConfig& config = {});

    std::unique_ptr<QPropertyAnimation> vestibularSafeScale(
        QWidget* target, double fromScale, double toScale,
        const Animation::FluentAnimationConfig& config = {});

    // High contrast animations
    std::unique_ptr<QPropertyAnimation> highContrastTransition(
        QWidget* target, const QColor& fromColor, const QColor& toColor,
        const Animation::FluentAnimationConfig& config = {});

    // Audio-described animations
    void playAudioDescription(const QString& description);
    void playAudioCue(FluentAudioCue cueType, const QString& context = {});

    // Focus management
    void animateFocusIndicator(QWidget* target, bool focused);
    void ensureFocusVisible(QWidget* target);

    // System integration
    static bool isReducedMotionEnabled();
    static bool isHighContrastEnabled();
    static FluentMotionSensitivity getSystemMotionSensitivity();
    static FluentContrastMode getSystemContrastMode();

signals:
    void animationStarted(QWidget* target, const QString& description);
    void animationFinished(QWidget* target, const QString& description);
    void accessibilityConfigChanged();
    void vestibularWarning(const QString& warning);

private slots:
    void onSystemSettingsChanged();
    void onAnimationStateChanged();

private:
    Animation::FluentAnimationConfig adaptConfigForAccessibility(
        const Animation::FluentAnimationConfig& config) const;

    FluentVestibularSafety assessVestibularSafety(
        const Animation::FluentAnimationConfig& config) const;

    void applyMotionReduction(Animation::FluentAnimationConfig& config) const;
    void applyContrastAdjustments(QWidget* target) const;
    void setupAudioFeedback();
    void setupHapticFeedback();

    FluentAccessibilityConfig m_config;
    std::unique_ptr<QSoundEffect> m_audioPlayer;
    QSettings* m_settings;
    QTimer* m_systemMonitor;

    // Audio cue sounds
    QHash<FluentAudioCue, QString> m_audioCuePaths;

    // High contrast color schemes
    QHash<FluentContrastMode, QHash<QString, QColor>> m_contrastSchemes;
};

// Vestibular safety analyzer
class FluentVestibularAnalyzer : public QObject {
    Q_OBJECT

public:
    explicit FluentVestibularAnalyzer(QObject* parent = nullptr);
    ~FluentVestibularAnalyzer() = default;

    // Safety analysis
    FluentVestibularSafety analyzeAnimation(
        const Animation::FluentAnimationConfig& config) const;
    FluentVestibularSafety analyzeSequence(
        const QList<Animation::FluentAnimationConfig>& configs) const;

    // Risk factors
    bool hasRapidMotion(const Animation::FluentAnimationConfig& config) const;
    bool hasRotationalMotion(
        const Animation::FluentAnimationConfig& config) const;
    bool hasScalingMotion(const Animation::FluentAnimationConfig& config) const;
    bool hasParallaxEffect(
        const Animation::FluentAnimationConfig& config) const;

    // Safety recommendations
    Animation::FluentAnimationConfig makeVestibularSafe(
        const Animation::FluentAnimationConfig& config) const;

    QStringList getSafetyRecommendations(
        const Animation::FluentAnimationConfig& config) const;

private:
    struct VestibularRiskFactors {
        double motionSpeed{0.0};
        double rotationAmount{0.0};
        double scaleChange{0.0};
        double parallaxIntensity{0.0};
        int simultaneousMotions{0};
        bool hasFlashing{false};
        bool hasStrobing{false};
    };

    VestibularRiskFactors analyzeRiskFactors(
        const Animation::FluentAnimationConfig& config) const;

    double calculateRiskScore(const VestibularRiskFactors& factors) const;

    // Safety thresholds
    static constexpr double SAFE_MOTION_SPEED = 200.0;    // pixels per second
    static constexpr double SAFE_ROTATION_AMOUNT = 45.0;  // degrees
    static constexpr double SAFE_SCALE_CHANGE = 0.5;      // scale factor
    static constexpr int SAFE_DURATION_MIN = 300;         // milliseconds
};

// Audio description generator
class FluentAudioDescriber : public QObject {
    Q_OBJECT

public:
    explicit FluentAudioDescriber(QObject* parent = nullptr);
    ~FluentAudioDescriber() = default;

    // Description generation
    QString generateDescription(
        QWidget* target, const Animation::FluentAnimationConfig& config) const;
    QString generateStateDescription(QWidget* target) const;
    QString generateTransitionDescription(const QString& fromState,
                                          const QString& toState) const;

    // Audio playback
    void playDescription(const QString& description);
    void playStateChange(QWidget* target, const QString& newState);
    void playInteractionFeedback(QWidget* target, const QString& interaction);

    // Voice settings
    void setVoiceSpeed(float speed) { m_voiceSpeed = speed; }
    void setVoicePitch(float pitch) { m_voicePitch = pitch; }
    void setVoiceVolume(float volume) { m_voiceVolume = volume; }

    // Language support
    void setLanguage(const QString& languageCode) { m_language = languageCode; }
    QStringList getSupportedLanguages() const;

signals:
    void descriptionStarted(const QString& description);
    void descriptionFinished();

private:
    QString describeAnimationType(
        const Animation::FluentAnimationConfig& config) const;
    QString describeDirection(const QVariant& from, const QVariant& to) const;
    QString describeDuration(std::chrono::milliseconds duration) const;
    QString describeEasing(Animation::FluentEasing easing) const;

    void initializeTextToSpeech();

    std::unique_ptr<QMediaPlayer> m_mediaPlayer;
    QString m_language{"en-US"};
    float m_voiceSpeed{1.0f};
    float m_voicePitch{1.0f};
    float m_voiceVolume{0.7f};

    // Description templates
    QHash<QString, QString> m_descriptionTemplates;
};

// High contrast animation manager
class FluentHighContrastManager : public QObject {
    Q_OBJECT

public:
    explicit FluentHighContrastManager(QObject* parent = nullptr);
    ~FluentHighContrastManager() = default;

    // Contrast mode management
    void setContrastMode(FluentContrastMode mode);
    FluentContrastMode contrastMode() const { return m_contrastMode; }

    // Color adaptation
    QColor adaptColor(const QColor& color) const;
    QColor getContrastColor(const QColor& backgroundColor) const;
    QColor getFocusColor() const;
    QColor getSelectionColor() const;

    // Widget styling
    void applyHighContrastStyle(QWidget* widget);
    void applyFocusStyle(QWidget* widget, bool focused);
    void applySelectionStyle(QWidget* widget, bool selected);

    // Animation adaptation
    Animation::FluentAnimationConfig adaptAnimationForContrast(
        const Animation::FluentAnimationConfig& config) const;

signals:
    void contrastModeChanged(FluentContrastMode mode);

private:
    void initializeContrastSchemes();
    void updateSystemContrast();

    FluentContrastMode m_contrastMode{FluentContrastMode::Standard};
    QHash<FluentContrastMode, QHash<QString, QColor>> m_colorSchemes;
    QSettings* m_settings;
};

// Granular motion control
class FluentMotionController : public QObject {
    Q_OBJECT

public:
    explicit FluentMotionController(QObject* parent = nullptr);
    ~FluentMotionController() = default;

    // Motion categories
    enum MotionCategory {
        Essential,   // Critical UI feedback
        Decorative,  // Visual enhancements
        Parallax,    // Depth effects
        Zoom,        // Scaling animations
        Rotation,    // Rotational effects
        Slide,       // Sliding transitions
        Fade,        // Opacity changes
        Background   // Background animations
    };

    // Category control
    void enableMotionCategory(MotionCategory category, bool enable);
    bool isMotionCategoryEnabled(MotionCategory category) const;

    void setMotionCategoryIntensity(MotionCategory category, float intensity);
    float motionCategoryIntensity(MotionCategory category) const;

    // Global motion control
    void setGlobalMotionEnabled(bool enabled) {
        m_globalMotionEnabled = enabled;
    }
    bool isGlobalMotionEnabled() const { return m_globalMotionEnabled; }

    void setGlobalMotionIntensity(float intensity) {
        m_globalMotionIntensity = intensity;
    }
    float globalMotionIntensity() const { return m_globalMotionIntensity; }

    // Animation filtering
    bool shouldAllowAnimation(
        MotionCategory category,
        const Animation::FluentAnimationConfig& config) const;

    Animation::FluentAnimationConfig filterAnimation(
        MotionCategory category,
        const Animation::FluentAnimationConfig& config) const;

signals:
    void motionSettingsChanged();
    void categoryEnabledChanged(MotionCategory category, bool enabled);

private:
    void loadSettings();
    void saveSettings();

    bool m_globalMotionEnabled{true};
    float m_globalMotionIntensity{1.0f};

    QHash<MotionCategory, bool> m_categoryEnabled;
    QHash<MotionCategory, float> m_categoryIntensity;

    QSettings* m_settings;
};

}  // namespace FluentQt::Accessibility

#endif  // FLUENTACCESSIBLEANIMATOR_H
