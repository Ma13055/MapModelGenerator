// -*- C++ -*-
/*!
 * @file  mapsManager.h
 * @brief 地図画像から地図モデルへの変換を管理し、結果の保存を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef MAPSMANAGER_H
#define MAPSMANAGER_H

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
using namespace std;

/*!
 * @class mapsManager
 * @brief 地図画像から地図モデルへの変換を管理し、結果の保存を行う
 *
 * 地図モデル生成の管理を行うコンポーネント
 * 何枚目の画像に対し地図モデル生成を行うか、処理アルゴリズムが無事
 * 完了したか、など、UIを介してユーザの指示を受け付ける
 *
 * 入力
 * receiveCamImg[CameraImage] - 地図モデル生成に用いる画像データ
 * receiveFlagData[TimedShort] -
 * 送り先コンポーネントの画像データ送信状況
 * tempFloderPath[TimedString] - 画像パスの保管場所
 * clickPoint[TimedPoint3D] - UIでクリックされた座標
 * compNormalizedMap[CameraImage] - 正規化された地図画像データ
 * compLineMap[TimedShortSeq] - 変換されたラインマップの座標群
 * 出力
 * backCamImg[CameraImage] - 送り先から受け取った地図画像
 * tempFolderPathOut[TimedString] - 一時作業領域のパス
 * makeNormSrcImg[CameraImage] - 正規化処理が行われる画像データ
 * makeNormSrcImgPath[TimedString] -
 * 正規化処理が行われる画像データのパス
 * compNormImg[CameraImage] - 正規化処理が完了した画像データ
 * compNormImgPath[TimedString] -
 * 正規化処理が完了した画像データのパス
 * makeLineSrcImg[CameraImage] -
 * ラインマップに変換する地図画像データ
 * makeLineSrcImgPath[TimedString] -
 * ラインマップに変換する地図画像データのパス
 * compLineImg[CameraImage] - ラインマップを描画した画像
 * compLineImgPath[TimedString] -
 * ラインマップを描画した画像のパス
 * lineMaps[TimedShortSeq] - 全地図画像のラインマップ情報
 * lineMapsPath[TImedString] - 全地図画像のラインマップ情報のパス
 *
 * 画像読み込みコンポーネントから画像を受け取り、
 * 1枚目から順に、地図モデル化処理を行う
 * 処理へ出力した画像と処理が完了した画像とのTimeStampを比較し、
 * 同一ならUI上に処理前と処理後の画像を表示する
 * UIから完了である指示が送られて来たら次の画像へ、
 * もう一度の指令が受け渡されたら、もう一度画像データに処理を行う
 * 最終処理でラインマップが完成したと判断されたら指定された場所への
 * 全座標群情報と描画イメージを出力する
 *
 */
class mapsManager
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  mapsManager(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~mapsManager();

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
   * 地図画像を地図モデルへ変化し保存する
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
   * 画像処理及び線画化処理を行う画像の番号を指定できる
   * (先のステップから戻ることができる）
   * - Name: int select_pict_num
   * - DefaultValue: 0
   * - Range: x>=0　かつ　x<=最大枚数
   */
  int m_select_pict_num;
  /*!
   * 完成したラインマップの座標情報群と描画画像を保存する先
   * - Name: string save_space
   * - DefaultValue: C:\\tmp\\makeMapsModel\\maps\\linemaps
   */
  std::string m_save_space;
  /*!
   * 正規化処理を飛ばすかどうかを選択するための変数
   * - Name: string skip_norm
   * - DefaultValue: NON
   * - Constraint: NON:正規化処理を行う
   *               SKIP:正規化処理を飛ばす
   */
  std::string m_skip_norm;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::CameraImage m_rec_cam_img;
  /*!
   * 画像データを受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 地図モデル生成に用いる画像データ
   */
  InPort<RTC::CameraImage> m_rec_cam_imgIn;
  RTC::TimedShort m_rec_flag;
  /*!
   * 画像データの送り先から現在の送信状態を受け取るポート
   * - Type: TimedShort
   * - Number: 1
   * - Semantics: フラグ説明
   *              -2：受け取り状態の初期化のフラグ
   *              -1：全画像データ送信済みのフラグ
   */
  InPort<RTC::TimedShort> m_rec_flagIn;
  RTC::TimedString m_In_temp_path;
  /*!
   * 一時作業領域のパスを受け取るためのポート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: 大きい画像データを送信する場合に用いる、画像パス
   *              の保管場所
   */
  InPort<RTC::TimedString> m_In_temp_pathIn;
  RTC::TimedPoint3D m_click_point;
  /*!
   * UIでクリックされた座標を受け取るポート
   * - Type: TimedPoint3D
   * - Number: 1
   * - Semantics: point.x - UI上のx座標
   *              point.y - UI上のy座標
   *              point.z - 操作に対するフラグ情報
   */
  InPort<RTC::TimedPoint3D> m_click_pointIn;
  RTC::CameraImage m_map_norm;
  /*!
   * 地図の正規化処理が終了した地図画像データを受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 地図から余計な要素を取り除く画像処理によって生成
   *              された地図画像データ
   */
  InPort<RTC::CameraImage> m_map_normIn;
  RTC::TimedShortSeq m_map_line;
  /*!
   * ラインマップ生成処理の結果を受け取るポート
   * - Type: TimedShortSeq
   * - Number: 全図形数*(1+図形の持つ凸数*2)
   * - Semantics: ラインマップを描画する際に用いるvector<vector<Point>
   *              >型の地図座標群
   *              [0][1][2][3][4]…　に対し
   *              [vector<Point>のサイズn*2][p1.x][p1.y][p2.x][p2.
   *              y]…[pn.x][pn.y][vector<Point>のサイズm*2]…
   *              という要素を持つ
   */
  InPort<RTC::TimedShortSeq> m_map_lineIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::CameraImage m_send_cam_img;
  /*!
   * 送り先から受け取った地図画像を、そのまま返すためのポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: フィードバッグ処理を疑似的に再現するために用いる
   *              送り先側でデータの一致を確認することで次画像が送
   *              られる
   */
  OutPort<RTC::CameraImage> m_send_cam_imgOut;
  RTC::TimedString m_out_temp_path;
  /*!
   * 一時作業領域のパスを送るためのポート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: 大きい画像データを送受信する場合に用いる、画像パ
   *              スの保管場所
   */
  OutPort<RTC::TimedString> m_out_temp_pathOut;
  RTC::CameraImage m_norm_src_img;
  /*!
   * 地図の正規化処理を行いたい画像を送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 正規化処理が行われる画像データ
   */
  OutPort<RTC::CameraImage> m_norm_src_imgOut;
  RTC::TimedString m_norm_src_img_path;
  /*!
   * 地図の正規化処理を行いたい画像のパスを送るポート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: 正規化処理が行われる画像のパス
   */
  OutPort<RTC::TimedString> m_norm_src_img_pathOut;
  RTC::CameraImage m_norm_comp_img;
  /*!
   * 正規化処理が完了した画像を送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: 正規化処理が行われた画像データ
   */
  OutPort<RTC::CameraImage> m_norm_comp_imgOut;
  RTC::TimedString m_norm_comp_img_path;
  /*!
   * 正規化処理が完了した画像のパスを送るポート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: 正規化処理が行われた画像のパス
   */
  OutPort<RTC::TimedString> m_norm_comp_img_pathOut;
  RTC::CameraImage m_line_src_img;
  /*!
   * ラインマップに変換したい正規化済み地図画像データを送るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: ラインマップに変換したい正規化済み地図画像データ
   */
  OutPort<RTC::CameraImage> m_line_src_imgOut;
  RTC::TimedString m_line_src_img_path;
  /*!
   * ラインマップに変換したい正規化済み地図画像データのパスを送るポ
   * ート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: ラインマップに変換したい正規化済み地図画像データ
   */
  OutPort<RTC::TimedString> m_line_src_img_pathOut;
  RTC::CameraImage m_line_comp_img;
  /*!
   * ラインマップの座標群情報を描画した画像を送るためのポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: ラインマップを成す座標群情報を描画した画像
   */
  OutPort<RTC::CameraImage> m_line_comp_imgOut;
  RTC::TimedString m_line_comp_img_path;
  /*!
   * ラインマップの座標群情報を描画した画像のパスを送るためのポート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: ラインマップを成す座標群情報を描画した画像のパス
   */
  OutPort<RTC::TimedString> m_line_comp_img_pathOut;
  RTC::TimedShortSeq m_line_maps;
  /*!
   * 全地図画像に対するラインマップの座標群情報を送るためのポート
   * - Type: TimedShortSeq
   * - Number: 全図形数*(1+図形の持つ凸数*2)
   * - Semantics: ラインマップは構成する頂点の座標<Point>を
   *              一つの図形にまとめた<vector<Point>が
   *              複数個存在する<vector<vector<Point>>という形式で
   *              変換される
   *              複数地図画像に対してこれら処理を行うことで
   *              vector<vector<vector<Point>>>型で全地図画像のライ
   *              ンマップを保持することができる
   *              これら情報を一括で送信するためにTimedShortSeq型に
   *              変換する必要がある
   *              Point p1,p2,p3からなるVector<Point>
   *              conv1,conv2を持つ
   *              vector<vector<Point>> floor1,floor2を持つ
   *              全地図画像のラインマップvector<vector<vector<Point>
   *              > map1
   *              に対しての変換例を以下に示す
   *              - map1.size() [0]
   *              - map1.floor1.size() [1]
   *              - map1.floor1.conv1.size() [2]
   *              - map1.floor1.conv1.p1.x [3]
   *              ・・・
   *              - map1.floor1.conv1.p3.y [8]
   *              - map1.floor1.conv2.size() [9]
   *              - map1.floor1.conv2.p1.x [10]
   *              ・・・
   *              - map1.floor1.conv2.p3.y [15]
   *              - map1.floor2.size() [16]
   *              - map1.floor2.conv1.size() [17]
   *              - map1.floor2.conv1.p1.x [18]
   *              ・・・
   *              - map1.floor2.conv2.p3.y [30]
   *              上記のように30の要素を配列に格納して送る
   *              すなわち[0]~[15]で一つのラインマップとなる
   */
  OutPort<RTC::TimedShortSeq> m_line_mapsOut;
  RTC::TimedString m_line_maps_path;
  /*!
   * ラインマップの保存データへのパスを、全地図のパス一括で送るため
   * のポート
   * - Type: TimedString
   * - Number: 1
   * - Semantics: 保存したラインマップデータのファイルのパスを送る
   *              処理した地図が複数枚ある場合でも","で区切って一つ
   *              のstring型にして送信する
   */
  OutPort<RTC::TimedString> m_line_maps_pathOut;
  
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
	int send_pict;
	int conf_pict_num;

	bool receiveEndFlag;
	bool first_flag;
	bool normalized_flag;
	bool convert_line_map_flag;
	bool remake;

	CameraImage old_comp_img;
	CameraImage old_receive_img;
	vector<CameraImage> getPictSeq;
	vector<CameraImage> compPictSeq;
	vector<vector<vector<Point> > > compLineMapSeq;
	vector<string> compMapsPath;

	TimedShortSeq old_map_line;
	vector<vector<Point> > map_line;

	string tempName;

  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void mapsManagerInit(RTC::Manager* manager);
};

#endif // MAPSMANAGER_H
