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
#include "gk_about_dialog.hpp"
#include <boost/exception/all.hpp>
#include <boost/lexical_cast.hpp>
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

    caches_enabled = false;
    db_ptr = database;
    global_db_temp_dir = temp_dir_path; // The (base) temporary directory where the database has been extracted to
    global_db_file_path = db_file_path; // The file-path to the (currently opened/newly created) database
    gkFileIo = file_io_ptr;

    gkStrOp = std::make_shared<GkStringOp>(this);
    gkDbRead = std::make_shared<GkDbRead>(db_ptr, gkStrOp, this);
    gkDbWrite = std::make_unique<GkDbWrite>(db_ptr, gkDbRead, gkStrOp, this);

    ui->interface_tabWidget->setCurrentIndex(0);
    ui->interface_tabWidget->setTabEnabled(2, false);
    ui->interface_tabWidget->setTabEnabled(3, false);

    refresh_caches();
    ui->lineEdit_new_id->setText(QString::fromStdString(gkStrOp->random_hash()));
    ui->dateTime_add_record->setMinimumDateTime(QDateTime::fromTime_t(1));
    ui->dateTime_add_record->setDate(QDate::currentDate());
    ui->dateTime_add_record->setTime(QTime::currentTime());

    charts_tab_enabled = false;
    find_date_ranges();
    insert_charts();
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
    try {
        sys::error_code ec;
        if (fs::is_directory(tmpDirLoc, ec)) {
            if (!fs::remove_all(tmpDirLoc, ec)) {
                QMessageBox::warning(this, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
                return false;
            } else {
                return true;
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return false;
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
    try {
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
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }


    return;
}

void HerpApp::on_actionSave_As_triggered()
{
    try {
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
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }

    return;
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
{
    QPointer<GkAboutDialog> aboutDialog = new GkAboutDialog(this);
    aboutDialog->setWindowFlags(Qt::Window);
    aboutDialog->setAttribute(Qt::WA_DeleteOnClose, true); // Delete itself on closing
    QObject::connect(aboutDialog, SIGNAL(destroyed(QObject*)), this, SLOT(show()));
    aboutDialog->show();
    return;
}

void HerpApp::on_pushButton_archive_next_clicked()
{
    std::string next_record = browse_records(archive_records, true);
    archive_fill_form_data(next_record);
}

void HerpApp::on_pushButton_archive_prev_clicked()
{
    std::string prev_record = browse_records(archive_records, false);
    archive_fill_form_data(prev_record);
}

void HerpApp::on_pushButton_archive_delete_clicked()
{}

void HerpApp::on_pushButton_browse_submit_clicked()
{
    try {
        int dateTimeStart = ui->dateTimeEdit_browse_start->dateTime().toTime_t();
        int dateTimeEnd = ui->dateTimeEdit_browse_end->dateTime().toTime_t();

        archive_records.clear();
        if (archive_records.empty()) {
            archive_records = gkDbRead->extractRecords(dateTimeStart, dateTimeEnd);
        } else {
            throw std::runtime_error(tr("There was an error in gathering the data. Please exit the program and try again!").toStdString());
        }

        if (!archive_records.empty()) {
            ui->interface_tabWidget->setTabEnabled(2, true);
            ui->interface_tabWidget->setCurrentIndex(2);

            viewed_records.clear();
            std::string next_record = browse_records(archive_records, true);
            archive_fill_form_data(next_record);
            return;
        } else {
            QMessageBox::information(this, tr("Info"), tr("There is no data to display!"), QMessageBox::Ok);
            return;
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }
}

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

void HerpApp::on_toolButton_records_calendar_popup_clicked()
{}

void HerpApp::on_comboBox_view_charts_select_licensee_currentIndexChanged(int index)
{}

void HerpApp::on_comboBox_view_charts_select_species_currentIndexChanged(int index)
{
    try {
        if (caches_enabled) {
            int counter = 0;
            auto animal_names = gkDbRead->get_misc_key_vals(GkRecords::MiscRecordType::gkId);
            ui->comboBox_existing_id->clear();

            for (auto it_sp = species_cache.begin(); it_sp != species_cache.end(); ++it_sp) {
                if (index == it_sp.value().second) {
                    auto it_an = animal_cache.find(it_sp.key()); // http://doc.qt.io/qt-5/qmultimap.html
                    while (it_an != animal_cache.end() && it_an.key() == it_sp.key()) {
                        for (auto animal = animal_names.begin(); animal != animal_names.end(); ++animal) {
                            if (it_an.value().first == animal.key()) {
                                ui->comboBox_view_charts_select_id->insertItem(counter, QString::fromStdString(animal.value()));
                                ++counter;
                                ++it_an;
                            }
                        }
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }
}

void HerpApp::on_comboBox_view_charts_select_id_currentIndexChanged(int index)
{
    // update_charts("");
    return;
}

void HerpApp::on_comboBox_existing_license_id_currentIndexChanged(int index)
{}

void HerpApp::on_comboBox_existing_species_currentIndexChanged(int index)
{
    try {
        if (caches_enabled) {
            int counter = 0;
            auto animal_names = gkDbRead->get_misc_key_vals(GkRecords::MiscRecordType::gkId);
            ui->comboBox_existing_id->clear();

            for (auto it_sp = species_cache.begin(); it_sp != species_cache.end(); ++it_sp) {
                if (index == it_sp.value().second) {
                    auto it_an = animal_cache.find(it_sp.key()); // http://doc.qt.io/qt-5/qmultimap.html
                    while (it_an != animal_cache.end() && it_an.key() == it_sp.key()) {
                        for (auto animal = animal_names.begin(); animal != animal_names.end(); ++animal) {
                            if (it_an.value().first == animal.key()) {
                                ui->comboBox_existing_id->insertItem(counter, QString::fromStdString(animal.value()));
                                ++counter;
                                ++it_an;
                            }
                        }
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }
}

void HerpApp::on_comboBox_view_records_licensee_currentIndexChanged(int index)
{}

void HerpApp::on_comboBox_view_records_species_currentIndexChanged(int index)
{}

void HerpApp::on_comboBox_view_records_animal_name_currentIndexChanged(int index)
{}

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
            submit.date_time = ui->dateTime_add_record->dateTime().toTime_t();
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

            on_toolButton_new_hash_clicked();
            find_date_ranges();

            return true;
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return false;
    }

    return false;
}

void HerpApp::refresh_caches()
{
    try {
        record_id_cache.clear();
        record_id_cache = gkDbRead->get_record_ids();

        auto species_temp_cache = gkDbRead->get_misc_key_vals(GkRecords::MiscRecordType::gkSpecies);
        if (!species_temp_cache.empty()) {
            species_cache.clear();
            int counter = 0;
            for (auto it = species_temp_cache.begin(); it != species_temp_cache.end(); ++it) {
                species_cache.insertMulti(it.key(), std::make_pair(it.value(), counter));
                ++counter;
            }
        }

        if (!record_id_cache.empty()) {
            std::vector<std::string> record_ids;
            auto animal_temp_cache = gkDbRead->get_misc_key_vals(GkRecords::MiscRecordType::gkId);
            animal_cache.clear();

            for (const auto &ids: record_id_cache) {
                if (!ids.first.empty()) {
                    record_ids.push_back(ids.first);
                }

                if (!animal_temp_cache.empty()) {
                    int counter = 0;
                    for (auto it = animal_temp_cache.begin(); it != animal_temp_cache.end(); ++it) {
                        if (it.key() == ids.second.second) {
                            animal_cache.insertMulti(ids.second.first, std::make_pair(it.key(), counter));
                            ++counter;
                        }
                    }
                }
            }

            minDateTime = gkDbRead->determineMinimumDate(record_ids);
            maxDateTime = gkDbRead->determineMaximumDate(record_ids);

            if (!species_cache.empty()) { // We need Record IDs for this operation and any successive operations from this one to work
                ui->comboBox_existing_species->clear();
                ui->comboBox_view_charts_select_species->clear();
                for (auto it = species_cache.begin(); it != species_cache.end(); ++it) {
                    ui->comboBox_existing_species->insertItem(it.value().second, QString::fromStdString(it.value().first));
                    ui->comboBox_view_charts_select_species->insertItem(it.value().second, QString::fromStdString(it.value().first));
                }
            }

            if (!caches_enabled) {
                caches_enabled = true;
            }

            return;
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }
}

void HerpApp::find_date_ranges()
{
    refresh_caches();
    if (!record_id_cache.empty()) {
        if ((minDateTime > 0) && (maxDateTime > 0)) {
            if (!ui->dateTimeEdit_browse_start->isEnabled() && !ui->dateTimeEdit_browse_end->isEnabled()) {
                ui->dateTimeEdit_browse_start->setEnabled(true);
                ui->dateTimeEdit_browse_end->setEnabled(true);
            }

            ui->dateTimeEdit_browse_start->setMinimumDateTime(QDateTime::fromTime_t(minDateTime));
            ui->dateTimeEdit_browse_start->setMaximumDateTime(QDateTime::fromTime_t(maxDateTime));
            ui->dateTimeEdit_browse_end->setMinimumDateTime(QDateTime::fromTime_t(minDateTime));
            ui->dateTimeEdit_browse_end->setMaximumDateTime(QDateTime::fromTime_t(maxDateTime));
            ui->dateTimeEdit_browse_end->setDateTime(QDateTime::fromTime_t(maxDateTime));
            return;
        }
    } else {
        ui->dateTimeEdit_browse_start->setEnabled(false);
        ui->dateTimeEdit_browse_end->setEnabled(false);
        return;
    }

    return;
}

/**
 * @brief HerpApp::browse_records determines which unique Record ID to display next.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-02
 * @param records A cache of records that have been determined to be within the given minimum and maximum date range.
 * @param forward Whether we are progressing forward or backwards in the record-set.
 * @return The unique Record ID to be displayed.
 */
std::string HerpApp::browse_records(const std::list<std::string> &records, const bool &forward)
{
    try {
        if (!records.empty()) {
            // Records are added to `viewed_records` as the `Next Record` button is pressed, and removed as
            // the `Previous Record` button is pressed.
            for (const auto &record: records) {
                if (forward) { // `Next Record` has been pressed
                    if (std::find(viewed_records.begin(), viewed_records.end(), record) == viewed_records.end()) { // Check that the cached record does not already exist within `viewed_records`.
                        viewed_records.push_back(record);
                        return record;
                    }
                }
            }

            if (!forward) { // `Previous Record` has been pressed
                if (viewed_records.size() > 1) {
                    viewed_records.pop_back();
                    return viewed_records.back();
                } else {
                    return records.front();
                }
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return "";
    }

    return "";
}

/**
 * @brief HerpApp::find_species_types will fill out the specified comboBox(es) with the found `Species` for the
 * given `Licensee`.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-08
 * @param dropbox_type Whether to insert data to the `Species` comboBox within the `Add Record` section, `View Records`
 * area, or in the `View Charts` bit.
 * @param index_no An integer that corresponds to the given `Licensee` within the `licensee_cache` QMultiMap variable.
 * @return A list of Species IDs that correspond to the given licensee.
 */
std::list<std::string> HerpApp::find_species_types(const GkRecords::comboBoxType &dropbox_type, const int &index_no)
{
    return std::list<std::string>();
}

/**
 * @brief HerpApp::find_animal_names will fill out the specified comboBox(es) with the found `Name / ID#`'s for the
 * given species.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-08
 * @param dropbox_type Whether to insert data to the `Name / ID#` comboBox in the `Add Record` section, the
 * `View Records` area, or `View Charts` bit.
 * @param index_no An integer that corresponds to the given species within the `species_cache` QMultiMap variable.
 * @return A list of Animal IDs that correspond to the given species.
 */
std::list<std::string> HerpApp::find_animal_names(const GkRecords::comboBoxType &dropbox_type, const int &index_no)
{
    try {
        int counter = 0;
        auto animal_names = gkDbRead->get_misc_key_vals(GkRecords::MiscRecordType::gkId);
        ui->comboBox_existing_id->clear();

        for (auto it_sp = species_cache.begin(); it_sp != species_cache.end(); ++it_sp) {
            if (index_no == it_sp.value().second) {
                auto it_an = animal_cache.find(it_sp.key()); // http://doc.qt.io/qt-5/qmultimap.html
                while (it_an != animal_cache.end() && it_an.key() == it_sp.key()) {
                    for (auto animal = animal_names.begin(); animal != animal_names.end(); ++animal) {
                        if (it_an.value().first == animal.key()) {
                            ui->comboBox_existing_id->insertItem(counter, QString::fromStdString(animal.value()));
                            ++counter;
                            ++it_an;
                        }
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return std::list<std::string>();
}

void HerpApp::archive_clear_forms()
{
    ui->lineEdit_records_dateTime->clear();

    ui->lineEdit_records_toilet_notes->clear();
    ui->lineEdit_records_hydration_notes->clear();
    ui->lineEdit_records_vitamins_notes->clear();
    ui->lineEdit_records_weight_notes->clear();

    ui->checkBox_records_went_toilet->setChecked(false);
    ui->checkBox_records_had_hydration->setChecked(false);
    ui->checkBox_records_had_vitamins->setChecked(false);

    ui->doubleSpinBox_records_weight->setValue(0.000);
    return;
}

/**
 * @brief HerpApp::archive_fill_form_data will fill out all the forms on the `viewRecords` tab of `ui->interface_tabWidget` by
 * being given just the unique Record ID.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-04
 * @param record_id The unique Record ID of the data to be extracted from the database for this operation.
 */
void HerpApp::archive_fill_form_data(const std::string &record_id)
{
    try {
        if (!record_id.empty()) {
            GkRecords::GkSubmit submit_data;
            for (const auto &cached_record: record_id_cache) {
                if (cached_record.first == record_id) {
                    submit_data.record_id = record_id;
                    submit_data.species.species_id = cached_record.second.first;
                    submit_data.identifier.name_id = cached_record.second.second;
                    break;
                }
            }

            auto species_data = gkDbRead->get_misc_key_vals(GkRecords::MiscRecordType::gkSpecies);
            auto ident_data = gkDbRead->get_misc_key_vals(GkRecords::MiscRecordType::gkId);

            for (auto it = species_data.begin(); it != species_data.end(); ++it) {
                if (it.key() == submit_data.species.species_id) {
                    submit_data.species.species_name = it.value();
                    break;
                }
            }

            for (auto it = ident_data.begin(); it != ident_data.end(); ++it) {
                if (it.key() == submit_data.identifier.name_id) {
                    submit_data.identifier.identifier_str = it.value();
                    break;
                }
            }

            submit_data.date_time = std::stoi(gkDbRead->read_item_db(submit_data.record_id, GkRecords::dateTime));
            submit_data.further_notes = gkDbRead->read_item_db(submit_data.record_id, GkRecords::furtherNotes);
            submit_data.vitamin_notes = gkDbRead->read_item_db(submit_data.record_id, GkRecords::vitaminNotes);
            submit_data.toilet_notes = gkDbRead->read_item_db(submit_data.record_id, GkRecords::toiletNotes);
            submit_data.weight_notes = gkDbRead->read_item_db(submit_data.record_id, GkRecords::weightNotes);
            submit_data.hydration_notes = gkDbRead->read_item_db(submit_data.record_id, GkRecords::hydrationNotes);
            submit_data.went_toilet = boost::lexical_cast<bool>(gkDbRead->read_item_db(submit_data.record_id, GkRecords::boolWentToilet));
            submit_data.had_hydration = boost::lexical_cast<bool>(gkDbRead->read_item_db(submit_data.record_id, GkRecords::boolHadHydration));
            submit_data.had_vitamins = boost::lexical_cast<bool>(gkDbRead->read_item_db(submit_data.record_id, GkRecords::boolHadVitamins));
            submit_data.weight = std::stod(gkDbRead->read_item_db(submit_data.record_id, GkRecords::weightMeasure));

            if ((submit_data.date_time > 0) && (!submit_data.species.species_name.empty()) && (!submit_data.identifier.identifier_str.empty())) {
                archive_clear_forms();

                QDateTime qdt;
                qdt.setTime_t(submit_data.date_time);
                ui->lineEdit_records_dateTime->setText(qdt.toString(tr("dd/MM/yyyy hh:mm:ss AP")));

                if (!submit_data.toilet_notes.empty()) {
                    ui->lineEdit_records_toilet_notes->setText(QString::fromStdString(submit_data.toilet_notes));
                }

                if (!submit_data.hydration_notes.empty()) {
                    ui->lineEdit_records_hydration_notes->setText(QString::fromStdString(submit_data.hydration_notes));
                }

                if (!submit_data.vitamin_notes.empty()) {
                    ui->lineEdit_records_vitamins_notes->setText(QString::fromStdString(submit_data.vitamin_notes));
                }

                if (!submit_data.weight_notes.empty()) {
                    ui->lineEdit_records_weight_notes->setText(QString::fromStdString(submit_data.weight_notes));
                }

                ui->checkBox_records_went_toilet->setChecked(submit_data.went_toilet);
                ui->checkBox_records_had_hydration->setChecked(submit_data.had_hydration);
                ui->checkBox_records_had_vitamins->setChecked(submit_data.had_vitamins);

                ui->doubleSpinBox_records_weight->setValue(submit_data.weight);
                return;
            } else {
                throw std::runtime_error(tr("Was unable to retrieve information from the database!").toStdString());
            }
        } else {
            return;
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }
}

/**
 * @brief HerpApp::insert_charts will insert the QChart widgets into the GUI, in their respective places.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-04
 */
void HerpApp::insert_charts()
{
    chart_weight = new QChart();
    chart_weight->legend()->show();
    // chart_weight->addSeries(line_series_weight);
    chart_weight->createDefaultAxes();
    chart_weight->setTitle(tr("Weight vs. Time"));
    chart_weight->setAnimationOptions(QChart::AllAnimations);

    QPointer<QChartView> chart_view_weight = new QChartView(chart_weight);
    chart_view_weight->setRenderHint(QPainter::Antialiasing);

    ui->vertLayout_chart_1->addWidget(chart_view_weight);
}

/**
 * @brief HerpApp::update_charts refreshes the QCharts with any new and previously available data.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-04
 */
void HerpApp::update_charts(const std::string &name_id)
{
    try {
        line_series_weight = new QLineSeries();
        refresh_caches();
        if (!record_id_cache.empty()) {
            auto dated_record_ids = gkDbRead->extractRecords(minDateTime, maxDateTime);
            auto ident_record_cache = gkDbRead->get_misc_key_vals(GkRecords::MiscRecordType::gkId);
            long int date_time;
            double weight_in_loop;
            GkRecords::GkSpecies species_struct;
            GkRecords::GkId ident_struct;

            if ((!dated_record_ids.empty()) && (!species_cache.empty()) && (!ident_record_cache.empty())) {
                for (const auto &dated_id: dated_record_ids) {
                    date_time = std::stol(gkDbRead->read_item_db(dated_id, GkRecords::dateTime));
                    if (!weight_measurements.contains(date_time)) { // Check that the key does not already exist in the cache!
                        for (const auto &cached_id: record_id_cache) {
                            if (dated_id == cached_id.first) {
                                weight_in_loop = std::stod(gkDbRead->read_item_db(dated_id, GkRecords::weightMeasure));
                                species_struct.species_id = cached_id.second.first;
                                ident_struct.name_id = cached_id.second.second;

                                for (auto it = species_cache.begin(); it != species_cache.end(); ++it) {
                                    if (species_struct.species_id == it.key()) {
                                        species_struct.species_name = it.value().first;
                                    }
                                }

                                for (auto it = ident_record_cache.begin(); it != ident_record_cache.end(); ++it) {
                                    if (ident_struct.name_id == it.key()) {
                                        ident_struct.identifier_str = it.value();
                                    }
                                }

                                GkRecords::GkGraph::WeightVsTime weight_struct;
                                weight_struct.record_id = dated_id;
                                weight_struct.species = species_struct;
                                weight_struct.identifier = ident_struct;
                                weight_struct.weight = weight_in_loop;

                                weight_measurements.insertMulti(date_time, weight_struct);

                                break;
                            }
                        }
                    }
                }

                if (dated_record_ids.size() > 1) { // Therefore there are two plot points for the graph(s)
                    if (!charts_tab_enabled) {
                        ui->interface_tabWidget->setTabEnabled(3, true);
                        charts_tab_enabled = true;
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }

    return;
}
