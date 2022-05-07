package com.ray.musicplayer;

/**
 * Author : Ray
 * Time : 2022/5/7 15:21
 * Description :
 */
public interface PlayerListener {

    void onPlayerPrepared();

    void onPlayerPause();

    void onPlayerResume();

    /**
     * @param loading true 正在加载音频数据
     */
    void onPlayerLoading(boolean loading);

}
