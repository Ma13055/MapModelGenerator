// -*- C++ -*-
/*!
 * @file  OpenPicts.h
 * @brief 与えられたファイル名から複数画像の読み込みと送信を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef OPENPICTS_H
#define OPENPICTS_H

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

/*!
 * @class OpenPicts
 * @brief 与えられたファイル名から複数画像の読み込みと送信を行う
 *
 * コンフィギュレーションパラメータのFileNameで指定された、又は入力
 * ポートより受け取った画像ファイルを読み込み、CameraImage型に変換
 * しOutPortから送るコンポーネント
 * 送り先のコンポーネントと相互通信を模したポート間のやり取りを用い
 * ることで複数枚の画像をCameraImage型で送ることができる
 *
 * 入力
 * FileName[TimeString] - 読み込む画像データのファイル名
 * backCameraImage[CameraImage] - 送り先からのフィードバック
 * 出力
 * sendCameraImage[CameraImage] - 読み込んだ画像
 * sendFlagData[TimedShort] -
 * 送信終了・送信のやり直しなどの送信状態
 *
 * ポートとコンフィグの変更を常に監視し、変更があった場合、画像の再
 * 読み込みを行う。
 * 送信はフィードバッグがなければ最初の読み込み画像を連続で送信、フ
 * ィードバッグがある場合は、送信が完了したらoutを終了して、入力変
 * 更の監視のみを行う。
 * それぞれの入出力のデータ送信ポリシーは、フィードバック処理を模し
 * た構造であるため、Newを選択することを推奨。
 *
 */
class OpenPicts
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  OpenPicts(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~OpenPicts();

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
   * 画像の読み込みを行う
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
   * 読み込みたいファイルのアドレス
   * ","で区切ることで複数取得可能
   * - Name: string FileName
   * - DefaultValue: *.jpg
   */
  std::string m_FileName;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::TimedString m_file_name;
  /*!
   * 読み込み画像データのファイル名
   * - Type: TimedString
   * - Number: 1
   * - Semantics: 読み込みたい画像のファイル名
   *              ","で区切ることで複数取得可能
   */
  InPort<RTC::TimedString> m_file_nameIn;
  RTC::CameraImage m_rec_cam_img;
  /*!
   * 送り先が現在受け取っている画像が何かを知るためのポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 本コンポーネントから送信されているCameraImageとこ
   *              のポートで受け取ったCameraImageが合致した場合、次
   *              の画像を送る処理への進む
   */
  InPort<RTC::CameraImage> m_rec_cam_imgIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::CameraImage m_send_cam_img;
  /*!
   * 画像データを送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: ファイル名から読み込んだ画像データ
   *              データ送信ポリシー：New推奨
   */
  OutPort<RTC::CameraImage> m_send_cam_imgOut;
  RTC::TimedShort m_send_flag;
  /*!
   * 送り先へ現在の送信状態を教えるためのポート
   * - Type: TimedShort
   * - Number: 1
   * - Semantics: フラグ処理の説明
   *              -2:こちらのファイル基データが書き換わったことを示
   *              すフラグ(要初期化
   *              -1:すべての画像を送信したことを示すフラグ
   */
  OutPort<RTC::TimedShort> m_send_flagOut;
  
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
	std::string FN;			//読み込むファイルの場所を示すstring
	std::string FN_conf;	//コンフィギュに入力された、読み込むファイルの場所を示すstring
	std::string FN_port;	//入力ポートから与えられた、読み込むファイルの場所を示すstring
	std::vector<CameraImage> out_datas;		//読み込んだ画像情報を保持しておくためのvector
	std::vector<CameraImage>::iterator pict_it;
	bool onExStartFlag; //メッセージ用フラグ
	bool change_name_flag;
	bool send_flag;
	bool send_loop;  

  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void OpenPictsInit(RTC::Manager* manager);
};

#endif // OPENPICTS_H
