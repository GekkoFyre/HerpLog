###
 ##   Thank you for using the "HerpLog" notetaker, logger and needs-manager
 ##   for your herpetology management requirements. You are looking at the
 ##   source code to make the application work and as such, it will require
 ##   compiling with the appropriate tools.
 ##
 ##
 ##   Copyright (C) 2017. GekkoFyre.
 ##
 ##
 ##   HerpLog is free software: you can redistribute it and/or modify
 ##   it under the terms of the GNU General Public License as published by
 ##   the Free Software Foundation, either version 3 of the License, or
 ##   (at your option) any later version.
 ##
 ##   HerpLog is distributed in the hope that it will be useful,
 ##   but WITHOUT ANY WARRANTY; without even the implied warranty of
 ##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ##   GNU General Public License for more details.
 ##
 ##   You should have received a copy of the GNU General Public License
 ##   along with HerpLog.  If not, see <http://www.gnu.org/licenses/>.
 ##
 ##
 ##   The latest source code updates can be obtained from [ 1 ] below at your
 ##   leisure. A web-browser or the 'git' application may be required.
 ##
 ##   [ 1 ] - https://github.com/GekkoFyre/HerpLog
 ##
 #################################################################################

# Try to find 'snappy'
#
# SNAPPY_FOUND
# SNAPPY_INCLUDE_DIR
# SNAPPY_LIBRARIES

find_library(SNAPPY_LIBRARIES
    NAMES "snappy"
    HINTS "${SNAPPY_LOCATION}/lib" "${SNAPPY_LOCATION}/lib64" "${SNAPPY_LOCATION}/lib32"
    DOC "The main snappy compression library"
)

# -----------------------------------------------------
# SNAPPY Include Directories
# -----------------------------------------------------
find_path(SNAPPY_INCLUDE_DIR
    NAMES "snappy-c.h"
    HINTS "${SNAPPY_LOCATION}" "${SNAPPY_LOCATION}/include/snappy" "${SNAPPY_LOCATION}/include/*" "/usr/include/snappy" "/usr/include"
    DOC "The snappy include directory"
)

if(SNAPPY_INCLUDE_DIR)
    message(STATUS "snappy includes found in ${SNAPPY_INCLUDE_DIR}")
endif()

# -----------------------------------------------------
# Handle the QUIETLY and REQUIRED arguments and set SNAPPY_FOUND to TRUE if
# all listed variables are TRUE
# -----------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(snappy DEFAULT_MSG SNAPPY_LIBRARIES SNAPPY_INCLUDE_DIR)
mark_as_advanced(SNAPPY_INCLUDE_DIR SNAPPY_LIBRARIES)

