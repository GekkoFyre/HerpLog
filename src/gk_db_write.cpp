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
#include <QVector>
#include <QMap>
#include <random>
#include <exception>
#include <sstream>
#include <utility>
#include <algorithm>
#include <vector>

using namespace GekkoFyre;
using namespace mini;
GkDbWrite::GkDbWrite(const GkFile::FileDb &gk_db_conn, const std::shared_ptr<GkDbRead> &gk_db_read,
                     const std::shared_ptr<GkStringOp> &gk_str_op, QObject *parent) : QObject(parent)
{
    db_conn = gk_db_conn;
    gkDbRead = gk_db_read;
    gkStrOp = gk_str_op;
}

GkDbWrite::~GkDbWrite()
{}

/**
 *@brief GkDbWrite::add_item_db will add a single item to the Google LevelDB database, after first deleting any
 * pre-existing items of the same key.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param record_id The Unique ID that identifies the record being written/deleted/read.
 * @param key The type of record that is being written/deleted/read, usually stated as unique string of characters.
 * @param value The information that you wish to store in the database.
 */
void GkDbWrite::add_item_db(const std::string &record_id, const std::string &key, std::string value)
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
 * @brief GkDbWrite::del_item_db deletes a single, specified item from the Google LevelDB database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param record_id The Unique ID that identifies the record being written/deleted/read.
 * @param key The type of record that is being written/deleted/read, usually stated as unique string of characters.
 */
void GkDbWrite::del_item_db(const std::string &record_id, const std::string &key)
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
 * @brief GkDbWrite::add_misc_key_val Adds a Unique Identifier for either a new Licensee, Species, or Name/ID sub-record
 * within the Google LevelDB database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param record_type Whether to add data to `store_licensee_id`, `store_species_id`, or `store_name_id` within
 * the Google LevelDB database.
 * @param record_id The Unique Identifier itself, usually a UUID in this case.
 * @param value The value to be stored alongside the UUID.
 * @return Whether the process was a success or not.
 */
void GkDbWrite::add_cat_key_vals(const GkRecords::MiscRecordType &record_type, const std::string &record_id,
                                 const std::string &value)
{
    if ((!record_id.empty()) && (!value.empty())) {
        std::ostringstream oss;
        using namespace GkRecords;

        leveldb::WriteOptions write_options;
        write_options.sync = true;
        leveldb::WriteBatch batch;

        switch (record_type) {
            case MiscRecordType::gkLicensee:
            {
                auto licensee_cache = gkDbRead->get_cat_key_vals(MiscRecordType::gkLicensee);
                if (!licensee_cache.empty()) {
                    for (auto it = licensee_cache.begin(); it != licensee_cache.end(); ++it) {
                        oss << it.key() << "," << it.value() << std::endl;
                    }
                }

                // Now we insert the new UUID alongside with its value
                oss << record_id << "," << value << std::endl;

                batch.Delete(LEVELDB_STORE_LICENSEE_ID);
                batch.Put(LEVELDB_STORE_LICENSEE_ID, oss.str());
            }

                break;
            case MiscRecordType::gkSpecies:
            {
                auto species_cache = gkDbRead->get_cat_key_vals(MiscRecordType::gkSpecies);
                if (!species_cache.empty()) {
                    for (auto it = species_cache.begin(); it != species_cache.end(); ++it) {
                        oss << it.key() << "," << it.value() << std::endl;
                    }
                }

                // Now we insert the new UUID alongside with its value
                oss << record_id << "," << value << std::endl;

                batch.Delete(LEVELDB_STORE_SPECIES_ID);
                batch.Put(LEVELDB_STORE_SPECIES_ID, oss.str());
            }

                break;
            case MiscRecordType::gkId:
            {
                auto id_cache = gkDbRead->get_cat_key_vals(MiscRecordType::gkId);
                if (!id_cache.empty()) {
                    for (auto it = id_cache.begin(); it != id_cache.end(); ++it) {
                        oss << it.key() << "," << it.value() << std::endl;
                    }
                }

                // Now we insert the new UUID alongside with its value
                oss << record_id << "," << value << std::endl;

                batch.Delete(LEVELDB_STORE_NAME_ID);
                batch.Put(LEVELDB_STORE_NAME_ID, oss.str());
            }

                break;
            default:
                throw std::invalid_argument(tr("Unable to read Unique Identifier from database!").toStdString());
        }

        std::lock_guard<std::mutex> locker(db_mutex);
        leveldb::Status s;
        s = db_conn.db->Write(write_options, &batch);
        if (!s.ok()) {
            throw std::runtime_error(s.ToString());
        }
    } else {
        throw std::invalid_argument(tr("One of the given Unique IDs and/or values are empty!").toStdString());
    }

    return;
}

/**
 * @brief GkDbWrite::del_misc_key_vals Deletes a specified Unique Identifier for either a Licensee, Species, or
 * Name/ID sub-record within the Google LevelDB database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-13
 * @param record_type Whether to remove data from `store_licensee_id`, `store_species_id`, or `store_name_id` within
 * the Google LevelDB database.
 * @param record_id The Unique Identifier itself, usually a UUID in this case.
 */
void GkDbWrite::del_cat_key_vals(const GkRecords::MiscRecordType &record_type, const std::string &record_id)
{
    if (!record_id.empty()) {
        std::ostringstream oss;
        using namespace GkRecords;

        leveldb::WriteOptions write_options;
        write_options.sync = true;
        leveldb::WriteBatch batch;

        switch (record_type) {
            case MiscRecordType::gkLicensee:
            {
                auto licensee_cache = gkDbRead->get_cat_key_vals(MiscRecordType::gkLicensee);
                bool write_db = false;
                if (!licensee_cache.empty()) {
                    for (auto it = licensee_cache.begin(); it != licensee_cache.end(); ++it) {
                        if (it.key() != record_id) { // Write out all the key/value pairs except for the specified one, thus deleting it
                            oss << it.key() << "," << it.value() << std::endl;
                            write_db = true;
                        }
                    }

                    batch.Delete(LEVELDB_STORE_LICENSEE_ID);

                    if (write_db) { // There is at least one key/value pair that needs to be written back to the database
                        batch.Put(LEVELDB_STORE_LICENSEE_ID, oss.str());
                    }
                }
            }

                break;
            case MiscRecordType::gkSpecies:
            {
                auto species_cache = gkDbRead->get_cat_key_vals(MiscRecordType::gkSpecies);
                bool write_db = false;
                if (species_cache.empty()) {
                    for (auto it = species_cache.begin(); it != species_cache.end(); ++it) {
                        if (it.key() != record_id) { // Write out all the key/value pairs except for the specified one, thus deleting it
                            oss << it.key() << "," << it.value() << std::endl;
                            write_db = true;
                        }
                    }

                    batch.Delete(LEVELDB_STORE_SPECIES_ID);

                    if (write_db) { // There is at least one key/value pair that needs to be written back to the database
                        batch.Put(LEVELDB_STORE_SPECIES_ID, oss.str());
                    }
                }
            }

                break;
            case MiscRecordType::gkId:
            {
                auto id_cache = gkDbRead->get_cat_key_vals(MiscRecordType::gkId);
                bool write_db = false;
                if (!id_cache.empty()) {
                    for (auto it = id_cache.begin(); it != id_cache.end(); ++it) {
                        if (it.key() != record_id) { // Write out all the key/value pairs except for the specified one, thus deleting it
                            oss << it.key() << "," << it.value() << std::endl;
                            write_db = true;
                        }
                    }

                    batch.Delete(LEVELDB_STORE_NAME_ID);

                    if (write_db) { // There is at least one key/value pair that needs to be written back to the database
                        batch.Put(LEVELDB_STORE_NAME_ID, oss.str());
                    }
                }
            }

                break;
            default:
                throw std::invalid_argument(tr("Unable to read Unique Identifier from database!").toStdString());
        }

        std::lock_guard<std::mutex> locker(db_mutex);
        leveldb::Status s;
        s = db_conn.db->Write(write_options, &batch);
        if (!s.ok()) {
            throw std::runtime_error(s.ToString());
        }
    } else {
        throw std::invalid_argument(tr("One of the given Unique IDs are empty!").toStdString());
    }

    return;
}

/**
 * @brief GkDbWrite::add_uuid Adds a new Unique Identifier for the record in question to the Google LevelDB database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @param uuid The UUID tieing all the separate database entries/keys together.
 * @param licensee The licensee in regard to this record in question.
 * @param species The species of the animal/lizard in question.
 * @param id The identifier, for the animal/lizard in question.
 * @return Whether the process was a success or not.
 */
bool GkDbWrite::add_uuid(const std::string &uuid, const GkRecords::GkLicensee &licensee,
                         const GkRecords::GkSpecies &species, const GkRecords::GkId &id)
{
    try {
        using namespace GkRecords;
        std::ostringstream oss;
        auto record_cache = gkDbRead->get_uuids();
        bool licensee_existing = false;
        bool species_existing = false;
        bool animal_existing = false;
        if (!record_cache.empty()) {
            for (const auto &record: record_cache) {
                oss << record.first << "," << record.second.licensee_id << "," << record.second.species_id << ","
                    << record.second.name_id << std::endl;

                if (record.second.licensee_id == licensee.licensee_id) {
                    licensee_existing = true;
                }

                if (record.second.species_id == species.species_id) {
                    species_existing = true;
                }

                if (record.second.name_id == id.name_id) {
                    animal_existing = true;
                }
            }
        }

        if ((!uuid.empty()) && (!licensee.licensee_id.empty()) && (!species.species_id.empty()) &&
                (!id.name_id.empty())) {
            // Now we insert the new UUID alongside with its value
            oss << uuid << "," << licensee.licensee_id << "," << species.species_id << "," << id.name_id << std::endl;
        } else {
            throw std::invalid_argument(tr("One of the given UUIDs are empty!").toStdString());
        }

        if (!licensee.licensee_id.empty()) {
            if (!licensee_existing) { // Check that the `Licensee` does not already exist in the database
                // We have a new entry for the Licensee sub-record!
                add_cat_key_vals(MiscRecordType::gkLicensee, licensee.licensee_id, licensee.licensee_name);
            }
        }

        if (!species.species_id.empty()) {
            if (!species_existing) { // Check that the `Species` does not already exist in the database
                // We have a new entry for the Species sub-record!
                add_cat_key_vals(MiscRecordType::gkSpecies, species.species_id, species.species_name);
            }
        }

        if (!id.name_id.empty()) {
            if (!animal_existing) { // Check that the `Animal ID` does not already exist in the database
                // We have a new entry for the Name/ID# sub-record!
                add_cat_key_vals(MiscRecordType::gkId, id.name_id, id.identifier_str);
            }
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
    }

    return false;
}

/**
 * @brief GkDbWrite::del_uuid Deletes a specified Unique Identifier for the record in question from the Google LevelDB
 * database.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03
 * @param uuid The UUID tieing all the separate database entries/keys together.
 * @return Whether the operation was a success or not.
 */
bool GkDbWrite::del_uuid(const std::string &uuid)
{
    try {
        if (!uuid.empty()) {
            using namespace GkRecords;
            leveldb::WriteOptions write_options;
            write_options.sync = true;
            leveldb::WriteBatch batch;

            std::ostringstream oss;
            auto record_cache = gkDbRead->get_uuids();
            bool write_db = false;

            if (!record_cache.empty()) {
                for (const auto &record: record_cache) {
                    if (record.first != uuid) {
                        oss << record.first << "," << record.second.licensee_id << "," << record.second.species_id << ","
                            << record.second.name_id << std::endl;
                        write_db = true;
                    }
                }
            } else {
                throw std::runtime_error(tr("There are no entries to delete!").toStdString());
            }

            batch.Delete(LEVELDB_STORE_RECORD_ID);
            if (write_db) { // There is at least one key/value pair that needs to be written back to the database
                batch.Put(LEVELDB_STORE_RECORD_ID, oss.str());
            }

            std::lock_guard<std::mutex> locker(db_mutex);
            leveldb::Status s;
            s = db_conn.db->Write(write_options, &batch);
            if (!s.ok()) {
                throw std::runtime_error(s.ToString());
            }

            return true;
        } else {
            throw std::invalid_argument(tr("One of the given UUIDs are empty!").toStdString());
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return false;
}

/**
 * @brief GkDbWrite::mass_del_id Will delete any number of records (depending on what's associated) from the Google LevelDB
 * database in one, swift go.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-03-19
 * @param record_type Whether to delete data from `store_licensee_id`, `store_species_id`, and/or `store_name_id` from within
 * the Google LevelDB database.
 * @param record_id The Unique ID of the record in question.
 * @param recursive If records of another type should be deleted recursively as well, or not.
 * @return Whether the operation was a success or not.
 */
bool GkDbWrite::mass_del_cat(const GkRecords::MiscRecordType &record_type, const std::string &record_id,
                             const bool &recursive)
{
    try {
        if (!record_id.empty()) {
            using namespace GkRecords;
            switch (record_type) {
                case MiscRecordType::gkLicensee:
                {
                    if (recursive) {
                        del_cat_key_vals(MiscRecordType::gkLicensee, record_id);
                        return true;
                    } else {
                        // Must firstly determine what species and animals are associated with this licensee record
                        auto uuid_cache = gkDbRead->get_uuids();
                        QMap<std::string, std::pair<std::string, std::string>> unique_species_map; // <Key: UUID, Value: <License ID, Species ID>>
                        QMap<std::string, std::pair<std::string, std::string>> unique_animals_map; // <Key: UUID, Value: <Species ID, Animal ID>>

                        for (const auto &uuid: uuid_cache) {
                            if (uuid.second.licensee_id == record_id) {
                                if (!unique_species_map.contains(uuid.second.species_id)) {
                                    unique_species_map.insert(uuid.first, std::make_pair(uuid.second.licensee_id, uuid.second.species_id));
                                }

                                if (!unique_animals_map.contains(uuid.second.name_id)) {
                                    unique_animals_map.insert(uuid.first, std::make_pair(uuid.second.species_id, uuid.second.name_id));
                                }
                            }
                        }

                        GkCategories categories;
                        categories.spec_record_id = record_id;
                        categories.species_cache = unique_species_map;
                        categories.animals_cache = unique_animals_map;
                        bool ret = gkStrOp->del_cat_msg_box(categories, MiscRecordType::gkLicensee);

                        if (ret) {
                            QVector<std::string> unique_uuid_vec;
                            del_cat_key_vals(MiscRecordType::gkLicensee, record_id); // This deletes the `Licensee ID`

                            for (auto it = unique_species_map.begin(); it != unique_species_map.end(); ++it) {
                                mass_del_cat(MiscRecordType::gkSpecies, it.value().second, true);

                                if (!unique_uuid_vec.contains(it.key())) {
                                    unique_uuid_vec.push_back(it.key());
                                }
                            }

                            for (auto it = unique_animals_map.begin(); it != unique_animals_map.end(); ++it) {
                                mass_del_cat(MiscRecordType::gkId, it.value().second, true);

                                if (!unique_uuid_vec.contains(it.key())) {
                                    unique_uuid_vec.push_back(it.key());
                                }
                            }

                            for (const auto &uuid: unique_uuid_vec) {
                                // Now delete the main UUID itself!
                                del_uuid(uuid);
                            }
                        }
                    }
                }

                    return true;
                case MiscRecordType::gkSpecies:
                {
                    if (recursive) {
                        del_cat_key_vals(MiscRecordType::gkSpecies, record_id);
                        return true;
                    } else {
                        // We must determine what licensees and animals are associated with this species record.
                        auto uuid_cache = gkDbRead->get_uuids();
                        QMap<std::string, std::string> unique_licensees_map; // <Key: UUID, Value: Licensee ID>
                        QMap<std::string, std::pair<std::string, std::string>> unique_animals_map; // <Key: UUID, Value: <Species ID, Animal ID>>

                        for (const auto &uuid: uuid_cache) {
                            if (uuid.second.species_id == record_id) {
                                if (!unique_licensees_map.contains(uuid.first)) {
                                    unique_licensees_map.insert(uuid.first, uuid.second.licensee_id);
                                }

                                if (!unique_animals_map.contains(uuid.first)) {
                                    unique_animals_map.insert(uuid.first, std::make_pair(uuid.second.species_id,
                                                                                         uuid.second.name_id));
                                }
                            }
                        }

                        GkCategories categories;
                        categories.spec_record_id = record_id;
                        categories.licensee_cache = unique_licensees_map;
                        categories.animals_cache = unique_animals_map;
                        bool ret = gkStrOp->del_cat_msg_box(categories, MiscRecordType::gkSpecies);

                        if (ret) {
                            QVector<std::string> unique_uuid_vec;
                            QVector<std::string> unique_species_vec;

                            for (auto it = unique_licensees_map.begin(); it != unique_licensees_map.end(); ++it) {
                                mass_del_cat(MiscRecordType::gkLicensee, it.value(), true);

                                if (!unique_uuid_vec.contains(it.key())) {
                                    unique_uuid_vec.push_back(it.key());
                                }
                            }

                            for (auto it = unique_animals_map.begin(); it != unique_animals_map.end(); ++it) {
                                mass_del_cat(MiscRecordType::gkId, it.value().second, true);

                                if (!unique_uuid_vec.contains(it.key())) {
                                    unique_uuid_vec.push_back(it.key());
                                }

                                if (!unique_species_vec.contains(it.value().first)) {
                                    unique_species_vec.push_back(it.value().first);
                                }
                            }

                            for (const auto &species: unique_species_vec) {
                                del_cat_key_vals(MiscRecordType::gkSpecies, species); // This deletes the `Species ID`
                            }

                            for (const auto &uuid: unique_uuid_vec) {
                                // Now delete the main UUID itself!
                                del_uuid(uuid);
                            }
                        }
                    }
                }

                    return true;
                case MiscRecordType::gkId:
                {
                    if (recursive) {
                        del_cat_key_vals(MiscRecordType::gkId, record_id);
                        return true;
                    } else {
                        // We must determine what licensees and species are associated with this animal record.
                        auto uuid_cache = gkDbRead->get_uuids();
                        QMap<std::string, std::string> unique_licensees_map; // <Key: UUID, Value: License ID>
                        QMap<std::string, std::pair<std::string, std::string>> unique_species_map; // <Key: UUID, Value: <Species ID, Animal ID>>

                        for (const auto &uuid: uuid_cache) {
                            if (uuid.second.name_id == record_id) {
                                if (!unique_licensees_map.contains(uuid.first)) {
                                    unique_licensees_map.insert(uuid.first, uuid.second.licensee_id);
                                }

                                if (!unique_species_map.contains(uuid.first)) {
                                    unique_species_map.insert(uuid.first, std::make_pair(uuid.second.species_id,
                                                                                         uuid.second.name_id));
                                }
                            }
                        }

                        GkCategories categories;
                        categories.spec_record_id = record_id;
                        categories.licensee_cache = unique_licensees_map;
                        categories.species_cache = unique_species_map;
                        bool ret = gkStrOp->del_cat_msg_box(categories, MiscRecordType::gkId);

                        if (ret) {
                            QVector<std::string> unique_uuid_vec;
                            QVector<std::string> unique_animals_vec;

                            for (auto it = unique_licensees_map.begin(); it != unique_licensees_map.end(); ++it) {
                                mass_del_cat(MiscRecordType::gkLicensee, it.value(), true);

                                if (!unique_uuid_vec.contains(it.key())) {
                                    unique_uuid_vec.push_back(it.key());
                                }
                            }

                            for (auto it = unique_species_map.begin(); it != unique_species_map.end(); ++it) {
                                mass_del_cat(MiscRecordType::gkSpecies, it.value().first, true);

                                if (!unique_uuid_vec.contains(it.key())) {
                                    unique_uuid_vec.push_back(it.key());
                                }

                                if (!unique_animals_vec.contains(it.value().second)) {
                                    unique_animals_vec.push_back(it.value().second);
                                }
                            }

                            for (const auto &animals: unique_animals_vec) {
                                del_cat_key_vals(MiscRecordType::gkId, animals); // This deletes the `Animal ID`
                            }

                            for (const auto &uuid: unique_uuid_vec) {
                                // Now delete the main UUID itself!
                                del_uuid(uuid);
                            }
                        }
                    }
                }

                    return true;
                default:
                    throw std::runtime_error(tr("Unable to delete records from the database!").toStdString());
            }
        } else {
            throw std::invalid_argument(tr("One of the given Unique IDs are empty!").toStdString());
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error!"), e.what(), QMessageBox::Ok);
    }

    return false;
}

/**
 * @brief GkDbWrite::create_uuid Creates a UUID and then converts it to uppercase, ready for use. It is primarily used
 * for database record-keeping purposes throughout HerpLog.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12
 * @return The thusly created UUID.
 */
std::string GkDbWrite::create_uuid()
{
    // TODO: Check to see if UUID already exists in the database or not...
    boost::mt19937 ran;
    std::random_device rd;
    ran.seed(rd());
    boost::uuids::basic_random_generator<boost::mt19937> gen(&ran);
    boost::uuids::uuid u = gen();
    std::string result = boost::uuids::to_string(u); // Convert the Boost UUID to a std::string
    boost::to_upper(result); // Convert to uppercase
    return result;
}
