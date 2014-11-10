OpenCV_Comps
=================
このフォルダでは、OpenCVの関数をコンポーネント化したものを提供しています。 


概要
--------
提供コンポーネントは以下の関数を持つコンポーネントです。
* CVCanny - エッジ検出コンポーネント
   * Canny();
* CVFeatureDetector - 特徴点検出コンポーネント
   * FastFeatureDetector
   * GoodFeaturesToTrackDetector
   * StarFeatureDetector
   * SiftFeatureDetector
   * SurfFeatureDetector
   * MserFeatureDetector
   * GridAdaptedFeatureDetector();
   * PyramidAdaptedFeatureDetector();
   * DynamicAdaptedFeatureDetector();
* CVFindContours - 輪郭点検出コンポーネント
   * makeContours();
   * approxPolyDP();
   * ConvexHull();
* CVHoughTransform - ハフ変換コンポーネント
   * HoughLine();
   * HoughLineP();
* CVImgThreshold - 二値化処理コンポーネント
   * threshold();
   * adaptiveThreshold();
* CVInpaint - 修復処理コンポーネント
   * inpaint();


仕様
--------
Windows7 32bit,64bit  
OpenRTM-aist-1.1.0-RELEASE（C++)  
Eclipse 3.8.1 + OpenRTM Eclipse tools 1.1.0-RC4


RTM2014
