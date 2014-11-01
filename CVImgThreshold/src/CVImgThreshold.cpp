// -*- C++ -*-
/*!
 * @file  CVImgThreshold.cpp
 * @brief 与えられた画像に二値化処理を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "CVImgThreshold.h"
#include "VectorConvert.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

using namespace std;
using namespace cv;


// string値を比較し、違う値の場合は比較元に比較対象を上書きする
bool equalPara(string &src, string para);
//double値を比較し、違う値の場合は比較元に比較対象を上書きする
bool equalPara(double &src, double para);
//int値を比較し、違う値の場合は比較元に比較対象を上書きする
bool equalPara(int &src, int para);
//short値を比較し、違う値の場合は比較元に比較対象を上書きする
bool equalPara(short &src, short para);
//与えられた文字列に対応するint値を返す
int switch_string(string str);
//与えられたパラメータを、制約条件に応じた変換を行い代入する
void makePara(bool &otsu,double &min_value,double &max_value,
	double otsu_para,double min_para,double max_para);
void makePara(double &max_value,int &thre_type,int &block_size,double &param1,
	double max_para,double type_para,double block_para,double param1_para);
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
static const char* cvimgthreshold_spec[] =
  {
    "implementation_id", "CVImgThreshold",
    "type_name",         "CVImgThreshold",
    "description",       "与えられた画像に二値化処理を行う",
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
    "conf.default.02_Algorithm", "NON",
    "conf.default.03_Parameter", "0,0,255,0,0,255,0,0,255",
    "conf.default.04_GaussianBlur", "ON",
    "conf.default.05_GaussianSize", "3,3",
    "conf.default.06_BitwiseNot", "ON",
    // Widget
    "conf.__widget__.01_ImageView", "radio",
    "conf.__widget__.02_Algorithm", "ordered_list",
    "conf.__widget__.03_Parameter", "text",
    "conf.__widget__.04_GaussianBlur", "radio",
    "conf.__widget__.05_GaussianSize", "text",
    "conf.__widget__.06_BitwiseNot", "radio",
    // Constraints
    "conf.__constraints__.01_ImageView", "(ON,OFF)",
    "conf.__constraints__.02_Algorithm", "(NON,Binary,BinaryInv,Trunc,ToZero,ToZeroInv,Adaptive_MEAN_CCV,Adaptive_GAUSSIAN_C)",
    "conf.__constraints__.04_GaussianBlur", "(ON,OFF)",
    "conf.__constraints__.05_GaussianSize", "x[0]>=0,x[1]>=0",
    "conf.__constraints__.06_BitwiseNot", "(ON,OFF)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CVImgThreshold::CVImgThreshold(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_rec_imgIn("srcImage", m_rec_img),
    m_thre_imgOut("thresholdImg", m_thre_img)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
CVImgThreshold::~CVImgThreshold()
{
}



RTC::ReturnCode_t CVImgThreshold::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("srcImage", m_rec_imgIn);
  
  // Set OutPort buffer
  addOutPort("thresholdImg", m_thre_imgOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_ImageView", m_img_view, "OFF");
  bindParameter("02_Algorithm", m_thre_algo, "NON");
  bindParameter("03_Parameter", m_thre_para, "0,0,255,0,0,255,0,0,255");
  bindParameter("04_GaussianBlur", m_gaus_blur, "ON");
  bindParameter("05_GaussianSize", m_gaus_size, "3,3");
  bindParameter("06_BitwiseNot", m_bitwise, "ON");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVImgThreshold::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVImgThreshold::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVImgThreshold::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t CVImgThreshold::onActivated(RTC::UniqueId ec_id)
{
	/*--------------パラメータの初期化--------------*/
	while(m_rec_imgIn.isNew()) m_rec_imgIn.read();

	/*--------コンフィギュの前回値を持つ変数-----------*/
	thre_algo = m_thre_algo;
	thre_para = m_thre_para;
	gaus_blur = m_gaus_blur;
	gaus_size = m_gaus_size;
	bitwise = m_bitwise;

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

RTC::ReturnCode_t CVImgThreshold::onDeactivated(RTC::UniqueId ec_id)
{
	destroyWindow("receiveImage");
	destroyWindow("thresholdImage");
  return RTC::RTC_OK;
}

/*!
 * 二値化処理を行う
 */

RTC::ReturnCode_t CVImgThreshold::onExecute(RTC::UniqueId ec_id)
{
	//入力ポートを更新
	if(m_rec_imgIn.isNew()){
		m_rec_imgIn.read();

		//すでに受け取っている画像と違う画像な場合
		if(!equalCamImg(old_img,m_rec_img)){

			//再検出フラグを立てる
			remake = true;

			//NewされたCameraImageをMat型に変換
			src_img = CamToMat(m_rec_img).clone();	//受信した画像
		
			//受け取った画像をグレースケールに変換
			if(src_img.elemSize() != 1)cvtColor(src_img,gray_img,CV_RGB2GRAY);
		}
	}

	if(gray_img.data != 0){
		//共通パラメータの更新
		if(thre_algo != m_thre_algo){
			thre_algo = m_thre_algo;
			remake = true;
		}
		if(thre_para != m_thre_para){
			thre_para = m_thre_para;
			remake = true;
		}
		if(!equalPara(gaus_blur,m_gaus_blur))remake = true;
		if(gaus_size != m_gaus_size){
			gaus_size = m_gaus_size;
			remake = true;
		}
		if(!equalPara(bitwise,m_bitwise))remake = true;

		if(remake == true){
			Mat thre_img = gray_img.clone();

			//画像の平滑化
			if(gaus_blur == "ON"){
				//ガウシアンカーネルサイズを取得
				Size size(gaus_size.at(0),gaus_size.at(1));

				//偶数の場合は奇数に変換
				if(size.height != 0 && size.height%2 == 0){
					size.height += 1;
				}
				if(size.width != 0 && size.width%2 == 0){
					size.width += 1;
				}

				//平均化処理
				GaussianBlur(thre_img,thre_img,size,0);
			}

			int th_para_counter = 0; //コンフィグのパラメータ指定をどこまで読み込んでいるかのフラグ変数
			//ordered_listのデータを読み出し二値化処理を行う
			for( vector<string>::iterator it = thre_algo.begin(); it != thre_algo.end(); ++it ){
			
				string str = *it;
				int algo = switch_string(str);	//行う二値化処理のフラグを入手
						
				//二値化パラメータを初期化
				bool OTSU = true;
				double min_value = 0;
				double max_value = 255;
				int thre_type = CV_THRESH_BINARY;
				int block_size = 3;
				double param1 = 10;

				switch(algo){
				//二値化アルゴリズム：Binary
				case 1:
					if(th_para_counter+2 < thre_para.size()) //配列範囲外へのアクセス防止
						makePara(OTSU,min_value,max_value,thre_para.at(th_para_counter),thre_para.at(th_para_counter+1),thre_para.at(th_para_counter+2));

					if(OTSU == true)
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_BINARY|CV_THRESH_OTSU);
					else
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_BINARY);

					th_para_counter += 3;
					break;

				//二値化アルゴリズム：BinaryInv
				case 2:
					if(th_para_counter+2 < thre_para.size()) //配列範囲外へのアクセス防止
						makePara(OTSU,min_value,max_value,thre_para.at(th_para_counter),thre_para.at(th_para_counter+1),thre_para.at(th_para_counter+2));

					if(OTSU == true)
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_BINARY_INV|CV_THRESH_OTSU);
					else
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_BINARY_INV);

					th_para_counter += 3;
					break;

				//二値化アルゴリズム：Trunc
				case 3:
					if(th_para_counter+2 < thre_para.size()) //配列範囲外へのアクセス防止
						makePara(OTSU,min_value,max_value,thre_para.at(th_para_counter),thre_para.at(th_para_counter+1),thre_para.at(th_para_counter+2));

					if(OTSU == true)
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_TRUNC|CV_THRESH_OTSU);
					else
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_TRUNC);

					th_para_counter += 3;
					break;

				//二値化アルゴリズム：ToZero
				case 4:
					if(th_para_counter+2 < thre_para.size()) //配列範囲外へのアクセス防止
						makePara(OTSU,min_value,max_value,thre_para.at(th_para_counter),thre_para.at(th_para_counter+1),thre_para.at(th_para_counter+2));

					if(OTSU == true)
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_TOZERO|CV_THRESH_OTSU);
					else
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_TOZERO);

					th_para_counter += 3;
					break;

				//二値化アルゴリズム：ToZeroInv
				case 5:
					if(th_para_counter+2 < thre_para.size()) //配列範囲外へのアクセス防止
						makePara(OTSU,min_value,max_value,thre_para.at(th_para_counter),thre_para.at(th_para_counter+1),thre_para.at(th_para_counter+2));

					if(OTSU == true)
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_TOZERO_INV|CV_THRESH_OTSU);
					else
						threshold(thre_img, thre_img, min_value, max_value, CV_THRESH_TOZERO_INV);

					th_para_counter += 3;
					break;

				//二値化アルゴリズム：Adaptive_Mean
				case 6:
					if(th_para_counter+3 < thre_para.size()) //配列範囲外へのアクセス防止
						makePara(max_value,thre_type,block_size,param1,thre_para.at(th_para_counter),thre_para.at(th_para_counter+1),thre_para.at(th_para_counter+2),thre_para.at(th_para_counter+3));
					
					adaptiveThreshold(thre_img, thre_img, max_value,CV_ADAPTIVE_THRESH_MEAN_C,thre_type,block_size,param1);
					
					th_para_counter += 4;
					break;

				//二値化アルゴリズム：Adaptive_Gaussian
				case 7:
					if(th_para_counter+3 < thre_para.size()) //配列範囲外へのアクセス防止
						makePara(max_value,thre_type,block_size,param1,thre_para.at(th_para_counter),thre_para.at(th_para_counter+1),thre_para.at(th_para_counter+2),thre_para.at(th_para_counter+3));

					adaptiveThreshold(thre_img, thre_img, max_value,CV_ADAPTIVE_THRESH_GAUSSIAN_C,thre_type,block_size,param1);

					th_para_counter += 4;
					break;
				}
			}

			//配列の各ビットを反転
			if(bitwise == "ON"){
				bitwise_not(thre_img, thre_img);
			}
			
			//描画変数に上書き
			pre_img = thre_img.clone();

			//出力ポートの型に変換
			m_thre_img = MatToCam(thre_img);
			m_thre_imgOut.write();
			remake = false;
		}

		if(m_img_view == "ON"){
			if(src_img.data != 0){
				namedWindow("receiveImage",1);
				imshow("receiveImage", src_img);
			}
			if(pre_img.data != 0){
				namedWindow("thresholdImage",1);
				imshow("thresholdImage", pre_img);
			}
		}else{
			destroyWindow("receiveImage");
			destroyWindow("thresholdImage");
		}
	}
	waitKey(1);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVImgThreshold::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVImgThreshold::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVImgThreshold::onReset(RTC::UniqueId ec_id)
{
	destroyWindow("receiveImage");
	destroyWindow("thresholdImage");

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVImgThreshold::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVImgThreshold::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void CVImgThresholdInit(RTC::Manager* manager)
  {
    coil::Properties profile(cvimgthreshold_spec);
    manager->registerFactory(profile,
                             RTC::Create<CVImgThreshold>,
                             RTC::Delete<CVImgThreshold>);
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
 * @para int &src	比較元
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
 * short値を比較し、違う値の場合は比較元に比較対象を上書きする
 * 
 * @para short &src	比較元
 * @para short para	比較対象
 * @return bool		同一だった場合はtrueを返す
 */
bool equalPara(short &src, short para){
	if(src != para){
		src = para;
		return false;
	}else{
		return true;
	}
}


/**
 * 与えられた文字列に対応するint値を返す
 * 返すint値はあらかじめ設定している
 * Binary…1 BinaryInv…2 Trunc…3 ToZero…4 ToZeroInv…5
 * Adaptive_MEAN_CCV…6 Adaptive_GAUSSIAN_C…7 それ以外…0
 *
 * @para string str		変換基となる文字列
 * @return int			変換後のint値
 */
int switch_string(string str){

	if(str == "Binary")						return 1;
	else if(str == "BinaryInv")				return 2;
	else if(str == "Trunc")					return 3;
	else if(str == "ToZero")				return 4;
	else if(str == "ToZeroInv")				return 5;
	else if(str == "Adaptive_MEAN_CCV")		return 6;
	else if(str == "Adaptive_GAUSSIAN_C")	return 7;
	else return 0;

}

/**
 * 与えられたパラメータを、制約条件に応じた変換を行い代入する
 * 条件を満たさない場合は代入を行わずに返す
 *
 * @para bool &otsu			thresholdのOTSUパラメータを用いるかどうかに使用される変数
 * @para double &min_value	最小値として使用される変数
 * @para double &max_value	最大値として使用される変数
 * @para double otsu_para	OTSUパラメータの使用可否の判断を前提として与えられた変数
 * @para double min_para　	最小値への代入を前提として与えられた変数
 * @para double max_para　	最大値への代入を前提として与えられた変数
 * @return void
 */
void makePara(bool &otsu,double &min_value,double &max_value,double otsu_para,double min_para,double max_para){

	if(otsu_para != 0)otsu = false;

	if(min_para >= 0 && min_para <= 255) min_value = min_para;

	if(min_value <= max_para && max_para >= 0 && max_para <= 255) max_value = max_para;

}

/**
 * 与えられたパラメータを、制約条件に応じた変換を行い代入する
 * 条件を満たさない場合は代入を行わずに返す
 *
 * @para double &max_value	最大値として使用される変数
 * @para int &thre_type		thresholdのタイプ指定に使用される変数
 * @para int &block_size	ピクセルに対する閾値を計算するために利用される変数
 * @para double &param1		利用する手法に依存するパラメータとして使用される変数
 * @para double max_para	最大値への代入を前提として与えられた変数
 * @para double type_para	使用する二値化タイプを指定する変数 0…BINARY,1…BINARY_INV
 * @para double block_para	ピクセル閾値を設定するための変数 制約：3以上の奇数
 * @para double param1_para 依存パラメータとして使用する変数　制約無
 * @return void
 */
void makePara(double &max_value,int &thre_type,int &block_size,double &param1,double max_para,double type_para,double block_para,double param1_para){
	
	if(max_para >= 0 && max_para <= 255) max_value = max_para;

	if((int)type_para == 1)	thre_type = CV_THRESH_BINARY_INV;
	else thre_type = CV_THRESH_BINARY;
	
	if((int)block_para > 1){
		if((int)block_para%2 == 1) block_size = (int)block_para;
		else block_size = (int)block_para + 1;
	}else{
		block_size = 3;
	}

	param1 = param1_para;
	
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
