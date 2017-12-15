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
 * @file gk_db.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-12
 * @brief Contains any database-related routines, specifically ones related to Google LevelDB.
 */

#include "gk_db.hpp"
#include "gk_csv.hpp"
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/cache.h>
#include <boost/filesystem.hpp>
#include <boost/exception/all.hpp>
#include <boost/crc.hpp>
#include <zipper.h>
#include <unzipper.h>
#include <QMessageBox>
#include <memory>
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace GekkoFyre;
using namespace zipper;
namespace sys = boost::system;
namespace fs = boost::filesystem;

GkDb::GkDb(QObject *parent) : QObject(parent) {}

GkDb::~GkDb() {}

GkFile::FileDb GkDb::openDatabase(const std::string &dbFile)
{
    decompress_file(dbFile); // Decompress the archive firstly

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
            throw std::runtime_error(tr("Unable to open/create database! %1").arg(QString::fromStdString(s.ToString())).toStdString());
        }

        if (fs::exists(dbFile, ec) && fs::is_directory(dbFile) && !doesExist) {
            std::cout << tr("Database object created. Status: ").toStdString() << s.ToString() << std::endl;
        }
    }

    return db_struct;
}

/**
 * @note <https://github.com/sebastiandev/zipper>
 * @param filesList
 * @param saveFileAsLoc The location of where you wish to save the zipped file as.
 * @return Whether the operation was successful or not.
 */
bool GkDb::compress_files(const std::vector<std::string> &filesList, const std::string &saveFileAsLoc)
{
    std::stringstream csv_out;
    Zipper zipper(saveFileAsLoc);
    sys::error_code ec;

    try {
        const std::string dir_name = fs::path(saveFileAsLoc).filename().string();
        const std::string path_to_zip = std::string(fs::path(saveFileAsLoc).remove_filename().string() + fs::path::preferred_separator + dir_name);
        for (size_t i = 0; i < filesList.size(); ++i) {
            if (fs::exists(filesList.at(i), ec)) {
                std::string fileData = readFileToString(filesList.at(i));
                csv_out << filesList.at(i) << "," << getCrc32(fileData.data()) << "," << "CRC32" << std::endl;
            } else {
                QMessageBox::warning(nullptr, tr("Error!"), ec.message().c_str(), QMessageBox::Ok);
                return false;
            }
        }

        if (fs::create_directory(path_to_zip, ec)) {
            const std::string csv_file = fs::path(path_to_zip + fs::path::preferred_separator + GkFile::GkCsv::zip_contents).string();
            std::ofstream output;
            output.open(csv_file, std::ios::out | std::ios::app);
            output << csv_out.str();
            output.close();
        } else {
            QMessageBox::warning(nullptr, tr("Error!"), ec.message().c_str(), QMessageBox::Ok);
            return false;
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error!"), tr("An error has occurred during the saving of your database file.\n\n%1").arg(e.what()),
                             QMessageBox::Ok);
        return false;
    }

    return false;
}

/**
 * @note <https://github.com/sebastiandev/zipper>
 * @param fileLoc
 * @return Whether the operation was successful or not.
 */
bool GkDb::decompress_file(const std::string &fileLoc)
{
    Unzipper unzipper(fileLoc);
    std::vector<ZipEntry> entries = unzipper.entries();
    std::vector<unsigned char> unzipped_csv;
    unzipper.extractEntryToMemory(GkFile::GkCsv::zip_contents, unzipped_csv);

    GkCsvReader csv_reader(3, std::string(reinterpret_cast<const char *>(unzipped_csv.data())), GkFile::GkCsv::fileName, GkFile::GkCsv::fileHash, GkFile::GkCsv::hashType);
    std::string file, hash, hashType;
    while (csv_reader.read_row(file, hash, hashType)) {
        if (!file.empty() && !hash.empty() && !hashType.empty()) {
        }
    }

    unzipper.close();
    return false;
}

int GkDb::getCrc32(const std::string &fileData)
{
    boost::crc_32_type result;
    result.process_bytes(fileData.data(), fileData.length());
    return result.checksum();
}

/**
 * @brief GkDb::readFileToString reads a whole file, all at once, into a std::string() whether binary or not.
 * @author paxos1977 <https://stackoverflow.com/questions/116038/what-is-the-best-way-to-read-an-entire-file-into-a-stdstring-in-c>
 * @param fileLoc
 * @return
 */
std::string GkDb::readFileToString(const std::string &fileLoc)
{
    std::ifstream ifs(fileLoc, std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return std::string(bytes.data(), fileSize);
}

std::string GkDb::convHashType_toStr(const GkFile::HashTypes &hashType)
{
    switch (hashType) {
        case GkFile::HashTypes::CRC32:
            return GkFile::GkCsv::hashTypeCRC32;
        default:
            return std::string();
    }

    return std::string();
}
