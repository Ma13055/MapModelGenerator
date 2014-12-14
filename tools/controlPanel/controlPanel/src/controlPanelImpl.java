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

import java.awt.Point;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.io.File;

import javax.imageio.ImageIO;
import sun.swing.MenuItemLayoutHelper.RectSize;

import RTC.CameraImage;
import RTC.Point3D;
import RTC.Time;
import RTC.TimedShort;
import RTC.TimedString;
import RTC.TimedPoint3D;
import RTC.TimedShortSeq;
import jp.go.aist.rtm.RTC.DataFlowComponentBase;
import jp.go.aist.rtm.RTC.Manager;
import jp.go.aist.rtm.RTC.port.InPort;
import jp.go.aist.rtm.RTC.port.OutPort;
import jp.go.aist.rtm.RTC.util.DataRef;
import RTC.ReturnCode_t;

/*!
 * @class controlPanelImpl
 * @brief 処理中のマウス操作情報を取得する
 *
 * 地図モデル生成中に必要となるユーザからの操作を取得するコンポーネント 
 * 所要機能は、読み込み画像の指定、一時作業領域の指定、クリック情報の取得、ドラッグ情報の取得、
 * ボタンによるイベント情報(決定操作やリセット操作など)の取得（情報形態はクリック情報と同一）
 * 
 * 入力
 * srcImage[CameraImage] - 処理前の画像
 * srcImagePath[TimedString] - 処理前の画像のパス
 * modifyImage[CameraImage] - ユーザの操作を必要とする処理中の画像
 * modifyImagePath[TimedString] - ユーザの操作を必要とする処理中の画像のパス
 * completeImage[CameraImage] - 処理結果の画像
 * completeImagePath[TimedString] - 処理結果の画像のパス
 * 
 * 出力
 * openFilePath[TimedString] - controlPanelで指定した読み込む画像のパス
 * tempFolderPath[TimedString] - controlPanelで指定した一時作業領域のパス
 * clickPoint[TimedPoint3D] - controlPanel上でのクリック座標データ
 * dragRectangle[TimedShortSeq] - controlPanel上でのドラッグ領域のデータ
 * 
 * 受け取った処理前、処理中、処理後の画像をウィンドウに表示する
 * それぞれの状態にあったイベントを提示し、その選択情報を送信する
 *
 */
public class controlPanelImpl extends DataFlowComponentBase {

	  private UIWindow frame;
	  private BufferedImage defo_img;
	  private BufferedImage modify_img;
	  private BufferedImage comp_img;
	  private Time tm;
	  private RectSize frame_size;

	
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
	public controlPanelImpl(Manager manager) {  
        super(manager);
        // <rtc-template block="initializer">
        m_src_img_val = new CameraImage();
        m_src_img = new DataRef<CameraImage>(m_src_img_val);
        m_srcImageIn = new InPort<CameraImage>("srcImage", m_src_img);
        m_src_img_path_val = new TimedString();
        m_src_img_path = new DataRef<TimedString>(m_src_img_path_val);
        m_srcImagePathIn = new InPort<TimedString>("srcImagePath", m_src_img_path);
        m_modi_img_val = new CameraImage();
        m_modi_img = new DataRef<CameraImage>(m_modi_img_val);
        m_modifyImageIn = new InPort<CameraImage>("modifyImage", m_modi_img);
        m_modi_img_path_val = new TimedString();
        m_modi_img_path = new DataRef<TimedString>(m_modi_img_path_val);
        m_modifyImagePathIn = new InPort<TimedString>("modifyImagePath", m_modi_img_path);
        m_comp_img_val = new CameraImage();
        m_comp_img = new DataRef<CameraImage>(m_comp_img_val);
        m_completeImageIn = new InPort<CameraImage>("completeImage", m_comp_img);
        m_comp_img_path_val = new TimedString();
        m_comp_img_path = new DataRef<TimedString>(m_comp_img_path_val);
        m_completeImagePathIn = new InPort<TimedString>("completeImagePath", m_comp_img_path);
        m_step_flag_val = new TimedShort();
        m_step_flag = new DataRef<TimedShort>(m_step_flag_val);
        m_stepFlagIn = new InPort<TimedShort>("stepFlag", m_step_flag);
        
        m_open_file_path_val = new TimedString();
        m_open_file_path = new DataRef<TimedString>(m_open_file_path_val);
        m_openFilePathOut = new OutPort<TimedString>("openFilePath", m_open_file_path);
        m_temp_space_path_val = new TimedString();
        m_temp_space_path = new DataRef<TimedString>(m_temp_space_path_val);
        m_tempFolderPathOut = new OutPort<TimedString>("tempFolderPath", m_temp_space_path);
        m_click_point_val = new TimedPoint3D();
        m_click_point = new DataRef<TimedPoint3D>(m_click_point_val);
        m_clickPointOut = new OutPort<TimedPoint3D>("clickPoint", m_click_point);
        m_drag_rect_val = new TimedShortSeq();
        m_drag_rect = new DataRef<TimedShortSeq>(m_drag_rect_val);
        m_dragRectangleOut = new OutPort<TimedShortSeq>("dragRectangle", m_drag_rect);
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
        addInPort("srcImage", m_srcImageIn);
        addInPort("srcImagePath", m_srcImagePathIn);
        addInPort("modifyImage", m_modifyImageIn);
        addInPort("modifyImagePath", m_modifyImagePathIn);
        addInPort("completeImage", m_completeImageIn);
        addInPort("completeImagePath", m_completeImagePathIn);
        addInPort("stepFlag", m_stepFlagIn);
        
        // Set OutPort buffer
        addOutPort("openFilePath", m_openFilePathOut);
        addOutPort("tempFolderPath", m_tempFolderPathOut);
        addOutPort("clickPoint", m_clickPointOut);
        addOutPort("dragRectangle", m_dragRectangleOut);
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
		frame = new UIWindow();
		frame.buildMenus();
		frame.buildContent();
		frame.pack();
		frame.setVisible(true);
		
		frame_size = new RectSize();
		frame_size.setHeight(frame.getHeight());
		frame_size.setWidth(frame.getWidth());

		
		defo_img = null;
		modify_img = null;
		
		File newdir = new File("c:\\tmp");
		newdir.mkdir();
		newdir = new File("C:\\tmp\\makeMapsModel");
		newdir.mkdir();
		newdir = new File("C:\\tmp\\makeMapsModel\\sub");
		newdir.mkdir();
		newdir = new File("C:\\tmp\\makeMapsModel\\maps");
		newdir.mkdir();
		newdir = new File("C:\\tmp\\makeMapsModel\\maps\\linemaps");
		newdir.mkdir();
		
        return super.onActivated(ec_id);
    }

    /***
     * 操作ウィンドウを削除する
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
    	
    	frame.setVisible(false);
    	frame.removeAll();
    	frame = null;
    	defo_img = null;
    	modify_img = null;
    	Runtime rt = Runtime.getRuntime();
    	rt.gc();
        return super.onDeactivated(ec_id);
    }

    /***
     * マウス操作を取得する
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
		tm = makeRTCTime();

    	if (m_srcImageIn.isNew()) {
    		m_srcImageIn.read();
    		defo_img = receiveCameraImage(defo_img, m_src_img);
    	}

    	if (m_modifyImageIn.isNew()) {
    		m_modifyImageIn.read();
    		modify_img = receiveCameraImage(modify_img, m_modi_img);
    	}
    	
    	if (m_completeImageIn.isNew()) {
    		m_completeImageIn.read();
    		comp_img = receiveCameraImage(comp_img, m_comp_img);
    	}

    	if(m_srcImagePathIn.isNew()){
    		m_srcImagePathIn.read();
    		defo_img = readPath(m_src_img_path.v);
    	}
    	
    	if(m_modifyImagePathIn.isNew()){
    		m_modifyImagePathIn.read();
    		modify_img = readPath(m_modi_img_path.v);
    		frame.modify.setSelected(true);
    	}
    	
    	if(m_completeImagePathIn.isNew()){
    		m_completeImagePathIn.read();
    		comp_img = readPath(m_comp_img_path.v);
    		frame.comp.setSelected(true);
    	}
    	
    	if(m_stepFlagIn.isNew()){
    		m_stepFlagIn.read();
    		
    		switch(m_step_flag.v.data){
    		case 1:
    			frame.popInfoMessage("画像の読み込みが完了しました");
    			break;
    		case 2:
    			frame.popInfoMessage("処理を確定し、次の画像の処理へ進みます");
    			break;
    		case 3:
    			frame.popInfoMessage("処理を確定し、同じ画像の処理を行います");
    			break;
    		case 4:
    			frame.popInfoMessage("処理結果を消去し、同じ画像の処理を行います");
    			break;
    		case 5:
    			frame.popInfoMessage("全画像への処理が完了しました\n次の工程へ進みます");
    			break;
    		case 6:
    			frame.popErrMessage("全ての画像の処理が終了していないため、\n次の工程へ進むことができませんでした");
    			break;
    		case 7:
    			frame.popInfoMessage("全画像へのラインマップ生成が完了しました\n次の工程へ進みます");
    			this.get_owned_contexts()[0].deactivate_component(this.getObjRef());
    			break;
    		case 8:
    			frame.popInfoMessage("現在の修正情報を一時保存しました");
    			break;
    		case 9:
    			frame.popInfoMessage("修正情報を初期化します");
    			break;
    		case 10:
    			frame.popInfoMessage("輪郭データの修正を終了します");
    			break;
    		case 11:
    			frame.popInfoMessage("輪郭データの出力が終了しました\n修正を開始してください");
    			break;
    		case 12:
    			frame.popInfoMessage("修復処理の修正を終了します");
    			break;
    		case 13:
    			frame.popInfoMessage("ラインマップの下地が生成されました\n修正を開始してください");    			
    			break;
    		case 14:
    			frame.popInfoMessage("ラインマップの修正を終了します");    			
    			break;
    		case 15:
    			frame.popInfoMessage("輪郭データを用いた修復処理が完了しました\n修復処理の修正を行ってください");
    			break;
    		}
    	}
    	
    	if(frame.defo.isSelected() == true && defo_img != null){
    		frame.img = defo_img;
    		frame.button_panel.buttonScene3();
    	}else if(frame.modify.isSelected() == true && modify_img != null){
    		frame.img = modify_img;
    		frame.button_panel.buttonScene1();
    	}else if(frame.comp.isSelected() == true && comp_img != null){
    		frame.img = comp_img;
    		frame.button_panel.buttonScene2();
    	}
    	
    	if(frame.img != null){
    		int width = frame.getWidth();
    		int height = frame.img.getHeight() + frame_size.getHeight();
    		
    		if(width < frame.img.getWidth()+8) width = frame.img.getWidth()+8;
    		else if(frame.img.getWidth()+8 < frame_size.getWidth()) width = frame_size.getWidth();
	    	
    		frame.setSize(width,height);
	    	frame.repaint();
    	}else{
    		frame.setSize(frame_size.getWidth(),frame_size.getHeight());
    	}

    	
    	switch(frame.getMouseFlag()){
	    	case 0:break;
	    	case 2:{
	    		
	    		Rectangle sub_dragged_rect = frame.getDraggRectangle();
	    		short[] sub_short_seq = {(short)sub_dragged_rect.x,(short)sub_dragged_rect.y,(short)sub_dragged_rect.width,(short)sub_dragged_rect.height};
	    		TimedShortSeq sub_rectangle = new TimedShortSeq(tm,sub_short_seq);
	    		m_dragRectangleOut.write(sub_rectangle);
	    	}	    		
	    	default:{
	    		
	    		Point sub_click_p = frame.getClickPoint();
	    		Point3D sub_p3D = new Point3D(sub_click_p.x,sub_click_p.y,frame.getMouseFlag());
	    		TimedPoint3D sub_point3D = new TimedPoint3D(tm,sub_p3D);
	    		m_clickPointOut.write(sub_point3D);
	    		
	    		frame.resetMouseFlag();
	    		break;
	    	}
    	}
    	
    	String file_name = frame.getFileName();
    	TimedString fileName = new TimedString(tm,file_name);
    	if(!file_name.equals(""))m_openFilePathOut.write(fileName);
    	
    	String folder_name = frame.getFolderName();
    	TimedString folderName = new TimedString(tm,folder_name);
    	m_tempFolderPathOut.write(folderName);
    	
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
     * 操作ウィンドウを削除する
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
    	frame.removeAll();
    	frame = null;
    	defo_img = null;
    	modify_img = null;
    	Runtime rt = Runtime.getRuntime();
    	rt.gc();
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
    protected CameraImage m_src_img_val;
    protected DataRef<CameraImage> m_src_img;
    /*!
     * 処理前の画像を受け取るポート
     * - Type: CameraImage
     * - Number: 1
     * - Semantics: 処理前の画像
     */
    protected InPort<CameraImage> m_srcImageIn;

    protected TimedString m_src_img_path_val;
    protected DataRef<TimedString> m_src_img_path;
    /*!
     * 処理前の画像のパスを受け取るポート
     * - Type: TimedString
     * - Number: 1
     * - Semantics: 処理前の画像のパス
     */
    protected InPort<TimedString> m_srcImagePathIn;

    protected CameraImage m_modi_img_val;
    protected DataRef<CameraImage> m_modi_img;
    /*!
     * ユーザの操作を必要とする処理中の画像を受け取るポート
     * - Type: CameraImage
     * - Number: 1
     * - Semantics: ユーザの操作を必要とする処理中の画像
     */
    protected InPort<CameraImage> m_modifyImageIn;

    protected TimedString m_modi_img_path_val;
    protected DataRef<TimedString> m_modi_img_path;
    /*!
     * ユーザの操作を必要とする処理中の画像のパスを受け取るポート
     * - Type: TimedString
     * - Number: 1
     * - Semantics: ユーザの操作を必要とする処理中の画像のパス
     */
    protected InPort<TimedString> m_modifyImagePathIn;

    protected CameraImage m_comp_img_val;
    protected DataRef<CameraImage> m_comp_img;
    /*!
     * 処理結果の画像を受け取るポート
     * - Type: CameraImage
     * - Number: 1
     * - Semantics: 処理結果の画像
     */
    protected InPort<CameraImage> m_completeImageIn;

    protected TimedString m_comp_img_path_val;
    protected DataRef<TimedString> m_comp_img_path;
    /*!
     * 処理結果の画像のパスを受け取るポート
     * - Type: TimedString
     * - Number: 1
     * - Semantics: 処理結果の画像のパス
     */
    protected InPort<TimedString> m_completeImagePathIn;


    protected TimedShort m_step_flag_val;
    protected DataRef<TimedShort> m_step_flag;
    /*!
     * 処理経過のダイアログを表示するためのフラグを受け取るポート
     * - Type: TimedShort
     * - Number: 1
     * - Semantics: 処理結果のフラグ
     */
    protected InPort<TimedShort> m_stepFlagIn;

    
    // </rtc-template>

    // DataOutPort declaration
    // <rtc-template block="outport_declare">
    protected TimedString m_open_file_path_val;
    protected DataRef<TimedString> m_open_file_path;
    /*!
     * controlPanelで指定した読み込む画像のパスを送るポート
     * - Type: TimedString
     * - Number: 1
     * - Semantics: controlPanelで指定した読み込む画像のパス
     */
    protected OutPort<TimedString> m_openFilePathOut;

    protected TimedString m_temp_space_path_val;
    protected DataRef<TimedString> m_temp_space_path;
    /*!
     * controlPanelで指定した一時作業領域のパスを送るポート
     * - Type: TimedString
     * - Number: 1
     * - Semantics: controlPanelで指定した一時作業領域のパス
     */
    protected OutPort<TimedString> m_tempFolderPathOut;

    protected TimedPoint3D m_click_point_val;
    protected DataRef<TimedPoint3D> m_click_point;
    /*!
     * controlPanel上でのクリック座標データを送るポート
     * - Type: TimedPoint3D
     * - Number: 1
     * - Semantics: controlPanel上でのクリック座標データ
     */
    protected OutPort<TimedPoint3D> m_clickPointOut;

    protected TimedShortSeq m_drag_rect_val;
    protected DataRef<TimedShortSeq> m_drag_rect;
    /*!
     * controlPanel上でのドラッグ領域のデータを送るポート
     * - Type: TimedShortSeq
     * - Number: 4
     * - Semantics: controlPanel上でのドラッグ領域のデータ
     */
    protected OutPort<TimedShortSeq> m_dragRectangleOut;

    
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

    
	private BufferedImage receiveCameraImage(BufferedImage img, DataRef<CameraImage> m_img){
		if (img == null
			||	m_img.v.width != img.getWidth() ||
				m_img.v.height != img.getHeight()
				) {
			img = new BufferedImage(m_img.v.width, m_img.v.height, BufferedImage.TYPE_INT_RGB);
		}
		for (int y = 0;y < m_img.v.height;y++) {
			for (int x = 0;x < m_img.v.width;x++) {
				int index = y * m_img.v.width + x;
				byte r = m_img.v.pixels[index*3 + 2];
				byte g = m_img.v.pixels[index*3 + 1];
				byte b = m_img.v.pixels[index*3 + 0];
				int rgb = (0x00FF & (int)r) << 16 | (0x00FF & (int)g) << 8 | (0x00FF & (int)b);
				img.setRGB(x, y, rgb);
			}
		}
		return img;
	}
		
	private BufferedImage readPath(TimedString path){
		long pathTime = ((long)path.tm.sec)*1000+((long)path.tm.nsec/1000000);
		while(true){
			try {
				BufferedImage readImage = null;

				File data = new File(path.data);
				if(pathTime < data.lastModified()){
					readImage = ImageIO.read(data);
				}else{
					continue;
				}
				return readImage;
			} catch (Exception e) {
				continue;
			}
		}
	}

}
