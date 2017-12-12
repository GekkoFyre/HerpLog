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
 * @file main.cpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-12
 * @brief This file is self-explanatory.
 */

#include "gui/mainwindow.hpp"
#include "options.hpp"
#include <QApplication>
#include <memory>

#ifdef Q_OS_LINUX
extern "C" {
#include <X11/Xlib.h>
};

#endif

int main(int argc, char *argv[])
{
    // https://github.com/notepadqq/notepadqq/issues/323
    #ifdef __linux__
    Display *d = XOpenDisplay(nullptr);
    std::unique_ptr<Screen> s = std::make_unique<Screen>();
    s.reset(DefaultScreenOfDisplay(d));
    int width = s->width;
    double ratio = ((double)width / GekkoFyre::FYREDL_DEFAULT_RESOLUTION_WIDTH);
    if (ratio > 1.1) {
        qputenv("QT_SCALE_FACTOR", QString::number(ratio).toLatin1());
    }

    #else
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    #endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}