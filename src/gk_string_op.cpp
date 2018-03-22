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
 * @file gk_string_op.cpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @brief This file holds any functions related to string processing/modification.
 */

#include "gk_string_op.hpp"
#include <boost/exception/all.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <QMessageBox>
#include <random>
#include <sstream>

using namespace GekkoFyre;
GkStringOp::GkStringOp(QObject *parent) : QObject(parent)
{}

GkStringOp::~GkStringOp()
{}

std::string GkStringOp::random_hash()
{
    boost::mt19937 ran;
    std::random_device rd;
    ran.seed(rd());
    boost::uuids::basic_random_generator<boost::mt19937> gen(&ran);
    boost::uuids::uuid u = gen();
    std::string result = boost::uuids::to_string(u); // Convert the Boost UUID to a std::string
    std::string substr = result.substr(0, 8); // Extract just the first few characters from the UUID
    boost::to_upper(substr); // Convert to uppercase
    return substr;
}

/**
 * @brief GkStringOp::multipart_key combines and creates a unique key that is suitable for lookup of individual records
 * in a Google LevelDB database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param args The multipart arguments to be provided, whether one, a dozen, or even more. But usually only two in this case.
 * @return The combined key, outputted as a std::string.
 */
std::string GkStringOp::multipart_key(const std::initializer_list<std::string> &args)
{
    std::ostringstream ret_val;
    static int counter;
    counter = 0;
    for (const auto &arg: args) {
        ++counter;
        if (counter == 1) {
            ret_val << arg;
        } else {
            ret_val << "_" << arg;
        }
    }

    return ret_val.str();
}

/**
 * @brief GkStringOp::del_cat_msg_box will pose a QMessageBox to the user of HerpLog and ask them if they want to proceed
 * with the action of deleting the specified categories in question (`Licensee`, `Species`, or `Animal ID`) from
 * the Google LevelDB database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-21
 * @param uuid_cache The cache of UUIDs that have been passed on from the previous function.
 * @param record_id The specified and unique Record ID in question that's to be deleted.
 * @param record_type The type of category we are dealing with in this instance (`Licensee`, `Species`, or `Animal ID`).
 * @return Whether the user wants to delete the specified categories in question (TRUE) or not (FALSE).
 * @see GekkoFyre::GkDbWrite::mass_del_cat()
 */
bool GkStringOp::del_cat_msg_box(const GkRecords::GkCategories &cat_struct, const GkRecords::MiscRecordType &record_type)
{
    try {
        if ((!cat_struct.spec_record_id.empty())) {
            using namespace GkRecords;
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Proceed?"));
            msgBox.setText(tr("Are you sure about deleting all of the specified `categories` and `log entries`?"));
            msgBox.setStandardButtons(QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::No);

            int count_licensees = 0, count_species = 0, count_animal_ids = 0, count_log_entries = 0;
            switch (record_type) {
                case MiscRecordType::gkLicensee:
                {
                    count_licensees += 1;
                    if ((!cat_struct.species_cache.empty()) && (!cat_struct.animals_cache.empty())) {
                        QVector<std::string> unique_uuid_vec;
                        QVector<std::string> unique_licensee_vec;
                        QVector<std::string> unique_species_vec;
                        QVector<std::string> unique_animals_vec;

                        for (auto it = cat_struct.species_cache.begin(); it != cat_struct.species_cache.end(); ++it) {
                            if (!unique_uuid_vec.contains(it.key())) {
                                unique_uuid_vec.push_back(it.key());
                            }

                            if (!unique_licensee_vec.contains(it.value().first)) {
                                unique_licensee_vec.push_back(it.value().first);
                            }

                            if (!unique_species_vec.contains(it.value().second)) {
                                unique_species_vec.push_back(it.value().second);
                            }
                        }

                        for (auto it = cat_struct.animals_cache.begin(); it != cat_struct.animals_cache.end(); ++it) {
                            if (!unique_uuid_vec.contains(it.key())) {
                                unique_uuid_vec.push_back(it.key());
                            }

                            if (!unique_animals_vec.contains(it.value().second)) {
                                unique_animals_vec.push_back(it.value().second);
                            }
                        }

                        // Calculate the numbers of each `category` that will be deleted
                        count_licensees = unique_licensee_vec.size();
                        count_species = unique_species_vec.size();
                        count_animal_ids = unique_animals_vec.size();

                        // Calculate the amount of `log entries` that will be deleted
                        count_log_entries = unique_uuid_vec.size();
                    } else {
                        throw std::invalid_argument(tr("Cache is not initialized! It's empty!").toStdString());
                    }
                }

                    break;
                case MiscRecordType::gkSpecies:
                {
                    if ((!cat_struct.licensee_cache.empty()) && (!cat_struct.animals_cache.empty())) {
                        QVector<std::string> unique_uuid_vec;
                        QVector<std::string> unique_licensee_vec;
                        QVector<std::string> unique_species_vec;
                        QVector<std::string> unique_animals_vec;

                        for (auto it = cat_struct.licensee_cache.begin(); it != cat_struct.licensee_cache.end(); ++it) {
                            if (!unique_uuid_vec.contains(it.key())) {
                                unique_uuid_vec.push_back(it.key());
                            }

                            if (!unique_licensee_vec.contains(it.value())) {
                                unique_licensee_vec.push_back(it.value());
                            }
                        }

                        for (auto it = cat_struct.animals_cache.begin(); it != cat_struct.animals_cache.end(); ++it) {
                            if (!unique_uuid_vec.contains(it.key())) {
                                unique_uuid_vec.push_back(it.key());
                            }

                            if (!unique_species_vec.contains(it.value().first)) {
                                unique_species_vec.push_back(it.value().first);
                            }

                            if (!unique_animals_vec.contains(it.value().second)) {
                                unique_animals_vec.push_back(it.value().second);
                            }
                        }

                        // Calculate the numbers of each `category` that will be deleted
                        count_licensees = unique_licensee_vec.size();
                        count_species = unique_species_vec.size();
                        count_animal_ids = unique_animals_vec.size();

                        // Calculate the amount of `log entries` that will be deleted
                        count_log_entries = unique_uuid_vec.size();
                    } else {
                        throw std::invalid_argument(tr("Cache is not initialized! It's empty!").toStdString());
                    }
                }

                    break;
                case MiscRecordType::gkId:
                {
                    if ((!cat_struct.licensee_cache.empty()) && (!cat_struct.species_cache.empty())) {
                        QVector<std::string> unique_uuid_vec;
                        QVector<std::string> unique_licensee_vec;
                        QVector<std::string> unique_species_vec;
                        QVector<std::string> unique_animals_vec;

                        for (auto it = cat_struct.licensee_cache.begin(); it != cat_struct.licensee_cache.end(); ++it) {
                            if (!unique_uuid_vec.contains(it.key())) {
                                unique_uuid_vec.push_back(it.key());
                            }

                            if (!unique_licensee_vec.contains(it.value())) {
                                unique_licensee_vec.push_back(it.value());
                            }
                        }

                        for (auto it = cat_struct.species_cache.begin(); it != cat_struct.species_cache.end(); ++it) {
                            if (!unique_uuid_vec.contains(it.key())) {
                                unique_uuid_vec.push_back(it.key());
                            }

                            if (!unique_species_vec.contains(it.value().first)) {
                                unique_species_vec.push_back(it.value().first);
                            }

                            if (!unique_animals_vec.contains(it.value().second)) {
                                unique_animals_vec.push_back(it.value().second);
                            }
                        }

                        // Calculate the numbers of each `category` that will be deleted
                        count_licensees = unique_licensee_vec.size();
                        count_species = unique_species_vec.size();
                        count_animal_ids = unique_animals_vec.size();

                        // Calculate the amount of `log entries` that will be deleted
                        count_log_entries = unique_uuid_vec.size();
                    } else {
                        throw std::invalid_argument(tr("Cache is not initialized! It's empty!").toStdString());
                    }
                }

                    break;
                default:
                    throw std::runtime_error(tr("Unable to perform calculation for deletion of records from the database!")
                                                     .toStdString());
            }

            msgBox.setDetailedText(tr("Total number of deletions include...\n\n%1 Licensees\n%2 Species\n%3 Animal IDs\n\n%4 Log entries")
                                           .arg(QString::number(count_licensees))
                                           .arg(QString::number(count_species))
                                           .arg(QString::number(count_animal_ids))
                                           .arg(QString::number(count_log_entries)));
            int ret = msgBox.exec();
            switch (ret) {
                case QMessageBox::YesToAll:
                    return true;
                case QMessageBox::No:
                    return false;
                case QMessageBox::Cancel:
                    return false;
                default:
                    // Should never be reached!
                    throw std::invalid_argument(tr("Unable to read Unique Identifier from database!").toStdString());
            }
        } else {
            throw std::invalid_argument(tr("One of the given UUID caches are empty!").toStdString());
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return false;
}
