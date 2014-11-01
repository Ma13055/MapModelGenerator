// -*- C++ -*-
/*!
 * @file  CVCanny.cpp
 * @brief 与えられた画像にエッジ検出を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "CVCanny.h"
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
//Mat型のデータをCameraImage型に変換する
CameraImage MatToCam(Mat &mat_img);
//CameraImage型のデータを初期化する
CameraImage makeEmptyCamImg();
//CameraImage型の比較を行う
bool equalCamImg(CameraImage &src_img,CameraImage &rec_img);


// Module specification
// <rtc-template block="module_spec">
static const char* cvcanny_spec[] =
  {
    "implementation_id", "CVCanny",
    "type_name",         "CVCanny",
    "description",       "与えられた画像にエッジ検出を行う",
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
    "conf.default.02_threshold1", "50",
    "conf.default.03_threshold2", "200",
    "conf.default.04_apertureSize", "3",
    "conf.default.05_L2gradient", "false",
    // Widget
    "conf.__widget__.01_ImageView", "radio",
    "conf.__widget__.02_threshold1", "text",
    "conf.__widget__.03_threshold2", "text",
    "conf.__widget__.04_apertureSize", "text",
    "conf.__widget__.05_L2gradient", "radio",
    // Constraints
    "conf.__constraints__.01_ImageView", "(ON,OFF)",
    "conf.__constraints__.02_threshold1", "x>0",
    "conf.__constraints__.03_threshold2", "x>0",
    "conf.__constraints__.04_apertureSize", "x>0",
    "conf.__constraints__.05_L2gradient", "(true,false)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CVCanny::CVCanny(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_src_imgIn("srcImage", m_src_img),
    m_canny_imgOut("cannyImage", m_canny_img)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
CVCanny::~CVCanny()
{
}



RTC::ReturnCode_t CVCanny::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("srcImage", m_src_imgIn);
  
  // Set OutPort buffer
  addOutPort("cannyImage", m_canny_imgOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_ImageView", m_img_view, "OFF");
  bindParameter("02_threshold1", m_thre1, "50");
  bindParameter("03_threshold2", m_thre2, "200");
  bindParameter("04_apertureSize", m_size, "3");
  bindParameter("05_L2gradient", m_gradient, "false");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVCanny::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVCanny::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVCanny::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t CVCanny::onActivated(RTC::UniqueId ec_id)
{
	/*--------------パラメータの初期化--------------*/
	//ポートの初期化
	while(m_src_imgIn.isNew())m_src_imgIn.read();

	/*--------コンフィギュの前回値を持つ変数-----------*/

	thre1 = m_thre1;
	thre2 = m_thre2;
	size = m_size;
	gradient = m_gradient;

	/*--------その他変数-----------*/
	old_img = makeEmptyCamImg();
	gray_img = Mat();
	src_img = Mat();
	pre_img = Mat();
	remake = true;		//再検出フラグ

	return RTC::RTC_OK;
}

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVCanny::onDeactivated(RTC::UniqueId ec_id)
{
	destroyWindow("cannyImageView");
	destroyWindow("receiveImage");

	return RTC::RTC_OK;
}

/*!
 * エッジ検出を行う
 */

RTC::ReturnCode_t CVCanny::onExecute(RTC::UniqueId ec_id)
{
	//入力ポートを更新
	if(m_src_imgIn.isNew()){
		m_src_imgIn.read();

		//すでに受け取っている画像と違う画像な場合
		if(!equalCamImg(old_img,m_src_img)){

			//再検出フラグを立てる
			remake = true;

			//NewされたCameraImageをMat型に変換
			src_img = CamToMat(m_src_img);	//受信した画像
		
			//受け取った画像をグレースケールに変換
			if(src_img.elemSize() != 1)cvtColor(src_img,gray_img,CV_RGB2GRAY);
			else gray_img = src_img.clone();
		}
	}

	//処理画像が存在する場合
	if(gray_img.data != 0){

		//共通パラメータの更新
		if(!equalPara(thre1,m_thre1))remake = true;
		if(!equalPara(thre2,m_thre2))remake = true;
		if(!equalPara(size,m_size))remake = true;
		if(!equalPara(gradient,m_gradient))remake = true;

		//再検出フラグが立っている場合
		if(remake == true){

			Mat gray_copy_img = gray_img.clone();
			//エッジ検出
			if(gradient == "false")Canny( gray_copy_img, gray_copy_img, thre1, thre2, size );
			else Canny( gray_copy_img, gray_copy_img, thre1, thre2, size, true);

			//描画画像を作成
			pre_img = gray_copy_img.clone();

			//エッジ検出結果をOutPortに送信
			m_canny_img = MatToCam(pre_img);
			setTimestamp(m_canny_img);
			m_canny_imgOut.write();

			//検出フラグを下す
			remake = false;
		}

		//画像情報をウィンドウに表示
		if(m_img_view == "ON"){
			if(src_img.data != 0){
				namedWindow("receiveImage",1);
				imshow("receiveImage", src_img);
			}
			if(pre_img.data != 0){
				namedWindow("cannyImageView",1);
				imshow("cannyImageView", pre_img);
			}
		}else{
			destroyWindow("receiveImage");
			destroyWindow("cannyImageView");
		}

	}

	waitKey(1);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVCanny::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVCanny::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVCanny::onReset(RTC::UniqueId ec_id)
{
	destroyWindow("cannyImageView");
	destroyWindow("receiveImage");

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVCanny::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVCanny::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void CVCannyInit(RTC::Manager* manager)
  {
    coil::Properties profile(cvcanny_spec);
    manager->registerFactory(profile,
                             RTC::Create<CVCanny>,
                             RTC::Delete<CVCanny>);
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
 * Mat型のデータをCameraImage型に変換する
 * タイムスタンプは現在の時間を使用
 *
 * @para Mat &mat_img		変換の基となる画像データ
 * @return CameraImage	変換後のCameraImage型の画像データ
 */
CameraImage MatToCam(Mat &mat_img){
	CameraImage cam_img;

	//タイムスタンプを設定
	setTimestamp(cam_img);

	//画像の縦と横を設定
	cam_img.height = mat_img.rows;
	cam_img.width = mat_img.cols;
	
	//画像のビット深度を設定
	cam_img.bpp = mat_img.elemSize()*8;

	//画像のサイズを取得
	cam_img.pixels.length(cam_img.height * cam_img.width * cam_img.bpp/8);

	//一行ずつデータをコピー
	for( int y = 0; y < cam_img.height; y++ ){
		memcpy((void *)&(cam_img.pixels[cam_img.width * cam_img.bpp/8 * y]),
			mat_img.ptr<unsigned char>(y), cam_img.width * cam_img.bpp/8);
	}

	return cam_img;
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
