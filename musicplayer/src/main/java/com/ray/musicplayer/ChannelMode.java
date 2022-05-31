package com.ray.musicplayer;

import androidx.annotation.IntDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

@IntDef({ChannelMode.CHANNEL_RIGHT, ChannelMode.CHANNEL_LEFT, ChannelMode.CHANNEL_STEREO})
@Retention(RetentionPolicy.SOURCE)
public @interface ChannelMode {
    int CHANNEL_LEFT = 0;
    int CHANNEL_RIGHT = 1;
    int CHANNEL_STEREO = 2;
}
