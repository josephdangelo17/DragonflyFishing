///
/// A basic button
///
/// Invokes callback() when clicked.
/// Changes to highlight color on mouse over.
/// 

#ifndef __BUTTON_H__
#define __BUTTON_H__

// Engine includes.
#include "EventMouse.h"    
#include "ViewObject.h"

namespace df {

/// Categories of button attributes that indicate modification.
enum class ButtonAttribute : unsigned int {
  APPEARANCE = 1 << (ViewObjectAttributeMax + 0),  // 1
};
const int ButtonAttributeMax = ViewObjectAttributeMax + 1;

class Button : public ViewObject {

 private:
  Color m_highlight_color;	 ///< Color when highlighted.
  Color m_default_color;	 ///< Color when not highlighted.

 public:
  Button();

  /// Handle "mouse" events.
  /// Return 0 if ignored, else 1.
  int eventHandler(const Event *p_e) override;

  /// Set highlight (when mouse over) color for Button.
  void setHighlightColor(Color new_highlight_color);

  /// Get highlight (when mouse over) color for Button.
  Color getHighlightColor() const;

  /// Set color of Button.
  void setDefaultColor(Color new_default_color);

  /// Get color of Button
  Color getDefaultColor() const;

  /// Return true if mouse over Button, else false. 
  bool mouseOverButton(const EventMouse *p_e) const;

  /// Called when Button clicked.
  /// Must be defined by derived class.
  virtual void callback() = 0;

  /// Serialize attributes to stream.
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
};

} // end of namespace df
#endif
