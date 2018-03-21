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
#include <vector>
#include <future>

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
    set_date_ranges();
    update_charts();
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
    // Go to `Next Record`
    std::string next_record = browse_records(archive_records, true);
    archive_curr_sel_record = next_record;
    archive_fill_form_data(next_record);
    return;
}

void HerpApp::on_pushButton_archive_prev_clicked()
{
    // Go to `Previous Record`
    std::string prev_record = browse_records(archive_records, false);
    archive_curr_sel_record = prev_record;
    archive_fill_form_data(prev_record);
    return;
}

void HerpApp::on_pushButton_archive_delete_clicked()
{
    // `Delete Record`
    if (!archive_curr_sel_record.empty()) {
        delete_record(archive_curr_sel_record);
        archive_curr_sel_record.clear();
    }

    return;
}

void HerpApp::on_pushButton_browse_submit_clicked()
{
    try {
        int dateTimeStart = ui->dateTimeEdit_browse_start->dateTime().toTime_t();
        int dateTimeEnd = ui->dateTimeEdit_browse_end->dateTime().toTime_t();

        archive_records.clear();
        if (archive_records.empty()) {
            archive_records = gkDbRead->extract_records(dateTimeStart, dateTimeEnd);
        } else {
            throw std::runtime_error(tr("There was an error in gathering the data. Please exit the program and try again!").toStdString());
        }

        if (!archive_records.empty()) {
            ui->interface_tabWidget->setTabEnabled(2, true);
            ui->interface_tabWidget->setCurrentIndex(2);

            viewed_records.clear();
            std::string next_record = browse_records(archive_records, true);
            archive_curr_sel_record = next_record;
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
{
    if (!licensee_cache.empty()) {
        std::string license_id = find_comboBox_id(GkRecords::MiscRecordType::gkLicensee, GkRecords::comboBoxType::ViewCharts, index);
        auto species_index = find_species_names(GkRecords::comboBoxType::ViewCharts, license_id);
        record_species_index(species_index, GkRecords::comboBoxType::ViewCharts);
        comboBox_view_graphs_licensee_sel = index;
        emit on_comboBox_view_charts_select_species_currentIndexChanged(index);
    }
}

void HerpApp::on_comboBox_view_charts_select_species_currentIndexChanged(int index)
{
    if (!comboBox_species.empty()) {
        comboBox_view_graphs_species_sel = index;
        for (const auto &species: comboBox_species) {
            if (species.comboBox.comboBox_type == GkRecords::ViewCharts) {
                if (species.comboBox.index_no == index) {
                    auto animals_index = find_animal_names(GkRecords::comboBoxType::ViewCharts, species.species_id);
                    record_animals_index(animals_index, GkRecords::comboBoxType::ViewCharts);
                    break;
                }
            }
        }
    }
}

void HerpApp::on_comboBox_view_charts_select_id_currentIndexChanged(int index)
{
    if (!comboBox_animals.empty()) {
        comboBox_view_graphs_animals_sel = index;
    }
}

void HerpApp::on_comboBox_existing_license_id_currentIndexChanged(int index)
{
    if (!licensee_cache.empty()) {
        std::string license_id = find_comboBox_id(GkRecords::MiscRecordType::gkLicensee, GkRecords::comboBoxType::AddRecord, index);
        auto species_index = find_species_names(GkRecords::comboBoxType::AddRecord, license_id);
        record_species_index(species_index, GkRecords::comboBoxType::AddRecord);
        comboBox_add_records_licensee_sel = index;
        emit on_comboBox_existing_species_currentIndexChanged(index);
    }
}

void HerpApp::on_comboBox_existing_species_currentIndexChanged(int index)
{
    if (!comboBox_species.empty()) {
        comboBox_add_records_species_sel = index;
        for (const auto &species: comboBox_species) {
            if (species.comboBox.comboBox_type == GkRecords::AddRecord) {
                if (species.comboBox.index_no == index) {
                    auto animals_index = find_animal_names(GkRecords::comboBoxType::AddRecord, species.species_id);
                    record_animals_index(animals_index, GkRecords::comboBoxType::AddRecord);
                    break;
                }
            }
        }
    }
}

void HerpApp::on_comboBox_existing_id_currentIndexChanged(int index)
{
    if (!comboBox_animals.empty()) {
        comboBox_add_records_animals_sel = index;
    }
}

void HerpApp::on_comboBox_view_records_licensee_currentIndexChanged(int index)
{
    if (!licensee_cache.empty()) {
        std::string license_id = find_comboBox_id(GkRecords::MiscRecordType::gkLicensee, GkRecords::comboBoxType::ViewRecords, index);
        auto species_index = find_species_names(GkRecords::comboBoxType::ViewRecords, license_id);
        record_species_index(species_index, GkRecords::comboBoxType::ViewRecords);
        emit on_comboBox_view_records_species_currentIndexChanged(index);
    }
}

void HerpApp::on_comboBox_view_records_species_currentIndexChanged(int index)
{
    if (!comboBox_species.empty()) {
        for (const auto &species: comboBox_species) {
            if (species.comboBox.comboBox_type == GkRecords::ViewRecords) {
                if (species.comboBox.index_no == index) {
                    auto animals_index = find_animal_names(GkRecords::comboBoxType::ViewRecords, species.species_id);
                    record_animals_index(animals_index, GkRecords::comboBoxType::ViewRecords);
                    break;
                }
            }
        }
    }
}

void HerpApp::on_comboBox_view_records_animal_name_currentIndexChanged(int index)
{
    if (!comboBox_animals.empty()) {
        comboBox_view_records_animals_sel = index;
    }
}

void HerpApp::on_toolButton_view_records_licensee_clicked()
{
    // Delete `Licensee` entry
    if (!licensee_cache.empty()) {
        int curr_sel = ui->comboBox_view_records_licensee->currentIndex();
        std::string licensee_id;
        for (auto it = licensee_cache.begin(); it != licensee_cache.end(); ++it) {
            if (it.value().second == curr_sel) {
                licensee_id = it.key();
                break;
            }
        }

        delete_category_id(GkRecords::MiscRecordType::gkLicensee, licensee_id);
    }
}

void HerpApp::on_toolButton_view_records_species_clicked()
{
    // Delete `Species` entry
    if (!comboBox_species.empty()) {
        int curr_sel = ui->comboBox_view_records_species->currentIndex();
        std::string species_id;
        for (auto it = comboBox_species.begin(); it != comboBox_species.end(); ++it) {
            if (it.value().comboBox.comboBox_type == GkRecords::comboBoxType::ViewRecords) {
                if (it.value().comboBox.index_no == curr_sel) {
                    species_id = it.value().species_id;
                    break;
                }
            }
        }

        delete_category_id(GkRecords::MiscRecordType::gkSpecies, species_id);
    }
}

void HerpApp::on_toolButton_view_records_animal_clicked()
{
    // Delete `Animals` entry
    if (!comboBox_animals.empty()) {
        int curr_sel = ui->comboBox_view_records_animal_name->currentIndex();
        std::string animal_id;
        for (auto it = comboBox_animals.begin(); it != comboBox_animals.end(); ++it) {
            if (it.value().comboBox.comboBox_type == GkRecords::comboBoxType::ViewRecords) {
                if (it.value().comboBox.index_no == curr_sel) {
                    animal_id = it.value().name_id;
                    break;
                }
            }
        }

        delete_category_id(GkRecords::MiscRecordType::gkId, animal_id);
    }
}

bool HerpApp::submit_record()
{
    try {
        if (!ui->comboBox_existing_license_id->currentText().isEmpty() || !ui->lineEdit_new_license_id->text().isEmpty()) {
            if (!ui->comboBox_existing_species->currentText().isEmpty() || !ui->lineEdit_new_species->text().isEmpty()) {
                if (!ui->comboBox_existing_id->currentText().isEmpty() || !ui->lineEdit_new_id->text().isEmpty()) {
                    using namespace GkRecords;

                    GkSubmit submit;
                    std::string unique_id = gkDbWrite->create_uuid();
                    if (!ui->lineEdit_new_license_id->text().isEmpty()) {
                        submit.licensee.licensee_name = ui->lineEdit_new_license_id->text().toStdString();
                        submit.licensee.licensee_id = gkDbWrite->create_uuid();
                    } else {
                        if (!ui->comboBox_existing_license_id->currentText().isEmpty()) {
                            submit.licensee.licensee_name = ui->comboBox_existing_license_id->currentText().toStdString();
                            submit.licensee.licensee_id = find_comboBox_id(GkRecords::MiscRecordType::gkLicensee,
                                                                           GkRecords::comboBoxType::AddRecord,
                                                                           comboBox_add_records_licensee_sel);
                        } else {
                            throw std::invalid_argument(tr("Invalid information provided by QComboBox!").toStdString());
                        }
                    }

                    if (!ui->lineEdit_new_species->text().isEmpty()) {
                        submit.species.species_name = ui->lineEdit_new_species->text().toStdString();
                        submit.species.species_id = gkDbWrite->create_uuid();
                    } else {
                        if (!ui->comboBox_existing_species->currentText().isEmpty()) {
                            submit.species.species_name = ui->comboBox_existing_species->currentText().toStdString();
                            submit.species.species_id = find_comboBox_id(GkRecords::MiscRecordType::gkSpecies,
                                                                         GkRecords::comboBoxType::AddRecord,
                                                                         comboBox_add_records_species_sel);;
                        } else {
                            throw std::invalid_argument(tr("Invalid information provided by QComboBox!").toStdString());
                        }
                    }

                    if (!ui->lineEdit_new_id->text().isEmpty()) {
                        submit.identifier.identifier_str = ui->lineEdit_new_id->text().toStdString();
                        submit.identifier.name_id = gkDbWrite->create_uuid();
                    } else {
                        if (!ui->comboBox_existing_id->currentText().isEmpty()) {
                            submit.identifier.identifier_str = ui->comboBox_existing_id->currentText().toStdString();
                            submit.identifier.name_id = find_comboBox_id(GkRecords::MiscRecordType::gkId,
                                                                         GkRecords::comboBoxType::AddRecord,
                                                                         comboBox_add_records_animals_sel);;
                        } else {
                            throw std::invalid_argument(tr("Invalid information provided by QComboBox!").toStdString());
                        }
                    }

                    submit.date_time = ui->dateTime_add_record->dateTime().toTime_t();
                    submit.further_notes = ui->plainTextEdit_furtherNotes->toPlainText().toStdString();
                    submit.vitamin_notes = ui->lineEdit_vitamins_notes->text().toStdString();
                    submit.toilet_notes = ui->lineEdit_toilet_notes->text().toStdString();
                    submit.temp_notes = ui->lineEdit_temperature_notes->text().toStdString();
                    submit.weight_notes = ui->lineEdit_weight_notes->text().toStdString();
                    submit.hydration_notes = ui->lineEdit_hydration_notes->text().toStdString();
                    submit.went_toilet = ui->checkBox_toilet->isChecked();
                    submit.had_hydration = ui->checkBox_hydration->isChecked();
                    submit.had_vitamins = ui->checkBox_vitamins->isChecked();
                    submit.weight = ui->spinBox_weight->value();

                    if ((!unique_id.empty()) && (!submit.licensee.licensee_id.empty()) && (!submit.species.species_id.empty()) &&
                            (!submit.identifier.name_id.empty())) {
                        gkDbWrite->add_uuid(unique_id, submit.licensee, submit.species, submit.identifier);
                        gkDbWrite->add_item_db(unique_id, dateTime, std::to_string(submit.date_time));
                        gkDbWrite->add_item_db(unique_id, furtherNotes, submit.further_notes);
                        gkDbWrite->add_item_db(unique_id, vitaminNotes, submit.vitamin_notes);
                        gkDbWrite->add_item_db(unique_id, toiletNotes, submit.toilet_notes);
                        gkDbWrite->add_item_db(unique_id, tempNotes, submit.temp_notes);
                        gkDbWrite->add_item_db(unique_id, weightNotes, submit.weight_notes);
                        gkDbWrite->add_item_db(unique_id, hydrationNotes, submit.hydration_notes);
                        gkDbWrite->add_item_db(unique_id, boolWentToilet, std::to_string(submit.went_toilet));
                        gkDbWrite->add_item_db(unique_id, boolHadHydration, std::to_string(submit.had_hydration));
                        gkDbWrite->add_item_db(unique_id, boolHadVitamins, std::to_string(submit.had_vitamins));
                        gkDbWrite->add_item_db(unique_id, weightMeasure, std::to_string(submit.weight));

                        // Reset all the input fields
                        ui->dateTime_add_record->setDate(QDate::currentDate());
                        ui->dateTime_add_record->setTime(QTime::currentTime());
                        ui->lineEdit_new_license_id->clear();
                        ui->lineEdit_new_species->clear();
                        ui->lineEdit_new_id->clear();
                        ui->lineEdit_toilet_notes->clear();
                        ui->lineEdit_hydration_notes->clear();
                        ui->lineEdit_vitamins_notes->clear();
                        ui->lineEdit_temperature_notes->clear();
                        ui->lineEdit_weight_notes->clear();
                        ui->checkBox_toilet->setChecked(false);
                        ui->checkBox_hydration->setChecked(false);
                        ui->checkBox_vitamins->setChecked(false);
                        ui->spinBox_weight->setValue(0.000);
                        ui->plainTextEdit_furtherNotes->clear();

                        ui->comboBox_existing_license_id->setCurrentIndex(0);
                        ui->comboBox_existing_species->setCurrentIndex(0);
                        ui->comboBox_existing_id->setCurrentIndex(0);
                        ui->comboBox_view_records_licensee->setCurrentIndex(0);
                        ui->comboBox_view_records_species->setCurrentIndex(0);
                        ui->comboBox_view_records_animal_name->setCurrentIndex(0);
                        ui->comboBox_view_charts_select_licensee->setCurrentIndex(0);
                        ui->comboBox_view_charts_select_species->setCurrentIndex(0);
                        ui->comboBox_view_charts_select_id->setCurrentIndex(0);

                        emit on_comboBox_existing_license_id_currentIndexChanged(0);
                        emit on_comboBox_existing_species_currentIndexChanged(0);
                        emit on_comboBox_existing_id_currentIndexChanged(0);
                        emit on_comboBox_view_records_licensee_currentIndexChanged(0);
                        emit on_comboBox_view_records_species_currentIndexChanged(0);
                        emit on_comboBox_view_records_animal_name_currentIndexChanged(0);
                        emit on_comboBox_view_charts_select_licensee_currentIndexChanged(0);
                        emit on_comboBox_view_charts_select_species_currentIndexChanged(0);
                        emit on_comboBox_view_charts_select_id_currentIndexChanged(0);

                        on_toolButton_new_hash_clicked();
                        set_date_ranges();
                        update_charts();

                        return true;
                    }
                } else {
                    QMessageBox::information(this, tr("Missing data!"), tr("You need to fill-in/select an animal name or ID!"), QMessageBox::Ok);
                }
            } else {
                QMessageBox::information(this, tr("Missing data!"), tr("You need to fill-in/select an animal species!"), QMessageBox::Ok);
            }
        } else {
            QMessageBox::information(this, tr("Missing data!"), tr("You need to fill-in/select a licensee!"), QMessageBox::Ok);
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return false;
    }

    return false;
}

/**
 * @brief HerpApp::delete_record Will delete a given database entry and all of its related data from the Google LevelDB
 * database when given a specified unique Record ID.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-13
 * @param record_id The unique Record ID of all the database entries that must be deleted.
 * @return Whether the operation was a success or not.
 */
bool HerpApp::delete_record(const std::string &record_id)
{
    try {
        if ((!record_id.empty()) && (!unique_id_map.empty())) {
            QMessageBox msgBox;
            msgBox.setText(tr("Delete Record..."));
            msgBox.setInformativeText(tr("Are you really sure about deleting this record?"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();

            switch (ret) {
                case QMessageBox::Yes:
                    // Delete the record
                {
                    using namespace GkRecords;
                    gkDbWrite->del_item_db(record_id, dateTime);
                    gkDbWrite->del_item_db(record_id, furtherNotes);
                    gkDbWrite->del_item_db(record_id, vitaminNotes);
                    gkDbWrite->del_item_db(record_id, toiletNotes);
                    gkDbWrite->del_item_db(record_id, tempNotes);
                    gkDbWrite->del_item_db(record_id, weightNotes);
                    gkDbWrite->del_item_db(record_id, hydrationNotes);
                    gkDbWrite->del_item_db(record_id, boolWentToilet);
                    gkDbWrite->del_item_db(record_id, boolHadHydration);
                    gkDbWrite->del_item_db(record_id, boolHadVitamins);
                    gkDbWrite->del_item_db(record_id, weightMeasure);
                    gkDbWrite->del_uuid(record_id);

                    set_date_ranges();
                    update_charts();
                    archive_clear_forms();
                    std::string prev_record = browse_records(archive_records, false);
                    archive_curr_sel_record = prev_record;
                    archive_fill_form_data(prev_record);
                }
                    return true;
                case QMessageBox::No:
                    // Do not delete the record
                    return true;
                case QMessageBox::Cancel:
                    // Do not delete the record
                    return true;
                default:
                    // Should never be reached
                    return true;
            }
        } else {
            throw std::invalid_argument(tr("Unable to read Record ID!").toStdString());
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return false;
}

void HerpApp::refresh_caches()
{
    try {
        std::lock_guard<std::mutex> locker(r_cache_mtx);

        unique_id_map.clear();
        unique_id_map = gkDbRead->get_uuids();

        auto licensee_temp_cache = gkDbRead->get_cat_key_vals(GkRecords::MiscRecordType::gkLicensee);
        if (!licensee_temp_cache.empty()) {
            licensee_cache.clear();
            ui->comboBox_existing_license_id->clear();
            ui->comboBox_view_records_licensee->clear();
            ui->comboBox_view_charts_select_licensee->clear();
            int counter = 0;
            for (auto it = licensee_temp_cache.begin(); it != licensee_temp_cache.end(); ++it) {
                licensee_cache.insertMulti(it.key(), std::make_pair(it.value(), counter));
                ui->comboBox_existing_license_id->insertItem(counter, QString::fromStdString(it.value()));
                ui->comboBox_view_records_licensee->insertItem(counter, QString::fromStdString(it.value()));
                ui->comboBox_view_charts_select_licensee->insertItem(counter, QString::fromStdString(it.value()));
                ++counter;
            }
        }

        if (!unique_id_map.empty()) {
            std::vector<std::string> record_ids;
            auto animal_temp_cache = gkDbRead->get_cat_key_vals(GkRecords::MiscRecordType::gkId);
            animal_cache.clear();

            for (const auto &ids: unique_id_map) {
                if (!ids.first.empty()) {
                    record_ids.push_back(ids.first);
                }

                if (!species_cache.contains(ids.second.licensee_id, ids.second.species_id)) {
                    auto species_temp_cache = gkDbRead->get_cat_key_vals(GkRecords::MiscRecordType::gkSpecies);
                    if (!species_temp_cache.empty()) {
                        for (auto it = species_temp_cache.begin(); it != species_temp_cache.end(); ++it) {
                            if (it.key() == ids.second.species_id) {
                                species_cache.insertMulti(ids.second.licensee_id, ids.second.species_id);
                                break;
                            }
                        }
                    }
                }

                if (!animal_cache.contains(ids.second.species_id, ids.second.name_id)) {
                    if (!animal_temp_cache.empty()) {
                        for (auto it = animal_temp_cache.begin(); it != animal_temp_cache.end(); ++it) {
                            if (it.key() == ids.second.name_id) {
                                animal_cache.insertMulti(ids.second.species_id, ids.second.name_id);
                                break;
                            }
                        }
                    }
                }
            }

            if (!record_ids.empty()) {
                minDateTime = gkDbRead->determine_min_date_time(record_ids);
                maxDateTime = gkDbRead->determine_max_date_time(record_ids);
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

/**
 * @brief HerpApp::find_date_ranges will find the most minimum/maximum possible date ranges for existing records between
 * the given `min_date_time` and `max_date_time` parameters.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-18
 * @param min_date_time The minimum applicable date and time.
 * @param max_date_time The maximum applicable date and time.
 * @return The found most <minimum/maximum> possible dates as dependent on what records exist between the values given
 * for the two parameters.
 */
std::pair<long int, long int> HerpApp::find_date_ranges(const long &min_date_time, const long &max_date_time)
{
    return std::make_pair(0, 0);
}

/**
 * @brief HerpApp::set_date_ranges will assign the most minimum/maximum possible date ranges to the UI
 * `dateTimeEdit_browse_start`, `dateTimeEdit_browse_end` QComboBoxes.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03
 */
void HerpApp::set_date_ranges()
{
    refresh_caches();
    if (!unique_id_map.empty()) {
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
        std::lock_guard<std::mutex> locker(r_browse_mtx);
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
    }

    return "";
}

/**
 * @brief HerpApp::find_comboBox_id Will find the Unique ID relating to the Licensee, Species, or Animal value for the
 * given index number.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-12
 * @param record_type Whether we are dealing with Licensee, Species, or Animal values.
 * @param comboBox_type Whether the QComboBoxes we are dealing with in question are located in the AddRecords, ViewRecords,
 * or ViewCharts tab.
 * @param index_no An integer relating to where the specific entry within the QMultiMap lies, and thus its position in
 * the displayed QComboBox(es).
 * @return If found, the discovered Unique ID relating to the Licensee, Species, or Animal value for the given index number.
 */
std::string HerpApp::find_comboBox_id(const GkRecords::MiscRecordType &record_type, const GkRecords::comboBoxType &comboBox_type,
                                      const int &index_no)
{
    try {
        if (record_type == GkRecords::MiscRecordType::gkSpecies) {
            auto i = comboBox_species.find(comboBox_type);
            while (i != comboBox_species.end() && i.key() == comboBox_type) {
                if (index_no == i.value().comboBox.index_no) {
                    return i.value().species_id;
                }
            }
        } else if (record_type == GkRecords::MiscRecordType::gkId) {
            auto i = comboBox_animals.find(comboBox_type);
            while (i != comboBox_animals.end() && i.key() == comboBox_type) {
                if (index_no == i.value().comboBox.index_no) {
                    return i.value().name_id;
                }
            }
        } else {
            for (auto it = licensee_cache.begin(); it != licensee_cache.end(); ++it) {
                if (it.value().second == index_no) {
                    return it.key();
                }
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return "";
}

/**
 * @brief HerpApp::find_species_names will fill out the specified comboBox(es) with the found `Species` for the
 * given `Licensee`.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-09
 * @param dropbox_type Whether to insert data to the `Species` comboBox within the `Add Record` section, `View Records`
 * area, or in the `View Charts` bit.
 * @param licensee_id The unique Licensee ID that is attached to the Species Names in question.
 * @return A list of Species IDs that correspond to the given licensee.
 */
std::list<GkRecords::GkSpecies> HerpApp::find_species_names(const GkRecords::comboBoxType &dropbox_type, const std::string &licensee_id)
{
    try {
        switch (dropbox_type) {
            case GkRecords::comboBoxType::AddRecord:
                ui->comboBox_existing_species->clear();
                break;
            case GkRecords::comboBoxType::ViewRecords:
                ui->comboBox_view_records_species->clear();
                break;
            case GkRecords::comboBoxType::ViewCharts:
                ui->comboBox_view_charts_select_species->clear();
                break;
            default:
                throw std::runtime_error(tr("An error occurred whilst filling a comboBox with info from the database!").toStdString());
        }

        auto tmp_species_db = gkDbRead->get_cat_key_vals(GkRecords::MiscRecordType::gkSpecies);
        std::list<GkRecords::GkSpecies> output; // A list of Species Data that correspond to the given licensee.
        int counter = 0;
        if ((!licensee_id.empty()) && (!species_cache.empty())) {
            for (auto it_ca = species_cache.begin(); it_ca != species_cache.end(); ++it_ca) {
                if (it_ca.key() == licensee_id) {
                    std::string species_id = it_ca.value();
                    GkRecords::GkSpecies species;
                    species.species_id = species_id;

                    for (auto it_tmp = tmp_species_db.begin(); it_tmp != tmp_species_db.end(); ++it_tmp) {
                        if (it_tmp.key() == species_id) {
                            switch (dropbox_type) {
                                case GkRecords::comboBoxType::AddRecord:
                                    ui->comboBox_existing_species->insertItem(counter, QString::fromStdString(it_tmp.value()));
                                    species.species_name = it_tmp.value();
                                    species.comboBox.index_no = counter;
                                    species.comboBox.comboBox_type = GkRecords::comboBoxType::AddRecord;
                                    output.push_back(species);
                                    ++counter;
                                    break;
                                case GkRecords::comboBoxType::ViewRecords:
                                    ui->comboBox_view_records_species->insertItem(counter, QString::fromStdString(it_tmp.value()));
                                    species.species_name = it_tmp.value();
                                    species.comboBox.index_no = counter;
                                    species.comboBox.comboBox_type = GkRecords::comboBoxType::ViewRecords;
                                    output.push_back(species);
                                    ++counter;
                                    break;
                                case GkRecords::comboBoxType::ViewCharts:
                                    ui->comboBox_view_charts_select_species->insertItem(counter, QString::fromStdString(it_tmp.value()));
                                    species.species_name = it_tmp.value();
                                    species.comboBox.index_no = counter;
                                    species.comboBox.comboBox_type = GkRecords::comboBoxType::ViewCharts;
                                    output.push_back(species);
                                    ++counter;
                                    break;
                            }

                            break;
                        }
                    }
                }
            }

            return output;
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return std::list<GkRecords::GkSpecies>();
}

/**
 * @brief HerpApp::find_animal_names will fill out the specified comboBox(es) with the found `Name / ID#`'s for the
 * given species.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-09
 * @param dropbox_type Whether to insert data to the `Name / ID#` comboBox in the `Add Record` section, the
 * `View Records` area, or `View Charts` bit.
 * @param species_id The unique Species ID that is attached to the Animal Names in question.
 * @return A list of Animal IDs that correspond to the given species.
 */
std::list<GkRecords::GkId> HerpApp::find_animal_names(const GkRecords::comboBoxType &dropbox_type, const std::string &species_id)
{
    try {
        switch (dropbox_type) {
            case GkRecords::comboBoxType::AddRecord:
                ui->comboBox_existing_id->clear();
                break;
            case GkRecords::comboBoxType::ViewRecords:
                ui->comboBox_view_records_animal_name->clear();
                break;
            case GkRecords::comboBoxType::ViewCharts:
                ui->comboBox_view_charts_select_id->clear();
                break;
            default:
                throw std::runtime_error(tr("An error occurred whilst filling a comboBox with info from the database!").toStdString());
        }

        std::list<GkRecords::GkId> output; // A list of Animal IDs that correspond to the given species.
        int counter = 0;
        if ((!species_id.empty()) && (!animal_cache.empty())) {
            for (auto it_ca = animal_cache.begin(); it_ca != animal_cache.end(); ++it_ca) {
                if (it_ca.key() == species_id) {
                    std::string animal_id = it_ca.value();
                    GkRecords::GkId animal;
                    animal.name_id = animal_id;

                    auto tmp_animals_db = gkDbRead->get_cat_key_vals(GkRecords::MiscRecordType::gkId);
                    for (auto it_tmp = tmp_animals_db.begin(); it_tmp != tmp_animals_db.end(); ++it_tmp) {
                        if (it_tmp.key() == animal_id) {
                            switch (dropbox_type) {
                                case GkRecords::comboBoxType::AddRecord:
                                    ui->comboBox_existing_id->insertItem(counter, QString::fromStdString(it_tmp.value()));
                                    animal.identifier_str = it_tmp.value();
                                    animal.comboBox.index_no = counter;
                                    animal.comboBox.comboBox_type = GkRecords::comboBoxType::AddRecord;
                                    output.push_back(animal);
                                    ++counter;
                                    break;
                                case GkRecords::comboBoxType::ViewRecords:
                                    ui->comboBox_view_records_animal_name->insertItem(counter, QString::fromStdString(it_tmp.value()));
                                    animal.identifier_str = it_tmp.value();
                                    animal.comboBox.index_no = counter;
                                    animal.comboBox.comboBox_type = GkRecords::comboBoxType::ViewRecords;
                                    output.push_back(animal);
                                    ++counter;
                                    break;
                                case GkRecords::comboBoxType::ViewCharts:
                                    ui->comboBox_view_charts_select_id->insertItem(counter, QString::fromStdString(it_tmp.value()));
                                    animal.identifier_str = it_tmp.value();
                                    animal.comboBox.index_no = counter;
                                    animal.comboBox.comboBox_type = GkRecords::comboBoxType::ViewCharts;
                                    output.push_back(animal);
                                    ++counter;
                                    break;
                            }

                            break;
                        }
                    }
                }
            }

            return output;
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return std::list<GkRecords::GkId>();
}

void HerpApp::record_species_index(const std::list<GkRecords::GkSpecies> &species_list, const GkRecords::comboBoxType &comboBox_type)
{
    try {
        if (!species_list.empty()) {
            if (!comboBox_species.empty()) {
                switch (comboBox_type) {
                    case GkRecords::comboBoxType::AddRecord:
                        comboBox_species.remove(GkRecords::comboBoxType::AddRecord);
                        break;
                    case GkRecords::comboBoxType::ViewRecords:
                        comboBox_species.remove(GkRecords::comboBoxType::ViewRecords);
                        break;
                    case GkRecords::comboBoxType::ViewCharts:
                        comboBox_species.remove(GkRecords::comboBoxType::ViewCharts);
                        break;
                    default:
                        throw std::runtime_error(tr("An error had occurred whilst filling a QComboBox with information!").toStdString());
                }
            }

            for (const auto &species: species_list) {
                comboBox_species.insertMulti(comboBox_type, species);
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }

    return;
}

void HerpApp::record_animals_index(const std::list<GkRecords::GkId> &animals_list, const GkRecords::comboBoxType &comboBox_type)
{
    try {
        if (!animals_list.empty()) {
            if (!comboBox_animals.empty()) {
                switch (comboBox_type) {
                    case GkRecords::comboBoxType::AddRecord:
                        comboBox_animals.remove(GkRecords::comboBoxType::AddRecord);
                        break;
                    case GkRecords::comboBoxType::ViewRecords:
                        comboBox_animals.remove(GkRecords::comboBoxType::ViewRecords);
                        break;
                    case GkRecords::comboBoxType::ViewCharts:
                        comboBox_animals.remove(GkRecords::comboBoxType::ViewCharts);
                        break;
                    default:
                        throw std::runtime_error(tr("An error had occurred whilst filling a QComboBox with information!").toStdString());
                }
            }

            for (const auto &animals: animals_list) {
                comboBox_animals.insertMulti(comboBox_type, animals);
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
        return;
    }

    return;
}

void HerpApp::archive_clear_forms()
{
    ui->lineEdit_records_dateTime->clear();

    ui->lineEdit_records_toilet_notes->clear();
    ui->lineEdit_records_hydration_notes->clear();
    ui->lineEdit_records_vitamins_notes->clear();
    ui->lineEdit_records_temperature->clear();
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
        std::lock_guard<std::mutex> locker(r_cache_mtx);
        // NOTE: It is not necessary to run `refresh_caches()` prior to any of this code henceforth...

        if (!record_id.empty()) {
            GkRecords::GkSubmit submit_data;
            submit_data.identifier.name_id = find_comboBox_id(GkRecords::MiscRecordType::gkId,
                                                              GkRecords::comboBoxType::ViewRecords,
                                                              comboBox_view_records_animals_sel);

            for (const auto &cached_record: unique_id_map) {
                if (cached_record.first == record_id) {
                    submit_data.record_id = record_id;
                    if (cached_record.second.name_id == submit_data.identifier.name_id) {
                        submit_data.licensee.licensee_id = cached_record.second.licensee_id;
                        submit_data.species.species_id = cached_record.second.species_id;
                        break;
                    }
                }
            }

            if ((!submit_data.licensee.licensee_id.empty()) && (!submit_data.species.species_id.empty())) {
                auto licensee_data = gkDbRead->get_cat_key_vals(GkRecords::MiscRecordType::gkLicensee);
                auto species_data = gkDbRead->get_cat_key_vals(GkRecords::MiscRecordType::gkSpecies);
                auto ident_data = gkDbRead->get_cat_key_vals(GkRecords::MiscRecordType::gkId);

                for (auto it = licensee_data.begin(); it != licensee_data.end(); ++it) {
                    if (it.key() == submit_data.licensee.licensee_id) {
                        submit_data.licensee.licensee_name = it.value();
                        break;
                    }
                }

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
                submit_data.temp_notes = gkDbRead->read_item_db(submit_data.record_id, GkRecords::tempNotes);
                submit_data.weight_notes = gkDbRead->read_item_db(submit_data.record_id, GkRecords::weightNotes);
                submit_data.hydration_notes = gkDbRead->read_item_db(submit_data.record_id, GkRecords::hydrationNotes);
                submit_data.went_toilet = boost::lexical_cast<bool>(gkDbRead->read_item_db(submit_data.record_id, GkRecords::boolWentToilet));
                submit_data.had_hydration = boost::lexical_cast<bool>(gkDbRead->read_item_db(submit_data.record_id, GkRecords::boolHadHydration));
                submit_data.had_vitamins = boost::lexical_cast<bool>(gkDbRead->read_item_db(submit_data.record_id, GkRecords::boolHadVitamins));
                submit_data.weight = std::stod(gkDbRead->read_item_db(submit_data.record_id, GkRecords::weightMeasure));

                if ((submit_data.date_time > 0) && (!submit_data.licensee.licensee_name.empty()) &&
                        (!submit_data.species.species_name.empty()) && (!submit_data.identifier.identifier_str.empty())) {
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

                    if (!submit_data.temp_notes.empty()) {
                        ui->lineEdit_records_temperature->setText(QString::fromStdString(submit_data.temp_notes));
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
                QMessageBox::information(this, tr("No data!"), tr("There is no information to present with the given variables. Please try another selection."),
                                         QMessageBox::Ok);
            }
        } else {
            archive_clear_forms();
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return;
}

/**
 * @brief HerpApp::delete_category_id
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-19
 * @param record_type
 * @param record_id
 * @return
 */
bool HerpApp::delete_category_id(const GkRecords::MiscRecordType &record_type, const std::string &record_id)
{
    // TODO: Finish this!
    try {
        if (!record_id.empty()) {
            using namespace GkRecords;
            switch (record_type) {
                case MiscRecordType::gkLicensee:
                {
                    gkDbWrite->mass_del_cat(MiscRecordType::gkLicensee, record_id);
                }
                    return true;
                case MiscRecordType::gkSpecies:
                {
                    gkDbWrite->mass_del_cat(MiscRecordType::gkSpecies, record_id);
                }
                    return true;
                case MiscRecordType::gkId:
                {
                    gkDbWrite->mass_del_cat(MiscRecordType::gkId, record_id);
                }
                    return true;
                default:
                    throw std::runtime_error(tr("An error occurred whilst deleting records from the database!").toStdString());
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return false;
}

/**
 * @brief HerpApp::insert_charts will insert the QChart widgets into the GUI, in their respective places.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-04
 */
void HerpApp::insert_charts()
{
    std::lock_guard<std::mutex> locker(r_charts_mtx);
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
void HerpApp::update_charts(const bool &update_caches)
{
    try {
        std::lock_guard<std::mutex> locker(r_charts_mtx);
        line_series_weight = new QLineSeries(this);

        if (update_caches) {
            refresh_caches();
        }

        if (!unique_id_map.empty()) {
            using namespace GkRecords;
            auto dated_record_ids = gkDbRead->extract_records(minDateTime, maxDateTime);
            long int date_time;

            if ((!dated_record_ids.empty()) && (!species_cache.empty()) && (!animal_cache.empty())) { // Check that the values we're using aren't empty
                for (const auto &dated_id: dated_record_ids) {
                    date_time = std::stol(gkDbRead->read_item_db(dated_id, dateTime)); // Extract dateTime values from the database
                    if (!weight_measurements.contains(date_time)) { // Check that the key does not already exist in the cache!
                        for (const auto &mapped_id: unique_id_map) {
                            GkSpecies species_struct;
                            GkId ident_struct;
                            double weight_in_loop;

                            if (dated_id == mapped_id.first) {
                                weight_in_loop = std::stod(gkDbRead->read_item_db(dated_id, weightMeasure));
                                species_struct.species_id = mapped_id.second.species_id;
                                ident_struct.name_id = mapped_id.second.name_id;

                                for (auto it = species_cache.begin(); it != species_cache.end(); ++it) {
                                    if (species_struct.species_id == it.key()) {
                                        species_struct.species_name = it.value();
                                    }
                                }

                                for (auto it = animal_cache.begin(); it != animal_cache.end(); ++it) {
                                    if (ident_struct.name_id == it.key()) {
                                        ident_struct.identifier_str = it.value();
                                    }
                                }

                                GkGraph::WeightVsTime weight_struct;
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

                if (dated_record_ids.size() >= 2) { // Therefore there are at least two plot points for the graph(s)
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
