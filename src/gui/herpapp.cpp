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
 * @file herpapp.cpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-18
 * @brief The primary interface to the HerpLog applications' workings.
 */

#include "herpapp.hpp"
#include "ui_herpapp.h"
#include <boost/exception/all.hpp>
#include <QMessageBox>
#include <QFileDialog>
#include <QToolButton>
#include <QDateTime>
#include <QString>
#include <exception>
#include <random>

namespace sys = boost::system;
HerpApp::HerpApp(const GkFile::FileDb &database, const std::string &temp_dir_path, const std::string &db_file_path,
                 const std::shared_ptr<GkFileIo> &file_io_ptr, QWidget *parent) : QMainWindow(parent), ui(new Ui::HerpApp)
{
    ui->setupUi(this);

    db_ptr = database;
    global_db_temp_dir = temp_dir_path; // The (base) temporary directory where the database has been extracted to
    global_db_file_path = db_file_path; // The file-path to the (currently opened/newly created) database
    gkFileIo = file_io_ptr;

    gkStrOp = std::make_shared<GkStringOp>(this);
    gkDbRead = std::make_shared<GkDbRead>(db_ptr, gkStrOp, this);
    gkDbWrite = std::make_unique<GkDbWrite>(db_ptr, gkDbRead, gkStrOp, this);

    record_id_cache = gkDbRead->get_record_ids();
    ui->lineEdit_new_id->setText(QString::fromStdString(gkStrOp->random_hash()));
    ui->dateTime_add_record->setDate(QDate::currentDate());
    ui->dateTime_add_record->setTime(QTime::currentTime());

    if (!record_id_cache.empty()) {
        std::vector<std::string> record_ids;
        for (const auto &ids: record_id_cache) {
            if (std::strcmp(ids.first.c_str(), GkRecords::csvRecordId) != 0) { // An ugly hack, I know...
                record_ids.push_back(ids.first);
            }
        }

        int minDateTime = gkDbRead->determineMinimumDate(record_ids);
        int maxDateTime = gkDbRead->determineMaximumDate(record_ids);

        ui->dateTimeEdit_browse_start->setMinimumDateTime(QDateTime::fromTime_t(minDateTime));
        ui->dateTimeEdit_browse_start->setMaximumDateTime(QDateTime::fromTime_t(maxDateTime));
        ui->dateTimeEdit_browse_end->setMinimumDateTime(QDateTime::fromTime_t(minDateTime));
        ui->dateTimeEdit_browse_end->setMaximumDateTime(QDateTime::fromTime_t(maxDateTime));
        ui->dateTimeEdit_browse_end->setDateTime(QDateTime::fromTime_t(maxDateTime));
    }
}

HerpApp::~HerpApp()
{
    remove_files(global_db_temp_dir);
    delete ui;
}

/**
 * @brief HerpApp::remove_files Attempts to remove the temporary directory that is created after extraction of the database (upon
 * opening), when making an exit from the application.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02
 * @param tmpDirLoc The location of the temporary directory itself.
 * @return Whether the operation was successful or not.
 */
bool HerpApp::remove_files(const fs::path &tmpDirLoc)
{
    sys::error_code ec;
    if (fs::is_directory(tmpDirLoc, ec)) {
        if (!fs::remove_all(tmpDirLoc, ec)) {
            QMessageBox::warning(this, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
            return false;
        } else {
            return true;
        }
    }

    return false;
}

void HerpApp::on_action_New_Database_triggered()
{
    QMessageBox::information(this, tr("Notice"), tr("This feature is not available yet, so check back soon!"), QMessageBox::Ok);
    return;
}

void HerpApp::on_action_Open_Database_triggered()
{
    QMessageBox::information(this, tr("Notice"), tr("This feature is not available yet, so check back soon!"), QMessageBox::Ok);
    return;
}

void HerpApp::on_action_Disconnect_triggered()
{
    QMessageBox::information(this, tr("Notice"), tr("This feature is not available yet, so check back soon!"), QMessageBox::Ok);
    return;
}

void HerpApp::on_action_Save_triggered()
{
    sys::error_code ec;
    if (fs::exists(global_db_file_path, ec)) { // Check that the database does exist, otherwise perform a "Save As"
        std::string temp_file_name = std::string(global_db_file_path + "." + gkStrOp->random_hash()); // Give the new, temporary file a random extension
        gkFileIo->compress_files(global_db_temp_dir.string(), temp_file_name); // Compress it
        if (!fs::remove(global_db_file_path, ec)) { // Remove the old database file
            QMessageBox::warning(this, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
            return;
        } else {
            fs::rename(temp_file_name, global_db_file_path, ec); // Rename the temporary database back so it's as if nothing changed, except for the contents of course :)
            if (ec.value() > 0) {
                QMessageBox::warning(this, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
                return;
            }
        }
    } else {
        on_actionSave_As_triggered();
    }

    return;
}

void HerpApp::on_actionSave_As_triggered()
{
    QString save_dest = QFileDialog::getSaveFileName(this, tr("Save As"), QString::fromStdString(global_db_file_path), tr("HerpLog Database Files (*.hdb)"));

    sys::error_code ec;
    std::string save_dest_str = save_dest.toStdString();
    if (fs::exists(save_dest_str, ec)) {
        if (!fs::remove(save_dest_str, ec)) {
            QMessageBox::warning(this, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
            return;
        } else {
            gkFileIo->compress_files(global_db_temp_dir.string(), save_dest_str);
        }
    }
}

void HerpApp::on_actionSave_A_ll_triggered()
{
    QMessageBox::information(this, tr("Notice"), tr("This feature is not available yet, so check back soon!"), QMessageBox::Ok);
    return;
}

void HerpApp::on_action_Print_triggered()
{}

void HerpApp::on_actionE_xit_triggered()
{
    QApplication::quit();
}

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

void HerpApp::on_pushButton_archive_delete_clicked()
{}

void HerpApp::on_pushButton_browse_submit_clicked()
{}

void HerpApp::on_pushButton_add_data_clicked()
{
    submit_record();
}

void HerpApp::on_toolButton_new_hash_clicked()
{
    ui->lineEdit_new_id->clear();
    ui->lineEdit_new_id->setText(QString::fromStdString(gkStrOp->random_hash()));
}

void HerpApp::on_toolButton_add_record_update_datetime_clicked()
{
    ui->dateTime_add_record->setDate(QDate::currentDate());
    ui->dateTime_add_record->setTime(QTime::currentTime());
}

bool HerpApp::submit_record()
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
            submit.date_time = QDateTime::currentSecsSinceEpoch();
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

            std::string unique_id = gkDbWrite->create_unique_id();
            if (!submit.species.species_name.empty()) {
                submit.species.species_id = gkDbWrite->create_unique_id();
            } else {
                submit.species.species_id = "";
            }

            if (!submit.identifier.identifier_str.empty()) {
                submit.identifier.name_id = gkDbWrite->create_unique_id();
            } else {
                submit.identifier.name_id = "";
            }

            if (!unique_id.empty() && !submit.species.species_id.empty() && !submit.identifier.name_id.empty()) {
                gkDbWrite->add_record_id(unique_id, submit.species, submit.identifier);
            } else {
                throw std::invalid_argument(tr("Invalid ID provided!").toStdString());
            }

            gkDbWrite->add_item_db(unique_id, dateTime, std::to_string(submit.date_time));
            gkDbWrite->add_item_db(unique_id, speciesId, submit.species.species_id);
            gkDbWrite->add_item_db(unique_id, speciesName, submit.species.species_name);
            gkDbWrite->add_item_db(unique_id, nameId, submit.identifier.name_id);
            gkDbWrite->add_item_db(unique_id, identifierStr, submit.identifier.identifier_str);
            gkDbWrite->add_item_db(unique_id, furtherNotes, submit.further_notes);
            gkDbWrite->add_item_db(unique_id, vitaminNotes, submit.vitamin_notes);
            gkDbWrite->add_item_db(unique_id, toiletNotes, submit.toilet_notes);
            gkDbWrite->add_item_db(unique_id, weightNotes, submit.weight_notes);
            gkDbWrite->add_item_db(unique_id, hydrationNotes, submit.hydration_notes);
            gkDbWrite->add_item_db(unique_id, boolWentToilet, std::to_string(submit.went_toilet));
            gkDbWrite->add_item_db(unique_id, boolHadHydration, std::to_string(submit.had_hydration));
            gkDbWrite->add_item_db(unique_id, boolHadVitamins, std::to_string(submit.had_vitamins));
            gkDbWrite->add_item_db(unique_id, weightMeasure, std::to_string(submit.weight));

            // Reset all the input fields
            ui->dateTime_add_record->setDate(QDate::currentDate());
            ui->dateTime_add_record->setTime(QTime::currentTime());
            ui->lineEdit_new_species->clear();
            ui->lineEdit_new_id->clear();
            ui->lineEdit_toilet_notes->clear();
            ui->lineEdit_hydration_notes->clear();
            ui->lineEdit_vitamins_notes->clear();
            ui->lineEdit_weight_notes->clear();
            ui->checkBox_toilet->setChecked(false);
            ui->checkBox_hydration->setChecked(false);
            ui->checkBox_vitamins->setChecked(false);
            ui->spinBox_weight->setValue(0.000);
            ui->plainTextEdit_furtherNotes->clear();

            return true;
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return false;
    }

    return false;
}
