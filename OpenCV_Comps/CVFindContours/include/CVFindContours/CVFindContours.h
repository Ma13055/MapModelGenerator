// -*- C++ -*-
/*!
 * @file  CVFindContours.h
 * @brief 与えられた二値画像に対して輪郭検出を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef CVFINDCONTOURS_H
#define CVFINDCONTOURS_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "VectorConvert.h"
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
using namespace std;
using namespace cv;

/*!
 * @class CVFindContours
 * @brief 与えられた二値画像に対して輪郭検出を行う
 *
 * 入力された二値画像に対して、コンフィギュレーションパラメータで与
 * えられた検出法や閾値を用いて輪郭検出を行うコンポーネント
 *
 * 入力
 * srcImage[CameraImage] -
 * 輪郭検出の結果を描画しその変換閾値などを確認するために用いる二値
 * 化前のカラー画像データ
 * thresholdImage[CameraImage] - 輪郭検出の対象二値画像
 * 出力
 * contoursData[TimedShortSeq] -
 * 輪郭検出の結果となるContoursを配列化したデータ
 * contoursRectangles [TimedShortSeq] -
 * 長方形化した輪郭データの配列
 * contoursConvex [TimedShortSeq] - 凸図形化した輪郭データの配列
 *
 * 入力二値画像に対して輪郭検出を行い、結果を描画しつつTimedShortS
 * eqに配列化して送る
 * 入力やコンフィギュの値を常に確認し、どこか一つでも変更があると輪
 * 郭の再検出を行う
 * 出力ポートに送信されるタイミングは、検出後一度きり
 *
 */
class CVFindContours
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  CVFindContours(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~CVFindContours();

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
   * 輪郭検出を行う
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
   * 出力するデータタイプを選択する変数
   * - Name: string send_type
   * - DefaultValue: Rectangle,Convex
   * - Constraint: NonExchange - 無変換の輪郭点データを出力する
   *               Rectangle -
   *               輪郭点データを基に長方形データを作成し出力する
   *               Convex -
   *               輪郭点データを基に凸図形データを作成し出力する
   *               Convex -
   */
  std::string m_send_type;
  /*!
   * 輪郭検出のモードを選択するための変数
   * - Name: string fc_mode
   * - DefaultValue: EXTERNAL
   * - Constraint: CV_RETR_EXTERNAL -
   *               最も外側の輪郭のみを抽出する
   *               CV_RETR_LIST -
   *               すべての輪郭を抽出し，それらをリストに保存する
   *               CV_RETR_CCOMP -
   *               すべての輪郭を抽出し，をれらを2階層構造として保
   *               存する：上のレベルには，連結成分の外側の境界線が
   *               ，下のレベルには，連結成分の内側に存在する穴の境
   *               界線が属する
   *               CV_RETR_TREE -
   *               すべての輪郭を抽出し，入れ子構造になった輪郭を完
   *               全に表現する階層構造を構成する
   */
  std::string m_fc_mode;
  /*!
   * 輪郭検出のメソッドを選択するための変数
   * - Name: string fc_method
   * - DefaultValue: NONE
   * - Constraint: CV_CHAIN_APPROX_NONE -
   *               チェーンコードの全ての点を，通常の点群に変換する
   *               CV_CHAIN_APPROX_SIMPLE -
   *               水平・垂直・斜めの線分を圧縮し，それらの端点のみ
   *               を残す
   *               CV_CHAIN_APPROX_TC89_L1,CV_CHAIN_APPROX_TC89_KC
   *               OS -
   *               Teh-Chinチェーン近似アルゴリズムの1つを適用する
   *               CV_LINK_RUNS -
   *               値が1のセグメントを水平方向に接続する，全く異な
   *               る輪郭抽出アルゴリズム.
   *               ただし、CV_LINK_RUNSはCVRETR_LISTが選択されてい
   *               なければ使うことができないため、モードがLISTでな
   *               い場合は、CODEを用いるように変更される
   */
  std::string m_fc_method;
  /*!
   * オプションのオフセット
   * 各輪郭点はこの値の分だけシフトします
   * - Name: vector<int> offset
   * - DefaultValue: 0,0
   */
  vector<int> m_offset;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::CameraImage m_src_img;
  /*!
   * 二値化前のカラー画像を受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: この画像に対し輪郭検出の結果を書き込み、閾値など
   *              の検討を行う
   */
  InPort<RTC::CameraImage> m_src_imgIn;
  RTC::CameraImage m_thre_img;
  /*!
   * 輪郭検出を行う二値画像を受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 輪郭検出を行いたいデータ
   *              ここの接続がない場合は、srcImageの画像を固定値で
   *              二値化し輪郭検出を行う
   */
  InPort<RTC::CameraImage> m_thre_imgIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::TimedShortSeq m_cont_data;
  /*!
   * 検出によって得た輪郭点群
   * - Type: TimedShortSeq
   * - Number: 検出輪郭群数*(1+輪郭点数*2)
   * - Semantics: 検出結果のcontoursの要素を配列化して送信する
   *              Point p1,p2,p3,p4からなるVector<Point>
   *              convex1に対し
   *              - convex1.size() [0]
   *              - convex1.p1.x [1]
   *              - convex1.p1.y [2]
   *              - convex1.p2.x [3]
   *              - convex1.p2.y [4]
   *              - convex1.p3.x [5]
   *              - convex1.p3.y [6]
   *              - convex1.p4.x [7]
   *              - convex1.p4.y [8]
   *              これら9つの要素を配列に格納して送る
   *              すなわち[0]~[8]で一つの図形となる
   *              受け取り側は[0]にあたるデータ数分のPointが来ると
   *              認識し、
   *              そのPoint数分格納したら次のvector処理へ進む
   */
  OutPort<RTC::TimedShortSeq> m_cont_dataOut;
  RTC::TimedShortSeq m_cont_rect;
  /*!
   * 検出によって得られた輪郭点群からなる長方形データを送るためのポ
   * ート
   * - Type: TimedShortSeq
   * - Number: 長方形数*4
   * - Semantics: Rectangleの要素を配列化して送信する
   *              Rectangle r1の左上の点p1.x,p1.yとwidth,heightを
   *              - r1.p1.x [0]
   *              - r1.p1.y [1]
   *              - r1.width [2]
   *              - r1.height [3]
   *              これら4つの要素を配列に格納して送る
   *              すなわち[0]~[3]で一つの長方形となる
   */
  OutPort<RTC::TimedShortSeq> m_cont_rectOut;
  RTC::TimedShortSeq m_cont_convex;
  /*!
   * 輪郭検出によって得られた輪郭点群からなる凸図形のデータを送るた
   * めのポート
   * - Type: TimedShortSeq
   * - Number: 全図形数*(1+図形の持つ凸数*2)
   * - Semantics: convexとして用いるvect<Point>のデータを配列化して
   *              送信する
   *              Pointp1,p2,p3,p4からなるVector<Point>
   *              convex1に対し
   *              - convex1.size() [0]
   *              - convex1.p1.x [1]
   *              - convex1.p1.y [2]
   *              - convex1.p2.x [3]
   *              - convex1.p2.y [4]
   *              - convex1.p3.x [5]
   *              - convex1.p3.y [6]
   *              - convex1.p4.x [7]
   *              - convex1.p4.y [8]
   *              これら9つの要素を配列に格納して送る
   *              すなわち[0]~[8]で一つの図形となる
   *              受け取り側は、[0]にあたるデータ数分のPointが来る
   *              と認識し、
   *              そのPoint数分格納したら次のvector処理へ進む
   */
  OutPort<RTC::TimedShortSeq> m_cont_convexOut;
  
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
	string mode;
	string method;
	string checkbox;

	//FindContoursの基礎パラメータ
	Point offset;

	/*--------その他変数-----------*/
	CameraImage old_img;
	CameraImage thre_img;
	Mat gray_img;
	Mat src_img;
	Mat draw_img;
	bool remake;	//再検出フラグ
	vector<vector<Point> > contours;	//輪郭点群
	vector<Rect> cont_rect;	//輪郭点を変換した長方形
	vector<vector<Point> > cont_convex;	//輪郭点を変換した凸角形
	bool checkbox_flag[3];	//チェックボックスのフラグ配列

  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void CVFindContoursInit(RTC::Manager* manager);
};

#endif // CVFINDCONTOURS_H
