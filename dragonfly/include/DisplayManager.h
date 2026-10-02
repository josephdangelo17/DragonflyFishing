///
/// The SFML display manager
///

#ifndef __DISPLAY_MANAGER_H__
#define __DISPLAY_MANAGER_H__

// System includes.
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

// Engine includes.
#include "Config.h" // For DF_DEPRECATED.
#include "Frame.h"
#include "Manager.h"
#include "Vector.h"

// Two-letter acronym for easier access to manager.
#define DM df::DisplayManager::getInstance()

namespace df {

/// String justifications.
enum Justification {
  LEFT_JUSTIFIED,
  CENTER_JUSTIFIED,
  RIGHT_JUSTIFIED,
};

/// Defaults for SFML window.
const int WINDOW_HORIZONTAL_PIXELS_DEFAULT = 1024;
const int WINDOW_VERTICAL_PIXELS_DEFAULT =  768;
const int WINDOW_HORIZONTAL_CHARS_DEFAULT = 80;
const int WINDOW_VERTICAL_CHARS_DEFAULT = 24;
const int WINDOW_STYLE_DEFAULT = sf::Style::Titlebar|sf::Style::Close;
const sf::State WINDOW_STATE_DEFAULT = sf::State::Windowed;
const Color WINDOW_BACKGROUND_COLOR_DEFAULT = BLACK;
const std::string WINDOW_TITLE_DEFAULT =  "Dragonfly";
const std::string FONT_FILE_DEFAULT = "df-font.ttf";
const float FONT_SCALE_DEFAULT = 2.0;

/// Compute character height in pixels, based on window size.
float charHeight(void);

/// Compute character width in pixels, based on window size.
float charWidth(void);

/// Convert ASCII spaces (x,y) to window pixels (x,y).
Vector spacesToPixels(Vector spaces);

/// Convert window pixels (x,y) to ASCII spaces (x,y).
Vector pixelsToSpaces(Vector pixels);

class DisplayManager : public Manager {

 private:
  DisplayManager();                      ///< Private since a singleton.
  DisplayManager(DisplayManager const&); ///< Don't allow copy.
  void operator=(DisplayManager const&); ///< Don't allow assignment.
  sf::Font m_font;		  ///< Font used for ASCII graphics.
  sf::RenderWindow *m_p_window;	  ///< Pointer to SFML window.
  int m_window_horizontal_pixels; ///< Horizontal pixels in window.
  int m_window_vertical_pixels;   ///< Vertical pixels in window.
  int m_window_horizontal_chars;  ///< Horizontal ASCII spaces in window.
  int m_window_vertical_chars;    ///< Vertical ASCII spaces in window.
  sf::RectangleShape *m_p_rectangle;    ///< Backing rectangle for under text.
  sf::Color m_window_background_color;  ///< Background color of window.
  sf::Text *m_p_text;                   ///< ASCII character to draw.
  int m_shake_duration;		        ///< Time left to shake (frames).
  int m_shake_scale_x, m_shake_scale_y; ///< Severity of shake (pixels).
  int m_shake_x, m_shake_y;		///< Shake (this frame).

 public:
  /// Get the one and only instance of the DisplayManager.
  static DisplayManager &getInstance();

  /// Open graphics window, ready for text-based display.
  /// Return 0 if ok, else -1.
  int startUp() override;

  /// Close graphics window.
  void shutDown() override;

  /// Draw character at window location (x,y) with color
  /// and rotation (degrees).
  /// Return 0 if ok, else -1.
  int drawCh(Vector world_pos, char ch, Color color,
	     float rotation=0.0f) const;

  /// Draw character at window location (x,y) with custom color,
  /// and rotation (degrees).
  /// Return 0 if ok, else -1.
  int drawCh(Vector world_pos, char ch, unsigned char r, 
	     unsigned char g, unsigned char b, float rotation=0.0f) const;

  /// Draw string at window location (x,y) with color.
  /// Justified left, center or right.
  /// Return 0 if ok, else -1.
  int drawString(Vector world_pos, std::string str, Justification just, 
                 Color color) const;

  /// Set window's horizontal maximum (in characters).
  void setHorizontal(int new_horizontal);

  /// Return window's horizontal maximum (in characters).
  int getHorizontal() const;

  /// Set window's vertical maximum (in characters).
  void setVertical(int new_vertical);
 
  /// Return window's vertical maximum (in characters).
  int getVertical() const;

  /// Return window's horizontal maximum (in pixels).
  int getHorizontalPixels() const;

  /// Return window's vertical maximum (in pixels).
  int getVerticalPixels() const;

  /// Set window's background color.
  /// Return true if ok, else false.
  bool setBackgroundColor(Color new_color);

  /// "Shake" window with severity scale (pixels) and duration (frames).
  /// Delta true then add else replace.
  void shake(int scale_x, int scale_y, int duration, bool delta=false);

  /// Render current window buffer.
  /// Return 0 if ok, else -1.
  int swapBuffers();

  /// Return pointer to SFML graphics window.
  sf::RenderWindow *getWindow() const;
};

} // end of namespace df
#endif //__DISPLAY_MANAGER_H__
