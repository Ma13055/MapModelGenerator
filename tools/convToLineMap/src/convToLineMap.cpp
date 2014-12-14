// -*- C++ -*-
/*!
 * @file  convToLineMap.cpp
 * @brief 輪郭情報と特徴点を基にラインマップへ変換する
 * @date $Date$
 *
 * @author 立川将(Tatekawa Masaru)
 * Email:y09148@shibaura-it.ac.jp
 *
 * $Id$
 */

#include "convToLineMap.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\features2d\features2d.hpp"

using namespace std;
using namespace cv;


//CameraImage型のデータをMat型に変換する
Mat CamToMat(CameraImage &cam_img);

//Mat型のデータをCameraImage型に変換する
CameraImage MatToCam(Mat &mat_img);

//CameraImage型のデータを初期化する
CameraImage makeEmptyCamImg();

//CameraImage型の二つのデータを比較
bool equalCamImg(CameraImage &src_img,CameraImage &rec_img);

//TimedShortSeq型のデータ二つを比較する
bool equalTimedShortSeq(TimedShortSeq &src,TimedShortSeq &rec);

//TimedFloatSeq型のデータ二つを比較する
bool equalTimedFloatSeq(TimedFloatSeq &src,TimedFloatSeq &rec);

//輪郭点データ群を画像に描画する
void drawContoursImage(vector<vector<Point> > cont,Mat &src_img,Scalar color,String region_fill);

//点群データ群を画像に描画する
void drawContPointImage(vector<vector<Point> > vvp,Mat &src_img);

//点データ群を画像に描画する
void drawPointVector(vector<Point> vp,Mat &src_img,Scalar c,int size);

//与えられた座標が指す図形の輪郭点データを削除する
bool eraseClickRect(Mat img,vector<vector<Point>> &cont,Point3D p);

//与えられた座標が指す図形の輪郭点データを最適化する
bool defragClickRect(Mat img,vector<vector<Point>> &cont,Point3D p);

//ドラッグによって形成された矩形の中に存在する輪郭図形データを削除する
void eraseDraggedRect(vector<vector<Point>> &cont,Rect dr);

//ドラッグによって形成された矩形の中に存在する輪郭点データを削除する
void eraseDraggedPoint(vector<vector<Point>> &cont,Rect dr);

//ドラッグによって形成された長方形のを図形輪郭点データ群に追加する
void addDraggedRect(vector<vector<Point>> &cont,Rect dr);

//画像の保存を行いそのパスを返す
TimedString makePathString(String str,CameraImage src);

//与えられた点に対して、点データ群の中から一番近い点の座標を取得する
Point getNearPoint(Mat img,vector<Point> &vp,Point p);

//与えられた点に対して、一番近い図形頂点のiteratorを取得する
bool getNearConvexPoint(Mat img,vector<vector<Point> > &conv,Point p,vector<vector<Point> >::iterator &c_it,vector<Point>::iterator &p_it);

//与えられた点群データ群からiteratorの指す要素を削除する
bool eraseVVPoint(vector<vector<Point> > &vvp,vector<vector<Point> >::iterator &vvp_it,vector<Point>::iterator &vp_it);

//与えられた点群データ群からiteratorの指す要素の中身を返す
Point getVVPoint(vector<vector<Point> > &vvp,vector<vector<Point> >::iterator &vvp_it,vector<Point>::iterator &vp_it);

//与えられた点群データ群からiteratorの指す一つ前の要素の中身を返す
Point getBackVVPoint(vector<vector<Point> > &vvp,vector<vector<Point> >::iterator &vvp_it,vector<Point>::iterator &vp_it);

//与えられた点群データ群からiteratorの指す一つ次の要素の中身を返す
Point getNextVVPoint(vector<vector<Point> > &vvp,vector<vector<Point> >::iterator &vvp_it,vector<Point>::iterator &vp_it);

//画像に与えられた三つの点を描画する
void drawPoints(Mat &img, Point c_p, Point n_p, Point b_p);

//与えられた画像に与えられた二つの点を描画する
void drawPoints(Mat &img, Point c_p, Point s_p);

//与えられた点が二つの点のどちらに近いかを判断し、近い方の点を返す
Point getSecondPoint(Point c_p,Point n_p,Point b_p);

//与えられたvector<vector<Point>>型をTimedShortSeq型にして返す
TimedShortSeq vectToSeq(vector<vector<Point> > vvp);

//与えられたTimedShortSeq型をvector<vector<Point>>型に変換する
void seqToVec(TimedShortSeq tss,vector<vector<Point> > &vvp);

//与えられた二点からなる線分の長さを返す
double getPointToPointLength(Point p1,Point p2);

//輪郭点群の最適化
void defragVector(vector<Point> &vp);

//順不同で同じ要素を持つvectorがあるかどうか調べる
bool aloneVector(vector<Point> alone_vp,vector<vector<Point> > vvp);

// Module specification
// <rtc-template block="module_spec">
static const char* convtolinemap_spec[] =
  {
    "implementation_id", "convToLineMap",
    "type_name",         "convToLineMap",
    "description",       "輪郭情報と特徴点を基にラインマップへ変換する",
    "version",           "3.1.0",
    "vendor",            "Masaru Tatekawa(SIT)",
    "category",          "Conversion",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.03_modifyType", "defrag",
    "conf.default.01_threshold", "3",
    "conf.default.02_regionFill", "ON",
    // Widget
    "conf.__widget__.03_modifyType", "radio",
    "conf.__widget__.01_threshold", "text",
    "conf.__widget__.02_regionFill", "radio",
    // Constraints
    "conf.__constraints__.03_modifyType", "(defrag,eraseFigure,erasePoint,add,movePoint)",
    "conf.__constraints__.01_threshold", "x>=0",
    "conf.__constraints__.02_regionFill", "(ON,OFF)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
convToLineMap::convToLineMap(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_temp_pathIn("tempFolderPath", m_temp_path),
    m_src_imgIn("srcImage", m_src_img),
    m_cont_dataIn("contoursData", m_cont_data),
    m_feature_pointsIn("featurePoints", m_feature_points),
    m_click_pointIn("clickPoint", m_click_point),
    m_drag_rectIn("draggedRect", m_drag_rect),
    m_modi_imgOut("modifyImage", m_modi_img),
    m_modi_img_pathOut("modifyImagePath", m_modi_img_path),
    m_map_lineOut("compLineMap", m_map_line),
    m_step_flagOut("stepFlag", m_step_flag)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
convToLineMap::~convToLineMap()
{
}



RTC::ReturnCode_t convToLineMap::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("tempFolderPath", m_temp_pathIn);
  addInPort("srcImage", m_src_imgIn);
  addInPort("contoursData", m_cont_dataIn);
  addInPort("featurePoints", m_feature_pointsIn);
  addInPort("clickPoint", m_click_pointIn);
  addInPort("draggedRect", m_drag_rectIn);
  
  // Set OutPort buffer
  addOutPort("modifyImage", m_modi_imgOut);
  addOutPort("modifyImagePath", m_modi_img_pathOut);
  addOutPort("stepFlag", m_step_flagOut);
  addOutPort("compLineMap", m_map_lineOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("03_modifyType", m_modi_type, "defrag");
  bindParameter("01_threshold", m_threshold, "3");
  bindParameter("02_regionFill", m_region_fill, "ON");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t convToLineMap::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t convToLineMap::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t convToLineMap::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * 初期化を行う
 */

RTC::ReturnCode_t convToLineMap::onActivated(RTC::UniqueId ec_id)
{
	cout<<"convToLineMap : onActivated : START"<<endl;
	//temp領域のパスを読み込む
	if(m_temp_pathIn.isNew()){
		m_temp_pathIn.read();
		tempName = m_temp_path.data;
	}else{
		tempName = "C:\\tmp\\makeMapsModel";
	}

	//ポートの初期化
	while(m_src_imgIn.isNew()) m_src_imgIn.read();
	while(m_cont_dataIn.isNew()) m_cont_dataIn.read();
	while(m_temp_pathIn.isNew()) m_temp_pathIn.read();
	while(m_click_pointIn.isNew()) m_click_pointIn.read();
	while(m_drag_rectIn.isNew()) m_drag_rectIn.read();
	while(m_feature_pointsIn.isNew()) m_feature_pointsIn.read();

	//持越し変数の初期化
	old_cont_data = TimedShortSeq();
	old_feature_points = TimedFloatSeq();
	old_region_fill = m_region_fill.c_str();

	line_base.clear();
	reset_line_base.clear();
	last_line_base.clear();

	keypoints.clear();

	src_cam_img = makeEmptyCamImg();
	old_img = makeEmptyCamImg();

	gray_img = Mat();
	gray_color_img = Mat();
	src_img = Mat();
	pre_img = Mat();

	remake = false;
	first_flag = true;
	contours_make_flag = false;
	contours_modify_flag = false;
	contours_click_erase_flag = false;

	add_click_step = 0;
	move_click_step = 0;
	click_cont_point = Point();
	back_cont_point = Point();
	next_cont_point = Point();
	second_cont_point = Point();
	click_feature_point = Point();

	cout<<"convToLineMap : onActivated : END"<<endl;

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t convToLineMap::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * UIとの連携とマップ情報の変換を行う
 */

RTC::ReturnCode_t convToLineMap::onExecute(RTC::UniqueId ec_id)
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
			cout<<"convToLineMap : changeImg"<<endl;

			src_cam_img = old_img;
			cout<<"convToLineMap : "<<src_cam_img.tm.sec<<","<<src_cam_img.tm.nsec<<endl;

			//NewされたCameraImageをMat型に変換
			src_img = CamToMat(m_src_img);	//受信した画像

			//受け取った画像をグレースケールに変換
			if(src_img.elemSize() != 1)cvtColor(src_img,gray_img,CV_RGB2GRAY);
			else gray_img = src_img.clone();

			cvtColor(gray_img,gray_color_img,CV_GRAY2RGB);
		}
	}

	//入力ポートの更新
	if(m_cont_dataIn.isNew()){
		m_cont_dataIn.read();

		//凸図形データ群が更新された場合
		if(!equalTimedShortSeq(old_cont_data,m_cont_data)){
			cout<<"convToLineMap : changeContoursData"<<endl;

			seqToVec(old_cont_data,cont_data);
			remake = true;
		}
	}

	//入力ポートを更新
	if(m_feature_pointsIn.isNew()){
		m_feature_pointsIn.read();

		//特徴点データが更新された場合
		if(!equalTimedFloatSeq(old_feature_points,m_feature_points)){
			cout<<"convToLineMap : changeFeaturePoint"<<endl;

			//保持してあった特徴点データを削除
			keypoints.clear();

/*	//KeyPoint型として受け取りたい場合の書き方
			//受け取った特徴点要素データをKeyPoint型に変換
			for(int i=0;i<old_feature_points.data.length();i+=7){
				KeyPoint key = KeyPoint(
					Point2f (old_feature_points.data[i+0],old_feature_points.data[i+1]),	//Pont2f
					old_feature_points.data[i+2],			//size
					old_feature_points.data[i+3],			//angle
					old_feature_points.data[i+4],			//response
					(int)old_feature_points.data[i+5],		//octave
					(int)old_feature_points.data[i+6]);		//class_id

				keypoints.push_back(key);
			}
*/
			Mat src_copy_img = gray_img.clone();
			//受け取った特徴点要素データをPoint型に変換
			for(int i=0;i<old_feature_points.data.length();i+=7){
				Point p = Point((int)old_feature_points.data[i+0],(int)old_feature_points.data[i+1]);
				keypoints.push_back(p);
			}

			remake = true;
		}
	}

/*---------ContoursとFeaturePointsを用いてラインマップの下地を作成する---------*/

	//画像データ、特徴点群、輪郭点群を受け取った場合
	if(gray_color_img.data != 0 && keypoints.size() != 0 && cont_data.size() != 0 && remake == true){
		cout<<"convToLineMap : MakeLineMapBase"<<endl;

		//輪郭図形群を初期化
		line_base.clear();
		double before_slope = 0;

		//輪郭点群と特徴点群を用いて輪郭図形群を作成
		for(vector<vector<Point> >::iterator vvp_it = cont_data.begin() ; vvp_it != cont_data.end() ; ++vvp_it){

			//iteratorの指す輪郭点群
			vector<Point> vp = *vvp_it;
			//輪郭点群に重なる特徴点群を保存する変数を作成
			vector<Point> line_base_points;

			for(vector<Point>::iterator vp_it = vp.begin() ; vp_it != vp.end() ; ++vp_it){

				//iteratorの指す輪郭点
				Point p = *vp_it;

				//輪郭点を図形の頂点として保持
				line_base_points.push_back(p);
			}

			//保持した図形頂点群から、連続して重なっている点を削除
			defragVector(line_base_points);
			//図形が線分以上として成り立っている場合かつ、同じ形の点群が存在しない場合は輪郭図形として保持
			if(line_base_points.size() > 1 && aloneVector(line_base_points,line_base))
				line_base.push_back(line_base_points);
		}
	}


/*- - - - - - - - - - - 下地ラインマップに対するユーザの修正 - - - - - - - - - - -*/

	if(line_base.size() != 0 && remake == true){
		cout<<"convToLineMap : DrawContours"<<endl;

		pre_img = Mat();

		Mat src_gray_copy_img = gray_color_img.clone();

		//前回情報・リセット情報に用いるためにvectorを輪郭データをコピー
		reset_line_base = line_base;
		last_line_base = line_base;
		//元画像のグレー画像に凸図形輪郭データを描画
		drawContoursImage(line_base, src_gray_copy_img, Scalar(0,0,255),m_region_fill);
		//元画像のグレー画像に輪郭点データを描画
		drawContPointImage(line_base, src_gray_copy_img);

		//処理する画像をtemp領域に保存
		//保存場所の絶対パスをOutPortにコピー
		m_modi_img_path = makePathString(tempName + "\\sub\\contours_modi_img.jpg",MatToCam(src_gray_copy_img));
		m_modi_img_pathOut.write();

		//輪郭データの抽出が終了したフラグを立てる
		remake = false;
		first_flag = true;
		contours_make_flag = true;
		contours_modify_flag = false;
		contours_click_erase_flag = false;

		//その他フラグ変数の初期化を行う
		add_click_step = 0;
		move_click_step = 0;
		click_cont_point = Point();
		back_cont_point = Point();
		next_cont_point = Point();
		second_cont_point = Point();
		click_feature_point = Point();


		m_step_flag.data = 13;
		setTimestamp(m_step_flag);
		m_step_flagOut.write();

		cout<<"convToLineMap : ContoursMedify"<<endl;
	}

	//輪郭データ抽出が完了し、ユーザによる修正が終わっていない場合
	if(contours_make_flag == true && contours_modify_flag == false){

		//クリック入力のバッファの初期化
		if(first_flag == true){
			while(m_click_pointIn.isNew()) m_click_pointIn.read();
			while(m_drag_rectIn.isNew()) m_drag_rectIn.read();
			cout<<"convToLineMap : clear Buf"<<endl;
			first_flag = false;

		}

		bool repaint_flag = false;

		if(old_region_fill != m_region_fill.c_str()){
			old_region_fill = m_region_fill.c_str();
			repaint_flag = true;
		}

		//ユーザのクリック情報をNew
		if(m_click_pointIn.isNew()){
			m_click_pointIn.read();

			repaint_flag = true;
			//追加に用いるiteratorの宣言
			vector<vector<Point>>::iterator near_convex_it;
			vector<Point>::iterator near_point_it;


			//ユーザからの処理フラグ情報(z軸データ)による場合分け
			switch((int)m_click_point.data.z){
			
			//クリック処理を表すフラグ
			case 1:{
				//凸図形輪郭データを保持
				last_line_base = line_base;

				//configで修正タイプがdefragと選択されている場合
				if(m_modi_type == "defrag"){
					//追加処理の初期化
					add_click_step = 0;
					move_click_step = 0;

					//クリックされた場所の輪郭データを取り除く
					defragClickRect(gray_img,line_base,m_click_point.data);

				//configで修正タイプがeraseFigureと選択されている場合
				}else if(m_modi_type == "eraseFigure"){
					//追加処理の初期化
					add_click_step = 0;
					move_click_step = 0;

					//クリックされた場所の輪郭データを取り除く
					eraseClickRect(gray_img,line_base,m_click_point.data);

				//configで修正タイプがreasePointと選択されている場合
				}else if(m_modi_type == "erasePoint"){
					//追加処理の初期化
					add_click_step = 0;
					move_click_step = 0;

					//クリック座標と一番近い凸図形頂点のiteratorが取得できた場合
					if(getNearConvexPoint(gray_img,line_base,Point(m_click_point.data.x,m_click_point.data.y),near_convex_it,near_point_it)){
						//iteraotrの指す要素を削除
						eraseVVPoint(line_base,near_convex_it,near_point_it);
					}

				//configで修正タイプがaddと選択されている場合
				}else if(m_modi_type == "add"){
					move_click_step = 0;
					first_flag = true;
					switch(add_click_step){
					case 0:
						//クリック座標と一番近い凸図形頂点のiteratorが取得できた場合
						if(getNearConvexPoint(gray_img,line_base,Point(m_click_point.data.x,m_click_point.data.y),near_convex_it,near_point_it)){
							//凸図形頂点の座標を取得
							click_cont_point = getVVPoint(line_base,near_convex_it,near_point_it);
							//凸図形頂点の次の座標を取得
							next_cont_point = getNextVVPoint(line_base,near_convex_it,near_point_it);
							//凸図形頂点の前の座標を取得
							back_cont_point = getBackVVPoint(line_base,near_convex_it,near_point_it);

							add_click_step++;
						}
						break;
					case 1:
						//前後どちらの頂点が選択されたかを判別し、その点を取得する
						second_cont_point = getSecondPoint(Point(m_click_point.data.x,m_click_point.data.y),next_cont_point,back_cont_point);
						add_click_step++;
						break;

					case 2:
						//追加したい凸図形頂点の座標を特徴点群より取得
						click_feature_point = getNearPoint(gray_img,keypoints,Point(m_click_point.data.x,m_click_point.data.y));
						//二つの選択頂点の間に指定された特徴点を追加
						//点i -> 点i+1の関係だったとき
						if(second_cont_point == next_cont_point){
							if(getNearConvexPoint(gray_img,line_base,next_cont_point,near_convex_it,near_point_it)){
								line_base.at(near_convex_it - line_base.begin()).insert(near_point_it,1,click_feature_point);
							}
						//点i -> 点i-1の関係だったとき
						}else{
							if(getNearConvexPoint(gray_img,line_base,click_cont_point,near_convex_it,near_point_it)){
								line_base.at(near_convex_it - line_base.begin()).insert(near_point_it,1,click_feature_point);
							}
						}

						add_click_step = 0;
						break;
					}

				//configで修正タイプがmovePointと選択されている場合
				}else{
					add_click_step = 0;
					switch(move_click_step){
					case 0:
						//クリック座標と一番近い凸図形頂点のiteratorが取得できた場合
						if(getNearConvexPoint(gray_img,line_base,Point(m_click_point.data.x,m_click_point.data.y),near_convex_it,near_point_it)){
							//凸図形頂点の座標を取得
							click_cont_point = getVVPoint(line_base,near_convex_it,near_point_it);

							move_click_step++;
						}
						break;
					case 1:
						//追加したい凸図形頂点の座標を特徴点群より取得
						Point click_f_p = getNearPoint(gray_img,keypoints,Point(m_click_point.data.x,m_click_point.data.y));
						//すでに選択されている凸図形頂点の座標と、選択された特徴点座標の値とを入れ替える
						getNearConvexPoint(gray_img,line_base,click_cont_point,near_convex_it,near_point_it);
						*near_point_it = click_f_p;

						move_click_step = 0;
						break;
					}
				}

				break;
			}
			case 2:
				//ドラッグ処理を受け付けるInPortをNew
				while(!m_drag_rectIn.isNew())cout<<"Dragged : WAIT"<<endl;
				m_drag_rectIn.read();
				//追加処理の初期化
				if(m_modi_type != "add")add_click_step = 0;
				if(m_modi_type != "movePoint")move_click_step = 0;

				//凸図形輪郭データを保持
				last_line_base = line_base;

				//configで修正タイプがeraseFigureと選択されている場合
				if(m_modi_type == "eraseFigure"){
					//ドラッグされた領域に含まれる輪郭図形データ群を削除する
					eraseDraggedRect(line_base,Rect(m_drag_rect.data[0],m_drag_rect.data[1],m_drag_rect.data[2],m_drag_rect.data[3]));

				//configで修正タイプがreasePointと選択されている場合
				}else if(m_modi_type == "erasePoint"){
					//ドラッグされた領域に含まれる輪郭データ群を削除する
					eraseDraggedPoint(line_base,Rect(m_drag_rect.data[0],m_drag_rect.data[1],m_drag_rect.data[2],m_drag_rect.data[3]));

				//configで修正タイプがaddと選択されている場合
				}else if(m_modi_type == "add"){
					Rect draggedRect = Rect(m_drag_rect.data[0],m_drag_rect.data[1],m_drag_rect.data[2],m_drag_rect.data[3]);
					
					if((draggedRect.height*draggedRect.width) > 20){
						addDraggedRect(line_base,draggedRect);
						add_click_step = 0;
					}else{
						//configで修正タイプがaddと選択されている場合
						switch(add_click_step){
						case 0:
							//クリック座標と一番近い凸図形頂点のiteratorが取得できた場合
							if(getNearConvexPoint(gray_img,line_base,Point(draggedRect.tl().x,draggedRect.tl().y),near_convex_it,near_point_it)){
								//凸図形頂点の座標を取得
								click_cont_point = getVVPoint(line_base,near_convex_it,near_point_it);
								//凸図形頂点の次の座標を取得
								next_cont_point = getNextVVPoint(line_base,near_convex_it,near_point_it);
								//凸図形頂点の前の座標を取得
								back_cont_point = getBackVVPoint(line_base,near_convex_it,near_point_it);

								add_click_step++;
							}
							break;
						case 1:
							//前後どちらの頂点が選択されたかを判別し、その点を取得する
							second_cont_point = getSecondPoint(Point(draggedRect.tl().x,draggedRect.tl().y),next_cont_point,back_cont_point);
							add_click_step++;
							break;

						case 2:
							//追加したい凸図形頂点の座標を特徴点群より取得
							click_feature_point = getNearPoint(gray_img,keypoints,Point(draggedRect.tl().x,draggedRect.tl().y));
							//二つの選択頂点の間に指定された特徴点を追加
							//点i -> 点i+1の関係だったとき
							if(second_cont_point == next_cont_point){
								if(getNearConvexPoint(gray_img,line_base,next_cont_point,near_convex_it,near_point_it)){
									line_base.at(near_convex_it - line_base.begin()).insert(near_point_it,1,click_feature_point);
								}
							//点i -> 点i-1の関係だったとき
							}else{
								if(getNearConvexPoint(gray_img,line_base,click_cont_point,near_convex_it,near_point_it)){
									line_base.at(near_convex_it - line_base.begin()).insert(near_point_it,1,click_feature_point);
								}
							}

							first_flag = true;
							add_click_step = 0;
							break;
						}
					}

				}

				break;

			//リセットデータの更新が押されたことを表すフラグ
			case 9:
				//現在の輪郭データ群を、未処理輪郭データ群に上書き
				reset_line_base = line_base;
				m_step_flag.data = 8;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();


				break;

			//戻るボタンが押されたことを表すフラグ
			case 3:
				//前回ループの輪郭データ群を、現在の輪郭データ群に上書き
				line_base = last_line_base;

				break;

			//リセットボタンが押されたことを表すフラグ
			case 4:
				//未処理輪郭データ群を、現在の輪郭データ群に上書き
				line_base = reset_line_base;
				m_step_flag.data = 9;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();

				break;

			//修正処理完了を示すフラグ
			case 5:
				//輪郭修正が終了したフラグを立てる
				contours_modify_flag = true;

				//完成したラインデータをOutPortに送る
				m_map_line = vectToSeq(line_base);
				m_map_line.tm = src_cam_img.tm;
				m_map_lineOut.write();

				m_step_flag.data = 14;
				setTimestamp(m_step_flag);
				m_step_flagOut.write();


				cout<<"convToLineMap : ContoursModifyFin"<<endl;

				break;
			default :
				break;
			}
		}

		if(repaint_flag){
			//変更された輪郭データ群に対して再描画を行う
			Mat src_gray_copy_img = gray_color_img.clone();
			//元画像のグレー画像に輪郭図形データを描画
			drawContoursImage(line_base, src_gray_copy_img, Scalar(0,0,255),m_region_fill);

			//クリック情報の可視化
			if(add_click_step == 1)	drawPoints(src_gray_copy_img, click_cont_point, next_cont_point, back_cont_point);
			else if(add_click_step == 2){
				drawPoints(src_gray_copy_img, click_cont_point, second_cont_point);
				drawPointVector(keypoints, src_gray_copy_img, Scalar(255,0,0),1);
			}else if(move_click_step == 1){
				drawPointVector(keypoints, src_gray_copy_img, Scalar(255,0,0),1);
				circle(src_gray_copy_img, click_cont_point, 3, Scalar(0,255,0), -1);
			}else{
				//元画像のグレー画像に輪郭点データを描画
				drawContPointImage(line_base, src_gray_copy_img);
			}

			//処理する画像をtemp領域に保存
			//保存場所の絶対パスをOutPortにコピー
			m_modi_img_path = makePathString(tempName + "\\sub\\contours_modi_img.jpg",MatToCam(src_gray_copy_img));
			m_modi_img_pathOut.write();
		}
	}

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t convToLineMap::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t convToLineMap::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t convToLineMap::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t convToLineMap::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t convToLineMap::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void convToLineMapInit(RTC::Manager* manager)
  {
    coil::Properties profile(convtolinemap_spec);
    manager->registerFactory(profile,
                             RTC::Create<convToLineMap>,
                             RTC::Delete<convToLineMap>);
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
 * TimedFloatSeq型のデータ二つを比較する
 *
 * @para TimedFloatSeq &src		比較データ１
 * @para TimedFloatSeq &rec		比較データ２
 * @return bool	比較結果　同一データであればtrue　違うデータであればfalse
 */
bool equalTimedFloatSeq(TimedFloatSeq &src,TimedFloatSeq &rec){
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
void drawContoursImage(vector<vector<Point> > cont,Mat &src_img,Scalar color,string region_fill){
		
	//輪郭を囲む凸図形を描画
	for( int i = 0; i< cont.size(); i++ ){
		if(region_fill == "ON")drawContours( src_img, cont, i, color, -1, 8, vector<Vec4i>(), 0, Point() );
		else drawContours( src_img, cont, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
	}
}

/**
 * 点群データ群を画像に描画する
 *
 * @para vector<vector<Point> > vvp	点群データ群
 * @para Mat &src_img				描画対象画像
 * @para Scalar c					書き込みカラー
 * @return void					
 */
void drawContPointImage(vector<vector<Point> > vvp,Mat &src_img){

	for(vector<vector<Point> >::iterator vvp_it = vvp.begin();vvp_it != vvp.end();++vvp_it){

		//描画の色を輪郭点ごとに変える
		Scalar color = Scalar(rand()%256,rand()%256,rand()%256);

		drawPointVector(*vvp_it,src_img,color,1);
	}
}


/**
 * 点データ群を画像に描画する
 *
 * @para vector<Point> vp	点データ群
 * @para Mat &src_img		描画対象画像
 * @para Scalar c			書き込みカラー
 * @para int size			書き込む点のサイズ
 * @return void					
 */
void drawPointVector(vector<Point> vp,Mat &src_img,Scalar color,int size){
	for(vector<Point>::iterator vp_it = vp.begin(); vp_it!=vp.end(); ++vp_it)
		circle(src_img, *vp_it, size, color, -1);
}

/**
 * 与えられた座標が輪郭点データ群によって成り立つ図形に入っているかを調べ、
 * もし入っている場合は入っている図形の中から一番面積が小さい図形の輪郭点データを削除する
 *
 * @para int img_size					基画像のサイズ
 * @para vector<vector<Point>> &cont	輪郭点データ群
 * @para Point3D p						座標データ
 * @return bool							削除を行った場合はtrueを返す
 */
bool eraseClickRect(Mat img,vector<vector<Point>> &cont,Point3D p){
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
				double vt = (p.y - r[fp].y) / (r[ep].y - r[fp].y);
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
		cout<<"convToLineMap : EraseConvex:"<<str.str()<<endl;
		cont.erase(min_it);
	}else{
		cout<<"convToLineMap : ErasePoint:NON"<<endl;
	}

	return it_flag;
}

/**
 * 与えられた座標が輪郭点データ群によって成り立つ図形に入っているかを調べ、
 * もし入っている場合は入っている図形の中から一番面積が小さい図形の輪郭点データを最適化する
 *
 * @para int img_size					基画像のサイズ
 * @para vector<vector<Point>> &cont	輪郭点データ群
 * @para Point3D p						座標データ
 * @return bool							最適化を行った場合はtrueを返す
 */
bool defragClickRect(Mat img,vector<vector<Point>> &cont,Point3D p){
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

	//最少長方形のiteratorが求まっている場合は、そのiteratorの指す輪郭点データから、重なる座標を削除する
	if(it_flag){
		vector<Point> r = *min_it;
		vector<Point> defrag_r;
		ostringstream str;
		for(vector<Point>::iterator it = r.begin(); it != r.end(); ++it){
			bool same_flag = false;
			Point pp = *it;
			for(int i=0;i<defrag_r.size();i++){
				if(pp.x == defrag_r.at(i).x && pp.y == defrag_r.at(i).y){
					same_flag =true;
					break;
				}
			}
			if(same_flag == false){
				str << "("; str << pp.x; str << ","; str << pp.y; str << ") ";
				defrag_r.push_back(pp);
			}
		}

		cout<<"convToLineMap : DefragConvex:"<<str.str()<<endl;
		cont.erase(min_it);
		vector<Point> contour;
		approxPolyDP( Mat(defrag_r), contour, 3, true );

		cont.push_back(contour);

	}else{
		cout<<"convToLineMap : DefragConvex:NON"<<endl;
	}

	return it_flag;
}


/**
 * ドラッグによって形成された長方形の中に存在する図形輪郭点データ群を削除する
 *
 * @para vector<vector<Point>> &cont	輪郭点データ群
 * @para Rect dragged_rect				ドラッグでできた長方形
 * @return void
 */
void eraseDraggedRect(vector<vector<Point>> &cont,Rect dr){

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
 * ドラッグによって形成された長方形の中に存在する輪郭点データを削除する
 *
 * @para vector<vector<Point>> &cont	輪郭点データ群
 * @para Rect dragged_rect				ドラッグでできた長方形
 * @return void
 */
void eraseDraggedPoint(vector<vector<Point>> &cont,Rect dr){

	//与えられた長方形の中に入っている頂点を削除する
	for (vector<vector<Point>>::iterator vp_it = cont.begin(); vp_it != cont.end();) {
		bool erase_flag = true;

		vector<Point> vp = *vp_it;

		for(vector<Point>::iterator p_it = vp.begin(); p_it != vp.end();){
			Point p = *p_it;
			if(dr.tl().x < p.x && dr.tl().y < p.y
				&& dr.br().x > p.x && dr.br().y > p.y){
					p_it = vp.erase(p_it);
			}else{
				p_it++;
			}
		}

		*vp_it = vp;
		if(vp.size() <= 1) vp_it = cont.erase(vp_it);
		else vp_it++;
	}
}

/**
 * ドラッグによって形成された長方形のを図形輪郭点データ群に追加する
 *
 * @para vector<vector<Point>> &cont	輪郭点データ群
 * @para Rect dragged_rect				ドラッグでできた長方形
 * @return void
 */
void addDraggedRect(vector<vector<Point>> &cont,Rect dr){
	//追加する点群を作成
	vector<Point> vp;
	//左上の点を追加
	vp.push_back(dr.tl());
	//右上の点を追加
	vp.push_back(Point(dr.br().x,dr.tl().y));
	//右下の点を追加
	vp.push_back(dr.br());
	//左下の点を追加
	vp.push_back(Point(dr.tl().x,dr.br().y));

	//輪郭データ群に追加
	cont.push_back(vp);
}

/**
 * 画像の保存を行いそのパスを返す
 *
 * @para String str			保存場所
 * @para CameraImage src	保存対象画像
 * @return TimedString		保存データへのパス
 */
TimedString makePathString(String str,CameraImage src){
	TimedString ts;

	setTimestamp(ts);

	//処理する画像をtemp領域に保存
	imwrite(str,CamToMat(src));

	//保存場所の絶対パスをTimedStringにコピー
	ts.data = str.c_str();

	return ts;
}

/**
 * 与えられた点に対して、点データ群の中から一番近い点の座標を取得する
 *
 * @para Mat img				対象捜索範囲となる画像
 * @para vector<Point> &fear	点データ群
 * @para Point p				入力座標
 * @return Point				一番近い点の座標
 */
Point getNearPoint(Mat img,vector<Point> &vp,Point p){
	Point n_p = Point();

	//点と点の距離の最小値を持つ変数
	//初期値は画像の対角線
	double min_length = sqrt(pow(img.cols,2.0) + pow(img.rows,2.0));

	//特徴点データ群全網羅
	for(vector<Point>::iterator vp_it = vp.begin();vp_it != vp.end();++vp_it){
		Point n_p_sub = *vp_it;

		double line_length = getPointToPointLength(p,n_p_sub);

		//現在保持している最小値よりも小さい値だった場合
		if(line_length < min_length){
			//最小値を更新
			min_length = line_length;
			//最小値となる特徴点座標を更新
			n_p = n_p_sub;
		}
	}
	return n_p;
}


/**
 * 与えられた点に対して、一番近い図形頂点のiteratorを取得する
 *
 * @para Mat img									対象捜索範囲となる画像
 * @para vector<vector<Point> > &conv				図形データ群
 * @para Point p									入力座標
 * @para vector<vector<Point> >::iterator &c_it		目標頂点を含む凸図形のiterator
 * @para vector<Point>::iterator &p_it				目標頂点のiterator
 * @para bool &get									iteratorが取得できたかのフラグ
 * @return bool		iteratorが取得できた場合はtrueを返す
 */
bool getNearConvexPoint(Mat img,vector<vector<Point> > &conv,Point p,vector<vector<Point> >::iterator &c_it,vector<Point>::iterator &p_it){
	//取得フラグの初期化
	bool get = false;

	//点と点の距離の最小値を持つ変数
	//初期値は画像の対角線
	double min_length = sqrt(pow(img.cols,2.0) + pow(img.rows,2.0));

	//凸図形データ群の全網羅for文
	for(vector<vector<Point>>::iterator cont_it = conv.begin();cont_it != conv.end();++cont_it){
		//内部要素のiteratorは親vectorから要求
		for(vector<Point>::iterator point_it = conv.at(cont_it - conv.begin()).begin();
			point_it != conv.at(cont_it - conv.begin()).end();++point_it){
			//凸図形の頂点
			Point convex_p = *point_it;

			//与えられた座標と頂点との距離
			double line_length = getPointToPointLength(p,convex_p);

			//現在保持している最小値よりも小さい値だった場合
			if(line_length < min_length){
				//最小値を更新
				min_length = line_length;
				//この頂点のvector<Point>を指すiteratorを保持
				p_it = point_it;
				//この頂点を持つvector<vector<Point>>を指すiteratorを保持
				c_it = cont_it;
				//iteratorが更新されたことを示すフラグを立てる
				get = true;
			}
		}
	}
	return get;
}

/**
 * 与えられた点群データ群からiteratorの指す要素を削除する
 *
 * @para vector<vector<Point> > &vvp				削除対象を含む点群データ群
 * @para vector<vector<Point> >::iterator &vvp_it	削除目標を含む点群データ群のiterator
 * @para vector<Point>::iterator &vp_it				削除目標のiterator
 * @return void
 */
bool eraseVVPoint(vector<vector<Point> > &vvp,vector<vector<Point> >::iterator &vvp_it,vector<Point>::iterator &vp_it){
	//それぞれのiteratorが指す要素の要素番号
	int vvp_num = vvp_it - vvp.begin();
	int vp_num = vp_it - vvp.at(vvp_num).begin();

	//iteratorの指す要素を削除
	vvp.at(vvp_num).erase(vp_it);
	//削除の結果、凸図形の頂点が一つ以下になってしまった場合、図形として成立しないため削除
	if(vvp.at(vvp_num).size() <= 1){
		vvp.erase(vvp_it);
		return false;
	}else{
		return true;
	}
}

/**
 * 与えられた点群データ群からiteratorの指す要素の中身を返す
 *
 * @para vector<vector<Point> > &vvp				点群データ群
 * @para vector<vector<Point> >::iterator &vvp_it	目標座標を含む点群データ群のiterator
 * @para vector<Point>::iterator &vp_it				目標座標のiterator
 * @return Point	要素の中身
 */
Point getVVPoint(vector<vector<Point> > &vvp,vector<vector<Point> >::iterator &vvp_it,vector<Point>::iterator &vp_it){
	//それぞれのiteratorが指す要素の要素番号
	int vvp_num = vvp_it - vvp.begin();
	int vp_num = vp_it - vvp.at(vvp_num).begin();

	//iteratorの指す要素を返す
	return vvp.at(vvp_num).at(vp_num);
}

/**
 * 与えられた点群データ群からiteratorの指す一つ前の要素の中身を返す
 *
 * @para vector<vector<Point> > &vvp				点群データ群
 * @para vector<vector<Point> >::iterator &vvp_it	目標座標を含む点群データ群のiterator
 * @para vector<Point>::iterator &vp_it				目標頂点のiterator
 * @return Point	要素の中身
 */
Point getBackVVPoint(vector<vector<Point> > &vvp,vector<vector<Point> >::iterator &vvp_it,vector<Point>::iterator &vp_it){
	//それぞれのiteratorが指す要素の要素番号
	int vvp_num = vvp_it - vvp.begin();
	int vp_num = vp_it - vvp.at(vvp_num).begin();
	//iteratorがvectorの先頭を指している場合
	if(vp_num == 0){
		//そのvectorの最後の要素を返す
		return vvp.at(vvp_num).at(vvp.at(vvp_num).size() - 1);
	}else{
		//iteratorの指す要素の一つ前を返す
		return vvp.at(vvp_num).at(vp_num - 1);
	}
}

/**
 * 与えられた点群データ群からiteratorの指す一つ次の要素の中身を返す
 *
 * @para vector<vector<Point> > &vvp				点群データ群
 * @para vector<vector<Point> >::iterator &vvp_it	目標座標を含む点群データ群のiterator
 * @para vector<Point>::iterator &vp_it				目標頂点のiterator
 * @return Point	要素の中身
 */
Point getNextVVPoint(vector<vector<Point> > &vvp,vector<vector<Point> >::iterator &vvp_it,vector<Point>::iterator &vp_it){
	//それぞれのiteratorが指す要素の要素番号
	int vvp_num = vvp_it - vvp.begin();
	int vp_num = vp_it - vvp.at(vvp_num).begin();
	//iteratorがvectorの最後を指している場合
	if(vp_num == vvp.at(vvp_num).size() - 1){
		//そのvectorの最初の要素を返す
		return vvp.at(vvp_num).at(0);
	}else{
		//iteratorの指す要素の一つ次を返す
		return vvp.at(vvp_num).at(vp_num + 1);
	}
}

/**
 * 画像に与えられた三つの点を描画する
 *
 * @para Mat &img		描画対象の画像
 * @para Point c_p		与えられた点１（クリックされた点）
 * @para Point n_p		与えられた点２（点１の次の頂点）
 * @para Point b_p		与えられた点３（点１の前の頂点）
 * @return void
 */
void drawPoints(Mat &img, Point c_p, Point n_p, Point b_p){
	//それぞれの点を描画
	circle(img, c_p, 3, Scalar(0,255,0), -1);
	circle(img, n_p, 3, Scalar(255,255,0), -1);
	circle(img, b_p, 3, Scalar(255,0,0), -1);
}

/**
 * 与えられた画像に与えられた二つの点を描画する
 *
 * @para Mat &img		描画対象の画像
 * @para Point c_p		与えられた点１（クリックされた点）
 * @para Point s_p		与えられた点２（次に選択された点）
 * @return void
 */
void drawPoints(Mat &img, Point c_p, Point s_p){
	//それぞれの点を描画
	circle(img, c_p, 3, Scalar(0,255,0), -1);
	circle(img, s_p, 3, Scalar(0,255,0), -1);
}

/**
 * 与えられた点が二つの点のどちらに近いかを判断し、近い方の点を返す
 *
 * @para Point c_p		与えられた点１（クリックされた点）
 * @para Point n_p		与えられた点２（点１の次の頂点）
 * @para Point b_p		与えられた点３（点１の前の頂点）
 * @return Point	点１に近い方の点
 */
Point getSecondPoint(Point c_p,Point n_p,Point b_p){
	//点１と点２の距離
	double np_length = getPointToPointLength(c_p,n_p);
	//点１と点３の距離
	double bp_length = getPointToPointLength(c_p,b_p);
	//より近い方の点を返す
	if(np_length <= bp_length) return n_p;
	else return b_p;
}


/**
 * 与えられたvector<vector<Point>>型をTimedShortSeq型にして返す
 * 変換の仕方は、vector<Point>のサイズ[0],Point1.x[1],Point2.y[2]・・・
 *
 * @para vector<vector<Point> > vvp		変換元となるvector<vector<Point>>型
 * @return TimedShortSeq				変換後のTimedShortSeq型
 */
TimedShortSeq vectToSeq(vector<vector<Point> > vvp){

	//変換先となる変数
	TimedShortSeq tss;

	//配列の要素番号として用いる変数の初期化
	int counter = 0;

	//全体要素数を把握
	//Point型
	for( int i = 0; i< vvp.size(); i++ ){
		counter += vvp[i].size()*2 + 1;
	}

	//TimeShortSeqのlengthを設定
	tss.data.length(counter);

	//再使用のため初期化
	counter = 0;

	//vector<vector<Point>>の全要素を通過するfor文
	for(int i = 0; i< vvp.size(); i++){
		//vector<Point>を書くために必要なサイズ
		tss.data[counter++] = vvp[i].size()*2;
		//vector<Point>の全要素を通過するfor文
		for(int j = 0; j < vvp[i].size(); j++){
			tss.data[counter++] = vvp[i][j].x;
			tss.data[counter++] = vvp[i][j].y;
		}
	}

	//タイムスタンプを設定
	setTimestamp(tss);

	return tss;
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
 * 与えられた二点からなる線分の長さを返す
 *
 * @para Point p1		線分の端点１
 * @para Point p2		線分の端点２
 * @return double		線分の長さ
 */
double getPointToPointLength(Point p1,Point p2){
	return sqrt(pow(p1.x - p2.x,2.0) + pow(p1.y - p2.y,2.0));
}

/**
 * 輪郭点群の最適化
 * 同じ点が連続で保管されている場合はその点を一だけ残して削除する
 *
 * @para vector<Point> &vp		最適化したい輪郭点群
 * @return void
 */
void defragVector(vector<Point> &vp){
	if(vp.size() > 1){
		for(vector<Point>::iterator vp_it = vp.begin()+1 ; vp_it != vp.end() ;){
			Point bp = *(vp_it-1);
			Point np = *vp_it;
			if(bp == np)vp_it = vp.erase(vp_it);
			else vp_it++;
		}
	}
}

/**
 * 順不同で同じ要素を持つvectorがあるかどうか調べる
 *
 * @para vector<Point> alone_vp			比較の元となるvector
 * @para vector<vector<Point> > vvp		比較対象のvector<vectr>
 * @return bool		同要素のvectorが存在しない場合trueを返す
 */
bool aloneVector(vector<Point> alone_vp,vector<vector<Point> > vvp){

	//比較対象のvectorを全網羅する
	for(vector<vector<Point> >::iterator vvp_it = vvp.begin();vvp_it != vvp.end();++vvp_it){
		vector<Point> vp = *vvp_it;

		//並びを含め同じものかどうかを判断
		//同じものならfalseを返す
		if(vp == alone_vp) return false;

		//比較結果を格納する変数
		//falseのままループを終えると同じ要素があったことを示す
		bool alone_flag = false;

		//比較対象と比較元のvectorを総当たりで比較
		for(int i=0;i<alone_vp.size();i++){

			//今回のループで取り出す比較対象要素が比較元に含まれているかの変数
			bool alone_point_check = true;

			//比較対象と総当たりで比較
			for(int j=0;j<vp.size();j++){
				//同じ要素だった場合
				if(alone_vp[i] == vp[j]){
					alone_point_check = false;
					break;
				}
			}
			//同じ要素がなかった場合
			if(alone_point_check){
				alone_flag = true;
				break;
			}
		}
		if(!alone_flag)return false;
	}
	return true;
}

