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
 * @file gk_db_write.cpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @brief Contains any database-related routines, specifically ones related to Google LevelDB
 * and writing data towards.
 */

#include "gk_db_write.hpp"
#include "3rd_party/minicsv/minicsv.h"
#include <boost/exception/all.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/cache.h>
#include <QMessageBox>
#include <random>
#include <exception>
#include <sstream>
#include <utility>
#include <algorithm>

using namespace GekkoFyre;
using namespace mini;
GkDb::GkDb(const GkFile::FileDb &database, const std::shared_ptr<GkStringOp> &gk_str_op, QObject *parent) : QObject(parent)
{
    db_conn = database;
    gkStrOp = gk_str_op;
}

GkDb::~GkDb()
{}

/**
 *@brief GkDb::add_item_db
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param record_id The Unique ID that identifies the record being written/deleted/read.
 * @param key The type of record that is being written/deleted/read, usually stated as unique string of characters.
 * @param value The information that you wish to store in the database.
 */
void GkDb::add_item_db(const std::string &record_id, const std::string &key, std::string value)
{
    if (value.empty()) {
        value = "";
    }

    leveldb::WriteOptions write_options;
    write_options.sync = true;
    leveldb::WriteBatch batch;
    std::lock_guard<std::mutex> locker(db_mutex);
    std::string key_joined = gkStrOp->multipart_key({record_id, key});
    batch.Delete(key_joined);
    batch.Put(key_joined, value);
    leveldb::Status s;
    s = db_conn.db->Write(write_options, &batch);
    if (!s.ok()) {
        throw std::runtime_error(s.ToString());
    }

    return;
}

/**
 * @brief GkDb::del_item_db
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param record_id The Unique ID that identifies the record being written/deleted/read.
 * @param key The type of record that is being written/deleted/read, usually stated as unique string of characters.
 */
void GkDb::del_item_db(const std::string &record_id, const std::string &key)
{
    leveldb::WriteOptions write_options;
    write_options.sync = true;
    leveldb::WriteBatch batch;
    std::lock_guard<std::mutex> locker(db_mutex);
    std::string key_joined = gkStrOp->multipart_key({record_id, key});
    batch.Delete(key_joined);
    leveldb::Status s;
    s = db_conn.db->Write(write_options, &batch);
    if (!s.ok()) {
        throw std::runtime_error(s.ToString());
    }

    return;
}

/**
 * @brief GkDb::read_item_db
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param record_id The Unique ID that identifies the record being written/deleted/read.
 * @param key The type of record that is being written/deleted/read, usually stated as unique string of characters.
 * @return The information that was retrieved from the database, given the Unique ID and Key.
 */
std::string GkDb::read_item_db(const std::string &record_id, const std::string &key)
{
    std::string read_data;
    leveldb::ReadOptions read_opt;
    leveldb::Status s;
    read_opt.verify_checksums = true;
    std::string key_joined = gkStrOp->multipart_key({record_id, key});

    std::lock_guard<std::mutex> locker(db_mutex);
    s = db_conn.db->Get(read_opt, key_joined, &read_data);
    if (!s.ok()) {
        throw std::runtime_error(s.ToString());
    }

    if (!read_data.empty()) {
        return read_data;
    } else {
        return "";
    }
}

/**
 * @brief GkDb::get_misc_key_vals will obtain all the Unique Identifiers from the database for the given key, IF it's related
 * to GkRecords::GkSpecies or GkRecords::GkId ONLY.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @return The information that was retrieved from the database.
 */
auto GkDb::get_misc_key_vals(const GkRecords::StrucType &struc_type)
{
    leveldb::ReadOptions read_opt;
    read_opt.verify_checksums = true;

    std::string csv_read_data;
    std::lock_guard<std::mutex> locker(db_mutex);

    std::unordered_map<std::string, std::string> cache;

    switch (struc_type) {
        case GkRecords::StrucType::gkSpecies:
            db_conn.db->Get(read_opt, GkRecords::speciesId, &csv_read_data);

            if (!csv_read_data.empty()) {
                try {
                    csv::istringstream iss(csv_read_data);
                    iss.set_delimiter(',', "$$");
                    std::string species_id, species_name;

                    while (iss.read_line()) {
                        iss >> species_id >> species_name;
                        cache.insert(std::make_pair(species_id, species_name));
                    }
                } catch (const std::exception &e) {
                    QMessageBox::warning(nullptr, tr("Error!"), e.what(), QMessageBox::Ok);
                }
            }

            break;
        case GkRecords::StrucType::gkId:
            db_conn.db->Get(read_opt, GkRecords::nameId, &csv_read_data);

            if (!csv_read_data.empty()) {
                try {
                    csv::istringstream iss(csv_read_data);
                    iss.set_delimiter(',', "$$");
                    std::string name_id, identity_str;
                    while (iss.read_line()) {
                        iss >> name_id >> identity_str;
                        cache.insert(std::make_pair(name_id, identity_str));
                    }
                } catch (const std::exception &e) {
                    QMessageBox::warning(nullptr, tr("Error!"), e.what(), QMessageBox::Ok);
                }
            }

            break;
        default:
            throw std::invalid_argument(tr("Unable to read Unique Identifier from database! This should not happen!").toStdString());
    }

    return cache;
}

/**
 * @brief GkDb::get_record_ids will obtain all the Unique Identifiers for each record that's in the database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @return The information that was retrieved from the database.
 */
std::unordered_map<std::string, std::pair<std::string, std::string>> GkDb::get_record_ids()
{
    leveldb::ReadOptions read_opt;
    read_opt.verify_checksums = true;

    std::string csv_read_data;
    std::lock_guard<std::mutex> locker(db_mutex);
    db_conn.db->Get(read_opt, GkRecords::LEVELDB_STORE_RECORD_ID, &csv_read_data);

    std::unordered_map<std::string, std::pair<std::string, std::string>> cache;
    if (!csv_read_data.empty()) {
        try {
            csv::istringstream iss(csv_read_data);
            iss.set_delimiter(',', "$$");
            std::string record_id, species_id, name_id;
            while (iss.read_line()) {
                iss >> record_id >> species_id >> name_id;
                cache.insert(std::make_pair(record_id, std::make_pair(species_id, name_id)));
            }
        } catch (const std::exception &e) {
            QMessageBox::warning(nullptr, tr("Error!"), e.what(), QMessageBox::Ok);
        }
    }

    return cache;
}

/**
 * @brief GkDb::add_misc_key_val Adds a Unique Identifier for either a new Species or Name/ID sub-record to the Google LevelDB
 * database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param struc_type Whether we are adding a key for the Species or Name/ID sub-record.
 * @param unique_id The Unique Identifier itself, usually a UUID in this case.
 * @param value The value to be stored alongside the UUID.
 * @return Whether the process was a success or not.
 */
void GkDb::add_misc_key_vals(const GkRecords::StrucType &struc_type, const std::string &unique_id,
                             const std::string &value)
{
    std::ostringstream oss;
    using namespace GkRecords;

    leveldb::WriteOptions write_options;
    write_options.sync = true;
    leveldb::WriteBatch batch;
    std::lock_guard<std::mutex> locker(create_key_mutex);

    switch (struc_type) {
        case StrucType::gkSpecies:
        {
            auto species_cache = get_misc_key_vals(StrucType::gkSpecies);

            if (species_cache.empty()) {
                oss << GkRecords::csvSpeciesId << "," << GkRecords::csvSpeciesName << std::endl;
            } else {
                for (const auto &species: species_cache) {
                    oss << species.first << "," << species.second << std::endl;
                }
            }

            // Now we insert the new UUID alongside with its value
            oss << unique_id << "," << value << std::endl;

            batch.Delete(LEVELDB_STORE_SPECIES_ID);
            batch.Put(LEVELDB_STORE_SPECIES_ID, oss.str());
        }

            break;
        case StrucType::gkId:
        {
            auto id_cache = get_misc_key_vals(StrucType::gkId);

            if (id_cache.empty()) {
                oss << GkRecords::csvNameId << "," << GkRecords::csvIdentifyStr << std::endl;
            } else {
                for (const auto &id: id_cache) {
                    oss << id.first << "," << id.second << std::endl;
                }
            }

            // Now we insert the new UUID alongside with its value
            oss << unique_id << "," << value << std::endl;

            batch.Delete(LEVELDB_STORE_NAME_ID);
            batch.Put(LEVELDB_STORE_NAME_ID, oss.str());
        }

            break;
        default:
            throw std::invalid_argument(tr("Unable to read Unique Identifier from database! This should not happen!").toStdString());
    }

    leveldb::Status s;
    s = db_conn.db->Write(write_options, &batch);
    if (!s.ok()) {
        throw std::runtime_error(s.ToString());
    }

    return;
}

/**
 * @brief GkDb::add_record_id Adds a new Unique Identifier for the record in question to the Google LevelDB database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @return Whether the process was a success or not.
 */
bool GkDb::add_record_id(const std::string &unique_id, const GkRecords::GkSpecies &species, const GkRecords::GkId &id)
{
    using namespace GkRecords;

    try {
        std::ostringstream oss;
        auto record_cache = get_record_ids();

        if (record_cache.empty()) {
            oss << GkRecords::csvRecordId << "," << GkRecords::csvSpeciesId << "," << GkRecords::csvNameId << std::endl;
        } else {
            for (const auto &record: record_cache) {
                oss << record.first << "," << record.second.first << "," << record.second.second << std::endl;
            }
        }

        // Now we insert the new UUID alongside with its value
        oss << unique_id << "," << species.species_id << "," << id.name_id << std::endl;

        if (!species.species_name.empty() && !species.species_id.empty()) {
            // We have a new entry for the Species sub-record!
            add_misc_key_vals(StrucType::gkSpecies, species.species_id, species.species_name);
        }

        if (!id.identifier_str.empty()) {
            // We have a new entry for the Name/ID# sub-record!
            add_misc_key_vals(StrucType::gkId, id.name_id, id.identifier_str);
        }

        leveldb::WriteOptions write_options;
        write_options.sync = true;
        leveldb::WriteBatch batch;
        std::lock_guard<std::mutex> locker(db_mutex);

        batch.Delete(LEVELDB_STORE_RECORD_ID);
        batch.Put(LEVELDB_STORE_RECORD_ID, oss.str());

        leveldb::Status s;
        s = db_conn.db->Write(write_options, &batch);
        if (!s.ok()) {
            throw std::runtime_error(s.ToString());
        }

        return true;
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error!"), e.what(), QMessageBox::Ok);
        return false;
    }
}

std::string GkDb::create_unique_id()
{
    std::lock_guard<std::mutex> locker(db_mutex);
    boost::mt19937 ran;
    std::random_device rd;
    ran.seed(rd());
    boost::uuids::basic_random_generator<boost::mt19937> gen(&ran);
    boost::uuids::uuid u = gen();
    std::string result = boost::uuids::to_string(u); // Convert the Boost UUID to a std::string
    boost::to_upper(result); // Convert to uppercase
    return result;
}

int GkDb::determineMinimumDate(const std::vector<std::string> &record_id)
{
    if (!record_id.empty()) {
        std::vector<std::string> dates_str_vec;
        for (const auto &id: record_id) {
            dates_str_vec.push_back(read_item_db(id, GkRecords::dateTime));
        }

        std::vector<int> dates_vec;
        for (const auto &date: dates_str_vec) {
            dates_vec.push_back(std::stoi(date));
        }

        return *std::min_element(dates_vec.begin(), dates_vec.end());
    }

    return 0;
}

int GkDb::determineMaximumDate(const std::vector<std::string> &record_id)
{
    if (!record_id.empty()) {
        std::vector<std::string> dates_str_vec;
        for (const auto &id: record_id) {
            dates_str_vec.push_back(read_item_db(id, GkRecords::dateTime));
        }

        std::vector<int> dates_vec;
        for (const auto &date: dates_str_vec) {
            dates_vec.push_back(std::stoi(date));
        }

        return *std::max_element(dates_vec.begin(), dates_vec.end());
    }

    return 0;
}
