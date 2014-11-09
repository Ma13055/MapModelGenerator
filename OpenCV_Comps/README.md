OpenCV_Comps
=================
このフォルダでは、OpenCVの関数をコンポーネント化したもとを提供しています。 


概要
--------
提供コンポーネントは以下の関数のコンポーネントです。
* CVCanny - エッジ検出コンポーネント
   * 
* OpenImage  
* GenerateMapModels  
に分かれております。

ファイル構成
--------
機能ごと、特に再利用性のレベルにファイル分解して公開をさせていただいております。  
MapModelGenerator  
|-OpenCV_Comps  
|-tools  

* OpenCV_Comps
    * OpenCVの関数をコンポーネント化した、再利用性の高いコンポーネント
* tools
    * UserInterface, OpenImageと、  
その他簡易地図生成に特化して作られた再利用性の低いコンポーネント


仕様
--------
Windows7 32bit,64bit  
OpenRTM-aist-1.1.0-RELEASE（C++,Java版)  
Eclipse 3.8.1 + OpenRTM Eclipse tools 1.1.0-RC4


RTM2014
