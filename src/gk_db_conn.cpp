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
 * @file gk_db_conn.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-12
 * @brief Contains any database-related routines, specifically ones related to Google LevelDB
 * and making a connection hereto.
 */

#include "gk_db_conn.hpp"
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/cache.h>
#include <boost/exception/all.hpp>
#include <boost/crc.hpp>
#include <zipper.h>
#include <unzipper.h>
#include <QMessageBox>
#include <QDir>
#include <ios>
#include <memory>
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace GekkoFyre;
using namespace zipper;
namespace sys = boost::system;

GkDbConn::GkDbConn(QObject *parent) : QObject(parent) {}

GkDbConn::~GkDbConn() {}

/**
 * @brief GkDb::open_database creates a database connection within the applications memory from the database files on
 * the local storage of the users computer, ready to be used for inserting/modifying/deleting records.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-17
 * @param dbFile The location of the database files on the local storage of the users computer.
 * @return A pointer to the connection opened within memory with regard to the database.
 */
GkFile::FileDb GkDbConn::open_database(const std::string &dbFile)
{
    leveldb::Status s;
    GkFile::FileDb db_struct;
    db_struct.options.create_if_missing = true;
    std::shared_ptr<leveldb::Cache>(db_struct.options.block_cache).reset(leveldb::NewLRUCache(LEVELDB_CFG_CACHE_SIZE));
    db_struct.options.compression = leveldb::CompressionType::kSnappyCompression;
    if (!dbFile.empty()) {
        sys::error_code ec;
        bool doesExist;
        doesExist = !fs::exists(dbFile, ec) ? false : true;

        leveldb::DB *raw_db_ptr;
        s = leveldb::DB::Open(db_struct.options, dbFile, &raw_db_ptr);
        db_struct.db.reset(raw_db_ptr);
        if (!s.ok()) {
            throw std::runtime_error(s.ToString());
        }

        if (fs::exists(dbFile, ec) && fs::is_directory(dbFile) && !doesExist) {
            std::cout << tr("Database object created. Status: ").toStdString() << s.ToString() << std::endl;
        }
    }

    return db_struct;
}
