///
/// The base game world object
///

#ifndef __OBJECT_H__
#define __OBJECT_H__

// System includes.
#include <string>

// Engine includes.
#include "Animation.h"        ///< Objects (often) have animated sprites.
#include "Box.h"              ///< Objects have a bounding box.
#include "Event.h"            ///< Objects can handle events.
#include "Path.h"	      ///< Objects can seek along path.
#include "PathFind.h"	      ///< Objects can do pathfinding.
#include "Shape.h"	      ///< Objects can have simple drawing shapes.
#include "Vector.h"           ///< Objects need a float location.

namespace df {

/// Max number of events Object can be interested in.
const int MAX_OBJ_EVENTS = 100;

/// Types of solidness of Object.
enum Solidness {
  HARD,       ///< Object causes collisions and impedes.
  SOFT,       ///< Object causes collision, but doesn't impede.
  SPECTRAL,   ///< Object doesn't cause collisions.
};

/// Categories of Object attributes that indicate modification.
enum class ObjectAttribute : unsigned int {
  ID                = 1 << 0,  // 0001
  ACTIVE            = 1 << 1,  // 0010 
  VISIBLE           = 1 << 2,  // 0100
  EVENTS            = 1 << 3,  // ...
  BOX               = 1 << 4,  
  POSITION          = 1 << 5,  
  TYPE              = 1 << 6,  
  ANIMATION         = 1 << 7,  
  ALTITUDE          = 1 << 8,  
  SOLIDNESS         = 1 << 9,  
  NO_SOFT           = 1 << 10, 
  SPEED             = 1 << 11, 
  DIRECTION         = 1 << 12, 
  ACCELERATION      = 1 << 13, 
  SHAPE             = 1 << 14, 
  PATH              = 1 << 15, 
  FOLLOW_PATH       = 1 << 16, 
  AVOID_COLLISIONS  = 1 << 17, 
};
const int ObjectAttributeMax = 18;

class PathFind;  // Forward reference.

class Object {

 public:
  static int max_id;	        ///< Monotonically increasing identifier.

 private:
  int m_id;		        ///< Unique game engine defined identifier.
  std::string m_type;	        ///< Game programmer defined type.
  bool m_is_active;             ///< If false, Object not acted upon.
  bool m_is_visible;            ///< If true, Object gets drawn.
  Box m_box;                    ///< Box for sprite boundary & collisions.
  Vector m_position;            ///< Position in game world.
  Vector m_direction;	        ///< Direction vector.
  float m_speed;                ///< Object speed in direction.
  Vector m_acceleration;	///< Acceleration vector.
  int m_altitude;               ///< 0 to MAX_ALTITUDE (higher drawn on top).
  Solidness m_solidness;        ///< Solidness state of Object.
  Animation m_animation;        ///< Animation associated with Object.
  bool m_no_soft;               ///< True if won't move on SOFT Objects.
  int m_event_count;            ///< Number of events of interest.
  std::string m_event_name[MAX_OBJ_EVENTS]; ///< Names of events interested in.
  unsigned int m_modified;	///< Mod. attributes mask since last serialize().
  Shape m_shape;	        ///< Basic geometric shape to draw.
  PathFind *m_p_path_find;      ///< Object performing pathfinding.
  Path m_path;                  ///< If following path, holds path information.
  bool m_follow_path;           ///< True if following path.
  bool m_avoid_collisions;      ///< True if should avoid collisions with HARD.

 public:

  /// Construct Object. 
  /// Set default parameters and add to game world (WorldManager).
  Object();

  /// Destroy Object. 
  /// Unregister for any interested events.
  /// Remove from game world (WorldManager).
  virtual ~Object();

  /// Set Object id.
  void setId(int new_id);

  /// Get Object id.
  int getId() const;
  
  /// Set type identifier of Object.
  virtual void setType(std::string new_type);

  /// Get type identifier of Object.
  std::string getType() const;

  /// Set position of Object.
  /// Return 0 if ok, else -1.
  virtual int setPosition(Vector new_position);

  /// Get position of Object.
  Vector getPosition() const;

  /// Handle event (default only handles pathfinding).
  /// Return 0 if ignored, else 1.
  virtual int eventHandler(const Event *p_event);

  /// Return True if Object is HARD or SOFT, else false.
  bool isSolid() const;    

  /// Set solidness of Object, with checks for consistency.  
  /// Return 0 if ok, else -1.
  virtual int setSolidness(Solidness new_solid);

  /// Return solidness of Object.
  Solidness getSolidness() const;

  /// Set "no soft" setting of Object (true - cannot move onto SOFT Objects).
  void setNoSoft(bool new_no_soft=true);

  /// Get "no soft" setting of Object (true - cannot move onto SOFT Objects).
  bool getNoSoft() const;

  /// Set "avoid collisions" (true - avoid collisions with HARD when moving).
  virtual void setAvoidCollisions(bool new_avoid_collisions=true);

  /// Get "avoid collisions" (true - avoid collisions with HARD when moving).
  bool getAvoidCollisions() const;

  /// Set altitude of Object, with checks for range [0, MAX_ALTITUDE].
  /// Return 0 if ok, else -1.
  virtual int setAltitude(int new_altitude);
   
  /// Return altitude of Object.
  int getAltitude() const;
   
  /// Set speed of Object.
  void setSpeed(float speed);

  /// Get speed of Object.
  float getSpeed() const;

  /// Set direction of Object.
  virtual void setDirection(Vector new_direction);

  /// Get direction of Object.
  Vector getDirection() const;

  /// Set direction and speed of Object.
  virtual void setVelocity(Vector velocity);

  /// Get velocity of Object based on direction and speed.
  Vector getVelocity() const;

  /// Set acceleration of Object.
  virtual void setAcceleration(Vector new_acceleration);

  /// Get acceleration of Object.
  Vector getAcceleration() const;

  /// Add acceleration to Object velocity.
  /// (Typically done once per step by World Manager).
  void accelerate();

  /// Predict Object position based on speed and direction.
  /// Return predicted position.
  Vector predictPosition();

  /// Navigate Object from current position to target position
  /// using pathfinding.
  /// Turns on avoid collisions.
  void pathTo(Vector position);

  /// Navigate Object from current position to current position
  /// of target Object using pathfinding.
  /// Turns on avoid collisions.
  void pathTo(Object *p_o_target);

  /// Set direction of Object towards position.
  /// (Called when pathfinding).
  void moveTo(Vector new_position);

  /// If following a path, update path progress.
  void doPathFollowing();

  /// Set visibility of Object.  Objects not visible are not drawn.
  /// Return 0 if ok, else -1.
  int setVisible(bool new_visible=true);

  /// Return visibility of Object.  Objects not visible are not drawn.
  virtual bool isVisible() const;

  /// Set activeness of Object.  Objects not active are not acted upon
  /// by engine.
  /// Return 0 if ok, else -1.
  virtual int setActive(bool new_active=true);

  /// Return activeness of Object.  Objects not active are not acted upon
  /// by engine.
  bool isActive() const;

  /// Set bounding box of Object.
  virtual void setBox(Box new_box);

  /// Get bounding box of Object.
  Box getBox() const;

  /// Register for interest in event.  
  /// Keeps track of manager and event.  
  /// Return 0 if ok, else -1.
  int registerInterest(std::string event_type);

  /// Unregister for interest in event.  
  /// Return 0 if ok, else -1.
  int unregisterInterest(std::string event_type);

  /// Get registered interest event count.
  int getEventCount() const;

  /// Set Sprite for this Object to animate.
  /// Return 0 if ok, else -1.
  virtual int setSprite(std::string sprite_label);

  /// Set Animation for this Object to new one.
  /// If set_box is true, set bounding box to size of associated Sprite.
  virtual void setAnimation(Animation new_animation, bool set_box=true);

  /// Get Animation for this Object.
  Animation getAnimation() const;

  /// Draw Object: Animation and/or Shape.
  /// Return 0 if ok, else -1.
  virtual int draw();

  /// Serialize attributes to stream.
  /// Can specify individual attribute(s) to force (modified or not).
  /// Default is only modified attributes.
  /// Clears modified bits for attributes serialized.
  /// Return 0 if ok, else -1.
  virtual int serialize(std::stringstream *p_ss, unsigned int attr=0);

  /// Deserialize stream to attributes and apply.
  /// p_ss - incoming stream to deserialize.
  /// p_a - outgoing bitmask of attributes modified (NULL means no outgoing).
  /// Return 0 if ok, else -1.  
  virtual int deserialize(std::stringstream *p_ss, unsigned int *p_a=NULL);

  /// Return true if attribute modified since last serialize.
  bool isModified(enum ObjectAttribute attribute) const;

  /// Return true if any attribute modified since last serialize.
  bool isModified() const;

  /// Set modified attribute mask.
  virtual void setModified(unsigned int new_modified);

  /// Return modified attribute mask.
  virtual unsigned int getModified() const;
  
  /// Set drawing shape.
  virtual void setShape(Shape new_shape);

  /// Get drawing shape.
  Shape getShape() const;

  /// Set follow_path (note: is also set in pathTo() method).
  /// If path defined, start moving to first node.
  virtual void setFollowPath(bool new_follow_path=true);

  /// Returns true if following a path (note: is set in pathTo() method).
  bool getFollowPath() const;

  /// Set Path (note: is also set in pathTo() method).
  void setPath(Path new_path);

  /// Get Path (note: is set in pathTo() method).
  Path getPath() const;

  /// Get object doing pathfinding. 
  PathFind *getPathFind();

  /// Return base attributes as string.
  std::string toString() const;

  /// Compare two Objects. 
  bool operator==(const Object &o);
  bool operator!=(const Object &o);
};

} // end of namespace df
#endif // __OBJECT_H__
