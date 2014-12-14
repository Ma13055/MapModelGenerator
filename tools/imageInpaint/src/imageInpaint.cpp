// -*- C++ -*-
/*!
 * @file  imageInpaint.cpp
 * @brief 輪郭情報を用いた画像修復を行う
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "imageInpaint.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <sstream>

using namespace cv;
using namespace std;

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
//輪郭点データ群を画像に描画する
void drawContoursImage(vector<Rect> cont,Mat& src_img);
void drawContoursImage(vector<vector<Point> > cont,Mat& src_img);
//与えられた座標が指す輪郭点データを削除する
bool eraceClickRect(int img_size,vector<Rect> &cont,Point3D p);
bool eraceClickRect(Mat img,vector<vector<Point>> &cont,Point3D p);
//ドラッグによって形成された長方形の中に存在する輪郭点データを削除する
void eraceDraggedRect(vector<Rect> &cont_rect,Rect dragged_rect);
void eraceDraggedRect(vector<vector<Point>> &cont,Rect dragged_rect);
//画像の保存を行い、そのパスを返す
TimedString makepathString(String str,CameraImage src);

// Module specification
// <rtc-template block="module_spec">
static const char* imageinpaint_spec[] =
  {
    "implementation_id", "imageInpaint",
    "type_name",         "imageInpaint",
    "description",       "輪郭情報を用いた画像修復を行う",
    "version",           "1.3.0",
    "vendor",            "Masaru Tatekawa(SIT)",
    "category",          "UI",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.01_contoursType", "Convex",
    "conf.default.02_MaskDataSelect", "Image",
    "conf.default.03_MaskTypeSelect", "All",
    // Widget
    "conf.__widget__.01_contoursType", "radio",
    "conf.__widget__.02_MaskDataSelect", "radio",
    "conf.__widget__.03_MaskTypeSelect", "radio",
    // Constraints
    "conf.__constraints__.01_contoursType", "(Rectangle,Convex)",
    "conf.__constraints__.02_MaskDataSelect", "(Image,Rectangle)",
    "conf.__constraints__.03_MaskTypeSelect", "(All,Single)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
imageInpaint::imageInpaint(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_temp_pathIn("tempFolderPath", m_temp_path),
    m_src_imgIn("srcImage", m_src_img),
    m_cont_rectIn("contoursRectangles", m_cont_rect),
    m_cont_convexIn("contoursConvex", m_cont_convex),
    m_click_pointIn("clickPoint", m_click_point),
    m_drag_rectIn("draggedRect", m_drag_rect),
    m_inpt_imgIn("inpaintImage", m_inpt_img),
    m_modi_imgOut("modifyImage", m_modi_img),
    m_modi_img_pathOut("modifyImagePath", m_modi_img_path),
    m_inpt_src_imgOut("inpaintSrcImage", m_inpt_src_img),
    m_inpt_mask_imgOut("inpaintMaskImage", m_inpt_mask_img),
    m_mask_areaOut("inpaintMaskArea", m_mask_area),
    m_proc_imgOut("processedImage", m_proc_img),
    m_step_flagOut("stepFlag", m_step_flag)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
imageInpaint::~imageInpaint()
{
}



RTC::ReturnCode_t imageInpaint::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("tempFolderPath", m_temp_pathIn);
  addInPort("srcImage", m_src_imgIn);
  addInPort("contoursRectangles", m_cont_rectIn);
  addInPort("contoursConvex", m_cont_convexIn);
  addInPort("clickPoint", m_click_pointIn);
  addInPort("draggedRect", m_drag_rectIn);
  addInPort("inpaintImage", m_inpt_imgIn);
  
  // Set OutPort buffer
  addOutPort("modifyImage", m_modi_imgOut);
  addOutPort("modifyImagePath", m_modi_img_pathOut);
  addOutPort("stepFlag", m_step_flagOut);
  addOutPort("inpaintSrcImage", m_inpt_src_imgOut);
  addOutPort("inpaintMaskImage", m_inpt_mask_imgOut);
  addOutPort("inpaintMaskArea", m_mask_areaOut);
  addOutPort("processedImage", m_proc_imgOut);

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("01_contoursType", m_cont_type, "Convex");
  bindParameter("02_MaskDataSelect", m_mask_select, "Image");
  bindParameter("03_MaskTypeSelect", m_mask_type, "All");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t imageInpaint::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t imageInpaint::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t imageInpaint::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t imageInpaint::onActivated(RTC::UniqueId ec_id)
{
	cout<<"imageInpaint : onActivated : START"<<endl;
	//temp領域のパスを読み込む
	if(m_temp_pathIn.isNew()){
		m_temp_pathIn.read();
		tempName = m_temp_path.data;
	}else{
		tempName = "C:\\tmp\\makeMapsModel";
	}

	/*--------------パラメータの初期化--------------*/
	//ポートの初期化
	while(m_src_imgIn.isNew()) m_src_imgIn.read();
	while(m_cont_rectIn.isNew()) m_cont_rectIn.read();
	while(m_cont_convexIn.isNew()) m_cont_convexIn.read();
	while(m_temp_pathIn.isNew()) m_temp_pathIn.read();
	while(m_click_pointIn.isNew()) m_click_pointIn.read();
	while(m_drag_rectIn.isNew()) m_drag_rectIn.read();
	while(m_inpt_imgIn.isNew()) m_inpt_imgIn.read();

	old_cont_rect = TimedShortSeq();
	old_cont_convex = TimedShortSeq();

	cont_rect.clear();
	reset_cont_rect.clear();
	last_cont_rect.clear();

	cont_convex.clear();
	reset_cont_convex.clear();
	last_cont_convex.clear();

	src_cam_img = makeEmptyCamImg();
	old_img = makeEmptyCamImg();
	inpt_cam_img = makeEmptyCamImg();
	old_inpt_img = makeEmptyCamImg();
	reset_inpt_img = makeEmptyCamImg();

	gray_img = Mat();
	gray_color_img = Mat();
	src_img = Mat();
	pre_img = Mat();

	remake = false;
	contours_make_flag = false;
	contours_modify_flag = false;
	inpaint_end_flag = false;
	inpaint_modify_flag = false;
	write_data_flag = false;

	inpt_it_rect = cont_rect.begin();
	inpt_it_convex = cont_convex.begin();

	cout<<"imageInpaint : onActivated : END"<<endl;

  return RTC::RTC_OK;
}

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t imageInpaint::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*!
 * 画像の輪郭情報を用いての画像修復を行う
 */

RTC::ReturnCode_t imageInpaint::onExecute(RTC::UniqueId ec_id)
{
/*-----------------入力ポート情報の読み込み---------------------*/
	//temp領域のパスを読み込む
	if(m_temp_pathIn.isNew()){
		m_temp_pathIn.read();
		tempName = m_temp_path.data;
	}

	//入力ポートを更新
	if(m_src_imgIn.isNew()){
		m_src_imgIn.read();

		//すでに受け取っている画像と違う画像な場合
		if(!equalCamImg(old_img,m_src_img)){
			cout<<"imageInpaint : changeImg"<<endl;

			src_cam_img = old_img;

			//NewされたCameraImageをMat型に変換
			src_img = CamToMat(m_src_img);	//受信した画像

			inpt_cam_img = old_img;

			//受け取った画像をグレースケールに変換
			if(src_img.elemSize() != 1)cvtColor(src_img,gray_img,CV_RGB2GRAY);
			else gray_img = src_img.clone();

			cvtColor(gray_img,gray_color_img,CV_GRAY2RGB);

		}
	}

	//入力ポートを更新
	if(m_cont_rectIn.isNew()){
		m_cont_rectIn.read();

		//長方形データ群が更新された場合
		if(!equalTimedShortSeq(old_cont_rect,m_cont_rect)){
			cout<<"imageInpaint : changeRect"<<endl;

			//保持してあった長方形データを削除
			cont_rect.clear();
			//受け取った領域データをrect型に変換
			for(int i=0;i<old_cont_rect.data.length();i+=4){
				Rect rect = Rect(old_cont_rect.data[i+0],old_cont_rect.data[i+1],
								 old_cont_rect.data[i+2],old_cont_rect.data[i+3]);
				cont_rect.push_back(rect);
			}
			remake = true;
		}
	}

	//入力ポートの更新
	if(m_cont_convexIn.isNew()){
		m_cont_convexIn.read();

		//凸図形データ群が更新された場合
		if(!equalTimedShortSeq(old_cont_convex,m_cont_convex)){
			cout<<"imageInpaint : changeConvex"<<endl;
			//保持してあった凸図形データを削除
			cont_convex.clear();
			//受け取ったTimedShortSeqをvector<vector<Point>>型に変換
			int i = 0;
			//Seqを網羅するwhile文
			while(i<old_cont_convex.data.length()){
				//vector<vector<Point>>にpushするvector<Point>を定義
				vector<Point> convex;
				//宣言されている輪郭点数分for文を回してvector<Point>を作成
				for(int j=0;j<old_cont_convex.data[i];j+=2){
					Point p = Point(old_cont_convex.data[i+j+1],
									old_cont_convex.data[i+j+2]);
					convex.push_back(p);
				}
				//作成したvector<Point>をvector<vector<Point>>にpush
				cont_convex.push_back(convex);
				//次の輪郭群へindexを進める
				i+=old_cont_convex.data[i]+1;
			}
			remake = true;
		}
	}

/*- - - - - - - - - - - FindContoursに対するユーザの修正 - - - - - - - - - - -*/

	if(gray_color_img.data != 0 && remake == true){

		old_inpt_img = makeEmptyCamImg();
		reset_inpt_img = makeEmptyCamImg();

		pre_img = Mat();

		Mat src_gray_copy_img = gray_color_img.clone();

		if(m_cont_type == "Rectangle"){

			//前回情報・リセット情報に用いるためにvectorを輪郭データをコピー
			reset_cont_rect = cont_rect;
			last_cont_rect = cont_rect;
			//元画像のグレー画像に長方形輪郭データを描画
			drawContoursImage(cont_rect,src_gray_copy_img);
			inpt_it_rect = cont_rect.begin();

		}else{

			//前回情報・リセット情報に用いるためにvectorを輪郭データをコピー
			reset_cont_convex = cont_convex;
			last_cont_convex = cont_convex;
			//元画像のグレー画像に凸図形輪郭データを描画
			drawContoursImage(cont_convex,src_gray_copy_img);
			inpt_it_convex = cont_convex.begin();

		}

		//処理する画像をtemp領域に保存
		//保存場所の絶対パスをOutPortにコピー
		m_modi_img_path = makepathString(tempName + "\\sub\\contours_draw_img.jpg",MatToCam(src_gray_copy_img));
		m_modi_img_pathOut.write();

 
		//輪郭描画後の画像をOutPortにコピー
		m_modi_img = MatToCam(src_gray_copy_img);
		m_modi_imgOut.write();

		//輪郭データの抽出が終了したフラグを立てる
		remake = false;
		first_flag = true;
		contours_make_flag = true;
		contours_modify_flag = false;
		inpaint_end_flag = false;
		inpaint_modify_flag = false;
		write_data_flag = false;

		m_step_flag.data = 11;
		setTimestamp(m_step_flag);
		m_step_flagOut.write();
	}

	//輪郭データ抽出が完了し、ユーザによる修正が終わっていない場合
	if(contours_make_flag == true && contours_modify_flag == false){
		
		//クリック入力のバッファの初期化
		if(first_flag == true){
			while(m_click_pointIn.isNew()) m_click_pointIn.read();
			while(m_drag_rectIn.isNew()) m_drag_rectIn.read();
			cout<<"imageInapint : clear Buf"<<endl;
			first_flag = false;

		}

		//ユーザのクリック情報をNew
		if(m_click_pointIn.isNew()){
			m_click_pointIn.read();

			//ユーザからの処理フラグ情報(z軸データ)による場合分け
			switch((int)m_click_point.data.z){
			
			//クリック処理を表すフラグ
			case 1:
				//前回ループ輪郭データを保持
				if(m_cont_type == "Rectangle"){
					//長方形輪郭データを保持
					last_cont_rect = cont_rect;
					//クリックされた場所の輪郭データを取り除く
					eraceClickRect(Rect(0,0,gray_img.cols,gray_img.rows).area(),cont_rect,m_click_point.data);
				}else{
					//凸図形輪郭データを保持
					last_cont_convex = cont_convex;
					//クリックされた場所の輪郭データを取り除く
					eraceClickRect(gray_img,cont_convex,m_click_point.data);
				}


				break;

			//リセットデータの更新が押されたことを表すフラグ
			case 9:
				//現在の輪郭データ群を、未処理輪郭データ群に上書き
				if(m_cont_type == "Rectangle"){
					reset_cont_rect = cont_rect;
				}else{
					reset_cont_convex = cont_convex;
				}

				m_step_flag.data = 8;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();

				break;

			//戻るボタンが押されたことを表すフラグ
			case 3:
				//前回ループの輪郭データ群を、現在の輪郭データ群に上書き
				if(m_cont_type == "Rectangle"){
					cont_rect = last_cont_rect;
				}else{
					cont_convex = last_cont_convex;
				}

				break;

			//リセットボタンが押されたことを表すフラグ
			case 4:
				//未処理輪郭データ群を、現在の輪郭データ群に上書き
				if(m_cont_type == "Rectangle"){
					cont_rect = reset_cont_rect;
				}else{
					cont_convex = reset_cont_convex;
				}

				m_step_flag.data = 9;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();

				break;

			//修正処理完了を示すフラグ
			case 5:
				//輪郭修正が終了したフラグを立てる
				contours_modify_flag = true;
				write_data_flag = true;

				if(m_cont_type == "Rectangle"){
					if(cont_rect.size() != 0){
						//輪郭データ群のvectorの先頭を得る
						inpt_it_rect = cont_rect.begin();
					}else{
						//輪郭データがない場合はinpaintの自動処理を飛ばす
						inpaint_end_flag = true;
					}
				}else{
					if(cont_convex.size() != 0){
						//輪郭データ群のvectorの先頭を得る
						inpt_it_convex = cont_convex.begin();
					}else{
						//輪郭データがない場合はinpaintの自動処理を飛ばす
						inpaint_end_flag = true;
					}
				}

				m_step_flag.data = 10;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();
				cout<<"imageInpaint : ModifyFIN"<<endl;

				break;

			default :
				break;
			}

			//ドラッグ処理を受け付けるInPortをNew
			if(m_drag_rectIn.isNew()){
				//Portの読み込み
				m_drag_rectIn.read();

				if(m_cont_type == "Rectangle"){
					//前回ループ輪郭データを保持
					last_cont_rect = cont_rect;

					//ドラッグされた領域に含まれる輪郭データ群を削除する
					eraceDraggedRect(cont_rect,Rect(m_drag_rect.data[0],m_drag_rect.data[1],m_drag_rect.data[2],m_drag_rect.data[3]));
				}else{
					//凸図形輪郭データを保持
					last_cont_convex = cont_convex;

					//ドラッグされた領域に含まれる輪郭データ群を削除する
					eraceDraggedRect(cont_convex,Rect(m_drag_rect.data[0],m_drag_rect.data[1],m_drag_rect.data[2],m_drag_rect.data[3]));
				}
			}

			//変更された輪郭データ群に対して再描画を行う
			Mat src_gray_copy_img = gray_color_img.clone();
			if(m_cont_type == "Rectangle"){
				//元画像のグレー画像に長方形輪郭データを描画
				drawContoursImage(cont_rect,src_gray_copy_img);
			}else{
				//元画像のグレー画像に凸図形輪郭データを描画
				drawContoursImage(cont_convex,src_gray_copy_img);
			}

			//処理する画像をtemp領域に保存
			//保存場所の絶対パスをOutPortにコピー
			m_modi_img_path = makepathString(tempName + "\\sub\\contours_draw_img.jpg",MatToCam(src_gray_copy_img));
			m_modi_img_pathOut.write();
 
			//輪郭描画後の画像をOutPortにコピー
			m_modi_img = MatToCam(src_gray_copy_img);
			m_modi_imgOut.write();
		}
	}

/*-----------------------Contoursデータに対するInpaint処理-----------------------*/

	//輪郭点データ群の修正が終了し、修復処理が終了していない場合
	if(contours_modify_flag == true && inpaint_end_flag == false){

		//修復したい画像と修復領域をOutPortに送る
		if(write_data_flag == true){
			cout<<"imageInpaint : SendInpaint"<<endl;

			//修復基画像の作成
			setTimestamp(inpt_cam_img);
			m_inpt_src_img = inpt_cam_img;
			m_inpt_src_imgOut.write();

			//修復処理に用いるマスク領域の指定
			if(m_cont_type == "Rectangle"){
				if(m_mask_select == "Image"){
					//修復対象画像と同じ大きさの黒画像を作成
					Mat mask_mat_img = Mat::zeros(src_img.size(), CV_8U);

					while(inpt_it_rect < cont_rect.end()){
						Rect mask_rect = *inpt_it_rect;
						//受け取った領域を黒画像中に白く描画
						rectangle( mask_mat_img, mask_rect.tl(), mask_rect.br(), Scalar::all(255), CV_FILLED, 8, 0 );
						if(m_mask_type == "All")inpt_it_rect++;
						else break;
					}

					//OutPortにmaskイメージを送る
					m_inpt_mask_img = MatToCam(mask_mat_img);
					m_inpt_mask_img.tm = inpt_cam_img.tm;
					m_inpt_mask_imgOut.write();
				}else{
					//OutPortにrectangle情報を送る
					Rect mask_rect = *inpt_it_rect;
					m_mask_area.tm = inpt_cam_img.tm;
					m_mask_area.data.length(4);
					m_mask_area.data[0] = mask_rect.x;
					m_mask_area.data[1] = mask_rect.y;
					m_mask_area.data[2] = mask_rect.width;
					m_mask_area.data[3] = mask_rect.height;
					m_mask_areaOut.write();
				}

			}else{
				//修復対象画像と同じ大きさの黒画像を作成
				Mat mask_mat_img = Mat::zeros(src_img.size(), CV_8U);

				while(inpt_it_convex < cont_convex.end()){
					vector<Point> mask_convex = *inpt_it_convex;
					//受け取った領域を黒画像中に白く描画
					fillConvexPoly(mask_mat_img, &mask_convex[0], mask_convex.size(), Scalar::all(255));
					if(m_mask_type == "All")inpt_it_convex++;
					else break;
				}

				//OutPortにmaskイメージを送る
				m_inpt_mask_img = MatToCam(mask_mat_img);
				m_inpt_mask_img.tm = inpt_cam_img.tm;
				m_inpt_mask_imgOut.write();
			}

			//OutPortに送ったことを示すフラグを立てる
			write_data_flag = false;

		//OutPortに必要情報を送っている場合
		} else {
			
			if(m_inpt_imgIn.isNew()){
				m_inpt_imgIn.read();

				//受け取った修復画像が送った画像と一致したタイムスタンプを持っている場合
				if(m_inpt_src_img.tm.sec == m_inpt_img.tm.sec 
					&& m_inpt_src_img.tm.nsec == m_inpt_img.tm.nsec){
					cout<<"imageInpaint : ReceiveInpaint"<<endl;

					//修復基画像を保持する変数に受け取った画像を上書き
					inpt_cam_img = m_inpt_img;

					//処理する画像をtemp領域に保存
					//保存場所の絶対パスをOutPortにコピー
					m_modi_img_path = makepathString(tempName + "\\sub\\inpt_img.jpg",inpt_cam_img);
					m_modi_img_pathOut.write();
 
					//処理中画像をOutPortにコピー
					m_modi_img = inpt_cam_img;
					m_modi_imgOut.write();

					//受け取り処理が完了したことを示すフラグを立てる
					write_data_flag = true;

					//vectorのiteratorを進める
					if(m_cont_type == "Rectangle" && inpt_it_rect != cont_rect.end())	inpt_it_rect++;
					else if(m_cont_type == "Convex" && inpt_it_convex != cont_convex.end())	inpt_it_convex++;
					else{
						first_flag = true;
						inpaint_end_flag = true;
						inpaint_modify_flag = false;
						old_inpt_img = inpt_cam_img;
						reset_inpt_img = inpt_cam_img;

						m_step_flag.data = 15;
						setTimestamp(m_step_flag);
						m_step_flagOut.write();

					}
				}
			}
		}
	}

/*- - - - - - - - - - - Inpaintに対するユーザの修正 - - - - - - - - - - -*/

	//輪郭点データ群の修正が終了し、修復処理が終了していない場合
	if(inpaint_end_flag == true && inpaint_modify_flag == false){

		//クリック入力のバッファの初期化
		if(first_flag == true){
			while(m_click_pointIn.isNew()) m_click_pointIn.read();
			while(m_drag_rectIn.isNew()) m_drag_rectIn.read();
			cout<<"imageInpaint : clear Buf"<<endl;
			first_flag = false;

		}

		//ユーザのクリック情報をNew
		if(m_click_pointIn.isNew()){
			m_click_pointIn.read();
			
			//ユーザからの処理フラグ情報(z軸データ)による場合分け
			switch((int)m_click_point.data.z){
			
			//戻るボタンが押されたことを表すフラグ
			case 3:{
				//前回ループのinpaint基画像を、現在の画像に上書き
				inpt_cam_img = old_inpt_img;
				write_data_flag = true;

				break;
			}

			//リセットデータの更新が押されたことを表すフラグ
			case 9:
				//現在画像を、inpaint基画像変数に上書き
				reset_inpt_img = inpt_cam_img;
				write_data_flag = true;

				m_step_flag.data = 8;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();

				break;


			//リセットボタンが押されたことを表すフラグ
			case 4:{
				//未処理のinpaint基画像を、現在の画像に上書き
				inpt_cam_img = reset_inpt_img;
				write_data_flag = true;

				m_step_flag.data = 9;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();				

				break;
			}

			//修正処理完了を示すフラグ
			case 5:
				//修正が終了したフラグを立てる
				inpaint_modify_flag = true;

				m_step_flag.data = 12;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();

				cout<<"imageInpaint : ModifyFIN"<<endl;

				break;
			default :
				break;
			}

			//ドラッグ処理を受け付けるInPortをNew
			if(m_drag_rectIn.isNew()){

				//Portの読み込み
				m_drag_rectIn.read();

				//前回ループ画像データを保持
				old_inpt_img = inpt_cam_img;

				//ドラッグされた領域を示す長方形を作成
				Rect dragged_rect = Rect(m_drag_rect.data[0],m_drag_rect.data[1],m_drag_rect.data[2],m_drag_rect.data[3]);

				//修復したい画像と修復領域をOutPortに送る
				if(write_data_flag == true){
					cout<<"imageInpaint : SendInpaint"<<endl;

					//修復基画像の作成
					setTimestamp(inpt_cam_img);
					m_inpt_src_img = inpt_cam_img;

					//修復処理に用いるマスク領域の指定
					m_mask_area.tm = inpt_cam_img.tm;
					m_mask_area.data.length(4);
					m_mask_area.data[0] = dragged_rect.tl().x;
					m_mask_area.data[1] = dragged_rect.tl().y;
					m_mask_area.data[2] = dragged_rect.width;
					m_mask_area.data[3] = dragged_rect.height;

					m_inpt_src_imgOut.write();
					m_mask_areaOut.write();

					//OutPortに送ったことを示すフラグを立てる
					write_data_flag = false;
				}

			}

			//処理する画像をtemp領域に保存
			//保存場所の絶対パスをOutPortにコピー
			m_modi_img_path = makepathString(tempName + "\\sub\\inpt_img.jpg",inpt_cam_img);
			m_modi_img_pathOut.write();

			//処理中画像をOutPortにコピー
			m_modi_img = inpt_cam_img;
			m_modi_imgOut.write();
		}
		
		if(write_data_flag == false && m_inpt_imgIn.isNew()){
			m_inpt_imgIn.read();

			//受け取った修復画像が送った画像と一致したタイムスタンプを持っている場合
			if(m_inpt_src_img.tm.sec == m_inpt_img.tm.sec 
				&& m_inpt_src_img.tm.nsec == m_inpt_img.tm.nsec){
				cout<<"imageInpaint : ReceiveInpaint"<<endl;

				//修復基画像を保持する変数に受け取った画像を上書き
				inpt_cam_img = m_inpt_img;

				//処理する画像をtemp領域に保存
				//保存場所の絶対パスをOutPortにコピー
				m_modi_img_path = makepathString(tempName + "\\sub\\inpt_img.jpg",inpt_cam_img);
				m_modi_img_pathOut.write();
 
				//処理中画像をOutPortにコピー
				m_modi_img = inpt_cam_img;
				m_modi_imgOut.write();

				write_data_flag = true;
			}
		}
	}



	//すべての処理が終了した場合
	if(inpaint_modify_flag == true){

		//完成データをOutPortに送る
		m_proc_img = inpt_cam_img;
		m_proc_img.tm = src_cam_img.tm;

		m_proc_imgOut.write();
	}		

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t imageInpaint::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t imageInpaint::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 表示ウィンドウを閉じる
 */

RTC::ReturnCode_t imageInpaint::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t imageInpaint::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t imageInpaint::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void imageInpaintInit(RTC::Manager* manager)
  {
    coil::Properties profile(imageinpaint_spec);
    manager->registerFactory(profile,
                             RTC::Create<imageInpaint>,
                             RTC::Delete<imageInpaint>);
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


/**
 * 輪郭点データ群を画像に描画する
 *
 * @para vector<Rect> cont		輪郭点データ群
 * @para Mat& src_img			描画対象画像
 * @return void					
 */
void drawContoursImage(vector<Rect> cont,Mat& src_img){
		
	Scalar color = Scalar(0,0,255);	//グレースケールに輪郭を囲む長方形を描画するためのカラー
	//輪郭を囲む長方形を描画
	for( int i = 0; i< cont.size(); i++ ){
		rectangle( src_img, cont[i].tl(), cont[i].br(),color, 1, 8, 0 );
	}
}

/**
 * 輪郭点データ群を画像に描画する
 *
 * @para vector<vector<Point> > cont	輪郭点データ群
 * @para Mat& src_img					描画対象画像
 * @return void					
 */
void drawContoursImage(vector<vector<Point> > cont,Mat& src_img){
		
	Scalar color = Scalar(0,0,255);	//グレースケールに輪郭を囲む長方形を描画するためのカラー
	//輪郭を囲む凸図形を描画
	for( int i = 0; i< cont.size(); i++ ){
		drawContours( src_img, cont, i, Scalar(0,255,0), 1, 8, vector<Vec4i>(), 0, Point() );
	}
}

/**
 * 与えられた座標が輪郭点データ群によって成り立つ長方形に入っているかを調べ、
 * もし入っている場合は入っている長方形の中から一番面積が小さい長方形の輪郭点データを削除する
 *
 * @para int img_size			基画像のサイズ
 * @para vector<Rect> &rect		輪郭点データ群
 * @para Point3D p				座標データ
 * @return bool					削除を行った場合はtrueを返す
 */
bool eraceClickRect(int img_size,vector<Rect> &cont,Point3D p){

	vector<vector<Rect>::iterator> cont_it;
	vector<int> rect_area;

	//与えられた座標が、輪郭点データ群から成される長方形の中に入っているかを調べ
	//入っている場合はその長方形の面積と、その長方形のiteratorを保持
	for (vector<Rect>::iterator it = cont.begin(); it != cont.end(); ++it) {
		Rect r = *it;
		if(r.x <= p.x && p.x <= r.br().x && p.y <= r.br().y && r.y <= p.y){
			cont_it.push_back(it);
			rect_area.push_back(r.area());
		}
	}

	vector<Rect>::iterator min_it;
	int min_rect_area = img_size;
	bool it_flag = false;

	//保持した長方形面積データ群の中から、最少の面積を持つ長方形のiteratorを求める
	for(int i=0;i<cont_it.size();i++){
		if(min_rect_area > rect_area.at(i)){
			it_flag = true;
			min_rect_area = rect_area.at(i);
			min_it = cont_it.at(i);
		}
	}

	//最少長方形のiteratorが求まっている場合は、そのiteratorの指す輪郭点データを削除する
	if(it_flag == true)cont.erase(min_it);

	return it_flag;
}

/**
 * 与えられた座標が輪郭点データ群によって成り立つ凸図形に入っているかを調べ、
 * もし入っている場合は入っている凸図形の中から一番面積が小さい凸図形の輪郭点データを削除する
 *
 * @para int img_size			基画像のサイズ
 * @para vector<Rect> &rect		輪郭点データ群
 * @para Point3D p				座標データ
 * @return bool					削除を行った場合はtrueを返す
 */
bool eraceClickRect(Mat img,vector<vector<Point>> &cont,Point3D p){
	vector<vector<vector<Point>>::iterator> cont_it;
	vector<double> rect_area;

	//与えられた座標が、輪郭点データ群から成される凸図形の中に入っているかを調べ
	//入っている場合はその凸図形の面積と、その凸図形のiteratorを保持
	for (vector<vector<Point>>::iterator it = cont.begin(); it != cont.end(); ++it) {
		vector<Point> r = *it;

		int cn = 0;

		for(int i = 0; i < r.size() ; i++){
			int fp = i; int ep = i+1;
			if(ep == r.size())ep = 0;

			// 上向きの辺。点Pがy軸方向について、始点と終点の間にある。ただし、終点は含まない。(ルール1)
			if( ((r[fp].y <= p.y) && (r[ep].y > p.y))
			// 下向きの辺。点Pがy軸方向について、始点と終点の間にある。ただし、始点は含まない。(ルール2)
				|| ((r[fp].y > p.y) && (r[ep].y <= p.y)) ){
			// ルール1,ルール2を確認することで、ルール3も確認できている。
            
				// 辺は点pよりも右側にある。ただし、重ならない。(ルール4)
				// 辺が点pと同じ高さになる位置を特定し、その時のxの値と点pのxの値を比較する。
				double vt = (double)(p.y - r[fp].y) / (double)(r[ep].y - r[fp].y);
				if(p.x < (r[fp].x + (vt * (r[ep].x - r[fp].x)))) ++cn;
			}
		}
		if(cn%2 == 1){
			cont_it.push_back(it);
			rect_area.push_back(contourArea(r));
		}
	}

	vector<vector<Point>>::iterator min_it;
	int min_rect_area = Rect(0,0,img.cols,img.rows).area();
	bool it_flag = false;

	//保持した長方形面積データ群の中から、最少の面積を持つ凸図形のiteratorを求める
	for(int i=0;i<cont_it.size();i++){
		if(min_rect_area > rect_area.at(i)){
			it_flag = true;
			min_rect_area = rect_area.at(i);
			min_it = cont_it.at(i);
		}
	}

	//最少長方形のiteratorが求まっている場合は、そのiteratorの指す輪郭点データを削除する
	if(it_flag == true){
		vector<Point> r = *min_it;
		ostringstream str;
		for(vector<Point>::iterator it = r.begin(); it != r.end(); ++it){
			Point pp = *it;
			str << "("; str << pp.x; str << ","; str << pp.y; str << ") ";
		}
		cout<<"imageInpaint : EraseConvex:"<<str.str()<<endl;
		cont.erase(min_it);
	}else{
		cout<<"imageInpaint : ErasePoint:NON"<<endl;
	}

	return it_flag;
}

/**
 * ドラッグによって形成された長方形の中に存在する輪郭点データを削除する
 *
 * @para vector<Rect> &cont_rect	輪郭点データ群
 * @para Rect dragged_rect			ドラッグでできた長方形
 * @return void
 */
void eraceDraggedRect(vector<Rect> &cont_rect,Rect dragged_rect){
	//ドラッグされた領域に含まれる輪郭データ群を削除する
	for (vector<Rect>::iterator it = cont_rect.begin(); it != cont_rect.end();) {
		Rect r = *it;
		if(dragged_rect.x <= r.x && r.br().x <= dragged_rect.br().x
			&& dragged_rect.y <= r.y && r.br().y <= dragged_rect.br().y){
			it = cont_rect.erase(it);
		}else{
			it++;
		}
	}
}

/**
 * ドラッグによって形成された長方形の中に存在する輪郭点データを削除する
 *
 * @para vector<vector<Point>> &cont	輪郭点データ群
 * @para Rect dragged_rect				ドラッグでできた長方形
 * @return void
 */
void eraceDraggedRect(vector<vector<Point>> &cont,Rect dr){

	//与えられた長方形の中に入っている凸図形輪郭点データ群を削除する
	for (vector<vector<Point>>::iterator it = cont.begin(); it != cont.end();) {
		bool erase_flag = true;

		vector<Point> p = *it;

        for(int i = 0; i < p.size() ; i++){
			if(dr.tl().x > p[i].x || dr.tl().y > p[i].y
				|| dr.br().x < p[i].x || dr.br().y < p[i].y){
					erase_flag = false;
					break;
			}
		}

		if(erase_flag == true) it = cont.erase(it);
		else it++;
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


