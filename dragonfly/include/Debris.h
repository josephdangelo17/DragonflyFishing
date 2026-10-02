///
/// A debris particle
///

#ifndef __DEBRIS_H__
#define __DEBRIS_H__

// Engine include.
#include "Particle.h"

namespace df {

class Debris : public Particle {
 private:
  char m_ch;			/// Character to draw.
  Color m_color;		/// Character color.
  float m_rotate;		/// Degrees per tick.
  float m_rotation;		/// Degrees rotated.

 public:
  /// Constructor.
  Debris(int age, unsigned char opacity, Color color,
	 Vector velocity, char ch, float rotate);

  /// Draw character, applying rotation.
  /// Return 0 if ok, else -1.
  virtual int draw() override;
};

} // end of namespace df

#endif //__DEBRIS_H__
