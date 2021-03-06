TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += dll
CONFIG -= static

QMAKE_CFLAGS += -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'

static: {
release:OBJECTS_DIR = ../../bin/_build/sdl2mixermod/_release/.obj
release:MOC_DIR     = ../../bin/_build/sdl2mixermod/_release/.moc
release:RCC_DIR     = ../../bin/_build/sdl2mixermod/_release/.rcc
release:UI_DIR      = ../../bin/_build/sdl2mixermod/_release/.ui

debug:OBJECTS_DIR   = ../../bin/_build/sdl2mixermod/_debug/.obj
debug:MOC_DIR       = ../../bin/_build/sdl2mixermod/_debug/.moc
debug:RCC_DIR       = ../../bin/_build/sdl2mixermod/_debug/.rcc
debug:UI_DIR        = ../../bin/_build/sdl2mixermod/_debug/.ui
} else {
release:OBJECTS_DIR = ../../bin/_build/_dynamic/sdl2mixermod/_release/.obj
release:MOC_DIR     = ../../bin/_build/_dynamic/sdl2mixermod/_release/.moc
release:RCC_DIR     = ../../bin/_build/_dynamic/sdl2mixermod/_release/.rcc
release:UI_DIR      = ../../bin/_build/_dynamic/sdl2mixermod/_release/.ui

debug:OBJECTS_DIR   = ../../bin/_build/_dynamic/sdl2mixermod/_debug/.obj
debug:MOC_DIR       = ../../bin/_build/_dynamic/sdl2mixermod/_debug/.moc
debug:RCC_DIR       = ../../bin/_build/_dynamic/sdl2mixermod/_debug/.rcc
debug:UI_DIR        = ../../bin/_build/_dynamic/sdl2mixermod/_debug/.ui
}


static:{
DESTDIR = ../_builds/sdl2_mixer_mod
TARGET = SDL2_mixer
}else{
DESTDIR = ../_builds/sdl2_mixer_mod
TARGET = SDL2_mixer
}

win32:{
LIBS += -L../_builds/win32/lib
LIBS += -lmingw32 -lSDL2main -mwindows
INCLUDEPATH += ../_builds/win32/include
}
unix:{
LIBS += -L../_builds/linux/lib
INCLUDEPATH += ../_builds/linux/include
}
macx:{
LIBS += -F../_builds/macos/frameworks -framework SDL2
INCLUDEPATH += ../_builds/macos/frameworks/SDL2.framework/Headers
} else {
LIBS += -lSDL2
}

win32:{
LIBS += -lwinmm -lm -lwinmm
}

DEFINES += main=SDL_main HAVE_SIGNAL_H HAVE_SETBUF WAV_MUSIC MID_MUSIC \
USE_TIMIDITY_MIDI OGG_MUSIC FLAC_MUSIC MP3_MAD_MUSIC SPC_MUSIC NO_OLDNAMES SPC_MORE_ACCURACY
DEFINES += MODPLUG_MUSIC

macx||win32: {
DEFINES += USE_NATIVE_MIDI
}

LIBS += -L../_builds/sdl2_mixer_mod

win32:{
    LIBS += -lvorbisfile.dll -lvorbis.dll -lmodplug.dll -lFLAC.dll -logg.dll -static-libgcc -static-libstdc++ -static -lpthread
} else {
    LIBS += -lvorbisfile -lvorbis -lmodplug -lFLAC -logg
}

LIBS += -lmad -lm

unix: {
    SDL2MixerH.path =  ../_builds/linux/include/SDL2
}
win32: {
    SDL2MixerH.path =  ../_builds/win32/include/SDL2
}
SDL2MixerH.files += SDL_mixer.h

unix: {
SDL2MixerSO.path = ../_builds/linux/lib
SDL2MixerSO.files += ../_builds/sdl2_mixer_mod/*.so*
}
win32: {
SDL2MixerSO.path = ../_builds/win32/lib
SDL2MixerSO.files += ../_builds/sdl2_mixer_mod/*.dll
}
INSTALLS = SDL2MixerH SDL2MixerSO


HEADERS += \
    begin_code.h \
    close_code.h \
    dynamic_flac.h \
    dynamic_fluidsynth.h \
    dynamic_mod.h \
    dynamic_modplug.h \
    dynamic_mp3.h \
    dynamic_ogg.h \
    effects_internal.h \
    fluidsynth.h \
    load_aiff.h \
    load_flac.h \
    load_mp3.h \
    load_ogg.h \
    load_voc.h \
    music_cmd.h \
    music_flac.h \
    music_mad.h \
    music_mod.h \
    music_modplug.h \
    music_ogg.h \
    SDL_mixer.h \
    wavestream.h \
    native_midi/native_midi.h \
    native_midi/native_midi_common.h \
    timidity/common.h \
    timidity/config.h \
    timidity/ctrlmode.h \
    timidity/filter.h \
    timidity/instrum.h \
    timidity/mix.h \
    timidity/output.h \
    timidity/playmidi.h \
    timidity/readmidi.h \
    timidity/resample.h \
    timidity/tables.h \
    timidity/timidity.h \
    snes_spc/blargg_common.h \
    snes_spc/blargg_config.h \
    snes_spc/blargg_endian.h \
    snes_spc/blargg_source.h \
    snes_spc/dsp.h \
    snes_spc/SNES_SPC.h \
    snes_spc/spc.h \
    snes_spc/SPC_CPU.h \
    snes_spc/SPC_DSP.h \
    snes_spc/SPC_Filter.h \
    music_spc.h \
    resample/audio.h \
    resample/global.h \
    resample/mad_resample.h

SOURCES += \
    dynamic_flac.c \
    dynamic_fluidsynth.c \
    dynamic_modplug.c \
    dynamic_mp3.c \
    dynamic_ogg.c \
    effect_position.c \
    effect_stereoreverse.c \
    effects_internal.c \
    fluidsynth.c \
    load_aiff.c \
    load_flac.c \
    load_mp3.c \
    load_ogg.c \
    load_voc.c \
    mixer.c \
    music.c \
    music_cmd.c \
    music_flac.c \
    music_mad.c \
    music_mod.c \
    music_modplug.c \
    music_ogg.c \
    wavestream.c \
    native_midi/native_midi_haiku.cpp \
    native_midi/native_midi_common.c \
    native_midi/native_midi_mac.c \
    native_midi/native_midi_macosx.c \
    native_midi/native_midi_win32.c \
    timidity/common.c \
    timidity/filter.c \
    timidity/instrum.c \
    timidity/mix.c \
    timidity/output.c \
    timidity/playmidi.c \
    timidity/readmidi.c \
    timidity/resample.c \
    timidity/sdl_a.c \
    timidity/sdl_c.c \
    timidity/tables.c \
    timidity/timidity.c \
    snes_spc/dsp.cpp \
    snes_spc/SNES_SPC.cpp \
    snes_spc/SNES_SPC_misc.cpp \
    snes_spc/SNES_SPC_state.cpp \
    snes_spc/spc.cpp \
    snes_spc/SPC_DSP.cpp \
    snes_spc/SPC_Filter.cpp \
    music_spc.c \
    resample/mad_resample.c \
    timidity/ctrlmode.c \
    dynamic_mod.c

DISTFILES += \
    timidity/COPYING \
    timidity/FAQ \
    timidity/README

