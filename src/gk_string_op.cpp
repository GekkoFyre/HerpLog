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
 * @file gk_string_op.cpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @brief This file holds any functions related to string processing/modification.
 */

#include "gk_string_op.hpp"
#include <boost/exception/all.hpp>
#include <boost/crc.hpp>

using namespace GekkoFyre;
GkStringOp::GkStringOp(QObject *parent) : QObject(parent)
{}

GkStringOp::~GkStringOp()
{}

/**
 * @brief GkStringOp::getCrc32 will calculate the CRC32 hash of any given string.
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2018-02-21
 * @note <http://www.boost.org/doc/libs/1_65_1/libs/crc/crc_example.cpp>
 * @param input The string in question, for which the hash is to be calculated henceforth.
 * @return The CRC32 hash of the given input.
 */
std::string GkStringOp::getCrc32(const std::string &input)
{
    boost::crc_32_type result;
    result.process_bytes(input.data(), input.length());
    std::ostringstream oss;
    oss << std::hex << std::uppercase << result.checksum();
    return oss.str();
}
