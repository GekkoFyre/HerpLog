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
 * @file gk_db_read.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-28
 * @brief Contains any database-related routines, specifically ones related to Google LevelDB
 * and reading data from within.
 */

#ifndef GKDB_READ_HPP
#define GKDB_READ_HPP

#include "options.hpp"
#include "gk_string_op.hpp"
#include <QtCore/QObject>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <utility>
#include <unordered_map>

namespace GekkoFyre {
class GkDbRead;

class GkDbRead : public QObject {
    Q_OBJECT
public:
    explicit GkDbRead(const GkFile::FileDb &gk_db_conn, const std::shared_ptr<GkStringOp> &gk_str_op, QObject *parent = nullptr);
    ~GkDbRead();

    std::string read_item_db(const std::string &record_id, const std::string &key);

    int determineMinimumDate(const std::vector<std::string> &record_id);
    int determineMaximumDate(const std::vector<std::string> &record_id);
    std::unordered_map<std::string, std::pair<std::string, std::string>> get_record_ids();
    std::vector<std::string> extractRecords(const int &dateStart, const int &dateEnd);

private:
    std::shared_ptr<GkStringOp> gkStrOp;
    GkFile::FileDb db_conn;

    std::mutex db_mutex;
};
}

#endif // GKDB_READ_HPP