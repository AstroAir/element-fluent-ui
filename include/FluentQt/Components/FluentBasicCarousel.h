// include/FluentQt/Components/FluentBasicCarousel.h
#pragma once

#include <QHBoxLayout>
#include <QVBoxLayout>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCarousel.h"

namespace FluentQt::Components {

/**
 * @brief Basic carousel component with navigation controls
 *
 * FluentBasicCarousel provides a simple carousel implementation with:
 * - Previous/Next navigation buttons
 * - Keyboard navigation support
 * - Accessibility features
 * - Clean, minimal design
 * - Focus management
 */
class FluentBasicCarousel : public FluentCarousel {
    Q_OBJECT
    Q_PROPERTY(
        bool showNavigationButtons READ showNavigationButtons WRITE
            setShowNavigationButtons NOTIFY navigationButtonsVisibilityChanged)
    Q_PROPERTY(QString previousButtonText READ previousButtonText WRITE
                   setPreviousButtonText NOTIFY previousButtonTextChanged)
    Q_PROPERTY(QString nextButtonText READ nextButtonText WRITE
                   setNextButtonText NOTIFY nextButtonTextChanged)
    Q_PROPERTY(
        FluentButtonStyle navigationButtonStyle READ navigationButtonStyle WRITE
            setNavigationButtonStyle NOTIFY navigationButtonStyleChanged)

public:
    explicit FluentBasicCarousel(QWidget* parent = nullptr);
    explicit FluentBasicCarousel(const FluentCarouselConfig& config,
                                 QWidget* parent = nullptr);
    ~FluentBasicCarousel() override = default;

    // Navigation button properties
    bool showNavigationButtons() const noexcept {
        return m_showNavigationButtons;
    }
    void setShowNavigationButtons(bool show);

    QString previousButtonText() const noexcept { return m_previousButtonText; }
    void setPreviousButtonText(const QString& text);

    QString nextButtonText() const noexcept { return m_nextButtonText; }
    void setNextButtonText(const QString& text);

    FluentButtonStyle navigationButtonStyle() const noexcept {
        return m_navigationButtonStyle;
    }
    void setNavigationButtonStyle(FluentButtonStyle style);

    // Button access for customization
    FluentButton* previousButton() const noexcept { return m_previousButton; }
    FluentButton* nextButton() const noexcept { return m_nextButton; }

    // Convenience factory methods
    static FluentBasicCarousel* createHorizontal(QWidget* parent = nullptr);
    static FluentBasicCarousel* createVertical(QWidget* parent = nullptr);
    static FluentBasicCarousel* createMinimal(QWidget* parent = nullptr);

public slots:
    void setNavigationButtonsEnabled(bool enabled);
    void updateNavigationButtonStates();

signals:
    void navigationButtonsVisibilityChanged(bool visible);
    void previousButtonTextChanged(const QString& text);
    void nextButtonTextChanged(const QString& text);
    void navigationButtonStyleChanged(FluentButtonStyle style);
    void navigationButtonClicked(int direction);  // -1 for previous, 1 for next

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onPreviousButtonClicked();
    void onNextButtonClicked();
    void onCurrentIndexChanged(int index);
    void onItemCountChanged(int count);

private:
    void initializeBasicCarousel();
    void createNavigationButtons();
    void updateNavigationLayout();
    void updateButtonStates();
    void updateButtonStyles();
    void setupKeyboardNavigation();
    void updateAccessibilityInfo();

private:
    // Navigation buttons
    FluentButton* m_previousButton{nullptr};
    FluentButton* m_nextButton{nullptr};
    QHBoxLayout* m_navigationLayout{nullptr};
    QWidget* m_navigationContainer{nullptr};

    // Configuration
    bool m_showNavigationButtons{true};
    QString m_previousButtonText{"Previous"};
    QString m_nextButtonText{"Next"};
    FluentButtonStyle m_navigationButtonStyle{FluentButtonStyle::Outline};

    // State
    bool m_navigationButtonsEnabled{true};
};

/**
 * @brief Navigation button widget for carousel
 */
class FluentCarouselNavigation : public QWidget {
    Q_OBJECT

public:
    explicit FluentCarouselNavigation(QWidget* parent = nullptr);
    ~FluentCarouselNavigation() override = default;

    void setPreviousButtonText(const QString& text);
    void setNextButtonText(const QString& text);
    void setButtonStyle(FluentButtonStyle style);
    void setButtonsEnabled(bool enabled);
    void updateButtonStates(bool canGoPrevious, bool canGoNext);

    FluentButton* previousButton() const noexcept { return m_previousButton; }
    FluentButton* nextButton() const noexcept { return m_nextButton; }

signals:
    void previousClicked();
    void nextClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void initializeNavigation();
    void updateLayout();

private:
    FluentButton* m_previousButton{nullptr};
    FluentButton* m_nextButton{nullptr};
    QHBoxLayout* m_layout{nullptr};
};

}  // namespace FluentQt::Components
