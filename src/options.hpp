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
 * @file options.h
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-12
 * @brief Contains options, enums, etc. for the entire program.
 */

#ifndef GKOPTIONS_HPP
#define GKOPTIONS_HPP

#include <boost/filesystem.hpp>
#include <leveldb/db.h>
#include <memory>
#include <string>

namespace fs = boost::filesystem;
namespace GekkoFyre {
    constexpr double FYREDL_DEFAULT_RESOLUTION_WIDTH = 1920.0;
    constexpr unsigned long LEVELDB_CFG_CACHE_SIZE = 32UL * 1024UL * 1024UL;

    namespace GkFile {
        struct path_leaf_string {
            std::string operator()(const fs::directory_entry &entry) const {
                return entry.path().leaf().string();
            }
        };

        struct FileDb {
            std::shared_ptr<leveldb::DB> db;
            leveldb::Options options;
        };

        namespace GkCsv {
            constexpr char fileName[] = "file_name";
            constexpr char fileHash[] = "file_hash";
            constexpr char hashType[] = "hash_type";
            constexpr char zip_contents_csv[] = "zip_contents.csv";

            constexpr char hashTypeCRC32[] = "CRC32";
        }

        enum HashTypes {
            CRC32
        };
    }
}

#endif // GKOPTIONS_HPP