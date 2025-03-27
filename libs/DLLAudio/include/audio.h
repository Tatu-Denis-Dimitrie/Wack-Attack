#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>

#ifdef AUDIO_DLL_EXPORTS
#define AUDIO_DLL_API __declspec(dllexport)
#else
#define AUDIO_DLL_API __declspec(dllimport)
#endif

class AUDIO_DLL_API Audio {
public:
	Audio();
	~Audio();

	bool Init();
	void PlayMusic(const std::string& file);
	void StopMusic();
	void PauseMusic();
	void ResumeMusic();

	void PlaySoundEffect(const std::string& file);

private:
	Mix_Music* music;
};
