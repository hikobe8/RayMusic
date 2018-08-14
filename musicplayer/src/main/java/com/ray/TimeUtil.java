package com.ray;

import java.text.SimpleDateFormat;
import java.util.Locale;

/***
 *  Author : ryu18356@gmail.com
 *  Create at 2018-08-14 16:18
 *  description : 时间转换工具类
 */
public class TimeUtil {

    /**
     * 获取hh:ss格式的时间
     * @param seconds 秒数
     * @return hh:ss
     */
    public static String getMMssTime(long seconds) {
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("mm:ss", Locale.getDefault());
        return simpleDateFormat.format(seconds*1000);
    }

}
