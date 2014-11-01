// -*- C++ -*-
/*!
 * @file  CVFeatureDetector.cpp
 * @brief 与えられた画像に特徴点検出を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "CVFeatureDetector.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\features2d\features2d.hpp"

using namespace cv;
using namespace std;


// string値を比較し、違う値の場合は比較元に比較対象を上書きする
bool equalPara(string &src, string para);
//double値を比較し、違う値の場合は比較元に比較対象を上書きする
bool equalPara(double &src, double para);
//int値を比較し、違う値の場合は比較元に比較対象を上書きする
bool equalPara(int &src, int para);
//CameraImage型のデータをMat型に変換する
Mat CamToMat(CameraImage &m_rec_img);
//CameraImage型のデータを初期化する
CameraImage makeEmptyCamImg();
//CameraImage型の比較を行う
bool equalCamImg(CameraImage &src_img,CameraImage &rec_img);

// Module specification
// <rtc-template block="module_spec">
static const char* cvfeaturedetector_spec[] =
  {
    "implementation_id", "CVFeatureDetector",
    "type_name",         "CVFeatureDetector",
    "description",       "与えられた画像に特徴点検出を行う",
    "version",           "1.1.0",
    "vendor",            "Masaru Tatekawa(SIT)",
    "category",          "Image Processing",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.01_ImageView", "OFF",
    "conf.default.02_Method", "FAST",
    "conf.default.03_Adapter", "NON",
    "conf.default.04_FAST-threshold", "1",
    "conf.default.05_Good-maxCorners", "1000",
    "conf.default.06_Good-qualityLevel", "0.01",
    "conf.default.07_Good-minDistance", "1",
    "conf.default.08_Good-blockSize", "3",
    "conf.default.09_Good-k", "0.04",
    "conf.default.10_Star-maxSize", "16",
    "conf.default.11_Star-responseThreshold", "30",
    "conf.default.12_Star-lineThresholdProjected", "10",
    "conf.default.13_Star-lineThresholdBinarized", "8",
    "conf.default.14_Star-suppressNonmaxSize", "5",
    "conf.default.15_SIFT-threshold", "0.05",
    "conf.default.16_SIFT-edgeThreshold", "10.0",
    "conf.default.17_SURF-hessianThreshold", "400.0",
    "conf.default.18_Grid-maxTotalKeypoints", "200",
    "conf.default.19_Grid-gridRows", "10",
    "conf.default.20_Grid-gridCols", "10",
    "conf.default.21_Pyramid-levels", "3",
    "conf.default.22_Dynamic_minFeatures", "400",
    "conf.default.23_Dynamic_maxFeatures", "500",
    "conf.default.24_Dynamic_maxIters", "10",
    // Widget
    "conf.__widget__.01_ImageView", "radio",
    "conf.__widget__.02_Method", "radio",
    "conf.__widget__.03_Adapter", "radio",
    "conf.__widget__.04_FAST-threshold", "text",
    "conf.__widget__.05_Good-maxCorners", "text",
    "conf.__widget__.06_Good-qualityLevel", "text",
    "conf.__widget__.07_Good-minDistance", "text",
    "conf.__widget__.08_Good-blockSize", "text",
    "conf.__widget__.09_Good-k", "text",
    "conf.__widget__.10_Star-maxSize", "text",
    "conf.__widget__.11_Star-responseThreshold", "text",
    "conf.__widget__.12_Star-lineThresholdProjected", "text",
    "conf.__widget__.13_Star-lineThresholdBinarized", "text",
    "conf.__widget__.14_Star-suppressNonmaxSize", "text",
    "conf.__widget__.15_SIFT-threshold", "text",
    "conf.__widget__.16_SIFT-edgeThreshold", "text",
    "conf.__widget__.17_SURF-hessianThreshold", "text",
    "conf.__widget__.18_Grid-maxTotalKeypoints", "text",
    "conf.__widget__.19_Grid-gridRows", "text",
    "conf.__widget__.20_Grid-gridCols", "text",
    "conf.__widget__.21_Pyramid-levels", "text",
    "conf.__widget__.22_Dynamic_minFeatures", "text",
    "conf.__widget__.23_Dynamic_maxFeatures", "text",
    "conf.__widget__.24_Dynamic_maxIters", "text",
    // Constraints
    "conf.__constraints__.01_ImageView", "(ON,OFF)",
    "conf.__constraints__.02_Method", "(FAST, Good, GoodHarris, Star, SIFT, SURF, MSER)",
    "conf.__constraints__.03_Adapter", "(NON, Grid, Pyramid, Dynamic)",
    "conf.__constraints__.04_FAST-threshold", "x>0",
    "conf.__constraints__.05_Good-maxCorners", "x>0",
    "conf.__constraints__.06_Good-qualityLevel", "x>0",
    "conf.__constraints__.07_Good-minDistance", "x>0",
    "conf.__constraints__.08_Good-blockSize", "x>0",
    "conf.__constraints__.09_Good-k", "x>0",
    "conf.__constraints__.10_Star-maxSize", "x>0",
    "conf.__constraints__.11_Star-responseThreshold", "x>0",
    "conf.__constraints__.12_Star-lineThresholdProjected", "x>0",
    "conf.__constraints__.13_Star-lineThresholdBinarized", "x>0",
    "conf.__constraints__.14_Star-suppressNonmaxSize", "x>0",
    "conf.__constraints__.15_SIFT-threshold", "x>0",
    "conf.__constraints__.16_SIFT-edgeThreshold", "x>0",
    "conf.__constraints__.17_SURF-hessianThreshold", "x>0",
    "conf.__constraints__.18_Grid-maxTotalKeypoints", "x>0",
    "conf.__constraints__.19_Grid-gridRows", "x>0",
    "conf.__constraints__.20_Grid-gridCols", "x>0",
    "conf.__constraints__.21_Pyramid-levels", "x>0",
    "conf.__constraints__.22_Dynamic_minFeatures", "x>0",
    "conf.__constraints__.23_Dynamic_maxFeatures", "x>0",
    "conf.__constraints__.24_Dynamic_maxIters", "x>0",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CVFeatureDetector::CVFeatureDetector(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_src_imgIn("srcImage", m_src_img),
    m_feature_pointsOut("featurePoints", m_feature_points)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
CVFeatureDetector::~CVFeatureDetector()
{
}



RTC::ReturnCode_t CVFeatureDetector::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("srcImage", m_src_imgIn);
  
  // Set OutPort buffer
  addOutPort("featurePoints", m_feature_pointsOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_ImageView", m_img_view, "OFF");
  bindParameter("02_Method", m_feature_method, "FAST");
  bindParameter("03_Adapter", m_adapt, "NON");
  bindParameter("04_FAST-threshold", m_fast_threshold, "1");
  bindParameter("05_Good-maxCorners", m_good_max, "1000");
  bindParameter("06_Good-qualityLevel", m_good_level, "0.01");
  bindParameter("07_Good-minDistance", m_good_min, "1");
  bindParameter("08_Good-blockSize", m_good_block, "3");
  bindParameter("09_Good-k", m_good_k, "0,04");
  bindParameter("10_Star-maxSize", m_star_max, "16");
  bindParameter("11_Star-responseThreshold", m_star_response, "30");
  bindParameter("12_Star-lineThresholdProjected", m_star_project, "10");
  bindParameter("13_Star-lineThresholdBinarized", m_star_binarized, "8");
  bindParameter("14_Star-suppressNonmaxSize", m_star_nonmax, "5");
  bindParameter("15_SIFT-threshold", m_sift_threshold, "0.05");
  bindParameter("16_SIFT-edgeThreshold", m_sift_edge, "10.0");
  bindParameter("17_SURF-hessianThreshold", m_surf_threshold, "400.0");
  bindParameter("18_Grid-maxTotalKeypoints", m_grid_max, "200");
  bindParameter("19_Grid-gridRows", m_grid_rows, "10");
  bindParameter("20_Grid-gridCols", m_grid_cols, "10");
  bindParameter("21_Pyramid-levels", m_pyramid_levels, "3");
  bindParameter("22_Dynamic_minFeatures", m_dynamic_min, "400");
  bindParameter("23_Dynamic_maxFeatures", m_dynamic_max, "500");
  bindParameter("24_Dynamic_maxIters", m_dynamic_iters, "10");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVFeatureDetector::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVFeatureDetector::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVFeatureDetector::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t CVFeatureDetector::onActivated(RTC::UniqueId ec_id)
{
	/*--------------パラメータの初期化--------------*/
	//ポートの初期化
	while(m_src_imgIn.isNew())m_src_imgIn.read();
	m_feature_points.data.length(0);

	//メソッド
	method = m_feature_method;
	//アダプタ
	adapt = m_adapt;

	//FASTのパラメータ
	fast_thre = m_fast_threshold;

	//固有値のパラメータ
	good_max	= m_good_max;
	good_level	= m_good_level;
	good_min	= m_good_min;
	good_block	= m_good_block;
	good_k		= m_good_k;

	//Starのパラメータ
	star_max		= m_star_max;
	star_response	= m_star_response;
	star_project	= m_star_project;
	star_binarized	= m_star_binarized;
	star_nonmax		= m_star_nonmax;

	//SIFTのパラメータ
	sift_thre	= m_sift_threshold;
	sift_edge	= m_sift_edge;

	//SURFのパラメータ
	surf_thre	= m_surf_threshold;

	//GRIDアダプタのパラメータ
	grid_max	= m_grid_max;
	grid_rows	= m_grid_rows;
	grid_cols	= m_grid_cols;

	//PYRAMIDアダプタのパラメータ
	pyramid_levels	= m_pyramid_levels;

	//dynamicアダプタのパラメータ
	dynamic_min		= m_dynamic_min;
	dynamic_max		= m_dynamic_max;
	dynamic_iters	= m_dynamic_iters;

	//再検出フラグ
	remake = true;

	//内部で使用する変数のクリア
	keypoints.clear();
	src_img = Mat();
	gray_img = Mat();

  return RTC::RTC_OK;
}

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVFeatureDetector::onDeactivated(RTC::UniqueId ec_id)
{
	destroyWindow("receiveImage");
	destroyWindow("featurePointsView");

  return RTC::RTC_OK;
}

/*!
 * 特徴点検出を行う
 */

RTC::ReturnCode_t CVFeatureDetector::onExecute(RTC::UniqueId ec_id)
{
	if(m_src_imgIn.isNew()){
		m_src_imgIn.read();

		if(!equalCamImg(old_img,m_src_img)){
			remake = true;

			//NewされたCameraImageをMat型に変換
			src_img = CamToMat(m_src_img);	//受信した画像

			//受け取った画像をウィンドウに表示
			if(m_img_view == "ON"){
				namedWindow("receiveImage",1);
				imshow("receiveImage", src_img);
			}else{
				destroyWindow("receiveImage");
			}
		
			//受け取った画像をグレースケールに変換
			if(src_img.elemSize() != 1)cvtColor(src_img,gray_img,CV_RGB2GRAY);
		}
	}

	if(gray_img.data != 0){
		if(!equalPara(method,m_feature_method))remake = true;
		if(!equalPara(adapt,m_adapt))remake = true;

		if(method == "FAST"){
			//FASTのパラメータを更新
			if(!equalPara(fast_thre,m_fast_threshold))remake =true;

			if(adapt == "Grid"){
				//Gridのパラメータ更新
				if(!equalPara(grid_max,m_grid_max))remake = true;
				if(!equalPara(grid_rows,m_grid_rows))remake = true;
				if(!equalPara(grid_cols,m_grid_cols))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					GridAdaptedFeatureDetector detector_grb(new FastFeatureDetector(fast_thre), grid_max, grid_rows, grid_cols);
					detector_grb.detect(gray_img, keypoints);
				}
			}else if(adapt == "Pyramid"){
				//Pyramidのパラメータ更新
				if(!equalPara(pyramid_levels,m_pyramid_levels))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					PyramidAdaptedFeatureDetector detector_pyr(new FastFeatureDetector(fast_thre), pyramid_levels);
					detector_pyr.detect(gray_img, keypoints);
				}
			}else if(adapt == "Dynamic"){
				//Dynamicのパラメータ更新
				if(!equalPara(dynamic_min,m_dynamic_min))remake = true;
				if(!equalPara(dynamic_max,m_dynamic_max))remake = true;
				if(!equalPara(dynamic_iters,m_dynamic_iters))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					DynamicAdaptedFeatureDetector detector_dyn(new FastAdjuster(fast_thre),  dynamic_min, dynamic_max, dynamic_iters);
					detector_dyn.detect(gray_img, keypoints);
				}
			}else{
				//特徴点検出
				if(remake == true){
					keypoints.clear();
					FastFeatureDetector detector_fast(fast_thre, false);
					detector_fast.detect(gray_img, keypoints);
				}
			}
		}else if(method == "Good" || method == "GoodHarris"){
			//固有値のパラメータを更新
			if(!equalPara(good_max,m_good_max))remake =true;
			if(!equalPara(good_level,m_good_level))remake =true;
			if(!equalPara(good_min,m_good_min))remake =true;
			if(!equalPara(good_block,m_good_block))remake =true;
			if(!equalPara(good_k,good_k))remake =true;
			bool harris = false;
			if(method == "GoodHarris") harris = true;

			if(adapt == "Grid"){
				//Gridのパラメータ更新
				if(!equalPara(grid_max,m_grid_max))remake = true;
				if(!equalPara(grid_rows,m_grid_rows))remake = true;
				if(!equalPara(grid_cols,m_grid_cols))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					GridAdaptedFeatureDetector detector_grb(new GoodFeaturesToTrackDetector(good_max, good_level, good_min, good_block, harris, good_k), grid_max, grid_rows, grid_cols);
					detector_grb.detect(gray_img, keypoints);
				}
			}else if(adapt == "Pyramid"){
				//Pyramidのパラメータ更新
				if(!equalPara(pyramid_levels,m_pyramid_levels))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					PyramidAdaptedFeatureDetector detector_pyr(new GoodFeaturesToTrackDetector(good_max, good_level, good_min, good_block, harris, good_k), pyramid_levels);
					detector_pyr.detect(gray_img, keypoints);
				}
			}else{
				//特徴点検出
				if(remake == true){
					keypoints.clear();
					GoodFeaturesToTrackDetector detector_eig(good_max, good_level, good_min, good_block, harris, good_k);
					detector_eig.detect(gray_img, keypoints);
				}
			}
		}else if(method == "Star"){
			//Starのパラメータを更新
			if(!equalPara(star_max,m_star_max))remake =true;
			if(!equalPara(star_response,m_star_response))remake =true;
			if(!equalPara(star_project,m_star_project))remake =true;
			if(!equalPara(star_binarized,m_star_binarized))remake =true;
			if(!equalPara(star_nonmax,m_star_nonmax))remake =true;

			if(adapt == "Grid"){
				//Gridのパラメータ更新
				if(!equalPara(grid_max,m_grid_max))remake = true;
				if(!equalPara(grid_rows,m_grid_rows))remake = true;
				if(!equalPara(grid_cols,m_grid_cols))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					GridAdaptedFeatureDetector detector_grb(new StarFeatureDetector(star_max, star_response, star_project, star_binarized, star_nonmax), grid_max, grid_rows, grid_cols);
					detector_grb.detect(gray_img, keypoints);
				}
			}else if(adapt == "Pyramid"){
				//Pyramidのパラメータ更新
				if(!equalPara(pyramid_levels,m_pyramid_levels))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					PyramidAdaptedFeatureDetector detector_pyr(new StarFeatureDetector(star_max, star_response, star_project, star_binarized, star_nonmax), pyramid_levels);
					detector_pyr.detect(gray_img, keypoints);
				}
			}else if(adapt == "Dynamic"){
				//Dynamicのパラメータ更新
				if(!equalPara(dynamic_min,m_dynamic_min))remake = true;
				if(!equalPara(dynamic_max,m_dynamic_max))remake = true;
				if(!equalPara(dynamic_iters,m_dynamic_iters))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					DynamicAdaptedFeatureDetector detector_dyn(new StarAdjuster(),  dynamic_min, dynamic_max, dynamic_iters);
					detector_dyn.detect(gray_img, keypoints);
				}
			}else{
				//特徴点検出
				if(remake == true){
					keypoints.clear();
					StarFeatureDetector detector_star(star_max, star_response, star_project, star_binarized, star_nonmax);
					detector_star.detect(gray_img, keypoints);
				}
			}
		}else if(method == "SIFT"){
			//SIFTのパラメータを更新
			if(!equalPara(sift_thre,m_sift_threshold))remake =true;
			if(!equalPara(sift_edge,m_sift_edge))remake =true;

			if(adapt == "Grid"){
				//Gridのパラメータ更新
				if(!equalPara(grid_max,m_grid_max))remake = true;
				if(!equalPara(grid_rows,m_grid_rows))remake = true;
				if(!equalPara(grid_cols,m_grid_cols))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					GridAdaptedFeatureDetector detector_grb(new SiftFeatureDetector(sift_thre,sift_edge), grid_max, grid_rows, grid_cols);
					detector_grb.detect(gray_img, keypoints);
				}
			}else if(adapt == "Pyramid"){
				//Pyramidのパラメータ更新
				if(!equalPara(pyramid_levels,m_pyramid_levels))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					PyramidAdaptedFeatureDetector detector_pyr(new SiftFeatureDetector(sift_thre,sift_edge), pyramid_levels);
					detector_pyr.detect(gray_img, keypoints);
				}
			}else{
				//特徴点検出
				if(remake == true){
					keypoints.clear();
					SiftFeatureDetector detector_sift(sift_thre,sift_edge);
					detector_sift.detect(gray_img, keypoints);
				}
			}
		}else if(method == "SURF"){
			//SURFのパラメータを更新
			if(!equalPara(surf_thre,m_surf_threshold))remake =true;

			if(adapt == "Grid"){
				//Gridのパラメータ更新
				if(!equalPara(grid_max,m_grid_max))remake = true;
				if(!equalPara(grid_rows,m_grid_rows))remake = true;
				if(!equalPara(grid_cols,m_grid_cols))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					GridAdaptedFeatureDetector detector_grb(new SurfFeatureDetector(surf_thre), grid_max, grid_rows, grid_cols);
					detector_grb.detect(gray_img, keypoints);
				}
			}else if(adapt == "Pyramid"){
				//Pyramidのパラメータ更新
				if(!equalPara(pyramid_levels,m_pyramid_levels))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					PyramidAdaptedFeatureDetector detector_pyr(new SurfFeatureDetector(surf_thre), pyramid_levels);
					detector_pyr.detect(gray_img, keypoints);
				}
			}else if(adapt == "Dynamic"){
				//Dynamicのパラメータ更新
				if(!equalPara(dynamic_min,m_dynamic_min))remake = true;
				if(!equalPara(dynamic_max,m_dynamic_max))remake = true;
				if(!equalPara(dynamic_iters,m_dynamic_iters))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					DynamicAdaptedFeatureDetector detector_dyn(new SurfAdjuster(),  dynamic_min, dynamic_max, dynamic_iters);
					detector_dyn.detect(gray_img, keypoints);
				}
			}else{
				//特徴点検出
				if(remake == true){
					keypoints.clear();
					SurfFeatureDetector detector_surf(surf_thre);
					detector_surf.detect(gray_img, keypoints);
				}
			}
		}else if(method == "MSER"){
			if(adapt == "Grid"){
				//Gridのパラメータ更新
				if(!equalPara(grid_max,m_grid_max))remake = true;
				if(!equalPara(grid_rows,m_grid_rows))remake = true;
				if(!equalPara(grid_cols,m_grid_cols))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					GridAdaptedFeatureDetector detector_grb(new MserFeatureDetector(), grid_max, grid_rows, grid_cols);
					detector_grb.detect(gray_img, keypoints);
				}
			}else if(adapt == "Pyramid"){
				//Pyramidのパラメータ更新
				if(!equalPara(pyramid_levels,m_pyramid_levels))remake = true;

				//特徴点検出
				if(remake == true){
					keypoints.clear();
					PyramidAdaptedFeatureDetector detector_pyr(new MserFeatureDetector(), pyramid_levels);
					detector_pyr.detect(gray_img, keypoints);
				}
			}else{
				//特徴点検出
				if(remake == true){
					keypoints.clear();
					MserFeatureDetector detector_mser;
					detector_mser.detect(gray_img, keypoints);
				}
			}
		}

		if(remake == true){
			m_feature_points.data.length(keypoints.size()*7);

			Scalar color(255,255,0);
			Mat pre_img = src_img.clone();
			int itk_counter=0;
			for(vector<KeyPoint>::iterator itk = keypoints.begin(); itk!=keypoints.end(); ++itk) {
				KeyPoint fp = *itk;
				m_feature_points.data[itk_counter*7+0] = fp.pt.x;
				m_feature_points.data[itk_counter*7+1] = fp.pt.y;
				m_feature_points.data[itk_counter*7+2] = fp.size;
				m_feature_points.data[itk_counter*7+3] = fp.angle;
				m_feature_points.data[itk_counter*7+4] = fp.response;
				m_feature_points.data[itk_counter*7+5] = fp.octave;
				m_feature_points.data[itk_counter*7+6] = fp.class_id;

				circle(pre_img, itk->pt, 1, color, -1);
				/*
				circle(pre_img, itk->pt, itk->size, color, 1, CV_AA);
				if(itk->angle>=0) {
					Point pt2(itk->pt.x + cos(itk->angle)*itk->size, itk->pt.y + sin(itk->angle)*itk->size);
					line(pre_img, itk->pt, pt2, color, 1, CV_AA);
				}
				*/
				itk_counter++;
			}

			//特徴点を描画した画像をウィンドウに表示
			if(m_img_view == "ON"){
				namedWindow("featurePointsView",1);
				imshow("featurePointsView", pre_img);
			}else{
				destroyWindow("featurePointsView");
			}

			setTimestamp(m_feature_points);
			m_feature_pointsOut.write();
			remake = false;
		}
	}

	waitKey(1);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVFeatureDetector::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVFeatureDetector::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVFeatureDetector::onReset(RTC::UniqueId ec_id)
{
	destroyWindow("receiveImage");
	destroyWindow("featurePointsView");

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVFeatureDetector::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVFeatureDetector::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void CVFeatureDetectorInit(RTC::Manager* manager)
  {
    coil::Properties profile(cvfeaturedetector_spec);
    manager->registerFactory(profile,
                             RTC::Create<CVFeatureDetector>,
                             RTC::Delete<CVFeatureDetector>);
  }
  
};



/**
 * string値を比較し、違う値の場合は比較元に比較対象を上書きする
 * 
 * @para string &src	比較元
 * @para string para	比較対象
 * @return bool			同一だった場合はtrueを返す
 */
bool equalPara(string &src, string para){
	if(src != para){
		src = para;
		return false;
	}else{
		return true;
	}
}

/**
 * double値を比較し、違う値の場合は比較元に比較対象を上書きする
 * 
 * @para double &src	比較元
 * @para double para	比較対象
 * @return bool			同一だった場合はtrueを返す
 */
bool equalPara(double &src, double para){
	if(src != para){
		src = para;
		return false;
	}else{
		return true;
	}
}

/**
 * int値を比較し、違う値の場合は比較元に比較対象を上書きする
 * 
 * @para itn &src	比較元
 * @para int para	比較対象
 * @return bool		同一だった場合はtrueを返す
 */
bool equalPara(int &src, int para){
	if(src != para){
		src = para;
		return false;
	}else{
		return true;
	}
}

/**
 * CameraImage型のデータをMat型に変換する
 *
 * @para CameraImage &camImg	変換の基となる画像データ
 * @return Mat				変換後のMat型の画像データ
 */
Mat CamToMat(CameraImage &cam_img){

	switch(cam_img.bpp){
	case 8:
		return Mat(cam_img.height, cam_img.width, CV_8UC1, &cam_img.pixels[0]);
	case 24:
		return Mat(cam_img.height, cam_img.width, CV_8UC3, &cam_img.pixels[0]);
	default :
		return Mat(cam_img.height, cam_img.width, CV_8UC3, &cam_img.pixels[0]);
	}

}

/**
 * CameraImage型のデータを初期化する
 *
 * @return CameraImage	空データ(0値)が入ったCameraImage型
 */
CameraImage makeEmptyCamImg(){
	CameraImage c_img;

	c_img.bpp = 0;
	c_img.height = 0;
	c_img.width = 0;
	c_img.pixels.length(0);

	return c_img;
}

/**
 * CameraImage型の保持データと更新データの二つのデータを比較して、
 * 更新データが保持データと違うデータであれば、保持データに更新データをコピーする
 *
 * @para CameraImage &src_img	すでに保持している画像データ
 * @para CameraImage &rec_img	更新されているかの比較に用いる画像データ
 * @return bool					等しい場合はtruueを返す
 */
bool equalCamImg(CameraImage &src_img,CameraImage &rec_img){
	if(src_img.tm.sec	!= rec_img.tm.sec	||
		src_img.tm.nsec != rec_img.tm.nsec	||
		src_img.bpp		!= rec_img.bpp		||
		src_img.height	!= rec_img.height	||
		src_img.width	!= rec_img.width	||
		src_img.pixels.length() != rec_img.pixels.length()){
			src_img = rec_img;
			cout<<"chImg"<<endl;
			return false;
	}else{
		return true;
	}
}

