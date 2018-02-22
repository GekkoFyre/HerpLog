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

#ifndef GK_FILE_IO_HPP
#define GK_FILE_IO_HPP

#include "options.hpp"
#include <QObject>
#include <boost/filesystem.hpp>

namespace GekkoFyre {
class GkFileIo;

class GkFileIo : public QObject {
    Q_OBJECT

public:
    explicit GkFileIo(QObject *parent = nullptr);
    ~GkFileIo();

    bool compress_files(const std::string &folderLoc, const std::string &saveFileAsLoc);
    std::string decompress_file(const std::string &fileLoc);

private:
    std::string readFileToString(const std::string &fileLoc);
    std::string getCrc32(const std::string &fileData);
    void read_directory(const std::string &dirLoc, std::vector<std::string> &output);
};
}

#endif // GK_FILE_IO_HPP