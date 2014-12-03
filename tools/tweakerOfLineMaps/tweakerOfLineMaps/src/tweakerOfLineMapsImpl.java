// -*- Java -*-
/*!
 * @file  tweakerOfLineMapsImpl.java
 * @brief fine adjustment to linemaps
 * @date  $Date$
 *
 * @author	立川将(Tatekawa Masaru)
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
 * @class tweakerOfLineMapsImpl
 * @brief fine adjustment to linemaps
 *
 * 与えられたラインマップ群に対して、
 * 「階変更」「オフセット変更」「位置及び縮尺調整」
 * の処理を行うUIコンポーネント
 *
 * 入力
 * lineMapsPathIn[TimedString] - 微調整を行いたいラインマップ群のパス
 * 
 * 出力
 * lineMapsPathOut[TimedString] - 微調整が行われたラインマップ群のパス
 *
 * 内部処理：
 * 	受け取ったラインマップのパスを読み込む
 *		バラバラに表示できるよう、LISTボタンで表示マップを管理
 *		また、上部バーによりファイルを開くことも可能
 * 	上部に階変更ボタンを設ける
 * 		ボタンを押すとポップ画面
 * 		Q表示している地図をどの階に挿入しますか？
 * 		LISTボタン
 * 		表示内容： 1階の前 1-2階の間 ～ 6-7階の間　7階の上
 * 			（現在の場所を指す選択肢も含む）
 * 	上部にオフセットボタンを設ける
 * 	上部にオフセット(x,y)入力ボックスを設ける
 * 		ボタンを押すと座標群の中で最も左上の点が
 * 		原点となるように座標群をスライド移動させる
 * 		また、オフセット入力ボックスに値の入力が
 * 		ある場合はその値を用いてスライド移動させる
 *		（画面に表示させる図は、オフセットなど関係なく、
 * 		左上の点が常に(10,10)の位置にくるように
 *		スライド移動させて表示させる。また、原点も上書きして表示する）
 * 	フロア位置調整ボタンを設ける
 * 		現在表示している階で４点を選択してもらい、
 * 		次に下の階で同じ上下関係を持つ点を４点選択してもらう
 * 		この選択の相関に伴い、縮尺及びオフセット変更を行う
 * 	上部に変更地図出力ボタンを設ける
 * 		このボタンが押されると、llinepamsフォルダに結果を保存し、
 * 		そのパス情報をoutPortから出力する
 *
 */
public class tweakerOfLineMapsImpl extends DataFlowComponentBase {

	  private tweakerOfLineMapsFrame frame;
	  private RectSize frame_size;	  
	  private TimedString model_path;
	  private Time tm;
	
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
	public tweakerOfLineMapsImpl(Manager manager) {  
        super(manager);
        // <rtc-template block="initializer">
        m_get_maps_path_val = new TimedString();
        m_get_maps_path = new DataRef<TimedString>(m_get_maps_path_val);
        m_lineMapsPathInIn = new InPort<TimedString>("lineMapsPathIn", m_get_maps_path);
        m_send_maps_path_val = new TimedString();
        m_send_maps_path = new DataRef<TimedString>(m_send_maps_path_val);
        m_lineMapsPathOutOut = new OutPort<TimedString>("lineMapsPathOut", m_send_maps_path);
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
        addInPort("lineMapsPathIn", m_lineMapsPathInIn);
        
        // Set OutPort buffer
        addOutPort("lineMapsPathOut", m_lineMapsPathOutOut);
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
        System.out.println("tweakerOfLineMaps : onActivated : START");
 //   	while(m_lineMapsPathIn.isNew())m_lineMapsPathIn.read();
    	
		frame = new tweakerOfLineMapsFrame();
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
    	
        System.out.println("tweakerOfLineMaps : onActivated : END");
        return super.onActivated(ec_id);
    }

    /***
     * フレームの削除を行う
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
        System.out.println("tweakerOfLineMaps : onDeactivated : START");
    	
    	frame.setVisible(false);
    	frame.removeAll();
    	frame = null;

        System.out.println("tweakerOfLineMaps : onDeactivated : END");
        return super.onDeactivated(ec_id);
    }

    /***
     * 微調整処理
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
    	if(m_lineMapsPathInIn.isNew()){
    		m_lineMapsPathInIn.read();
    		if(!model_path.data.equals(m_get_maps_path.v.data)){
    			model_path = m_get_maps_path.v;
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
        	m_lineMapsPathOutOut.write(ts);

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
     * フレームの削除を行う
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
    	System.out.println("tweakerOfLineMaps : onReset : START");

    	frame.setVisible(false);
    	frame.removeAll();
    	frame = null;

    	System.out.println("tweakerOfLineMaps : onReset : END");    	
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
    protected TimedString m_get_maps_path_val;
    protected DataRef<TimedString> m_get_maps_path;
    /*!
     * 微調整を行いたいラインマップ群のパスを受け取る
     * - Type: TimedString
     * - Number: 1
     * - Semantics: 受け取ったパスを読み込み、UI上で微調整を行う
     */
    protected InPort<TimedString> m_lineMapsPathInIn;

    
    // </rtc-template>

    // DataOutPort declaration
    // <rtc-template block="outport_declare">
    protected TimedString m_send_maps_path_val;
    protected DataRef<TimedString> m_send_maps_path;
    /*!
     * 微調整を行ったラインマップのパスを送る
     * - Type: TimedString
     * - Number: 1
     * - Semantics: UI上で微調整された結果のラインマップ群のパスが送られる
     */
    protected OutPort<TimedString> m_lineMapsPathOutOut;

    
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
