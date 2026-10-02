///
/// The view object
///

#ifndef __VIEW_OBJECT_H__
#define __VIEW_OBJECT_H__

// System includes.
#include <string>

// Engine includes.
#include "Color.h"
#include "Event.h"        
#include "Object.h"

namespace df {

/// Categories of ViewObject attributes that indicate modification.
enum class ViewObjectAttribute : unsigned int {
  VALUE      = 1 << (ObjectAttributeMax + 0),  // 01 
  APPEARANCE = 1 << (ObjectAttributeMax + 1),  // 10
};
const int ViewObjectAttributeMax = ObjectAttributeMax + 2;

/// General location on screen.
enum ViewObjectLocation {
  UNDEFINED=-1,
  TOP_LEFT,
  TOP_CENTER,
  TOP_RIGHT,
  CENTER_LEFT,
  CENTER_CENTER,
  CENTER_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_CENTER,
  BOTTOM_RIGHT,
};

class ViewObject : public Object {

 private:
  std::string m_view_string;     ///< Label for value (e.g., "Points").
  int m_value;                   ///< Value displayed (e.g., points).
  bool m_draw_value;	         ///< True if should draw value.
  bool m_border;                 ///< True if border around display.
  Color m_color;                 ///< Color for text (and border).
  ViewObjectLocation m_location; ///< Location of ViewObject.

 public:
  /// Construct ViewObject. 
  /// Object settings: SPECTRAL, max altitude.
  /// ViewObject defaults: border, top_center, default color, draw_value.
  ViewObject();

  /// Draw view string (and value).
  /// Return 0 if ok, negative if not.
  virtual int draw() override;

  /// Handle "view" event if tag matches view_string (others ignored).
  /// Return 0 if ignored, else 1.
  virtual int eventHandler(const Event *p_e) override;

  /// Set general location of ViewObject on screen.
  virtual void setLocation(ViewObjectLocation new_location);

  /// Get general location of ViewObject on screen.
  ViewObjectLocation getLocation() const;

  /// Set true to draw value with display string.
  virtual void setDrawValue(bool new_draw_value = true);

  /// Get draw value (true if draw value with display string).
  bool getDrawValue() const;

  /// Set view value.
  virtual void setValue(int new_value);

  /// Get view value.
  int getValue() const;

  /// Set view border (true = display border).
  virtual void setBorder(bool new_border);

  /// Get view border (true = display border).
  bool getBorder() const;

  /// Set view color.
  virtual void setColor(Color new_color);

  /// Get view color.
  Color getColor() const;

  /// Set view display string.
  virtual void setViewString(std::string new_view_string);

  /// Get view display string.
  std::string getViewString() const;
  
  /// Set position of ViewObject, with setting "location" to UNDEFINED.
  virtual int setPosition(Vector new_pos) override;

  /// Serialize modified attributes to stream.
  /// Can specify individual attribute(s) to force (modified or not).
  /// Default is only modified attributes.
  /// Clears modified bits for attributes serialized.
  /// Return 0 if ok, else -1.
  virtual int serialize(std::stringstream *p_ss, unsigned int attr=0) override;

  /// Deserialize stream to attributes and apply.
  /// p_ss - incoming stream to deserialize.
  /// p_a - outgoing bitmask of attributes modified (NULL means no outgoing).
  /// Return 0 if ok, else -1.  
  virtual int deserialize(std::stringstream *p_ss, unsigned int *p_a=NULL) override;

  /// Return base attributes as string.
  std::string toString() const;
  
  /// Compare two ViewObjects. 
  bool operator==(const ViewObject &vo);
  bool operator!=(const ViewObject &vo);
};

} // end of namespace df
#endif // __VIEW_OBJECT_H__
