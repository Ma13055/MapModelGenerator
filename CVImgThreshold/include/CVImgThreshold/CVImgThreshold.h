// -*- C++ -*-
/*!
 * @file  CVImgThreshold.h
 * @brief 与えられた画像に二値化処理を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef CVIMGTHRESHOLD_H
#define CVIMGTHRESHOLD_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "opencv2\core\core.hpp"


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
 * @class CVImgThreshold
 * @brief 与えられた画像に二値化処理を行う
 *
 * 与えられたCameraImageの画像に対し二値化処理を行うコンポーネント
 *
 * 入力
 * srcImage[CameraImage] -処理対象となる画像データを受け取る
 * 出力
 * thresholdImg[CameraImage] - 処理後のデータを送る
 *
 * 受け取った画像に対し、コンフィギュレーションパラメータで設定され
 * た処理を行い処理後の画像をCemeraImage型に変換しthresholdから出力
 *
 */
class CVImgThreshold
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  CVImgThreshold(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~CVImgThreshold();

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
   * 二値化処理を行う
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
   * 二値化処理アルゴリズムを指定するための変数
   * (同じ画像に対して複数回使用する場合に対応)
   * - Name: vector<string> thre_algo
   * - DefaultValue: NON
   */
  vector<string> m_thre_algo;
  /*!
   * cvThreshold()及びcvAdaptiveThreshold()で用いる引数
   * 入力データが足りない、または制約条件から外れている場合はデフォ
   * ルト値を用いて処理を行います
   * - Name: vector<double> thre_para
   * - DefaultValue: 0,0,255,0,0,255,0,0,255
   * - Constraint: threAlgoがAdaptedThresholdではないとき、下記のt
   *               hrePara[]が入力値となる
   *               cvThreshold( ----, ----, threPara[0],
   *               threPara[1], threAlgo)
   *               threPara[0]…double threshold – 閾値．:制約 0
   *               <= x <= 255:デフォルト 0
   *               threPara[1]…double maxValue -
   *               threAlgoがBINARYとBINARY_INVの場合に利用される，
   *               最大値の値．:制約 0 <= x <= 255 :デフォルト
   *               255
   *               threAlgoがAdaptedThresholdであるとき、下記のthr
   *               ePara[]が入力値となる
   *               cvAdaptiveThreshold(----, ----, threPara[0],
   *               threAlgo,
   *               threPara[1], threPara[2], threPara[3])
   *               threPara[0]…double maxValue –
   *               CV_THRESH_BINARY とCV_THRESH_BINARY_INV
   *               の場合に利用される，最大値の値．:制約 0 <= x
   *               <= 255 :デフォルト 255
   *               threPara[1]…int threshold_type
   *               –閾値処理の種類．以下のうちの1つ 0か1で選択
   *               　CV_THRESH_BINARY - 0
   *               　CV_THRESH_BINARY_INV - 1　:制約 0 or 1 :
   *               デフォルト 0
   *               threPara[2]…int blockSize -
   *               ピクセルに対する閾値を計算するために利用する，そ
   *               のピクセルの近傍領域のサイズ：3，5，7，など．:制
   *               約 1<x かつ奇数:デフォルト 3
   *               threPara[3]…double param1 -
   *               利用する手法に依存するパラメータ．:デフォルト
   *               10
   *               threAlgoがAdaptedThresholdではないとき
   *               threPara[0]…0 <= x <= 255
   *               threPara[1]…0 <= x <= 255
   *               threPara[0]…0 <= x <= 255
   *               threPara[1]…制約 0 or 1
   *               threPara[2]…1<x かつ 奇数
   *               threPara[3]…特になし
   */
  vector<double> m_thre_para;
  /*!
   * GaussianBlurを行うかどうかを決めるための変数
   * - Name: string gaus_blur
   * - DefaultValue: ON
   */
  std::string m_gaus_blur;
  /*!
   * 画像の平均化に用いるガウシアンカーネルサイズ
   * - Name: vector<short> gaus_size
   * - DefaultValue: 3,3
   */
  vector<short> m_gaus_size;
  /*!
   * Bitwise_notを行うかどうかを決めるための変数
   * - Name: string bitwise
   * - DefaultValue: ON
   */
  std::string m_bitwise;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::CameraImage m_rec_img;
  /*!
   * 処理対象となる画像データを受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: このポートより受け取るCameraImageに対し、Mat型へ
   *              変換を行った後二値化処理を行う
   */
  InPort<RTC::CameraImage> m_rec_imgIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::CameraImage m_thre_img;
  /*!
   * 二値化後の画像データを送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 二値化後の画像データ
   */
  OutPort<RTC::CameraImage> m_thre_imgOut;
  
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
	/*--------コンフィギュの前回値を持つ変数-----------*/

	//Thresholdのパラメータ
	std::vector<std::string> thre_algo;
	std::vector<double> thre_para;
	string gaus_blur;
	std::vector<short> gaus_size;
	string bitwise;

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
  DLL_EXPORT void CVImgThresholdInit(RTC::Manager* manager);
};

#endif // CVIMGTHRESHOLD_H
