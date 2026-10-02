///
/// An animation for a sprite
/// 

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

// System includes.
#include <string>

// Engine includes.
#include "Box.h"
#include "Sprite.h"

namespace df {

class Animation {

 private:
  Sprite *m_p_sprite;    ///< Sprite associated with Animation.
  int m_index;           ///< Current index frame for Sprite.
  int m_slowdown_count;  ///< Slowdown counter (-1 means stop).
  std::string m_name;    ///< Sprite name in ResourceManager.
  Transform m_transform; ///< Transform sprite before drawing.
  bool m_modified;       ///< True if modified since serialize().

 public:
  /// Animation constructor
  Animation();             

  /// Set associated Sprite to new one.
  /// Note, Sprite is managed by ResourceManager.
  /// Set Sprite index to 0 (first frame).
  void setSprite(Sprite *p_new_sprite);

  /// Return pointer to associated Sprite.
  const Sprite *getSprite() const;

  /// Set Sprite name (in ResourceManager).
  void setName(std::string new_name);

  /// Get Sprite name (in ResourceManager).
  std::string getName() const;

  /// Set transform for Sprite.
  void setTransform(Transform new_transform);

  /// Get transform for Sprite.
  Transform getTransform(void) const;

  /// Set index of current Sprite frame to be displayed.
  void setIndex(int new_index);

  /// Get index of current Sprite frame to be displayed.
  int getIndex() const;

  /// Set animation slowdown count (-1 means stop animation).
  void setSlowdownCount(int new_slowdown_count);

  /// Set animation slowdown count (-1 means stop animation).
  int getSlowdownCount() const;
  
  /// Get bounding box of associated Sprite.
  Box getBox() const;

  /// Serialize attributes to stream.
  /// Return 0 if ok, else -1.
  int serialize(std::stringstream *p_ss) const;

  /// Deserialize attributes from stream.
  /// Return 0 if ok, else -1.
  int deserialize(std::stringstream *p_ss);

  /// Return true if any attributes modified since last serialize.
  bool isModified() const;

  /// Draw single frame centered at position (x,y).
  /// Drawing accounts for slowdown, and advances Sprite frame.
  /// Return 0 if ok, else -1.
  int draw(Vector position);

  /// Compare two Animations.
  bool operator==(const Animation &a);
  bool operator!=(const Animation &a);

  /// Return attributes as string.
  std::string toString() const; 
};

} // end of namespace df
#endif // __ANIMATION_H__
