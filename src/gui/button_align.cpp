/**
 **  _   _                 _
 ** | | | |               | |
 ** | |_| | ___ _ __ _ __ | |     ___   __ _
 ** |  _  |/ _ \ '__| '_ \| |    / _ \ / _` |
 ** | | | |  __/ |  | |_) | |___| (_) | (_| |
 ** \_| |_/\___|_|  | .__/\_____/\___/ \__, |
 **                 | |                 __/ |
 **                 |_|                |___/
 **
 **   Thank you for using "HerpLog" for your herpetology management requirements!
 **   Copyright (C) 2017. GekkoFyre.
 **
 **
 **   HerpLog is free software: you can redistribute it and/or modify
 **   it under the terms of the GNU General Public License as published by
 **   the Free Software Foundation, either version 3 of the License, or
 **   (at your option) any later version.
 **
 **   HerpLog is distributed in the hope that it will be useful,
 **   but WITHOUT ANY WARRANTY; without even the implied warranty of
 **   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **   GNU General Public License for more details.
 **
 **   You should have received a copy of the GNU General Public License
 **   along with HerpLog.  If not, see <http://www.gnu.org/licenses/>.
 **
 **
 **   The latest source code updates can be obtained from [ 1 ] below at your
 **   discretion. A web-browser or the 'git' application may be required.
 **
 **   [ 1 ] - https://github.com/GekkoFyre/HerpLog
 **
 ********************************************************************************/

/**
 * @file button_align.cpp
 * @note jpo38 <https://stackoverflow.com/questions/44091339/qpushbutton-icon-aligned-left-with-text-centered>
 * @date 2017-12-18
 * @brief A class that specializes QPushButton by overriding paintEvent and sizeHint, and in effect
 * aligns the icons on the button to the left and the text to the centre.
 */

#include "button_align.hpp"
#include "./../options.hpp"
#include <QPainter>

using namespace GekkoFyre;
BtnAlign::BtnAlign(QWidget *parent) : QPushButton(parent)
{}

BtnAlign::~BtnAlign()
{}

void BtnAlign::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QSize BtnAlign::sizeHint() const
{
    const auto parentHint = QPushButton::sizeHint();

    // Add margins here if required
    return QSize((parentHint.width() + m_pixmap.width()), std::max(parentHint.height(), m_pixmap.height()));
}

void BtnAlign::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);

    if (!m_pixmap.isNull()) {
        const int y = ((height() - m_pixmap.height()) / 2); // Add margin if required
        QPainter painter(this);
        painter.drawPixmap(GekkoFyre::HERPLOG_DEFAULT_HORIZONTAL_MARGIN_PUSHBUTTON, y, m_pixmap);
    }
}
