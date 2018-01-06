#ifndef POINT2D_H_
#define POINT2D_H_

#include <cstddef> // for size_t
#include "Forward.h"

/*
 * Based on the Point2D.java class from Chapter 1.2 of "Algorithms Fourth Edition" by Robert Sedgewick and Kevin Wayne.
 */
struct Point2D {
private:
    static const double EPS;

public:
    double x; // x-coordiante
    double y; // y-coordiante

    Point2D();
    Point2D(double x, double y);
    ~Point2D();
    double distanceTo(const Point2D &that) const;
    double distanceSquaredTo(const Point2D &that) const;
    Point2D &rotate(const Point2D &that, double theta) const;
    double angleTo(const Point2D &that) const;
    int compareTo(const Point2D &that) const;
    bool operator == (const Point2D &that) const;
    bool operator != (const Point2D &that) const;
    static bool xOrderLt(const Point2D &p, const Point2D &q);
    static bool xOrderLe(const Point2D &p, const Point2D &q);
    static bool xOrderGt(const Point2D &p, const Point2D &q);
    static bool xOrderGe(const Point2D &p, const Point2D &q);
    static bool yOrderLt(const Point2D &p, const Point2D &q);
    static bool yOrderLe(const Point2D &p, const Point2D &q);
    static bool yOrderGt(const Point2D &p, const Point2D &q);
    static bool yOrderGe(const Point2D &p, const Point2D &q);
    bool distanceToOrderLt(const Point2D &p, const Point2D &q) const;
    bool distanceToOrderLe(const Point2D &p, const Point2D &q) const;
    bool distanceToOrderGt(const Point2D &p, const Point2D &q) const;
    bool distanceToOrderGe(const Point2D &p, const Point2D &q) const;
    bool atan2OrderLt(const Point2D &p, const Point2D &q) const;
    bool atan2OrderLe(const Point2D &p, const Point2D &q) const;
    bool atan2OrderGt(const Point2D &p, const Point2D &q) const;
    bool atan2OrderGe(const Point2D &p, const Point2D &q) const;
};

struct Point2D_hash {
    size_t operator ()(const Point2D &p) const;
};

#endif
