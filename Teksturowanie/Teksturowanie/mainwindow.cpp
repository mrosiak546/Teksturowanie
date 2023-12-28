#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>
#include <QPainter>
#include <QQueue>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    im_source = QImage(ui->im_source->width(), ui->im_source->height(), QImage::Format_RGB32);
    im_destination = QImage(ui->im_destination->width(), ui->im_destination->height(), QImage::Format_RGB32);
    draw();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.drawImage(ui->im_source->x(), ui->im_source->y(), im_source);
    p.drawImage(ui->im_destination->x(), ui->im_destination->y(), im_destination);
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
    QPoint sourceTopLeft = ui->im_source->pos();
    QPoint destinationTopLeft = ui->im_destination->pos();
    int sourceWidth = ui->im_source->width();
    int sourceHeight = ui->im_source->height();
    int destinationWidth = ui->im_destination->width();
    int destinationHeight = ui->im_destination->height();


//Zabezpieczenie przed klikniecie punktu poza ekranem
    if (e->pos().x() >= sourceTopLeft.x() && e->pos().x() <= sourceTopLeft.x() + sourceWidth &&
        e->pos().y() >= sourceTopLeft.y() && e->pos().y() <= sourceTopLeft.y() + sourceHeight) {
        startPos = e->pos() - sourceTopLeft;
        im_source_clicked=true;
        im_destination_clicked=false;
    }
    else if (e->pos().x() >= destinationTopLeft.x() && e->pos().x() <= destinationTopLeft.x() + destinationWidth &&
             e->pos().y() >= destinationTopLeft.y() && e->pos().y() <= destinationTopLeft.y() + destinationHeight) {
        startPos = e->pos() - destinationTopLeft;
        im_source_clicked=false;
        im_destination_clicked=true;
    }


    if (e->button() == Qt::LeftButton) {
        if(im_source_clicked){
            if(sourcePoints.size()<3) sourcePoints.append(startPos);
        }
        if(im_destination_clicked){
            if(destinationPoints.size()<3) destinationPoints.append(startPos);
        }
    }
    else if (e->buttons() & Qt::RightButton){
        if(im_source_clicked){
            iSourcePoints = findPoints(startPos, sourcePoints);
        }
        if(im_destination_clicked){
            iDestinationPoints = findPoints(startPos, destinationPoints);
        }
    }

    draw();
    QMainWindow::mousePressEvent(e);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e){
    QPoint sourceTopLeft = ui->im_source->pos();
    QPoint destinationTopLeft = ui->im_destination->pos();
    int sourceWidth = ui->im_source->width();
    int sourceHeight = ui->im_source->height();
    int destinationWidth = ui->im_destination->width();
    int destinationHeight = ui->im_destination->height();

    if (e->pos().x() >= sourceTopLeft.x() && e->pos().x() <= sourceTopLeft.x() + sourceWidth &&
        e->pos().y() >= sourceTopLeft.y() && e->pos().y() <= sourceTopLeft.y() + sourceHeight) {
        endPos = e->pos() - sourceTopLeft;
        im_source_clicked=true;
        im_destination_clicked=false;
    }
    else if (e->pos().x() >= destinationTopLeft.x() && e->pos().x() <= destinationTopLeft.x() + destinationWidth &&
             e->pos().y() >= destinationTopLeft.y() && e->pos().y() <= destinationTopLeft.y() + destinationHeight) {
        endPos = e->pos() - destinationTopLeft;
        im_source_clicked=false;
        im_destination_clicked=true;
    }

    if (e->buttons() & Qt::RightButton){
        if(im_source_clicked && iSourcePoints!=-1){
            sourcePoints.replace(iSourcePoints, endPos);
            draw();
        }
        if(im_destination_clicked && iDestinationPoints!=-1){
            destinationPoints.replace(iDestinationPoints, endPos);
            draw();
        }
    }
}

void MainWindow::drawPixel(QImage *im, QPoint p, QColor color){
    if(p.x()<im->width() && p.x()>=0 && p.y()<im->height() && p.y()>=0){
        uchar *pix = im->scanLine(p.y())+4*p.x();
        pix[0]=color.blue();
        pix[1]=color.green();
        pix[2]=color.red();
    }
}

void MainWindow::drawLine(QImage *im, QPoint start, QPoint end, QColor color){
    int x, y;
    if ((start.x()==end.x()) && (start.y()==end.y())){
        drawPixel(im, start, color);
        return;
    }
    else if(abs(end.y()-start.y()) <= abs(end.x()-start.x())){
        if (start.x() > end.x()){
            QPoint temp=start;
            start=end;
            end=temp;
        }
        for(x=start.x(); x<=end.x(); x++){
            y=start.y()+(x-start.x())*(end.y()-start.y())/(end.x()-start.x());
            drawPixel(im, {x, y}, color);
        }
    }
    else{
        if (start.y() > end.y()){
            QPoint temp=start;
            start=end;
            end=temp;
        }
        for(y=start.y(); y<=end.y(); y++){
            x=start.x()+(y-start.y())*(end.x()-start.x())/(end.y()-start.y());
            drawPixel(im, {x, y}, color);
        }
    }
}

void MainWindow::drawCircle(QImage *im, QPoint start, QPoint end, QColor color){
    int R = sqrt(((end.x()-start.x())*(end.x()-start.x()))+((end.y()-start.y())*(end.y()-start.y())));
    for(int i=-R; i<=R; i++){
        double y = sqrt(R*R - i*i);
        drawPixel(im, {i+start.x(),(int)floor(y+0.5)+start.y()},    color);
        drawPixel(im, {i+start.x(),-(int)floor(y+0.5)+start.y()},   color);
        drawPixel(im, {-i+start.x(),(int)floor(y+0.5)+start.y()},   color);
        drawPixel(im, {-i+start.x(),-(int)floor(y+0.5)+start.y()},  color);
        drawPixel(im, {(int)floor(y+0.5)+start.x(),i+start.y()},    color);
        drawPixel(im, {-(int)floor(y+0.5)+start.x(),i+start.y()},   color);
        drawPixel(im, {(int)floor(y+0.5)+start.x(),-i+start.y()},   color);
        drawPixel(im, {-(int)floor(y+0.5)+start.x(),-i+start.y()},  color);
    }
}

void MainWindow::draw()
{
    im_source.load("C:/Users/rosia/OneDrive/Pulpit/Studia/Grafika Komputerowa/Teksturowanie/Teksturowanie/image.png");
    im_destination.fill(0);

    if(sourcePoints.size()==3 && destinationPoints.size()==3)
        fill();

    drawTriangle(&im_source, sourcePoints);
    drawTriangle(&im_destination, destinationPoints);


    update();
}

void MainWindow::drawTriangle(QImage *im, QQueue<QPoint> points)
{
    for(int i=0; i<points.size(); i++){
        drawCircle(im, points[i], {points[i].x(), points[i].y()+4}, Qt::red);
        if(points.size()==3)
            drawLine(im, points[i], points[(i+1)%3], Qt::white);
    }
}

void MainWindow::fill(){


    if(destinationPoints.size()<2) return;
    int yMax=destinationPoints[0].y(), yMin=destinationPoints[0].y();
    for(int i=0; i<destinationPoints.size(); i++){
        if(destinationPoints[i].y()>yMax) yMax=destinationPoints[i].y();
        if(destinationPoints[i].y()<yMin) yMin=destinationPoints[i].y();
    }

    QVector<int> X;

    for(int y=yMin; y<=yMax; y++){
        for(int i=0; i<destinationPoints.size(); i++){
            int p1=i, p2=(i+1)%destinationPoints.size();

            if(destinationPoints[p1].y()>destinationPoints[p2].y()) std::swap(p1, p2);

            if (y >= destinationPoints[p1].y() && y < destinationPoints[p2].y() && destinationPoints[p1].y() != destinationPoints[p2].y()){
                X.push_back(destinationPoints[p1].x()+(y-destinationPoints[p1].y())*(destinationPoints[p2].x()-destinationPoints[p1].x())/(destinationPoints[p2].y()-destinationPoints[p1].y()));
            }
        }
        std::sort(X.begin(), X.end());
        if(X.size()>1){
            for(int x=X[0]; x<X[1]; x++){
                double W, Wv, Ww, u, v, w;
                double xt, yt;
                QPoint a=destinationPoints[0], b=destinationPoints[1], c=destinationPoints[2];
                QPoint at=sourcePoints[0], bt=sourcePoints[1], ct=sourcePoints[2];

                W = (b.x() - a.x())*(c.y() - a.y()) - (b.y() - a.y())*(c.x() - a.x());
                Wv = ((double)x - a.x())*(c.y() - a.y()) - ((double)y - a.y())*(c.x() - a.x());
                Ww = (b.x() - a.x())*((double)y - a.y()) - (b.y() - a.y())*((double)x - a.x());
                v=Wv/W;
                w=Ww/W;
                u=1-v-w;
                xt=u*at.x() + v*bt.x() + w*ct.x();
                yt=u*at.y() + v*bt.y() + w*ct.y();

                im_destination.setPixel(x, y, im_source.pixel(xt, yt));
            }
        }

        X.clear();
    }
}


int MainWindow::findPoints(QPoint p, QQueue<QPoint> points){
    int minDist = -1;
    int n = -1;
    for (int i = 0; i < points.size(); i++){
        int dist = (points.at(i) - p).manhattanLength();
        if (minDist == -1 || dist < minDist) {
            minDist = dist;
            n = i;
        }
    }
    return n;
}

