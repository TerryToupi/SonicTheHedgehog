#pragma once

namespace sound
{
	typedef void* SFX;
	typedef void* Track;

	struct AudioSystemSpecs
	{
		int freq = 48000;
		int channels = 2;
		float volume = 0.75f;
	};

	void  	Open(const AudioSystemSpecs& specs = AudioSystemSpecs());
	void 	Close();

	SFX 	LoadSFX(const char* path);
	Track 	LoadTrack(const char* path);
	void 	DestroySFX(SFX sfx);
	void 	DestroyTrack(Track track);

	void 	PlaySFX(SFX sfx);
	void 	PlayTrack(Track track, int loop);
	void 	StopTrack(Track track, int fadoutMS);
	void 	SetTrackVolume(Track track, float volume);
}