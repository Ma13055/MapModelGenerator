// -*- Java -*-
/*!
 * @file  controlPanelImpl.java
 * @brief 処理中のマウス操作情報を取得する
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * 		   Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

import jp.go.aist.rtm.RTC.Manager;
import jp.go.aist.rtm.RTC.RTObject_impl;
import jp.go.aist.rtm.RTC.RtcDeleteFunc;
import jp.go.aist.rtm.RTC.RtcNewFunc;
import jp.go.aist.rtm.RTC.RegisterModuleFunc;
import jp.go.aist.rtm.RTC.util.Properties;

/*!
 * @class controlPanel
 * @brief 処理中のマウス操作情報を取得する
 */
public class controlPanel implements RtcNewFunc, RtcDeleteFunc, RegisterModuleFunc {

//  Module specification
//  <rtc-template block="module_spec">
    public static String component_conf[] = {
    	    "implementation_id", "controlPanel",
    	    "type_name",         "controlPanel",
    	    "description",       "Get mouse event",
    	    "version",           "1.3.0",
    	    "vendor",            "Masaru Tatekawa(SIT)",
    	    "category",          "UI",
    	    "activity_type",     "UNIQUE",
    	    "max_instance",      "1",
    	    "language",          "Java",
    	    "lang_type",         "compile",
    	    ""
            };
//  </rtc-template>

    public RTObject_impl createRtc(Manager mgr) {
        return new controlPanelImpl(mgr);
    }

    public void deleteRtc(RTObject_impl rtcBase) {
        rtcBase = null;
    }
    public void registerModule() {
        Properties prop = new Properties(component_conf);
        final Manager manager = Manager.instance();
        manager.registerFactory(prop, new controlPanel(), new controlPanel());
    }
}
