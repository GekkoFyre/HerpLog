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
 * @file mainwindow.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-15
 * @brief The main, opening window to the program.
 */

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "./../options.hpp"
#include "./../gk_db.hpp"
#include <boost/filesystem.hpp>
#include <QMainWindow>
#include <memory>

using namespace GekkoFyre;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button_create_db_clicked();
    void on_button_open_db_clicked();
    void on_button_exit_clicked();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<GkDb> gkDb;
    GkFile::FileDb db_ptr;
    fs::path tmp_db_loc;
};

#endif // MAINWINDOW_HPP
