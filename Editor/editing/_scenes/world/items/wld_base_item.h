/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WLD_BASE_ITEM_H
#define WLD_BASE_ITEM_H

#include <QString>
#include <QPoint>

class WldBaseItem
{
public:
    WldBaseItem();
    virtual ~WldBaseItem();

    ///
    /// \brief arrayApply();
    /// Registring current state of item into data array
    ///
    virtual void arrayApply();

    ///
    /// \brief returnBack();
    /// Returning item into last saved position
    ///
    virtual void returnBack();

    ///
    /// \brief removeFromArray();
    /// Removing item from array
    ///
    virtual void removeFromArray();

    ///
    /// \brief setLayer();
    /// Changes parent layer
    ///
    /// \param layer
    /// Layer name
    ///
    virtual void setLayer(QString layer);

    ///
    /// \brief getGridSize
    /// \return grid size of item
    ///
    virtual int getGridSize();

    ///
    /// \brief gridOffset
    /// \return offset between grid point and aligned point
    ///
    virtual QPoint gridOffset();

    ///
    /// \brief sourcePos
    /// \return registred position of item
    ///
    virtual QPoint sourcePos();
};

#endif // WLD_BASE_ITEM_H
