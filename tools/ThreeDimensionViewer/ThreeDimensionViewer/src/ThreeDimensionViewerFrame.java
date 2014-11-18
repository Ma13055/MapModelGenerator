import java.awt.*;
import java.awt.event.*;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;

import javax.swing.*;

import RTC.Point3D;

public class ThreeDimensionViewerFrame extends JFrame{

	private static final long serialVersionUID = 1L;
	
	//マウス位置
	private Point mouse_click_pos;
    private Point mouse_presse_pos;
	private Point mouse_release_pos;
	private Point mouse_dragged_pos;
//	private Point pre_pos;
	private Point dra_pos;
	
	//panelの宣言
	public CenterPanel model_panel;
	public NorthPanel button_panel;
	public TopMenue menue_bar;

	//図形情報の変数
    private ArrayList<ArrayList<Vertex>> vertices;   // 頂点列を保持する
    private ArrayList<ArrayList<Face>> faces;      // 面（三角形）列を保持する

    //モデルの生成済みを示すフラグ
    private boolean make_model_flag = false;
		
    //フレームの作成
	public ThreeDimensionViewerFrame(){
		setTitle("3DViewer");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); 
	}

	/*
	 * JMenuBar, JMenu, JMenuItem
	 */

	//メニューバーの作成
	protected void buildMenus(){
		JMenuBar bar = new JMenuBar();
		menue_bar = new TopMenue("File(F)");
		bar.add(menue_bar);
		setJMenuBar(bar);
	}
	
	//メニューバーの詳細設定
	class TopMenue extends JMenu implements ActionListener{
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		TopMenue(String name){
			this.setText(name);
			this.setMnemonic(KeyEvent.VK_F);
			
			//ファイルを開く処理
			JMenuItem i1 = new JMenuItem("FileOpen(O)");
			this.add(i1);
			i1.setMnemonic(KeyEvent.VK_O);
			i1.addActionListener(this);
			i1.setActionCommand("open");
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String cmd = e.getActionCommand();
			
			//ファイルを開く処理により特定のファイルが指定されたとき
			if(cmd.equals("open")){
				JFileChooser fc = new JFileChooser();
				fc.setMultiSelectionEnabled(true);
				
				int selected = fc.showOpenDialog(this);
				if (selected == JFileChooser.APPROVE_OPTION){
					
					//ファイルの読み込み
					File[] files = fc.getSelectedFiles();
					
					readModelData(files);
				}
			}
		}
		
		public void readModelData(File[] files){
			
			//モデルデータの初期化
			vertices = new ArrayList<ArrayList<Vertex>>();
			faces = new ArrayList<ArrayList<Face>>();

			//floorセレクトのコンボボックスを初期化
			button_panel.floor.removeAllItems();
			button_panel.floor.addItem("ALL");
			//読み込んだファイルをデータに書き起こし
			for(int i=0;i<files.length;i++){
				if(files[i].exists()){
					button_panel.floor.addItem(i+1+"階");	//コンボボックスに階を追加
					model_panel.setModelData(files[i],i);	//データに書き起こし
				}
			}
		}
	}
		
	/*
	 * JPanel
	 */

	protected void buildContent(){

		Container c = getContentPane();


		//上側のUIボタンパネル
		c.add( button_panel = new NorthPanel(), BorderLayout.NORTH);


		//画像表示用のパネル
		c.add( model_panel = new CenterPanel(), BorderLayout.CENTER);

		
	}
	
	
	//上側のUIボタンパネル
	class NorthPanel extends JPanel implements ActionListener{
		private static final long serialVersionUID = 1L;
		
		//表示ボタンの宣言
		private JButton b1;
		private JTextField scale_text,position_x_text,position_y_text,position_z_text,floor_height;
		private JLabel jl1,jl2,jl3,jl4,jl5,jl6;
		public JComboBox<String> floor;

		NorthPanel(){
			//パネルの周囲に溝みたいな装飾
			this.setBorder( BorderFactory.createEtchedBorder());

			//BoxLayout でボタンやらラジオボックスやら貼る
			this.setLayout( new BoxLayout(this, BoxLayout.X_AXIS));

			//部品間の調整用スペース
			Dimension space = new Dimension(10,1);
			
			//ボタン
			b1 = new JButton("変更");
			b1.addActionListener(this);
			b1.setActionCommand("change");
			
			jl1 = new JLabel("倍率");
			jl2 = new JLabel("　　中心位置　X:");
			jl3 = new JLabel(" Y:");
			jl4 = new JLabel(" Z:");
			jl5 = new JLabel(" 階の高さ");
			jl6 = new JLabel("　表示階");
			
			scale_text = new JTextField("1", 1);
			position_x_text = new JTextField("0",1);
			position_y_text = new JTextField("0",1);
			position_z_text = new JTextField("0",1);
			floor_height = new JTextField("30",1);
			
			floor = new JComboBox<String>();
			floor.addItem("ALL");
			floor.addActionListener(this);
			
			this.add(jl1);
			this.add(scale_text);
			this.add(jl2);
			this.add(position_x_text);
			this.add(jl3);
			this.add(position_y_text);
			this.add(jl4);
			this.add(position_z_text);
			this.add(jl5);
			this.add(floor_height);
			this.add(jl6);
			this.add(floor);
			
			this.add(b1);
			this.add(Box.createRigidArea(space));
		}
		
		public void buttonScene1(){
			b1.setEnabled(true);
		}
		
		public void buttonScene2(){
			b1.setEnabled(false);
		}		

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String cmd = e.getActionCommand();
			
			//"変更"ボタンが押されたとき
			if(cmd.equals("change")){
				System.out.println("ChangePara");
				try{
					model_panel.scale = Double.valueOf(scale_text.getText());
					model_panel.center3d = new Point3D(
							Float.valueOf(position_x_text.getText()),
							Float.valueOf(position_y_text.getText()),
							Float.valueOf(position_z_text.getText()));
					model_panel.modelHeight = Double.valueOf(floor_height.getText());
					
					//変更パラメータを用いて再描画
					model_panel.movePointArray();
					model_panel.setScreenPosition();
					model_panel.repaint();					
				}catch(NumberFormatException e1){
					System.out.println("数字以外が入力されました");
				}
			}
		}
	}
	
	
	//中央画面パネルの作成
	class CenterPanel extends JPanel implements MouseMotionListener, MouseListener{

		private static final long serialVersionUID = 1L;

		//描画イメージ変数
	    Graphics g;
	    Image img;
	    public Dimension windowSize;	//画面サイズ	    
	    
	    //パネルのサイズ
	    final int WIDTH = 700;
	    final int HEIGHT = 700;

/*-------------------------------------------------------------*/
	    		
	    Point center;         // アプレットの中心座標
	    Point mouse_pos;		//マウス位置
	    public double scale;    // モデル描画時のスケール
	    double phi;			//x軸周りの回転角
	    double theta;			//y軸周りの回転角
	    public Point3D center3d;	//回転及び描画の中心座標
	    double modelHeight;	//フロアの高さ

/*-------------------------------------------------------------*/
	    
	    
		CenterPanel(){
	        addMouseMotionListener(this);
	        addMouseListener(this);

	        //マウス位置の初期化
	    	mouse_click_pos = new Point(0,0);
	        mouse_presse_pos = new Point(0,0);
	    	mouse_release_pos = new Point(0,0);
	    	mouse_dragged_pos = new Point(0,0);
//	    	pre_pos = new Point(0,0);
	    	dra_pos = new Point(0,0);
	    	
	        this.setSize(WIDTH, HEIGHT);

	        // ウィンドウサイズの取得
	        windowSize = this.getSize();
	        
	        this.g = this.getGraphics();
	        // ウィンドウの中心座標の取得
	        center = new Point(windowSize.width / 2, windowSize.height*3 / 4);
	        //マウス位置の初期化
	        mouse_pos = new Point(0,0);
	        
	        //回転角の初期化
	        phi = 0.0;
	        theta = 0.0;
	        
			center3d = new Point3D(0,0,0);
			modelHeight = 30;
			scale = 1;
		}
		
		//文字列の配列を頂点座標の配列へ変換する
		protected Vertex[] makePointArray(String[] str,int c){
			Vertex[] p = null;
			int counter=0;
			for(int i=0;i<str.length;i+=2){
				if(p == null){
					p = new Vertex[2];
				}
				else p = Arrays.copyOf(p, p.length+2);
				p[counter++] = new Vertex(Double.parseDouble(str[i]),
						c*modelHeight + 0          ,
						Double.parseDouble(str[i+1]),
						center3d);	//立体の底の座標
				p[counter++] = new Vertex(Double.parseDouble(str[i]),
						c*modelHeight + modelHeight,
						Double.parseDouble(str[i+1]),
						center3d);	//立体の屋根の座標
			}
			
			return p;
		}
		
		//中心座標が変更された時に、その変更を反映させる
		public void movePointArray(){
			for(int i=0;i<vertices.size();i++){
				for(int j=0;j<vertices.get(i).size();j++){
					vertices.get(i).get(j).move(center3d);
				}
			}
		}

		
        // モデルデータの設定
		public void setModelData(File f,int c){
			//保持変数が作られていない場合は初期化
			if(vertices == null) vertices = new ArrayList<ArrayList<Vertex>>();
			if(faces == null) faces = new ArrayList<ArrayList<Face>>();
			
			//一フロア分の頂点と面データを保持する変数
			ArrayList<Vertex> sub_vertices = new ArrayList<Vertex>();	// 頂点列を初期化
			ArrayList<Face> sub_faces = new ArrayList<Face>();	// 面列を初期化

			BufferedReader br;
			try {
				br = new BufferedReader(new FileReader(f));	//ファイルを読み込み
				String str = br.readLine();					//一行読み込み
				while(str != null){							//ファイルの終わりじゃないとき
					Vertex[] verArray = makePointArray(str.split(",", 0),c).clone();	//一行の文字列を頂点の配列へ変換
					
					int begin_vect = 0;
					if(sub_vertices != null)begin_vect = sub_vertices.size();	//頂点群の末尾の番号を控える
					Face bottom_face = new Face();
					Face top_face = new Face();
					
					for(int j=0;j<verArray.length;j++){	//頂点配列を網羅
						sub_vertices.add(verArray[j]);	//一フロアの頂点を保持する変数に頂点を加える

						
						//現在処理している頂点が、図形の上底か下底かを判別
						if(j%2==0){
							bottom_face.add(begin_vect+j);
							
							//下底なら、側面のデータも作成する
							Face side_face = new Face();
							side_face.add(begin_vect+j);
							side_face.add(begin_vect+j+1);
							side_face.add(begin_vect+((j+3)%verArray.length));
							side_face.add(begin_vect+((j+2)%verArray.length));
							sub_faces.add(side_face);
						}else{
							top_face.add(begin_vect+j);
						}
					}
					//作成した面データを保持
					sub_faces.add(bottom_face);
					sub_faces.add(top_face);
	
					str = br.readLine();
				}
				
				//作成した面データ群と頂点データ群を保持
				vertices.add(sub_vertices);
				faces.add(sub_faces);
				br.close();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			//モデルを作成したフラグを立てる
			make_model_flag = true;
			//モデルの描画処理
			setScreenPosition();
			repaint();
		}
					
        // 頂点スクリーン座標の設定
	 	public void setScreenPosition(){
	 		if(make_model_flag == false) return;
	 		for(int k=0;k<vertices.size();k++){
		 		for(int i=0;i<vertices.get(k).size();i++){
		 			Vertex v = (Vertex) vertices.get(k).get(i);
		 			
		 			//回転後の座標値の算出
		 			v.rx = v.mx*Math.cos(theta) + v.mz*Math.sin(theta);
		 			v.ry = v.mx*Math.sin(phi)*Math.sin(theta) + v.my*Math.cos(phi) - v.mz*Math.sin(phi)*Math.cos(theta);
		 			v.rz = - v.mx*Math.cos(phi)*Math.sin(theta) + v.my*Math.sin(phi) + v.mz*Math.cos(phi)*Math.cos(theta);
		 			
		 			//スクリーン座標の算出
		 			v.screenX = (int)(center.x + scale*v.rx);
		 			v.screenY = (int)(center.y/2 - scale*v.ry);
		 		}
	 		}
	 	}
	 	
	 	//モデルの描画
	 	private void drawModel(Graphics g){
	 		if(make_model_flag == false) return;
	 		
	 	    // 描画色を白に設定
	 	    g.setColor(Color.white);
	 	    g.fillRect(0, 0, windowSize.width, windowSize.height);
	 	    
 	        // 面の輪郭線の描画
 	        g.setColor(Color.black);

	 	    // 各面の描画
 	        int floor_num = button_panel.floor.getSelectedIndex();

 	        int start_num,end_num;
 	        if(floor_num == 0){
 	        	start_num = 0; end_num = faces.size();
 	        }else{
 	        	start_num = floor_num-1; end_num = floor_num;
 	        }
 	        
 	        for(int k = start_num; k < end_num; k++){
		 	    for(int i = 0; i < faces.get(k).size(); i++) {
		 	    	Face face = faces.get(k).get(i);
		 	
		 	        for(int j = 0; j < face.size(); j++){
		 	        	Vertex start = vertices.get(k).get(face.v[j]);
		 	        	Vertex end = vertices.get(k).get(face.v[(j + 1) % face.size()]);
		 	        	g.drawLine(start.screenX, start.screenY, end.screenX, end.screenY);
		 	        }
		 	    }
 	        }
	 	}

	 	
		@Override
		public void mouseClicked(MouseEvent arg0) {
			// TODO Auto-generated method stub
			mouse_click_pos.setLocation(arg0.getX(), arg0.getY());
//			System.out.println("CLICKED : " + mouse_click_pos.x + "," + mouse_click_pos.y);	
		}

		@Override
		public void mouseEntered(MouseEvent arg0) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void mouseExited(MouseEvent arg0) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void mousePressed(MouseEvent arg0) {
			// TODO Auto-generated method stub
			//マウス位置の更新
			mouse_presse_pos.setLocation(arg0.getX(), arg0.getY());
//			pre_pos = mouse_presse_pos.getLocation();
			
			//マウス位置の更新
			mouse_pos = mouse_presse_pos.getLocation();
		}



		@Override
		public void mouseReleased(MouseEvent arg0) {
			// TODO Auto-generated method stub
			mouse_release_pos.setLocation(arg0.getX(), arg0.getY());
		}

		@Override
		public void mouseDragged(MouseEvent arg0) {	
			// TODO Auto-generated method stub
			mouse_dragged_pos.setLocation(arg0.getX(),arg0.getY());
			dra_pos = mouse_dragged_pos.getLocation();
			
			//回転角の更新
			theta += (dra_pos.getX() - mouse_pos.x)*0.01;
			phi   += (dra_pos.getY() - mouse_pos.y)*0.01;
			
			//x軸周りの回転角に上限を設定
			phi = Math.min(phi, Math.PI/2);
			phi = Math.max(phi, -Math.PI/2);
			
			//マウス位置の更新
			mouse_pos = mouse_dragged_pos.getLocation();
			
			setScreenPosition();
			repaint();
		}

		@Override
		public void mouseMoved(MouseEvent arg0) {
			// TODO Auto-generated method stub
		}
		
		@Override
		public void paint(Graphics g) {
			
	 		//ダブルバッファリング用のイメージを作成
	 		if(img == null){
	 			img = createImage(windowSize.width,windowSize.height);
	 		}

	 		//バッファにモデルを描画
	 		drawModel(img.getGraphics());
	 		//バッファイメージをアプレットに描画
	 		g.drawImage(img, 0, 0, this);
		}

	}	
	
	//面クラス
	public class Face {
		public int[] v;

		public Face(){
			v=null;
		}
		
		public Face(int[] v_array){
			v = v_array.clone();
		}
		
		public void add(int v0){
			if(v==null){
				v = new int[1];
			}else{
				v = Arrays.copyOf(v, v.length+1);
			}
			v[v.length-1] = v0;
		}
		
		public int size(){
			return v.length;
		}
	}

	//頂点クラス
	public class Vertex{
		public double x, y, z; // モデルの頂点座標
		public double mx,my,mz;
		public double MathX,MathY,MathZ;
		public double rx, ry, rz;	//回転させた後の座標
		public int screenX, screenY;	//スクリーン上の座標

		public Vertex(double x,double y,double z) {
		    this.x = x;
		    this.y = y;
		    this.z = z;
		    this.mx = x;
		    this.my = y;
		    this.mz = z;
		}

		public Vertex(double x,double y,double z,Point3D m_p) {
		    this.x = x;
		    this.y = y;
		    this.z = z;
		    this.mx = x + m_p.x;
		    this.my = y + m_p.y;
		    this.mz = z + m_p.z;
		}
		
		public void move(Point3D m_p){
			this.mx = x + m_p.x;
			this.my = y + m_p.y;
			this.mz = z + m_p.z;
		}
			
		public void print(){
			System.out.println("["+x+","+y+","+z+"]");
		}
	}
}
