#include <cmath>
#include <functional> // for hash
#include "Point2D.h"

using namespace std;

const double Point2D::EPS = 1e-9;

/**
 * Default values for the point.
 */
Point2D::Point2D() {
    x = 0.0;
    y = 0.0;
}

/**
 * Initializes a new point in the 2-D cartesian plane.
 * @param x the x-coordinate
 * @param y the y-coordinate
 */
Point2D::Point2D(double x, double y) {
    if (x == 0.0) this->x = 0.0;  // convert -0.0 to +0.0
    else this->x = x;
    if (y == 0.0) this->y = 0.0;  // convert -0.0 to +0.0
    else this->y = y;
}

/**
 * Deconstructs the Point
 */
Point2D::~Point2D() {}

/**
 * Returns the Euclidean distance between this point and that point.
 * @param that the point to compute the distance between
 */
double Point2D::distanceTo(const Point2D &that) const {
    double dx = x - that.x; // difference in x value
    double dy = y - that.y; // difference in y value
    return sqrt(dx * dx + dy * dy); // pythagorean theorem
}

/**
 * Returns the square of the Euclidean distance between this point and that point.
 * @param that the point to compute the distance between
 */
double Point2D::distanceSquaredTo(const Point2D &that) const {
    double dx = x - that.x; // difference in x value
    double dy = y - that.y; // difference in y value
    return dx * dx + dy * dy; // pythagorean theorem
}

/**
 * Returns a point that is this point rotated theta radians around that point.
 *
 * @param that the pivot point
 * @param theta the angle in radians
 * @return a point that is this point rotated theta radians around that point
 */
Point2D &Point2D::rotate(const Point2D &that, double theta) const {
    double x = that.x + (this->x - that.x) * cos(theta) - (this->y - that.y) * sin(theta);
    double y = that.y + (this->x - that.x) * sin(theta) + (this->y - that.y) * cos(theta);
    return *(new Point2D(x, y));
}

/**
 * Returns the angle between this point and that point.
 * @return the angle in radians (between -pi and pi) between this point and that point (0 if equal)
 */
double Point2D::angleTo(const Point2D &that) const {
    double dx = that.x - x;
    double dy = that.y - y;
    return atan2(dy, dx);
}

/**
 * Compares the 2 points by y coordinate and breaks ties by its x coordinate
 * This point is less than that point if and only if this.y < that.y or
 * this.y = that.y and this.x < that.x.
 *
 * @param  that the point to compare to
 * @return -1 if and only if this point is less than that point
 *         0 if and only if this point is equal to that point
 *         +1 if and only if this point is greater than that point
 */
int Point2D::compareTo(const Point2D &that) const {
    if (y < that.y) return -1;
    if (y > that.y) return +1;
    if (x < that.x) return -1;
    if (x > that.x) return +1;
    return 0;
}

/**
 * Compares this point to that point.
 *
 * @param  that the other point
 * @return true if and only if this point equals the other, false otherwise
 */
bool Point2D::operator == (const Point2D &that) const {
    return abs(x - that.x) <= EPS && abs(y - that.y) <= EPS;
}

/**
 * Compares this point to that point.
 *
 * @param  that the other point
 * @return true if and only if this point does not equals the other, false otherwise
 */
bool Point2D::operator != (const Point2D &that) const {
    return abs(x - that.x) > EPS || abs(y - that.y) > EPS;
}

/**
 * Compares two points by x-coordinate. Less than comparator.
 */
bool Point2D::xOrderLt(const Point2D &p, const Point2D &q) { return q.x - p.x < EPS; }

/**
 * Compares two points by x-coordinate. Less than or equal to comparator.
 */
bool Point2D::xOrderLe(const Point2D &p, const Point2D &q) { return q.x - p.x <= -EPS; }

/**
 * Compares two points by x-coordinate. Greater than comparator.
 */
bool Point2D::xOrderGt(const Point2D &p, const Point2D &q) { return p.x - q.x < EPS; }

/**
 * Compares two points by x-coordinate. Greater than or equal to comparator.
 */
bool Point2D::xOrderGe(const Point2D &p, const Point2D &q) { return p.x - q.x <= -EPS; }

// Y ORDER

/**
 * Compares two points by y-coordinate. Less than comparator.
 */
bool Point2D::yOrderLt(const Point2D &p, const Point2D &q) { return q.y - p.y < EPS; }

/**
 * Compares two points by y-coordinate. Less than or equal to comparator.
 */
bool Point2D::yOrderLe(const Point2D &p, const Point2D &q) { return q.y - p.y <= -EPS; }

/**
 * Compares two points by y-coordinate. Greater than comparator.
 */
bool Point2D::yOrderGt(const Point2D &p, const Point2D &q) { return p.y - q.y < EPS; }

/**
 * Compares two points by y-coordinate. Greater than or equal to comparator.
 */
bool Point2D::yOrderGe(const Point2D &p, const Point2D &q) { return p.y - q.y <= -EPS; }

// DISTANCETO ORDER

/**
 * Compares two points by distance to this point. Less than comparator.
 */
bool Point2D::distanceToOrderLt(const Point2D &p, const Point2D &q) const { return distanceSquaredTo(q) - distanceSquaredTo(p) < EPS; }

/**
 * Compares two points by distance to this point. Less than or equal to comparator.
 */
bool Point2D::distanceToOrderLe(const Point2D &p, const Point2D &q) const { return distanceSquaredTo(q) - distanceSquaredTo(p) <= -EPS; }

/**
 * Compares two points by distance to this point. Greater than comparator.
 */
bool Point2D::distanceToOrderGt(const Point2D &p, const Point2D &q) const { return distanceSquaredTo(p) - distanceSquaredTo(q) < EPS; }

/**
 * Compares two points by distance to this point. Greater than or equal to comparator.
 */
bool Point2D::distanceToOrderGe(const Point2D &p, const Point2D &q) const { return distanceSquaredTo(p) - distanceSquaredTo(q) <= -EPS; }

// ATAN2ORDER

/**
 * Compares two points by atan2() angle (between -pi and pi) with respect to this point.
 * Less than comparator.
 */
bool Point2D::atan2OrderLt(const Point2D &p, const Point2D &q) const { return angleTo(q) - angleTo(p) < EPS; }

/**
 * Compares two points by atan2() angle (between -pi and pi) with respect to this point.
 * Less than or equal to comparator.
 */
bool Point2D::atan2OrderLe(const Point2D &p, const Point2D &q) const { return angleTo(q) - angleTo(p) <= -EPS; }

/**
 * Compares two points by atan2() angle (between -pi and pi) with respect to this point.
 * Greater than comparator.
 */
bool Point2D::atan2OrderGt(const Point2D &p, const Point2D &q) const { return angleTo(p) - angleTo(q) < EPS; }

/**
 * Compares two points by atan2() angle (between -pi and pi) with respect to this point.
 * Greater than or equal to comparator.
 */
bool Point2D::atan2OrderGe(const Point2D &p, const Point2D &q) const { return angleTo(p) - angleTo(q) <= -EPS; }

size_t Point2D_hash::operator ()(const Point2D &p) const  {
    return 31 * hash<double> {}(p.x) + hash<double> {}(p.y);
}
