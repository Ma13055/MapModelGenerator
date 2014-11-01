// -*- C++ -*-
/*!
 * @file  CVInpaint.h
 * @brief 与えられた画像に与えられた領域を用いて、修復処理を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef CVINPAINT_H
#define CVINPAINT_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include <opencv2\core\core.hpp>


// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;
using namespace cv;

/*!
 * @class CVInpaint
 * @brief 与えられた画像に与えられた領域を用いて、修復処理を行う
 *
 * 入力された画像に対して、コンフィギュレーションパラメータで与えら
 * れた値を用いて与えられた領域に対して修復処理を行うコンポーネント
 *
 * 入力
 * inpaintSrcImage[CameraImage] - 修復処理の対象画像
 * maskImage[CameraImage] - 修復マスク画像
 * maskRectangle[TimedShortSeq] - 修復領域を示す長方形のデータ
 * 出力
 * inpaintImage[CameraImage] -
 * 入力画像から対象領域の修復を行った画像
 *
 * 入力画像に対して指定された領域に対する修復処理を行い、結果をOut
 * Portに送る
 * 入力やコンフィギュの値を常に確認し、どこか一つでも変更があると輪
 * 郭の再検出を行う
 * 出力ポートに送信されるタイミングは、検出後一度きり
 *
 */
class CVInpaint
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  CVInpaint(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~CVInpaint();

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry() 
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onFinalize();

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
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

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
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

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
   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   * 表示ウィンドウを閉じる
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
   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   * 選択領域に修復を行う
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
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

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
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

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
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   * 表示ウィンドウを閉じる
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
   virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
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
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

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
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">
  /*!
   * 画像表示を選択するための変数
   * - Name: string img_view
   * - DefaultValue: OFF
   */
  std::string m_img_view;
  /*!
   * 修復したい領域をユーザが設定するためのパラーメータ
   * 左上のx座標，左上のy座標，横幅，縦幅を持つlongの配列
   * - Name: vector<long> inp_space
   * - DefaultValue: 0,0,0,0
   */
  vector<long> m_inp_space;
  /*!
   * 修復される各点を中心とする近傍円形領域の半径
   * - Name: double inp_radius
   * - DefaultValue: 0.0
   * - Range: x[0] >= 0, x[1] >= 0
   */
  double m_inp_radius;
  /*!
   * inpaintの修復手法
   * - Name: string inp_flag
   * - DefaultValue: INPAINT_NS
   * - Constraint: INPAINT_NS  -
   *               ナビエ・ストークス(Navier-Stokes) ベース手法
   *               INPAINT_TELEA  - Alexandru Telea による手法
   *               Telea04
   */
  std::string m_inp_flag;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::CameraImage m_src_img;
  /*!
   * 画像データを受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: この画像に画像修復処理が行われる
   *              修復の基になる領域は、maskImage,maskRectangleまた
   *              は
   *              コンフィギュレーションパラメータより選択する
   */
  InPort<RTC::CameraImage> m_src_imgIn;
  RTC::CameraImage m_mask_img;
  /*!
   * 修復マスク画像を受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 修復処理のマスクに用いる画像
   */
  InPort<RTC::CameraImage> m_mask_imgIn;
  RTC::TimedShortSeq m_mask_rect;
  /*!
   * 修復領域を示す長方形のデータ
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 左上のx座標，左上のy座標，横幅，縦幅を持つlongの
   *              配列
   */
  InPort<RTC::TimedShortSeq> m_mask_rectIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::CameraImage m_inp_img;
  /*!
   * 修復処理後画像を送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 修復処理済みの画像
   */
  OutPort<RTC::CameraImage> m_inp_imgOut;
  
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

 private:
  // <rtc-template block="private_attribute">
	 /*--------ポートの前回値を持つ変数-----------*/
	 CameraImage src_cam_img;
	 CameraImage mask_cam_img;
	 TimedShortSeq old_rect;

	/*--------その他変数-----------*/
	 Mat src_mat_img;
	 Mat inpaint_mat_img;
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void CVInpaintInit(RTC::Manager* manager);
};

#endif // CVINPAINT_H
