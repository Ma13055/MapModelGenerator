// -*- Java -*-
/*!
 * @file  ThreeDimensionViewerImpl.java
 * @brief 作成されたラインマップを3D化するコンポーネント
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * 		   Email:y09148@shibaura-it.ac.jp *
 * $Id$
 */

import java.io.File;
import java.util.Arrays;

import sun.swing.MenuItemLayoutHelper.RectSize;
import RTC.TimedString;
import jp.go.aist.rtm.RTC.DataFlowComponentBase;
import jp.go.aist.rtm.RTC.Manager;
import jp.go.aist.rtm.RTC.port.InPort;
import jp.go.aist.rtm.RTC.util.DataRef;
import RTC.ReturnCode_t;

/*!
 * @class ThreeDimensionViewerImpl
 * @brief 作成されたラインマップを3D化する
 *
 * 与えられたラインマップを元に3Dマップを作成するコンポーネント
 *
 * 入力
 * lineMapsPath[TimedString] - 全地図のラインマップの保存パス
 *
 * フレームに対するドラッグ操作により内部の3D図形を回転させることができる
 *
 */
public class ThreeDimensionViewerImpl extends DataFlowComponentBase {

	  private ThreeDimensionViewerFrame frame;
	  private RectSize frame_size;	  
	  private TimedString model_path;

  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
	public ThreeDimensionViewerImpl(Manager manager) {  
        super(manager);
        // <rtc-template block="initializer">
        m_line_maps_path_val = new TimedString();
        m_line_maps_path = new DataRef<TimedString>(m_line_maps_path_val);
        m_lineMapsPathIn = new InPort<TimedString>("lineMapsPath", m_line_maps_path);
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
        // </rtc-template>
        
        System.out.println("onInitialize");
        
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
        System.out.println("onActivatedStart");
 //   	while(m_lineMapsPathIn.isNew())m_lineMapsPathIn.read();
    	
		frame = new ThreeDimensionViewerFrame();
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

    	
        System.out.println("onActivatedEnd");    	
        return super.onActivated(ec_id);
    }

    /***
     * UIフレームを削除する
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
        System.out.println("onDeactivatedStart");
    	
    	frame.setVisible(false);
    	frame = null;

        System.out.println("onDeactivatedEnd");
        return super.onDeactivated(ec_id);
    }

    /***
     * UIフレームに3Dイメージを作成し描画
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
//        System.out.println("onExecuteStart");
    	
    	//入力ポートにファイルパスを受け取った場合
    	if(m_lineMapsPathIn.isNew()){
    		m_lineMapsPathIn.read();
    		if(!model_path.data.equals(m_line_maps_path.v.data)){
    			model_path = m_line_maps_path.v;
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

//        System.out.println("onExecuteEnd");
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
     * UIフレームを削除する
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

    	frame.setVisible(false);
    	frame = null;
    	    	
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
    protected TimedString m_line_maps_path_val;
    protected DataRef<TimedString> m_line_maps_path;
    /*!
     * 全地図のラインマップの保存パスを一括で受け取るためのポート
     * - Type: TimedString
     * - Number: 1
     * - Semantics: 保存したラインマップデータのファイルのパスを受け取る
     */
    protected InPort<TimedString> m_lineMapsPathIn;

    
    // </rtc-template>

    // DataOutPort declaration
    // <rtc-template block="outport_declare">
    
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


}
