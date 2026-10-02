///
/// A basic TextBox, with word wrap and scrolling.
/// 

#ifndef __TEXTBOX_H__
#define __TEXTBOX_H__

// Engine includes.
#include "Event.h"
#include "ViewObject.h"

namespace df {

/// Categories of TextBox attributes that indicate modification.
enum class TextBoxAttribute : unsigned int {
  TEXT       = 1 << (ViewObjectAttributeMax + 0),  // 01
  APPEARANCE = 1 << (ViewObjectAttributeMax + 1),  // 10
};
const int TextBoxAttributeMax = ViewObjectAttributeMax + 2;

class TextBox : public ViewObject {

 private:
  std::string m_text;  ///< Text in box.
  Vector m_size;       ///< Width (x), height (y), in spaces.
  bool m_padding;      ///< True to pad rows from side bars.
  bool m_word_wrap;    ///< True to wrap lines at spaces.

 public:
  TextBox();

  /// Set text, overwritting any current text.
  void setText(std::string new_text);

  /// Append text, adding to any current text.
  void addText(std::string new_text);

  /// Get text.
  std::string getText() const;

  /// Remove all text.
  void clearText();

  /// Get size.
  Vector getSize() const;

  /// Set size.
  void setSize(df::Vector new_size);
  
  /// Get padding.
  bool getPadding() const;

  /// Set padding.
  void setPadding(bool new_padding);

  /// Get word wrap.
  bool getWordWrap() const;

  /// Set word wrap.
  void setWordWrap(bool new_word_wrap);

  /// Draw box and text.
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
