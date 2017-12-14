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

# Try to find 'zipper'
#
# ZIPPER_FOUND
# ZIPPER_INCLUDE_DIR
# ZIPPER_LIBRARIES

find_library(ZIPPER_LIBRARIES
    NAMES "Zipper"
    HINTS "${ZIPPER_LOCATION}/lib" "${ZIPPER_LOCATION}/lib64" "${ZIPPER_LOCATION}/lib32"
    DOC "The main zipper compression library"
)

# -----------------------------------------------------
# ZIPPER Include Directories
# -----------------------------------------------------
find_path(ZIPPER_INCLUDE_DIR
    NAMES "zipper.h" "unzipper.h"
    HINTS "${ZIPPER_LOCATION}" "${ZIPPER_LOCATION}/include/zipper" "${ZIPPER_LOCATION}/include/*" "/usr/include/zipper" "/usr/include" "/usr/local/include/*"
    DOC "The zipper include directory"
)

if(ZIPPER_INCLUDE_DIR)
    message(STATUS "zipper includes found in ${ZIPPER_INCLUDE_DIR}")
endif()

# -----------------------------------------------------
# Handle the QUIETLY and REQUIRED arguments and set ZIPPER_FOUND to TRUE if
# all listed variables are TRUE
# -----------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(zipper DEFAULT_MSG ZIPPER_LIBRARIES ZIPPER_INCLUDE_DIR)
mark_as_advanced(ZIPPER_INCLUDE_DIR ZIPPER_LIBRARIES)

