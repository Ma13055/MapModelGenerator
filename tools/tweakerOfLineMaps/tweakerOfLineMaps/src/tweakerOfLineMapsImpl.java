// -*- Java -*-
/*!
 * @file  tweakerOfLineMapsImpl.java
 * @brief fine adjustment to linemaps
 * @date  $Date$
 *
 * @author	���쏫(Tatekawa Masaru)
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
 * �^����ꂽ���C���}�b�v�Q�ɑ΂��āA
 * �u�K�ύX�v�u�I�t�Z�b�g�ύX�v�u�ʒu�y�яk�ڒ����v
 * �̏������s��UI�R���|�[�l���g
 *
 * ����
 * lineMapsPathIn[TimedString] - ���������s���������C���}�b�v�Q�̃p�X
 * 
 * �o��
 * lineMapsPathOut[TimedString] - ���������s��ꂽ���C���}�b�v�Q�̃p�X
 *
 * ���������F
 * 	�󂯎�������C���}�b�v�̃p�X��ǂݍ���
 *		�o���o���ɕ\���ł���悤�ALIST�{�^���ŕ\���}�b�v���Ǘ�
 *		�܂��A�㕔�o�[�ɂ��t�@�C�����J�����Ƃ��\
 * 	�㕔�ɊK�ύX�{�^����݂���
 * 		�{�^���������ƃ|�b�v���
 * 		Q�\�����Ă���n�}���ǂ̊K�ɑ}�����܂����H
 * 		LIST�{�^��
 * 		�\�����e�F 1�K�̑O 1-2�K�̊� �` 6-7�K�̊ԁ@7�K�̏�
 * 			�i���݂̏ꏊ���w���I�������܂ށj
 * 	�㕔�ɃI�t�Z�b�g�{�^����݂���
 * 	�㕔�ɃI�t�Z�b�g(x,y)���̓{�b�N�X��݂���
 * 		�{�^���������ƍ��W�Q�̒��ōł�����̓_��
 * 		���_�ƂȂ�悤�ɍ��W�Q���X���C�h�ړ�������
 * 		�܂��A�I�t�Z�b�g���̓{�b�N�X�ɒl�̓��͂�
 * 		����ꍇ�͂��̒l��p���ăX���C�h�ړ�������
 *		�i��ʂɕ\��������}�́A�I�t�Z�b�g�ȂǊ֌W�Ȃ��A
 * 		����̓_�����(10,10)�̈ʒu�ɂ���悤��
 *		�X���C�h�ړ������ĕ\��������B�܂��A���_���㏑�����ĕ\������j
 * 	�t���A�ʒu�����{�^����݂���
 * 		���ݕ\�����Ă���K�łS�_��I�����Ă��炢�A
 * 		���ɉ��̊K�œ����㉺�֌W�����_���S�_�I�����Ă��炤
 * 		���̑I���̑��ւɔ����A�k�ڋy�уI�t�Z�b�g�ύX���s��
 * 	�㕔�ɕύX�n�}�o�̓{�^����݂���
 * 		���̃{�^�����������ƁAllinepams�t�H���_�Ɍ��ʂ�ۑ����A
 * 		���̃p�X����outPort����o�͂���
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
     * ���������s��
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
     * �t���[���̍폜���s��
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
     * ����������
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
    	
    	//���̓|�[�g�Ƀt�@�C���p�X���󂯎�����ꍇ
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
     * �t���[���̍폜���s��
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
     * ���������s���������C���}�b�v�Q�̃p�X���󂯎��
     * - Type: TimedString
     * - Number: 1
     * - Semantics: �󂯎�����p�X��ǂݍ��݁AUI��Ŕ��������s��
     */
    protected InPort<TimedString> m_lineMapsPathInIn;

    
    // </rtc-template>

    // DataOutPort declaration
    // <rtc-template block="outport_declare">
    protected TimedString m_send_maps_path_val;
    protected DataRef<TimedString> m_send_maps_path;
    /*!
     * ���������s�������C���}�b�v�̃p�X�𑗂�
     * - Type: TimedString
     * - Number: 1
     * - Semantics: UI��Ŕ��������ꂽ���ʂ̃��C���}�b�v�Q�̃p�X��������
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
