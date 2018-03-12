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
 * @file gk_db_read.cpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-28
 * @brief Contains any database-related routines, specifically ones related to Google LevelDB
 * and reading data from within.
 */

#include "gk_db_read.hpp"
#include "3rd_party/minicsv/minicsv.h"
#include <QMessageBox>

using namespace GekkoFyre;
using namespace mini;
GkDbRead::GkDbRead(const GekkoFyre::GkFile::FileDb &gk_db_conn, const std::shared_ptr<GekkoFyre::GkStringOp> &gk_str_op,
                              QObject *parent)
{
    db_conn = gk_db_conn;
    gkStrOp = gk_str_op;
}

GkDbRead::~GkDbRead()
{}

/**
 * @brief GkDbRead::read_item_db
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param record_id The Unique ID that identifies the record being written/deleted/read.
 * @param key The type of record that is being written/deleted/read, usually stated as unique string of characters.
 * @return The information that was retrieved from the database, given the Unique ID and Key.
 */
std::string GkDbRead::read_item_db(const std::string &record_id, const std::string &key)
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

long int GkDbRead::determineMinimumDate(const std::vector<std::string> &record_id)
{
    if (!record_id.empty()) {
        std::vector<std::string> dates_str_vec;
        for (const auto &id: record_id) {
            std::string db_val = read_item_db(id, GkRecords::dateTime);
            if (!db_val.empty()) {
                dates_str_vec.push_back(db_val);
            }
        }

        if (!dates_str_vec.empty()) {
            std::vector<long int> dates_vec;
            for (const auto &date: dates_str_vec) {
                dates_vec.push_back(std::stol(date));
            }

            return *std::min_element(dates_vec.begin(), dates_vec.end());
        }
    }

    return 0;
}

long int GkDbRead::determineMaximumDate(const std::vector<std::string> &record_id)
{
    if (!record_id.empty()) {
        std::vector<std::string> dates_str_vec;
        for (const auto &id: record_id) {
            std::string db_val = read_item_db(id, GkRecords::dateTime);
            if (!db_val.empty()) {
                dates_str_vec.push_back(db_val);
            }
        }

        if (!dates_str_vec.empty()) {
            std::vector<long int> dates_vec;
            for (const auto &date: dates_str_vec) {
                dates_vec.push_back(std::stol(date));
            }

            return *std::max_element(dates_vec.begin(), dates_vec.end());
        }
    }

    return 0;
}

/**
 * @brief GkDbRead::get_record_ids will obtain all the Unique Identifiers for each record that's in the database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @return The information that was retrieved from the database.
 */
std::unordered_map<std::string, GkRecords::MiscUniqueIds> GkDbRead::get_record_ids()
{
    leveldb::ReadOptions read_opt;
    read_opt.verify_checksums = true;

    std::string csv_read_data;
    std::lock_guard<std::mutex> locker(db_mutex);
    db_conn.db->Get(read_opt, GkRecords::LEVELDB_STORE_RECORD_ID, &csv_read_data);

    std::unordered_map<std::string, GkRecords::MiscUniqueIds> cache;
    if (!csv_read_data.empty()) {
        try {
            csv::istringstream iss(csv_read_data);
            iss.set_delimiter(',', "$$");
            std::string record_id;
            GkRecords::MiscUniqueIds unique_ids;
            while (iss.read_line()) {
                iss >> record_id >> unique_ids.licensee_id >> unique_ids.species_id >> unique_ids.name_id;

                if ((!record_id.empty()) && (!unique_ids.licensee_id.empty()) && (!unique_ids.species_id.empty()) &&
                        (!unique_ids.name_id.empty())) {
                    cache.insert(std::make_pair(record_id, unique_ids));
                } else {
                    throw std::invalid_argument(tr("An error had occurred whilst obtaining information about stored-keys from the database!").toStdString());
                }
            }
        } catch (const std::exception &e) {
            QMessageBox::warning(nullptr, tr("Error!"), e.what(), QMessageBox::Ok);
        }
    }

    return cache;
}

/**
 * @brief GkDbRead::get_misc_key_vals will obtain all the Unique Identifiers from the database for the given key, IF it's related
 * to GkRecords::GkSpecies or GkRecords::GkId ONLY.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param struc_type Whether to get data from `store_species_id` or `store_name_id` within the Google LevelDB database.
 * @return The information that was retrieved from the database; <Key: Species ID/Name ID, Value: Species Name/Name Value>
 */
QMultiMap<std::string, std::string> GkDbRead::get_misc_key_vals(const GkRecords::MiscRecordType &struc_type)
{
    try {
        leveldb::ReadOptions read_opt;
        read_opt.verify_checksums = true;

        std::string csv_read_data;
        std::lock_guard<std::mutex> locker(db_mutex);

        QMultiMap<std::string, std::string> cache;

        switch (struc_type) {
            case GkRecords::MiscRecordType::gkLicensee:
                db_conn.db->Get(read_opt, GkRecords::LEVELDB_STORE_LICENSEE_ID, &csv_read_data);
                break;
            case GkRecords::MiscRecordType::gkSpecies:
                db_conn.db->Get(read_opt, GkRecords::LEVELDB_STORE_SPECIES_ID, &csv_read_data);
                break;
            case GkRecords::MiscRecordType::gkId:
                db_conn.db->Get(read_opt, GkRecords::LEVELDB_STORE_NAME_ID, &csv_read_data);
                break;
            default:
                throw std::invalid_argument(tr("Unable to read Unique Identifier from database!").toStdString());
        }

        if (!csv_read_data.empty()) {
            csv::istringstream iss(csv_read_data);
            iss.set_delimiter(',', "$$");
            std::string id, name;

            while (iss.read_line()) {
                iss >> id >> name;
                cache.insertMulti(id, name);
            }
        }

        return cache;
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error!"), tr("Unable to read Unique Identifier from database! Error:\n\n%1").arg(e.what()), QMessageBox::Ok);
    }

    return QMultiMap<std::string, std::string>();
}

/**
 * @brief GkDbRead::extractRecords will extract whatever Record IDs that lay within a given date range, depending on when
 * they were `submitted`.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-28
 * @param dateStart The beginning of the date range, as UNIX Epoch Time.
 * @param dateEnd The end of the date range, as UNIX Epoch Time.
 * @return The extracted Record IDs that lay within the given date range.
 */
std::list<std::string> GkDbRead::extractRecords(const long int &dateStart, const long int &dateEnd)
{
    // Extract all the possible Record IDs from the database
    auto record_id_cache = get_record_ids();
    if (!record_id_cache.empty()) {
        std::vector<std::string> record_ids;
        for (const auto &ids: record_id_cache) {
            if (!ids.first.empty()) {
                record_ids.push_back(ids.first); // Put just the Record IDs ONLY into a std::vector<>()
            }
        }

        std::list<long int> collected_dates;
        std::unordered_map<std::string, long int> date_cache; // Key: Record IDs, Value: Dates

        // Filter out the dates that match our criteria within the database
        for (const auto &record: record_ids) {
            long int possible_date = std::stoi(read_item_db(record, GkRecords::dateTime)); // Extract the dates one-by-one from the database
            if (possible_date >= dateStart && possible_date <= dateEnd) { // Filter the dates through our criteria
                collected_dates.push_back(possible_date); // Add the dates that meet our criteria to an std::list
                date_cache.insert(std::make_pair(record, possible_date));
            }
        }

        collected_dates.sort(); // Sort the dates into ascending order
        std::list<std::string> output;
        for (const auto &date: collected_dates) {
            for (const auto &cache: date_cache) {
                if (date == cache.second) {
                    output.push_back(cache.first);
                }
            }
        }

        return output;
    }

    return std::list<std::string>();
}
