// -*- C++ -*-
/*!
 * @file  imageInpaint.h
 * @brief 輪郭情報を用いた画像修復を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef IMAGEINPAINT_H
#define IMAGEINPAINT_H

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
 * @class imageInpaint
 * @brief 輪郭情報を用いた画像修復を行う
 *
 * 入力ポートから受け取った輪郭点データ群の情報を用いて画像修復を行
 * うコンポーネント
 * 受け取った輪郭点データ群に対し、UIコンポーネントを用いて削除等の
 * 操作を行うことができる
 * また、修復に対しても、UIコンポーネントでのドラッグ入力をマスクイ
 * メージとして修復処理コンポーネント(CVInpaint)に送ることで、容易
 * に指定領域の画像修復を行うことができます。
 *
 * 入力
 * tempFolderPath[TimedString] -
 * 画像情報送信に用いる一時保存領域のパス
 * srcImage[CameraImage] - 修復処理を行いたい画像データ
 * contoursRectangles[TimedShortSeq] -
 * 輪郭点群がベースとなっている輪郭長方形データ群
 * contoursConvex[TimedShortSeq] -
 * 輪郭点群がベースとなっている輪郭凸図形データ群
 * clickPoint[TimedPoint3D] - UIコンポーネントとクリック情報
 * draggedRect[TimedShortSeq] -
 * ドラッグされて生成された長方形情報
 * inpaintImage[CameraImage] - 修復処理の実行結果となる画像
 * 出力
 * modifyImage[CameraImage] - UIコンポーネントへ表示する画像
 * modifyImagePath[TimedString] - 及び画像の保存アドレスのパス
 * inpaintSrcImage[CameraImage] - 修復処理のベース画像
 * inpaintMaskImage[CameraImage] - 修復に用いるマスク画像
 * inpaintMaskArea[TimedShortSeq] -
 * マスク画像作成に用いる長方形データ
 * processedImage[CameraImage] - 全ての処理が終わった結果の画像
 *
 * 入力画像と入力輪郭データを用いて修復処理を行い、結果を出力する
 * 輪郭データに対するUIを用いての修正処理、輪郭データを用いての自動
 * 修復処理、UIを用いての手動による修復処理の順で進む
 *
 */
class imageInpaint
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  imageInpaint(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~imageInpaint();

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
   * 画像の輪郭情報を用いての画像修復を行う
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
   * 処理に用いる輪郭データタイプを選ぶ
   * 両方の形式のデータをポートから受け取っている場合に用いる
   * - Name: string cont_type
   * - DefaultValue: Convex
   * - Constraint: Rectangle -
   *               輪郭点データを長方形データに変換したデータ群を用
   *               いて処理を進める
   *               Convex -
   *               輪郭点データを凸図形データに変換したデータ群を用
   *               いて処理を進める
   */
  std::string m_cont_type;
  /*!
   * CVInpaintへ送るマスクの情報を選択する
   * - Name: string mask_select
   * - DefaultValue: Image
   * - Constraint: Image -
   *               マスク画像を作成して出力ポートinpaintMaskImageか
   *               らCVInpaintへ送る
   *               Rectangle -
   *               マスク領域を作成して出力ポートinpaintMaskAreaか
   *               らCVInpaintへ送る
   */
  std::string m_mask_select;
  /*!
   * maskImageを送る場合、全輪郭データを一度に処理するかどうかを選
   * 択する
   * - Name: string mask_type
   * - DefaultValue: All
   * - Constraint: All - すべてのマスク関係データを送る
   *               Single - 単体のマスク関係データを送る
   */
  std::string m_mask_type;

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
  RTC::TimedShortSeq m_cont_rect;
  /*!
   * 輪郭検出によって得た輪郭点群からなる長方形のデータを受け取るた
   * めのポート
   * - Type: TimedShortSeq
   * - Number: 長方形数*4
   * - Semantics: 配列化されたRectangleの要素を受け取る
   *              Rectangle r1の左上の点p1.x,p1.yとwidth,heightが
   *              - r1.p1.x [0]
   *              - r1.p1.y [1]
   *              - r1.width [2]
   *              - r1.height [3]
   *              これら4つの要素が配列に格納されている
   *              すなわち[0]~[3]で一つの長方形となる
   */
  InPort<RTC::TimedShortSeq> m_cont_rectIn;
  RTC::TimedShortSeq m_cont_convex;
  /*!
   * 配列化されたconvexとして用いるvector<Point>のデータを受信する
   * ためのポート
   * - Type: TimedShortSeq
   * - Number: 全図形数*(1+図形の持つ凸数*2)
   * - Semantics: Point p1,p2,p3,p4からなるVector<Point>
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
   *              受け取り側は、[0]にあたるデータ数分のPointが来る
   *              と認識しそのPoint数分格納したら次のvector処理へ進
   *              む
   */
  InPort<RTC::TimedShortSeq> m_cont_convexIn;
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
   * - Semantics: 長方形の左上のx座標, 左上のy座標,
   *              横幅,縦幅を持つshortの配列
   */
  InPort<RTC::TimedShortSeq> m_drag_rectIn;
  RTC::CameraImage m_inpt_img;
  /*!
   * 修復処理が行われた画像を受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 修復処理後の画像データ
   *              CVInpaintから受け取ることを前提としている
   */
  InPort<RTC::CameraImage> m_inpt_imgIn;
  
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
  RTC::CameraImage m_inpt_src_img;
  /*!
   * 修復処理に用いる画像を送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: この画像に対し送り先で修復処理が行われる
   *              CVInpaintに繋がることを前提としている
   */
  OutPort<RTC::CameraImage> m_inpt_src_imgOut;
  RTC::CameraImage m_inpt_mask_img;
  /*!
   * 修復処理に用いるマスク画像を送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: このマスク画像を用いて修復処理が行われる
   *              CVInpaintに繋がることを前提としている
   *              (同タイミングで長方形座標データを送った場合こちら
   *              からの情報が優先して処理される)
   */
  OutPort<RTC::CameraImage> m_inpt_mask_imgOut;
  RTC::TimedShortSeq m_mask_area;
  /*!
   * マスク画像作成に用いる座標群を送るポート
   * - Type: TimedShortSeq
   * - Number: 4
   * - Semantics: この座標群情報を用いてマスク画像が作成されて修復
   *              処理が行われる
   *              CVInpaintと繋がることを前提としている
   */
  OutPort<RTC::TimedShortSeq> m_mask_areaOut;
  RTC::CameraImage m_proc_img;
  /*!
   * すべての処理が終わった画像を送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 輪郭情報を用いて修復処理が完了した画像
   */
  OutPort<RTC::CameraImage> m_proc_imgOut;
  RTC::TimedShort m_step_flag;
  /*!
   * 工程の段階をコントロールパネルに表示するためのポート
   * - Type: TimedShort
   * - Number: 1
   * - Semantics: UIに対する操作が認められた場合に、
   *              そのダイアログをUIで表示することを頼むために使用
   *              する
   */
  OutPort<RTC::TimedShort> m_step_flagOut;
  
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
	TimedShortSeq old_cont_rect;
	TimedShortSeq old_cont_convex;

	vector<Rect> cont_rect;			//輪郭点を変換した長方形
	vector<Rect> reset_cont_rect;	//リセット処理に用いるデータ
	vector<Rect> last_cont_rect;		//戻る処理に用いるデータ

	vector<vector<Point> > cont_convex;			//輪郭点を変換した凸図形
	vector<vector<Point> > reset_cont_convex;	//リセット処理に用いるデータ
	vector<vector<Point> > last_cont_convex;		//戻る処理に用いるデータ

	CameraImage src_cam_img;
	CameraImage old_img;
	CameraImage inpt_cam_img;
	CameraImage old_inpt_img;
	CameraImage reset_inpt_img;
	Mat gray_img;
	Mat gray_color_img;
	Mat src_img;
	Mat pre_img;
	bool remake;
	bool first_flag;
	bool contours_make_flag;
	bool contours_modify_flag;
	bool inpaint_end_flag;
	bool inpaint_modify_flag;
	bool write_data_flag;
	vector<Rect>::iterator inpt_it_rect;
	vector<vector<Point>>::iterator inpt_it_convex;

  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void imageInpaintInit(RTC::Manager* manager);
};

#endif // IMAGEINPAINT_H
