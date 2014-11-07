// -*- C++ -*-
/*!
 * @file  OpenPicts.cpp
 * @brief 与えられたファイル名から複数画像の読み込みと送信を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "OpenPicts.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

using namespace cv;
using namespace std;

//文字列の分割を行う
vector<string> split(const string &str, char delim);
//Mat型のデータをCameraImage型に変換する
CameraImage MatToCam(Mat &mat_img);
//CameraImage型のデータをMat型に変換する
Mat CamToMat(CameraImage &m_recImg);
//CameraImage型のデータを初期化する
CameraImage makeEmptyCamImg();

// Module specification
// <rtc-template block="module_spec">
static const char* openpicts_spec[] =
  {
    "implementation_id", "OpenPicts",
    "type_name",         "OpenPicts",
    "description",       "与えられたファイル名から複数画像の読み込みと送信を行う",
    "version",           "1.1.0",
    "vendor",            "Masaru Tatekawa(SIT)",
    "category",          "UI",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.01_ImageView", "OFF",
    "conf.default.02_FileName", "*.jpg",
    // Widget
    "conf.__widget__.01_ImageView", "radio",
    "conf.__widget__.02_FileName", "text",
    // Constraints
    "conf.__constraints__.01_ImageView", "(ON,OFF)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
OpenPicts::OpenPicts(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_file_nameIn("FileName", m_file_name),
    m_rec_cam_imgIn("backCameraImage", m_rec_cam_img),
    m_send_cam_imgOut("sendCameraImage", m_send_cam_img),
    m_send_flagOut("sendFlagData", m_send_flag)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
OpenPicts::~OpenPicts()
{
}



RTC::ReturnCode_t OpenPicts::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("FileName", m_file_nameIn);
  addInPort("backCameraImage", m_rec_cam_imgIn);
  
  // Set OutPort buffer
  addOutPort("sendCameraImage", m_send_cam_imgOut);
  addOutPort("sendFlagData", m_send_flagOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_ImageView", m_img_view, "OFF");
  bindParameter("02_FileName", m_FileName, "*.jpg");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t OpenPicts::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenPicts::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenPicts::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t OpenPicts::onActivated(RTC::UniqueId ec_id)
{
	cout<<"onActivated : START"<<endl;


	/*--------------パラメータの初期化--------------*/
	//ポートの初期化
	while(m_rec_cam_imgIn.isNew()) m_rec_cam_imgIn.read();
	while(m_file_nameIn.isNew()) m_file_nameIn.read();

	FN = m_FileName;
	FN_conf = m_FileName;
	FN_port = "*.jpg";

	out_datas.clear();

	onExStartFlag = false;
	if(FN != "*.jpg")change_name_flag = true;
	else change_name_flag = false;
	send_flag = false;
	send_loop = true;

	cout<<"onActivated : SUCCESS"<<endl;

  return RTC::RTC_OK;
}

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t OpenPicts::onDeactivated(RTC::UniqueId ec_id)
{
	cout<<"onDeactivated : START"<<endl;

	destroyWindow("OpenImage");

	cout<<"onDeactivated : SUCCESS"<<endl;

  return RTC::RTC_OK;
}

/*!
 * 画像の読み込みを行う
 */

RTC::ReturnCode_t OpenPicts::onExecute(RTC::UniqueId ec_id)
{

	if(onExStartFlag == false){
		cout<<"onExecute : START"<<endl;
		onExStartFlag = true;
	}


	if(m_file_nameIn.isNew()){
		m_file_nameIn.read();

		String st = m_file_name.data;
		//InportのfileNameに変更があった場合
		//ファイル読み込みのフラグを立てる
		if(FN_port != st){
			FN_port = st;
			FN = FN_port;
			change_name_flag = true;
		}
	}

	//コンフィグのfileNameに変更があった場合
	//ファイル読み込みのフラグを立てる
	if(FN_conf != m_FileName){
		FN_conf = m_FileName;
		FN = FN_conf;
		change_name_flag = true;
	}

	

	//読み込みファイル情報に変更があった場合
	if( change_name_flag == true ){
		change_name_flag = false;

		//読み込み画像ファイルが変わることに対する、変数の初期化
		out_datas.clear();
		m_send_flag.data = -2;
		setTimestamp(m_send_flag);
		m_send_flagOut.write();

		//コンフィギュレーションパラメータのStriangを読み込み、1ファイル単位に分割
		vector<string> file_name = split(FN,',');
		for( vector<string>::iterator it = file_name.begin(); it != file_name.end(); ++it ){
			cout<<"Try Open:"<<*it<<endl;

			string str = *it;
			Mat p_img;

			//画像の読み込み
			p_img = imread(*it);
			if( p_img.empty() ){
				cout<<"Cannot Open:"<<*it<<endl;
				continue;	//ファイル読み込みに失敗した場合、vectorの次要素へ
			}


			//出力ポートの型に変換
			CameraImage cam_img = MatToCam(p_img);

			out_datas.push_back( cam_img );	//vectorに保存

			cout<<"SUCCESS : "<<*it
				<<"\ntm.sec : "<<cam_img.tm.sec
				<<"\ntm.nsec : "<<cam_img.tm.nsec
				<<"\nheight : "<<cam_img.height
				<<"\nwidth : "<<cam_img.width
				<<"\nSize : "<<cam_img.pixels.length()<<endl;
		}

		send_flag = false;
		send_loop = true;
		if(out_datas.size()!=0) pict_it = out_datas.begin();
		while(1){
			if(!m_rec_cam_imgIn.isNew())break;
			else m_rec_cam_imgIn.read();
		}
		cout<<"Output START"<<endl;
	}

	//読み込んだ画像が1枚以上ある場合
	if(out_datas.size() != 0 && send_loop == true){

		if(send_flag == false){
			cout<<"SendImage"<<endl;
			m_send_cam_img = *pict_it;
			m_send_cam_imgOut.write();
			send_flag = true;

			if(m_img_view == "ON"){
				namedWindow("OpenImage",1);
				imshow("OpenImage",CamToMat(m_send_cam_img));
			}else{
				destroyWindow("OpenImage");
			}

		}else{
			if(m_rec_cam_imgIn.isNew()){
				cout<<"ReadImage"<<endl;
				m_rec_cam_imgIn.read();

				if(m_rec_cam_img.tm.sec == m_send_cam_img.tm.sec
					&& m_rec_cam_img.tm.nsec == m_send_cam_img.tm.nsec){
						cout<<"ReceiveImage"<<endl;
						send_flag = false;
						pict_it++;
						if(pict_it == out_datas.end()){
							m_send_flag.data = -1;
							setTimestamp(m_send_flag);
							m_send_flagOut.write();
							send_loop = false;
							cout<<"SendFIN"<<endl;
						}
				}
			}
		}
	}


	waitKey(1);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t OpenPicts::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenPicts::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t OpenPicts::onReset(RTC::UniqueId ec_id)
{
	destroyWindow("OpenImage");

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t OpenPicts::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenPicts::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void OpenPictsInit(RTC::Manager* manager)
  {
    coil::Properties profile(openpicts_spec);
    manager->registerFactory(profile,
                             RTC::Create<OpenPicts>,
                             RTC::Delete<OpenPicts>);
  }
  
};

/**
* 文字列の分割を行う
* @para const string &str	分割の対象となる文字列
* @para char delim			分割のサインとなる文字
* @return vector<string>	分割結果の文字列のvector
*/
vector<string> split(const string &str, char delim){
	vector<string> res;
	size_t current = 0, found;
	while((found = str.find_first_of(delim, current)) != string::npos){
		res.push_back(string(str, current, found - current));
		current = found + 1;
	}
	res.push_back(string(str, current, str.size() - current));
	return res;		//分割されない場合は受け取ったstrの情報を0分割して返す
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
 * CameraImage型のデータをMat型に変換する
 *
 * @para CameraImage &camImg	変換の基となる画像データ
 * @return Mat				変換後のMat型の画像データ
 */
Mat CamToMat(CameraImage &camImg){

	switch(camImg.bpp){
	case 8:
		return Mat(camImg.height, camImg.width, CV_8UC1, &camImg.pixels[0]);
	case 24:
		return Mat(camImg.height, camImg.width, CV_8UC3, &camImg.pixels[0]);
	default :
		return Mat(camImg.height, camImg.width, CV_8UC3, &camImg.pixels[0]);
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

