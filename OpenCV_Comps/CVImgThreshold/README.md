CVImgThreshold
=================
このコンポーネントは入力画像に対して二値化処理を行うコンポーネントです。

概要
--------
![ImgThreshold01](http://www.sic.shibaura-it.ac.jp/~ma13055/ImgThreshold01)
![ImgThreshold02_](http://www.sic.shibaura-it.ac.jp/~ma13055/ImgThreshold02_)


### 使用関数 ###
[OpenCV_adaptiveThreshold,threshold](http://opencv.jp/opencv-2svn/cpp/miscellaneous_image_transformations.html "OpenCV_adaptiveThreshold,threshold")

      void adaptiveThreshold(
       const Mat& src,     //入力画像
       Mat& dst,           //出力画像
       double maxValue,    //条件を満足するピクセルに割り当てられる非0の値
       int adaptiveMethod, //利用される適応的閾値アルゴリズム
       int thresholdType,  //閾値の種類
       int blockSize,      //ピクセルの閾値を求めるために利用される近傍領域のサイズ
       double C            //平均または加重平均から引かれる定数
       );

     void cvCanny(
      const CvArr* image,   // 入力画像
      CvArr* edges,         // 出力画像
      double threshold1,    // 1番目のしきい値
      double threshold2,    // 2番目のしきい値
      int aperture_size = 3 // アパーチャサイズ
      );

仕様
--------
Windows7 32bit,64bit  
OpenRTM-aist-1.1.0-RELEASE（C++)
Eclipse 3.8.1 + OpenRTM Eclipse tools 1.1.0-RC4


RTM2014
