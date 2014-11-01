// -*- C++ -*-
/*!
 * @file  CVHoughThransform.cpp
 * @brief 与えられた二値画像に対してハフ変換を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "CVHoughThransform.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

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
static const char* cvhoughthransform_spec[] =
  {
    "implementation_id", "CVHoughThransform",
    "type_name",         "CVHoughThransform",
    "description",       "与えられた二値画像に対してハフ変換を行う",
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
    "conf.default.02_HoughMethod", "STANDARD",
    "conf.default.03_rho", "1",
    "conf.default.04_theta", "0.0174533",
    "conf.default.05_threshold", "100",
    "conf.default.06_srn", "0",
    "conf.default.07_stn", "0",
    "conf.default.08_minLineLength", "30",
    "conf.default.09_maxLineGap", "10",
    // Widget
    "conf.__widget__.01_ImageView", "radio",
    "conf.__widget__.02_HoughMethod", "radio",
    "conf.__widget__.03_rho", "text",
    "conf.__widget__.04_theta", "text",
    "conf.__widget__.05_threshold", "text",
    "conf.__widget__.06_srn", "text",
    "conf.__widget__.07_stn", "text",
    "conf.__widget__.08_minLineLength", "text",
    "conf.__widget__.09_maxLineGap", "text",
    // Constraints
    "conf.__constraints__.01_ImageView", "(ON,OFF)",
    "conf.__constraints__.02_HoughMethod", "(STANDARD, MULTI_SCALE, PROBABILISTIC)",
    "conf.__constraints__.03_rho", "x>0",
    "conf.__constraints__.04_theta", "x>0",
    "conf.__constraints__.05_threshold", "x>0",
    "conf.__constraints__.06_srn", "x>=0",
    "conf.__constraints__.07_stn", "x>=0",
    "conf.__constraints__.08_minLineLength", "x>0",
    "conf.__constraints__.09_maxLineGap", "x>0",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CVHoughThransform::CVHoughThransform(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_src_imgIn("srcImage", m_src_img),
    m_thre_imgIn("thresholdImage", m_thre_img),
    m_hough_linesOut("houghLines", m_hough_lines)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
CVHoughThransform::~CVHoughThransform()
{
}



RTC::ReturnCode_t CVHoughThransform::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("srcImage", m_src_imgIn);
  addInPort("thresholdImage", m_thre_imgIn);
  
  // Set OutPort buffer
  addOutPort("houghLines", m_hough_linesOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_ImageView", m_img_view, "OFF");
  bindParameter("02_HoughMethod", m_hough_method, "STANDARD");
  bindParameter("03_rho", m_rho, "1");
  bindParameter("04_theta", m_theta, "0.0174533");
  bindParameter("05_threshold", m_threshold, "100");
  bindParameter("06_srn", m_srn, "0");
  bindParameter("07_stn", m_stn, "0");
  bindParameter("08_minLineLength", m_min_length, "30");
  bindParameter("09_maxLineGap", m_max_gap, "10");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVHoughThransform::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVHoughThransform::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVHoughThransform::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t CVHoughThransform::onActivated(RTC::UniqueId ec_id)
{
	/*--------------パラメータの初期化--------------*/
	//ポートの初期化
	while(m_src_imgIn.isNew())m_src_imgIn.read();
	while(m_thre_imgIn.isNew())m_thre_imgIn.read();
	m_hough_lines.data.length(0);

	/*--------コンフィギュの前回値を持つ変数-----------*/

	//メソッド
	method = m_hough_method;

	//HoughLinesの基礎パラメータ
	rho = m_rho;
	theta = m_theta;
	threshold = m_threshold;

	//マルチスケールハフ変換のパラメータ
	srn = m_srn;
	stn = m_stn;

	//確率的ハフ変換のパラメータ
	min_length = m_min_length;
	max_gap = m_max_gap;

	/*--------その他変数-----------*/
	old_img = makeEmptyCamImg();
	thre_img = makeEmptyCamImg();
	gray_img = Mat();
	src_img = Mat();
	pre_img = Mat();
	remake = true;		//再検出フラグ
	lines_p.clear();	//線情報が入る変数
	lines_s.clear();	//線情報が入る変数

  return RTC::RTC_OK;
}

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVHoughThransform::onDeactivated(RTC::UniqueId ec_id)
{
	destroyWindow("receiveImage");
	destroyWindow("houghLineView");

	return RTC::RTC_OK;
}

/*!
 * ハフ変換を行う
 */

RTC::ReturnCode_t CVHoughThransform::onExecute(RTC::UniqueId ec_id)
{
	//入力ポートdefaultImageのデータを読み込む
	if(m_src_imgIn.isNew()){
		m_src_imgIn.read();
	}

	//入力ポートthresholdImageのデータを読み込む
	if(m_thre_imgIn.isNew()){
		m_thre_imgIn.read();
	}

	//defaultImageかthresholdImageのデータがすでに受け取っているデータと違う場合
	bool make_src_img = equalCamImg(old_img,m_src_img);
	bool make_thre_img = equalCamImg(thre_img,m_thre_img);
	if( (!make_src_img || !make_thre_img) && 
		(old_img.pixels.length() != 0 || thre_img.pixels.length() != 0) ){

		//再検出のフラグを立てる
		remake = true;

		//threImageのCameraImageをMat型に変換し、
		//グレースケールでない場合はグレースケルへ変換する
		if(thre_img.pixels.length() != 0){
			cout<<"makeThreImg"<<endl;

			//CameraImage型からMat型へ変換
			gray_img = CamToMat(thre_img).clone();

			//基画像用Matへカラー化してコピー
			if(gray_img.elemSize() == 1){
				cvtColor(gray_img, src_img, CV_GRAY2RGB);
			}
			//二値画像でない場合は変換
			else if(gray_img.elemSize() != 1){
				src_img = gray_img.clone();
				cvtColor(gray_img,gray_img,CV_RGB2GRAY);
				Canny( gray_img, gray_img, 50, 200, 3 );
			}
		}

		//srcImageのCameraImageをMat型に変換し、
		//threImageのデータがない場合はグレースケールへ変換しthreImageの変数へ上書きする
		if(old_img.pixels.length() != 0){
			cout<<"makeSrcImg"<<endl;

			//CameraImage型からMat型へ変換
			src_img = CamToMat(old_img).clone();	
			
			//二値画像が受け取れていない場合
			if(gray_img.data == 0){
				cout<<"makeThreImg"<<endl;

				//二値の変数に上書き
				cvtColor(src_img,gray_img,CV_RGB2GRAY);
				Canny( gray_img, gray_img, 50, 200, 3 );
			}
		}
	}

	if(gray_img.data != 0){
		//メソッドの更新
		if(!equalPara(method,m_hough_method))remake = true;

		//共通パラメータの更新
		if(!equalPara(rho,m_rho))remake = true;
		if(!equalPara(theta,m_theta))remake = true;
		if(!equalPara(threshold,m_threshold))remake = true;

		if(method == "PROBABILISTIC"){
			//確率的ハフ変換のパラメータを更新
			if(!equalPara(min_length,m_min_length))remake =true;
			if(!equalPara(max_gap,m_max_gap))remake =true;

			//再変換フラグが立っている場合
			if(remake == true){
				lines_p.clear();

				//コンフィギュ値を用いて確率的ハフ変換を行う
				HoughLinesP(gray_img,lines_p,rho,theta,threshold,min_length,max_gap);
			}
		}else{
			//マルチスケールハフのパラメータを更新
			if(!equalPara(srn,m_srn))remake =true;
			if(!equalPara(stn,m_stn))remake =true;

			//再変換フラグが立っている場合
			if(remake == true){
				lines_s.clear();
				lines_p.clear();

				//コンフィギュ値を用いてハフ変換を行う
				if(method == "MULTI_SCALE")
					HoughLines(gray_img,lines_s,rho,theta,threshold,srn,stn);
				else
					HoughLines(gray_img,lines_s,rho,theta,threshold);

				//変換結果をOutPortの形に変換
				for(size_t i=0; i<lines_s.size();i++){
					float rho = lines_s[i][0];
					float theta = lines_s[i][1];
					double a = cos(theta), b = sin(theta);
					double x0 = a*rho, y0 = b*rho;
					lines_p.push_back(Vec4i(cvRound(x0 + 1000*(-b)), cvRound(y0 + 1000*(a)),
											cvRound(x0 - 1000*(-b)), cvRound(y0 - 1000*(a))));
				}
			}
		}

		if(remake == true){
			//送信に必要な配列の長さを確保する
			m_hough_lines.data.length(lines_p.size()*4);

			//描画のための前処理
			Scalar color(0,0,255);
			pre_img = src_img.clone();

			for(size_t i = 0; i<lines_p.size(); i++) {
				//描画を行う
				line(pre_img,Point(lines_p[i][0], lines_p[i][1]),
					Point(lines_p[i][2], lines_p[i][3]), color, 1, 1);

				//OutPortに送信
				m_hough_lines.data[i*4+0] = lines_p[i][0];
				m_hough_lines.data[i*4+1] = lines_p[i][1];
				m_hough_lines.data[i*4+2] = lines_p[i][2];
				m_hough_lines.data[i*4+3] = lines_p[i][3];
			}
			setTimestamp(m_hough_lines);
			m_hough_linesOut.write();
			remake = false;
		}

		//画像情報をウィンドウに表示
		if(m_img_view == "ON"){
			if(src_img.data != 0){
				namedWindow("receiveImage",1);
				imshow("receiveImage", src_img);
			}
			if(pre_img.data != 0){
				namedWindow("houghLineView",1);
				imshow("houghLineView", pre_img);
			}
		}else{
			destroyWindow("receiveImage");
			destroyWindow("houghLineView");
		}
	}

	waitKey(1);

	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVHoughThransform::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVHoughThransform::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVHoughThransform::onReset(RTC::UniqueId ec_id)
{
	destroyWindow("receiveImage");
	destroyWindow("houghLineView");

	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVHoughThransform::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVHoughThransform::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void CVHoughThransformInit(RTC::Manager* manager)
  {
    coil::Properties profile(cvhoughthransform_spec);
    manager->registerFactory(profile,
                             RTC::Create<CVHoughThransform>,
                             RTC::Delete<CVHoughThransform>);
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
