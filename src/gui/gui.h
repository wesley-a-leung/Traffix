#ifndef GUI_H
#define GUI_H

#include <unordered_map>
#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include "../framework/Framework.h"

namespace Ui {
    class GUI;
}

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class GUI : public QMainWindow {
    Q_OBJECT

public:
    explicit GUI(WeightedDigraph *G, QWidget *parent = 0);
    void drawComponents();
    ~GUI();

private:
    QWidget *parent;
    void renderImage();
    Ui::GUI *ui;
    WeightedDigraph *graph;
    QImage image;
    QLabel *efficiencyLabel;
    std::unordered_map<int, QLabel*> labels;
};
#endif // GUI_H
