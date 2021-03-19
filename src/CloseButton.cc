/*
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// own
#include "CloseButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// Qt
#include <QPainter>

namespace Fluent
{

CloseButton::CloseButton(Decoration *decoration, QObject *parent)
    : DecorationButton(KDecoration2::DecorationButtonType::Close, decoration, parent)
{
    auto *decoratedClient = decoration->client().toStrongRef().data();
    connect(decoratedClient, &KDecoration2::DecoratedClient::closeableChanged,
            this, &CloseButton::setVisible);

    connect(this, &CloseButton::hoveredChanged, this,
        [this] {
            update();
        });

    const int titleBarThin = decoration->titleBarVertical()?decoration->titleBarWidth():decoration->titleBarHeight();
    const int titleBarWide=qRound(titleBarThin * 1.33)+(titleBarThin-qRound(titleBarThin * 1.33))%2;
    const QSize size(decoration->titleBarVertical()?titleBarThin:titleBarWide, decoration->titleBarVertical()?titleBarWide:titleBarThin);
    setGeometry(QRect(QPoint(0, 0), size));
    setVisible(decoratedClient->isCloseable());
}

CloseButton::~CloseButton()
{
}

void CloseButton::paint(QPainter *painter, const QRect &repaintRegion)
{
    Q_UNUSED(repaintRegion)

    const QRectF buttonRect = geometry();
    int sz=buttonRect.width()>buttonRect.height()?buttonRect.height():buttonRect.width();
   
    if((sz-qRound(sz/3.0))%2==0){
         sz=qRound(sz/3.0);
    }else{
         sz=qRound(sz/3.0)-1;
    }
    QRectF crossRect = QRectF(0, 0, sz,sz);
    QPoint center=buttonRect.center().toPoint();
    // crossRect.moveCenter(center);

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing, false);

    // Background.
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor());
    painter->drawRect(buttonRect);

    // Foreground.
    painter->setPen(foregroundColor());
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(crossRect.topLeft()+center-QPoint(sz/2,sz/2), crossRect.topLeft()+QPoint(sz/2,sz/2)+center);
    painter->drawLine(crossRect.topLeft()+QPoint(-sz/2,sz/2)+center, crossRect.topLeft()+QPoint(sz/2,-sz/2)+center);

    painter->restore();
}

QColor CloseButton::backgroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    if (isPressed()) {
        auto *decoratedClient = deco->client().toStrongRef().data();
        return decoratedClient->color(
            KDecoration2::ColorGroup::Warning,
            KDecoration2::ColorRole::Foreground
        ).lighter();
    }

    if (isHovered()) {
        auto *decoratedClient = deco->client().toStrongRef().data();
        return decoratedClient->color(
            KDecoration2::ColorGroup::Warning,
            KDecoration2::ColorRole::Foreground
        );
    }

    return Qt::transparent;
}

QColor CloseButton::foregroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    return deco->titleBarForegroundColor();
}

} // namespace Fluent
