//
// Utility functions to support Dragonfly and its games.
//
// Functions here do not use any attributes of any classes, so
// can stand alone.
//

#ifndef __UTILITY_H__
#define __UTILITY_H__

// Engine includes
#ifndef NO_NETWORK
#include "EventNetwork.h"
#endif
#include "Box.h"
#include "Circle.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "Frame.h"
#include "Line.h"
#include "Object.h"
#include "Particle.h"
#include "Precipitation.h"
#include "Vector.h"
#include "ViewObject.h"

namespace df {

/// Return true if value is between min and max (inclusive).
bool valueInRange(float value, float min, float max);

/// Convert relative bounding Box for Object to absolute world Box.
Box getWorldBox(const Object *p_o);

/// Convert relative bounding Box for Object to absolute world Box
/// at postion where.
Box getWorldBox(const Object *p_o, Vector where);

/// Return distance between any two positions.
float distance(Vector p1, Vector p2);

/// Return distance between any two Objects.
/// Return -1.0 if error.
float distance(Object *p_o1, Object *p_o2);

/// Return true of position is within Box.
bool boxContainsPosition(Box b, Vector p);

/// Return true if Box 1 completely contains Box 2.
bool boxContainsBox(Box b1, Box b2);

/// Return true if Line segments intersect.
/// (Parallel line segments don't intersect).
/// (Co-linear lines will NOT intersect).
bool lineIntersectsLine(Line line1, Line line2);

/// Return true if Line segment intersects Box.
bool lineIntersectsBox(Line line, Box b);

/// Return true if Boxes intersect.
bool boxIntersectsBox(Box box1, Box box2);

/// Return true if Circle intersects or contains Box.
bool circleIntersectsBox(Circle circle, Box b);

/// Launch splash screen. Return 0 if ok, else -1.
int splash();

/// Returns pretty-formatted time as char * string.
char *getTimeString();	    

/// Convert world position to view position.
Vector worldToView(Vector world_pos);

/// Convert view position to world position.
Vector viewToWorld(Vector view_pos);

/// Convert integer to string, returning string.
std::string toString(int i);

/// Convert float to string, returning string.
std::string toString(float f);

/// Convert character to string, returning string.
std::string toString(char c);

/// Convert boolean to string, returning string.
std::string toString(bool b);

/// Convert Dragonfly Color to string, returning string.
std::string toString(Color color);

/// Convert Path nodes (not index) to string, returning string.
std::string toString(Path path);

/// Convert Dragonfly key to string, returning string.
std::string toString(Keyboard::Key key_val);

/// Convert Solidness to string, returning string.
std::string toString(Solidness solid);

/// Convert ViewObjectLocation to string, returning string.
std::string toString(ViewObjectLocation location);

// Convert Transform to string, returning string.
std::string toString(Transform transform);

#ifndef NO_NETWORK
// Convert NetworkEventLabel to string, returning string.
std::string toString(NetworkEventLabel label);
#endif
  
/// Match key:value pair in string in str, returning value.
/// Pairs separated by commas (,).
/// If str is empty, use previously parsed string str.
/// All whitespace is ignored.
/// Return empty string if no match.
std::string match(std::string str, std::string find);

/// Add particles.  Each parameter has average and spread.
///  count - number to add
///  position - location 
///  direction - direction to move [(0,0) for random]
///  size - size (pixels)
///  speed - speed (spaces/tick)
///  age - age (ticks)
///  opacity - how "see through" [0-255, 0 is transparent]
///  r, g, b - color in RGB values
/// Particle class - class to add (default is PARTICLE).
/// Return 0 if ok, else -1.
int addParticles(int count, int count_spread,
		 Vector position, float position_spread, 
		 Vector direction, float direction_spread,
		 float size, float size_spread, 
		 float speed, float speed_spread,
		 int age, int age_spread,
		 unsigned char opacity, char opacity_spread,
		 unsigned char r, unsigned char g, unsigned char b,
		 unsigned char color_spread, 
		 ParticleClass particle_class=PARTICLE);

/// Add particles of specific type.
///  type - type of particle: SMOKE, SPARKS, RINGS, FIREWORKS.
///  position - location 
///  scale - scale size (default 1.0)
///  color - dragonfly color to use (default 'built-in')
/// Return 0 if ok, else -1.
int addParticles(ParticleType type, Vector position,
		 float scale=1.0, Color color=CUSTOM);

/// Add environment particles of specific type.
///  type - type of particle: RAIN, SNOW, STARFIELD.
///  direction - direction particle travels: UP, DOWN, LEFT, RIGHT.
///  scale - scale size (default 1.0)
///  color - dragonfly color to use (default 'built-in')
/// Return 0 if ok, else -1.
int addParticles(PrecipitationType type, Direction direction, float scale=1.0, 
		 Color color=CUSTOM);

/// Set RGB color based on DF color.
void colorToRGB(Color color, unsigned char &r, unsigned char &g, 
		 unsigned char &b);

/// Return SFML color based on DF color.
sf::Color colorToSFML(Color color);

/// Return normal between two colliding Objects.
/// Object 1 caused collision with Object 2.
/// Return (0,0) if no normal (no collision or 
/// Object 1 inside Object 2).
Vector getNormal(const Object *p_o1, const Object *p_o2);

/// Return bitmask as string.
std::string maskToString(unsigned int mask);

/// "Explode" a sprite frame into small Particles.
/// p_sprite - sprite to explode
/// index - frame index
/// position - location of frame
/// age - how long it should last (in ticks)
/// speed - speed (in spaces per tick) (negative to "implode")
/// rotate - degrees to spin (per tick)
/// transform - option to flip horizontally, vertically (default NONE)
/// Return 0 if ok, else -1.
int explode(const Sprite *p_sprite, int index, Vector position, int age,
	    float speed, float rotate, Transform transform = NONE);
  
// Compute rotation angle from vector.
float vectorToAngle(df::Vector v);

// Potentially log error message based on stringstream.
// prefix - prefix for logfile (e.g., object::method()).
// p_ss - stringstream to check.
// Return 0 (and no log) if good, else -1 if error.
int stringStreamCheck(const char *prefix, std::stringstream *p_ss);

} // end of namespace df
#endif // __UTILITY_H__
