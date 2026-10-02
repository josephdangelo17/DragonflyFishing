///
/// A 2-d bounding box
///

#ifndef __BOX_H__
#define __BOX_H__

// System includes.
#include <sstream>

// Game includes.
#include "Vector.h"

namespace df {

class Box {

 private:
  Vector m_corner;         ///< Upper left corner of box.
  float m_horizontal;	   ///< Horizontal dimension.
  float m_vertical;	   ///< Vertical dimension.

 public:
  /// Create box with upper-left corner, horiz and vert sizes.
  Box(Vector init_corner, float init_horizontal, float init_vertical);

  /// Create box with (0,0) for corner, and 0 for horiz and vert.
  Box();

  /// Set upper left corner of box.
  void setCorner(Vector new_corner);

  /// Get upper left corner of box.
  Vector getCorner() const;

  /// Set horizontal size of box.
  void setHorizontal(float new_horizontal);

  /// Get horizontal size of box.
  float getHorizontal() const;

  /// Set vertical size of box.
  void setVertical(float new_vertical);

  /// Get vertical size of box.
  float getVertical() const;

  /// Return attributes as string.
  std::string toString() const;

  /// Compare two boxes.
  bool operator==(const Box &other) const;
  bool operator!=(const Box &other) const;

  /// Draw box outline in pixels.
  void draw();

  /// Serialize attributes to stream.
  /// Return 0 if ok, else -1.
  int serialize(std::stringstream *p_ss) const;

  /// Deserialize attributes from stream.
  /// Return 0 if ok, else -1.
  int deserialize(std::stringstream *p_ss);
};


} // end of namespace df
#endif //__BOX_H__
