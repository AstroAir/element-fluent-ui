// src/Components/FluentImageView.cpp
#include "FluentQt/Components/FluentImageView.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QAccessible>
#include <QFileInfo>
#include <QImageReader>
#include <QTransform>
#include <QtMath>
#include <algorithm>

namespace FluentQt::Components {

FluentImageView::FluentImageView(QWidget* parent)
    : Core::FluentComponent(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setObjectName("FluentImageView");
    
    setupUI();
    setupControls();
    updateColors();
    updateFonts();
    
    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentImageView::onThemeChanged);
}

FluentImageView::FluentImageView(const QPixmap& pixmap, QWidget* parent)
    : FluentImageView(parent)
{
    setPixmap(pixmap);
}

FluentImageView::FluentImageView(const QString& source, QWidget* parent)
    : FluentImageView(parent)
{
    setSource(source);
}

FluentImageView::~FluentImageView() = default;

void FluentImageView::setupUI() {
    FLUENT_PROFILE("FluentImageView::setupUI");
    
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Create scroll area for image
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName("FluentImageView_ScrollArea");
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidgetResizable(true);
    
    // Create image label
    m_imageLabel = new QLabel(m_scrollArea);
    m_imageLabel->setObjectName("FluentImageView_ImageLabel");
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setMinimumSize(1, 1);
    m_imageLabel->setText(m_placeholderText);
    m_scrollArea->setWidget(m_imageLabel);
    
    m_mainLayout->addWidget(m_scrollArea);
    
    // Set minimum size
    setMinimumSize(100, 100);
}

void FluentImageView::setupControls() {
    FLUENT_PROFILE("FluentImageView::setupControls");
    
    // Create controls widget
    m_controlsWidget = new QWidget(this);
    m_controlsWidget->setObjectName("FluentImageView_Controls");
    m_controlsWidget->setFixedHeight(40);
    m_controlsWidget->hide(); // Initially hidden
    
    // Create controls layout
    m_controlsLayout = new QHBoxLayout(m_controlsWidget);
    m_controlsLayout->setContentsMargins(8, 4, 8, 4);
    m_controlsLayout->setSpacing(4);
    
    // Create zoom in button
    m_zoomInButton = new QPushButton("🔍+", m_controlsWidget);
    m_zoomInButton->setObjectName("FluentImageView_ZoomInButton");
    m_zoomInButton->setFixedSize(32, 32);
    m_zoomInButton->setToolTip("Zoom In");
    m_controlsLayout->addWidget(m_zoomInButton);
    
    // Create zoom out button
    m_zoomOutButton = new QPushButton("🔍-", m_controlsWidget);
    m_zoomOutButton->setObjectName("FluentImageView_ZoomOutButton");
    m_zoomOutButton->setFixedSize(32, 32);
    m_zoomOutButton->setToolTip("Zoom Out");
    m_controlsLayout->addWidget(m_zoomOutButton);
    
    // Create zoom to fit button
    m_zoomToFitButton = new QPushButton("⬜", m_controlsWidget);
    m_zoomToFitButton->setObjectName("FluentImageView_ZoomToFitButton");
    m_zoomToFitButton->setFixedSize(32, 32);
    m_zoomToFitButton->setToolTip("Zoom to Fit");
    m_controlsLayout->addWidget(m_zoomToFitButton);
    
    // Create actual size button
    m_actualSizeButton = new QPushButton("1:1", m_controlsWidget);
    m_actualSizeButton->setObjectName("FluentImageView_ActualSizeButton");
    m_actualSizeButton->setFixedSize(32, 32);
    m_actualSizeButton->setToolTip("Actual Size");
    m_controlsLayout->addWidget(m_actualSizeButton);
    
    // Add separator
    m_controlsLayout->addSpacing(8);
    
    // Create rotate left button
    m_rotateLeftButton = new QPushButton("↺", m_controlsWidget);
    m_rotateLeftButton->setObjectName("FluentImageView_RotateLeftButton");
    m_rotateLeftButton->setFixedSize(32, 32);
    m_rotateLeftButton->setToolTip("Rotate Left");
    m_controlsLayout->addWidget(m_rotateLeftButton);
    
    // Create rotate right button
    m_rotateRightButton = new QPushButton("↻", m_controlsWidget);
    m_rotateRightButton->setObjectName("FluentImageView_RotateRightButton");
    m_rotateRightButton->setFixedSize(32, 32);
    m_rotateRightButton->setToolTip("Rotate Right");
    m_controlsLayout->addWidget(m_rotateRightButton);
    
    // Add stretch to center controls
    m_controlsLayout->addStretch();
    
    m_mainLayout->addWidget(m_controlsWidget);
    
    // Connect control signals
    connect(m_zoomInButton, &QPushButton::clicked, this, &FluentImageView::onZoomInClicked);
    connect(m_zoomOutButton, &QPushButton::clicked, this, &FluentImageView::onZoomOutClicked);
    connect(m_zoomToFitButton, &QPushButton::clicked, this, &FluentImageView::onZoomToFitClicked);
    connect(m_actualSizeButton, &QPushButton::clicked, this, &FluentImageView::onActualSizeClicked);
    connect(m_rotateLeftButton, &QPushButton::clicked, this, &FluentImageView::onRotateLeftClicked);
    connect(m_rotateRightButton, &QPushButton::clicked, this, &FluentImageView::onRotateRightClicked);
}

QPixmap FluentImageView::pixmap() const {
    return m_pixmap;
}

void FluentImageView::setPixmap(const QPixmap& pixmap) {
    if (m_pixmap.cacheKey() != pixmap.cacheKey()) {
        m_pixmap = pixmap;
        m_source.clear();
        
        if (m_movie) {
            m_movie->stop();
            m_movie = nullptr;
        }
        
        updateImageDisplay();
        m_sizeHintValid = false;
        updateGeometry();
        emit pixmapChanged(pixmap);
    }
}

QString FluentImageView::source() const {
    return m_source;
}

void FluentImageView::setSource(const QString& source) {
    if (m_source != source) {
        m_source = source;
        
        if (m_movie) {
            m_movie->stop();
            m_movie = nullptr;
        }
        
        if (!source.isEmpty()) {
            QFileInfo fileInfo(source);
            if (fileInfo.exists()) {
                // Check if it's an animated format
                QImageReader reader(source);
                if (reader.supportsAnimation()) {
                    m_movie = new QMovie(source, QByteArray(), this);
                    m_imageLabel->setMovie(m_movie);
                    m_movie->start();
                    m_pixmap = QPixmap();
                } else {
                    m_pixmap.load(source);
                    updateImageDisplay();
                }
            } else {
                m_pixmap = QPixmap();
                updateImageDisplay();
            }
        } else {
            m_pixmap = QPixmap();
            updateImageDisplay();
        }
        
        m_sizeHintValid = false;
        updateGeometry();
        emit sourceChanged(source);
    }
}

FluentImageScaleMode FluentImageView::scaleMode() const {
    return m_scaleMode;
}

void FluentImageView::setScaleMode(FluentImageScaleMode mode) {
    if (m_scaleMode != mode) {
        m_scaleMode = mode;
        updateImageDisplay();
        emit scaleModeChanged(mode);
    }
}

FluentImageAlignment FluentImageView::alignment() const {
    return m_alignment;
}

void FluentImageView::setAlignment(FluentImageAlignment alignment) {
    if (m_alignment != alignment) {
        m_alignment = alignment;
        m_imageLabel->setAlignment(getQtAlignment());
        emit alignmentChanged(alignment);
    }
}

bool FluentImageView::smoothTransformation() const {
    return m_smoothTransformation;
}

void FluentImageView::setSmoothTransformation(bool smooth) {
    if (m_smoothTransformation != smooth) {
        m_smoothTransformation = smooth;
        updateImageDisplay();
        emit smoothTransformationChanged(smooth);
    }
}

bool FluentImageView::isZoomEnabled() const {
    return m_zoomEnabled;
}

void FluentImageView::setZoomEnabled(bool enabled) {
    if (m_zoomEnabled != enabled) {
        m_zoomEnabled = enabled;
        updateControlsVisibility();
        emit zoomEnabledChanged(enabled);
    }
}

qreal FluentImageView::zoomFactor() const {
    return m_zoomFactor;
}

void FluentImageView::setZoomFactor(qreal factor) {
    const qreal clampedFactor = qBound(m_minZoomFactor, factor, m_maxZoomFactor);
    if (!qFuzzyCompare(m_zoomFactor, clampedFactor)) {
        m_zoomFactor = clampedFactor;
        updateImageDisplay();
        emit zoomFactorChanged(clampedFactor);
    }
}

qreal FluentImageView::minZoomFactor() const {
    return m_minZoomFactor;
}

void FluentImageView::setMinZoomFactor(qreal factor) {
    if (!qFuzzyCompare(m_minZoomFactor, factor) && factor > 0.0) {
        m_minZoomFactor = factor;
        if (m_zoomFactor < factor) {
            setZoomFactor(factor);
        }
        emit minZoomFactorChanged(factor);
    }
}

qreal FluentImageView::maxZoomFactor() const {
    return m_maxZoomFactor;
}

void FluentImageView::setMaxZoomFactor(qreal factor) {
    if (!qFuzzyCompare(m_maxZoomFactor, factor) && factor > 0.0) {
        m_maxZoomFactor = factor;
        if (m_zoomFactor > factor) {
            setZoomFactor(factor);
        }
        emit maxZoomFactorChanged(factor);
    }
}

bool FluentImageView::showControls() const {
    return m_showControls;
}

void FluentImageView::setShowControls(bool show) {
    if (m_showControls != show) {
        m_showControls = show;
        updateControlsVisibility();
        emit showControlsChanged(show);
    }
}

QString FluentImageView::placeholderText() const {
    return m_placeholderText;
}

void FluentImageView::setPlaceholderText(const QString& text) {
    if (m_placeholderText != text) {
        m_placeholderText = text;
        if (m_pixmap.isNull() && !m_movie) {
            m_imageLabel->setText(text);
        }
        emit placeholderTextChanged(text);
    }
}

bool FluentImageView::isAnimated() const {
    return m_movie != nullptr;
}

void FluentImageView::setMovie(QMovie* movie) {
    if (m_movie != movie) {
        if (m_movie) {
            m_movie->stop();
        }
        
        m_movie = movie;
        m_pixmap = QPixmap();
        m_source.clear();
        
        if (movie) {
            m_imageLabel->setMovie(movie);
            movie->start();
        } else {
            updateImageDisplay();
        }
        
        m_sizeHintValid = false;
        updateGeometry();
    }
}

QMovie* FluentImageView::movie() const {
    return m_movie;
}

QSize FluentImageView::sizeHint() const {
    FLUENT_PROFILE("FluentImageView::sizeHint");
    
    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }
    
    QSize hint(300, 200); // Default size
    
    if (!m_pixmap.isNull()) {
        hint = m_pixmap.size();
        // Limit to reasonable size
        hint = hint.boundedTo(QSize(800, 600));
    }
    
    // Add controls height if visible
    if (m_showControls) {
        hint.setHeight(hint.height() + 40);
    }
    
    m_cachedSizeHint = hint;
    m_sizeHintValid = true;
    
    return hint;
}

QSize FluentImageView::minimumSizeHint() const {
    return QSize(100, 100);
}

void FluentImageView::zoomIn() {
    setZoomFactor(m_zoomFactor * 1.25);
}

void FluentImageView::zoomOut() {
    setZoomFactor(m_zoomFactor / 1.25);
}

void FluentImageView::zoomToFit() {
    if (m_pixmap.isNull()) return;

    const QSize availableSize = m_scrollArea->viewport()->size();
    const QSize imageSize = m_pixmap.size();

    const qreal scaleX = static_cast<qreal>(availableSize.width()) / imageSize.width();
    const qreal scaleY = static_cast<qreal>(availableSize.height()) / imageSize.height();
    const qreal scale = std::min(scaleX, scaleY);

    setZoomFactor(scale);
}

void FluentImageView::zoomToActualSize() {
    setZoomFactor(1.0);
}

void FluentImageView::resetZoom() {
    setZoomFactor(1.0);
    m_rotation = 0;
    m_horizontalFlip = false;
    m_verticalFlip = false;
    updateImageDisplay();
}

void FluentImageView::rotateLeft() {
    m_rotation = (m_rotation - 90) % 360;
    updateImageDisplay();
}

void FluentImageView::rotateRight() {
    m_rotation = (m_rotation + 90) % 360;
    updateImageDisplay();
}

void FluentImageView::flipHorizontal() {
    m_horizontalFlip = !m_horizontalFlip;
    updateImageDisplay();
}

void FluentImageView::flipVertical() {
    m_verticalFlip = !m_verticalFlip;
    updateImageDisplay();
}

void FluentImageView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentImageView::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect rect = this->rect();

    // Paint border
    paintBorder(painter, rect);
}

void FluentImageView::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    QPen borderPen(palette.neutralQuaternary, 1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    const int radius = cornerRadius();
    const QRectF borderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

    painter.drawRoundedRect(borderRect, radius, radius);

    painter.restore();
}

void FluentImageView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit imageClicked();
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentImageView::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit imageDoubleClicked();

        // Toggle between fit and actual size on double click
        if (qFuzzyCompare(m_zoomFactor, 1.0)) {
            zoomToFit();
        } else {
            zoomToActualSize();
        }
    }

    Core::FluentComponent::mouseDoubleClickEvent(event);
}

void FluentImageView::wheelEvent(QWheelEvent* event) {
    if (m_zoomEnabled && (event->modifiers() & Qt::ControlModifier)) {
        const int delta = event->angleDelta().y();
        if (delta > 0) {
            zoomIn();
        } else if (delta < 0) {
            zoomOut();
        }
        event->accept();
    } else {
        Core::FluentComponent::wheelEvent(event);
    }
}

void FluentImageView::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
    m_sizeHintValid = false;

    // Auto-fit on resize if scale mode is Fit
    if (m_scaleMode == FluentImageScaleMode::Fit) {
        zoomToFit();
    }
}

void FluentImageView::updateStateStyle() {
    update();
}

void FluentImageView::performStateTransition(Core::FluentState from, Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    update();
}

void FluentImageView::onZoomInClicked() {
    zoomIn();
}

void FluentImageView::onZoomOutClicked() {
    zoomOut();
}

void FluentImageView::onZoomToFitClicked() {
    zoomToFit();
}

void FluentImageView::onActualSizeClicked() {
    zoomToActualSize();
}

void FluentImageView::onRotateLeftClicked() {
    rotateLeft();
}

void FluentImageView::onRotateRightClicked() {
    rotateRight();
}

void FluentImageView::onThemeChanged() {
    updateColors();
    updateFonts();
    update();
}

void FluentImageView::updateColors() {
    // Colors are handled in paintEvent and by child widgets
}

void FluentImageView::updateFonts() {
    const auto& theme = Styling::FluentTheme::instance();
    m_imageLabel->setFont(theme.bodyFont());
}

QPixmap FluentImageView::getScaledPixmap(const QSize& targetSize) const {
    if (m_pixmap.isNull()) {
        return QPixmap();
    }

    QPixmap scaledPixmap = m_pixmap;

    // Apply transformations
    if (m_rotation != 0 || m_horizontalFlip || m_verticalFlip) {
        QTransform transform;

        if (m_horizontalFlip) {
            transform.scale(-1, 1);
        }
        if (m_verticalFlip) {
            transform.scale(1, -1);
        }
        if (m_rotation != 0) {
            transform.rotate(m_rotation);
        }

        scaledPixmap = scaledPixmap.transformed(transform,
            m_smoothTransformation ? Qt::SmoothTransformation : Qt::FastTransformation);
    }

    // Apply zoom
    if (!qFuzzyCompare(m_zoomFactor, 1.0)) {
        const QSize zoomedSize = scaledPixmap.size() * m_zoomFactor;
        scaledPixmap = scaledPixmap.scaled(zoomedSize, Qt::KeepAspectRatio,
            m_smoothTransformation ? Qt::SmoothTransformation : Qt::FastTransformation);
    }

    // Apply scale mode
    switch (m_scaleMode) {
        case FluentImageScaleMode::None:
            // No additional scaling
            break;

        case FluentImageScaleMode::Fit:
            scaledPixmap = scaledPixmap.scaled(targetSize, Qt::KeepAspectRatio,
                m_smoothTransformation ? Qt::SmoothTransformation : Qt::FastTransformation);
            break;

        case FluentImageScaleMode::Fill:
            scaledPixmap = scaledPixmap.scaled(targetSize, Qt::KeepAspectRatioByExpanding,
                m_smoothTransformation ? Qt::SmoothTransformation : Qt::FastTransformation);
            break;

        case FluentImageScaleMode::Stretch:
            scaledPixmap = scaledPixmap.scaled(targetSize, Qt::IgnoreAspectRatio,
                m_smoothTransformation ? Qt::SmoothTransformation : Qt::FastTransformation);
            break;

        case FluentImageScaleMode::FitWidth:
            {
                const qreal scale = static_cast<qreal>(targetSize.width()) / scaledPixmap.width();
                const QSize newSize(targetSize.width(), scaledPixmap.height() * scale);
                scaledPixmap = scaledPixmap.scaled(newSize, Qt::IgnoreAspectRatio,
                    m_smoothTransformation ? Qt::SmoothTransformation : Qt::FastTransformation);
            }
            break;

        case FluentImageScaleMode::FitHeight:
            {
                const qreal scale = static_cast<qreal>(targetSize.height()) / scaledPixmap.height();
                const QSize newSize(scaledPixmap.width() * scale, targetSize.height());
                scaledPixmap = scaledPixmap.scaled(newSize, Qt::IgnoreAspectRatio,
                    m_smoothTransformation ? Qt::SmoothTransformation : Qt::FastTransformation);
            }
            break;
    }

    return scaledPixmap;
}

Qt::Alignment FluentImageView::getQtAlignment() const {
    switch (m_alignment) {
        case FluentImageAlignment::TopLeft:
            return Qt::AlignTop | Qt::AlignLeft;
        case FluentImageAlignment::TopCenter:
            return Qt::AlignTop | Qt::AlignHCenter;
        case FluentImageAlignment::TopRight:
            return Qt::AlignTop | Qt::AlignRight;
        case FluentImageAlignment::CenterLeft:
            return Qt::AlignVCenter | Qt::AlignLeft;
        case FluentImageAlignment::Center:
            return Qt::AlignCenter;
        case FluentImageAlignment::CenterRight:
            return Qt::AlignVCenter | Qt::AlignRight;
        case FluentImageAlignment::BottomLeft:
            return Qt::AlignBottom | Qt::AlignLeft;
        case FluentImageAlignment::BottomCenter:
            return Qt::AlignBottom | Qt::AlignHCenter;
        case FluentImageAlignment::BottomRight:
            return Qt::AlignBottom | Qt::AlignRight;
        default:
            return Qt::AlignCenter;
    }
}

void FluentImageView::updateControlsVisibility() {
    const bool shouldShow = m_showControls && !m_pixmap.isNull();
    m_controlsWidget->setVisible(shouldShow);

    // Update button states
    if (shouldShow) {
        m_zoomInButton->setEnabled(m_zoomEnabled && m_zoomFactor < m_maxZoomFactor);
        m_zoomOutButton->setEnabled(m_zoomEnabled && m_zoomFactor > m_minZoomFactor);
        m_zoomToFitButton->setEnabled(m_zoomEnabled);
        m_actualSizeButton->setEnabled(m_zoomEnabled);
    }
}

void FluentImageView::updateImageDisplay() {
    if (m_movie) {
        // Animated image is handled by QMovie
        return;
    }

    if (m_pixmap.isNull()) {
        m_imageLabel->setText(m_placeholderText);
        m_imageLabel->setPixmap(QPixmap());
    } else {
        m_imageLabel->setText(QString());

        // Get the scaled pixmap
        const QSize targetSize = m_scrollArea->viewport()->size();
        const QPixmap scaledPixmap = getScaledPixmap(targetSize);

        m_imageLabel->setPixmap(scaledPixmap);
        m_imageLabel->resize(scaledPixmap.size());
    }

    updateControlsVisibility();
}

} // namespace FluentQt::Components
