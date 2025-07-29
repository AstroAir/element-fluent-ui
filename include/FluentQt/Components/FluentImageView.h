// include/FluentQt/Components/FluentImageView.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QPixmap>
#include <QMovie>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QScrollArea>
#include <memory>

namespace FluentQt::Components {

enum class FluentImageScaleMode {
    None,           // No scaling
    Fit,            // Scale to fit while maintaining aspect ratio
    Fill,           // Scale to fill, may crop
    Stretch,        // Stretch to fill, may distort
    FitWidth,       // Scale to fit width
    FitHeight       // Scale to fit height
};

enum class FluentImageAlignment {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

class FluentImageView : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(FluentImageScaleMode scaleMode READ scaleMode WRITE setScaleMode NOTIFY scaleModeChanged)
    Q_PROPERTY(FluentImageAlignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(bool smoothTransformation READ smoothTransformation WRITE setSmoothTransformation NOTIFY smoothTransformationChanged)
    Q_PROPERTY(bool zoomEnabled READ isZoomEnabled WRITE setZoomEnabled NOTIFY zoomEnabledChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(qreal minZoomFactor READ minZoomFactor WRITE setMinZoomFactor NOTIFY minZoomFactorChanged)
    Q_PROPERTY(qreal maxZoomFactor READ maxZoomFactor WRITE setMaxZoomFactor NOTIFY maxZoomFactorChanged)
    Q_PROPERTY(bool showControls READ showControls WRITE setShowControls NOTIFY showControlsChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText NOTIFY placeholderTextChanged)

public:
    explicit FluentImageView(QWidget* parent = nullptr);
    explicit FluentImageView(const QPixmap& pixmap, QWidget* parent = nullptr);
    explicit FluentImageView(const QString& source, QWidget* parent = nullptr);
    ~FluentImageView() override;

    // Image properties
    QPixmap pixmap() const;
    void setPixmap(const QPixmap& pixmap);

    QString source() const;
    void setSource(const QString& source);

    // Display properties
    FluentImageScaleMode scaleMode() const;
    void setScaleMode(FluentImageScaleMode mode);

    FluentImageAlignment alignment() const;
    void setAlignment(FluentImageAlignment alignment);

    bool smoothTransformation() const;
    void setSmoothTransformation(bool smooth);

    // Zoom properties
    bool isZoomEnabled() const;
    void setZoomEnabled(bool enabled);

    qreal zoomFactor() const;
    void setZoomFactor(qreal factor);

    qreal minZoomFactor() const;
    void setMinZoomFactor(qreal factor);

    qreal maxZoomFactor() const;
    void setMaxZoomFactor(qreal factor);

    // Control properties
    bool showControls() const;
    void setShowControls(bool show);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    // Animation support
    bool isAnimated() const;
    void setMovie(QMovie* movie);
    QMovie* movie() const;

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void zoomToActualSize();
    void resetZoom();
    void rotateLeft();
    void rotateRight();
    void flipHorizontal();
    void flipVertical();

signals:
    void pixmapChanged(const QPixmap& pixmap);
    void sourceChanged(const QString& source);
    void scaleModeChanged(FluentImageScaleMode mode);
    void alignmentChanged(FluentImageAlignment alignment);
    void smoothTransformationChanged(bool smooth);
    void zoomEnabledChanged(bool enabled);
    void zoomFactorChanged(qreal factor);
    void minZoomFactorChanged(qreal factor);
    void maxZoomFactorChanged(qreal factor);
    void showControlsChanged(bool show);
    void placeholderTextChanged(const QString& text);
    void imageClicked();
    void imageDoubleClicked();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from, Core::FluentState to) override;

private slots:
    void onZoomInClicked();
    void onZoomOutClicked();
    void onZoomToFitClicked();
    void onActualSizeClicked();
    void onRotateLeftClicked();
    void onRotateRightClicked();
    void onThemeChanged();

private:
    // Setup methods
    void setupUI();
    void setupControls();
    void updateColors();
    void updateFonts();

    // Painting methods
    void paintImage(QPainter& painter, const QRect& rect);
    void paintPlaceholder(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);

    // Utility methods
    QPixmap getScaledPixmap(const QSize& targetSize) const;
    QRect calculateImageRect(const QSize& imageSize, const QRect& targetRect) const;
    Qt::Alignment getQtAlignment() const;
    void updateControlsVisibility();
    void updateImageDisplay();

private:
    // Image data
    QPixmap m_pixmap;
    QString m_source;
    QMovie* m_movie{nullptr};
    
    // Display properties
    FluentImageScaleMode m_scaleMode{FluentImageScaleMode::Fit};
    FluentImageAlignment m_alignment{FluentImageAlignment::Center};
    bool m_smoothTransformation{true};
    
    // Zoom properties
    bool m_zoomEnabled{true};
    qreal m_zoomFactor{1.0};
    qreal m_minZoomFactor{0.1};
    qreal m_maxZoomFactor{10.0};
    
    // Transform properties
    int m_rotation{0}; // In degrees
    bool m_horizontalFlip{false};
    bool m_verticalFlip{false};
    
    // Control properties
    bool m_showControls{false};
    QString m_placeholderText{"No image"};
    
    // UI components
    QVBoxLayout* m_mainLayout{nullptr};
    QLabel* m_imageLabel{nullptr};
    QScrollArea* m_scrollArea{nullptr};
    QWidget* m_controlsWidget{nullptr};
    QHBoxLayout* m_controlsLayout{nullptr};
    QPushButton* m_zoomInButton{nullptr};
    QPushButton* m_zoomOutButton{nullptr};
    QPushButton* m_zoomToFitButton{nullptr};
    QPushButton* m_actualSizeButton{nullptr};
    QPushButton* m_rotateLeftButton{nullptr};
    QPushButton* m_rotateRightButton{nullptr};
    
    // Cached values
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

} // namespace FluentQt::Components
