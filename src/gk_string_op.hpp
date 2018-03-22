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
 * @file gk_string_op.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @brief This file holds any functions related to string processing/modification.
 */

#ifndef GKSTRINGOP_HPP
#define GKSTRINGOP_HPP

#include "options.hpp"
#include <QtCore/QObject>
#include <initializer_list>
#include <string>
#include <QMap>

namespace GekkoFyre {
class GkStringOp;

class GkStringOp : public QObject {
    Q_OBJECT

public:
    explicit GkStringOp(QObject *parent = nullptr);
    ~GkStringOp();

    std::string random_hash();
    std::string multipart_key(const std::initializer_list<std::string> &args);
    bool del_cat_msg_box(const QMap<std::string, std::string> &cat_map_one,
                         const QMap<std::string, std::string> &cat_map_two, const std::string &record_id,
                         const GkRecords::MiscRecordType &record_type);
};
}

#endif // GKSTRINGOP_HPP