/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */
#include "DecorationMagicButtonGroup.h"
#include "DecorationMagicButtonGroup_p.h"
#include <KDecoration2/DecoratedClient>
#include <KDecoration2/DecorationButtonGroup>
#include <KDecoration2/DecorationSettings>
#include <KDecoration2/Decoration>
// #include "decorationsettings.h"

#include <QDebug>

namespace KDecoration2G
{

DecorationMagicButtonGroup::Private::Private(KDecoration2::Decoration *decoration, KDecoration2G::DecorationMagicButtonGroup *parent)
    : decoration(decoration)
    , spacing(0.0)
    , q(parent)
{
}

DecorationMagicButtonGroup::Private::~Private() = default;

void DecorationMagicButtonGroup::Private::setGeometry(const QRectF &geo)
{
    if (geometry == geo) {
        return;
    }
    geometry = geo;
    emit q->geometryChanged(geometry);
}

namespace {
static bool s_layoutRecursion = false;
}

void DecorationMagicButtonGroup::Private::updateLayout()
{
    if (s_layoutRecursion) {
        return;
    }
    s_layoutRecursion = true;
    const QPointF &pos = geometry.topLeft();
    // first calculate new size
    qreal height = 0;
    qreal width = 0;
    for (auto it = buttons.constBegin(); it != buttons.constEnd(); ++it) {
        if (!(*it)->isVisible()) {
            continue;
        }
        width = qMax(width, qreal((*it)->size().width()));
        height += (*it)->size().height();
        if (it + 1 != buttons.constEnd()) {
            height += spacing;
        }
    }
    setGeometry(QRectF(pos, QSizeF(width, height)));

    // now position all buttons
    qreal position = pos.y();
    const auto &constButtons = buttons;
    for (auto button: constButtons) {
        if (!button->isVisible()) {
            continue;
        }
        const QSizeF size = button->size();
        // TODO: center
        button->setGeometry(QRectF(QPointF(pos.x(),position), size));
        position += size.height() + spacing;
    }
    s_layoutRecursion = false;
}

DecorationMagicButtonGroup::DecorationMagicButtonGroup(KDecoration2::Decoration *parent)
    : QObject(parent)
    , d(new Private(parent, this))
{
}

DecorationMagicButtonGroup::DecorationMagicButtonGroup(KDecoration2::DecorationButtonGroup::Position type, KDecoration2::Decoration *parent, std::function<KDecoration2::DecorationButton*(KDecoration2::DecorationButtonType, KDecoration2::Decoration*, QObject*)> buttonCreator)
    : QObject(parent)
    , d(new Private(parent, this))
{
    auto settings = parent->settings();
    auto createButtons = [=] {
        const auto &buttons = (type == KDecoration2::DecorationButtonGroup::Position::Left) ?
            settings->decorationButtonsLeft() :
            settings->decorationButtonsRight();
        for (KDecoration2::DecorationButtonType type : buttons) {
            if (KDecoration2::DecorationButton *b = buttonCreator(type, parent, this)) {
                addButton(QPointer<KDecoration2::DecorationButton>(b));
            }
        }
    };
    createButtons();
    auto changed = type == KDecoration2::DecorationButtonGroup::Position::Left ? &KDecoration2::DecorationSettings::decorationButtonsLeftChanged : &KDecoration2::DecorationSettings::decorationButtonsRightChanged;
    connect(settings.data(), changed, this,
        [this, createButtons] {
            qDeleteAll(d->buttons);
            d->buttons.clear();
            createButtons();
        }
    );
}

DecorationMagicButtonGroup::~DecorationMagicButtonGroup() = default;

QPointer<KDecoration2::Decoration> DecorationMagicButtonGroup::decoration() const
{
    return QPointer<KDecoration2::Decoration>(d->decoration);
}

QRectF DecorationMagicButtonGroup::geometry() const
{
    return d->geometry;
}

bool DecorationMagicButtonGroup::hasButton(KDecoration2::DecorationButtonType type) const
{
    // TODO: check for deletion of button
    auto it = std::find_if(d->buttons.begin(), d->buttons.end(),
        [type](const QPointer<KDecoration2::DecorationButton> &button) {
            return button->type() == type;
        }
    );
    return it != d->buttons.end();
}

qreal DecorationMagicButtonGroup::spacing() const
{
    return d->spacing;
}

QPointF DecorationMagicButtonGroup::pos() const
{
    return d->geometry.topLeft();
}

void DecorationMagicButtonGroup::setPos(const QPointF &pos)
{
    if (d->geometry.topLeft() == pos) {
        return;
    }
    d->setGeometry(QRectF(pos, d->geometry.size()));
    d->updateLayout();
}

void DecorationMagicButtonGroup::setSpacing(qreal spacing)
{
    if (d->spacing == spacing) {
        return;
    }
    d->spacing = spacing;
    emit spacingChanged(d->spacing);
    d->updateLayout();
}

void DecorationMagicButtonGroup::addButton(const QPointer<KDecoration2::DecorationButton> &button)
{
    Q_ASSERT(!button.isNull());
    connect(button.data(), &KDecoration2::DecorationButton::visibilityChanged, this, [this]() { d->updateLayout(); });
    connect(button.data(), &KDecoration2::DecorationButton::geometryChanged, this, [this]() { d->updateLayout(); });
    d->buttons.append(button);
    d->updateLayout();
}

QVector<QPointer<KDecoration2::DecorationButton>> DecorationMagicButtonGroup::buttons() const
{
    return d->buttons;
}

void DecorationMagicButtonGroup::removeButton(KDecoration2::DecorationButtonType type)
{
    bool needUpdate = false;
    auto it = d->buttons.begin();
    while (it != d->buttons.end()) {
        if ((*it)->type() == type) {
            it = d->buttons.erase(it);
            needUpdate = true;
        } else {
            it++;
        }
    }
    if (needUpdate) {
        d->updateLayout();
    }
}

void DecorationMagicButtonGroup::removeButton(const QPointer<KDecoration2::DecorationButton> &button)
{
    bool needUpdate = false;
    auto it = d->buttons.begin();
    while (it != d->buttons.end()) {
        if (*it == button) {
            it = d->buttons.erase(it);
            needUpdate = true;
        } else {
            it++;
        }
    }
    if (needUpdate) {
        d->updateLayout();
    }
}

void DecorationMagicButtonGroup::paint(QPainter *painter, const QRect &repaintArea)
{
    const auto &buttons = d->buttons;
    for (auto button: buttons) {
        if (!button->isVisible()) {
            continue;
        }
        button->paint(painter, repaintArea);
    }
}

} // namespace