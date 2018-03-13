#include <M12864.h>
#include<ABSACC.h>
uchar temp=0x00;//	PA8155_A需要临时变量
/*--------------------延时子程序------------------------------------*/
static void Delay10ms(uchar s){	   //误差 0us
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
/*--------------------读忙碌子程序----------------------------------*/
void Read_busy(){
	PA8155_A=0X00;//P0 = 0x00;
	temp&=0XFB;//	RS = 0;
	PC8155_A=temp;
	temp|=0X02;//RW = 1;
	PC8155_A=temp;
	temp|=0X01;//E = 1;
	PC8155_A=temp;
	COM8155_A=0X0C;//改变PA端口的工作方式为输入
	while(PA8155_A&0x80);
	COM8155_A=0X0D;//改变PA端口的工作方式为输出
	temp&=0XFE;//	E = 0;
	PC8155_A=temp;
}
/*--------------------写命令到LCD-----------------------------------*/
void write_com(unsigned char cmdcode){
	Read_busy();			//每次读写都要判断是否忙碌
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
	Read_busy();			//每次读写都要判断是否忙碌
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
/*--------------------显示边框--------------------------------------*/
void frame(){
	unsigned char i;
	temp&=0XDF;//Mcs = 0;
	PC8155_A=temp; 												//两个屏幕
	temp&=0XEF;//Scs = 0;
	PC8155_A=temp;
/*********************************************************************/	
	write_com(Page_Add + 0);					  			//上面横线
	write_com(Col_Add + 0);
	for (i = 0;i < 64;i++) write_data(0x01);
/*********************************************************************/
	write_com(Page_Add + 7);					 			//下面横线
	write_com(Col_Add + 0);
	for (i = 0;i < 64;i++) write_data(0x80);
/*********************************************************************/
	for (i = 0;i < 8;i++)
	{
		temp&=0XEF;//Mcs = 0;				   							//右半屏
		PC8155_A=temp;
		temp|=0X20;//Scs = 1;
		PC8155_A=temp;
		write_com(Page_Add + i);	 						//共8页
		write_com(Col_Add + 63);							//最后一列
		write_data(0xff);									//ff表示每页的8行全亮
	 
		temp|=0X10;//Mcs = 1;				  							//左半屏与右半屏类似
		PC8155_A=temp;
		temp&=0XDF;//Scs = 0;
		PC8155_A=temp;
		write_com(Page_Add + i);
		write_com(Col_Add + 0);
		write_data(0xff); 
	}
}
/*--------------------清除内存--------------------------------------*/
void Clr_Scr(){
	unsigned char j, k;
	for (k=0;k<8;k++)
	{
		temp|=0X10;
		PC8155_A=temp;
		temp&=0XDF;//Mcs = 1;Scs = 0;
		PC8155_A=temp;
		delay(10);						  //左半屏清屏
		write_com(Page_Add + k);
		write_com(Col_Add + 0);
		for (j = 0;j < 64;j++) write_data(0x00);
	
		temp&=0XEF;
		PC8155_A=temp;
		temp|=0X20;//Mcs = 0;Scs = 1;
		PC8155_A=temp; 
		delay(10);						  //右半屏清屏
		write_com(Page_Add + k);
		write_com(Col_Add + 0);
		for (j = 64;j < 128;j++) write_data(0x00);
	}          
}
///*------------------显示12864图片---------------------------*/
//void Disp_Img(unsigned char code *img)
//{
//	unsigned char j, k;
//	for (k = 0;k < 8;k++)		  							//共8页
//	{
//		temp|=0X10;
//		PC8155_A=temp;
//		temp&=0XDF;
//		PC8155_A=temp;
//		//Mcs = 1;Scs = 0;
//		delay(10);							//左半屏数据
//		write_com(Page_Add + k);
//		write_com(Col_Add + 0);
//		for (j = 0;j < 64;j++) write_data(img[k*128 + j]);	//每半块屏共64列
//	  	temp&=0xEF;
//		PC8155_A=temp;
//		temp|=0x20;
//		PC8155_A=temp;
//		//Mcs = 0;Scs = 1; 
//		delay(10);							//右半屏数据
//		write_com(Page_Add + k);
//		write_com(Col_Add + 0);
//		for (j = 64;j < 128;j++) write_data(img[k*128 + j]); 
//	}
//}
///*-------------------------显示横---------------------------*/
//void Disp_H_Line()
//{
//	unsigned char i, j;
//	temp&=0xEF;
//	PC8155_A=temp;
//	temp&=0xDF;
//	PC8155_A=temp;
//	//Mcs = 0;
//	//Scs = 0;
//	write_com(Disp_On);										//显示开
//
//	write_com(Page_Add);									//页地址
//	write_com(Start_Line);									//开始行
//	write_com(Col_Add);										//列地址
//
//	for (j = 0;j < 8;j++)									//共8页，所以循环8次
//	{
//		write_com(Page_Add + j);
//		write_com(Col_Add + 0);								//第一列开始
//		write_com(Start_Line+0);							//第一行开始
//		for (i = 0;i < 64;i++)write_data(0x55);	 			//每半屏幕共64列
//	}
//}
///*------------------------显示列-----------------------------*/
//void Disp_V_Line()
//{
//	unsigned char i, j;
//	write_com(Disp_On);	   									//显示开
//	write_com(Page_Add);									//页地址
//	write_com(Start_Line);									//列地址
//	write_com(Col_Add);
//
//	for (j = 0;j < 8;j++)									//共8页
//	{
//		write_com(Page_Add + j);
//		write_com(Col_Add + 0);
//		write_com(Start_Line + 0);
//		for (i = 0;i < 32;i++)								//64列，写两次，一列空白，一列实线，所以循环32次
//		{
//			write_data(0x00); 								//一列空
//			write_data(0xff);								//一列实
//		}
//	}
//}
/*--------------------指定位置显示（数字，单位，符号）16*8----------*/
void other_disp16(unsigned char pag, unsigned char col, unsigned char code *hzk){
	unsigned char j = 0, i = 0;
	for (j = 0;j < 2;j++)	  								//一个汉字需要占用2页，1页8行，共16行
	{
		write_com(Page_Add + pag + j);
		write_com(Col_Add + col);
		for (i = 0;i < 8;i++) 								//16列，所以会循环16次
			write_data(hzk[8*j + i]);
	}
}
/*--------------------指定位置显示汉字16*16-------------------------*/
void hz_disp16(unsigned char pag, unsigned char col, unsigned char code *hzk){
	unsigned char j = 0, i = 0;
	for (j = 0;j < 2;j++)	  								//一个汉字需要占用2页，1页8行，共16行
	{
		write_com(Page_Add + pag + j);
		write_com(Col_Add + col);
		for (i = 0;i < 16;i++) 								//16列，所以会循环16次
			write_data(hzk[16*j + i]);
	}
}
/*--------------------初始化LCD屏-----------------------------------*/
void init_lcd(){
	Read_busy();											//每次读写都要判断是否忙碌
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
	write_com(Disp_Off);	 								//关显示
	write_com(Page_Add + 0);
	write_com(Start_Line + 0);
	write_com(Col_Add + 0);
	write_com(Disp_On);
	return;										//开显示
}
/*--------------------行键盘扫描------------------------------------*/
uchar  KeyBoardScan(void){
  uchar key;
  uchar temp_B;
  uchar i,j=0;
  for(i=0;i<3;i++){
  	 COM8155_A=0x0f;	//pb输出
	 if(i==0) PB8155_A = 0xfe; //11111110
	 if(i==1) PB8155_A = 0xfd;// 11111101
	 if(i==2) PB8155_A = 0Xfb; //11111011
	 COM8155_A = 0X0D; //pb输入
	 temp_B=PB8155_A;  //pb输入读取
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