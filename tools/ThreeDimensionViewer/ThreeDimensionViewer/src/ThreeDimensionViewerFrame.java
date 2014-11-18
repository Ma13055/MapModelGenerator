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
	
	//�}�E�X�ʒu
	private Point mouse_click_pos;
    private Point mouse_presse_pos;
	private Point mouse_release_pos;
	private Point mouse_dragged_pos;
//	private Point pre_pos;
	private Point dra_pos;
	
	//panel�̐錾
	public CenterPanel model_panel;
	public NorthPanel button_panel;
	public TopMenue menue_bar;

	//�}�`���̕ϐ�
    private ArrayList<ArrayList<Vertex>> vertices;   // ���_���ێ�����
    private ArrayList<ArrayList<Face>> faces;      // �ʁi�O�p�`�j���ێ�����

    //���f���̐����ς݂������t���O
    private boolean make_model_flag = false;
		
    //�t���[���̍쐬
	public ThreeDimensionViewerFrame(){
		setTitle("3DViewer");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); 
	}

	/*
	 * JMenuBar, JMenu, JMenuItem
	 */

	//���j���[�o�[�̍쐬
	protected void buildMenus(){
		JMenuBar bar = new JMenuBar();
		menue_bar = new TopMenue("File(F)");
		bar.add(menue_bar);
		setJMenuBar(bar);
	}
	
	//���j���[�o�[�̏ڍאݒ�
	class TopMenue extends JMenu implements ActionListener{
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

		TopMenue(String name){
			this.setText(name);
			this.setMnemonic(KeyEvent.VK_F);
			
			//�t�@�C�����J������
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
			
			//�t�@�C�����J�������ɂ�����̃t�@�C�����w�肳�ꂽ�Ƃ�
			if(cmd.equals("open")){
				JFileChooser fc = new JFileChooser();
				fc.setMultiSelectionEnabled(true);
				
				int selected = fc.showOpenDialog(this);
				if (selected == JFileChooser.APPROVE_OPTION){
					
					//�t�@�C���̓ǂݍ���
					File[] files = fc.getSelectedFiles();
					
					readModelData(files);
				}
			}
		}
		
		public void readModelData(File[] files){
			
			//���f���f�[�^�̏�����
			vertices = new ArrayList<ArrayList<Vertex>>();
			faces = new ArrayList<ArrayList<Face>>();

			//floor�Z���N�g�̃R���{�{�b�N�X��������
			button_panel.floor.removeAllItems();
			button_panel.floor.addItem("ALL");
			//�ǂݍ��񂾃t�@�C�����f�[�^�ɏ����N����
			for(int i=0;i<files.length;i++){
				if(files[i].exists()){
					button_panel.floor.addItem(i+1+"�K");	//�R���{�{�b�N�X�ɊK��ǉ�
					model_panel.setModelData(files[i],i);	//�f�[�^�ɏ����N����
				}
			}
		}
	}
		
	/*
	 * JPanel
	 */

	protected void buildContent(){

		Container c = getContentPane();


		//�㑤��UI�{�^���p�l��
		c.add( button_panel = new NorthPanel(), BorderLayout.NORTH);


		//�摜�\���p�̃p�l��
		c.add( model_panel = new CenterPanel(), BorderLayout.CENTER);

		
	}
	
	
	//�㑤��UI�{�^���p�l��
	class NorthPanel extends JPanel implements ActionListener{
		private static final long serialVersionUID = 1L;
		
		//�\���{�^���̐錾
		private JButton b1;
		private JTextField scale_text,position_x_text,position_y_text,position_z_text,floor_height;
		private JLabel jl1,jl2,jl3,jl4,jl5,jl6;
		public JComboBox<String> floor;

		NorthPanel(){
			//�p�l���̎��͂ɍa�݂����ȑ���
			this.setBorder( BorderFactory.createEtchedBorder());

			//BoxLayout �Ń{�^����烉�W�I�{�b�N�X���\��
			this.setLayout( new BoxLayout(this, BoxLayout.X_AXIS));

			//���i�Ԃ̒����p�X�y�[�X
			Dimension space = new Dimension(10,1);
			
			//�{�^��
			b1 = new JButton("�ύX");
			b1.addActionListener(this);
			b1.setActionCommand("change");
			
			jl1 = new JLabel("�{��");
			jl2 = new JLabel("�@�@���S�ʒu�@X:");
			jl3 = new JLabel(" Y:");
			jl4 = new JLabel(" Z:");
			jl5 = new JLabel(" �K�̍���");
			jl6 = new JLabel("�@�\���K");
			
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
			
			//"�ύX"�{�^���������ꂽ�Ƃ�
			if(cmd.equals("change")){
				System.out.println("ChangePara");
				try{
					model_panel.scale = Double.valueOf(scale_text.getText());
					model_panel.center3d = new Point3D(
							Float.valueOf(position_x_text.getText()),
							Float.valueOf(position_y_text.getText()),
							Float.valueOf(position_z_text.getText()));
					model_panel.modelHeight = Double.valueOf(floor_height.getText());
					
					//�ύX�p�����[�^��p���čĕ`��
					model_panel.movePointArray();
					model_panel.setScreenPosition();
					model_panel.repaint();					
				}catch(NumberFormatException e1){
					System.out.println("�����ȊO�����͂���܂���");
				}
			}
		}
	}
	
	
	//������ʃp�l���̍쐬
	class CenterPanel extends JPanel implements MouseMotionListener, MouseListener{

		private static final long serialVersionUID = 1L;

		//�`��C���[�W�ϐ�
	    Graphics g;
	    Image img;
	    public Dimension windowSize;	//��ʃT�C�Y	    
	    
	    //�p�l���̃T�C�Y
	    final int WIDTH = 700;
	    final int HEIGHT = 700;

/*-------------------------------------------------------------*/
	    		
	    Point center;         // �A�v���b�g�̒��S���W
	    Point mouse_pos;		//�}�E�X�ʒu
	    public double scale;    // ���f���`�掞�̃X�P�[��
	    double phi;			//x������̉�]�p
	    double theta;			//y������̉�]�p
	    public Point3D center3d;	//��]�y�ѕ`��̒��S���W
	    double modelHeight;	//�t���A�̍���

/*-------------------------------------------------------------*/
	    
	    
		CenterPanel(){
	        addMouseMotionListener(this);
	        addMouseListener(this);

	        //�}�E�X�ʒu�̏�����
	    	mouse_click_pos = new Point(0,0);
	        mouse_presse_pos = new Point(0,0);
	    	mouse_release_pos = new Point(0,0);
	    	mouse_dragged_pos = new Point(0,0);
//	    	pre_pos = new Point(0,0);
	    	dra_pos = new Point(0,0);
	    	
	        this.setSize(WIDTH, HEIGHT);

	        // �E�B���h�E�T�C�Y�̎擾
	        windowSize = this.getSize();
	        
	        this.g = this.getGraphics();
	        // �E�B���h�E�̒��S���W�̎擾
	        center = new Point(windowSize.width / 2, windowSize.height*3 / 4);
	        //�}�E�X�ʒu�̏�����
	        mouse_pos = new Point(0,0);
	        
	        //��]�p�̏�����
	        phi = 0.0;
	        theta = 0.0;
	        
			center3d = new Point3D(0,0,0);
			modelHeight = 30;
			scale = 1;
		}
		
		//������̔z��𒸓_���W�̔z��֕ϊ�����
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
						center3d);	//���̂̒�̍��W
				p[counter++] = new Vertex(Double.parseDouble(str[i]),
						c*modelHeight + modelHeight,
						Double.parseDouble(str[i+1]),
						center3d);	//���̂̉����̍��W
			}
			
			return p;
		}
		
		//���S���W���ύX���ꂽ���ɁA���̕ύX�𔽉f������
		public void movePointArray(){
			for(int i=0;i<vertices.size();i++){
				for(int j=0;j<vertices.get(i).size();j++){
					vertices.get(i).get(j).move(center3d);
				}
			}
		}

		
        // ���f���f�[�^�̐ݒ�
		public void setModelData(File f,int c){
			//�ێ��ϐ�������Ă��Ȃ��ꍇ�͏�����
			if(vertices == null) vertices = new ArrayList<ArrayList<Vertex>>();
			if(faces == null) faces = new ArrayList<ArrayList<Face>>();
			
			//��t���A���̒��_�Ɩʃf�[�^��ێ�����ϐ�
			ArrayList<Vertex> sub_vertices = new ArrayList<Vertex>();	// ���_���������
			ArrayList<Face> sub_faces = new ArrayList<Face>();	// �ʗ��������

			BufferedReader br;
			try {
				br = new BufferedReader(new FileReader(f));	//�t�@�C����ǂݍ���
				String str = br.readLine();					//��s�ǂݍ���
				while(str != null){							//�t�@�C���̏I��肶��Ȃ��Ƃ�
					Vertex[] verArray = makePointArray(str.split(",", 0),c).clone();	//��s�̕�����𒸓_�̔z��֕ϊ�
					
					int begin_vect = 0;
					if(sub_vertices != null)begin_vect = sub_vertices.size();	//���_�Q�̖����̔ԍ����T����
					Face bottom_face = new Face();
					Face top_face = new Face();
					
					for(int j=0;j<verArray.length;j++){	//���_�z���ԗ�
						sub_vertices.add(verArray[j]);	//��t���A�̒��_��ێ�����ϐ��ɒ��_��������

						
						//���ݏ������Ă��钸�_���A�}�`�̏�ꂩ���ꂩ�𔻕�
						if(j%2==0){
							bottom_face.add(begin_vect+j);
							
							//����Ȃ�A���ʂ̃f�[�^���쐬����
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
					//�쐬�����ʃf�[�^��ێ�
					sub_faces.add(bottom_face);
					sub_faces.add(top_face);
	
					str = br.readLine();
				}
				
				//�쐬�����ʃf�[�^�Q�ƒ��_�f�[�^�Q��ێ�
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

			//���f�����쐬�����t���O�𗧂Ă�
			make_model_flag = true;
			//���f���̕`�揈��
			setScreenPosition();
			repaint();
		}
					
        // ���_�X�N���[�����W�̐ݒ�
	 	public void setScreenPosition(){
	 		if(make_model_flag == false) return;
	 		for(int k=0;k<vertices.size();k++){
		 		for(int i=0;i<vertices.get(k).size();i++){
		 			Vertex v = (Vertex) vertices.get(k).get(i);
		 			
		 			//��]��̍��W�l�̎Z�o
		 			v.rx = v.mx*Math.cos(theta) + v.mz*Math.sin(theta);
		 			v.ry = v.mx*Math.sin(phi)*Math.sin(theta) + v.my*Math.cos(phi) - v.mz*Math.sin(phi)*Math.cos(theta);
		 			v.rz = - v.mx*Math.cos(phi)*Math.sin(theta) + v.my*Math.sin(phi) + v.mz*Math.cos(phi)*Math.cos(theta);
		 			
		 			//�X�N���[�����W�̎Z�o
		 			v.screenX = (int)(center.x + scale*v.rx);
		 			v.screenY = (int)(center.y/2 - scale*v.ry);
		 		}
	 		}
	 	}
	 	
	 	//���f���̕`��
	 	private void drawModel(Graphics g){
	 		if(make_model_flag == false) return;
	 		
	 	    // �`��F�𔒂ɐݒ�
	 	    g.setColor(Color.white);
	 	    g.fillRect(0, 0, windowSize.width, windowSize.height);
	 	    
 	        // �ʂ̗֊s���̕`��
 	        g.setColor(Color.black);

	 	    // �e�ʂ̕`��
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
			//�}�E�X�ʒu�̍X�V
			mouse_presse_pos.setLocation(arg0.getX(), arg0.getY());
//			pre_pos = mouse_presse_pos.getLocation();
			
			//�}�E�X�ʒu�̍X�V
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
			
			//��]�p�̍X�V
			theta += (dra_pos.getX() - mouse_pos.x)*0.01;
			phi   += (dra_pos.getY() - mouse_pos.y)*0.01;
			
			//x������̉�]�p�ɏ����ݒ�
			phi = Math.min(phi, Math.PI/2);
			phi = Math.max(phi, -Math.PI/2);
			
			//�}�E�X�ʒu�̍X�V
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
			
	 		//�_�u���o�b�t�@�����O�p�̃C���[�W���쐬
	 		if(img == null){
	 			img = createImage(windowSize.width,windowSize.height);
	 		}

	 		//�o�b�t�@�Ƀ��f����`��
	 		drawModel(img.getGraphics());
	 		//�o�b�t�@�C���[�W���A�v���b�g�ɕ`��
	 		g.drawImage(img, 0, 0, this);
		}

	}	
	
	//�ʃN���X
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

	//���_�N���X
	public class Vertex{
		public double x, y, z; // ���f���̒��_���W
		public double mx,my,mz;
		public double MathX,MathY,MathZ;
		public double rx, ry, rz;	//��]��������̍��W
		public int screenX, screenY;	//�X�N���[����̍��W

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
