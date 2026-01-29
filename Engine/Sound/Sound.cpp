#include "Sound/Sound.h"

#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_properties.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "Utils/Assert.h"

#include <cstdlib>

namespace sound
{
	struct TrackData
	{
		MIX_Audio* audio = nullptr;
		MIX_Track* track = nullptr;	
	};

	MIX_Mixer* g_pMixer = nullptr;

	void Open(const AudioSystemSpecs& specs)
	{
		ASSERT(MIX_Init(), "FAILED. SDL failed to initialize mixer!");

		SDL_AudioSpec mixer;
		mixer.freq = specs.freq;
		mixer.channels = specs.channels;
		mixer.format = SDL_AUDIO_F32;

		g_pMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &mixer);
		MIX_SetMixerGain(g_pMixer, specs.volume);
	}

	void Close()
	{
		MIX_DestroyMixer(g_pMixer);
		MIX_Quit();
	}

	SFX LoadSFX(const char* path)
	{
		MIX_Audio* audio = MIX_LoadAudio(g_pMixer, path, false);
		return (SFX)audio;
	}

	Track LoadTrack(const char* path)
	{
		MIX_Audio* audio = MIX_LoadAudio(g_pMixer, path, false);
		MIX_Track* track = MIX_CreateTrack(g_pMixer);
		MIX_SetTrackAudio(track, audio);

		TrackData* data = (TrackData*)malloc(sizeof(TrackData));
		data->audio = audio;
		data->track = track;

		return (Track)data;
	}

	void DestroySFX(SFX sfx)
	{
		auto audio = static_cast<MIX_Audio*>(sfx);
		MIX_DestroyAudio(audio);
	}

	void DestroyTrack(Track track)
	{
		auto data = static_cast<TrackData*>(track);
		MIX_DestroyTrack(data->track);
		MIX_DestroyAudio(data->audio);
		free(data);
	}

	void PlaySFX(SFX sfx)
	{
		auto data = static_cast<MIX_Audio*>(sfx);
		ASSERT(MIX_PlayAudio(g_pMixer, data), "FAILED. SDL mixer failed to play SFX audio!");
	}

	void PlayTrack(Track track, int loop)
	{
		auto data = static_cast<TrackData*>(track);

		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loop);

		ASSERT(MIX_PlayTrack(data->track, props), "FAILED. SDL mixer failed to play Track audio!");
	}

	void StopTrack(Track track, int fadoutMS)
	{
		auto data = static_cast<TrackData*>(track);

		Sint64 fadeFrames = MIX_AudioMSToFrames(data->audio, fadoutMS);
		ASSERT(MIX_StopTrack(data->track, fadeFrames), "FAILED. SDL mixer failed to stop Track audio");
	}
}