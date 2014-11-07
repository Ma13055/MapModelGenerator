// -*- C++ -*-
/*!
 * @file  convToLineMap.h
 * @brief 輪郭情報と特徴点を基にラインマップへ変換する
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef CONVTOLINEMAP_H
#define CONVTOLINEMAP_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "opencv2\core\core.hpp"
#include "opencv2\features2d\features2d.hpp"

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
 * @class convToLineMap
 * @brief 輪郭情報と特徴点を基にラインマップへ変換する
 *
 * 輪郭情報と特徴点を基にラインマップへ変換するコンポーネント
 *
 * 入力
 * tempFolderPath[TimedString] -
 * 画像情報送信に用いる一時保存領域のパス
 * srcImage[CameraImage] - 修復処理を行いたい画像データ
 * contoursData[TimedShortSeq] -
 * 輪郭検出の結果となるContoursを配列化したデータ
 * clickPoint[TimedPoint3D] - UIコンポーネントとクリック情報
 * draggedRect[TimedShortSeq] -
 * ドラッグされて生成された長方形情報
 * featurePoints[TimedFloatSeq] -
 * 特徴点検出の結果となるKeyPointを配列化したデータ
 * 出力
 * modifyImage[CameraImage] - UIコンポーネントへ表示する画像
 * modifyImagePath[TimedString] - 画像の保存アドレスのパス
 * compLineMap[TimedShortSeq] - 変換が完了した地図のライン情報
 *
 * 輪郭点データ群と特徴点データ群とを合わせることで凸図形を作成し、
 * UI上で修正を行うことでラインマップに変換する。
 * ここでのラインマップは「地図の頂点を示す座標群と、その座標同士の
 * 関係性」を持つ点と線の情報群を指す
 *
 */
class convToLineMap
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  convToLineMap(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~convToLineMap();

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
  // virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   * UIとの連携とマップ情報の変換を行う
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
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
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
   * 輪郭データ群に対するユーザからの操作の種類を選択する
   * - Name: string modi_type
   * - DefaultValue: erase
   * - Constraint: erase -
   *               クリックされた一番近い凸図形頂点を、輪郭データ群
   *               から消す
   *               add -
   *               クリック操作によって隣り合う凸図形頂点を選択して
   *               もらい、その二つの頂点の間に新たに頂点を追加する
   *               追加する頂点は特徴点の中から選択する
   */
  std::string m_modi_type;
  /*!
   * ラインマップの下地を作成する際の、特徴点を頂点座標として用いる
   * かどうかを調べるための閾値
   * - Name: double threshold
   * - DefaultValue: 3
   * - Range: x>=0
   */
  double m_threshold;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::TimedString m_temp_path;
  /*!
   * 一時作業領域のパスを受け取るためのポート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: 大きい画像データの受け渡しや、ファイルの保存など
   *              に用いる作業領域を指定するアドレスとして用いる
   */
  InPort<RTC::TimedString> m_temp_pathIn;
  RTC::CameraImage m_src_img;
  /*!
   * 修復処理を行う画像データを受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: この画像に対し修復処理が行われる
   *              また、UI上に表示される画像もこの画像データを用い
   *              る
   */
  InPort<RTC::CameraImage> m_src_imgIn;
  RTC::TimedShortSeq m_cont_data;
  /*!
   * 配列化されたcontoursとして用いるvector<vector<Point>>のデータ
   * を受信するためのポート
   * - Type: TimedShortSeq
   * - Number: 検出輪郭群数*(1+輪郭点数*2)
   * - Semantics: この輪郭群がラインマップのベースとなる
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
   *              これら9つの要素が配列に格納されている
   *              すなわち[0]~[8]で一つの図形となる
   *              [0]にあたるデータ数分のPointが来ると認識しそのPo
   *              int数分格納したら次のvector処理へ進む
   */
  InPort<RTC::TimedShortSeq> m_cont_dataIn;
  RTC::TimedFloatSeq m_feature_points;
  /*!
   * 特徴検出の出力結果であるキーポイントを配列化したものを受け取る
   * ポート
   * - Type: TimedFloatSeq
   * - Number: 特徴点の数*7
   * - Semantics: この特徴点群を用いてラインマップを修正する
   *              KeyPoint型のデータを、各要素ごとに分解して配列化
   *              されているため
   *              KeyPoint
   *              - float angle [0]
   *              - int class_id [1]
   *              - int octave [2]
   *              - Point pt (pt.x [3] pt.y [4])
   *              - float response [5]
   *              - float size [6]
   *              KeyPointの7個データをそれぞれ配列の一要素として用
   *              いる
   */
  InPort<RTC::TimedFloatSeq> m_feature_pointsIn;
  RTC::TimedPoint3D m_click_point;
  /*!
   * UIでクリックされた座標を受け取るポート
   * - Type: TimedPoint3D
   * - Number: 1
   * - Semantics: Point3Dには
   *              point.x - UI上のx座標
   *              point.y - UI上のy座標
   *              point.z - 操作に対するフラグ情報
   */
  InPort<RTC::TimedPoint3D> m_click_pointIn;
  RTC::TimedShortSeq m_drag_rect;
  /*!
   * UIでドラッグされた長方形を受け取るポート
   * - Type: TimedShortSeq
   * - Number: 4
   * - Semantics: ドラッグで描かれた長方形の左上のx座標,
   *              左上のy座標, 横幅,縦幅を持つshortの配列
   */
  InPort<RTC::TimedShortSeq> m_drag_rectIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::CameraImage m_modi_img;
  /*!
   * ユーザの修正処理のためにUI上で表示する画像を送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 抽出領域を書き込んだ画像や修復処理後の画像
   */
  OutPort<RTC::CameraImage> m_modi_imgOut;
  RTC::TimedString m_modi_img_path;
  /*!
   * ユーザの修正処理のためにUI上で表示する画像のパスを送るポート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: 抽出領域が書き込まれた画像や修復処理後の画像のパ
   *              ス
   */
  OutPort<RTC::TimedString> m_modi_img_pathOut;
  RTC::TimedShortSeq m_map_line;
  /*!
   * 処理の結果完成したラインマップの情報を送るためのポート
   * - Type: TimedShortSeq
   * - Number: 全図形数*(1+図形の持つ凸数*2)
   * - Semantics: ラインマップの情報はvector<vector<Point>>型で出来
   *              上がるため、この情報をTimedShortSeq型へ変換した情
   *              報を送る
   *              Point
   *              p1,p2,p3,p4からなるVector<Point>　convex1に対し
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
   *              と認識し、そのPoint数分格納したら次のvector処理へ
   *              進む
   */
  OutPort<RTC::TimedShortSeq> m_map_lineOut;
  
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
   	string tempName;

	TimedShortSeq old_cont_data;
	TimedFloatSeq old_feature_points;

	vector<vector<Point> > cont_data;			//輪郭点データ
	vector<vector<Point> > line_base;
	vector<vector<Point> > reset_line_base;		//リセット処理に用いるデータ
	vector<vector<Point> > last_line_base;		//戻る処理に用いるデータ

//	vector<KeyPoint> keypoints;		//特徴点が入る変数
	vector<Point> keypoints;		//特徴点が入る変数

	CameraImage src_cam_img;
	CameraImage old_img;

	Mat gray_img;
	Mat gray_color_img;
	Mat src_img;
	Mat pre_img;

	bool remake;
	bool first_flag;
	bool contours_make_flag;
	bool contours_modify_flag;
	bool contours_click_erase_flag;

	int click_step;

	Point click_cont_point;
	Point back_cont_point;
	Point next_cont_point;
	Point second_cont_point;
	Point click_feature_point;
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void convToLineMapInit(RTC::Manager* manager);
};

#endif // CONVTOLINEMAP_H
