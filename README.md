MapModelGenerator
=================
ここでは、我々が日常で使用する地図画像を元に、  
プログラム上で使用する地図モデルデータへと変換するコンポーネント群を公開しています。  


概要
--------
本コンポーネント群は、理論設計をSysMLで行い、機能を分解しています。  
変換システムの大枠での内部ブロック図としましては  
* UserInterface  
* OpenImage  
* GenerateMapModels

に分かれております。  
![SysML01](http://www.sic.shibaura-it.ac.jp/~ma13055/内部ブロック図_地図モデル生成)
![SysML02](http://www.sic.shibaura-it.ac.jp/~ma13055/内部ブロック図_地図モデル変換)
![SysML03](http://www.sic.shibaura-it.ac.jp/~ma13055/内部ブロック図_正規マップ変換)
![SysML04](http://www.sic.shibaura-it.ac.jp/~ma13055/内部ブロック図_ラインマップ変換)

ファイル構成
--------
機能ごと、特に再利用性のレベルにファイル分解して公開をさせていただいております。  
MapModelGenerator  
|-OpenCV_Comps  
|-tools  
--MakeLineMaps

* OpenCV_Comps
    * OpenCVの関数をコンポーネント化した、再利用性の高いコンポーネント
* tools
    * UserInterface, OpenImageと、  
その他簡易地図生成に特化して作られた再利用性の低いコンポーネント
* MakeLineMaps
    * C++で作成されているコンポーネント群のrtcdと、  
その他toolsのJAVAコンポーネント群のJar、  
ラインマップ生成に用いるコンポーネント群の自動ポート接続を行うrtshellが入っています


仕様
--------
Windows7 32bit,64bit  
OpenRTM-aist-1.1.0-RELEASE（C++,Java版)  
Eclipse 3.8.1 + OpenRTM Eclipse tools 1.1.0-RC4


RTM2014
