#include <cmath>
#include <assert.h>
#include <QBrush>
#include <QPainter>
#include <ui_gui.h>
#include "gui.h"
#include "ui_gui.h"

using namespace std;

#define SCREEN_WIDTH 2000
#define SCREEN_HEIGHT 2000
#define SCALE_FACTOR 1
#define EPS 1e-9
#define ROAD_WIDTH 5
#define ROAD_SEPARATION 5
#define INTERSECTION_RADIUS 10
#define CAR_RADIUS 1
#define LABEL_DIST 25
#define PI 3.14159265358979323846
#define LABEL_WIDTH 80
#define LABEL_HEIGHT 60
#define EFF_LABEL_WIDTH 55
#define EFF_LABEL_HEIGHT 30
#define TRAFFIC_LIGHT_RADIUS 5

// colour constants
const color COLOR_BLACK = {0, 0, 0};
const color COLOR_WHITE = {255, 255, 255};
const color COLOR_RED = {255, 0, 0};
const color COLOR_YELLOW = {255, 255, 0};
const color COLOR_GREEN = {0, 255, 0};
const color COLOR_BLUE = {0, 0, 255};

const Point2D origin(0.0, 0.0);
const Point2D TRAFFIC_LIGHT_BACK_OFFSET(-20, 0); // offset as a point, relative to the intersection
const Point2D TRAFFIC_LIGHT_LEFT_OFFSET(-20, -12); // offset as a point, relative to the intersection

/**
 * Constructs a
 * @param G The WeightedDigraph to be drawn to the screen
 * @param parent The parent Qt GUI componenet. SHould be a null pointer in all cases.
 */
GUI::GUI(WeightedDigraph *G, QWidget *parent) : QMainWindow(parent), ui(new Ui::GUI), image(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format::Format_ARGB32) {
    ui->setupUi(this);
    this->graph = G;
    this->parent = parent;
    ui->scrollArea->setWidget(ui->imageLabel);
    efficiencyLabel = new QLabel(this);
    drawComponents();
}

/**
 * Deconstructs the GUI object and the QLabels for each RoadSegment.s
 */
GUI::~GUI() {
    delete efficiencyLabel;
    for (pair<int, QLabel*> p : labels) {
        delete p.second;
    }
    delete ui;
}

void GUI::renderImage() {
     ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}
/**
 * Draws the Intersections, Road Segments, and Cars to the Window
 */
void GUI::drawComponents() {
    char effLabelBuffer[8];
    string effLabelStr;
    sprintf(effLabelBuffer,"%.2f%%",graph->getEfficiency() * 100.0);
    effLabelStr = string(effLabelBuffer);
    efficiencyLabel->setText(QString::fromStdString(effLabelStr));
    efficiencyLabel->setGeometry(730, 30, EFF_LABEL_WIDTH, EFF_LABEL_HEIGHT);

    //efficiencyLabel = QLabel(QString::fromStdString(s))
    QPainter painter(&image);
    QPen *pen;
    //QBrush brush;
    image.fill(QColor(COLOR_WHITE.r,COLOR_WHITE.g,COLOR_WHITE.b));
    Intersection *intersection;
    RoadSegment *road;
    TrafficLight *light;
    Car *car;

    // draws circles to represent each intersection
    for(pair<int, Intersection*> p: graph->getIntersections()) {
        intersection = p.second;
        Point2D tempLoc = intersection->getLocation();
        QPoint location(tempLoc.x * SCALE_FACTOR, tempLoc.y * SCALE_FACTOR);
        painter.setPen(QPen(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b))); // border colour
        painter.setBrush(QBrush(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b))); // fill colour
        painter.drawEllipse(location, INTERSECTION_RADIUS, INTERSECTION_RADIUS);
    }

    char buffer[25];
    string s;

    // draws Lines to represent each road
    for(pair<int, RoadSegment*> p: graph->getRoadSegments()) {
        road = p.second;
        Point2D tempSource = road->getSource()->getLocation();
        Point2D tempDest = road->getDestination()->getLocation();
        double angle = tempSource.angleTo(tempDest); // (-PI, PI]
        double labelPosX = LABEL_DIST * cos(angle + PI / 2) + ((tempSource.x + tempDest.x) / 2) - (LABEL_WIDTH / 2);
        double labelPosY = LABEL_DIST * sin(angle + PI / 2) + ((tempSource.y + tempDest.y) / 2) - (LABEL_HEIGHT / 2);
        double scrollBarX = ui->scrollArea->horizontalScrollBar()->value();
        double scrollBarY = ui->scrollArea->verticalScrollBar()->value();
        double adjX = (angle >= 0.0 ? -1.0 : 1.0) * ROAD_SEPARATION * abs(sin(angle));
        double adjY = (abs(angle) >= PI / 2 ? -1.0 : 1.0) * ROAD_SEPARATION * abs(cos(angle));
        QPoint source(tempSource.x * SCALE_FACTOR + adjX, tempSource.y * SCALE_FACTOR + adjY);
        QPoint destination(tempDest.x * SCALE_FACTOR + adjX, tempDest.y * SCALE_FACTOR + adjY);

        // colours roads based on flow:capacity ratio
        double percentage = (double) road->getFlow() / (double) road->getCapacity();
        if (abs(percentage) <= EPS) pen = new QPen(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b));
        else if (percentage <= 0.33) pen = new QPen(QColor(COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b));
        else if (percentage <= 0.67) pen = new QPen(QColor(COLOR_YELLOW.r, COLOR_YELLOW.g, COLOR_YELLOW.b));
        else pen = new QPen(QColor(COLOR_RED.r, COLOR_RED.g, COLOR_RED.b));

        pen->setWidth(ROAD_WIDTH);
        painter.setPen(*pen);
        painter.drawLine(source, destination);

        // draws blue dots to represent cars
        for (pair<int, Car*> c : road->getCars()) {
            car = c.second;
            QPoint location(car->getCurrentLocation().x * SCALE_FACTOR + adjX, car->getCurrentLocation().y * SCALE_FACTOR + adjY);
            painter.setPen(QPen(QColor(COLOR_BLUE.r, COLOR_BLUE.g, COLOR_BLUE.b))); // border colour is changed here
            painter.setBrush(QBrush(QColor(COLOR_BLUE.r, COLOR_BLUE.g, COLOR_BLUE.b))); // fill colour is changed here
            painter.drawEllipse(location, CAR_RADIUS, CAR_RADIUS); // change the constant to change the radius, DO NOT change this value here
        }
        if (!labels.count(p.first)) labels[p.first] = new QLabel(this);
        labels[p.first]->setAlignment(Qt::AlignCenter);
        sprintf(buffer,"%.1f\n%d / %d",road->getSpeedLimit(), road->getFlow(), road->getCapacity());
        s = string(buffer);
        labels[p.first]->setText(QString::fromStdString(s));
        labels[p.first]->setGeometry(SCALE_FACTOR * (labelPosX - scrollBarX), SCALE_FACTOR * (labelPosY - scrollBarY), LABEL_WIDTH, LABEL_HEIGHT);
        delete pen;
    }
    // draws small coloured dots to represent traffic light
    for(pair<int, Intersection*> p: graph->getIntersections()) {
        intersection = p.second;
        Point2D tempLoc = intersection->getLocation();
        QPoint location(tempLoc.x * SCALE_FACTOR, tempLoc.y * SCALE_FACTOR);
        for (pair<int, TrafficLight*> t : intersection->getTrafficLights()) {
            light = t.second;
            int type = light->getType();
            if (type == UTURN || type == RIGHT) continue; // these types of lights do not need to be displayed
            assert((type == LEFT || type == STRAIGHT) &&
                "there exists a traffic light that is not a valid type and somehow did not get caught in the traffic light contructor ... this is a problem ...");
            int state = light->getState();;
            double fromDirection = light->getFrom()->getDirection();
            Point2D trafficLightTempLoc = (type == LEFT ? TRAFFIC_LIGHT_LEFT_OFFSET : TRAFFIC_LIGHT_BACK_OFFSET).rotate(origin, fromDirection);
            trafficLightTempLoc.x += tempLoc.x;
            trafficLightTempLoc.y += tempLoc.y;
            QPoint trafficLightLoc(trafficLightTempLoc.x * SCALE_FACTOR, trafficLightTempLoc.y * SCALE_FACTOR);
            QColor *fill;
            if (state == RED) fill = new QColor(COLOR_RED.r, COLOR_RED.g, COLOR_RED.b);
            else if (state == GREEN) fill = new QColor(COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b);
            else if (state == YELLOW) fill = new QColor(COLOR_YELLOW.r, COLOR_YELLOW.g, COLOR_YELLOW.b);
            else assert(false && "This is really bad. This means the traffic light is something other than red, yellow, green.");
            painter.setPen(QPen(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b))); // border colour
            painter.setBrush(QBrush(*fill)); // fill colour
            painter.drawEllipse(trafficLightLoc, TRAFFIC_LIGHT_RADIUS, TRAFFIC_LIGHT_RADIUS);
            delete fill;
        }
    }
    painter.end();
    ui->imageLabel->setPixmap(QPixmap::fromImage(image)); // paints components to the ui
}
