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

int GkDbRead::determineMinimumDate(const std::vector<std::string> &record_id)
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

int GkDbRead::determineMaximumDate(const std::vector<std::string> &record_id)
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

/**
 * @brief GkDbRead::get_record_ids will obtain all the Unique Identifiers for each record that's in the database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @return The information that was retrieved from the database.
 */
std::unordered_map<std::string, std::pair<std::string, std::string>> GkDbRead::get_record_ids()
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
 * @brief GkDbRead::extractRecords will extract whatever Record IDs that lay within a given date range, depending on when
 * they were `submitted`.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-28
 * @param dateStart The beginning of the date range, as UNIX Epoch Time.
 * @param dateEnd The end of the date range, as UNIX Epoch Time.
 * @return The extracted Record IDs that lay within the given date range.
 */
std::vector<std::string> GkDbRead::extractRecords(const int &dateStart, const int &dateEnd)
{
    auto record_id_cache = get_record_ids();
}
