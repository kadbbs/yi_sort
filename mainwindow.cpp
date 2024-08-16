#include "mainwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QGraphicsEllipseItem>
#include <algorithm>
#include <cmath>
#include <QFileInfo>

Point calculateCentroid(const std::vector<Point>& points) {
    double sumX = 0.0, sumY = 0.0;
    int n = points.size();

    for (const auto& point : points) {
        sumX += point.x;
        sumY += point.y;
    }

    Point centroid;
    centroid.x = sumX / n;
    centroid.y = sumY / n;

    return centroid;
}

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

    points = importPoints(filePath);  // 导入点

    qDebug()<<"displayPoints"<<"\n";

    displayPoints(points);
    //画出中心
    scene->addEllipse(centre.x, -centre.y, 1, 1, QPen(Qt::red), QBrush(Qt::red));

#if 0

    if (filesPoints.find(filePath) != filesPoints.end()) {
        points = filesPoints[filePath];
        points = importPoints(filePath);  // 导入点

        qDebug()<<"displayPoints"<<"\n";

        displayPoints(points);
    }
#endif
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

    Point frontEdge = *std::min_element(points.begin(), points.end(), [](const Point &a, const Point &b) {
        return a.x > b.x;
    });

//    centre.x=(rearEdge.x+frontEdge.x)/2;
//    centre.y=(rearEdge.y+frontEdge.y)/2;
    centre = calculateCentroid(points);


#if 0
    //构造待聚类数据集
    std::vector< std::vector<double> > data;
    for (int i = 0; i < points.size(); i++)
    {
        data.push_back(points[i]);
    }

    //构建聚类算法
    KMEANS<double> kmeans;
    //数据加载入算法
    kmeans.loadData(data);
    //运行k均值聚类算法
    kmeans.kmeans(1);



#endif

    Point centre_;
    centre_.x=centre.x;
    centre_.y=centre.y;


    // 按角度排序，从后缘点顺时针
    std::sort(points.begin(), points.end(), [centre_](const Point &a, const Point &b) {
        double angleA = atan2(a.y - centre_.y, a.x - centre_.x);
        double angleB = atan2(b.y - centre_.y, b.x - centre_.x);
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
                scene->addLine(points[i].x, -points[i].y, points[i+1].x, -points[i+1].y, QPen(Qt::blue,0.5));
            }
            // 最后一个点与第一个点相连
            scene->addLine(points.back().x, -points.back().y, points.front().x, -points.front().y, QPen(Qt::blue,0.5));
        }

        ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}


//Point findo(std::vector<Point> &points){
//    for(int i=0;i<points.size();i++){
//        for(int j=i+1;j<points.size();j++){
//            double d=sqrt(pow((points[i].x-points[j].x),2)+pow((points[i].y-points[j].y),2));
//            // ptopd_map[std::make_pair(points[i],points[j])]=sqrt(pow((points[i].x-points[j].x),2)+pow((points[i].y-points[j].y),2));
//            ptopd_map[d]=std::make_pair(points[i],points[j]);
//            ptopd.push_back(d);
//        }
//    }

//    sort(ptopd.begin(),ptopd.end(),[](double a,double b){
//        return a>b;
//    });

//    point p;
//    p.x=(ptopd_map[ptopd[0]].first.x+ptopd_map[ptopd[0]].second.x)/2;
//    p.y=(ptopd_map[ptopd[0]].first.y+ptopd_map[ptopd[0]].second.y)/2;


//    return p;
//}


