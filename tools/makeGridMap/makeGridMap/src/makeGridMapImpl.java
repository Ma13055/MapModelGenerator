// -*- Java -*-
/*!
 * @file  makeGridMapImpl.java
 * @brief make GridMap from LineMap
 * @date  $Date$
 *
 * @author 	立川将(Tatekawa Masaru)
 * 			Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

import java.io.File;
import java.util.Arrays;

import sun.swing.MenuItemLayoutHelper.RectSize;
import RTC.Time;
import RTC.TimedString;
import jp.go.aist.rtm.RTC.DataFlowComponentBase;
import jp.go.aist.rtm.RTC.Manager;
import jp.go.aist.rtm.RTC.port.InPort;
import jp.go.aist.rtm.RTC.port.OutPort;
import jp.go.aist.rtm.RTC.util.DataRef;
import RTC.ReturnCode_t;

/*!
 * @class makeGridMapImpl
 * @brief make GridMap from LineMap
 *
 * ラインマップを読み込み、線によって囲まれることによって形成されているオブジェクトに対して
 * 属性を付与することができるコンポーネント
 *
 * 入力
 * lineMapsPath[TimedString] - 属性付与を行うラインマップのパス
 * 出力
 * gridMapsStatusPath[TimedString] - 付与された属性情報を保持しているファイルのパス
 *
 * ラインマップを表示しているパネルに対するクリック情報をトリガーに、
 * そのクリックが指すオブジェクトに属性付与を行う
 * 属性付与は、ダイアログウィンドウを表示し、入力を受け付ける
 * また、すでに付与された属性情報を同操作で確認することができる
 * 上部メニューバーより、ラインマップの読み込みと属性データの読み込み
 * 上部ボタンパネルにより現在表示しているラインマップの属性データを直接読み込むことができる
 *
 */
public class makeGridMapImpl extends DataFlowComponentBase {
	
	  private makeGridMapFrame frame;
	  private RectSize frame_size;	  
	  private TimedString model_path;
	  private Time tm;

  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
	public makeGridMapImpl(Manager manager) {  
        super(manager);
        // <rtc-template block="initializer">
        m_lmaps_path_val = new TimedString();
        m_lmaps_path = new DataRef<TimedString>(m_lmaps_path_val);
        m_lineMapsPathIn = new InPort<TimedString>("lineMapsPath", m_lmaps_path);
        m_gmaps_path_val = new TimedString();
        m_gmaps_path = new DataRef<TimedString>(m_gmaps_path_val);
        m_gridMapsPathOut = new OutPort<TimedString>("gridMapsPath", m_gmaps_path);
        // </rtc-template>

    }

    /**
     *
     * The initialize action (on CREATED->ALIVE transition)
     * formaer rtc_init_entry() 
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
    @Override
    protected ReturnCode_t onInitialize() {
        // Registration: InPort/OutPort/Service
        // <rtc-template block="registration">
        // Set InPort buffers
        addInPort("lineMapsPath", m_lineMapsPathIn);
        
        // Set OutPort buffer
        addOutPort("gridMapsPath", m_gridMapsPathOut);
        // </rtc-template>
        return super.onInitialize();
    }

    /***
     *
     * The finalize action (on ALIVE->END transition)
     * formaer rtc_exiting_entry()
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
//    @Override
//    protected ReturnCode_t onFinalize() {
//        return super.onFinalize();
//    }

    /***
     *
     * The startup action when ExecutionContext startup
     * former rtc_starting_entry()
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
//    @Override
//    protected ReturnCode_t onStartup(int ec_id) {
//        return super.onStartup(ec_id);
//    }

    /***
     *
     * The shutdown action when ExecutionContext stop
     * former rtc_stopping_entry()
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
//    @Override
//    protected ReturnCode_t onShutdown(int ec_id) {
//        return super.onShutdown(ec_id);
//    }

    /***
     * 初期化を行う
     *
     * The activated action (Active state entry action)
     * former rtc_active_entry()
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
    @Override
    protected ReturnCode_t onActivated(int ec_id) {
        System.out.println("makeGridStatus : onActivated : START");
 //   	while(m_lineMapsPathIn.isNew())m_lineMapsPathIn.read();
    	
		frame = new makeGridMapFrame();
		frame.buildMenus();
		frame.buildContent();
		frame.pack();
		frame.setVisible(true);
		
		frame_size = new RectSize();
		frame_size.setHeight(frame.getHeight());
		frame_size.setWidth(frame.getWidth());
		
		int width = frame.model_panel.windowSize.width + 16;
		int height = frame.model_panel.windowSize.height + frame_size.getHeight();
		
		frame.setSize(width,height);
    	frame.repaint();
	
    	model_path = new TimedString();
    	model_path.data = "";
    	
		File newdir = new File("c:\\tmp");
		newdir.mkdir();
		newdir = new File("C:\\tmp\\makeMapsModel");
		newdir.mkdir();
		newdir = new File("C:\\tmp\\makeMapsModel\\sub");
		newdir.mkdir();
		newdir = new File("C:\\tmp\\makeMapsModel\\maps");
		newdir.mkdir();
		newdir = new File("C:\\tmp\\makeMapsModel\\maps\\gridmaps");
		newdir.mkdir();

    	
        System.out.println("makeGridStatus : onActivated : END");
    	
        return super.onActivated(ec_id);
    }

    /***
     * フレームを削除する
     *
     * The deactivated action (Active state exit action)
     * former rtc_active_exit()
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
    @Override
    protected ReturnCode_t onDeactivated(int ec_id) {
        System.out.println("makeGridStatus : onDeactivated : START");
    	
    	frame.setVisible(false);
    	frame.removeAll();
    	frame = null;

        System.out.println("makeGridStatus : onDeactivated : END"); 
        
        return super.onDeactivated(ec_id);
    }

    /***
     * 属性付与及びグリッドマップ変換を行う
     *
     * The execution action that is invoked periodically
     * former rtc_active_do()
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
    @Override
    protected ReturnCode_t onExecute(int ec_id) {
    	
    	//入力ポートにファイルパスを受け取った場合
    	if(m_lineMapsPathIn.isNew()){
    		m_lineMapsPathIn.read();
    		if(!model_path.data.equals(m_lmaps_path.v.data)){
    			model_path = m_lmaps_path.v;
    			String[] str_array = model_path.data.split(",",0);
    			File[] files = null;
    			for(int i=0;i<str_array.length;i++){
    				if(files == null){
    					files = new File[1];
    					files[0] = new File(str_array[i]);
    				}else{
    					files = Arrays.copyOf(files, files.length+1);
    					files[files.length -1] = new File(str_array[i]);
    				}
    			}
    			frame.menue_bar.readModelData(files);
    		}
    	}
    	
    	if(frame.button_panel.select_save_flag == true){
    		tm = makeRTCTime();
    		
    		frame.button_panel.select_save_flag = false;
    	}
    	
    	if(frame.button_panel.make_save_path == true){
        	TimedString ts = new TimedString(tm,frame.save_file_path);
        	m_gridMapsPathOut.write(ts);

    		frame.button_panel.make_save_path = false;
    	}

    	Runtime rt = Runtime.getRuntime();
    	rt.gc();

        return super.onExecute(ec_id);
    }

    /***
     *
     * The aborting action when main logic error occurred.
     * former rtc_aborting_entry()
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
//  @Override
//  public ReturnCode_t onAborting(int ec_id) {
//      return super.onAborting(ec_id);
//  }

    /***
     *
     * The error action in ERROR state
     * former rtc_error_do()
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
//    @Override
//    public ReturnCode_t onError(int ec_id) {
//        return super.onError(ec_id);
//    }

    /***
     * フレームを削除する
     *
     * The reset action that is invoked resetting
     * This is same but different the former rtc_init_entry()
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
    @Override
    protected ReturnCode_t onReset(int ec_id) {
        System.out.println("makeGridStatus : onReset : START");
    	
    	frame.setVisible(false);
    	frame.removeAll();
    	frame = null;

        System.out.println("makeGridStatus : onReset : END");    	
        return super.onReset(ec_id);
    }

    /***
     *
     * The state update action that is invoked after onExecute() action
     * no corresponding operation exists in OpenRTm-aist-0.2.0
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
//    @Override
//    protected ReturnCode_t onStateUpdate(int ec_id) {
//        return super.onStateUpdate(ec_id);
//    }

    /***
     *
     * The action that is invoked when execution context's rate is changed
     * no corresponding operation exists in OpenRTm-aist-0.2.0
     *
     * @param ec_id target ExecutionContext Id
     *
     * @return RTC::ReturnCode_t
     * 
     * 
     */
//    @Override
//    protected ReturnCode_t onRateChanged(int ec_id) {
//        return super.onRateChanged(ec_id);
//    }
//
    // DataInPort declaration
    // <rtc-template block="inport_declare">
    protected TimedString m_lmaps_path_val;
    protected DataRef<TimedString> m_lmaps_path;
    /*!
     */
    protected InPort<TimedString> m_lineMapsPathIn;

    
    // </rtc-template>

    // DataOutPort declaration
    // <rtc-template block="outport_declare">
    protected TimedString m_gmaps_path_val;
    protected DataRef<TimedString> m_gmaps_path;
    /*!
     */
    protected OutPort<TimedString> m_gridMapsPathOut;

    
    // </rtc-template>

    // CORBA Port declaration
    // <rtc-template block="corbaport_declare">
    
    // </rtc-template>

    // Service declaration
    // <rtc-template block="service_declare">
    
    // </rtc-template>

    // Consumer declaration
    // <rtc-template block="consumer_declare">
    
    // </rtc-template>

    public Time makeRTCTime(){
		long nanoTm = System.nanoTime()%1000000;
		long millisTm = System.currentTimeMillis();
		int sec = (int)(millisTm/1000);
		int nsec = (int)((millisTm%1000)*1000000+nanoTm);
		Time tm = new Time(sec,nsec);
    	
    	return tm;
    }


}
