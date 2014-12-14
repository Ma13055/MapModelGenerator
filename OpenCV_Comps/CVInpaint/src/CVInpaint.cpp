// -*- C++ -*-
/*!
 * @file  CVInpaint.cpp
 * @brief 与えられた画像に与えられた領域を用いて、修復処理を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "CVInpaint.h"
#include "VectorConvert.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

using namespace std;
using namespace cv;

//CameraImage型のデータをMat型に変換する
Mat CamToMat(CameraImage &m_rec_img);
//Mat型のデータをCameraImage型に変換する
CameraImage MatToCam(Mat &mat_img);
//CameraImage型のデータを初期化する
CameraImage makeEmptyCamImg();
//CameraImage型の比較を行う
bool equalCamImg(CameraImage &src_img,CameraImage &rec_img);
//TimedShortSeq型のデータ二つを比較する
bool equalTimedShortSeq(TimedShortSeq &src,TimedShortSeq &rec);

// Module specification
// <rtc-template block="module_spec">
static const char* cvinpaint_spec[] =
  {
    "implementation_id", "CVInpaint",
    "type_name",         "CVInpaint",
    "description",       "与えられた画像に与えられた領域を用いて、修復処理を行う",
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
    "conf.default.02_InpaintSpace", "0,0,0,0",
    "conf.default.03_InpaintRadius", "0.0",
    "conf.default.04_InpaintFlags", "INPAINT_NS",
    // Widget
    "conf.__widget__.01_ImageView", "radio",
    "conf.__widget__.02_InpaintSpace", "text",
    "conf.__widget__.03_InpaintRadius", "text",
    "conf.__widget__.04_InpaintFlags", "radio",
    // Constraints
    "conf.__constraints__.01_ImageView", "(ON,OFF)",
    "conf.__constraints__.02_InpaintSpace", "x[0], x[1], x[2] >= 0, x[3] >= 0",
    "conf.__constraints__.03_InpaintRadius", "x[0] >= 0, x[1] >= 0",
    "conf.__constraints__.04_InpaintFlags", "(INPAINT_NS,INPAINT_TELEA)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CVInpaint::CVInpaint(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_src_imgIn("inpaintSrcImage", m_src_img),
    m_mask_imgIn("maskImage", m_mask_img),
    m_mask_rectIn("maskRectangle", m_mask_rect),
    m_inp_imgOut("inpaintImage", m_inp_img)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
CVInpaint::~CVInpaint()
{
}



RTC::ReturnCode_t CVInpaint::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("inpaintSrcImage", m_src_imgIn);
  addInPort("maskImage", m_mask_imgIn);
  addInPort("maskRectangle", m_mask_rectIn);
  
  // Set OutPort buffer
  addOutPort("inpaintImage", m_inp_imgOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_ImageView", m_img_view, "OFF");
  bindParameter("02_InpaintSpace", m_inp_space, "0,0,0,0");
  bindParameter("03_InpaintRadius", m_inp_radius, "0.0");
  bindParameter("04_InpaintFlags", m_inp_flag, "INPAINT_NS");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVInpaint::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVInpaint::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVInpaint::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t CVInpaint::onActivated(RTC::UniqueId ec_id)
{
	cout<<"CVInpaint : onActivate : START"<<endl;
	while(m_src_imgIn.isNew()) m_src_imgIn.read();
	while(m_mask_imgIn.isNew()) m_mask_imgIn.read();
	while(m_mask_rectIn.isNew()) m_mask_rectIn.read();

	 /*--------ポートの前回値を持つ変数-----------*/
	src_cam_img = makeEmptyCamImg();
	mask_cam_img = makeEmptyCamImg();
	old_rect.data.length(4);

	/*--------その他変数-----------*/
	src_mat_img = Mat();
	inpaint_mat_img = Mat();
	inp_space = m_inp_space;
	change_inp_space = false;
	change_img_flag = false;

	cout<<"CVInpaint : onActivate : END"<<endl;

  return RTC::RTC_OK;
}

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVInpaint::onDeactivated(RTC::UniqueId ec_id)
{
	destroyWindow("CVInpaint : InpaintSourceImage");
	destroyWindow("CVInpaint : InpaintMaskImage");
	destroyWindow("CVInpaint : InpaintImage");

  return RTC::RTC_OK;
}

/*!
 * 選択領域に修復を行う
 */

RTC::ReturnCode_t CVInpaint::onExecute(RTC::UniqueId ec_id)
{
	Mat mask_mat_img = Mat();

	//コンフィギュで指定されている修復領域が変更された場合
	if(m_inp_space != inp_space){
		inp_space = m_inp_space;
		change_inp_space = true;
	}

	if(change_inp_space){
		//修復対象画像と同じ大きさの黒画像を作成
		mask_mat_img = Mat::zeros(src_mat_img.size(), CV_8U);

		//受け取った領域データをrect型に変換
		Rect mask_rect = Rect(inp_space.at(0),inp_space.at(1),inp_space.at(2),inp_space.at(3));
		//受け取った領域を黒画像中に白く描画
		rectangle( mask_mat_img, mask_rect.tl(), mask_rect.br(), Scalar::all(255), CV_FILLED, 8, 0 );
	}

	//入力ポートdefaultImageのデータを読み込む
	if(m_src_imgIn.isNew()){
		m_src_imgIn.read();

		//すでに受け取っている画像と違う画像な場合
		if(!equalCamImg(src_cam_img,m_src_img)){

			//NewされたCameraImageをMat型に変換
			src_mat_img = CamToMat(m_src_img).clone();	//受信した画像
			change_img_flag = true;
		}
	}

	//入力ポートmaskImageのデータを読み込む
	if(m_mask_imgIn.isNew() && src_mat_img.data != 0 && change_img_flag){
		m_mask_imgIn.read();

		//前回までのmaskImageと違うデータの場合は上書き
		if(!equalCamImg(mask_cam_img,m_mask_img)){
			mask_mat_img = CamToMat(mask_cam_img);
		}
	}

	//マスク画像の受け取りがなく、修正領域のみ受け取った場合
	if(m_mask_rectIn.isNew() && src_mat_img.data != 0 && mask_mat_img.data == 0 && change_img_flag){
		m_mask_rectIn.read();

		//受け取った領域からマスク画像を作成
		if(!equalTimedShortSeq(old_rect,m_mask_rect)){
			//修復対象画像と同じ大きさの黒画像を作成
			mask_mat_img = Mat::zeros(src_mat_img.size(), CV_8U);

			//受け取った領域データをrect型に変換
			Rect mask_rect = Rect(m_mask_rect.data[0],m_mask_rect.data[1],m_mask_rect.data[2],m_mask_rect.data[3]);
			//受け取った領域を黒画像中に白く描画
			rectangle( mask_mat_img, mask_rect.tl(), mask_rect.br(), Scalar::all(255), CV_FILLED, 8, 0 );
		}
	}

	//maskImageが存在する場合
	if(mask_mat_img.data != 0 && change_img_flag){
		//コンフィギュのflagデータによる場合分け
		//flagデータを用いてinpaintを行う
		if(m_inp_flag == "INPAINT_NS")inpaint(src_mat_img, mask_mat_img, inpaint_mat_img, m_inp_radius, INPAINT_NS);
		else inpaint(src_mat_img, mask_mat_img, inpaint_mat_img, m_inp_radius, INPAINT_TELEA);

		//修復済み画像をCameraImage型に変換
		m_inp_img = MatToCam(inpaint_mat_img);
		//タイムスタンプを修復元画像のものに上書き
		m_inp_img.tm = src_cam_img.tm;
		change_img_flag = false;
	}

	//画像データをウィンドウに表示
	if(m_img_view == "ON"){
		if(src_mat_img.data != 0){
			namedWindow("CVInpaint : InpaintSourceImage",1);
			imshow("CVInpaint : InpaintSourceImage",src_mat_img);
		}

		if(mask_mat_img.data != 0){
			namedWindow("CVInpaint : InpaintMaskImage",1);
			imshow("CVInpaint : InpaintMaskImage",mask_mat_img);
		}

		if(inpaint_mat_img.data != 0){
			namedWindow("CVInpaint : InpaintImage",1);
			imshow("CVInpaint : InpaintImage",inpaint_mat_img);
		}
	}else{
		destroyWindow("CVInpaint : InpaintSourceImage");
		destroyWindow("CVInpaint : InpaintMaskImage");
		destroyWindow("CVInpaint : InpaintImage");
	}

	if(m_inp_img.pixels.length() != 0){
		m_inp_imgOut.write();
	}

	waitKey(1);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVInpaint::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVInpaint::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVInpaint::onReset(RTC::UniqueId ec_id)
{
	destroyWindow("CVInpaint : InpaintSourceImage");
	destroyWindow("CVInpaint : InpaintMaskImage");
	destroyWindow("CVInpaint : InpaintImage");

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVInpaint::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVInpaint::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void CVInpaintInit(RTC::Manager* manager)
  {
    coil::Properties profile(cvinpaint_spec);
    manager->registerFactory(profile,
                             RTC::Create<CVInpaint>,
                             RTC::Delete<CVInpaint>);
  }
  
};



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
 * @para Mat &matImg		変換の基となる画像データ
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
 * @return bool					
 */
bool equalCamImg(CameraImage &src_img,CameraImage &rec_img){
	if(		src_img.tm.sec		!=	rec_img.tm.sec	||
			src_img.tm.nsec		!=	rec_img.tm.nsec	||
			src_img.bpp			!=	rec_img.bpp		||
			src_img.height		!=	rec_img.height	||
			src_img.width		!=	rec_img.width	||
		src_img.pixels.length() !=	rec_img.pixels.length()){
			src_img = rec_img;
			return false;
	}else{
		return true;
	}
}

/**
 * TimedShortSeq型のデータ二つを比較する
 *
 * @para TimedShortSeq &src		比較データ１
 * @para TimedShortSeq &rec		比較データ２
 * @return bool	比較結果　同一データであればtrue　違うデータであればfalse
 */
bool equalTimedShortSeq(TimedShortSeq &src,TimedShortSeq &rec){
	if(			src.tm.sec	==	rec.tm.sec
		&&		src.tm.nsec	==	rec.tm.nsec
		&&	src.data.length()	==	rec.data.length()){
			for(int i = 0 ; i < rec.data.length() ; i++){
				if(src.data[i] != rec.data[i]){
					src = rec;
					return false;
				}
			}
	}else{
		src = rec;
		return false;
	}
	return true;
}

