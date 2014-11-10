CVCanny
=================
このコンポーネントは入力画像に対してエッジ検出を行うコンポーネントです。

概要
--------
![Canny02](http://www.sic.shibaura-it.ac.jp/~ma13055/Canny02)


### 使用関数 ###
[OpenCV_Canny](http://opencv.jp/opencv-2.2/c/imgproc_feature_detection.html "OpenCV_Canny")

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
