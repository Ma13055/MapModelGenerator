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
import java.util.Arrays;

import javax.swing.*;

public class makeGridMapFrame extends JFrame{

	private static final long serialVersionUID = 1L;
		
	//panelの宣言
	public CenterPanel model_panel;
	public NorthPanel button_panel;
	public TopMenue menue_bar;
	public AddStatusDialog dialog_panel;

	//図形情報の変数
    private ArrayList<ArrayList<ArrayList<Point>>> vertices;   // 頂点列を保持する
    //グリッドマップに対するステータスを保持する変数
    private gridStatus[][] grid_status;
    //カテゴリーを保持する変数
    private String[] category = {};
    //グリッドマップを保持する変数
    private gridData[][][] grid_data;
    
    //読み込んだファイルの名前を保持しておく変数
    private ArrayList<String> file_names;

    //モデルの生成済みを示すフラグ
    private boolean make_model_flag = false;
    private boolean change_status = false;
    
    public String open_folder_path = "";
    public String save_file_path = "";
    public String save_folder_path = "C:\\tmp\\makeMapsModel\\maps\\gridmaps\\";
		
    //フレームの作成
	public makeGridMapFrame(){
		setTitle("makeGridStatus");
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
		
		public void setGridData(int floor,Point tl,Point br){
			for(int y=0,a=0;y<br.y-tl.y;y+=3,a++){
				for(int x=0,b=0;x<br.x-tl.x;x+=3,b++){
					int maxIndex = model_panel.clickFigure(floor,new Point(x+10,y+10));
					if(maxIndex >= 0){
						if(grid_status[floor][maxIndex].throw_flag) grid_data[floor][a][b].throw_grid = 0;
						else grid_data[floor][a][b].throw_grid = 1;
						grid_data[floor][a][b].status_grid = grid_status[floor][maxIndex].object_num;
					}else{
						grid_data[floor][a][b].throw_grid = -1;
						grid_data[floor][a][b].status_grid = -1;
					}
				}
			}
		}

		
		public void readModelData(File[] files){
			//モデルデータの初期化
			vertices = new ArrayList<ArrayList<ArrayList<Point>>>();
			file_names = new ArrayList<String>();

			//floorセレクトのコンボボックスを初期化
			button_panel.floor.removeAllItems();
			//読み込んだファイルをデータに書き起こし
			for(int i=0;i<files.length;i++){
				if(files[i].exists()){
					button_panel.floor.addItem(i+1+"階");	//コンボボックスに階を追加
					button_panel.floor.setSelectedIndex(0);
					model_panel.setModelData(files[i]);	//データに書き起こし
					file_names.add(files[i].getName());
				}
			}
			
			grid_status = new gridStatus[vertices.size()][];
			for(int i=0;i<grid_status.length;i++){
				grid_status[i] = new gridStatus[vertices.get(i).size()];
				for(int j=0;j<grid_status[i].length;j++){
					grid_status[i][j] = new gridStatus(j);
				}
			}
						
			grid_data = new gridData[vertices.size()][][];
			for(int i=0;i<grid_data.length;i++){
				Point tl = model_panel.getTopLeftPoint(i);
				Point br = model_panel.getBottomRightPoint(i);
				
				grid_data[i] = new gridData[(br.y - tl.y)/3 + 1][(br.x - tl.x)/3 + 1];

				for(int j=0;j<grid_data[i].length;j++){
					for(int k=0;k<grid_data[i][j].length;k++){
						grid_data[i][j][k] = new gridData();
					}
				}
				setGridData(i,tl,br);
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
	class NorthPanel extends JPanel implements ActionListener,ItemListener{
		private static final long serialVersionUID = 1L;
		
		//表示ボタンの宣言
		private JButton b1,b2;
		private JLabel jl1;
		public JComboBox<String> floor;
		public JCheckBox ckbox1;
		public JRadioButton radio1,radio2,radio3,radio4,radio5;
		public ButtonGroup group1,group2;
		
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
			b1 = new JButton("読み込み");
			b1.addActionListener(this);
			b1.setActionCommand("read_status");

			b2 = new JButton("保存");
			b2.addActionListener(this);
			b2.setActionCommand("save_status");
			
			radio1 = new JRadioButton("表示階");
			radio2 = new JRadioButton("全体");
			group1 = new ButtonGroup();
			group1.add(radio1);
			group1.add(radio2);
			radio1.setSelected(true);
			jl1 = new JLabel(" | 表示階");
			
			floor = new JComboBox<String>();
			floor.addItemListener(this);
			
			ckbox1 = new JCheckBox("Status",true);
			ckbox1.addItemListener(this);
			
			radio3 = new JRadioButton("AddStatus");
			radio4 = new JRadioButton("GridMap1");
			radio5 = new JRadioButton("GridMap2");
			radio3.addActionListener(this);
			radio4.addActionListener(this);
			radio5.addActionListener(this);
			group2 = new ButtonGroup();
			
			group2.add(radio3);
			group2.add(radio4);
			group2.add(radio5);
			radio3.setSelected(true);
			
			this.add(radio1);	//◎表示階
			this.add(radio2);	//◎全体
			this.add(b1);		//【読み込み】
			this.add(b2);		//【保存】
			this.add(jl1);		// | 表示階
			this.add(floor);	//【】▽
			this.add(ckbox1);	//Status
			this.add(radio3);	//◎AddStatus
			this.add(radio4);	//◎GridMap1
			this.add(radio5);	//◎GridMap2
			
			this.add(Box.createRigidArea(space));
			
			buttonScene2();
		}
		
		public void buttonScene1(){
			b1.setEnabled(true);
			b2.setEnabled(true);
		}
		
		public void buttonScene2(){
			b1.setEnabled(false);
			b2.setEnabled(false);
		}
		
		private String convBoole(boolean flag){
			if(flag) return "0,";
			else return "1,";
		}
		
		private boolean readGridStatusData(int floor_num,File file){
			try {
				BufferedReader br = new BufferedReader(new FileReader(file));	//ファイルを読み込み
				String str = br.readLine();						//一行読み込み
				str = br.readLine();							//一行読み込み
				category = str.split(",",0);					//カテゴリーを保持する配列に二行目のデータを与える
				str = br.readLine();							//一行読み込み
				int counter = 0;
				while(str != null){								//ファイルの終わりじゃないとき
					String[] r_status = str.split(",",0);

					grid_status[floor_num][counter].object_num = Integer.valueOf(r_status[0]);
					if(r_status[1].equals("0")) grid_status[floor_num][counter].set_flag = true;
					else grid_status[floor_num][counter].set_flag = false;
					if(r_status[2].equals("0")) grid_status[floor_num][counter].throw_flag = true;
					else grid_status[floor_num][counter].throw_flag = false;
					grid_status[floor_num][counter].object_name = r_status[3];
					grid_status[floor_num][counter].category_num = Integer.valueOf(r_status[4]);
					grid_status[floor_num][counter].open_time = Integer.valueOf(r_status[5]);
					grid_status[floor_num][counter].close_time = Integer.valueOf(r_status[6]);
					grid_status[floor_num][counter].other = r_status[7];
					
					//次の行を読み込む
					str = br.readLine();
					counter++;
					
					if(grid_status[floor_num].length < counter-2)return false;
				}
				
				br.close();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return false;
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return false;
			}

			//モデルの描画処理
			repaint();

			return true;
		}
		
		private boolean saveGridData(int floor_num,String filePath){
			File f = new File(filePath);
			try {
				if(!f.exists())f.createNewFile();
				PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(f)));
				String lineMaps_path =  file_names.get(floor_num);
				pw.println(lineMaps_path);
				
				for(int i = 0;i < grid_data[floor_num].length ;i++){
					String write_str = "";
					for(int j = 0;j < grid_data[floor_num][i].length ;j++){
						write_str += String.valueOf(grid_data[floor_num][i][j].throw_grid);
						if(j<grid_data[floor_num][i].length) write_str += ",";
					}
					pw.println(write_str);
				}
				pw.close();
				return true;
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
				return false;
			}
		}
		
		
		private boolean saveGridStatusData(int floor_num,String filePath){
			File f = new File(filePath);
			try {
				if(!f.exists())f.createNewFile();
				PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(f)));
				String lineMaps_path = file_names.get(floor_num);
				String category_list = "";
				for(int i = 0;i < category.length ;i++){
					category_list += category[i];
					if(i < category.length-1) category_list += ",";
				}
				pw.println(lineMaps_path);
				pw.println(category_list);
				
				for(int i = 0;i < grid_status[floor_num].length ;i++){
					String write_str = "";
					write_str += grid_status[floor_num][i].object_num + ",";
					write_str += convBoole(grid_status[floor_num][i].set_flag);
					write_str += convBoole(grid_status[floor_num][i].throw_flag);
					write_str += grid_status[floor_num][i].object_name +",";
					write_str += grid_status[floor_num][i].category_num +",";
					write_str += grid_status[floor_num][i].open_time+",";
					write_str += grid_status[floor_num][i].close_time+",";
					write_str += grid_status[floor_num][i].other+",-";
					pw.println(write_str);
				}
				pw.close();
				return true;
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
				return false;
			}
		}


		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String cmd = e.getActionCommand();

			if(make_model_flag){
				int floor_num = model_panel.getSelectFloor();
				//表示階の"読み込み"ボタンが押されたとき
				if(cmd.equals("read_status")){
					JFileChooser fc = new JFileChooser("C:\\tmp\\makeMapsModel\\maps\\linemaps");
					fc.setMultiSelectionEnabled(true);
					int selected = fc.showOpenDialog(this);
					if (selected == JFileChooser.APPROVE_OPTION){

						if(radio1.isSelected()){
							
							//ファイルの読み込み
							File file = fc.getSelectedFile();
							
							if(!readGridStatusData(floor_num,file))JOptionPane.showMessageDialog(this, "読み込みに失敗しました", "Error",JOptionPane.ERROR_MESSAGE);
							else model_panel.repaint();
						}else{
							//ファイルの読み込み
							File[] files = fc.getSelectedFiles();
							
							for(int i=0; i< files.length; i++){
								if(!readGridStatusData(i,files[i]))JOptionPane.showMessageDialog(this, files[i].getPath() + "の読み込みに失敗しました", "Error",JOptionPane.ERROR_MESSAGE);
								else model_panel.repaint();
							}
						}
					}

				//表示階の"保存"ボタンが押されたとき
				}else if(cmd.equals("save_status")){
					if(radio1.isSelected()){
						String grid_path = save_folder_path + file_names.get(floor_num) + "_grid";
						if(saveGridData(floor_num,grid_path)) JOptionPane.showMessageDialog(this, "グリッドマップを保存しました", "Info", JOptionPane.INFORMATION_MESSAGE);
						else JOptionPane.showMessageDialog(this, "グリッドマップの保存に失敗しました", "Error",JOptionPane.ERROR_MESSAGE);

						String grid_status_path = save_folder_path + file_names.get(floor_num) + "_grid_status";
						if(saveGridStatusData(floor_num,grid_status_path)) JOptionPane.showMessageDialog(this, "グリッドステータスを保存しました", "Info", JOptionPane.INFORMATION_MESSAGE);
						else JOptionPane.showMessageDialog(this, "グリッドステータスの保存に失敗しました", "Error",JOptionPane.ERROR_MESSAGE);
					}else{
						select_save_flag = true;
						boolean success_flag = true;
						for( int i = 0 ; i < file_names.size() ; i++ ){
							String grid_path = save_folder_path + file_names.get(floor_num) + "_grid";
							String grid_status_path = save_folder_path + file_names.get(floor_num) + "_grid_status";
							if(saveGridStatusData(i,grid_status_path)){
								save_file_path += grid_status_path;
								if(i < file_names.size() - 1)save_file_path += ",";
							}else success_flag = false;
						}
						if(success_flag){
							JOptionPane.showMessageDialog(this, "保存しました", "Info", JOptionPane.INFORMATION_MESSAGE);
							make_save_path = true;
						}else JOptionPane.showMessageDialog(this, "保存に失敗しました", "Error",JOptionPane.ERROR_MESSAGE);
					}
			    }else if(e.getActionCommand().equals("GridMap1") || e.getActionCommand().equals("AddStatus") || e.getActionCommand().equals("GridMap2")){
			    	model_panel.repaint();
			    }
					
			}
		}

		@Override
		public void itemStateChanged(ItemEvent arg0) {
			// TODO Auto-generated method stub
			if(arg0.getStateChange() == ItemEvent.SELECTED || arg0.getStateChange() == ItemEvent.DESELECTED){
				model_panel.repaint();
			}
		}
		
	}
	
	//階挿入移動のときのポップアップウィンドウ
	class AddStatusDialog extends JDialog implements ActionListener{

		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		private JLabel jl1,jl2,jl3,jl4,jl5,jl6;
		private JComboBox<String> category_name,open_time,close_time;
		private JButton set = new JButton("決定");
		private JTextField tex1,tex2,tex3;
		private JCheckBox check1;
		private int floor_num;
		private int object_num;
		
		String[] time_combo = {"--:--","00:00","00:30","01:00","01:30","02:00","02:30","03:00","03:30","04:00","04:30","05:00","05:30","06:00",
				"06:30","07:00","07:30","08:00","08:30","09:00","09:30","10:00","10:30","11:00","11:30","12:00","12:30","13:00","13:30",
				"14:00","14:30","15:00","15:30","16:00","16:30","17:00","17:30","18:00","18:30","19:00","19:30","20:00","20:30","21:00",
				"21:30","22:00","22:30","23:00","23:30"};
		
		/**
		 * ・通行可否
		 * ・店の属性
		 * 	・名前
		 * 	・カテゴリー
		 * 	・開店時間
		 * 	・閉店時間
		 */
		AddStatusDialog(int floor,int object,boolean flag){
			floor_num = floor;
			object_num = object;
			JPanel panel = new JPanel();
			add(panel);
			panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
			setModal(true);
			
			JPanel panel1 = new JPanel();
			jl1 = new JLabel("オブジェクトのステータスを選択してください");
			panel1.add(jl1);
			
			JPanel panel2 = new JPanel();
			check1 = new JCheckBox("通行可能");
			if(flag)check1.setSelected(grid_status[floor][object].throw_flag);
			panel2.add(check1);
			
			JPanel panel3 = new JPanel();
			jl2 = new JLabel("名前");
			if(flag) tex1 = new JTextField(grid_status[floor][object].object_name,5);
			else  tex1 = new JTextField("",5);
			panel3.add(jl2);
			panel3.add(tex1);
			
			JPanel panel4 = new JPanel();
			jl3 = new JLabel("カテゴリー");
			category_name = new JComboBox(category);
			category_name.addItem("右のボックスへ入力");
			if(flag)category_name.setSelectedIndex(grid_status[floor][object].category_num);
			tex2 = new JTextField("",5);
			panel4.add(jl3);
			panel4.add(category_name);
			panel4.add(tex2);
			
			JPanel panel6 = new JPanel();
			jl4 = new JLabel("OPEN");
			open_time = new JComboBox(time_combo);
			if(flag)open_time.setSelectedIndex(grid_status[floor][object].open_time);
			jl5 = new JLabel("　～　CLOSE");
			close_time = new JComboBox(time_combo);
			if(flag)close_time.setSelectedIndex(grid_status[floor][object].close_time);
			panel6.add(jl4);
			panel6.add(open_time);			
			panel6.add(jl5);
			panel6.add(close_time);
			
			JPanel panel7 = new JPanel();
			jl6 = new JLabel("その他");
			if(flag)tex3 = new JTextField(grid_status[floor][object].other,8);
			else tex3 = new JTextField("",8);
			panel7.add(jl6);
			panel7.add(tex3);
			
			JPanel panel5 = new JPanel();
			set.addActionListener(this);			
			panel5.add(set);
			
			panel.add(panel1);
			panel.add(panel2);
			panel.add(panel3);
			panel.add(panel4);
			panel.add(panel6);
			panel.add(panel7);
			panel.add(panel5);

			this.pack();
			setVisible(true);
		}		
		@Override
		public void actionPerformed(ActionEvent arg0) {
			// TODO Auto-generated method stub
			if(arg0.getSource() == set){
				grid_status[floor_num][object_num].set_flag = true;
				grid_status[floor_num][object_num].throw_flag = check1.isSelected();
				grid_status[floor_num][object_num].object_name = tex1.getText();
				if(category_name.getSelectedItem().equals("右のボックスへ入力")){
	 				if(category==null) category = new String[1];
	 				else category = Arrays.copyOf(category, category.length+1);
	 				category[category.length-1] = tex2.getText();
				}
				grid_status[floor_num][object_num].category_num = category_name.getSelectedIndex();
				grid_status[floor_num][object_num].open_time = open_time.getSelectedIndex();
				grid_status[floor_num][object_num].close_time = close_time.getSelectedIndex();
				grid_status[floor_num][object_num].other = tex3.getText();
				change_status = true;
				setVisible(false);
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
	    private Point mouse_click_pos;		//マウス位置
	    public Point offset;	//回転及び描画の中心座標
	    
/*-------------------------------------------------------------*/
	    
	    
		CenterPanel(){
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
	        
			offset = new Point(0,0);
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
		public Point getTopLeftPoint(int floor_num){
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
		
		//指定された階の座標群の中から、最も右下に位置する座標を抽出する
		//また、右下の点が存在しない場合は、疑似的にもっとも右下となる点を作成する
		public Point getBottomRightPoint(int floor_num){
			Point br = null;
			for(int i = 0; i < vertices.get(floor_num).size(); i++) {
	 	    	ArrayList<Point> object_vertices = vertices.get(floor_num).get(i);
	 	        for(int j = 0; j < object_vertices.size(); j++){
	 	        	Point p = object_vertices.get(j);
	 	        	if(br == null)br = (Point) p.clone();
	 	        	if(br.x < p.x)br.x = p.x;
	 	        	if(br.y < p.y)br.y = p.y;
	 	        }
	 	    }
			return br;
		}
		
		public int getSelectFloor(){
			int floor_num = button_panel.floor.getSelectedIndex();
			return floor_num;
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
 	        Point br_point = getBottomRightPoint(floor_num);

 	        Point err_tl = new Point(tl_point.x - 10 , tl_point.y - 10);

 	        if(button_panel.radio3.isSelected()){
 	        	
		 	    for(int i = 0; i < vertices.get(floor_num).size(); i++) {
		 	    	ArrayList<Point> vert = vertices.get(floor_num).get(i);
		 	    	
	 	        	int[] px = new int[vert.size()];
	 	        	int[] py = new int[vert.size()];
	
		 	        for(int j = 0; j < vert.size(); j++){
			 			px[j] = vert.get(j).x - err_tl.x;
			 			py[j] = vert.get(j).y - err_tl.y;
		 	        }
		 	        
		 	       if(button_panel.ckbox1.isSelected()){
		 	    	   if(grid_status[floor_num][i].select_flag)g.setColor(Color.red);
		 	    	   else if(!grid_status[floor_num][i].set_flag)g.setColor(Color.green);
		 	    	   else g.setColor(Color.blue);
			 	       if(vert.size() > 0)g.fillPolygon(px, py, vert.size());
		 	       }
		 	        
		 	        g.setColor(Color.black);
		 	        if(vert.size() > 0)g.drawPolygon(px, py, vert.size());
		 	    }
 	        }else if(button_panel.radio4.isSelected()){
 	        	if(change_status){
 	        		menue_bar.setGridData(floor_num,tl_point,br_point);
 	        		change_status = false;
 	        	}
 	        	g.setColor(Color.black);
 	        	for(int i=0;i<grid_data[floor_num].length;i++){
 	        		for(int j=0;j<grid_data[floor_num][i].length;j++){
 	        			if(grid_data[floor_num][i][j].throw_grid == 1)g.fillRect(10+(j*4), 10+(i*4), 4, 4);
 	        			else g.drawRect(10+(j*4), 10+(i*4), 4, 4);
 	        		}
 	        	}
 	        }else if(button_panel.radio5.isSelected()){
 	        	if(change_status){
 	        		menue_bar.setGridData(floor_num,tl_point,br_point);
 	        		change_status = false;
 	        	}
 	        	
 	        	for(int i=0;i<grid_data[floor_num].length;i++){
 	        		for(int j=0;j<grid_data[floor_num][i].length;j++){
 	        			if(grid_data[floor_num][i][j].status_grid == -1){
 	        				g.setColor(Color.black);
 	        				g.drawRect(10+(j*4), 10+(i*4), 4, 4); 	        				
 	        			}else{
 	        				int col = (grid_data[floor_num][i][j].status_grid*10)%255;
 	        				Color co = null;
 	        				switch(grid_data[floor_num][i][j].status_grid%3){
 	        				case 0:
 	 	        				co = new Color(col, 0, 0);
 	        					break;
 	        				case 1:
 	 	        				co = new Color(0, col, 0);
 	        					break;
 	        				case 2:
 	 	        				co = new Color(0, 0, col);
 	        					break;
 	        				}
 	        				g.setColor(co);
 	        				g.fillRect(10+(j*4), 10+(i*4), 4, 4);
 	        			}
 	        		}
 	        	}
 	        }

 	        g.setColor(Color.red);
	 	    g.drawOval(-err_tl.x - 2, -err_tl.y - 2, 4, 4);

	 	    if(button_panel.floor.getItemCount() > 0)button_panel.buttonScene1();
	 	    
	 	} 	
	 	
	 	private int clickFigure(int floor_num,Point click_p){
	 		
	 		//表示しているフロアの頂点群
	 		ArrayList<ArrayList<Point>> floor_vertices = vertices.get(floor_num);
	 		
	 		//クリックされた図形を求めるためのインデックスを保持するリスト
	 		ArrayList<Integer> index = new ArrayList<Integer>();
	 		//クリックされた図形を求めるためのオブジェクトの面積を保持するリスト
	 		ArrayList<Double> area = new ArrayList<Double>();
	 		
 	        Point tl_point = getTopLeftPoint(floor_num);
 	        Point err_tl = new Point(tl_point.x - 10 , tl_point.y - 10);
 	        click_p.x += err_tl.x;
 	        click_p.y += err_tl.y;
	 		
	 		//与えられた座標が、輪郭点データ群から成される凸図形の中に入っているかを調べ
	 		//入っている場合はその凸図形の面積と、その凸図形のiteratorを保持
	 		for (int i = 0;i<floor_vertices.size();i++){
	 			ArrayList<Point> obj_vertices = floor_vertices.get(i);

	 			int cn = 0;

	 			for(int j = 0; j < obj_vertices.size() ; j++){
	 				int fp = j; int ep = (j+1)%obj_vertices.size();
	 				
	 				Point fv = obj_vertices.get(fp);
	 				Point ev = obj_vertices.get(ep);

	 				// 上向きの辺。点Pがy軸方向について、始点と終点の間にある。ただし、終点は含まない。(ルール1)
	 				if( ((fv.y <= click_p.y) && (ev.y > click_p.y))
	 				// 下向きの辺。点Pがy軸方向について、始点と終点の間にある。ただし、始点は含まない。(ルール2)
	 					|| ((fv.y > click_p.y) && (ev.y <= click_p.y)) ){
	 				// ルール1,ルール2を確認することで、ルール3も確認できている。
	 	            
	 					// 辺は点pよりも右側にある。ただし、重ならない。(ルール4)
	 					// 辺が点pと同じ高さになる位置を特定し、その時のxの値と点pのxの値を比較する。
	 					double vt =(double)(click_p.y - fv.y) / (double)(ev.y - fv.y);
	 					if(click_p.x < (fv.x + (vt * (ev.x - fv.x)))) ++cn;
	 					
	 				}
	 			}
	 			if(cn%2 == 1){
	 				index.add(i);
	 				double a = 0;
	 				for(int j = obj_vertices.size(),u = 0 ; j-->0 ; u= j){
	 					a += (obj_vertices.get(j).x - obj_vertices.get(u).x*(obj_vertices.get(j).y + obj_vertices.get(u).y));
	 				}
	 				a = Math.abs(0.5*a);
	 				area.add(a);
	 			}
	 		}

	 		double minArea = -1;
	 		int maxIndex = -1;
	 		for(int i=0;i<index.size();i++){
	 			if(i==0){
	 				minArea = area.get(i);
	 				maxIndex = index.get(i);
	 			}
	 			if(minArea > area.get(i)){
	 				minArea = area.get(i);
	 				maxIndex = index.get(i);
	 			}
	 		}
	 		
	 		return maxIndex;
	 	}

	 	
		@Override
		public void mouseClicked(MouseEvent arg0) {
			// TODO Auto-generated method stub
			mouse_click_pos.setLocation(arg0.getX(), arg0.getY());
			if(make_model_flag && button_panel.radio3.isSelected()){
		 		int floor_num = getSelectFloor();
				int maxIndex = clickFigure(floor_num,mouse_click_pos);
		 		if(maxIndex >= 0){
		 			grid_status[floor_num][maxIndex].select_flag = true;
		 			repaint();
		 			dialog_panel = new AddStatusDialog(floor_num,maxIndex,grid_status[floor_num][maxIndex].set_flag);
		 			grid_status[floor_num][maxIndex].select_flag = false;
		 			repaint();
		 		}

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
	
	public class gridStatus{
		public int object_num;
		public boolean select_flag;
		public boolean set_flag;
		public boolean throw_flag;
		public int category_num;
		public String object_name;
		public int open_time;
		public int close_time;
		public String other;
		
		gridStatus(){
			object_num = -1;
			select_flag = false;
			set_flag = false;
			throw_flag = false;
			category_num = -1;
			object_name = "";
			open_time = -1;
			close_time = -1;
			other = "";
		}
		
		gridStatus(int num){
			object_num = num;
			select_flag = false;
			set_flag = false;
			throw_flag = false;
			category_num = -1;
			object_name = "";
			open_time = -1;
			close_time = -1;
			other = "";
		}
		
	}
	
	public class gridData{
		int throw_grid;
		int status_grid;
		
		gridData(){
			throw_grid = 0;
			status_grid = 0;
		}
		
		gridData(int throw_num,int obj_num){
			throw_grid = throw_num;
			status_grid = obj_num;
		}
	}
	
}
