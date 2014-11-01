// -*- C++ -*-
/*!
 * @file  CVHoughThransform.h
 * @brief 与えられた二値画像に対してハフ変換を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef CVHOUGHTHRANSFORM_H
#define CVHOUGHTHRANSFORM_H

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
 * @class CVHoughThransform
 * @brief 与えられた二値画像に対してハフ変換を行う
 *
 * 入力された二値画像に対して、コンフィギュレーションパラメータで与
 * えられた値を用いてハフ変換を行う
 *
 * 入力
 * srcImage[CameraImage] - 結果出力を行いたい画像データ
 * threshold[CameraImage] - ハフ変換の対象二値画像
 * 出力
 * houghLine[TimedShortSeq] -
 * ハフ変換の結果となるLinesを配列化したもの
 *
 * 二値画像に対してハフ変換を行い、結果を入力された結果出力用画像に
 * 書き込み表示を行う
 * 入力やコンフィギュの値を常に確認し、どこか一つでも変更があるとエ
 * ッジ再検出を行う
 * 出力ポートに送信されるタイミングは、検出後一度きり
 *
 */
class CVHoughThransform
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  CVHoughThransform(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~CVHoughThransform();

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
   * ハフ変換を行う
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
   * 画像表示の選択
   * - Name: string img_view
   * - DefaultValue: OFF
   */
  std::string m_img_view;
  /*!
   * ハフ変換のmethodを選択するためのradioボタン
   * - Name: string hough_method
   * - DefaultValue: STANDARD
   * - Constraint: STANDARD - 標準的ハフ変換
   *               MULTI_SCALE - マルチスケール型の古典的ハフ変換
   *               PROBABILISTIC - 確率的ハフ変換
   */
  std::string m_hough_method;
  /*!
   * ピクセル単位で表される投票空間の距離分解能
   * - Name: double rho
   * - DefaultValue: 1
   * - Range: x > 0
   */
  double m_rho;
  /*!
   * ラジアン単位で表される投票空間の角度分解能
   * - Name: double theta
   * - DefaultValue: 0.0174533
   * - Range: x > 0
   */
  double m_theta;
  /*!
   * 投票の閾値パラメータ
   * 十分な票( >threshold )を得た直線のみ出力されます
   * - Name: int threshold
   * - DefaultValue: 100
   * - Range: x > 0
   */
  int m_threshold;
  /*!
   * マルチスケールハフ変換において、距離分解能rhoの除数となる値
   * 投票空間の粗い距離分解能はrhoとなり、細かい分解能はrho/srnとな
   * ります
   * もしsrn = 0 かつ stn =
   * 0の場合は、古典的ハフ変換が利用されます
   * そうでない場合は、両方のパラメータが正値である必要があります
   * - Name: double srn
   * - DefaultValue: 0
   * - Range: x >= 0
   */
  double m_srn;
  /*!
   * マルチスケールハフ変換において、角度分解能thetaの除数となる値
   * - Name: double stn
   * - DefaultValue: 0
   * - Range: x >= 0
   */
  double m_stn;
  /*!
   * 最小の線分長
   * これより短い線分は破棄されます
   * - Name: double min_length
   * - DefaultValue: 30
   * - Range: x > 0
   */
  double m_min_length;
  /*!
   * 2点が同一線分上にあると見なす場合に許容される最大距離
   * - Name: double max_gap
   * - DefaultValue: 10
   * - Range: x>0
   */
  double m_max_gap;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::CameraImage m_src_img;
  /*!
   * 二値画像の元画像を受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 結果出力を行いたい画像データ
   */
  InPort<RTC::CameraImage> m_src_imgIn;
  RTC::CameraImage m_thre_img;
  /*!
   * ハフ変換を行う二値画像を受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: ハフ変換を行いたい二値画像データ
   */
  InPort<RTC::CameraImage> m_thre_imgIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::TimedShortSeq m_hough_lines;
  /*!
   * ハフ変換によって得たlinesのデータを送るためのポート
   * - Type: TimedShortSeq
   * - Number: 検出line数*4
   * - Semantics: lineとして用いるPoint
   *              p1,p2をデータ配列化して送信する
   *              Point p1,p2からなるline l1に対し
   *              - l1.p1.x　[0]
   *              - l1.p1.y　[1]
   *              - l1.p2.x　[2]
   *              - l1.p2.y　[3]
   *              これら４つの要素を配列に格納して送る
   *              すなわち[0]~[3]で一本の線となる
   */
  OutPort<RTC::TimedShortSeq> m_hough_linesOut;
  
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

	 //メソッド
	 string method;

	 //HoughLinesの基礎パラメータ
	 double	rho;
	 double theta;
	 int	threshold;

	 //マルチスケールハフ変換のパラメータ
	 double srn;
	 double stn;

	 //確率的ハフ変換のパラメータ
	 double min_length;
	 double max_gap;

	 /*--------その他変数-----------*/
	 CameraImage old_img;
	 CameraImage thre_img;
	 Mat gray_img;
	 Mat src_img;
	 Mat pre_img;
	 bool remake;	//再検出フラグ
	 vector<Vec4i> lines_p;	//線情報が入る変数
	 vector<Vec2f> lines_s;	//線情報が入る変数
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void CVHoughThransformInit(RTC::Manager* manager);
};

#endif // CVHOUGHTHRANSFORM_H
