CVImgThreshold
=================
このコンポーネントは入力画像に対して二値化処理を行うコンポーネントです。

概要
--------
![ImgThreshold01](http://www.sic.shibaura-it.ac.jp/~ma13055/ImgThreshold01.png)
![ImgThreshold02_](http://www.sic.shibaura-it.ac.jp/~ma13055/ImgThreshold02_.png)


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

       double threshold(
        const Mat& src,       //入力画像
        Mat& dst,             //出力画像
        double thresh,        //閾値
        double maxVal,        //処理に用いる二値最大値の値
        int thresholdType     //閾値処理の種類
        );

閾値の種類は次のようになっています。  

 * THRESH_BINARY  
       ![THRESH_BINARY](http://www.sic.shibaura-it.ac.jp/~ma13055/threshold/THRESH_BINARY.png)
 * THRESH_BINARY_INV  
       ![THRESH_BINARY_INV](http://www.sic.shibaura-it.ac.jp/~ma13055/threshold/THRESH_BINARY_INV.png)
 * THRESH_TRUNC  
       ![THRESH_TRUNC](http://www.sic.shibaura-it.ac.jp/~ma13055/threshold/THRESH_TRUNC.png)
 * THRESH_TOZERO  
       ![THRESH_TOZERO](http://www.sic.shibaura-it.ac.jp/~ma13055/threshold/THRESH_TOZERO.png)
 * THRESH_TOZERO_INV  
       ![THRESH_TOZERO_INV](http://www.sic.shibaura-it.ac.jp/~ma13055/threshold/THRESH_TOZERO_INV.png)

仕様
--------
Windows7 32bit,64bit  
OpenRTM-aist-1.1.0-RELEASE（C++)  
Eclipse 3.8.1 + OpenRTM Eclipse tools 1.1.0-RC4  


RTM2014
