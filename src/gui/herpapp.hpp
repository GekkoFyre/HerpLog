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
 * @file herpapp.hpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-18
 * @brief The primary interface to the HerpLog applications' workings.
 */

#ifndef HERPAPP_HPP
#define HERPAPP_HPP

#include "./../options.hpp"
#include "./../gk_db_write.hpp"
#include "./../gk_db_read.hpp"
#include "./../gk_string_op.hpp"
#include "./../gk_file_io.hpp"
#include <boost/filesystem.hpp>
#include <QObject>
#include <QMainWindow>
#include <QResizeEvent>
#include <QPointer>
#include <QtCharts>
#include <QMultiMap>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <list>

using namespace GekkoFyre;
namespace fs = boost::filesystem;
namespace Ui {
class HerpApp;
}

class HerpApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit HerpApp(const GkFile::FileDb &database, const std::string &temp_dir_path, const std::string &db_file_path,
                     const std::shared_ptr<GkFileIo> &file_io_ptr, QWidget *parent = nullptr);
    ~HerpApp();

private slots:
    void on_action_New_Database_triggered();
    void on_action_Open_Database_triggered();
    void on_action_Disconnect_triggered();
    void on_action_Save_triggered();
    void on_actionSave_As_triggered();
    void on_actionSave_A_ll_triggered();
    void on_action_Print_triggered();
    void on_actionE_xit_triggered();
    void on_action_Undo_triggered();
    void on_action_Redo_triggered();
    void on_actionCu_t_triggered();
    void on_action_Copy_triggered();
    void on_action_Paste_triggered();
    void on_actionF_ind_triggered();
    void on_action_Settings_triggered();
    void on_action_Documentation_triggered();
    void on_action_About_triggered();
    void on_pushButton_archive_next_clicked();
    void on_pushButton_archive_prev_clicked();
    void on_pushButton_archive_delete_clicked();
    void on_pushButton_browse_submit_clicked();
    void on_pushButton_add_data_clicked();
    void on_toolButton_new_hash_clicked();
    void on_toolButton_add_record_update_datetime_clicked();
    void on_toolButton_records_calendar_popup_clicked();
    void on_comboBox_view_charts_select_licensee_currentIndexChanged(int index);
    void on_comboBox_view_charts_select_species_currentIndexChanged(int index);
    void on_comboBox_view_charts_select_id_currentIndexChanged(int index);
    void on_comboBox_existing_license_id_currentIndexChanged(int index);
    void on_comboBox_existing_species_currentIndexChanged(int index);
    void on_comboBox_view_records_licensee_currentIndexChanged(int index);
    void on_comboBox_view_records_species_currentIndexChanged(int index);
    void on_comboBox_view_records_animal_name_currentIndexChanged(int index);

private:
    Ui::HerpApp *ui;

    bool remove_files(const fs::path &tmpDirLoc);
    void refresh_caches();
    void find_date_ranges();

    std::string find_licensee_id(const int &index_no);
    std::string find_species_id();
    std::list<GkRecords::GkSpecies> find_species_names(const GkRecords::comboBoxType &dropbox_type, const std::string &licensee_id);
    std::list<GkRecords::GkId> find_animal_names(const GkRecords::comboBoxType &dropbox_type, const std::string &species_id);
    void record_species_index(const std::list<GkRecords::GkSpecies> &species_list, const GkRecords::comboBoxType &comboBox_type);
    void record_animals_index(const std::list<GkRecords::GkId> &animals_list, const GkRecords::comboBoxType &comboBox_type);

    bool submit_record();
    std::string browse_records(const std::list<std::string> &records, const bool &forward);
    void archive_clear_forms();
    void archive_fill_form_data(const std::string &record_id);

    void insert_charts();
    void update_charts(const std::string &name_id);

    GkFile::FileDb db_ptr;
    std::unique_ptr<GkDbWrite> gkDbWrite;
    std::shared_ptr<GkDbRead> gkDbRead;
    std::shared_ptr<GkStringOp> gkStrOp;
    std::shared_ptr<GkFileIo> gkFileIo;

    fs::path global_db_temp_dir;
    std::string global_db_file_path;
    std::mutex r_browse_mtx;
    std::mutex r_cache_mtx;
    std::mutex r_charts_mtx;

    long int minDateTime;
    long int maxDateTime;

    std::unordered_map<std::string, GkRecords::MiscUniqueIds> record_id_cache;
    std::list<std::string> archive_records;
    QMultiMap<std::string, std::pair<std::string, int>> licensee_cache; // <Key: Licensee ID, Value: <Licensee Name, Index No.>>
    QMultiMap<std::string, std::pair<std::string, int>> species_cache; // <Key: Licensee ID, Value: <Species ID, Index No.>>
    QMultiMap<std::string, std::pair<std::string, int>> animal_cache; // <Key: Species ID, Value <Animal ID, Index No.>>
    // Records are added to `viewed_records` as the `Next Record` button is pressed, and removed as
    // the `Previous Record` button is pressed.
    std::list<std::string> viewed_records;

    // Cached values for the comboBox selections
    QMultiMap<GkRecords::comboBoxType, GkRecords::GkSpecies> comboBox_species;
    QMultiMap<GkRecords::comboBoxType, GkRecords::GkId> comboBox_animals;

    QMultiMap<long int, GkRecords::GkGraph::WeightVsTime> weight_measurements; // The key is QDateTime
    QPointer<QLineSeries> line_series_weight;
    QPointer<QChart> chart_weight;
    bool charts_tab_enabled;
    bool caches_enabled;
};

#endif // HERPAPP_HPP
