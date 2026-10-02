///
/// A basic text entry.
///
/// Invokes callback() when enter pressed.
/// 

#ifndef __TEXTENTRY_H__
#define __TEXTENTRY_H__

// Engine includes.
#include "EventMouse.h"    
#include "ViewObject.h"

namespace df {

/// Categories of TextEntry attributes that indicate modification.
enum class TextEntryAttribute : unsigned int {
  TEXT       = 1 << (ViewObjectAttributeMax + 0),  // 01
  APPEARANCE = 1 << (ViewObjectAttributeMax + 1),  // 10
};
const int TextEntryAttributeMax = ViewObjectAttributeMax + 2;

class TextEntry : public ViewObject {

 private:
  std::string m_text;             ///< Text entered.
  int m_limit;                    ///< Character limit in text.
  int m_cursor;			  ///< Cursor location.
  char m_cursor_char;             ///< Cursor character.
  int m_blink_rate;               ///< Cursor blink rate.
  bool m_numbers_only;            ///< True if only numbers.
  bool m_shift_down;		  ///< True if shift key pressed.
  
 public:
  TextEntry();

  /// Clear text entry.
  void clearText();

  /// Set text, increasing limit if needed.
  void setText(std::string new_text);

  /// Get text entered.
  std::string getText() const;

  /// Handle "keyboard" events.
  /// Return 0 if ignored, else 1.
  int eventHandler(const Event *p_e) override;

  /// Called when TextEntry enter hit.
  /// Must be defined by derived class.
  virtual void callback() = 0;

  /// Set limit of number of characters allowed.
  void setLimit(int new_limit);

  /// Get limit of number of characters allowed.
  int getLimit() const;

  /// Set cursor to location.
  void setCursor(int new_cursor);

  /// Get cursor location.
  int getCursor() const;

  /// Set blink rate for cursor (in ticks).
  void setBlinkRate(int new_blink_rate);

  /// Get blink rate for cursor (in ticks).
  int getBlinkRate() const;

  /// Return true if only numbers can be entered.
  bool numbersOnly() const;

  /// Set to allow only numbers to be entered.
  void setNumbersOnly(bool new_numbers_only = true);

  /// Set cursor character.
  void setCursorChar(char new_cursor_char);

  /// Get cursor character.
  char getCursorChar() const;

  /// Draw viewstring + text entered.
  virtual int draw() override;

  /// Serialize attributes to stream.
  /// Can specify individual attribute(s) to force (modified or not).
  /// Default is only modified attributes.
  /// Clears modified bits for attributes serialized.
  /// Return 0 if ok, else -1.
  virtual int serialize(std::stringstream *p_ss, unsigned int attr=0)  override;

  /// Deserialize stream to attributes and apply.
  /// p_ss - incoming stream to deserialize.
  /// p_a - outgoing bitmask of attributes modified (NULL means no outgoing).
  /// Return 0 if ok, else -1.  
  virtual int deserialize(std::stringstream *p_ss, unsigned int *p_a=NULL) override;
};

} // end of namespace df
#endif
