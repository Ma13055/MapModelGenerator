// -*- C++ -*-
/*!
 * @file  CVFeatureDetector.h
 * @brief 与えられた画像に特徴点検出を行う
 * @date  $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#ifndef CVFEATUREDETECTOR_H
#define CVFEATUREDETECTOR_H

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
using namespace std;
using namespace cv;
/*!
 * @class CVFeatureDetector
 * @brief 与えられた画像に特徴点検出を行う
 *
 * 入力された画像に対して、コンフィギュレーションパラメータで選択さ
 * れたメソッドやアダプタ、入力された閾値を用いて特徴点検出を行うコ
 * ンポーネント
 *
 * 入力
 * srcImage[CameraImage] - 特徴点検出の対象画像を受け取る
 * 出力
 * featurePoint [TimedFloatSeq] -
 * 特徴点検出の結果となるKeyPointを配列化したデータを送る
 *
 * 入力画像に対して特徴検出を行い、結果を描画しつつTimedFloatSeqに
 * 配列化して送る
 * 入力やコンフィギュの値を常に確認し、どこか一つでも変更があるとエ
 * ッジ再検出を行う
 * 出力ポートに送信されるタイミングは、検出後一度きり
 *
 */
class CVFeatureDetector
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  CVFeatureDetector(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~CVFeatureDetector();

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
   * 特徴点検出を行う
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
   * 特徴検出のメソッドを選択するための変数
   * - Name: string feature_method
   * - DefaultValue: Good
   * - Constraint: FAST - FAST()メソッドを用いた特徴検出
   *               Good -
   *               GoodFeaturesToTrack()クラスを用いた特徴検出
   *               GoodHarris -
   *               GoodFeaturesToTrack()クラスにおいて、HarrisDete
   *               ctorにtrueを与えた特徴検出
   *               Star - StarDetector()クラスを用いた特徴検出
   *               SIFT - SIFT()クラスを用いた特徴検出
   *               SURF - SURF()クラスを用いた特徴検出
   *               MSER - MSER()クラスを用いた特徴検出
   */
  std::string m_feature_method;
  /*!
   * 特徴検出のアダプタを選択するための変数
   * - Name: string adapt
   * - DefaultValue: NON
   * - Constraint: NON - アダプタを使用せず検出
   *               Grid
   *               -入力画像をグリッド状に分割して各セルでポイント
   *               検出
   *               Pyramid
   *               -ガウシアンピラミッドの複数レベルから検出
   *               Dynamic
   *               -指定した固定数の特徴が見つかるまで繰り返し検出
   */
  std::string m_adapt;
  /*!
   * FAS()のパラメータ
   * - Name: int fast_threshold
   * - DefaultValue: 1
   * - Range: x>0
   */
  int m_fast_threshold;
  /*!
   * GoodFeatureToTrack()のパラメータ
   * - Name: int good_max
   * - DefaultValue: 1000
   * - Range: x>0
   */
  int m_good_max;
  /*!
   * GoodFeatureToTrack()のパラメータ
   * - Name: double good_level
   * - DefaultValue: 0.01
   * - Range: x>0
   */
  double m_good_level;
  /*!
   * GoodFeatureToTrack()のパラメータ
   * - Name: double good_min
   * - DefaultValue: 1
   * - Range: x>0
   */
  double m_good_min;
  /*!
   * GoodFeatureToTrack()のパラメータ
   * - Name: int good_block
   * - DefaultValue: 3
   * - Range: x>0
   */
  int m_good_block;
  /*!
   * GoodFeatureToTrack()のパラメータ
   * - Name: double good_k
   * - DefaultValue: 0.04
   * - Range: x>0
   */
  double m_good_k;
  /*!
   * StarDetector()のパラメータ
   * - Name: int star_max
   * - DefaultValue: 16
   * - Range: x>0
   */
  int m_star_max;
  /*!
   * StarDetector()のパラメータ
   * - Name: int star_response
   * - DefaultValue: 30
   * - Range: x>0
   */
  int m_star_response;
  /*!
   * StarDetector()のパラメータ
   * - Name: int star_projected
   * - DefaultValue: 10
   * - Range: x>0
   */
  int m_star_projected;
  /*!
   * StarDetector()のパラメータ
   * - Name: int star_binarized
   * - DefaultValue: 8
   * - Range: x>0
   */
  int m_star_binarized;
  /*!
   * StarDetector()のパラメータ
   * - Name: int star_nonmax
   * - DefaultValue: 5
   * - Range: x>0
   */
  int m_star_nonmax;
  /*!
   * SIFT()のパラメータ
   * - Name: double sift_threshold
   * - DefaultValue: 0.05
   * - Range: x>0
   */
  double m_sift_threshold;
  /*!
   * SIFT()のパラメータ
   * - Name: double sift_edge
   * - DefaultValue: 10.0
   * - Range: x>0
   */
  double m_sift_edge;
  /*!
   * SURF()のパラメータ
   * - Name: double surf_threshold
   * - DefaultValue: 400.0
   * - Range: x>0
   */
  double m_surf_threshold;
  /*!
   * 画像から検出されるキーポイントの最大数
   * 強いキーポイントのみが保存される
   * - Name: int grid_max
   * - DefaultValue: 200
   * - Range: x>0
   */
  int m_grid_max;
  /*!
   * グリッドの行数
   * - Name: int grid_rows
   * - DefaultValue: 10
   * - Range: x>0
   */
  int m_grid_rows;
  /*!
   * グリッドの列数
   * - Name: int grid_cols
   * - DefaultValue: 10
   * - Range: x>0
   */
  int m_grid_cols;
  /*!
   * スケーリングレベル
   * - Name: int pyramid_levels
   * - DefaultValue: 3
   * - Range: x>0
   */
  int m_pyramid_levels;
  /*!
   * 出力キーポイント個数の最少数
   * - Name: int dynamic_min
   * - DefaultValue: 400
   * - Range: x>0
   */
  int m_dynamic_min;
  /*!
   * 出力キーポイント個数の最大数
   * - Name: int dynamic_max
   * - DefaultValue: 500
   * - Range: x>0
   */
  int m_dynamic_max;
  /*!
   * 条件通り見つかるまでの特徴検出最大繰り返し回数
   * - Name: int dynamic_iters
   * - DefaultValue: 10
   * - Range: x>0
   */
  int m_dynamic_iters;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::CameraImage m_src_img;
  /*!
   * 特徴点検出を行う画像を受け取るポート
   * - Type: CameraImage
   * - Number: 1
   * - Semantics: この画像に対して特徴点検出を行う
   */
  InPort<RTC::CameraImage> m_src_imgIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::TimedFloatSeq m_feature_points;
  /*!
   * 特徴検出の出力結果であるキーポイントを配列化したもの
   * - Type: TimedFloatSeq
   * - Number: 特徴点の数*7
   * - Semantics: KeyPoint型のデータを、各要素ごとに分解して配列化
   *              して送信する
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
  OutPort<RTC::TimedFloatSeq> m_feature_pointsOut;
  
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
	 //アダプタ
	 string adapt;

	 //FASTのパラメータ
	 int	fast_thre;

	 //固有値のパラメータ
	 int	good_max;
	 double good_level;
	 double good_min;
	 int	good_block;
	 double good_k;

	 //Starのパラメータ
	 int star_max;
	 int star_response;
	 int star_projected;
	 int star_binarized;
	 int star_nonmax;

	 //SIFTのパラメータ
	 double sift_thre;
	 double sift_edge;

	 //SURFのパラメータ
	 double surf_thre;

	 //GRIDアダプタのパラメータ
	 int grid_max;
	 int grid_rows;
	 int grid_cols;

	 //PYRAMIDアダプタのパラメータ
	 int pyramid_levels;

	 //dynamicアダプタのパラメータ
	 int dynamic_min;
	 int dynamic_max;
	 int dynamic_iters;

	 /*--------その他変数-----------*/
	 CameraImage old_img;
	 Mat gray_img;
	 Mat src_img;
	 bool remake;	//再検出フラグ
	 vector<cv::KeyPoint> keypoints;	//特徴点が入る変数
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void CVFeatureDetectorInit(RTC::Manager* manager);
};

#endif // CVFEATUREDETECTOR_H
