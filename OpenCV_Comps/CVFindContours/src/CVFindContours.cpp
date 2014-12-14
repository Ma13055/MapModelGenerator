// -*- C++ -*-
/*!
 * @file  CVFindContours.cpp
 * @brief 与えられた二値画像に対して輪郭検出を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "CVFindContours.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

using namespace cv;
using namespace std;

//文字列の分割を行う
vector<string> split(string str, char delim);
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
//コンフィギュで設定されている値を基にfindContoursを行う
void makeContours( Mat &binImg, vector<vector<Point>> &cont
					, string mode, string method, Point offset );

// Module specification
// <rtc-template block="module_spec">
static const char* cvfindcontours_spec[] =
  {
    "implementation_id", "CVFindContours",
    "type_name",         "CVFindContours",
    "description",       "与えられた二値画像に対して輪郭検出を行う",
    "version",           "1.3.0",
    "vendor",            "Masaru Tatekawa(SIT)",
    "category",          "Image Processing",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.01_ImageView", "OFF",
    "conf.default.02_sendDataType", "NonExchange,Convex",
    "conf.default.03_FindContourMode", "LIST",
    "conf.default.04_FindContourMethod", "NONE",
    "conf.default.05_offset", "0,0",
    // Widget
    "conf.__widget__.01_ImageView", "radio",
    "conf.__widget__.02_sendDataType", "checkbox",
    "conf.__widget__.03_FindContourMode", "radio",
    "conf.__widget__.04_FindContourMethod", "radio",
    "conf.__widget__.05_offset", "checkbox",
    // Constraints
    "conf.__constraints__.01_ImageView", "(ON,OFF)",
    "conf.__constraints__.02_sendDataType", "(NonExchange,Rectangle,Convex)",
    "conf.__constraints__.03_FindContourMode", "(EXTERNAL,LIST,CCOMP,TREE)",
    "conf.__constraints__.04_FindContourMethod", "(NONE,SIMPLE,TC89_L1,TC89_KCOS,LINK_RUNS)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CVFindContours::CVFindContours(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_src_imgIn("srcImage", m_src_img),
    m_thre_imgIn("thresholdImage", m_thre_img),
    m_cont_dataOut("contoursData", m_cont_data),
    m_cont_rectOut("contoursRectangles", m_cont_rect),
    m_cont_convexOut("contoursConvex", m_cont_convex)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
CVFindContours::~CVFindContours()
{
}



RTC::ReturnCode_t CVFindContours::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("srcImage", m_src_imgIn);
  addInPort("thresholdImage", m_thre_imgIn);
  
  // Set OutPort buffer
  addOutPort("contoursData", m_cont_dataOut);
  addOutPort("contoursRectangles", m_cont_rectOut);
  addOutPort("contoursConvex", m_cont_convexOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_ImageView", m_img_view, "OFF");
  bindParameter("02_sendDataType", m_send_type, "NonExchange,Convex");
  bindParameter("03_FindContourMode", m_fc_mode, "LIST");
  bindParameter("04_FindContourMethod", m_fc_method, "NONE");
  bindParameter("05_offset", m_offset, "0,0");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVFindContours::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVFindContours::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVFindContours::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t CVFindContours::onActivated(RTC::UniqueId ec_id)
{
	cout<<"CVFindContours : onActivated : START"<<endl;
	/*--------------パラメータの初期化--------------*/
	//ポートの初期化
	while(m_src_imgIn.isNew()) m_src_imgIn.read();
	while(m_thre_imgIn.isNew()) m_thre_imgIn.read();
	m_cont_rect.data.length(0);
	m_cont_convex.data.length(0);

	/*--------コンフィギュの前回値を持つ変数-----------*/

	mode = m_fc_mode;
	method = m_fc_method;

	//FindContoursの基礎パラメータ
	offset.x = m_offset.at(0);
	offset.y = m_offset.at(1);

	/*--------その他変数-----------*/
	old_img = makeEmptyCamImg();
	thre_img = makeEmptyCamImg();
	gray_img = Mat();
	src_img = Mat();
	draw_img = Mat();
	remake = true;	//再検出フラグ
	cont_rect.clear();	//輪郭点を変換した長方形
	cont_convex.clear();	//輪郭点を変換した凸角形

	cout<<"CVFindContours : onActivated : END"<<endl;

  return RTC::RTC_OK;
}

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVFindContours::onDeactivated(RTC::UniqueId ec_id)
{
	destroyWindow("CVFindContours : receiveImage");
	destroyWindow("CVFindContours : contoursView");

  return RTC::RTC_OK;
}

/*!
 * 輪郭検出を行う
 */

RTC::ReturnCode_t CVFindContours::onExecute(RTC::UniqueId ec_id)
{
	//入力ポートdefaultImageのデータを読み込む
	if(m_src_imgIn.isNew()){
		m_src_imgIn.read();
	}

	//入力ポートthresholdImageのデータを読み込む
	if(m_thre_imgIn.isNew()){
		m_thre_imgIn.read();
	}

	//コンフィグのcheckboxに変更があった場合
	//出力形式選択配列の変更を行う
	bool repaint = false;
	if(checkbox != m_send_type){
		checkbox = m_send_type;

		for(int i = 0;i<3;i++){
			checkbox_flag[i] = false;
		}

		vector<string> vs_checkbox = split(checkbox,',');
		for(vector<string>::iterator it = vs_checkbox.begin(); it != vs_checkbox.end() ; it++){
			if(*it == "NonExchange")checkbox_flag[0] = true;
			else if(*it == "Rectangle")checkbox_flag[1] = true;
			else checkbox_flag[2] = true;
		}
		repaint = true;
	}

	//defaultImageかthresholdImageのデータがすでに受け取っているデータと違う場合
	bool make_src_img = equalCamImg(old_img,m_src_img);
	bool make_thre_img = equalCamImg(thre_img,m_thre_img);
	if( (!make_src_img || !make_thre_img) && 
		(old_img.pixels.length() != 0 || thre_img.pixels.length() != 0) ){

		remake = true;

		//threImageのCameraImageをMat型に変換し、
		//グレースケールでない場合はグレースケルへ変換する
		if(thre_img.pixels.length() != 0){
			cout<<"CVFindContours : makeThreImg"<<endl;

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
			cout<<"CVFindContours : makeSrcImg"<<endl;

			//CameraImage型からMat型へ変換
			src_img = CamToMat(old_img).clone();	
			
			//二値画像が受け取れていない場合
			if(gray_img.data == 0){
				cout<<"CVFindContours : makeThreImg"<<endl;

				//二値の変数に上書き
				cvtColor(src_img,gray_img,CV_RGB2GRAY);
				Canny( gray_img, gray_img, 50, 200, 3 );
			}
		}
	}

	if(gray_img.data != 0){
		//メソッドの更新
		if(!equalPara(method,m_fc_method))remake = true;
		//モードの更新
		if(!equalPara(mode,m_fc_mode))remake = true;

		//FindContoursの基礎パラメータの更新
		if(!equalPara(offset.x,m_offset.at(0)))remake = true;
		if(!equalPara(offset.y,m_offset.at(1)))remake = true;

		/*------------輪郭の再検出--------------*/
		if(remake == true){
			//輪郭抽出 contoursに保存
			vector<vector<Point> > contours_sub;
			contours.clear();

			makeContours( gray_img, contours_sub, mode, method, offset);

			///輪郭データに補正
			contours.resize(contours_sub.size());
			for( int i = 0; i < contours_sub.size(); i++ ){
				approxPolyDP( Mat(contours_sub[i]), contours[i], 3, true );
			}

			//輪郭点データ群を基に輪郭データ群を囲む長方形と凸図形を作成
			cont_rect.clear();
			cont_rect.resize( contours.size() );
			cont_convex.clear();
			cont_convex.resize( contours.size() );
			for(int i=0;i<contours.size();i++){
				cont_rect[i] = boundingRect( Mat(contours[i]) );
				convexHull( Mat(contours[i]), cont_convex[i], false );
			}
		}


		/*------------描画及びOurPortの更新--------------*/
		if(remake == true || repaint == true){

			//元画像を黒白のカラー画像にする
			if(src_img.elemSize() != 1)cvtColor(src_img,draw_img,CV_RGB2GRAY);
			cvtColor(draw_img,draw_img,CV_GRAY2RGB);

			int hull_counter = 0;
			int cont_counter = 0;

			//輪郭点データ群, 輪郭を囲む長方形, 輪郭を囲む凸図形を描画
			for( int i = 0; i< contours.size(); i++ ){
				//checkboxのNonExchangeがONの場合
				//輪郭点データ群を描画
				if(checkbox_flag[0]){
					drawContours( draw_img, contours, i, Scalar(255,0,0), 1, 8, vector<Vec4i>(), 0, Point() );
					cont_counter += contours[i].size()*2 + 1;
				}

				//checkboxのRectangleがONの場合
				//長方形データ群を描画
				if(checkbox_flag[1])rectangle( draw_img, cont_rect[i].tl(), cont_rect[i].br(),Scalar(0,0,255), 1, 8, 0 );

				//checkboxのConvexがONの場合
				//凸図形データ群を描画
				if(checkbox_flag[2]){
					drawContours( draw_img, cont_convex, i, Scalar(0,255,0), 1, 8, vector<Vec4i>(), 0, Point() );
					hull_counter += cont_convex[i].size()*2 + 1;
				}
			}

			//それぞれのoutPortの配列数を変更
			if(checkbox_flag[0])m_cont_data.data.length(cont_counter);
			if(checkbox_flag[1])m_cont_rect.data.length(cont_rect.size()*4);
			if(checkbox_flag[2])m_cont_convex.data.length(hull_counter);

			cont_counter = 0;
			hull_counter = 0;
				
			//OutPortに輪郭データを送信
			for(int i = 0; i< contours.size(); i++){
				//checkboxのNonExchangeがONの場合
				//輪郭点データ群をOutPortに書き込み
				if(checkbox_flag[0]){
					m_cont_data.data[cont_counter++] = contours[i].size()*2;
					for(int j = 0; j < contours[i].size(); j++){
						m_cont_data.data[cont_counter++] = contours[i][j].x;
						m_cont_data.data[cont_counter++] = contours[i][j].y;
					}
				}

				//checkboxのRectangleがONの場合
				//長方形データ群をOutPortに書き込み
				if(checkbox_flag[1]){
					m_cont_rect.data[i*4+0] = cont_rect[i].tl().x;
					m_cont_rect.data[i*4+1] = cont_rect[i].tl().y;
					m_cont_rect.data[i*4+2] = cont_rect[i].width;
					m_cont_rect.data[i*4+3] = cont_rect[i].height;
				}

				//checkboxのConvexがONの場合
				//凸図形データ群をOutPortに書き込み
				if(checkbox_flag[2]){
					m_cont_convex.data[hull_counter++] = cont_convex[i].size()*2;
					for(int j = 0; j < cont_convex[i].size(); j++){
						m_cont_convex.data[hull_counter++] = cont_convex[i][j].x;
						m_cont_convex.data[hull_counter++] = cont_convex[i][j].y;
					}
				}
			}

			//無変換の輪郭点データ群を送信
			if(checkbox_flag[0]){
				m_cont_data.data.length(cont_counter);
				setTimestamp(m_cont_data);
				m_cont_dataOut.write();
			}
			//長方形の輪郭点データ群を送信
			if(checkbox_flag[1]){
				m_cont_rect.data.length(cont_rect.size()*4);
				setTimestamp(m_cont_rect);
				m_cont_rectOut.write();
			}
			//凸図形の輪郭点データ群を送信
			if(checkbox_flag[2]){
				m_cont_convex.data.length(hull_counter);
				setTimestamp(m_cont_convex);
				m_cont_convexOut.write();
			}

			remake = false;
			repaint = false;
		}

		//処理画像をウィンドウに表示
		if(m_img_view == "ON"){
			if(src_img.data != 0){
				namedWindow("CVFindContours : receiveImage",1);
				imshow("CVFindContours : receiveImage", src_img);
			}
			if(draw_img.data != 0){
				namedWindow("CVFindContours : contoursView",1);
				imshow("CVFindContours : contoursView", draw_img);
			}
		}else{
			destroyWindow("CVFindContours : receiveImage");
			destroyWindow("CVFindContours : contoursView");
		}

	}

	waitKey(1);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVFindContours::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVFindContours::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t CVFindContours::onReset(RTC::UniqueId ec_id)
{
	destroyWindow("CVFindContours : receiveImage");
	destroyWindow("CVFindContours : contoursView");

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CVFindContours::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CVFindContours::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void CVFindContoursInit(RTC::Manager* manager)
  {
    coil::Properties profile(cvfindcontours_spec);
    manager->registerFactory(profile,
                             RTC::Create<CVFindContours>,
                             RTC::Delete<CVFindContours>);
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
			cout<<"CVFindContours : chImg"<<endl;
			return false;
	}else{
		return true;
	}
}

/**
 * コンフィギュで設定されている値を使ってfindContoursを行う
 *
 * @para Mat &grayImg					入力画像(８ビットシングルチャンネル）
 * @para vector<vector<Point>> &cont	抽出された輪郭を保存する領域
 * @para string mode					抽出モード 
 * @para string method					推定手法
 * @para Point offset					オフセット
 * @return void					
 */
void makeContours( Mat &grayImg, vector<vector<Point>> &cont, string mode, string method, Point offset ){
	cout<<"CVFindContours : makeContours : START"<<endl;

	Mat binImg = grayImg.clone();

	int fc_mode = CV_RETR_EXTERNAL;
	int fc_method = CV_CHAIN_APPROX_NONE;

	if(mode == "EXTERNAL")		;
	else if(mode == "LIST")		fc_mode = CV_RETR_LIST;
	else if(mode == "CCOMP")	fc_mode = CV_RETR_CCOMP;
	else						fc_mode = CV_RETR_TREE;

	if(method == "NONE")				;
	else if(method == "SIMPLE")			fc_method = CV_CHAIN_APPROX_SIMPLE;
	else if(method == "TC89_L1")		fc_method = CV_CHAIN_APPROX_TC89_L1;
	else if(method == "TC89_KCOS")		fc_method = CV_CHAIN_APPROX_TC89_KCOS;
	else if(fc_mode == CV_RETR_LIST)	fc_method = CV_LINK_RUNS;

	//  2値画像中の輪郭を見つける
	findContours(
		binImg,		//	入力画像(８ビットシングルチャンネル）
		cont,		//	抽出された輪郭を保存する領域
		fc_mode,	//	抽出モード 
		fc_method,	//	推定手法
		offset		//	オフセット
	);

	cout<<"CVFindContours : makeContours : END"<<endl;
	return;
}

/**
* 文字列の分割を行う
* @para const string &str	分割の対象となる文字列
* @para char delim			分割のサインとなる文字
* @return vector<string>	分割結果の文字列のvector
*/
vector<string> split(string str,char delim){
	vector<string> res;
	size_t current = 0, found;
	while((found = str.find_first_of(delim, current)) != string::npos){
		res.push_back(string(str, current, found - current));
		current = found + 1;
	}
	res.push_back(string(str, current, str.size() - current));
	return res;		//分割されない場合は受け取ったstrの情報を0分割して返す
}