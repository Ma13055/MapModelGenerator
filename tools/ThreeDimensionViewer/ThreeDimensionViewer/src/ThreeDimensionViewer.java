// -*- Java -*-
/*!
 * @file ThreeDimensionViewer.java
 * @date $Date$
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
 * @class ThreeDimensionViewer
 * @brief 作成されたラインマップを3D化するコンポーネント
 */
public class ThreeDimensionViewer implements RtcNewFunc, RtcDeleteFunc, RegisterModuleFunc {

//  Module specification
//  <rtc-template block="module_spec">
    public static String component_conf[] = {
    	    "implementation_id", "ThreeDimensionViewer",
    	    "type_name",         "ThreeDimensionViewer",
    	    "description",       "Three Dimension Model Viewe",
    	    "version",           "1.1.0",
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
        return new ThreeDimensionViewerImpl(mgr);
    }

    public void deleteRtc(RTObject_impl rtcBase) {
        rtcBase = null;
    }
    public void registerModule() {
        Properties prop = new Properties(component_conf);
        final Manager manager = Manager.instance();
        manager.registerFactory(prop, new ThreeDimensionViewer(), new ThreeDimensionViewer());
    }
}
