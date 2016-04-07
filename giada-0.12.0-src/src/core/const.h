/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2016 Giovanni A. Zuliani | Monocasual
 *
 * This file is part of Giada - Your Hardcore Loopmachine.
 *
 * Giada - Your Hardcore Loopmachine is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Giada - Your Hardcore Loopmachine is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giada - Your Hardcore Loopmachine. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */




#ifndef CONST_H
#define CONST_H



#define G_VERSION_STR   "0.12.0"
#define G_APP_NAME      "Giada"
#define G_VERSION_MAJOR 0
#define G_VERSION_MINOR 12
#define G_VERSION_PATCH 0

#define CONF_FILENAME		"giada.conf"

#ifndef BUILD_DATE
# define BUILD_DATE __DATE__
#endif



#ifdef _WIN32
	#define GUI_SLEEP			1000/24
#else
	#define GUI_SLEEP			1000000/24 
#endif
#define GUI_WIDTH				   816
#define GUI_HEIGHT			   510
#define GUI_PLUGIN_RATE    0.05  
#define GUI_FONT_SIZE_BASE 12

#define COLOR_BD_0     fl_rgb_color(78,  78,  78)			
#define COLOR_BD_1     fl_rgb_color(188, 188, 188)    
#define COLOR_BG_0     fl_rgb_color(37,  37,  37)     
#define COLOR_BG_1     fl_rgb_color(78,  78,  78)     
#define COLOR_BG_2     fl_rgb_color(177, 142, 142)    
#define COLOR_BG_3     fl_rgb_color(28,  32,  80)     
#define COLOR_BG_4     fl_rgb_color(113, 31,  31)     
#define COLOR_ALERT    fl_rgb_color(239, 75,  53)     
#define COLOR_TEXT_0   fl_rgb_color(200, 200, 200)
#define COLOR_TEXT_1   fl_rgb_color(25,  25,  25)      
#define COLOR_BG_MAIN  fl_rgb_color(25,  25,  25)		   
#define COLOR_BG_DARK  fl_rgb_color(0,   0,   0)		   




#define MAX_BEATS				   32
#define MAX_BARS				   32
#define MAX_PATCHNAME_LEN	 32
#define DB_MIN_SCALE		   60.0f
#define MAX_VST_EVENTS     32
#define MIN_COLUMN_WIDTH   140




#define SYS_API_NONE		0x00  
#define SYS_API_JACK		0x01  
#define SYS_API_ALSA		0x02  
#define SYS_API_DS			0x04  
#define SYS_API_ASIO		0x08  
#define SYS_API_CORE		0x10  
#define SYS_API_PULSE   0x20  
#define SYS_API_ANY     0x3F  

#define KERNEL_OK					 0
#define KERNEL_UNDERRUN	  -1
#define KERNEL_CRITICAL	  -2




#define MIDI_API_JACK		0x01  
#define MIDI_API_ALSA		0x02  




#if defined(__linux__)
	#define DEFAULT_SOUNDSYS	SYS_API_NONE
#elif defined(_WIN32)
	#define DEFAULT_SOUNDSYS 	SYS_API_DS
#elif defined(__APPLE__)
	#define DEFAULT_SOUNDSYS 	SYS_API_CORE
#endif

#define DEFAULT_SOUNDDEV_OUT  0       
#define DEFAULT_SOUNDDEV_IN   -1			
#define DEFAULT_MIDI_SYSTEM   0
#define DEFAULT_MIDI_PORT_IN  -1
#define DEFAULT_MIDI_PORT_OUT -1
#define DEFAULT_SAMPLERATE   44100
#define DEFAULT_BUFSIZE		   1024
#define DEFAULT_DELAYCOMP		 0
#define DEFAULT_VOL				   0.0f
#define DEFAULT_BOOST			   0.0f
#define gDEFAULT_PITCH			 1.0f   
#define DEFAULT_OUT_VOL  	   1.0f
#define DEFAULT_IN_VOL  	   0.0f
#define DEFAULT_CHANMODE	   SINGLE_BASIC
#define DEFAULT_BPM				   120.0f
#define DEFAULT_BEATS			   4
#define DEFAULT_BARS			   1
#define DEFAULT_QUANTIZE     0		  
#define DEFAULT_FADEOUT_STEP 0.01f  
#define DEFAULT_COLUMN_WIDTH 380




#define LOOP_BASIC			 0x01 
#define LOOP_ONCE				 0x02 
#define	SINGLE_BASIC		 0x04 
#define SINGLE_PRESS		 0x08 
#define SINGLE_RETRIG		 0x10 
#define LOOP_REPEAT			 0x20 
#define SINGLE_ENDLESS   0x40 
#define LOOP_ONCE_BAR    0x80 

#define LOOP_ANY				 0xA3 
#define SINGLE_ANY		   0x5C 

#define	STATUS_ENDING		 0x01 
#define	STATUS_WAIT			 0x02 
#define	STATUS_PLAY			 0x04 
#define STATUS_OFF			 0x08 
#define STATUS_EMPTY	   0x10 
#define STATUS_MISSING   0x20 
#define STATUS_WRONG     0x40 

#define REC_WAITING			 0x01 
#define REC_ENDING       0x02 
#define REC_READING      0x04 
#define REC_STOPPED      0x08 




#define ACTION_KEYPRESS		0x01 
#define ACTION_KEYREL			0x02 
#define ACTION_KILLCHAN		0x04 
#define ACTION_MUTEON			0x08 
#define ACTION_MUTEOFF		0x10 
#define ACTION_VOLUME     0x20 
#define ACTION_MIDI       0x40 

#define ACTION_KEYS       0x03 
#define ACTION_MUTES      0x24 

#define RANGE_CHAR        0x01 
#define RANGE_FLOAT       0x02 




#define SAMPLE_LOADED_OK      0x01
#define SAMPLE_LEFT_EMPTY     0x02
#define SAMPLE_NOT_VALID      0x04
#define SAMPLE_MULTICHANNEL   0x08
#define SAMPLE_WRONG_BIT      0x10
#define SAMPLE_WRONG_ENDIAN   0x20
#define SAMPLE_WRONG_FORMAT   0x40
#define SAMPLE_READ_ERROR     0x80
#define SAMPLE_PATH_TOO_LONG  0x100






#define LOG_MODE_STDOUT 0x01
#define LOG_MODE_FILE   0x02
#define LOG_MODE_MUTE   0x04




#define BROWSER_LOAD_PATCH   0x00
#define BROWSER_LOAD_SAMPLE  0x01
#define BROWSER_SAVE_PATCH   0x02
#define BROWSER_SAVE_SAMPLE  0x04
#define BROWSER_SAVE_PROJECT 0x08
#define BROWSER_LOAD_PLUGIN  0x10




#define CHANNEL_SAMPLE 0x01
#define CHANNEL_MIDI   0x02





#define WID_BEATS         -1
#define WID_BPM           -2
#define WID_ABOUT         -3
#define WID_FILE_BROWSER  -4
#define WID_CONFIG        -5
#define WID_FX_LIST       -6
#define WID_ACTION_EDITOR -7
#define WID_SAMPLE_EDITOR -8
#define WID_FX            -9
#define WID_KEY_GRABBER   -10




#define PATCH_UNREADABLE    0x01
#define PATCH_INVALID       0x02
#define PATCH_READ_OK       0x04
#define PATCH_WRONG_PLUGINS 0x08  
#define PATCH_WRONG_SAMPLES 0x10  




#define MIDIMAP_NOT_SPECIFIED 0x00
#define MIDIMAP_UNREADABLE    0x01
#define MIDIMAP_INVALID       0x02
#define MIDIMAP_READ_OK       0x04







#define MIDI_CONTROLLER     0xB0 << 24
#define MIDI_NOTE_ON        0x90 << 24
#define MIDI_NOTE_OFF       0x80 << 24
#define MIDI_ALL_NOTES_OFF (MIDI_CONTROLLER) | (0x7B << 16)
#define MIDI_VOLUME        (MIDI_CONTROLLER) | (0x07 << 16)



#define MIDI_SYSEX          0xF0
#define MIDI_MTC_QUARTER    0xF1
#define MIDI_POSITION_PTR   0xF2
#define MIDI_CLOCK          0xF8
#define MIDI_START          0xFA
#define MIDI_CONTINUE       0xFB
#define MIDI_STOP           0xFC
#define MIDI_EOX            0xF7  



#define MIDI_CHAN_0         0x00 << 24
#define MIDI_CHAN_1         0x01 << 24
#define MIDI_CHAN_2         0x02 << 24
#define MIDI_CHAN_3         0x03 << 24
#define MIDI_CHAN_4         0x04 << 24
#define MIDI_CHAN_5         0x05 << 24
#define MIDI_CHAN_6         0x06 << 24
#define MIDI_CHAN_7         0x07 << 24
#define MIDI_CHAN_8         0x08 << 24
#define MIDI_CHAN_9         0x09 << 24
#define MIDI_CHAN_10        0x0A << 24
#define MIDI_CHAN_11        0x0B << 24
#define MIDI_CHAN_12        0x0C << 24
#define MIDI_CHAN_13        0x0D << 24
#define MIDI_CHAN_14        0x0E << 24
#define MIDI_CHAN_15        0x0F << 24

const int MIDI_CHANS[16] = {
	MIDI_CHAN_0,  MIDI_CHAN_1,	MIDI_CHAN_2,  MIDI_CHAN_3,
	MIDI_CHAN_4,  MIDI_CHAN_5,	MIDI_CHAN_6,  MIDI_CHAN_7,
	MIDI_CHAN_8,  MIDI_CHAN_9,	MIDI_CHAN_10, MIDI_CHAN_11,
	MIDI_CHAN_12, MIDI_CHAN_13,	MIDI_CHAN_14, MIDI_CHAN_15
};



#define MIDI_SYNC_NONE      0x00
#define MIDI_SYNC_CLOCK_M   0x01
#define MIDI_SYNC_CLOCK_S   0x02
#define MIDI_SYNC_MTC_M     0x04
#define MIDI_SYNC_MTC_S     0x08



#define PATCH_KEY_HEADER                       "header"
#define PATCH_KEY_VERSION                      "version"
#define PATCH_KEY_VERSION_MAJOR                "version_major"
#define PATCH_KEY_VERSION_MINOR                "version_minor"
#define PATCH_KEY_VERSION_PATCH                "version_patch"
#define PATCH_KEY_NAME                         "name"
#define PATCH_KEY_BPM                          "bpm"
#define PATCH_KEY_BARS                         "bars"
#define PATCH_KEY_BEATS                        "beats"
#define PATCH_KEY_QUANTIZE                     "quantize"
#define PATCH_KEY_MASTER_VOL_IN                "master_vol_in"
#define PATCH_KEY_MASTER_VOL_OUT               "master_vol_out"
#define PATCH_KEY_METRONOME                    "metronome"
#define PATCH_KEY_LAST_TAKE_ID                 "last_take_id"
#define PATCH_KEY_SAMPLERATE                   "samplerate"
#define PATCH_KEY_COLUMNS                      "columns"
#define PATCH_KEY_MASTER_OUT_PLUGINS           "master_out_plugins"
#define PATCH_KEY_MASTER_IN_PLUGINS            "master_in_plugins"
#define PATCH_KEY_CHANNELS                     "channels"
#define PATCH_KEY_CHANNEL_TYPE                 "type"
#define PATCH_KEY_CHANNEL_INDEX                "index"
#define PATCH_KEY_CHANNEL_COLUMN               "column"
#define PATCH_KEY_CHANNEL_MUTE                 "mute"
#define PATCH_KEY_CHANNEL_MUTE_S               "mute_s"
#define PATCH_KEY_CHANNEL_SOLO                 "solo"
#define PATCH_KEY_CHANNEL_VOLUME               "volume"
#define PATCH_KEY_CHANNEL_PAN_LEFT             "pan_left"
#define PATCH_KEY_CHANNEL_PAN_RIGHT            "pan_right"
#define PATCH_KEY_CHANNEL_MIDI_IN              "midi_in"
#define PATCH_KEY_CHANNEL_MIDI_IN_KEYPRESS     "midi_in_keypress"
#define PATCH_KEY_CHANNEL_MIDI_IN_KEYREL       "midi_in_keyrel"
#define PATCH_KEY_CHANNEL_MIDI_IN_KILL         "midi_in_kill"
#define PATCH_KEY_CHANNEL_MIDI_IN_VOLUME       "midi_in_volume"
#define PATCH_KEY_CHANNEL_MIDI_IN_MUTE         "midi_in_mute"
#define PATCH_KEY_CHANNEL_MIDI_IN_SOLO         "midi_in_solo"
#define PATCH_KEY_CHANNEL_MIDI_OUT_L           "midi_out_l"
#define PATCH_KEY_CHANNEL_MIDI_OUT_L_PLAYING   "midi_out_l_playing"
#define PATCH_KEY_CHANNEL_MIDI_OUT_L_MUTE      "midi_out_l_mute"
#define PATCH_KEY_CHANNEL_MIDI_OUT_L_SOLO      "midi_out_l_solo"
#define PATCH_KEY_CHANNEL_SAMPLE_PATH          "sample_path"
#define PATCH_KEY_CHANNEL_KEY                  "key"
#define PATCH_KEY_CHANNEL_MODE                 "mode"
#define PATCH_KEY_CHANNEL_BEGIN                "begin"
#define PATCH_KEY_CHANNEL_END                  "end"
#define PATCH_KEY_CHANNEL_BOOST                "boost"
#define PATCH_KEY_CHANNEL_REC_ACTIVE           "rec_active"
#define PATCH_KEY_CHANNEL_PITCH                "pitch"
#define PATCH_KEY_CHANNEL_MIDI_IN_READ_ACTIONS "midi_in_read_actions"
#define PATCH_KEY_CHANNEL_MIDI_IN_PITCH        "midi_in_pitch"
#define PATCH_KEY_CHANNEL_MIDI_OUT             "midi_out"
#define PATCH_KEY_CHANNEL_MIDI_OUT_CHAN        "midi_out_chan"
#define PATCH_KEY_CHANNEL_PLUGINS              "plugins"
#define PATCH_KEY_CHANNEL_ACTIONS              "actions"
#define PATCH_KEY_ACTION_TYPE                  "type"
#define PATCH_KEY_ACTION_FRAME                 "frame"
#define PATCH_KEY_ACTION_F_VALUE               "f_value"
#define PATCH_KEY_ACTION_I_VALUE               "i_value"
#define PATCH_KEY_PLUGIN_PATH                  "path"
#define PATCH_KEY_PLUGIN_BYPASS                "bypass"
#define PATCH_KEY_PLUGIN_PARAMS                "params"
#define PATCH_KEY_COLUMN_INDEX                 "index"
#define PATCH_KEY_COLUMN_WIDTH                 "width"
#define PATCH_KEY_COLUMN_CHANNELS              "channels"



#define CONF_KEY_HEADER                   "header"
#define CONF_KEY_LOG_MODE                 "log_mode"
#define CONF_KEY_SOUND_SYSTEM             "sound_system"
#define CONF_KEY_SOUND_DEVICE_IN          "sound_device_in"
#define CONF_KEY_SOUND_DEVICE_OUT         "sound_device_out"
#define CONF_KEY_CHANNELS_IN              "channels_in"
#define CONF_KEY_CHANNELS_OUT             "channels_out"
#define CONF_KEY_SAMPLERATE               "samplerate"
#define CONF_KEY_BUFFER_SIZE              "buffer_size"
#define CONF_KEY_DELAY_COMPENSATION       "delay_compensation"
#define CONF_KEY_LIMIT_OUTPUT             "limit_output"
#define CONF_KEY_RESAMPLE_QUALITY         "resample_quality"
#define CONF_KEY_MIDI_SYSTEM              "midi_system"
#define CONF_KEY_MIDI_PORT_OUT            "midi_port_out"
#define CONF_KEY_MIDI_PORT_IN             "midi_port_in"
#define CONF_KEY_NO_NOTE_OFF              "no_note_off"
#define CONF_KEY_MIDIMAP_PATH             "midimap_path"
#define CONF_KEY_LAST_MIDIMAP             "last_midimap"
#define CONF_KEY_MIDI_SYNC                "midi_sync"
#define CONF_KEY_MIDI_TC_FPS              "midi_tc_fps"
#define CONF_KEY_MIDI_IN_REWIND           "midi_in_rewind"
#define CONF_KEY_MIDI_IN_START_STOP       "midi_in_start_stop"
#define CONF_KEY_MIDI_IN_ACTION_REC       "midi_in_action_rec"
#define CONF_KEY_MIDI_IN_INPUT_REC        "midi_in_input_rec"
#define CONF_KEY_MIDI_IN_METRONOME        "midi_in_metronome"
#define CONF_KEY_MIDI_IN_VOLUME_IN        "midi_in_volume_in"
#define CONF_KEY_MIDI_IN_VOLUME_OUT       "midi_in_volume_out"
#define CONF_KEY_MIDI_IN_BEAT_DOUBLE      "midi_in_beat_doble"
#define CONF_KEY_MIDI_IN_BEAT_HALF        "midi_in_beat_half"
#define CONF_KEY_RECS_STOP_ON_CHAN_HALT   "recs_stop_on_chan_halt"
#define CONF_KEY_CHANS_STOP_ON_SEQ_HALT   "chans_stop_on_seq_halt"
#define CONF_KEY_TREAT_RECS_AS_LOOPS      "treat_recs_as_loops"
#define CONF_KEY_RESIZE_RECORDINGS        "resize_recordings"
#define CONF_KEY_PLUGINS_PATH             "plugins_path"
#define CONF_KEY_PATCHES_PATH             "patches_path"
#define CONF_KEY_SAMPLES_PATH             "samples_path"
#define CONF_KEY_MAIN_WINDOW_X            "main_window_x"
#define CONF_KEY_MAIN_WINDOW_Y            "main_window_y"
#define CONF_KEY_MAIN_WINDOW_W            "main_window_w"
#define CONF_KEY_MAIN_WINDOW_H            "main_window_h"
#define CONF_KEY_BROWSER_X                "browser_x"
#define CONF_KEY_BROWSER_Y                "browser_y"
#define CONF_KEY_BROWSER_W                "browser_w"
#define CONF_KEY_BROWSER_H                "browser_h"
#define CONF_KEY_ACTION_EDITOR_X          "action_editor_x"
#define CONF_KEY_ACTION_EDITOR_Y          "action_editor_y"
#define CONF_KEY_ACTION_EDITOR_W          "action_editor_w"
#define CONF_KEY_ACTION_EDITOR_H          "action_editor_h"
#define CONF_KEY_ACTION_EDITOR_ZOOM       "action_editor_zoom"
#define CONF_KEY_ACTION_EDITOR_GRID_VAL   "action_editor_grid_val"
#define CONF_KEY_ACTION_EDITOR_GRID_ON    "action_editor_grid_on"
#define CONF_KEY_SAMPLE_EDITOR_X          "sample_editor_x"
#define CONF_KEY_SAMPLE_EDITOR_Y          "sample_editor_y"
#define CONF_KEY_SAMPLE_EDITOR_W          "sample_editor_w"
#define CONF_KEY_SAMPLE_EDITOR_H          "sample_editor_h"
#define CONF_KEY_SAMPLE_EDITOR_GRID_VAL   "sample_editor_grid_val"
#define CONF_KEY_SAMPLE_EDITOR_GRID_ON    "sample_editor_grid_on"
#define CONF_KEY_PIANO_ROLL_Y             "piano_roll_y"
#define CONF_KEY_PIANO_ROLL_H             "piano_roll_h"
#define CONF_KEY_PLUGIN_LIST_X            "plugin_list_x"
#define CONF_KEY_PLUGIN_LIST_Y            "plugin_list_y"
#define CONF_KEY_CONFIG_X                 "config_x"
#define CONF_KEY_CONFIG_Y                 "config_y"
#define CONF_KEY_BPM_X                    "bpm_x"
#define CONF_KEY_BPM_Y                    "bpm_y"
#define CONF_KEY_BEATS_X                  "beats_x"
#define CONF_KEY_BEATS_Y                  "beats_y"
#define CONF_KEY_ABOUT_X                  "about_x"
#define CONF_KEY_ABOUT_Y                  "about_y"



#define MIDIMAP_KEY_BRAND          "brand"
#define MIDIMAP_KEY_DEVICE         "device"
#define MIDIMAP_KEY_INIT_COMMANDS  "init_commands"
#define MIDIMAP_KEY_MUTE_ON        "mute_on"
#define MIDIMAP_KEY_MUTE_OFF       "mute_off"
#define MIDIMAP_KEY_SOLO_ON        "solo_on"
#define MIDIMAP_KEY_SOLO_OFF       "solo_off"
#define MIDIMAP_KEY_WAITING        "waiting"
#define MIDIMAP_KEY_PLAYING        "playing"
#define MIDIMAP_KEY_STOPPING       "stopping"
#define MIDIMAP_KEY_STOPPED        "stopped"
#define MIDIMAP_KEY_CHANNEL        "channel"
#define MIDIMAP_KEY_MESSAGE        "message"

#endif
