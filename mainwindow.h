#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QGraphicsScene>
#include <vector>
#include <QListWidgetItem>
#include <QDebug>
namespace Ui {
class MainWindow;
}

struct Point {
    double x;
    double y;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
        std::vector<Point> points;
        std::map<QString, std::vector<Point>> filesPoints; // 存储文件名及其对应的点

        QString rootfilepath;
        QString crrentfilepath;

        std::vector<Point> importPoints(const QString &filePath);
        void exportPoints();
        std::vector<Point> sortPoints(std::vector<Point> &points);
        void displayPoints(const std::vector<Point> &points);
};

#endif // MAINWINDOW_H
