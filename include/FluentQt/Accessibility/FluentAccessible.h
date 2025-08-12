// include/FluentQt/Accessibility/FluentAccessible.h
#pragma once

#include <QAccessible>
#include <QAccessibleWidget>
#include <QString>
#include <memory>

namespace FluentQt::Accessibility {

class FluentAccessibleInterface : public QAccessibleWidget {
public:
    explicit FluentAccessibleInterface(QWidget* widget);

    // QAccessibleInterface implementation
    QString text(QAccessible::Text type) const override;
    void setText(QAccessible::Text type, const QString& text) override;
    QAccessible::Role role() const override;
    QAccessible::State state() const override;

    // Enhanced descriptions for Fluent components
    QString description() const;
    QString help() const;
    QString keyBindingsDescription() const;

protected:
    virtual QAccessible::Role getFluentRole() const;
    virtual QAccessible::State getFluentState() const;
    virtual QString getFluentDescription() const;

    // Safe object access helper methods (protected for testing)
    QObject* safeObject() const;
    QWidget* safeWidget() const;
    bool isObjectValid() const;
};

// Accessibility helper functions
void setAccessibleName(QWidget* widget, const QString& name);
void setAccessibleDescription(QWidget* widget, const QString& description);
void setAccessibleRole(QWidget* widget, QAccessible::Role role);
void announceToScreenReader(const QString& message);

// High contrast detection
bool isHighContrastMode();
void updateForHighContrast(QWidget* widget);

}  // namespace FluentQt::Accessibility
