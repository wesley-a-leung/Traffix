#include <string>
#include <cstdio>
#include <ctime>
#include "GUIDriver.h"

using namespace std;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    GUIDriver *gd = new GUIDriver(argc, argv, 20, "", 1, true);
    // GUIDriver *gd = new GUIDriver(argc, argv, 20, ":/data/diagonalGridDemo.txt", 1, false);
    // GUIDriver *gd = new GUIDriver(argc, argv, 20, ":/data/gridDemo.txt", 1, false);
    gd->run();
    delete gd;
    return 0;
}
