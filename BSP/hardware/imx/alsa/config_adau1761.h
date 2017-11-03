/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/* Copyright (C) 2012 Freescale Semiconductor, Inc. */

#ifndef ANDROID_INCLUDE_IMX_CONFIG_ADAU1761_H
#define ANDROID_INCLUDE_IMX_CONFIG_ADAU1761_H

#include "audio_hardware.h"

#define MIXER_ADAU1761_SPEAKER_VOLUME  "Lineout Playback Volume"
#define MIXER_ADAU1761_INPUT2_CAPTURE_VOLUME  "Input 2 Capture Volume"
#define MIXER_ADAU1761_INPUT4_CAPTURE_VOLUME  "Input 4 Capture Volume"

static struct route_setting bt_output_adau1761[] = {
    {
        .ctl_name = NULL,
    },
};

static struct route_setting speaker_output_adau1761[]={
        {
            .ctl_name = MIXER_ADAU1761_SPEAKER_VOLUME,
            .intval = 57,
        },
        {
            .ctl_name = NULL,
        },
};

static struct route_setting mm_main_mic_input_adau1761[]={
        {
            .ctl_name = MIXER_ADAU1761_INPUT2_CAPTURE_VOLUME,
            .intval = 0,
        },
        {
            .ctl_name = MIXER_ADAU1761_INPUT4_CAPTURE_VOLUME,
            .intval = 0,
        },
        {
            .ctl_name = NULL,
        },
};

static struct route_setting mm_bt_mic_input_adau1761[] = {
    {
        .ctl_name = NULL,
    },
};

/* ALSA cards for IMX, these must be defined according different board / kernel config*/
static struct audio_card  adau1761_card = {
    .name = "musio-adau1761",
    .driver_name = "musio-adau1761",
    .supported_out_devices =  (AUDIO_DEVICE_OUT_SPEAKER |
                               AUDIO_DEVICE_OUT_WIRED_HEADPHONE |
                               AUDIO_DEVICE_OUT_ALL_SCO |
                               AUDIO_DEVICE_OUT_DEFAULT),
    .supported_in_devices = (AUDIO_DEVICE_IN_WIRED_HEADSET |
    							AUDIO_DEVICE_IN_BUILTIN_MIC |
    							AUDIO_DEVICE_IN_ALL_SCO |
    							AUDIO_DEVICE_IN_DEFAULT), 
    .defaults            = NULL,
    .bt_output           = bt_output_adau1761,
    .speaker_output      = speaker_output_adau1761,
    .hs_output           = NULL,
    .earpiece_output     = NULL,
    .vx_hs_mic_input     = NULL,
    .mm_main_mic_input   = mm_main_mic_input_adau1761,
    .vx_main_mic_input   = NULL,
    .mm_hs_mic_input     = NULL,
    .vx_bt_mic_input     = NULL,
    .mm_bt_mic_input     = mm_bt_mic_input_adau1761,
    .card                = 0,
    .out_rate            = 0,
    .out_channels        = 0,
    .out_format          = 0,
    .in_rate             = 0,
    .in_channels         = 0,
    .in_format           = 0,
};

#endif  /* ANDROID_INCLUDE_IMX_CONFIG_ADAU1761_H */
