//
// A fading particle
//

#ifndef __FADER_H__
#define __FADER_H__

// Engine include.
#include "Particle.h"

namespace df {

class Fader : public Particle {
 private:
  unsigned char m_fade;		///< Amount of fade each step. 

 public:
  /// Create fading particle with size (pixels), age (in ticks), 
  //// opacity (0-255) and rgb color.
  Fader(float size, int age, unsigned char opacity, 
	unsigned char r, unsigned char g, unsigned char b);
  
  // Handle step events.
  // Return 0 if ignored, else 1.
  int eventHandler(const df::Event *p_e)  override;
};

} // end of namespace df

#endif //__FADER_H__
