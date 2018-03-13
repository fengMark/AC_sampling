#include <M12864.h>
#include<ABSACC.h>
uchar temp=0x00;//	PA8155_A��Ҫ��ʱ����
/*--------------------��ʱ�ӳ���------------------------------------*/
static void Delay10ms(uchar s){	   //��� 0us
	unsigned char a,b;
	for(s=1;s>0;s--)
		for(b=38;b>0;b--)
			for(a=130;a>0;a--);
}
void delay(unsigned int t){
	unsigned int i, j;
	for (i = 0;i < t;i++)
		for (j = 0;j < 10;j++)
		{
		       ;
		}
}
/*--------------------��æµ�ӳ���----------------------------------*/
void Read_busy(){
	PA8155_A=0X00;//P0 = 0x00;
	temp&=0XFB;//	RS = 0;
	PC8155_A=temp;
	temp|=0X02;//RW = 1;
	PC8155_A=temp;
	temp|=0X01;//E = 1;
	PC8155_A=temp;
	COM8155_A=0X0C;//�ı�PA�˿ڵĹ�����ʽΪ����
	while(PA8155_A&0x80);
	COM8155_A=0X0D;//�ı�PA�˿ڵĹ�����ʽΪ���
	temp&=0XFE;//	E = 0;
	PC8155_A=temp;
}
/*--------------------д���LCD-----------------------------------*/
void write_com(unsigned char cmdcode){
	Read_busy();			//ÿ�ζ�д��Ҫ�ж��Ƿ�æµ
	temp&=0XFB;//RS = 0;
	PC8155_A=temp;
	temp&=0XFD;//RW = 0;
	PC8155_A=temp;
	PA8155_A=cmdcode;//Lcd_Bus = cmdcode;
	delay(0);
	temp|=0X01;//E = 1;
	PC8155_A=temp;
	delay(0);
	temp&=0XFE;//E = 0;
	PC8155_A=temp;
}         
void write_data(unsigned char Dispdata){
	Read_busy();			//ÿ�ζ�д��Ҫ�ж��Ƿ�æµ
	temp|=0X04;//RS = 1;
	PC8155_A=temp;
	temp&=0XFD;//RW = 0;
	PC8155_A=temp;
	PA8155_A = Dispdata;
	delay(0);
	temp|=0X01;//E = 1;
	PC8155_A=temp;
	delay(0);
	temp&=0XFE;	//E = 0;
	PC8155_A=temp;
}
/*--------------------��ʾ�߿�--------------------------------------*/
void frame(){
	unsigned char i;
	temp&=0XDF;//Mcs = 0;
	PC8155_A=temp; 												//������Ļ
	temp&=0XEF;//Scs = 0;
	PC8155_A=temp;
/*********************************************************************/	
	write_com(Page_Add + 0);					  			//�������
	write_com(Col_Add + 0);
	for (i = 0;i < 64;i++) write_data(0x01);
/*********************************************************************/
	write_com(Page_Add + 7);					 			//�������
	write_com(Col_Add + 0);
	for (i = 0;i < 64;i++) write_data(0x80);
/*********************************************************************/
	for (i = 0;i < 8;i++)
	{
		temp&=0XEF;//Mcs = 0;				   							//�Ұ���
		PC8155_A=temp;
		temp|=0X20;//Scs = 1;
		PC8155_A=temp;
		write_com(Page_Add + i);	 						//��8ҳ
		write_com(Col_Add + 63);							//���һ��
		write_data(0xff);									//ff��ʾÿҳ��8��ȫ��
	 
		temp|=0X10;//Mcs = 1;				  							//��������Ұ�������
		PC8155_A=temp;
		temp&=0XDF;//Scs = 0;
		PC8155_A=temp;
		write_com(Page_Add + i);
		write_com(Col_Add + 0);
		write_data(0xff); 
	}
}
/*--------------------����ڴ�--------------------------------------*/
void Clr_Scr(){
	unsigned char j, k;
	for (k=0;k<8;k++)
	{
		temp|=0X10;
		PC8155_A=temp;
		temp&=0XDF;//Mcs = 1;Scs = 0;
		PC8155_A=temp;
		delay(10);						  //���������
		write_com(Page_Add + k);
		write_com(Col_Add + 0);
		for (j = 0;j < 64;j++) write_data(0x00);
	
		temp&=0XEF;
		PC8155_A=temp;
		temp|=0X20;//Mcs = 0;Scs = 1;
		PC8155_A=temp; 
		delay(10);						  //�Ұ�������
		write_com(Page_Add + k);
		write_com(Col_Add + 0);
		for (j = 64;j < 128;j++) write_data(0x00);
	}          
}
///*------------------��ʾ12864ͼƬ---------------------------*/
//void Disp_Img(unsigned char code *img)
//{
//	unsigned char j, k;
//	for (k = 0;k < 8;k++)		  							//��8ҳ
//	{
//		temp|=0X10;
//		PC8155_A=temp;
//		temp&=0XDF;
//		PC8155_A=temp;
//		//Mcs = 1;Scs = 0;
//		delay(10);							//���������
//		write_com(Page_Add + k);
//		write_com(Col_Add + 0);
//		for (j = 0;j < 64;j++) write_data(img[k*128 + j]);	//ÿ�������64��
//	  	temp&=0xEF;
//		PC8155_A=temp;
//		temp|=0x20;
//		PC8155_A=temp;
//		//Mcs = 0;Scs = 1; 
//		delay(10);							//�Ұ�������
//		write_com(Page_Add + k);
//		write_com(Col_Add + 0);
//		for (j = 64;j < 128;j++) write_data(img[k*128 + j]); 
//	}
//}
///*-------------------------��ʾ��---------------------------*/
//void Disp_H_Line()
//{
//	unsigned char i, j;
//	temp&=0xEF;
//	PC8155_A=temp;
//	temp&=0xDF;
//	PC8155_A=temp;
//	//Mcs = 0;
//	//Scs = 0;
//	write_com(Disp_On);										//��ʾ��
//
//	write_com(Page_Add);									//ҳ��ַ
//	write_com(Start_Line);									//��ʼ��
//	write_com(Col_Add);										//�е�ַ
//
//	for (j = 0;j < 8;j++)									//��8ҳ������ѭ��8��
//	{
//		write_com(Page_Add + j);
//		write_com(Col_Add + 0);								//��һ�п�ʼ
//		write_com(Start_Line+0);							//��һ�п�ʼ
//		for (i = 0;i < 64;i++)write_data(0x55);	 			//ÿ����Ļ��64��
//	}
//}
///*------------------------��ʾ��-----------------------------*/
//void Disp_V_Line()
//{
//	unsigned char i, j;
//	write_com(Disp_On);	   									//��ʾ��
//	write_com(Page_Add);									//ҳ��ַ
//	write_com(Start_Line);									//�е�ַ
//	write_com(Col_Add);
//
//	for (j = 0;j < 8;j++)									//��8ҳ
//	{
//		write_com(Page_Add + j);
//		write_com(Col_Add + 0);
//		write_com(Start_Line + 0);
//		for (i = 0;i < 32;i++)								//64�У�д���Σ�һ�пհף�һ��ʵ�ߣ�����ѭ��32��
//		{
//			write_data(0x00); 								//һ�п�
//			write_data(0xff);								//һ��ʵ
//		}
//	}
//}
/*--------------------ָ��λ����ʾ�����֣���λ�����ţ�16*8----------*/
void other_disp16(unsigned char pag, unsigned char col, unsigned char code *hzk){
	unsigned char j = 0, i = 0;
	for (j = 0;j < 2;j++)	  								//һ��������Ҫռ��2ҳ��1ҳ8�У���16��
	{
		write_com(Page_Add + pag + j);
		write_com(Col_Add + col);
		for (i = 0;i < 8;i++) 								//16�У����Ի�ѭ��16��
			write_data(hzk[8*j + i]);
	}
}
/*--------------------ָ��λ����ʾ����16*16-------------------------*/
void hz_disp16(unsigned char pag, unsigned char col, unsigned char code *hzk){
	unsigned char j = 0, i = 0;
	for (j = 0;j < 2;j++)	  								//һ��������Ҫռ��2ҳ��1ҳ8�У���16��
	{
		write_com(Page_Add + pag + j);
		write_com(Col_Add + col);
		for (i = 0;i < 16;i++) 								//16�У����Ի�ѭ��16��
			write_data(hzk[16*j + i]);
	}
}
/*--------------------��ʼ��LCD��-----------------------------------*/
void init_lcd(){
	Read_busy();											//ÿ�ζ�д��Ҫ�ж��Ƿ�æµ
	temp&=0xF7;
	PC8155_A=temp;
//	RST = 0;
	delay(100);
	temp|=0x08;
	PC8155_A=temp;
//	RST = 1;
	delay(100);
	temp&=0xEF;
	PC8155_A=temp;
	temp&=0xDF;
	PC8155_A=temp;
	//Mcs = 0;
	//Scs = 0;
	delay(100);
	write_com(Disp_Off);	 								//����ʾ
	write_com(Page_Add + 0);
	write_com(Start_Line + 0);
	write_com(Col_Add + 0);
	write_com(Disp_On);
	return;										//����ʾ
}
/*--------------------�м���ɨ��------------------------------------*/
uchar  KeyBoardScan(void){
  uchar key;
  uchar temp_B;
  uchar i,j=0;
  for(i=0;i<3;i++){
  	 COM8155_A=0x0f;	//pb���
	 if(i==0) PB8155_A = 0xfe; //11111110
	 if(i==1) PB8155_A = 0xfd;// 11111101
	 if(i==2) PB8155_A = 0Xfb; //11111011
	 COM8155_A = 0X0D; //pb����
	 temp_B=PB8155_A;  //pb�����ȡ
	 temp_B=temp_B&0xf0;
	 if(temp_B!=0xf0){
	 	 Delay10ms(1);
		 temp_B = PB8155_A;
		 temp_B = temp_B&0xf0;
		 if(temp_B!=0xf0){
		 switch(temp_B){
		 	case 0xe0:{
				   key = 7-i*3;
			}
			break;
			case 0xd0:{
				key = 8-i*3;
			}
			break;
			case 0xb0:{
				key = 9-i*3;
			}
			break;
		 }
		}
		 return key;
	 } 
  }
  return 10;
}