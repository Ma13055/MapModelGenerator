// -*- C++ -*-
/*!
 * @file  mapsManager.cpp
 * @brief 地図画像から地図モデルへの変換を管理し、結果の保存を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "mapsManager.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <sstream>
#include <direct.h>
#include <Shlwapi.h> 

#pragma comment(lib, "shlwapi.lib")

using namespace cv;
using namespace std;

//CameraImage型のデータをMat型に変換する
Mat CamToMat(CameraImage &m_recImg);
//Mat型のデータをCameraImage型に変換する
CameraImage MatToCam(Mat &mat_img);
//CameraImage型のデータを初期化する
CameraImage makeEmptyCamImg();
//CameraImage型の比較を行う
bool equalCamImg(CameraImage src_img,CameraImage rec_img);
//画像の保存を行い、そのパスを返す
TimedString makepathString(String str,CameraImage src);
//与えられたTimedShortSeq型をvector<vector<Point>>型に変換する
void seqToVec(TimedShortSeq tss,vector<vector<Point> > &vvp);
//TimedShortSeq型のデータ二つを比較する
bool equalTimedShortSeq(TimedShortSeq &src,TimedShortSeq &rec);
//輪郭点データ群を画像に描画する
void drawContoursImage(vector<vector<Point> > cont,Mat &src_img);
//与えられた保存先にvector<vector<Point>>型のデータを書き出す
void writeLineMapData(string str,vector<vector<Point> > &vvp);
//現在日時を指す文字列を返す
string makeTimeString();

// Module specification
// <rtc-template block="module_spec">
static const char* mapsmanager_spec[] =
  {
    "implementation_id", "mapsManager",
    "type_name",         "mapsManager",
    "description",       "地図画像から地図モデルへの変換を管理し、結果の保存を行う",
    "version",           "1.3.0",
    "vendor",            "Masaru Tatekawa(SIT)",
    "category",          "Conversion",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.01_ImageView", "OFF",
    "conf.default.02_SelectPictNum", "0",
    "conf.default.03_SaveSpace", "C:\\tmp\\makeMapsModel\\maps\\linemaps",
    "conf.default.04_SkipNormalize", "NON",
    // Widget
    "conf.__widget__.01_ImageView", "radio",
    "conf.__widget__.02_SelectPictNum", "text",
    "conf.__widget__.03_SaveSpace", "text",
    "conf.__widget__.04_SkipNormalize", "radio",
    // Constraints
    "conf.__constraints__.01_ImageView", "(ON,OFF)",
    "conf.__constraints__.02_SelectPictNum", "x>=0",
    "conf.__constraints__.04_SkipNormalize", "(NON,SKIP)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
mapsManager::mapsManager(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_rec_cam_imgIn("receiveCamImg", m_rec_cam_img),
    m_rec_flagIn("receiveFlagData", m_rec_flag),
    m_In_temp_pathIn("tempFolderPath", m_In_temp_path),
    m_click_pointIn("clickPoint", m_click_point),
    m_map_normIn("compNormalizedMap", m_map_norm),
    m_map_lineIn("compLineMap", m_map_line),
    m_send_cam_imgOut("backCamImg", m_send_cam_img),
    m_out_temp_pathOut("tempFolderPathOut", m_out_temp_path),
    m_norm_src_imgOut("makeNormSrcImg", m_norm_src_img),
    m_norm_src_img_pathOut("makeNormSrcImgPath", m_norm_src_img_path),
    m_norm_comp_imgOut("compNormImg", m_norm_comp_img),
    m_norm_comp_img_pathOut("compNormImgPath", m_norm_comp_img_path),
    m_line_src_imgOut("makeLineSrcImg", m_line_src_img),
    m_line_src_img_pathOut("makeLineSrcImgPath", m_line_src_img_path),
    m_line_comp_imgOut("compLineImg", m_line_comp_img),
    m_line_comp_img_pathOut("compLineImgPath", m_line_comp_img_path),
    m_step_flagOut("stepFlag", m_step_flag),
    m_line_maps_pathOut("lineMapsPath", m_line_maps_path)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
mapsManager::~mapsManager()
{
}



RTC::ReturnCode_t mapsManager::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("receiveCamImg", m_rec_cam_imgIn);
  addInPort("receiveFlagData", m_rec_flagIn);
  addInPort("tempFolderPath", m_In_temp_pathIn);
  addInPort("clickPoint", m_click_pointIn);
  addInPort("compNormalizedMap", m_map_normIn);
  addInPort("compLineMap", m_map_lineIn);
  
  // Set OutPort buffer
  addOutPort("backCamImg", m_send_cam_imgOut);
  addOutPort("tempFolderPathOut", m_out_temp_pathOut);
  addOutPort("makeNormSrcImg", m_norm_src_imgOut);
  addOutPort("makeNormSrcImgPath", m_norm_src_img_pathOut);
  addOutPort("compNormImg", m_norm_comp_imgOut);
  addOutPort("compNormImgPath", m_norm_comp_img_pathOut);
  addOutPort("makeLineSrcImg", m_line_src_imgOut);
  addOutPort("makeLineSrcImgPath", m_line_src_img_pathOut);
  addOutPort("compLineImg", m_line_comp_imgOut);
  addOutPort("compLineImgPath", m_line_comp_img_pathOut);
  addOutPort("stepFlag", m_step_flagOut);
  addOutPort("lineMapsPath", m_line_maps_pathOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_ImageView", m_img_view, "OFF");
  bindParameter("02_SelectPictNum", m_select_pict_num, "0");
  bindParameter("03_SaveSpace", m_save_space, "C:\\tmp\\makeMapsModel\\maps\\linemaps");
  bindParameter("04_SkipNormalize", m_skip_norm, "NON");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t mapsManager::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t mapsManager::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t mapsManager::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t mapsManager::onActivated(RTC::UniqueId ec_id)
{
	cout<<"mapsManager : onActivate:START"<<endl;

	//temp領域のパスを読み込む
	if(m_In_temp_pathIn.isNew()){
		m_In_temp_pathIn.read();
		tempName = m_In_temp_path.data;
	}else{
		tempName = "C:\\tmp\\makeMapsModel";
	}
	m_out_temp_path.data = tempName.c_str();
	setTimestamp(m_out_temp_path);
	m_out_temp_pathOut.write();

	while(m_rec_cam_imgIn.isNew()) m_rec_cam_imgIn.read();
	while(m_rec_flagIn.isNew()) m_rec_flagIn.read();
	while(m_In_temp_pathIn.isNew()) m_In_temp_pathIn.read();
	while(m_click_pointIn.isNew()) m_click_pointIn.read();
	while(m_map_normIn.isNew()) m_map_normIn.read();
	while(m_map_lineIn.isNew()) m_map_lineIn.read();

	old_comp_img = makeEmptyCamImg();
	old_receive_img = makeEmptyCamImg();

	old_map_line = TimedShortSeq();

	string tempName;
	send_pict = 0;
	conf_pict_num = 0;

	getPictSeq.clear();
	compPictSeq.clear();
	compLineMapSeq.clear();
	map_line.clear();

	receiveEndFlag = false;
	normalized_flag = false;
	convert_line_map_flag = false;
	first_flag = true;
	remake = false;
	check_click_flag = false;

	cout<<"mapsManager : onActivate:END"<<endl;

  return RTC::RTC_OK;
}

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t mapsManager::onDeactivated(RTC::UniqueId ec_id)
{
	destroyWindow("rectImage");
  return RTC::RTC_OK;
}

/*!
 * 地図画像を地図モデルへ変化し保存する
 */

RTC::ReturnCode_t mapsManager::onExecute(RTC::UniqueId ec_id)
{
	//temp領域のパスを読み込む
	if(m_In_temp_pathIn.isNew()){
		m_In_temp_pathIn.read();
		tempName = m_In_temp_path.data;
	}

	//temp領域のパスをOutPortに送る
	m_out_temp_path.data = tempName.c_str();
	setTimestamp(m_out_temp_path);
	m_out_temp_pathOut.write();

	//画像受け渡しの送り側からのフラグ信号を更新
	if(m_rec_flagIn.isNew()){
		m_rec_flagIn.read();

		if(m_rec_flag.data == -2){
			//初期化を行う
			while(m_rec_cam_imgIn.isNew()) m_rec_cam_imgIn.read();
			while(m_rec_flagIn.isNew()) m_rec_flagIn.read();
			while(m_In_temp_pathIn.isNew()) m_In_temp_pathIn.read();
			while(m_map_normIn.isNew()) m_map_normIn.read();
			while(m_click_pointIn.isNew()) m_click_pointIn.read();
			while(m_map_lineIn.isNew()) m_map_lineIn.read();

			old_comp_img = makeEmptyCamImg();
			old_receive_img = makeEmptyCamImg();

			send_pict = 0;
			conf_pict_num = 0;
			m_rec_flag.data = 0;

			getPictSeq.clear();
			compPictSeq.clear();
			receiveEndFlag = false;
			normalized_flag = false;
			convert_line_map_flag = false;
			first_flag = true;
			check_click_flag = false;

			cout<<"mapsManager : Initialize"<<endl;

		}else if(m_rec_flag.data == -1 && receiveEndFlag == false){
			//受け取り画像が1枚以上ある時
			if(getPictSeq.size() > 0 || old_receive_img.pixels.length() != 0){
				//受け取り画像の最後の画像を変数に保管
				getPictSeq.push_back(old_receive_img);

				//後の処理結果を保存するvectorのサイズを受け取り画像数にresize
				compPictSeq.resize(getPictSeq.size());
				compLineMapSeq.resize(getPictSeq.size());
				compMapsPath.resize(getPictSeq.size());

				cout<<"mapsManager : END : "<<getPictSeq.size()<<endl;
				//受け渡し処理終了フラグを立てる
				receiveEndFlag = true;

				m_step_flag.data = 1;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();
			}
		}
	}



	//画像受け渡し終了フラグが立っているか確認
	//受け渡しが完了していない場合
	if(receiveEndFlag == false){

	/*--------------------------画像受け取り中処理----------------------------*/

		//画像受け取りポートの更新
		if(m_rec_cam_imgIn.isNew()){
			m_rec_cam_imgIn.read();

			//初めての受け取りの場合
			if(old_receive_img.pixels.length() == 0){
				//前回比較のための画像変数に保管
				old_receive_img = m_rec_cam_img;
			}else{
				//前回画像と今回受け取り画像とを比較
				if(!equalCamImg(old_receive_img,m_rec_cam_img)){
					//違う画像だった場合、前回画像を画像保管vectorに保管
					getPictSeq.push_back(old_receive_img);
					cout<<"mapsManager : RECEIVE : "<<getPictSeq.size()<<endl;
					//前回画像変数を更新
					old_receive_img = m_rec_cam_img;
				}
			}

			//フィードバックOutPortに今回受け取った画像を送る
			m_send_cam_img = old_receive_img;
			m_send_cam_imgOut.write();
		}

/*----------------------画像受けとり済み---------------------*/

	}else{

		//受け渡し終了かつ、受け渡しやり直し要求無しの場合
			
		//コンフィグのselectNumが更新されている場合はその値を受け取る
		if(conf_pict_num != m_select_pict_num && getPictSeq.size() > m_select_pict_num){
			conf_pict_num = m_select_pict_num;
			send_pict = m_select_pict_num;
		}

		//正規化処理のスキップが選択されているかの確認
		if(normalized_flag == false && m_skip_norm == "SKIP"){
			//受け取った画像を処理済み画像配列へコピー
			
			compPictSeq = getPictSeq;
			//座標マップ化への初期化を行う
			old_comp_img = makeEmptyCamImg();
			old_receive_img = makeEmptyCamImg();

			send_pict = 0;
			conf_pict_num = 0;

			normalized_flag = true;

			cout<<"mapsManager : normalize SKIP"<<endl;
		}

/*------------------------地図モデル生成のための、地図画像の正規化----------------------------*/

		if(normalized_flag == false){

			//処理する画像をOutPortに送る
			//対応する完成済みデータが存在する場合はその完成済み画像を、
			//対応する完成済みのデータがない場合は、「戻る」処理用のデータを作り、処理前の画像を送る
			if(compPictSeq.at(send_pict).pixels.length() != 0)
				m_norm_src_img = compPictSeq.at(send_pict);
			else{
				m_norm_src_img = getPictSeq.at(send_pict);
				old_comp_img = m_norm_src_img;
			}
			m_norm_src_imgOut.write();

			//処理する画像をtemp領域に保存
			//保存場所の絶対パスをOutPortにコピー
			m_norm_src_img_path = makepathString(tempName + "\\sub\\src_pict.jpg",m_norm_src_img);
			m_norm_src_img_pathOut.write();

			if(m_map_normIn.isNew()){
				m_map_normIn.read();

				//現在処理に送っている画像の処理後画像が受信された場合
				if(m_map_norm.tm.sec == m_norm_src_img.tm.sec && m_map_norm.tm.nsec == m_norm_src_img.tm.nsec){

					//クリック入力のバッファの初期化
					if(first_flag){
						while(m_click_pointIn.isNew())m_click_pointIn.read();
						first_flag = false;
					}

					//CameraImage型として保持
					CameraImage compImg = m_map_norm;

					//UIに処理後画像を送信
					m_norm_comp_img = compImg;
					m_norm_comp_imgOut.write();

					//処理する画像をtemp領域に保存
					//保存場所の絶対パスをOutPortにコピー
					m_norm_comp_img_path = makepathString(tempName + "\\sub\\comp_pict.jpg",compImg);
					m_norm_comp_img_pathOut.write();
					

					//UIからのクリックを受け取った場合
					if(m_click_pointIn.isNew()){
						m_click_pointIn.read();

						Point3D click_p = m_click_point.data;

						//処理完了を示すフラグ
						if(click_p.z == 7){

							ostringstream str_sub_img;
							string time_string = makeTimeString();

							//途中結果の画像を出力
							str_sub_img << tempName; str_sub_img << "\\sub\\NormalizeMapImage"; str_sub_img << send_pict;
							str_sub_img << "_" << time_string << ".jpg";
							makepathString(str_sub_img.str(),compImg);

							//処理後画像を保存するvectorにpush
							compPictSeq.at(send_pict) = compImg;
							//処理対象画像のインデックスをすすめる
							send_pict++;
							if(send_pict == getPictSeq.size())send_pict = 0;

							m_step_flag.data = 2;
							setTimestamp(m_step_flag);
							m_step_flagOut.write();

							cout<<"mapsManager : normalize : nextMap"<<endl;

						//再度処理を示すフラグ
						}else if(click_p.z == 6){

							setTimestamp(compImg);
							old_comp_img = compImg;
							compPictSeq.at(send_pict) = compImg;

							m_step_flag.data = 3;
							setTimestamp(m_step_flag);
							m_step_flagOut.write();

							cout<<"mapsManager : normalize : reprocessing"<<endl;

						//処理結果を消去するフラグ
						}else if(click_p.z == 3){

							setTimestamp(old_comp_img);
							compPictSeq.at(send_pict) = old_comp_img;


							m_step_flag.data = 4;
							setTimestamp(m_step_flag);
							m_step_flagOut.write();
							cout<<"mapsManager : normalize : clear"<<endl;

						//全画像に対する処理が終了したことを示すフラグ
						}else if(click_p.z == 8){

							compPictSeq.at(send_pict) = compImg;

							//全画像に対し処理が終了しているかどうかのチェック
							bool full_comp_flag = true;
							for(int i=0;i<compPictSeq.size();i++){
								if(compPictSeq[i].pixels.length() == 0)full_comp_flag = false;
							}

							//全画像の処理が終了している場合
							if(full_comp_flag == true){
								//座標マップ化への初期化を行う
								old_comp_img = makeEmptyCamImg();
								old_receive_img = makeEmptyCamImg();

								send_pict = 0;
								conf_pict_num = 0;

							
								m_step_flag.data = 5;
								setTimestamp(m_step_flag);
								m_step_flagOut.write();

								normalized_flag = true;

								cout<<"mapsManager : normalize : Comp"<<endl;

							}else{

								m_step_flag.data = 6;
								setTimestamp(m_step_flag);
								m_step_flagOut.write();

								cout<<"mapsManager : Non Full Comp"<<endl;
							}
						}
						first_flag = true;

					}
				}
			}

			
		}else if(normalized_flag == true && convert_line_map_flag == false){

/*----------------------正規化された地図を座標ベースの地図に変換------------------------*/


			//処理する画像をOutPortに送る
			m_line_src_img = compPictSeq.at(send_pict);
			m_line_src_imgOut.write();

			//処理する画像をtemp領域に保存
			//保存場所の絶対パスをOutPortにコピー
			m_line_src_img_path = makepathString(tempName + "\\sub\\mapManager_linesrc_pict.jpg",m_line_src_img);
			m_line_src_img_pathOut.write();


			bool through_flag = false;
			if(compLineMapSeq.at(send_pict).empty() == false) through_flag = true;

			if(m_map_lineIn.isNew()){
				m_map_lineIn.read();

				//処理結果が更新された場合
				if(!equalTimedShortSeq(old_map_line,m_map_line)){
					cout<<"mapsManager : changeLine"<<endl;

					seqToVec(old_map_line,map_line);
				}

				//現在処理に送っている画像の処理結果かどうかを調べる
				if((m_line_src_img.tm.sec == m_map_line.tm.sec &&
					m_line_src_img.tm.nsec == m_map_line.tm.nsec) || through_flag){

					//バッファに残っている情報を削除
					if(first_flag == true){
						while(m_click_pointIn.isNew())m_click_pointIn.read();
						first_flag = false;
					}
						
					Mat pre_img(CamToMat(compPictSeq.at(send_pict)).size(), CV_8UC3, Scalar::all(255));
					if(through_flag){
						//ラインマップ情報を描画する
						drawContoursImage(compLineMapSeq.at(send_pict),pre_img);
					}else{
						//ラインマップ情報を描画する
						drawContoursImage(map_line,pre_img);
					}

					//CameraImage型に変換
					CameraImage compImg = MatToCam(pre_img);

					//UIに処理後画像を送信
					m_line_comp_img = compImg;
					m_line_comp_imgOut.write();

					//処理する画像をtemp領域に保存
					//保存場所の絶対パスをOutPortにコピー
					m_line_comp_img_path = makepathString(tempName + "\\sub\\comp_pict.jpg",compImg);
					m_line_comp_img_pathOut.write();

					if(!through_flag){

						ostringstream str_sub_line;
						ostringstream str_sub_img;
						string time_string = makeTimeString();

						//ラインマップの書き出し先を作成
						str_sub_line << tempName; str_sub_line << "\\sub\\LineMapCordinate"; str_sub_line << send_pict;
						str_sub_line << "_" << time_string;
						str_sub_img << tempName; str_sub_img << "\\sub\\LineMapImage"; str_sub_img << send_pict;
						str_sub_img << "_" << time_string << ".jpg";
						//結果のラインマップのvector<vector<Point>>を保存先に書き出し
						writeLineMapData(str_sub_line.str(),map_line);
						compMapsPath.at(send_pict) = str_sub_line.str();
						//結果の描画画像を保存先に書き出し
						makepathString(str_sub_img.str(),compImg);
					}

					check_click_flag = true;
				}

			}

			if(check_click_flag || through_flag){
				//UIからのクリックを受け取った場合
				if(m_click_pointIn.isNew()){
					m_click_pointIn.read();

					Point3D click_p = m_click_point.data;

					Mat pre_img(CamToMat(compPictSeq.at(send_pict)).size(), CV_8UC3, Scalar::all(255));
					if(through_flag){
						//ラインマップ情報を描画する
						drawContoursImage(compLineMapSeq.at(send_pict),pre_img);
					}else{
						//ラインマップ情報を描画する
						drawContoursImage(map_line,pre_img);
					}

					//CameraImage型に変換
					CameraImage compImg = MatToCam(pre_img);


					//処理完了を示すフラグ
					if(click_p.z == 7){

						//処理後ラインマップを保存するvectorにpush
						compLineMapSeq.at(send_pict) = map_line;

						ostringstream str_line;
						ostringstream str_img;
						if( PathFileExists( m_save_space.c_str() )){
							//ラインマップの書き出し先を作成
							str_line << m_save_space; str_line << "\\LineMapCordinate"; str_line << send_pict;
							str_img << m_save_space; str_img << "\\LineMapImage"; str_img << send_pict; str_img << ".jpg";

						}else{
							//ラインマップの書き出し先を作成
							str_line << tempName; str_line << "\\sub\\LineMapCordinate"; str_line << send_pict;
							str_img << tempName; str_img << "\\sub\\LineMapImage"; str_img << send_pict; str_img<<".jpg";

						}
						//結果のラインマップのvector<vector<Point>>を保存先に書き出し
						writeLineMapData(str_line.str(),map_line);
						compMapsPath.at(send_pict) = str_line.str();
						//結果の描画画像を保存先に書き出し
						makepathString(str_img.str(),compImg);

						//処理対象画像のインデックスをすすめる
						send_pict = (send_pict + 1)%getPictSeq.size();

						m_step_flag.data = 2;
						setTimestamp(m_step_flag);
						m_step_flagOut.write();


						first_flag=true;

						check_click_flag = false;

					//再度処理を示すフラグ
					}else if(click_p.z == 6){
						compLineMapSeq.at(send_pict).clear();
						setTimestamp(compPictSeq.at(send_pict));
						first_flag=true;
						check_click_flag = false;
						m_step_flag.data = 3;
						setTimestamp(m_step_flag);
						m_step_flagOut.write();

					}else if(click_p.z == 3){
						setTimestamp(compPictSeq.at(send_pict));
						first_flag=true;
						check_click_flag = false;
						m_step_flag.data = 4;
						setTimestamp(m_step_flag);
						m_step_flagOut.write();

					}else if(click_p.z == 8){
						compLineMapSeq.at(send_pict) = map_line;

						ostringstream str_line;
						ostringstream str_img;
						if( PathFileExists( m_save_space.c_str() )){
							//ラインマップの書き出し先を作成
							str_line << m_save_space; str_line << "\\LineMapCordinate"; str_line << send_pict;
							str_img << m_save_space; str_img << "\\LineMapImage"; str_img << send_pict; str_img << ".jpg";

						}else{
							//ラインマップの書き出し先を作成
							str_line << tempName; str_line << "\\sub\\LineMapCordinate"; str_line << send_pict;
							str_img << tempName; str_img << "\\sub\\LineMapImage"; str_img << send_pict; str_img << ".jpg";
						}
						//結果のラインマップのvector<vector<Point>>を保存先に書き出し
						writeLineMapData(str_line.str(),map_line);
						compMapsPath.at(send_pict) = str_line.str();
						//結果の描画画像を保存先に書き出し
						imwrite(str_img.str(),CamToMat(compImg));

						//全地図に対してラインマップの生成が完了しているかどうかの確認
						bool full_comp_flag = true;
						for(int i=0;i<compLineMapSeq.size();i++){
							if(compLineMapSeq[i].size() == 0)full_comp_flag = false;
						}
						if(full_comp_flag){
							convert_line_map_flag = true;
							m_step_flag.data = 7;
							setTimestamp(m_step_flag);
							m_step_flagOut.write();
						}else{
							cout<<"mapsManager : Non Comp LineMaps"<<endl;
							m_step_flag.data = 6;
							setTimestamp(m_step_flag);
							m_step_flagOut.write();
						}
						check_click_flag = false;
					}
				}
			}

		}else if(convert_line_map_flag == true){
			ostringstream out_path;
			for(vector<string>::iterator it = compMapsPath.begin(); it != compMapsPath.end();++it){
				out_path << *it;
				if(it+1 != compMapsPath.end())out_path << ",";
			}

			m_line_maps_path.data = out_path.str().c_str();
			setTimestamp(m_line_maps_path);
			m_line_maps_pathOut.write();

		}
	}

	if(m_img_view == "ON"){
		namedWindow("rectImage",1);
		imshow("rectImage",CamToMat(old_receive_img));
	}else{
		destroyWindow("rectImage");
	}

	waitKey(1);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t mapsManager::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t mapsManager::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t mapsManager::onReset(RTC::UniqueId ec_id)
{
	destroyWindow("rectImage");
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t mapsManager::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t mapsManager::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void mapsManagerInit(RTC::Manager* manager)
  {
    coil::Properties profile(mapsmanager_spec);
    manager->registerFactory(profile,
                             RTC::Create<mapsManager>,
                             RTC::Delete<mapsManager>);
  }
  
};


/**
 * CameraImage型のデータをMat型に変換する
 *
 * @para CameraImage &camImg	変換の元となる画像データ
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
 * @return bool					等しい場合はtrueを返す
 */
bool equalCamImg(CameraImage src_img,CameraImage rec_img){
	if(		src_img.tm.sec		!=	rec_img.tm.sec	||
			src_img.tm.nsec		!=	rec_img.tm.nsec	||
			src_img.bpp			!=	rec_img.bpp		||
			src_img.height		!=	rec_img.height	||
			src_img.width		!=	rec_img.width	||
		src_img.pixels.length() !=	rec_img.pixels.length()){
			return false;
	}else{
		return true;
	}
}

/**
 * 画像の保存を行いそのパスを返す
 *
 * @para String str			保存場所
 * @para CameraImage src	保存対象画像
 * @return TimedString		保存データへのパス
 */
TimedString makepathString(String str,CameraImage src){
	TimedString ts;

	setTimestamp(ts);

	//処理する画像をtemp領域に保存
	imwrite(str,CamToMat(src));

	//保存場所の絶対パスをTimedStringにコピー
	ts.data = str.c_str();

	return ts;
}

/**
 * 与えられたTimedShortSeq型をvector<vector<Point>>型に変換する
 *
 * @para TimedShortSeq tss				変換元となるTimedShortSeq型
 * @para vector<vector<Point> > &vvp	変換後のvector<vector<Point> >型
 * @return void
 */
void seqToVec(TimedShortSeq tss,vector<vector<Point> > &vvp){
	//vectorを初期化
	vvp.clear();

	//受け取ったTimedShortSeqをvector<vector<Point>>型に変換
	int i = 0;
	//Seqを網羅するwhile文
	while(i<tss.data.length()){
		//vector<vector<Point>>にpushするvector<Point>を定義
		vector<Point> vp;
		//宣言されている輪郭点数分for文を回してvector<Point>を作成
		for(int j=0;j<tss.data[i];j+=2){
			Point p = Point(tss.data[i+j+1],
							tss.data[i+j+2]);
			vp.push_back(p);
		}
		//作成したvector<Point>をvector<vector<Point>>にpush
		vvp.push_back(vp);
		//次の輪郭群へindexを進める
		i+=tss.data[i]+1;
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

/**
 * 輪郭点データ群を画像に描画する
 *
 * @para vector<vector<Point> > cont	輪郭点データ群
 * @para Mat& src_img					描画対象画像
 * @return void					
 */
void drawContoursImage(vector<vector<Point> > cont,Mat &src_img){
		
	Scalar color = Scalar(0,0,0);	//グレースケールに輪郭を囲む長方形を描画するためのカラー
	//輪郭を囲む凸図形を描画
	for( int i = 0; i< cont.size(); i++ ){
		drawContours( src_img, cont, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
	}
}

/**
 * 与えられた保存先にvector<vector<Point>>型のデータを書き出す
 * 
 * @para string str							保存先
 * @para vector<vector<Point> > &map_line	保存対象の変数
 * @return void
 */
void writeLineMapData(string str,vector<vector<Point> > &vvp){
	// ファイル出力ストリームの初期化
	ofstream ofs(str);

	//vector<vector<Point>>の全要素網羅
	for(vector<vector<Point>>::iterator vvp_it = vvp.begin();vvp_it != vvp.end();++vvp_it){
		vector<Point> vp = *vvp_it;
		ostringstream vp_str;
		//vector<Point>の全要素網羅
		for(vector<Point>::iterator vp_it = vp.begin(); vp_it != vp.end() ; ++vp_it){
			Point p = *vp_it;
			//書き出す一行を作成
			vp_str << p.x; vp_str << ","; vp_str << p.y;
			if(vp_it + 1 != vp.end())vp_str << ",";
		}
		//vector<Point>の全要素で一行としてファイルに書き出し
		ofs << vp_str.str() << endl;
	}
}

/**
 * 現在日時を指す文字列を返す
 * 
 * @return string	年_月_日_時_分_秒の文字列を返す
 */
string makeTimeString(){
	ostringstream time_line;

	struct tm *date;
	time_t now;
	int year,month,day,hour,minute,second;
	time(&now);
	date = localtime(&now);
	year = date -> tm_year + 1990;
	month = date ->tm_mon + 1;
	day = date -> tm_mday;
	hour = date -> tm_hour;
	minute = date -> tm_min;
	second = date -> tm_sec;

	time_line << year << "_" << month << "_" << day << "_" << hour << "_" << minute << "_" << second;

	return time_line.str();
}