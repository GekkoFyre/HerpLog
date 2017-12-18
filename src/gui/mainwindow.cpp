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
 * @file mainwindow.cpp
 * @author Phobos Aryn'dythyrn D'thorga <phobos.gekko@gmail.com>
 * @date 2017-12-15
 * @brief The main, opening window to the program.
 */

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "herpapp.hpp"
#include <boost/exception/all.hpp>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

namespace sys = boost::system;
namespace fs = boost::filesystem;
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    gkDb = std::make_unique<GkDb>(this);
}

MainWindow::~MainWindow()
{
    db_ptr.db.reset();
    delete ui;
}

void MainWindow::on_button_create_db_clicked()
{
    fs::path home_dir(QDir::homePath().toStdString());
    sys::error_code ec;
    try {
        if (fs::is_directory(home_dir, ec)) {
            QString saveFileName = QFileDialog::getSaveFileName(this, tr("Create Database"), QString::fromStdString(home_dir.string()), tr("HerpLog Database Files (*.hdb)"));
            if (!saveFileName.isEmpty()) {
                fs::path dirName = fs::path(saveFileName.toStdString()).filename();
                fs::path temp_dir = std::string(QDir::tempPath().toStdString() + fs::path::preferred_separator + dirName.string());
                db_ptr = gkDb->openDatabase(temp_dir.string());

                fs::path parent_path = fs::path(saveFileName.toStdString()).parent_path();
                fs::path zip_file = std::string(parent_path.string() + fs::path::preferred_separator + dirName.string() + "." + "hdb");
                gkDb->compress_files(temp_dir.string(), zip_file.string());
                tmp_db_loc = temp_dir;

                return;
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), tr("A problem was encountered whilst trying to open a database. Error:\n\n%1")
                .arg(e.what()), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_button_open_db_clicked()
{
    fs::path home_dir(QDir::homePath().toStdString());
    sys::error_code ec;
    try {
        if (fs::is_directory(home_dir, ec)) {
            QString fileName = QFileDialog::getOpenFileName(this, tr("Open Database"), QString::fromStdString(home_dir.string()), tr("HerpLog Database Files (*.hdb);;Any files (*.*)"));
            std::string fileName_str = fileName.toStdString();

            if (!fileName.isEmpty() && fs::exists(fileName_str, ec)) {
                std::string tmp_extraction_loc = gkDb->decompress_file(fileName_str);
                if (!tmp_extraction_loc.empty() && fs::is_directory(tmp_extraction_loc, ec)) {
                    db_ptr = gkDb->openDatabase(tmp_extraction_loc);

                    this->close();
                    HerpApp *herpAppWin = new HerpApp(db_ptr, tmp_extraction_loc, this);
                    herpAppWin->setWindowFlags(Qt::Window);
                    herpAppWin->setAttribute(Qt::WA_DeleteOnClose, true); // Delete itself on closing
                    QObject::connect(herpAppWin, SIGNAL(destroyed(QObject*)), this, SLOT(show()));
                    herpAppWin->show();

                    return;
                }
            }
        }

        if (ec.value() != sys::errc::errc_t::success) {
            QMessageBox::warning(this, tr("Error!"), QString::fromStdString(ec.message()), QMessageBox::Ok);
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error!"), tr("A problem was encountered whilst trying to open a database. Error:\n\n%1")
                .arg(e.what()), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_button_exit_clicked()
{
    QApplication::exit(0);
}
