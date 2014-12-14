// -*- C++ -*-
/*!
 * @file  CVCanny.h
 * @brief 与えられた画像にエッジ検出を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef CVCANNY_H
#define CVCANNY_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"


// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;
using namespace cv;
using namespace std;

/*!
 * @class CVCanny
 * @brief 与えられた画像にエッジ検出を行う
 *
 * 入力された画像に対して、コンフィギュレーションパラメータで与えら
 * れた値を用いてエッジ検出を行う
 *
 * 入力
 * srcImage[CameraImage] - エッジ検出の対象画像
 * 出力
 * cannyImage[CameraImage] - 入力画像から抽出したエッジ画像
 *
 * 入力やコンフィギュの値を常に確認し、どこか一つでも変更があるとエ
 * ッジ再検出を行う
 * 出力ポートに送信されるタイミングは、検出後一度きり
 *
 */
class CVCanny
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  CVCanny(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~CVCanny();

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
   * エッジ検出を行う
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
   * ヒステリシスが存在する処理の一番目の閾値
   * thre1 と thre2 の内，小さい方の値をエッジの接続に利用
   * 大きい方の値を明確なエッジの初期セグメントを検出するのに利用
   * - Name: double thre1
   * - DefaultValue: 50
   * - Range: x>0
   */
  double m_thre1;
  /*!
   * ヒステリシスが存在する処理の二番目の閾値
   * - Name: double thre2
   * - DefaultValue: 200
   * - Range: x>0
   */
  double m_thre2;
  /*!
   * オペレータのアパーチャサイズ
   * - Name: int size
   * - DefaultValue: 3
   * - Range: x>0
   */
  int m_size;
  /*!
   * 画像勾配の強度を求めるために，より精度の高いL_2ノルムを利用す
   * るか，L_1ノルムで十分（L2gridient=false）かを指定します．
   * - Name: string gradient
   * - DefaultValue: true
   */
  std::string m_gradient;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::CameraImage m_src_img;
  /*!
   * エッジ検出を行う対象の画像データ
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: この画像に対してエッジ検出を行う
   */
  InPort<RTC::CameraImage> m_src_imgIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::CameraImage m_canny_img;
  /*!
   * 入力画像からエッジを抽出した画像データ
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 入力画像に対してコンフィギュの値を用いてエッジ検
   *              出を行った結果画像
   */
  OutPort<RTC::CameraImage> m_canny_imgOut;
  
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
	//Cannyのパラメータ
	double thre1;
	double thre2;
	int size;
	string gradient;

	/*--------その他変数-----------*/
	CameraImage old_img;
	Mat gray_img;
	Mat src_img;
	Mat pre_img;
	bool remake;	//再検出フラグ
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void CVCannyInit(RTC::Manager* manager);
};

#endif // CVCANNY_H
