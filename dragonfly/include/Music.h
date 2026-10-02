///
/// The music
/// 

#ifndef __MUSIC_H__
#define __MUSIC_H__

// System includes.
#include <string>
#include <SFML/Audio.hpp>

namespace df {

class Music {

 private:
#ifndef NO_SOUND
  sf::Music m_music;	        ///< The SFML music.
#endif
  std::string m_label;	        ///< Text label to identify music.
  Music(Music const&);          ///< SFML doesn't allow music copy.
  void operator=(Music const&); ///< SFML doesn't allow music assignment.

 public:
  Music();

  /// Associate music buffer with file.
  /// Return 0 if ok, else -1.
  int loadMusic(std::string filename);

  /// Set label associated with music.
  void setLabel(std::string new_label);

  /// Get label associated with music.
  std::string getLabel() const;

  /// Play music.
  /// If loop is true, repeat play when done.
  void play(bool loop=true);

  /// Stop music.
  void stop();

  /// Pause music.
  void pause();

  /// Return pointer to SFML music.
#ifndef NO_SOUND
  sf::Music *getMusic();
#endif
};

} // end of namespace df
#endif // __MUSIC_H__
