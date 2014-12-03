import java.awt.*;
import java.awt.event.*;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import javax.swing.*;

import RTC.Point2D;

public class tweakerOfLineMapsFrame extends JFrame{

	private static final long serialVersionUID = 1L;
		
	//panelの宣言
	public CenterPanel model_panel;
	public NorthPanel button_panel;
	public TopMenue menue_bar;
	public MapDialog dialog_panel;

	//図形情報の変数
    private ArrayList<ArrayList<ArrayList<Point>>> vertices;   // 頂点列を保持する

    //モデルの生成済みを示すフラグ
    private boolean make_model_flag = false;
    
    public String open_folder_path = "";
    public String save_file_path = "";
		
    //フレームの作成
	public tweakerOfLineMapsFrame(){
		setTitle("tweaker");
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
			open_folder_path = "";
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
				JFileChooser fc = new JFileChooser("C:\\tmp\\makeMapsModel\\maps\\linemaps");
				fc.setMultiSelectionEnabled(true);
				
				int selected = fc.showOpenDialog(this);
				if (selected == JFileChooser.APPROVE_OPTION){
					
					//ファイルの読み込み
					File[] files = fc.getSelectedFiles();
					open_folder_path = files[0].getParent();
					
					readModelData(files);
				}
			}
		}
		
		public void readModelData(File[] files){
			
			//モデルデータの初期化
			vertices = new ArrayList<ArrayList<ArrayList<Point>>>();

			//floorセレクトのコンボボックスを初期化
			button_panel.floor.removeAllItems();
			//読み込んだファイルをデータに書き起こし
			for(int i=0;i<files.length;i++){
				if(files[i].exists()){
					button_panel.floor.addItem(i+1+"階");	//コンボボックスに階を追加
					button_panel.floor.setSelectedIndex(0);
					model_panel.setModelData(files[i]);	//データに書き起こし
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
		c.add( model_panel = new CenterPanel(false), BorderLayout.CENTER);

		
	}
	
	
	//上側のUIボタンパネル
	class NorthPanel extends JPanel implements ActionListener,ItemListener{
		private static final long serialVersionUID = 1L;
		
		//表示ボタンの宣言
		private JButton b1,b2,b3,b4,b5;
		private JTextField offset_x_text,offset_y_text;
		private JLabel jl1,jl2,jl3,jl4,jl5,jl6;
		public JComboBox<String> floor;
		
		public boolean select_save_flag;
		public boolean make_save_path;


		NorthPanel(){
			select_save_flag = false;
			make_save_path = false;
			
			//パネルの周囲に溝みたいな装飾
			this.setBorder( BorderFactory.createEtchedBorder());

			//BoxLayout でボタンやらラジオボックスやら貼る
			this.setLayout( new BoxLayout(this, BoxLayout.X_AXIS));

			//部品間の調整用スペース
			Dimension space = new Dimension(10,1);
			
			//ボタン
			b1 = new JButton("階変更");
			b1.addActionListener(this);
			b1.setActionCommand("floor_change");

			b2 = new JButton("変更");
			b2.addActionListener(this);
			b2.setActionCommand("offset_change");

			b3 = new JButton("初期化");
			b3.addActionListener(this);
			b3.setActionCommand("offset_initialize");
			
			b4 = new JButton("位置調節");
			b4.addActionListener(this);
			b4.setActionCommand("position_change");
			
			b5 = new JButton("保存");
			b5.addActionListener(this);
			b5.setActionCommand("save");			

			jl1 = new JLabel("：オフセット");
			jl2 = new JLabel("X");
			jl3 = new JLabel("Y");
			jl4 = new JLabel("：表示階");
			jl5 = new JLabel(":");
			jl6 = new JLabel(":");
			
			offset_x_text = new JTextField("0",1);
			offset_y_text = new JTextField("0",1);
			
			floor = new JComboBox<String>();
			floor.addItemListener(this);
			
			
			this.add(b1);
			this.add(jl1);
			this.add(b3);
			this.add(b2);
			this.add(jl2);
			this.add(offset_x_text);
			this.add(jl3);
			this.add(offset_y_text);
			this.add(jl4);
			this.add(floor);
			this.add(jl5);
			this.add(b4);
			this.add(jl6);
			this.add(b5);
			
			this.add(Box.createRigidArea(space));
			
			buttonScene4();
		}
		
		public void buttonScene1(){
			b1.setEnabled(true);
			b2.setEnabled(true);
			b3.setEnabled(true);
			b4.setEnabled(true);
			b5.setEnabled(true);
		}
		
		public void buttonScene2(){
			b1.setEnabled(true);
			b2.setEnabled(true);
			b3.setEnabled(true);
			b4.setEnabled(false);
			b5.setEnabled(true);
		}
		
		public void buttonScene3(){
			b1.setEnabled(false);
			b2.setEnabled(true);
			b3.setEnabled(true);
			b4.setEnabled(false);
			b5.setEnabled(true);
		}
		
		public void buttonScene4(){
			b1.setEnabled(false);
			b2.setEnabled(false);
			b3.setEnabled(false);
			b4.setEnabled(false);
			b5.setEnabled(false);
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String cmd = e.getActionCommand();

			if(make_model_flag){
				//"階変更"ボタンが押されたとき
				if(cmd.equals("floor_change")){
					new FloorDialog(vertices.size());
					
				//"オフセットの変更"ボタンが押されたとき
				}else if(cmd.equals("offset_change")){
					try{
						model_panel.offset = new Point(
								Integer.valueOf(offset_x_text.getText()),
								Integer.valueOf(offset_y_text.getText()));
						
						//変更パラメータを用いて再描画
						model_panel.movePointArray();
						model_panel.repaint();					
					}catch(NumberFormatException e1){
						System.out.println("数字以外が入力されました");
					}
					
				//"オフセットの初期化"ボタンが押されたとき
				}else if(cmd.equals("offset_initialize")){
					Point tl = model_panel.getTopLeftPoint(floor.getSelectedIndex());
					model_panel.offset = new Point(-tl.x,-tl.y);
					model_panel.movePointArray();
					model_panel.repaint();
					
				//"位置調整"ボタンが押されたとき
				}else if(cmd.equals("position_change")){
					if(floor.getSelectedIndex() != 0){
						dialog_panel = new MapDialog(floor.getSelectedIndex()-1);
					}

				//"保存"ボタンが押されたとき
				}else if(cmd.equals("save")){
					JFileChooser filechooser = new JFileChooser(open_folder_path);
	
				    int selected = filechooser.showSaveDialog(this);
				    if (selected == JFileChooser.APPROVE_OPTION){
						File file = filechooser.getSelectedFile();
						String str = file.getName();
						save_file_path = "";
						select_save_flag = true;
				      
				      
						for( int i = 0 ; i < vertices.size() ; i++ ){
							if(i != 0) save_file_path += ",";
							String filePath = "";
							if(i < 10) 	filePath = file.getParent() + "\\" + str + "0" + i;
							else 			filePath = file.getParent() + "\\" + str + i;
							File f = new File(filePath);
							try {
								if(!file.exists())f.createNewFile();
								PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(f)));
								for(int j = 0;j < vertices.get(i).size(); j++){
									String write_str = "";
									for(int k = 0;k < vertices.get(i).get(j).size(); k++){
										write_str += vertices.get(i).get(j).get(k).x + "," +
														vertices.get(i).get(j).get(k).y;
										if(k < vertices.get(i).get(j).size()-1)
											write_str += ",";
									}
									pw.println(write_str);
								}
								pw.close();
							} catch (IOException e1) {
								// TODO Auto-generated catch block
								e1.printStackTrace();
							}
							save_file_path += filePath;
						}
						make_save_path = true;
				    }
				}
			}
		}

		@Override
		public void itemStateChanged(ItemEvent arg0) {
			// TODO Auto-generated method stub
			if(arg0.getStateChange() == ItemEvent.SELECTED){
				model_panel.repaint();
			}
		}
		
	}
	
	//階挿入移動のときのポップアップウィンドウ
	class FloorDialog extends JDialog implements ActionListener{

		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		private JButton transpose = new JButton("入れ替え");
		private JComboBox<String> floor_list;
		
		FloorDialog(int floor_num){
			setLayout(new FlowLayout());
			setModal(true);
			JPanel panel1 = new JPanel();
			floor_list = new JComboBox<String>();
			floor_list.addItem("1階の下");
			for(int i=0;i<floor_num-1;i++){
				floor_list.addItem((i+1)+"階と"+(i+2)+"階の間");	//コンボボックスに階を追加
			}
			floor_list.addItem(floor_num+"階の上");
			floor_list.setSelectedIndex(0);
			panel1.add(new JLabel("どの位置に挿入しますか？"));
			panel1.add(floor_list);
			add(panel1);
			JPanel panel2 = new JPanel();
			panel2.add(transpose);
			add(panel2);
			transpose.addActionListener(this);
			this.pack();
			setVisible(true);
		}
		
		@Override
		public void actionPerformed(ActionEvent arg0) {
			// TODO Auto-generated method stub
			if(arg0.getSource() == transpose){
				model_panel.transposeArray(floor_list.getSelectedIndex());
				model_panel.repaint();
			}

			setVisible(false);
		}
	}
	
	
	//階挿入移動のときのポップアップウィンドウ
	class MapDialog extends JDialog{

		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		CenterPanel map_panel;
		
		MapDialog(int floor_num){
			map_panel = new CenterPanel(true);
			add(map_panel);
			this.setSize(model_panel.getSize());
			setVisible(true);
			
			
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
	    private Point mouse_click_pos;		//マウス位置
	    public double scale;    // モデル描画時のスケール
	    double phi;			//x軸周りの回転角
	    double theta;			//y軸周りの回転角
	    public Point offset;	//回転及び描画の中心座標
	    double modelHeight;	//フロアの高さ
	    
	    private boolean dialog_flag;
	    private int step;
	    
	    public Point[] bot_v;
	    public Point[] top_v;
	    
	    public boolean set_top_flag;
	    

/*-------------------------------------------------------------*/
	    
	    
		CenterPanel(boolean flag){
	        addMouseMotionListener(this);
	        addMouseListener(this);

	        //マウス位置の初期化
	    	mouse_click_pos = new Point(0,0);
	    	
	        this.setSize(WIDTH, HEIGHT);

	        // ウィンドウサイズの取得
	        windowSize = this.getSize();
	        
	        this.g = this.getGraphics();
	        // ウィンドウの中心座標の取得
	        center = new Point(windowSize.width / 2, windowSize.height*3 / 4);
	        //マウス位置の初期化
	        mouse_click_pos = new Point(0,0);
	        
	        //回転角の初期化
	        phi = 0.0;
	        theta = 0.0;
	        
			offset = new Point(0,0);
			modelHeight = 30;
			scale = 1;
			
			dialog_flag = flag;
			step = 0;
			
			bot_v = new Point[3];
			top_v = new Point[3];
			
			set_top_flag = false;
		}
		
		//文字列の配列を頂点座標の配列へ変換する
		protected ArrayList<Point> makePointArray(String[] str){
			ArrayList<Point> p = new ArrayList<Point>();
			for(int i=0;i<str.length;i+=2)
				p.add(new Point(Integer.valueOf(str[i]),Integer.valueOf(str[i+1])));	//頂点の座標
			return p;
		}
		
		//中心座標が変更された時に、その変更を反映させる
		public void movePointArray(){
			int floor_num = button_panel.floor.getSelectedIndex();
			for(int i=0;i<vertices.get(floor_num).size();i++){
				for(int j=0;j<vertices.get(floor_num).get(i).size();j++){
					vertices.get(floor_num).get(i).get(j).x += offset.x;
					vertices.get(floor_num).get(i).get(j).y += offset.y;
				}
			}
		}
		
		//現在表示されている階を指定された階層へ移動させる
		public void transposeArray(int insert_floor){
			ArrayList<ArrayList<ArrayList<Point>>> sub_vertices = new ArrayList<ArrayList<ArrayList<Point>>>();
			int floor_num = button_panel.floor.getSelectedIndex();
			for(int i=0;i<=vertices.size();i++){
				if(insert_floor == i){
					System.out.println("現在の表示階");
					sub_vertices.add(vertices.get(floor_num));
				}
				if(floor_num != i && i < vertices.size()){
					System.out.println((i+1)+"階");
					sub_vertices.add(vertices.get(i));
				}
			}
			vertices = sub_vertices;
		}

		
        // モデルデータの設定
		public void setModelData(File f){
			//保持変数が作られていない場合は初期化
			if(vertices == null) vertices = new ArrayList<ArrayList<ArrayList<Point>>>();
			
			//一フロア分の頂点と面データを保持する変数
			ArrayList<ArrayList<Point>> sub_vertices = new ArrayList<ArrayList<Point>>();	// 頂点列を初期化

			BufferedReader br;
			try {
				br = new BufferedReader(new FileReader(f));	//ファイルを読み込み
				String str = br.readLine();					//一行読み込み
				while(str != null){							//ファイルの終わりじゃないとき
					//一行分の頂点を保持する変数
					ArrayList<Point> verArray = makePointArray(str.split(",", 0));	//一行の文字列を頂点の配列へ変換

					//作成した一行分のデータを一つの輪郭として頂点データ群に保持
					sub_vertices.add(verArray);

					//次の行を読み込む
					str = br.readLine();
				}
				
				//作成した面データ群と頂点データ群を保持
				vertices.add(sub_vertices);
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
			repaint();
		}
		
		//指定された階の座標群の中から、最も左上に位置する座標を抽出する
		//また、左上の点が存在しない場合は、疑似的にもっとも左上となる点を作成する
		private Point getTopLeftPoint(int floor_num){
			Point tl = null;
			for(int i = 0; i < vertices.get(floor_num).size(); i++) {
	 	    	ArrayList<Point> object_vertices = vertices.get(floor_num).get(i);
	 	        for(int j = 0; j < object_vertices.size(); j++){
	 	        	Point p = object_vertices.get(j);
	 	        	if(tl == null)tl = (Point) p.clone();
	 	        	if(tl.x > p.x)tl.x = p.x;
	 	        	if(tl.y > p.y)tl.y = p.y;
	 	        }
	 	    }
			return tl;
		}
		
		private int getSelectFloor(){
			int floor_num = button_panel.floor.getSelectedIndex();
			if(dialog_flag) floor_num -= 1;
			return floor_num;
		}

		private double lengthLine(Point p1,Point p2){
			double length = Math.sqrt(Math.pow((p1.x - p2.x),2) + Math.pow((p1.y - p2.y),2));			
			return length;
		}
		
		private Point getClickVertices(Point click_p){
			
 	        int floor_num = getSelectFloor();
 	        
 	        Point tl_point = getTopLeftPoint(floor_num);
 	        Point err_tl = new Point(tl_point.x - 10 , tl_point.y - 10);
			ArrayList<ArrayList<Point>> draw_floor = vertices.get(floor_num);
 	        
			Point p2 = new Point(click_p.x + err_tl.x,click_p.y + err_tl.y);
			Point near_p = new Point();
			double length = -1;

			for(int i = 0;i<draw_floor.size();i++){
				for(int j = 0;j<draw_floor.get(i).size();j++){
					Point p1 = new Point(draw_floor.get(i).get(j));
					double l = lengthLine(p1,p2);
					
					if((length < 0) || (length > l)){
						length = l;
						near_p.setLocation(p1);
					}
				}
			}
			
			return near_p;
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
 	        int floor_num = getSelectFloor();
 	        
 	        Point tl_point = getTopLeftPoint(floor_num);
 	        Point err_tl = new Point(tl_point.x - 10 , tl_point.y - 10);

	 	    for(int i = 0; i < vertices.get(floor_num).size(); i++) {
	 	    	ArrayList<Point> object_vertices = vertices.get(floor_num).get(i);
	 	        for(int j = 0; j < object_vertices.size(); j++){
	 	        	Point start = (Point) object_vertices.get(j).clone();
	 	        	start.x -= err_tl.x; start.y -= err_tl.y;
	 	        	Point end = (Point) object_vertices.get((j + 1) % object_vertices.size()).clone();
	 	        	end.x -= err_tl.x; end.y -= err_tl.y;
	 	        	g.drawLine(start.x, start.y, end.x, end.y);
	 	        }
	 	    }
 	        g.setColor(Color.red);
	 	    g.drawOval(-err_tl.x - 2, -err_tl.y - 2, 4, 4);
	 	    
	 	    for(int i = 0;i < bot_v.length ; i++){
	 	    	g.setColor(Color.blue);
	 	    	if(bot_v[i] != null) g.drawOval(bot_v[i].x - err_tl.x - 2, bot_v[i].y - err_tl.y - 2, 4, 4);
	 	    	g.setColor(Color.magenta);
	 	    	if(top_v[i] != null) g.drawOval(top_v[i].x - err_tl.x - 2, top_v[i].y - err_tl.y - 2, 4, 4);
	 	    }
	 	    
	 	    if(floor_num > 0)button_panel.buttonScene1();
	 	    else if(floor_num == 0 && vertices.size() > 1)button_panel.buttonScene2();
	 	    else button_panel.buttonScene3();
	 	}
	 	
	 	private void adjustPoint(){
	 		Point[] top_err = new Point[2];
	 		Point[] bot_err = new Point[2];
	 		Point2D[] s_err = new Point2D[2];
	 		Point2D scale_err = new Point2D();
	 		Point position_err = new Point();
	 		
	 		for(int i=0;i<top_err.length;i++){
	 			System.out.println(top_v[i] + " : " + top_v[i+1]);
	 			System.out.println(bot_v[i] + " : " + bot_v[i+1]);
	 			top_err[i] = new Point(top_v[i].x-top_v[i+1].x,top_v[i].y-top_v[i+1].y);
	 			bot_err[i] = new Point(bot_v[i].x-bot_v[i+1].x,bot_v[i].y-bot_v[i+1].y);
	 			if(bot_err[i].x == 0 && bot_err[i].y == 0) s_err[i] = new Point2D(1,1);
	 			else if(bot_err[i].x == 0)s_err[i] = new Point2D(1,bot_err[i].y/top_err[i].y);
	 			else if(bot_err[i].y == 0)s_err[i] = new Point2D(bot_err[i].x/top_err[i].x,1);
	 			else s_err[i] = new Point2D((double)bot_err[i].x/(double)top_err[i].x,(double)bot_err[i].y/(double)top_err[i].y);
	 		}
	 		
	 		scale_err = s_err[0];
	 		if(scale_err.x == 1 || scale_err.x == 0) scale_err.x = s_err[1].x;
	 		if(scale_err.y == 1 || scale_err.y == 0) scale_err.y = s_err[1].y;
	 		System.out.println(scale_err.x +","+scale_err.y);
	 		
	 		position_err = new Point(top_v[0].x - bot_v[0].x,top_v[0].y - bot_v[0].y);
	 		
	 		int floor_num = getSelectFloor();
			for(int i=0;i<vertices.get(floor_num).size();i++){
				for(int j=0;j<vertices.get(floor_num).get(i).size();j++){
					vertices.get(floor_num).get(i).get(j).x = (int) (vertices.get(floor_num).get(i).get(j).x*scale_err.x) - position_err.x;
					vertices.get(floor_num).get(i).get(j).y = (int) (vertices.get(floor_num).get(i).get(j).y*scale_err.y) - position_err.y;
				}
			}
			

	 	}

	 	private void getAdjustmentPoint(){
	 		if(dialog_flag)bot_v[step++] = getClickVertices(mouse_click_pos);
			else top_v[step++] = getClickVertices(mouse_click_pos);
			repaint();
			if(step == 3){
			    String selectvalues[] = {"はい", "やり直し", "キャンセル"};

			    int select = JOptionPane.showOptionDialog(this,
			      "選択された頂点でよろしいですか？", 
			      "階下頂点選択", 
			      JOptionPane.YES_NO_CANCEL_OPTION,
			      JOptionPane.QUESTION_MESSAGE,
			      null, 
			      selectvalues, 
			      selectvalues[0]
			    );
			    
			    if(select == JOptionPane.CLOSED_OPTION || select == 2){
					if(dialog_flag)dialog_panel.setVisible(false);
					else{
				    	step = 0;
				    	top_v = new Point[3];
				    	bot_v = new Point[3];
				    	set_top_flag = false;
				    	repaint();
					}
			    }else if(select == 1){
			    	if(dialog_flag)bot_v = new Point[3];
			    	else top_v = new Point[3];
			    	step = 0;
			    	repaint();
			    }else{
			    	if(dialog_flag){
				    	model_panel.bot_v = bot_v.clone();
						model_panel.set_top_flag = true;
						model_panel.repaint();
				    	dialog_panel.setVisible(false);
			    	}else{
			    		adjustPoint();
			    		
			    		repaint();
			    		
				    	step = 0;
				    	top_v = new Point[3];
				    	bot_v = new Point[3];
				    	set_top_flag = false;
			    	}
			    }
			}	 		
	 	}
	 	
		@Override
		public void mouseClicked(MouseEvent arg0) {
			// TODO Auto-generated method stub
			mouse_click_pos.setLocation(arg0.getX(), arg0.getY());
			
			if(dialog_flag || set_top_flag){
				getAdjustmentPoint();
			}
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
		}

		@Override
		public void mouseReleased(MouseEvent arg0) {
			// TODO Auto-generated method stub
		}

		@Override
		public void mouseDragged(MouseEvent arg0) {	
			// TODO Auto-generated method stub
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
	
	
}
