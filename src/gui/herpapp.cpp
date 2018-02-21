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
#include <exception>
#include <random>
#include <chrono>

namespace sys = boost::system;
HerpApp::HerpApp(const GkFile::FileDb &database, const std::string &temp_db_dir, QWidget *parent) :
        QMainWindow(parent), ui(new Ui::HerpApp)
{
    ui->setupUi(this);

    db_ptr = database;
    global_temp_dir = temp_db_dir;

    gkStrOp = std::make_shared<GkStringOp>(this);
    gkDb = std::make_unique<GkDb>(db_ptr, gkStrOp, this);

    ui->lineEdit_new_id->setText(QString::fromStdString(gkStrOp->random_hash()));
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

void HerpApp::on_toolButton_new_hash_clicked()
{
    ui->lineEdit_new_id->clear();
    ui->lineEdit_new_id->setText(QString::fromStdString(gkStrOp->random_hash()));
}

void HerpApp::submit_record()
{
    try {
        if (!ui->lineEdit_new_species->text().isEmpty() && !ui->lineEdit_new_id->text().isEmpty()) {
            using namespace GkRecords;
            std::lock_guard<std::mutex> locker(w_record_mtx);

            GkSpecies species;
            species.species_name = ui->lineEdit_new_species->text().toStdString();

            GkId identifier;
            identifier.identifier_str = ui->lineEdit_new_id->text().toStdString();

            GkSubmit submit;
            submit.date_time = std::time(nullptr);
            submit.species = species;
            submit.identifier = identifier;
            submit.further_notes = ui->plainTextEdit_furtherNotes->toPlainText().toStdString();
            submit.vitamin_notes = ui->lineEdit_vitamins_notes->text().toStdString();
            submit.toilet_notes = ui->lineEdit_toilet_notes->text().toStdString();
            submit.weight_notes = ui->lineEdit_weight_notes->text().toStdString();
            submit.hydration_notes = ui->lineEdit_hydration_notes->text().toStdString();
            submit.went_toilet = ui->checkBox_toilet->isChecked();
            submit.had_hydration = ui->checkBox_hydration->isChecked();
            submit.had_vitamins = ui->checkBox_vitamins->isChecked();
            submit.weight = ui->spinBox_weight->value();
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }

    return;
}
