#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <Windows.h>
#include <mmsystem.h>

// Function declaration and definition
inline void PlaySoundFile(const char* filePath) {
    PlaySoundA(filePath, NULL, SND_FILENAME | SND_ASYNC);
}

#endif // SOUNDPLAYER_H
#pragma once
