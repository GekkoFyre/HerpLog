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
 * @file herpapp.cpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-18
 * @brief The primary interface to the HerpLog applications' workings.
 */

#include "herpapp.hpp"
#include "ui_herpapp.h"
#include <boost/exception/all.hpp>
#include <QMessageBox>
#include <QToolButton>
#include <random>
#include <chrono>

namespace sys = boost::system;
HerpApp::HerpApp(const GkFile::FileDb &database, const std::string &temp_db_dir, QWidget *parent) :
        QMainWindow(parent), ui(new Ui::HerpApp)
{
    ui->setupUi(this);

    db_ptr = database;
    global_temp_dir = temp_db_dir;

    gkDb = std::make_unique<GkDb>(this);
    gkStrOp = std::make_unique<GkStringOp>(this);
}

HerpApp::~HerpApp()
{
    remove_files(global_temp_dir);
    delete ui;
}

bool HerpApp::remove_files(const fs::path &dirLoc)
{
    sys::error_code ec;
    if (fs::is_directory(dirLoc, ec)) {
        if (!fs::remove_all(dirLoc, ec)) {
            QMessageBox::warning(nullptr, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
            return false;
        } else {
            return true;
        }
    }

    QMessageBox::warning(nullptr, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
    return false;
}

void HerpApp::on_action_New_Database_triggered()
{}

void HerpApp::on_action_Open_Database_triggered()
{}

void HerpApp::on_action_Disconnect_triggered()
{}

void HerpApp::on_action_Save_triggered()
{}

void HerpApp::on_actionSave_As_triggered()
{}

void HerpApp::on_actionSave_A_ll_triggered()
{}

void HerpApp::on_action_Print_triggered()
{}

void HerpApp::on_actionE_xit_triggered()
{}

void HerpApp::on_action_Undo_triggered()
{}

void HerpApp::on_action_Redo_triggered()
{}

void HerpApp::on_actionCu_t_triggered()
{}

void HerpApp::on_action_Copy_triggered()
{}

void HerpApp::on_action_Paste_triggered()
{}

void HerpApp::on_actionF_ind_triggered()
{}

void HerpApp::on_action_Settings_triggered()
{}

void HerpApp::on_action_Documentation_triggered()
{}

void HerpApp::on_action_About_triggered()
{}

void HerpApp::on_pushButton_archive_next_clicked()
{}

void HerpApp::on_pushButton_archive_prev_clicked()
{}

void HerpApp::on_pushButton_browse_submit_clicked()
{}

void HerpApp::on_pushButton_add_data_clicked()
{}

std::string HerpApp::random_hash()
{
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 mt_rand(seed);
    std::uniform_int_distribution<int> dist(1, 10);
    auto result = dist(mt_rand);

    return gkStrOp->getCrc32(std::to_string(result));
}
