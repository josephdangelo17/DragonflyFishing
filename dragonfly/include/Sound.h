///
/// The sound
/// 

#ifndef __SOUND_H__
#define __SOUND_H__

// System includes.
#include <string>
#include <SFML/Audio.hpp>

namespace df {

class Sound {

 private:
#ifndef NO_SOUND
  sf::Sound *m_p_sound;	          ///< SFML sound.
  sf::SoundBuffer m_sound_buffer; ///< SFML sound buffer associated with sound.
#endif
  std::string m_label;	          ///< Text label to identify sound.

 public:
  Sound();
  ~Sound();

  /// Load sound buffer from file.
  /// Return 0 if ok, else -1.
  int loadSound(std::string filename);

  /// Set label associated with sound.
  void setLabel(std::string new_label);

  /// Get label associated with sound.
  std::string getLabel() const;

  /// Play sound.
  /// If loop is true, repeat play when done.
  void play(bool loop=false);

  /// Stop sound.
  void stop();

  /// Pause sound.
  void pause();

  /// Return SFML sound.
#ifndef NO_SOUND
  sf::Sound getSound() const;
#endif
};

} // end of namespace df
#endif // __SOUND_H__
