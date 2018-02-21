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
 * @file gk_db_write.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @brief Contains any database-related routines, specifically ones related to Google LevelDB
 * and writing data towards.
 */

#ifndef GKDB_WRITE_HPP
#define GKDB_WRITE_HPP

#include "options.hpp"
#include "gk_string_op.hpp"
#include <QtCore/QObject>
#include <string>
#include <vector>
#include <mutex>

namespace GekkoFyre {
class GkDb;

class GkDb : public QObject {
    Q_OBJECT

public:
    explicit GkDb(const GkFile::FileDb &database, const std::shared_ptr<GkStringOp> &gk_str_op, QObject *parent = nullptr);
    ~GkDb();

    void add_item_db(const std::string &record_id, const std::string &key, std::string value);
    void del_item_db(const std::string &record_id, const std::string &key);
    std::string read_item_db(const std::string &record_id, const std::string &key);

    auto get_misc_key_vals(const GkRecords::StrucType &struc_type);
    auto get_record_ids();
    int incr_id(const std::string &record_id, const std::string &key);

private:
    std::shared_ptr<GkStringOp> gkStrOp;
    GkFile::FileDb db_conn;

    std::mutex db_mutex;
};
}

#endif // GKDB_WRITE_HPP