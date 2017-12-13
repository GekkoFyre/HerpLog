#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button_create_new_db_clicked();
    void on_button_open_db_clicked();
    void on_button_exit_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HPP
