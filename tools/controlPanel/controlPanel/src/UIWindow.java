import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.File;

import javax.swing.*;

public class UIWindow extends JFrame{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public BufferedImage img;
	public JRadioButton defo,modify,comp;
	
	//マウス位置
	private Point mouse_click_pos;
    private Point mouse_presse_pos;
	private Point mouse_release_pos;
	private Point mouse_dragged_pos;
	private Point pre_pos;
	private Point dra_pos;
	
	private int mouse_event_flag;

	private CenterPanel img_panel;
	public NorthPanel button_panel;
	
	private String file_name = "";
	private String folder_name = "C:\\tmp\\makeMapsModel";
	private boolean file_open_flag = false;
	private boolean folder_open_flag = false;

	public UIWindow(){
		setTitle("UserInterface");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); 
	}

	/*
	 * JMenuBar, JMenu, JMenuItem
	 */

	protected void buildMenus(){
		JMenuBar bar = new JMenuBar();
		JMenu m1 = new TopMenue("File(F)");
		bar.add(m1);
		setJMenuBar(bar);
	}
	
	public void popInfoMessage(String str){
		JOptionPane.showMessageDialog(this, str, "Info", JOptionPane.INFORMATION_MESSAGE);
	}
	
	public void popErrMessage(String str){
		JOptionPane.showMessageDialog(this, str, "Error",JOptionPane.ERROR_MESSAGE);
	}

	class TopMenue extends JMenu implements ActionListener{
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		TopMenue(String name){
			this.setText(name);
			this.setMnemonic(KeyEvent.VK_F);
			
			JMenuItem i1 = new JMenuItem("WorkSpace(W)");
			this.add(i1);
			i1.setMnemonic(KeyEvent.VK_W);
			i1.addActionListener(this);
			i1.setActionCommand("select");

			this.addSeparator(); //項目の区切りの横バー

			JMenuItem i2 = new JMenuItem("FileOpen(O)");
			this.add(i2);
			i2.setMnemonic(KeyEvent.VK_O);
			i2.addActionListener(this);
			i2.setActionCommand("open");

		}

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String cmd = e.getActionCommand();
			
			//一時作業フォルダの選択を実行
			if(cmd.equals("select")){
				folder_open_flag = true;
				JFileChooser filechooser = new JFileChooser("C:\\tmp\\makeMapsModel");
				filechooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

				int selected = filechooser.showSaveDialog(this);
				if (selected == JFileChooser.APPROVE_OPTION){
			    	File file = filechooser.getSelectedFile();
			    	folder_name = "C:\\tmp\\makeMapsModel";
			    	folder_name = file.getAbsolutePath();
			    	//作業領域の作成
			    	if(!folder_name.equals("C:\\tmp\\makeMapsModel")){
			    		File newdir = new File(folder_name + "\\makeMapsModel");
			    		newdir.mkdir();
			    		newdir = new File(folder_name + "\\makeMapsModel\\sub");
			    		newdir.mkdir();
			    		newdir = new File(folder_name + "\\makeMapsModel\\maps");
			    		newdir.mkdir();
			    		newdir = new File(folder_name + "\\makeMapsModel\\maps\\linemaps");
			    		newdir.mkdir();
				    	folder_name = folder_name + "makeMapsModel";
			    	}
					System.out.println("選択されたフォルダ名:" + folder_name);
					if(containsUnicode(folder_name)){
						 System.out.println("日本語が含まれています。　再選択してください。");
						folder_name = new String("");
					}
				}
				folder_open_flag = false;
			}else if(cmd.equals("open")){
				file_open_flag = true;
				JFileChooser fc = new JFileChooser();
				fc.setMultiSelectionEnabled(true);
				
				int selected = fc.showOpenDialog(this);
				if (selected == JFileChooser.APPROVE_OPTION){
					File[] files = fc.getSelectedFiles();
					
					file_name = "";
					for(int i = 0 ; i < files.length ; i++){
						file_name += files[i].getAbsolutePath();
						if(i != files.length - 1)file_name += ",";
					}
					System.out.println("選択されたファイル名:" + file_name);
					if(containsUnicode(file_name)){
						 System.out.println("日本語が含まれています。　再選択してください。");
						file_name = new String("");
					}
				}
				file_open_flag = false;
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
		c.add( img_panel = new CenterPanel(), BorderLayout.CENTER);
		
	}
	
	class NorthPanel extends JPanel implements ActionListener{
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		private JButton b1,b2,b3,b4,b5,b6,b7;

		NorthPanel(){
			//パネルの周囲に溝みたいな装飾
			this.setBorder( BorderFactory.createEtchedBorder());

			//BoxLayout でボタンやらラジオボックスやら貼る
			this.setLayout( new BoxLayout(this, BoxLayout.X_AXIS));

			//部品間の調整用スペース
			Dimension space = new Dimension(10,1);
			
			//ボタン
			b1 = new JButton("修正完了");
			b1.addActionListener(this);
			b1.setActionCommand("do");
			
			b2 = new JButton("リセット");
			b2.addActionListener(this);
			b2.setActionCommand("reset");

			b3 = new JButton("戻す");
			b3.addActionListener(this);
			b3.setActionCommand("retry");
			
			b4 = new JButton("再処理");
			b4.addActionListener(this);
			b4.setActionCommand("restart");
			
			b5 = new JButton("次画像へ");
			b5.addActionListener(this);
			b5.setActionCommand("next");
			
			b6 = new JButton("全処理完了");
			b6.addActionListener(this);
			b6.setActionCommand("comp");
			
			b7 = new JButton("一時保存");
			b7.addActionListener(this);
			b7.setActionCommand("remakeReset");
			
			defo = new JRadioButton("元画像");
			comp = new JRadioButton("処理後");
			modify = new JRadioButton("修正画像");
			ButtonGroup img_radio = new ButtonGroup();

			img_radio.add(defo);
			img_radio.add(comp);
			img_radio.add(modify);
			modify.setSelected(true);
			
			this.add(b1);
			this.add(b7);
			this.add(b2);
			this.add(b3);
			this.add(b4);
			this.add(b5);
			this.add(b6);
			this.add(defo);
			this.add(comp);
			this.add(modify);
			this.add(Box.createRigidArea(space));
		}
		
		/*それぞれの操作可能ボタンの設定*/
		public void buttonScene1(){
			b1.setEnabled(true);
			b2.setEnabled(true);
			b3.setEnabled(true);
			b4.setEnabled(false);
			b5.setEnabled(false);
			b6.setEnabled(false);
			b7.setEnabled(true);
		}
		
		public void buttonScene2(){
			b1.setEnabled(false);
			b2.setEnabled(false);
			b3.setEnabled(true);
			b4.setEnabled(true);
			b5.setEnabled(true);
			b6.setEnabled(true);
			b7.setEnabled(false);
		}
		
		public void buttonScene3(){
			b1.setEnabled(false);
			b2.setEnabled(false);
			b3.setEnabled(false);
			b4.setEnabled(false);
			b5.setEnabled(false);
			b6.setEnabled(false);
			b7.setEnabled(false);
		}


		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			String cmd = e.getActionCommand();
			
			if(cmd.equals("retry")){
				mouse_event_flag = 3;
				System.out.println("RETRY");
			}else if(cmd.equals("reset")){
				mouse_event_flag = 4;
				System.out.println("RESET");
			}else if(cmd.equals("do")){
				mouse_event_flag = 5;
				System.out.println("DO");
			}else if(cmd.equals("restart")){
				mouse_event_flag = 6;
				System.out.println("RESTART");
			}else if(cmd.equals("next")){
				mouse_event_flag = 7;
				defo.setSelected(true);				
				buttonScene3();
				System.out.println("NEXT");
			}else if(cmd.equals("comp")){
				mouse_event_flag = 8;
				System.out.println("COMPLETE");
			}else if(cmd.equals("remakeReset")){
				mouse_event_flag = 9;
				System.out.println("REMAKERESET");
			}

		}
	}
	
	class CenterPanel extends JPanel implements MouseMotionListener, MouseListener{		
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		CenterPanel(){
	        addMouseMotionListener(this);
	        addMouseListener(this);

	    	mouse_click_pos = new Point(0,0);
	        mouse_presse_pos = new Point(0,0);
	    	mouse_release_pos = new Point(0,0);
	    	mouse_dragged_pos = new Point(0,0);
	    	pre_pos = new Point(0,0);
	    	dra_pos = new Point(0,0);
	    	mouse_event_flag = 0;
		}
		
		@Override
		public void mouseClicked(MouseEvent arg0) {
			// TODO Auto-generated method stub
			if(mouse_event_flag == 0 || mouse_event_flag == 2){
				mouse_click_pos.setLocation(arg0.getX(), arg0.getY());
				System.out.println("CLICKED : " + mouse_click_pos.x + "," + mouse_click_pos.y);
				mouse_event_flag = 1;
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
			//マウス位置の更新
			mouse_presse_pos.setLocation(arg0.getX(), arg0.getY());
			pre_pos = mouse_presse_pos.getLocation();
//			System.out.println("PRESSED : " + mouse_presse_pos.x + "," + mouse_presse_pos.y);

		}



		@Override
		public void mouseReleased(MouseEvent arg0) {
			// TODO Auto-generated method stub
			if(mouse_event_flag == 0){
				mouse_release_pos.setLocation(arg0.getX(), arg0.getY());
//				System.out.println("RELEASED : " + mouse_release_pos.x + "," + mouse_release_pos.y);
				if(mouse_event_flag != 1)mouse_event_flag = 2;
			}
			pre_pos.setLocation(0, 0);
			dra_pos.setLocation(0, 0);
			
		}

		@Override
		public void mouseDragged(MouseEvent arg0) {
			mouse_dragged_pos.setLocation(arg0.getX(),arg0.getY());
			dra_pos = mouse_dragged_pos.getLocation();
			// TODO Auto-generated method stub
			repaint();
		}

		@Override
		public void mouseMoved(MouseEvent arg0) {
			// TODO Auto-generated method stub
			
		}
		
		@Override
		public void paint(Graphics g) {
			g.drawImage(img, 0, 0, img_panel);
			g.setColor(Color.BLUE);
			if(dra_pos.x != 0 && dra_pos.y != 0){
				if(pre_pos.x < dra_pos.x && pre_pos.y < dra_pos.y) g.drawRect(pre_pos.x, pre_pos.y, dra_pos.x-pre_pos.x, dra_pos.y-pre_pos.y);
				else if(pre_pos.x < dra_pos.x && pre_pos.y > dra_pos.y) g.drawRect(pre_pos.x, dra_pos.y, dra_pos.x-pre_pos.x, pre_pos.y-dra_pos.y);
				else if(pre_pos.x > dra_pos.x && pre_pos.y < dra_pos.y) g.drawRect(dra_pos.x, pre_pos.y, pre_pos.x-dra_pos.x, dra_pos.y-pre_pos.y);
				else if(pre_pos.x > dra_pos.x && pre_pos.y > dra_pos.y) g.drawRect(dra_pos.x, dra_pos.y, pre_pos.x-dra_pos.x, pre_pos.y-dra_pos.y);
			}
		}

	}

	public String getFolderName(){
		while(folder_open_flag){
			System.out.println();
		}
		return folder_name;
	}
	
	public String getFileName(){
		while(file_open_flag){
			System.out.println();
		}
		return file_name;
	}
	
	public int getMouseFlag(){
		return mouse_event_flag;
	}
	
	public void resetMouseFlag(){
		mouse_event_flag = 0;
	}
	
	public Point getClickPoint(){
		return mouse_click_pos;
	}
	
	public Rectangle getDraggRectangle(){
		Rectangle r = new Rectangle();
		Point p1 = mouse_presse_pos;
		Point p2 = mouse_release_pos;
		if(p1.x < p2.x && p1.y < p2.y){
			r = new Rectangle(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
		}else if(p1.x < p2.x && p2.y < p1.y){
			r = new Rectangle(p1.x, p2.y, p2.x - p1.x, p1.y - p2.y);
		}else if(p2.x < p1.x && p1.y < p2.y){
			r = new Rectangle(p2.x, p1.y, p1.x - p2.x, p2.y - p1.y);
		}else{
			r = new Rectangle(p2.x, p2.y, p1.x - p2.x, p1.y - p2.y);
		}
		return r;
	}
	
	//文字列中に日本語が存在するかどうかの判定
	public static boolean containsUnicode(String str) {
		for(int i = 0 ; i < str.length() ; i++) {
		char ch = str.charAt(i);
		Character.UnicodeBlock unicodeBlock = Character.UnicodeBlock.of(ch);

		if (Character.UnicodeBlock.HIRAGANA.equals(unicodeBlock))
		return true;

		if (Character.UnicodeBlock.KATAKANA.equals(unicodeBlock))
		return true;

		if (Character.UnicodeBlock.HALFWIDTH_AND_FULLWIDTH_FORMS.equals(unicodeBlock))
		return true;

		if (Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS.equals(unicodeBlock))
		return true;

		if (Character.UnicodeBlock.CJK_SYMBOLS_AND_PUNCTUATION.equals(unicodeBlock))
		return true;
		}
		return false;
	}

}
