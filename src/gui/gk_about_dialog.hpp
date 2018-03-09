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
 **   Copyright (C) 2017-2018. GekkoFyre.
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
 * @file gk_about_dialog.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-09
 * @brief Displays licensing, author, contributor and contact information with regards to what has been involved in the
 * development of "HerpLog".
 */

#ifndef GK_ABOUT_DIALOG_HPP
#define GK_ABOUT_DIALOG_HPP

#include "./../options.hpp"
#include <QDialog>

using namespace GekkoFyre;
namespace Ui {
class GkAboutDialog;
}

class GkAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GkAboutDialog(QWidget *parent = nullptr);
    ~GkAboutDialog();

private slots:
    void on_pushButton_close_window_clicked();

private:
    Ui::GkAboutDialog *ui;
};

#endif // GK_ABOUT_DIALOG_HPP
