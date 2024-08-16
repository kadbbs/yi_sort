#include "mainwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QGraphicsEllipseItem>
#include <algorithm>
#include <cmath>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
      scene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_listWidget_itemClicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

#if 0

void MainWindow::on_pushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "All Files (*.*)");

    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        rootfilepath=fileInfo.absolutePath();

        points = importPoints(filePath);
//        displayPoints(points);
        filesPoints[filePath]=points;


        // 添加文件名到列表中
        ui->listWidget->addItem(fileInfo.fileName());

        // 自动选择并显示该文件内容
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);


    }


}


#endif

void MainWindow::on_pushButton_clicked()
{
    // 允许用户选择多个文件
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "Open Files", "", "All Files (*.*)");

    if (!filePaths.isEmpty()) {
        foreach (const QString &filePath, filePaths) {
            QFileInfo fileInfo(filePath);
            rootfilepath = fileInfo.absolutePath();  // 获取文件的路径

            points = importPoints(filePath);  // 导入点
            filesPoints[filePath] = points;   // 保存文件和对应的点信息

            // 将文件名添加到列表中
            ui->listWidget->addItem(fileInfo.fileName());
        }

        // 自动选择并显示第一个文件内容
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (!points.empty()) {
        points = sortPoints(points);
        displayPoints(points);
        exportPoints();
    }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString filePath = item->text();
    filePath=rootfilepath+"/"+filePath;
    crrentfilepath=filePath;

    qDebug()<<"filePath: "<<filePath<<"\n";

    if (filesPoints.find(filePath) != filesPoints.end()) {
        points = filesPoints[filePath];
        points = importPoints(filePath);  // 导入点

        qDebug()<<"displayPoints"<<"\n";

        displayPoints(points);
    }
    qDebug()<<"on_listWidget_itemClicked"<<"\n";
}

std::vector<Point> MainWindow::importPoints(const QString &filePath)
{
    std::vector<Point> points;
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
//            QStringList coords = line.split(",");
            QStringList coords = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

            if (coords.size() == 2) {
                Point pt;
                pt.x = coords[0].toDouble();
                pt.y = coords[1].toDouble();
                points.push_back(pt);
            }
        }
        file.close();
    }
    return points;

}

void MainWindow::exportPoints()
{
    QString newFilePath = crrentfilepath;

    int dotIndex = crrentfilepath.lastIndexOf('.'); // 找到最后一个 '.' 的位置
    if (dotIndex != -1) {
        newFilePath.insert(dotIndex, "_sort"); // 在 '.' 之前插入 '_sort'
    } else {
        // 如果文件名中没有 '.'，返回原始路径
        newFilePath+="_sort";
    }


    QFile file(newFilePath);

    // 尝试打开文件，使用写入模式 (WriteOnly)
    if (file.open(QIODevice::WriteOnly | QIODevice::Text|QIODevice::NewOnly)) {
        QTextStream out(&file);

        // 遍历 points 并写入文件
        for (const Point &point : points) {
            out << point.x << " " << point.y << "\n";
        }

        file.close();
        qDebug() << "Points successfully exported to" << newFilePath;
    } else {
        qDebug() << "Failed to open file for writing:" << newFilePath;
    }

    // 添加文件名到列表中
            QFileInfo fileInfo(newFilePath);

    ui->listWidget->addItem(fileInfo.fileName());

    // 自动选择并显示该文件内容
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
}

std::vector<Point> MainWindow::sortPoints(std::vector<Point> &points)
{
    // 找到后缘点（X值最小的点）
    Point rearEdge = *std::min_element(points.begin(), points.end(), [](const Point &a, const Point &b) {
        return a.x < b.x;
    });

    // 按角度排序，从后缘点顺时针
    std::sort(points.begin(), points.end(), [&rearEdge](const Point &a, const Point &b) {
        double angleA = atan2(a.y - rearEdge.y, a.x - rearEdge.x);
        double angleB = atan2(b.y - rearEdge.y, b.x - rearEdge.x);
        return angleA < angleB;
    });

    return points;
}

void MainWindow::displayPoints(const std::vector<Point> &points)
{
        scene->clear();
        for (const Point &pt : points) {
            scene->addEllipse(pt.x, -pt.y, 1, 1, QPen(Qt::black), QBrush(Qt::black));
        }

        // 连接点以形成翼型轮廓
        if (points.size() > 1) {
            for (size_t i = 0; i < points.size() - 1; ++i) {
                scene->addLine(points[i].x, -points[i].y, points[i+1].x, -points[i+1].y, QPen(Qt::blue));
            }
            // 最后一个点与第一个点相连
            scene->addLine(points.back().x, -points.back().y, points.front().x, -points.front().y, QPen(Qt::blue));
        }

        ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}


