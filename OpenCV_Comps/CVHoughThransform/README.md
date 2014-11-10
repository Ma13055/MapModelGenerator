CVHoughThransform
=================
このコンポーネントは入力二値画像に対して標準ハフ変換を用いて直線を検出するコンポーネントです。

概要
--------
![HoughTransform01](http://www.sic.shibaura-it.ac.jp/~ma13055/HoughTransform01)
![HoughTransform02](http://www.sic.shibaura-it.ac.jp/~ma13055/HoughTransform02)


### 使用関数 ###
[OpenCV_HoughLines](http://opencv.jp/opencv-2svn/cpp/imgproc_feature_detection.html?highlight "OpenCV_HoughLines,HoughLinesP")

      //標準ハフ変換を行う関数
      void HoughLines(
       Mat& image,             //シングルチャンネルの2値入力画像
       vector<Vec2f>& lines,   //検出された直線が出力されるベクトル
       double rho,             //ピクセル単位で表される投票空間の距離分解能
       double theta,           //ラジアン単位で表される投票空間の角度分解能
       int threshold,          //投票の閾値パラメータ
       double srn=0,           //マルチスケールハフ変換において，距離分解能 rho の除数となる値
       double stn=0            //マルチスケールハフ変換において，角度分解能 theta の除数となる値
      );

      //確率的ハフ変換を行う関数
      void HoughLinesP(
       Mat& image,               //シングルチャンネルの2値入力画像
       vector<Vec4i>& lines,     //検出された直線が出力されるベクトル
       double rho,               //ピクセル単位で表される投票空間の距離分解能
       double theta,             //ラジアン単位で表される投票空間の角度分解能
       int threshold,            //投票の閾値パラメータ
       double minLineLength=0,   //最小の線分長
       double maxLineGap=0       //2点が同一線分上にあると見なす場合に許容される最大距離
      );

### outPortの具体説明 ###
outPortのTimedShortSeqには、線分の端点座標が配列化されています。  
線分に対する端点の配列順は以下の通りです。

      class line{
         Point p1,p2;
      }
      
      line l1;

      outPort.data[0] = l1.p1.x;
      outPort.data[1] = l1.p1.y;
      outPort.data[2] = l1.p2.x;
      outPort.data[3] = l1.p2.y;

受け取り側の処理の例は以下のような手順が考えられます。

      vector<vector<Point>> line_vector;
      
      if(inportIn.isNew()){
         inportIn.read();
         
         for(int i=0;i<inport.data.length();i+=4){
            vector<Point> line;
            line.pushback(Point(inport.data[i],inport.
            
      
      
      


仕様
--------
Windows7 32bit,64bit  
OpenRTM-aist-1.1.0-RELEASE（C++)  
Eclipse 3.8.1 + OpenRTM Eclipse tools 1.1.0-RC4  


RTM2014
