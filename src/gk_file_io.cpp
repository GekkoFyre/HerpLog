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
 * @file gk_db_write.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @brief Contains any routines relating to file input/output operations.
 */

#include "gk_file_io.hpp"
#include "gk_csv.hpp"
#include <boost/exception/all.hpp>
#include <boost/crc.hpp>
#include <zipper.h>
#include <unzipper.h>
#include <QMessageBox>
#include <QDir>
#include <exception>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ios>

using namespace GekkoFyre;
using namespace zipper;
namespace sys = boost::system;
GkFileIo::GkFileIo(QObject *parent) : QObject(parent)
{}

GkFileIo::~GkFileIo()
{}

/**
 * @brief GkFileIo::compress_files will create a HerpLog Database File for you, out of a typical Google LevelDB database and
 * a CSV containing some information about the files within the archive itself, such as CRC32 Hashes.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-17
 * @note <https://github.com/sebastiandev/zipper>
 * @param folderLoc is the location of the folder to be compressed.
 * @param saveFileAsLoc The location of where you wish to save the compressed archive as.
 * @return Whether the operation was successful or not.
 */
bool GkFileIo::compress_files(const std::string &folderLoc, const std::string &saveFileAsLoc)
{
    std::stringstream csv_out;
    Zipper zipper(saveFileAsLoc);
    sys::error_code ec;

    try {
        const std::string dir_name = fs::path(saveFileAsLoc).filename().string();
        std::vector<std::string> dir_contents;
        read_directory(folderLoc, dir_contents);
        zipper.open();
        for (const auto &file: dir_contents) {
            fs::path file_path = std::string(folderLoc + fs::path::preferred_separator + file);
            if (fs::exists(file_path, ec)) { // Check that the file(s) to be compressed do exist still
                std::string fileData = readFileToString(file_path.string());
                csv_out << file << "," << getCrc32(fileData) << "," << "CRC32" << std::endl; // Create the CSV strings
                zipper.add(file_path.string());
            } else {
                QMessageBox::warning(nullptr, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
                return false;
            }
        }

        const std::string csv_file = fs::path(folderLoc + fs::path::preferred_separator + GkFile::GkCsv::zip_contents_csv).string();
        std::ofstream output;
        output.open(csv_file, std::ios::out | std::ios::app);
        output << csv_out.str();
        output.close();
        zipper.add(csv_file);
        zipper.close();
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error!"), tr("An error has occurred during the saving of your database file.\n\n%1").arg(e.what()),
                             QMessageBox::Ok);
        return false;
    }

    return false;
}

/**
 * @brief GkFileIo::decompress_file will decompress the given HerpLog Database File into a given directory location for you.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-17
 * @note <https://github.com/sebastiandev/zipper>
 * @param fileLoc The location to the file to be decompressed, on local storage.
 * @return The temporary location of where the files from the archive were decompressed.
 */
std::string GkFileIo::decompress_file(const std::string &fileLoc)
{
    Unzipper unzipper(fileLoc);
    std::vector<ZipEntry> entries = unzipper.entries();
    std::vector<unsigned char> unzipped_data_csv;
    unzipper.extractEntryToMemory(GkFile::GkCsv::zip_contents_csv, unzipped_data_csv);

    GkCsvReader csv_reader(3, std::string(reinterpret_cast<const char *>(unzipped_data_csv.data())), GkFile::GkCsv::fileName, GkFile::GkCsv::fileHash, GkFile::GkCsv::hashType);
    std::string csv_file_entry, csv_hash_entry, hashType;
    fs::path fileName = fs::path(fileLoc).filename();

    // Remove all file-extensions from the filename
    while(!fileName.extension().empty()) {
        fileName = fileName.stem();
    }

    std::string temp_dir = std::string(QDir::tempPath().toStdString() + fs::path::preferred_separator + fileName.string());
    unzipper.extract(temp_dir); // Extract the contents of the zip-file into a temporary directory
    while (csv_reader.read_row(csv_file_entry, csv_hash_entry, hashType)) { // Read out the CSV information
        if (!csv_file_entry.empty() && !csv_hash_entry.empty() && !hashType.empty()) { // Make sure the CSV strings are not empty, otherwise abort
            for (const auto &entry: entries) { // Read out the information contained within the zip-file datastream itself
                if (!entry.name.empty()) { // Make sure the filename is valid
                    const std::string cur_file = entry.name;
                    if (cur_file == csv_file_entry) {
                        // The full-path to the currently addressed, unzipped file
                        fs::path cur_file_full_path = std::string(temp_dir += fs::path::preferred_separator + cur_file);

                        sys::error_code ec;
                        if (fs::exists(cur_file_full_path, ec)) {
                            // The binary data of the currently addressed file within a std::string
                            std::string fileData = readFileToString(cur_file_full_path.string());

                            // The CRC32 Hash of the currently addressed file
                            std::string crc32 = getCrc32(fileData);
                            if (crc32 == csv_hash_entry) {
                                continue;
                            } else {
                                QMessageBox::warning(nullptr, tr("Error!"), tr("The database, \"%1\", appears to be corrupt. Aborting...")
                                        .arg(QString::fromStdString(fileName.string())), QMessageBox::Ok);
                                unzipper.close();
                                return nullptr;
                            }
                        } else {
                            QMessageBox::warning(nullptr, tr("Error!"), tr("A problem was encountered whilst opening your saved database. Error:\n\n%1")
                                    .arg(QString::fromStdString(ec.message())), QMessageBox::Ok);
                            unzipper.close();
                            return nullptr;
                        }
                    }
                }
            }
        }
    }

    unzipper.close();
    return temp_dir;
}

/**
 * @brief GkFileIo::readFileToString reads a whole file, all at once, into a std::string() whether binary or not.
 * @author paxos1977 <https://stackoverflow.com/questions/116038/what-is-the-best-way-to-read-an-entire-file-into-a-stdstring-in-c>
 * @param fileLoc The full-path to the file itself, on local storage.
 * @return The binary data of the given file, compartmentalized within a 'std::string()'.
 */
std::string GkFileIo::readFileToString(const std::string &fileLoc)
{
    std::ifstream ifs(fileLoc, std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return std::string(bytes.data(), fileSize);
}

/**
 * @brief GkFileIo::getCrc32 will calculate the CRC32 hash of any given file when granted the binary data to it.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-17
 * @note <http://www.boost.org/doc/libs/1_65_1/libs/crc/crc_example.cpp>
 * @param fileData The actual binary data of the file, for which the hash is to be calculated henceforth.
 * @return The CRC32 hash of the given file.
 */
std::string GkFileIo::getCrc32(const std::string &fileData)
{
    boost::crc_32_type result;
    result.process_bytes(fileData.data(), fileData.length());
    std::ostringstream oss;
    oss << std::hex << std::uppercase << result.checksum();
    return oss.str();
}

/**
 * @brief GkFileIo::read_directory reads the contents of a directory on the local storage into a STL vector.
 * @param dirLoc The location of the directory to be read.
 * @param output The contents of the directory.
 */
void GkFileIo::read_directory(const std::string &dirLoc, std::vector<std::string> &output)
{
    fs::path path(dirLoc);
    fs::directory_iterator start(path);
    fs::directory_iterator end;
    std::transform(start, end, std::back_inserter(output), GkFile::path_leaf_string());
}
