// src/Components/FluentImageView.cpp
#include "FluentQt/Components/FluentImageView.h"
#include "FluentQt/Accessibility/FluentAccessibilityCompliance.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QFileInfo>
#include <QFontMetrics>
#include <QImageReader>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTransform>
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

namespace FluentQt::Components {

FluentImageView::FluentImageView(QWidget* parent)
    : Core::FluentComponent(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setObjectName("FluentImageView");

    setupUI();
    setupControls();
    setupAccessibility();
    updateColors();
    updateFonts();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentImageView::onThemeChanged);
}

FluentImageView::FluentImageView(const QPixmap& pixmap, QWidget* parent)
    : FluentImageView(parent) {
    setPixmap(pixmap);
}

FluentImageView::FluentImageView(const QString& source, QWidget* parent)
    : FluentImageView(parent) {
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
    m_controlsWidget->hide();  // Initially hidden

    // Create controls layout with design token spacing
    m_controlsLayout = new QHBoxLayout(m_controlsWidget);
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    const int padding = tokenUtils.getSpacing("s");   // 8px
    const int spacing = tokenUtils.getSpacing("xs");  // 4px
    m_controlsLayout->setContentsMargins(padding, padding / 2, padding,
                                         padding / 2);
    m_controlsLayout->setSpacing(spacing);

    // Create zoom in button
    m_zoomInButton = FluentButton::createIconButton(QIcon(":/icons/zoom_in"),
                                                    m_controlsWidget);
    m_zoomInButton->setObjectName("FluentImageView_ZoomInButton");
    m_zoomInButton->setButtonSize(FluentButtonSize::Small);
    m_zoomInButton->setToolTip("Zoom In (Ctrl++)");
    m_zoomInButton->setAccessibleName("Zoom In");
    m_controlsLayout->addWidget(m_zoomInButton);

    // Create zoom out button
    m_zoomOutButton = FluentButton::createIconButton(QIcon(":/icons/zoom_out"),
                                                     m_controlsWidget);
    m_zoomOutButton->setObjectName("FluentImageView_ZoomOutButton");
    m_zoomOutButton->setButtonSize(FluentButtonSize::Small);
    m_zoomOutButton->setToolTip("Zoom Out (Ctrl+-)");
    m_zoomOutButton->setAccessibleName("Zoom Out");
    m_controlsLayout->addWidget(m_zoomOutButton);

    // Create zoom to fit button
    m_zoomToFitButton = FluentButton::createIconButton(
        QIcon(":/icons/fit_screen"), m_controlsWidget);
    m_zoomToFitButton->setObjectName("FluentImageView_ZoomToFitButton");
    m_zoomToFitButton->setButtonSize(FluentButtonSize::Small);
    m_zoomToFitButton->setToolTip("Zoom to Fit (F)");
    m_zoomToFitButton->setAccessibleName("Zoom to Fit");
    m_controlsLayout->addWidget(m_zoomToFitButton);

    // Create actual size button
    m_actualSizeButton = FluentButton::createIconButton(
        QIcon(":/icons/actual_size"), m_controlsWidget);
    m_actualSizeButton->setObjectName("FluentImageView_ActualSizeButton");
    m_actualSizeButton->setButtonSize(FluentButtonSize::Small);
    m_actualSizeButton->setToolTip("Actual Size (0)");
    m_actualSizeButton->setAccessibleName("Actual Size");
    m_controlsLayout->addWidget(m_actualSizeButton);

    // Add separator with design token spacing
    const int separatorSpacing = tokenUtils.getSpacing("s");  // 8px
    m_controlsLayout->addSpacing(separatorSpacing);

    // Create rotate left button
    m_rotateLeftButton = FluentButton::createIconButton(
        QIcon(":/icons/rotate_left"), m_controlsWidget);
    m_rotateLeftButton->setObjectName("FluentImageView_RotateLeftButton");
    m_rotateLeftButton->setButtonSize(FluentButtonSize::Small);
    m_rotateLeftButton->setToolTip("Rotate Left (Ctrl+L)");
    m_rotateLeftButton->setAccessibleName("Rotate Left");
    m_controlsLayout->addWidget(m_rotateLeftButton);

    // Create rotate right button
    m_rotateRightButton = FluentButton::createIconButton(
        QIcon(":/icons/rotate_right"), m_controlsWidget);
    m_rotateRightButton->setObjectName("FluentImageView_RotateRightButton");
    m_rotateRightButton->setButtonSize(FluentButtonSize::Small);
    m_rotateRightButton->setToolTip("Rotate Right (Ctrl+R)");
    m_rotateRightButton->setAccessibleName("Rotate Right");
    m_controlsLayout->addWidget(m_rotateRightButton);

    // Add stretch to center controls
    m_controlsLayout->addStretch();

    m_mainLayout->addWidget(m_controlsWidget);

    // Set up tab order for keyboard navigation
    setupTabOrder();

    // Connect control signals
    connect(m_zoomInButton, &FluentButton::clicked, this,
            &FluentImageView::onZoomInClicked);
    connect(m_zoomOutButton, &FluentButton::clicked, this,
            &FluentImageView::onZoomOutClicked);
    connect(m_zoomToFitButton, &FluentButton::clicked, this,
            &FluentImageView::onZoomToFitClicked);
    connect(m_actualSizeButton, &FluentButton::clicked, this,
            &FluentImageView::onActualSizeClicked);
    connect(m_rotateLeftButton, &FluentButton::clicked, this,
            &FluentImageView::onRotateLeftClicked);
    connect(m_rotateRightButton, &FluentButton::clicked, this,
            &FluentImageView::onRotateRightClicked);
}

QPixmap FluentImageView::pixmap() const { return m_pixmap; }

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

QString FluentImageView::source() const { return m_source; }

void FluentImageView::setSource(const QString& source) {
    if (m_source != source) {
        m_source = source;
        m_errorMessage.clear();

        if (m_movie) {
            m_movie->stop();
            m_movie = nullptr;
        }

        if (!source.isEmpty()) {
            // Validate source before attempting to load
            if (!isValidImageSource(source)) {
                QString validationError;
                QFileInfo fileInfo(source);

                if (!fileInfo.exists()) {
                    validationError = "Image file does not exist";
                } else if (!fileInfo.isReadable()) {
                    validationError = "Image file is not readable";
                } else if (fileInfo.size() > 100 * 1024 * 1024) {
                    validationError = "Image file is too large (max 100MB)";
                } else if (!isValidImageFormat(source)) {
                    validationError =
                        QString(
                            "Unsupported image format. Supported formats: %1")
                            .arg(supportedImageFormats().join(", "));
                } else {
                    validationError = "Invalid image source";
                }

                m_errorMessage = validationError;
                emit errorMessageChanged(m_errorMessage);
                emit imageValidationFailed(source, validationError);
                m_pixmap = QPixmap();
                updateImageDisplay();

                m_sizeHintValid = false;
                updateGeometry();
                emit sourceChanged(source);
                return;
            }

            // Set loading state
            m_loading = true;
            emit loadingChanged(true);

            try {
                QImageReader reader(source);
                if (reader.supportsAnimation()) {
                    m_movie = new QMovie(source, QByteArray(), this);
                    if (m_movie->isValid()) {
                        m_imageLabel->setMovie(m_movie);
                        m_movie->start();
                        m_pixmap = QPixmap();
                    } else {
                        delete m_movie;
                        m_movie = nullptr;
                        m_errorMessage = "Failed to load animated image";
                        emit errorMessageChanged(m_errorMessage);
                        emit imageLoadFailed(source, m_errorMessage);
                    }
                } else {
                    if (!m_pixmap.load(source)) {
                        QString loadError = reader.errorString();
                        if (loadError.isEmpty()) {
                            loadError = "Failed to load image";
                        }
                        m_errorMessage = loadError;
                        emit errorMessageChanged(m_errorMessage);
                        emit imageLoadFailed(source, m_errorMessage);
                    }
                    updateImageDisplay();
                }
            } catch (const std::exception& e) {
                m_errorMessage =
                    QString("Exception while loading image: %1").arg(e.what());
                emit errorMessageChanged(m_errorMessage);
                emit imageLoadFailed(source, m_errorMessage);
                m_pixmap = QPixmap();
                updateImageDisplay();
            }

            // Clear loading state
            m_loading = false;
            emit loadingChanged(false);
        } else {
            m_pixmap = QPixmap();
            updateImageDisplay();
        }

        m_sizeHintValid = false;
        updateGeometry();
        emit sourceChanged(source);
    }
}

FluentImageScaleMode FluentImageView::scaleMode() const { return m_scaleMode; }

void FluentImageView::setScaleMode(FluentImageScaleMode mode) {
    if (m_scaleMode != mode) {
        m_scaleMode = mode;
        updateImageDisplay();
        emit scaleModeChanged(mode);
    }
}

FluentImageAlignment FluentImageView::alignment() const { return m_alignment; }

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

bool FluentImageView::isZoomEnabled() const { return m_zoomEnabled; }

void FluentImageView::setZoomEnabled(bool enabled) {
    if (m_zoomEnabled != enabled) {
        m_zoomEnabled = enabled;
        updateControlsVisibility();
        emit zoomEnabledChanged(enabled);
    }
}

qreal FluentImageView::zoomFactor() const { return m_zoomFactor; }

void FluentImageView::setZoomFactor(qreal factor) {
    const qreal clampedFactor =
        qBound(m_minZoomFactor, factor, m_maxZoomFactor);
    if (!qFuzzyCompare(m_zoomFactor, clampedFactor)) {
        m_zoomFactor = clampedFactor;
        updateImageDisplay();
        emit zoomFactorChanged(clampedFactor);
    }
}

qreal FluentImageView::minZoomFactor() const { return m_minZoomFactor; }

void FluentImageView::setMinZoomFactor(qreal factor) {
    if (!qFuzzyCompare(m_minZoomFactor, factor) && factor > 0.0) {
        m_minZoomFactor = factor;
        if (m_zoomFactor < factor) {
            setZoomFactor(factor);
        }
        emit minZoomFactorChanged(factor);
    }
}

qreal FluentImageView::maxZoomFactor() const { return m_maxZoomFactor; }

void FluentImageView::setMaxZoomFactor(qreal factor) {
    if (!qFuzzyCompare(m_maxZoomFactor, factor) && factor > 0.0) {
        m_maxZoomFactor = factor;
        if (m_zoomFactor > factor) {
            setZoomFactor(factor);
        }
        emit maxZoomFactorChanged(factor);
    }
}

bool FluentImageView::showControls() const { return m_showControls; }

void FluentImageView::setShowControls(bool show) {
    if (m_showControls != show) {
        m_showControls = show;
        updateControlsVisibility();
        emit showControlsChanged(show);
    }
}

QString FluentImageView::placeholderText() const { return m_placeholderText; }

void FluentImageView::setPlaceholderText(const QString& text) {
    if (m_placeholderText != text) {
        m_placeholderText = text;
        if (m_pixmap.isNull() && !m_movie) {
            m_imageLabel->setText(text);
        }
        emit placeholderTextChanged(text);
    }
}

int FluentImageView::elevation() const { return m_elevation; }

void FluentImageView::setElevation(int elevation) {
    if (m_elevation != elevation) {
        m_elevation = elevation;
        update();
        emit elevationChanged(elevation);
    }
}

bool FluentImageView::isLoading() const { return m_loading; }

QString FluentImageView::errorMessage() const { return m_errorMessage; }

bool FluentImageView::isValidImageSource(const QString& source) const {
    if (source.isEmpty()) {
        return false;
    }

    // Check if it's a valid file path
    QFileInfo fileInfo(source);
    if (!fileInfo.exists()) {
        return false;
    }

    // Check if it's a readable file
    if (!fileInfo.isReadable()) {
        return false;
    }

    // Check file size (limit to 100MB)
    const qint64 maxFileSize = 100 * 1024 * 1024;  // 100MB
    if (fileInfo.size() > maxFileSize) {
        return false;
    }

    // Check if it's a supported image format
    return isValidImageFormat(source);
}

bool FluentImageView::isValidImageFormat(const QString& filePath) const {
    QImageReader reader(filePath);
    return reader.canRead();
}

QStringList FluentImageView::supportedImageFormats() const {
    QStringList formats;
    const auto supportedFormats = QImageReader::supportedImageFormats();
    for (const auto& format : supportedFormats) {
        formats.append(QString::fromLatin1(format).toLower());
    }
    return formats;
}

QString FluentImageView::accessibleName() const { return m_accessibleName; }

void FluentImageView::setAccessibleName(const QString& name) {
    if (m_accessibleName != name) {
        m_accessibleName = name;
        updateAccessibility();
        emit accessibleNameChanged(name);
    }
}

QString FluentImageView::accessibleDescription() const {
    return m_accessibleDescription;
}

void FluentImageView::setAccessibleDescription(const QString& description) {
    if (m_accessibleDescription != description) {
        m_accessibleDescription = description;
        updateAccessibility();
        emit accessibleDescriptionChanged(description);
    }
}

bool FluentImageView::isKeyboardNavigationEnabled() const {
    return m_keyboardNavigationEnabled;
}

void FluentImageView::setKeyboardNavigationEnabled(bool enabled) {
    if (m_keyboardNavigationEnabled != enabled) {
        m_keyboardNavigationEnabled = enabled;
        setFocusPolicy(enabled ? Qt::StrongFocus : Qt::NoFocus);
        updateAccessibility();
        emit keyboardNavigationEnabledChanged(enabled);
    }
}

int FluentImageView::rotationAngle() const { return m_rotationAngle; }

void FluentImageView::setRotationAngle(int angle) {
    // Normalize angle to 0-359 range
    angle = angle % 360;
    if (angle < 0)
        angle += 360;

    if (m_rotationAngle != angle) {
        m_rotationAngle = angle;
        updateImageDisplay();
        updateAccessibility();
        emit rotationAngleChanged(angle);
    }
}

qreal FluentImageView::zoomStep() const { return m_zoomStep; }

void FluentImageView::setZoomStep(qreal step) {
    if (step > 0 && m_zoomStep != step) {
        m_zoomStep = step;
        emit zoomStepChanged(step);
    }
}

bool FluentImageView::isAnimated() const { return m_movie != nullptr; }

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

QMovie* FluentImageView::movie() const { return m_movie; }

QSize FluentImageView::sizeHint() const {
    FLUENT_PROFILE("FluentImageView::sizeHint");

    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }

    QSize hint(300, 200);  // Default size

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

QSize FluentImageView::minimumSizeHint() const { return QSize(100, 100); }

void FluentImageView::zoomIn() {
    setZoomFactor(m_zoomFactor * (1.0 + m_zoomStep));
}

void FluentImageView::zoomOut() {
    setZoomFactor(m_zoomFactor / (1.0 + m_zoomStep));
}

void FluentImageView::zoomToFit() {
    if (m_pixmap.isNull())
        return;

    const QSize availableSize = m_scrollArea->viewport()->size();
    const QSize imageSize = m_pixmap.size();

    const qreal scaleX =
        static_cast<qreal>(availableSize.width()) / imageSize.width();
    const qreal scaleY =
        static_cast<qreal>(availableSize.height()) / imageSize.height();
    const qreal scale = std::min(scaleX, scaleY);

    setZoomFactor(scale);
}

void FluentImageView::zoomToActualSize() { setZoomFactor(1.0); }

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

void FluentImageView::rotateToAngle(int angle) { setRotationAngle(angle); }

void FluentImageView::zoomToPoint(const QPoint& point, qreal factor) {
    if (!m_zoomEnabled || m_pixmap.isNull()) {
        return;
    }

    // Calculate the new zoom factor
    qreal newZoomFactor = m_zoomFactor * factor;
    newZoomFactor = qBound(m_minZoomFactor, newZoomFactor, m_maxZoomFactor);

    if (newZoomFactor == m_zoomFactor) {
        return;  // No change needed
    }

    // Get the current scroll position
    QScrollBar* hScrollBar = m_scrollArea->horizontalScrollBar();
    QScrollBar* vScrollBar = m_scrollArea->verticalScrollBar();

    // Calculate the point relative to the image
    QPoint imagePoint = point - m_imageLabel->pos();

    // Calculate the relative position (0.0 to 1.0) within the current view
    qreal relativeX = static_cast<qreal>(imagePoint.x()) /
                      (m_imageLabel->width() * m_zoomFactor);
    qreal relativeY = static_cast<qreal>(imagePoint.y()) /
                      (m_imageLabel->height() * m_zoomFactor);

    // Update zoom factor
    m_zoomFactor = newZoomFactor;
    updateImageDisplay();

    // Calculate new scroll position to keep the point under the cursor
    int newScrollX =
        static_cast<int>(relativeX * m_imageLabel->width() * m_zoomFactor) -
        point.x();
    int newScrollY =
        static_cast<int>(relativeY * m_imageLabel->height() * m_zoomFactor) -
        point.y();

    // Apply the new scroll position
    hScrollBar->setValue(newScrollX);
    vScrollBar->setValue(newScrollY);

    updateControlsVisibility();
    updateAccessibility();
}

void FluentImageView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentImageView::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect rect = this->rect();

    // Paint elevation shadow first
    paintElevation(painter, rect);

    // Paint border
    paintBorder(painter, rect);

    // Paint loading indicator if loading
    if (m_loading) {
        paintLoadingIndicator(painter, rect);
    }

    // Paint error state if there's an error
    if (!m_errorMessage.isEmpty()) {
        paintErrorState(painter, rect);
    }
}

void FluentImageView::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    // Use design tokens for border styling with state-aware colors
    const int borderWidth = tokenUtils.getBorderWidth("thin");
    QColor borderColor;

    switch (state()) {
        case Core::FluentState::Hovered:
            borderColor = tokenUtils.getSemanticColor("stroke.control.hover");
            break;
        case Core::FluentState::Pressed:
            borderColor = tokenUtils.getSemanticColor("stroke.control.pressed");
            break;
        case Core::FluentState::Focused:
            borderColor =
                tokenUtils.getBrandColor(100);  // Accent color for focus
            break;
        default:
            borderColor = tokenUtils.getSemanticColor("stroke.control.default");
            break;
    }

    if (!borderColor.isValid()) {
        borderColor = palette.neutralQuaternary;
    }

    QPen borderPen(borderColor, borderWidth);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    // Use design token for corner radius
    const int radius = tokenUtils.getBorderRadius("medium");
    const QRectF borderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

    painter.drawRoundedRect(borderRect, radius, radius);

    // Paint focus ring if focused
    if (state() == Core::FluentState::Focused) {
        QPen focusPen(borderColor, 2);
        focusPen.setStyle(Qt::DashLine);
        painter.setPen(focusPen);
        QRectF focusRect = borderRect.adjusted(-2, -2, 2, 2);
        painter.drawRoundedRect(focusRect, radius + 2, radius + 2);
    }

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

void FluentImageView::keyPressEvent(QKeyEvent* event) {
    if (m_keyboardNavigationEnabled && handleKeyboardNavigation(event)) {
        event->accept();
        return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentImageView::focusInEvent(QFocusEvent* event) {
    Core::FluentComponent::focusInEvent(event);
    updateAccessibility();
}

void FluentImageView::focusOutEvent(QFocusEvent* event) {
    Core::FluentComponent::focusOutEvent(event);
    updateAccessibility();
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
    // Update visual styling based on current state
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    // Apply state-specific styling to scroll area
    if (m_scrollArea) {
        QPalette palette = m_scrollArea->palette();
        QColor backgroundColor;

        switch (state()) {
            case Core::FluentState::Hovered:
                backgroundColor =
                    tokenUtils.getSemanticColor("fill.control.secondary");
                break;
            case Core::FluentState::Pressed:
                backgroundColor =
                    tokenUtils.getSemanticColor("fill.control.tertiary");
                break;
            case Core::FluentState::Focused:
                backgroundColor =
                    tokenUtils.getSemanticColor("fill.control.default");
                break;
            default:
                backgroundColor =
                    tokenUtils.getSemanticColor("fill.control.default");
                break;
        }

        if (backgroundColor.isValid()) {
            palette.setColor(QPalette::Base, backgroundColor);
            m_scrollArea->setPalette(palette);
        }
    }

    update();
}

void FluentImageView::performStateTransition(Core::FluentState from,
                                             Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)

    // Animate state transitions if enabled
    if (isAnimated()) {
        // Create smooth transition animation
        auto* animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(transitionDuration());
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // Update styling for new state
    updateStateStyle();
}

void FluentImageView::onZoomInClicked() { zoomIn(); }

void FluentImageView::onZoomOutClicked() { zoomOut(); }

void FluentImageView::onZoomToFitClicked() { zoomToFit(); }

void FluentImageView::onActualSizeClicked() { zoomToActualSize(); }

void FluentImageView::onRotateLeftClicked() { rotateLeft(); }

void FluentImageView::onRotateRightClicked() { rotateRight(); }

void FluentImageView::onThemeChanged() {
    updateColors();
    updateFonts();
    update();
}

void FluentImageView::updateColors() {
    // Colors are handled in paintEvent and by child widgets
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    // Update scroll area background
    if (m_scrollArea) {
        QPalette palette = m_scrollArea->palette();
        QColor backgroundColor =
            tokenUtils.getSemanticColor("fill.control.default");
        if (backgroundColor.isValid()) {
            palette.setColor(QPalette::Base, backgroundColor);
            m_scrollArea->setPalette(palette);
        }
    }
}

void FluentImageView::updateFonts() {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    const QFont bodyFont = tokenUtils.getBodyFont();
    m_imageLabel->setFont(bodyFont);
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

        scaledPixmap = scaledPixmap.transformed(
            transform, m_smoothTransformation ? Qt::SmoothTransformation
                                              : Qt::FastTransformation);
    }

    // Apply zoom
    if (!qFuzzyCompare(m_zoomFactor, 1.0)) {
        const QSize zoomedSize = scaledPixmap.size() * m_zoomFactor;
        scaledPixmap = scaledPixmap.scaled(zoomedSize, Qt::KeepAspectRatio,
                                           m_smoothTransformation
                                               ? Qt::SmoothTransformation
                                               : Qt::FastTransformation);
    }

    // Apply scale mode
    switch (m_scaleMode) {
        case FluentImageScaleMode::None:
            // No additional scaling
            break;

        case FluentImageScaleMode::Fit:
            scaledPixmap = scaledPixmap.scaled(targetSize, Qt::KeepAspectRatio,
                                               m_smoothTransformation
                                                   ? Qt::SmoothTransformation
                                                   : Qt::FastTransformation);
            break;

        case FluentImageScaleMode::Fill:
            scaledPixmap = scaledPixmap.scaled(
                targetSize, Qt::KeepAspectRatioByExpanding,
                m_smoothTransformation ? Qt::SmoothTransformation
                                       : Qt::FastTransformation);
            break;

        case FluentImageScaleMode::Stretch:
            scaledPixmap = scaledPixmap.scaled(
                targetSize, Qt::IgnoreAspectRatio,
                m_smoothTransformation ? Qt::SmoothTransformation
                                       : Qt::FastTransformation);
            break;

        case FluentImageScaleMode::FitWidth: {
            const qreal scale =
                static_cast<qreal>(targetSize.width()) / scaledPixmap.width();
            const QSize newSize(targetSize.width(),
                                scaledPixmap.height() * scale);
            scaledPixmap = scaledPixmap.scaled(newSize, Qt::IgnoreAspectRatio,
                                               m_smoothTransformation
                                                   ? Qt::SmoothTransformation
                                                   : Qt::FastTransformation);
        } break;

        case FluentImageScaleMode::FitHeight: {
            const qreal scale =
                static_cast<qreal>(targetSize.height()) / scaledPixmap.height();
            const QSize newSize(scaledPixmap.width() * scale,
                                targetSize.height());
            scaledPixmap = scaledPixmap.scaled(newSize, Qt::IgnoreAspectRatio,
                                               m_smoothTransformation
                                                   ? Qt::SmoothTransformation
                                                   : Qt::FastTransformation);
        } break;
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
        m_zoomInButton->setEnabled(m_zoomEnabled &&
                                   m_zoomFactor < m_maxZoomFactor);
        m_zoomOutButton->setEnabled(m_zoomEnabled &&
                                    m_zoomFactor > m_minZoomFactor);
        m_zoomToFitButton->setEnabled(m_zoomEnabled);
        m_actualSizeButton->setEnabled(m_zoomEnabled);
        m_rotateLeftButton->setEnabled(true);
        m_rotateRightButton->setEnabled(true);
    }

    // Update tab order when controls visibility changes
    updateTabOrder();
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

void FluentImageView::paintElevation(QPainter& painter, const QRect& rect) {
    if (m_elevation <= 0)
        return;

    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    painter.save();

    // Get shadow properties based on elevation level
    const int radius = tokenUtils.getBorderRadius("medium");

    // Define shadow properties for different elevation levels
    struct ShadowLayer {
        int offsetX, offsetY, blur;
        QColor color;
    };

    QList<ShadowLayer> shadowLayers;

    switch (m_elevation) {
        case 1:  // Subtle shadow
            shadowLayers.append({0, 1, 2, QColor(0, 0, 0, 14)});
            shadowLayers.append({0, 1, 3, QColor(0, 0, 0, 12)});
            break;
        case 2:  // Default shadow
            shadowLayers.append({0, 2, 4, QColor(0, 0, 0, 14)});
            shadowLayers.append({0, 1, 10, QColor(0, 0, 0, 12)});
            break;
        case 3:  // Medium shadow
            shadowLayers.append({0, 4, 8, QColor(0, 0, 0, 14)});
            shadowLayers.append({0, 2, 16, QColor(0, 0, 0, 12)});
            break;
        case 4:  // Strong shadow
            shadowLayers.append({0, 8, 16, QColor(0, 0, 0, 14)});
            shadowLayers.append({0, 4, 32, QColor(0, 0, 0, 12)});
            break;
        default:  // Very strong shadow
            shadowLayers.append({0, 16, 32, QColor(0, 0, 0, 14)});
            shadowLayers.append({0, 8, 64, QColor(0, 0, 0, 12)});
            break;
    }

    // Paint shadow layers
    for (const auto& shadow : shadowLayers) {
        painter.setBrush(shadow.color);
        painter.setPen(Qt::NoPen);

        QRect shadowRect = rect.adjusted(shadow.offsetX, shadow.offsetY,
                                         shadow.offsetX, shadow.offsetY);

        // Simulate blur by drawing multiple offset rectangles with decreasing
        // opacity
        for (int i = 0; i < shadow.blur / 2; ++i) {
            QColor blurColor = shadow.color;
            blurColor.setAlpha(shadow.color.alpha() / (i + 1));
            painter.setBrush(blurColor);

            QRect blurRect = shadowRect.adjusted(-i, -i, i, i);
            painter.drawRoundedRect(blurRect, radius, radius);
        }
    }

    painter.restore();
}

void FluentImageView::paintPlaceholder(QPainter& painter, const QRect& rect) {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    // Paint placeholder background with proper FluentUI styling
    QColor backgroundColor =
        tokenUtils.getSemanticColor("fill.control.default");
    if (!backgroundColor.isValid()) {
        backgroundColor = palette.neutralLighter;
    }

    const int radius = tokenUtils.getBorderRadius("medium");
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, radius, radius);

    // Paint placeholder icon
    QRect iconRect(0, 0, 48, 48);
    iconRect.moveCenter(rect.center());
    iconRect.moveTop(rect.center().y() - 50);

    QColor iconColor = tokenUtils.getSemanticColor("text.secondary");
    if (!iconColor.isValid()) {
        iconColor = palette.neutralSecondary;
    }

    painter.setPen(QPen(iconColor, 2));
    painter.setBrush(Qt::NoBrush);

    // Draw image placeholder icon (simple rectangle with mountain)
    painter.drawRoundedRect(iconRect, 4, 4);

    // Draw mountain shape inside
    QPolygon mountain;
    mountain << QPoint(iconRect.left() + 12, iconRect.bottom() - 8)
             << QPoint(iconRect.left() + 20, iconRect.top() + 12)
             << QPoint(iconRect.left() + 28, iconRect.top() + 18)
             << QPoint(iconRect.right() - 12, iconRect.bottom() - 8);
    painter.drawPolyline(mountain);

    // Draw sun
    painter.drawEllipse(iconRect.right() - 18, iconRect.top() + 8, 8, 8);

    // Paint placeholder text below icon
    painter.setPen(iconColor);
    painter.setFont(tokenUtils.getBodyFont());

    QRect textRect = rect;
    textRect.setTop(iconRect.bottom() + tokenUtils.getSpacing("s"));
    textRect = textRect.adjusted(tokenUtils.getSpacing("m"), 0,
                                 -tokenUtils.getSpacing("m"),
                                 -tokenUtils.getSpacing("m"));

    painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop,
                     m_placeholderText);

    painter.restore();
}

void FluentImageView::paintLoadingIndicator(QPainter& painter,
                                            const QRect& rect) {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    painter.save();

    // Paint semi-transparent overlay with proper FluentUI styling
    QColor overlayColor = tokenUtils.getSemanticColor("fill.control.secondary");
    if (!overlayColor.isValid()) {
        overlayColor = QColor(255, 255, 255, 200);
    }
    overlayColor.setAlpha(200);

    painter.fillRect(rect, overlayColor);

    // Paint loading spinner in center with FluentUI styling
    QRect spinnerRect(0, 0, 40, 40);
    spinnerRect.moveCenter(rect.center());

    QColor accentColor = tokenUtils.getBrandColor(100);
    if (!accentColor.isValid()) {
        accentColor = QColor(0, 120, 212);  // Default blue
    }

    painter.setPen(QPen(accentColor, 4));
    painter.setBrush(Qt::NoBrush);

    // Animated rotating arc with smooth animation
    static int rotation = 0;
    rotation = (rotation + 8) % 360;
    painter.drawArc(spinnerRect, rotation * 16, 90 * 16);

    // Add loading text below spinner
    QRect textRect = rect;
    textRect.setTop(spinnerRect.bottom() + tokenUtils.getSpacing("s"));

    painter.setPen(tokenUtils.getSemanticColor("text.primary"));
    painter.setFont(tokenUtils.getCaptionFont());
    painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, "Loading...");

    painter.restore();

    // Schedule repaint for smooth animation
    QTimer::singleShot(33, this, [this]() {
        if (m_loading)
            update();
    });
}

void FluentImageView::paintErrorState(QPainter& painter, const QRect& rect) {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    painter.save();

    // Paint error background with proper corner radius
    QColor errorColor = tokenUtils.getSemanticColor("fill.critical.secondary");
    if (!errorColor.isValid()) {
        errorColor = QColor(255, 240, 240);
    }

    const int radius = tokenUtils.getBorderRadius("medium");
    painter.setBrush(errorColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, radius, radius);

    // Paint error icon in center
    QRect iconRect(0, 0, 32, 32);
    iconRect.moveCenter(rect.center());
    iconRect.moveTop(rect.center().y() - 40);

    QColor iconColor = tokenUtils.getSemanticColor("text.critical.primary");
    if (!iconColor.isValid()) {
        iconColor = QColor(196, 43, 28);
    }

    painter.setPen(QPen(iconColor, 3));
    painter.setBrush(Qt::NoBrush);

    // Draw error icon (exclamation mark in circle)
    painter.drawEllipse(iconRect);

    // Draw exclamation mark
    QRect exclamationRect = iconRect.adjusted(12, 8, -12, -8);
    painter.drawLine(exclamationRect.center().x(), exclamationRect.top(),
                     exclamationRect.center().x(),
                     exclamationRect.bottom() - 6);
    painter.drawEllipse(exclamationRect.center().x() - 1,
                        exclamationRect.bottom() - 3, 2, 2);

    // Paint error text below icon
    QColor textColor = tokenUtils.getSemanticColor("text.critical.primary");
    if (!textColor.isValid()) {
        textColor = QColor(196, 43, 28);
    }

    painter.setPen(textColor);
    painter.setFont(tokenUtils.getBodyFont());

    QRect textRect = rect;
    textRect.setTop(iconRect.bottom() + tokenUtils.getSpacing("s"));
    textRect = textRect.adjusted(tokenUtils.getSpacing("m"), 0,
                                 -tokenUtils.getSpacing("m"),
                                 -tokenUtils.getSpacing("m"));

    painter.drawText(
        textRect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
        m_errorMessage.isEmpty() ? "Failed to load image" : m_errorMessage);

    painter.restore();
}

void FluentImageView::setupAccessibility() {
    // Register component with accessibility system
    using namespace Accessibility;
    auto& accessibilityManager =
        FluentAccessibilityComplianceManager::instance();

    FluentComponentAccessibility metadata;
    metadata.role = "img";
    metadata.label =
        m_accessibleName.isEmpty() ? "Image viewer" : m_accessibleName;
    metadata.description = m_accessibleDescription.isEmpty()
                               ? "Interactive image display component"
                               : m_accessibleDescription;
    metadata.isFocusable = m_keyboardNavigationEnabled;
    metadata.keyboardShortcut =
        "Space: Toggle controls, +/-: Zoom, Arrow keys: Navigate";

    accessibilityManager.registerComponent(this, metadata);

    // Set initial accessibility properties
    updateAccessibility();
}

void FluentImageView::updateAccessibility() {
    using namespace Accessibility;
    auto& accessibilityManager =
        FluentAccessibilityComplianceManager::instance();

    // Update accessible name
    QString accessibleName = m_accessibleName;
    if (accessibleName.isEmpty()) {
        if (!m_source.isEmpty()) {
            QFileInfo fileInfo(m_source);
            accessibleName = QString("Image: %1").arg(fileInfo.baseName());
        } else if (!m_pixmap.isNull()) {
            accessibleName = QString("Image %1x%2")
                                 .arg(m_pixmap.width())
                                 .arg(m_pixmap.height());
        } else {
            accessibleName = "Image viewer";
        }
    }

    // Add state information
    QStringList stateInfo;
    if (m_loading) {
        stateInfo << "Loading";
    }
    if (!m_errorMessage.isEmpty()) {
        stateInfo << "Error";
    }
    if (m_zoomEnabled) {
        stateInfo << QString("Zoom %1%").arg(qRound(m_zoomFactor * 100));
    }

    if (!stateInfo.isEmpty()) {
        accessibleName += QString(" (%1)").arg(stateInfo.join(", "));
    }

    accessibilityManager.setAriaLabel(this, accessibleName);

    // Update description
    QString description = m_accessibleDescription;
    if (description.isEmpty()) {
        if (!m_errorMessage.isEmpty()) {
            description = QString("Error: %1").arg(m_errorMessage);
        } else if (m_loading) {
            description = "Loading image...";
        } else if (!m_pixmap.isNull()) {
            description =
                QString("Image loaded successfully. Size: %1x%2 pixels")
                    .arg(m_pixmap.width())
                    .arg(m_pixmap.height());
        } else {
            description = "No image loaded";
        }
    }

    accessibilityManager.setAriaDescription(this, description);

    // Update ARIA states
    if (m_loading) {
        accessibilityManager.setAriaState(this, "busy", "true");
    } else {
        accessibilityManager.setAriaState(this, "busy", "false");
    }

    // Update focus policy
    setFocusPolicy(m_keyboardNavigationEnabled ? Qt::StrongFocus : Qt::NoFocus);
}

bool FluentImageView::handleKeyboardNavigation(QKeyEvent* event) {
    if (!m_keyboardNavigationEnabled) {
        return false;
    }

    // Handle modifier combinations first
    const bool ctrlPressed = event->modifiers() & Qt::ControlModifier;
    // Note: Shift modifier reserved for future use

    switch (event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            if (m_zoomEnabled && ctrlPressed) {
                zoomIn();
                updateAccessibility();  // Update screen reader info
                return true;
            }
            break;

        case Qt::Key_Minus:
        case Qt::Key_Underscore:
            if (m_zoomEnabled && ctrlPressed) {
                zoomOut();
                updateAccessibility();
                return true;
            }
            break;

        case Qt::Key_0:
            if (m_zoomEnabled && ctrlPressed) {
                zoomToActualSize();
                updateAccessibility();
                return true;
            }
            break;

        case Qt::Key_F:
            if (m_zoomEnabled && !ctrlPressed) {
                zoomToFit();
                updateAccessibility();
                return true;
            }
            break;

        case Qt::Key_Space:
            if (!ctrlPressed) {
                setShowControls(!m_showControls);
                return true;
            }
            break;

        case Qt::Key_R:
            if (ctrlPressed) {
                rotateRight();
                updateAccessibility();
                return true;
            }
            break;

        case Qt::Key_L:
            if (ctrlPressed) {
                rotateLeft();
                updateAccessibility();
                return true;
            }
            break;

        case Qt::Key_Home:
            if (ctrlPressed) {
                resetZoom();
                updateAccessibility();
                return true;
            }
            break;

        case Qt::Key_H:
            if (ctrlPressed) {
                flipHorizontal();
                updateAccessibility();
                return true;
            }
            break;

        case Qt::Key_V:
            if (ctrlPressed) {
                flipVertical();
                updateAccessibility();
                return true;
            }
            break;

        case Qt::Key_Escape:
            // Clear focus or hide controls
            if (m_showControls) {
                setShowControls(false);
                return true;
            }
            break;

        case Qt::Key_Tab:
            // Let Qt handle tab navigation, but ensure proper order
            if (m_showControls && m_controlsWidget->isVisible()) {
                // Tab navigation will be handled by Qt using our tab order
                return false;  // Let Qt handle it
            }
            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Toggle controls on Enter
            setShowControls(!m_showControls);
            return true;

        default:
            break;
    }

    return false;
}

void FluentImageView::setupTabOrder() {
    if (!m_keyboardNavigationEnabled) {
        return;
    }

    // Create tab order for control buttons when they're visible
    QList<QWidget*> tabOrder;

    // Add main image view first
    tabOrder.append(this);

    // Add control buttons if they exist and are visible
    if (m_showControls && m_controlsWidget && m_controlsWidget->isVisible()) {
        if (m_zoomInButton)
            tabOrder.append(m_zoomInButton);
        if (m_zoomOutButton)
            tabOrder.append(m_zoomOutButton);
        if (m_zoomToFitButton)
            tabOrder.append(m_zoomToFitButton);
        if (m_actualSizeButton)
            tabOrder.append(m_actualSizeButton);
        if (m_rotateLeftButton)
            tabOrder.append(m_rotateLeftButton);
        if (m_rotateRightButton)
            tabOrder.append(m_rotateRightButton);
    }

    // Set tab order
    for (int i = 0; i < tabOrder.size() - 1; ++i) {
        QWidget::setTabOrder(tabOrder[i], tabOrder[i + 1]);
    }
}

void FluentImageView::updateTabOrder() { setupTabOrder(); }

}  // namespace FluentQt::Components
