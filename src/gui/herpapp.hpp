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
 * @file herpapp.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-18
 * @brief The primary interface to the HerpLog applications' workings.
 */

#ifndef HERPAPP_HPP
#define HERPAPP_HPP

#include "./../options.hpp"
#include <QMainWindow>
#include <memory>

using namespace GekkoFyre;
namespace Ui {
class HerpApp;
}

class HerpApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit HerpApp(const GkFile::FileDb &database, QWidget *parent = 0);
    ~HerpApp();

private slots:
    void on_action_New_Database_triggered();
    void on_action_Open_Database_triggered();
    void on_action_Disconnect_triggered();
    void on_action_Save_triggered();
    void on_actionSave_As_triggered();
    void on_actionSave_A_ll_triggered();
    void on_action_Print_triggered();
    void on_actionE_xit_triggered();
    void on_action_Undo_triggered();
    void on_action_Redo_triggered();
    void on_actionCu_t_triggered();
    void on_action_Copy_triggered();
    void on_action_Paste_triggered();
    void on_actionF_ind_triggered();
    void on_action_Settings_triggered();
    void on_action_Documentation_triggered();
    void on_action_About_triggered();

private:
    Ui::HerpApp *ui;

    GkFile::FileDb db_ptr;
};

#endif // HERPAPP_HPP
