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
#include <chrono>

namespace fs = boost::filesystem;
namespace GekkoFyre {
    constexpr double HERPLOG_DEFAULT_RESOLUTION_WIDTH = 1920.0;
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
            constexpr char zip_contents_csv[] = "zip_contents.csv";
        }

        enum HashTypes {
            CRC32
        };
    }

    namespace GkRecords {
        constexpr char dateTime[] = "date_time";
        constexpr char furtherNotes[] = "further_notes";
        constexpr char vitaminNotes[] = "vitamin_notes";
        constexpr char toiletNotes[] = "toilet_notes";
        constexpr char weightNotes[] = "weight_notes";
        constexpr char hydrationNotes[] = "hydration_notes";
        constexpr char boolWentToilet[] = "bool_went_toilet";
        constexpr char boolHadHydration[] = "bool_had_hydration";
        constexpr char boolHadVitamins[] = "bool_had_vitamins";
        constexpr char weightMeasure[] = "weight_measurement";

        constexpr char LEVELDB_STORE_RECORD_ID[] = "store_unique_id";
        constexpr char LEVELDB_STORE_SPECIES_ID[] = "store_species_id";
        constexpr char LEVELDB_STORE_NAME_ID[] = "store_name_id";

        struct GkSpecies {
            std::string species_id;         // The species Unique ID
            std::string species_name;       // The species of the animal/lizard in question
        };

        struct GkId {
            std::string name_id;            // Self-explanatory
            std::string identifier_str;     // The identifier as a string, for the animal/lizard in question
        };

        struct GkSubmit {
            std::string record_id;          // The Unique Identifier for the entire record in question
            std::time_t date_time;          // The epoch at the time of submitting/modifying this record
            GkSpecies species;              // The species of the animal/lizard in question
            GkId identifier;                // The identifier as a string, for the animal/lizard in question
            std::string further_notes;      // Any further notes, usually as one, long std::string
            std::string vitamin_notes;      // Any extra notes about the lizard's vitamin intake
            std::string toilet_notes;       // Any extra notes about the lizard's bathroom habits
            std::string weight_notes;       // Any extra notes about the lizard's weight
            std::string hydration_notes;    // Any extra notes about the lizard's hydration intake
            bool went_toilet;               // Whether the lizard went to the bathroom or not
            bool had_hydration;             // Whether the lizard had any hydration or not
            bool had_vitamins;              // Whether the lizard had any vitamins or not
            double weight;                  // The weight of the lizard in question
        };

        enum StrucType {
            gkSpecies,
            gkId,
            gkSubmit
        };

        namespace GkGraph {
            struct WeightVsTime {
                std::string record_id;
                GkSpecies species;
                GkId identifier;
                double weight;
            };
        }
    }
}

#endif // GKOPTIONS_HPP