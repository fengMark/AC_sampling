#include <reg52.h>
#include <ZK.h>
#include<ABSACC.h>
#include<Solve.h>
#include<M12864.h>
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
#define VOLUTAGE 1
#define CURRENT 0
#define adio (1000*10/4095)//转换进率，12位精度(2^12-1=4095)，满量程为10V:转换为可存储的电压值
/*----------62256存储器地址-------------------------*/
uint code UIaddr[5]={0x0000,0x0010,0x0020,0x0030,0x0040};
uint code PQaddr[5]={0x0100,0x0110,0x0120,0x0130,0x0140}; 
uint xdata *  U_I_Data;//这里定义是一定要注意数据类型，这里的类型是指地址里的Ua,Ub,Uc,Ia,Ib,Ic
long xdata *  P_Q_Data;
/*----------8155端口和命令状态寄存器地址-------------*/
#define COM8155 XBYTE[0X7f00]
#define PA8155  XBYTE[0X7f01]
#define PB8155  XBYTE[0X7f02]
#define PC8155  XBYTE[0X7f03]
#define LT8155  XBYTE[0X7F04]
#define HT8155  XBYTE[0x7f05]
/*--------------------38译码器芯片引脚-----------------------*/
sbit G1 = P1^7;  
sbit G2 = P1^6;
sbit G3 = P1^5;
sbit ADSTART  = P1^4;           
sbit ADINT  = P1^3;             
sbit FREINT = P1^2;             
sbit PS2 = P1^1; 
sbit PS1 = P1^0;
/*----------------8155定时器的高低位的计数次数---------------------*/
uchar HTime;
uchar LTime;
/*---------------------频率周期-------------------------------*/
uint fre[3],real_fre;
uint x0,x1;
uchar times=0;
uchar flage;
/*--------------------电压和电流的基准值-----------------------*/ 
uchar biaozhiwei;
int u,i;
/*------------------------显示数组---------------------------*/
uint  disBuf_U_I[6];
long  disBuf_P_Q[2];
void config_8155_A();
void config_8155_B();
void Read62256(uchar channel);
void DisPlay_uint(uint value,uchar i);
void DisPlay_long(long value,uchar i);
void Msg(int page,uchar channel);
void TimerConfig(void);
void Outside_Init(void);
int ReadAD1674(int U_I_flag);
void Measure_Fre(uchar s);
void Measure_U_I(uchar s);
void Acs_working(void);
uchar operate(uchar KeyValue);
/*---------------------主程序-------------------------------------------------*/
void main(void){
	uchar flage=1;
	uchar choice=0;
	ADINT=1;
	FREINT=1;
	config_8155_A();//选中12864和键盘
	init_lcd();
	Clr_Scr();
	frame();
	hz_disp16(2, 8, yao);
	hz_disp16(2, 40, ce);
	hz_disp16(4, 40, di);
	temp&=0xEF;
	PC8155_A=temp;
	temp|=0x20;
	PC8155_A=temp;//Mcs = 0;Scs = 1;	
	hz_disp16(2, 8, zhong);
	hz_disp16(2, 40, duan);
	hz_disp16(4, 8, lu);
	while (flage)
	{
			flage=operate(KeyBoardScan());
	}
}

/*---------------------配置8155(外设12864和键盘)芯片--------------------------*/
void config_8155_A(){
	G1=1;
	G2=1;
	G3=0;
	COM8155_A=0X0f;
}
/*---------------------配置8155(控制多路切换)芯片-----------------------------*/
void config_8155_B(){ //多路切换
			G1=0;
			G2=0;
			G3=1;
			COM8155=0X0f;
}
/*---------------------从62256存储器中查找数据--------------------------------*/
void Read62256(uchar channel){
	uchar i;
	U_I_Data=(uint  *)(UIaddr[channel-1]);
	P_Q_Data=(long  *)(PQaddr[channel-1]);
	G1=1;G2=0;G3=1;//选中62256，从62256中找数据
	for(i=0;i<6;i++){
		disBuf_U_I[i]=*U_I_Data; //取内容
		U_I_Data++;
	}
	for(i=0;i<2;i++){
		disBuf_P_Q[i]=*P_Q_Data; //取内容
		P_Q_Data++;
	}
}
/*---------------------显示电压和电流值---------------------------------------*/
void DisPlay_uint(uint value,uchar i){
	uchar wei;
	config_8155_A();//选中8155扩展的12864和键盘
	temp&=0xEf;	  					//Mcs = 0;Scs = 1;
	PC8155=temp;
	temp|=0x20;
	PC8155=temp;
	wei=value/1000;		  //右屏显示电压有效值和电流有效值
	other_disp16(2*i,0,num[wei]);//个位数字	0.000
	other_disp16(2*i,8,dot);	 //小数点
	wei=value%1000/100;
	other_disp16(2*i,16,num[wei]);
	wei=value%1000%100/10;
	other_disp16(2*i,24,num[wei]);
	wei=value%1000%100%10;
	other_disp16(2*i,32,num[wei]);
}
/*---------------------显示有效和无效的功率值---------------------------------*/
void DisPlay_long(long value,uchar i){
	uchar wei;
	config_8155_A();
	temp|=0x10;						//Mcs = 1;Scs = 0;
	PC8155=temp;
	temp&=0xDf;
	PC8155=temp;
	if(value<0){			//负号和整数部分左边的屏显示
		other_disp16(2*(i+2),40,fuhao);	
	}
	wei=value/10000000;
	other_disp16(2*(i+2),48,num[wei]);
	wei=value%10000000/1000000;
	other_disp16(2*(i+2),56,num[wei]);
	temp&=0xEf;	  					//Mcs = 0;Scs = 1;
	PC8155=temp;
	temp|=0x20;
	PC8155=temp;			 //小数点和小数部分右屏显示
	other_disp16(2*(i+2),0,dot);
	wei=value%10000000%1000000/100000;
	other_disp16(2*(i+2),8,num[wei]);
	wei=value%10000000%1000000%100000/10000;
	other_disp16(2*(i+2),16,num[wei]);
	wei=value%10000000%1000000%100000%10000/1000;
	other_disp16(2*(i+2),24,num[wei]);
	wei=value%10000000%1000000%100000%10000%1000/100;
	other_disp16(2*(i+2),32,num[wei]);
	wei=value%10000000%1000000%100000%10000%1000%100/10;
	other_disp16(2*(i+2),40,num[wei]);
	wei=value%10000000%1000000%100000%10000%1000%100%10;
	other_disp16(2*(i+2),48,num[wei]);
}
/*---------------------信息显示:可用信息和退出页面----------------------------*/
void Msg(int page,uchar channel){			 
	uchar i; 
	Clr_Scr();                                                                                              //清屏                                                                                                  //显示边框
	if(page == 0)    //信息一页
	{                                                                                                               //显示“测试开始”
		temp|=0x10;						//Mcs = 1;Scs = 0;
		PC8155=temp;
		temp&=0xDf;
		PC8155=temp;
		other_disp16(0, 0, dianya);
		other_disp16(0, 8, zimu_a);
		other_disp16(0, 16, fenghao);
		other_disp16(2, 0, dianya);
		other_disp16(2, 8, zimu_b);
		other_disp16(2, 16, fenghao);
		other_disp16(4, 0, dianya);
		other_disp16(4, 8, zimu_c);
		other_disp16(4, 16, fenghao);
		other_disp16(6, 0, dianliu);
		other_disp16(6, 8, zimu_a);
		other_disp16(6, 16, fenghao);
		temp&=0xEf;	  					//Mcs = 0;Scs = 1;
		PC8155=temp;
		temp|=0x20;
		PC8155=temp;
		other_disp16(0,56, fu);
		other_disp16(2,56, fu);
		other_disp16(4,56, fu);
		other_disp16(6,56, an);
		Read62256(channel);
		for(i=0;i<4;i++){
			DisPlay_uint(disBuf_U_I[i],i);
		}
	}
	else if(page == 1){     //信息二页                                                                                                               //显示“测试完毕”
		temp|=0x10;
		PC8155=temp;
		temp&=0xDf;
		PC8155=temp;
		//Mcs = 1;Scs = 0;
		other_disp16(0, 0, dianliu);
		other_disp16(0, 8, zimu_b);
		other_disp16(0, 16, fenghao);
		other_disp16(2, 0, dianliu);
		other_disp16(2, 8, zimu_c);
		other_disp16(2, 16, fenghao);
		other_disp16(4, 0, effective_P);//P
		other_disp16(4, 16, fenghao);
		other_disp16(6, 0, reactive_P); //Q
		other_disp16(6, 16, fenghao);
		temp&=0xEf;
		PC8155=temp;
		temp|=0x20;
		PC8155=temp;
		//Mcs = 0;Scs = 1;
		other_disp16(0,56, an);
		other_disp16(2,56, an);
		other_disp16(4,56, wa);
		other_disp16(6,56, wa);
		for(i=4;i<6;i++){
			DisPlay_uint(disBuf_U_I[i],i);
		}
		for(i=0;i<2;i++){
			DisPlay_long(disBuf_P_Q[i],i);	
		}
	}
	else if(page==2){//退出页
		temp|=0x10;
		PC8155=temp;//Mcs = 1;Scs = 0;
		temp&=0xDf;
		PC8155=temp;
		hz_disp16(2, 0, xie);
		hz_disp16(2, 32, xie);
		temp&=0xEf;	  	//Mcs = 0;Scs = 1;
		PC8155=temp;
		temp|=0x20;
		PC8155=temp;
		hz_disp16(2,0, shi);
		hz_disp16(2,32, yong);
	}
}
/*---------------------定时器设置---------------------------------------------*/
void TimerConfig(void){
	TMOD=0x01;           //模式设置，00000001，可见采用的是定时器0，工作与模式1（M1=0，M0=1）。
	TR0=1;                   //打开定时器
	TH0=(65536-50000)/256;// 由于晶振为12Mhz,故所记次数应50000，计时器每隔50000微秒发起一次中断。
	TL0=(65536-50000)%256;//50000的来历，为50000*11.0592/12
	ET0=1;                   //开定时器0中断
	EA=1;                    //开总中断
}
/*---------------------定时器中断服务-----------------------------------------*/
void time0(void) interrupt 1{   
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;  
}
/*---------------------初始化外部中断0----------------------------------------*/
void Outside_Init(void){
	EX0=1; //开外部中断0
	IT0=1; //下降沿触发方式
	EA=1;  //开总中�
}
/*---------------------外部中断处理程序---------------------------------------*/
int ReadAD1674(int U_I_flag){
	int value;
	uint d;
	uchar H_value,L_value;
	if(U_I_flag==CURRENT){
		G1=0;G2=1;G3=0;//选择高8位电流
		H_value=P0;
		G1=0;G2=0;G3=1;//选择输出低4位电流
		L_value=P0;	//p0口低四位
		d=(H_value<<8)|(L_value<<4);//高位和低位合成，L_value为P0口的低四位
	}
	if(U_I_flag==VOLUTAGE){
		G1=0;G2=0;G3=0;//选择高8位电压
		H_value=P0;
		G1=0;G2=0;G3=1;//选择输出低4位电压
		L_value=P0;
		d=(H_value<<8)|L_value;//高位和低位合成，temp2为P0口的吣�
	}  
	d=d>>4;
	if((d>0x000)&&(d<0x800)){   
		d=0x800-d;   
		biaozhiwei=0;  
	}   
	if(d==0x000){   
		d=d+2047;     
		biaozhiwei=0;  
	}  
	if(d==0x800){   
		d=d-0x800;    
		biaozhiwei=1;  
	}
	if(d>0x800){   
		d=d-0x800;    
		biaozhiwei=1;  
	}
	value=(int)(d*adio);//转换为可存储的电压或者电流
	if(biaozhiwei==0){//为负数时候
		value=-value;
	}
	return value;
}
void Outside_process(void)interrupt  0{
	//判断中断的方式
	if(ADINT==0){//AD测量中断
		if(flage==1){
			ADSTART=1;//开启数据并行输出
			u=ReadAD1674(VOLUTAGE);
			i=ReadAD1674(CURRENT);
			flage=0;                                                        
		}
	}
	if(FREINT==0){//频率中断
		if(times==0){
			x0=TH0*256+TL0;
			times++;
			return;
		}
		if(times==1){
			x1=TH0*256+TL0;
			times++;
			return;
		}
	}    
}
/*---------------------测量一条线路的频率-------------------------------------*/
void Measure_Fre(uchar s){
	uchar i,j;
	uint sum=0;
	ADINT=1;
	FREINT=0;//接通频率中断
	config_8155_B();//选中8155扩张控制多路切换的芯片；
	for(i=s,j=0;i<s+3;i++,j++){
		PA8155=i;       //接通一条线路的一相，只测电压的频率：电流的频率和电压的频率相同
		TimerConfig();//启动定时器
		Outside_Init();//启动外部中断 当有外部中断（下降沿信号）触发两次，外部中断服务中记录两次的中断时间
		while(times!=2);
		EX0=0;           //关外部中断0
		ET0=0;                   //关定时器0中断
		EA=0;                    //关总中断
		fre[j]=x1-x0;    //计算出信号的频率周期
		sum+=fre[j];
		times=0;                 //乱淮饪都剖跏蓟�
	}
	real_fre=sum/3;//计算出一条线路的信号频率
}
/*---------------------测量一条线路电压和电流（32点）-------------------------*/
void Measure_U_I(uchar s){
	uchar temp;
	uchar j;
	uint Tss=((real_fre/96)-25)*16;//8155脉冲计数：减去25us相当于消耗25us用于12位AD转换 (1/16)us外部有源晶振的时钟周期
	//乘以16：Ts/(1/16)==脉冲计数次数
	ADINT=0;
	FREINT=1;//关闭频率中断,允许AD中断
	for(j=0;j<32;j++){       //每相32次测电流和电压
       for(temp=s;temp<s+3;temp++){ //三次三相
			config_8155_B();//选中多路切换8155
			PA8155=temp;//电压接通
			PB8155=temp;//电流接通
			LT8155=Tss%256;//设定计数电压频率50Hz 周期为0.02s，所以Tss大约等于208.3333....微妙 。8155进行16MHz分频-计数208.333/(1/16)=3333(0d05)
			HT8155=Tss/256;//设置输出
			COM8155=0X0F|0XC0;//开启计数
			while(COM8155&0X40); //计数到数
			flage=1;
			ADSTART=1;//启动AD转换
			while(flage);
			//向62256数据存储器中写入检测数据
			G1=1;G2=0;G3=1;//选中62256，向62256数据存储器中写入检测数据
			if(temp==s){            //A相
				jacqu_volutage_arrayAA=(int xdata *)jacqu_addr[0];
				jacqu_current_arrayAA=(int xdata *)jacqu_addr[1];
				*jacqu_volutage_arrayAA=u;
				*jacqu_current_arrayAA=i;
				jacqu_volutage_arrayAA++;
				jacqu_current_arrayAA++;
			}
			if(temp==(s+1)){        //B相
				jacqu_volutage_arrayBB=(int xdata *)jacqu_addr[2];
				jacqu_current_arrayBB=(int xdata *)jacqu_addr[3];
				*jacqu_volutage_arrayBB=u;
				*jacqu_current_arrayBB=i;
				jacqu_volutage_arrayBB++;
				jacqu_current_arrayBB++;
			}
			if(temp==(s+2)){        //C相
				jacqu_volutage_arrayBB=(int xdata *)jacqu_addr[4];
				jacqu_current_arrayBB =(int xdata *)jacqu_addr[5];
				*jacqu_volutage_arrayCC=u;
				*jacqu_current_arrayCC=i;
				jacqu_volutage_arrayCC++;
				jacqu_current_arrayCC++;
			}               
      }
	}
}
void Acs_working(void){
	uchar i,j;
	uchar channel=0,choice;
	while(1){
		channel = KeyBoardScan();
		if(channel>=1&&channel<=5){
			break;
		}
	}
	temp&=0xEF;
	PC8155_A=temp;
	temp|=0x20;
	PC8155_A=temp;//Mcs = 0;Scs = 1;
	other_disp16(4,0,num[channel]);//显示线路号
	for(i=0x08,j=0;i<0x17;i+=3,j++){
		Measure_Fre(i);//测量一条线路的频率
		Measure_U_I(i);//测量一条线路的电压和电流
		G1=1;G2=0;G3=1;//62256芯片选中
		U_I_Data=(uint  *)(UIaddr[j]);
		P_Q_Data=(long  *)(PQaddr[j]);
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayAA);U_I_Data++;		 //存第j+1条线路的Ua,Ub,Uc,Ia,Ib,Ic,P,Q
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayBB);U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayCC);U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayAA); U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayBB); U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayCC); U_I_Data++;
		*P_Q_Data=JiSuan_P();P_Q_Data++;
		*P_Q_Data=JiSuan_Q();
	}
	config_8155_A(); //显示主机界面
	Msg(0,channel);   //线路信息显示：第一页
	choice= KeyBoardScan();//下一页
	while(choice!=7){
		choice= KeyBoardScan();
	}
	Msg(1,channel);	  //呗信息显示  第二页
}
uchar operate(uchar KeyValue){
	switch(KeyValue){
		case(9):other_disp16(4,0,num[0]);delay(10000);Acs_working();KeyValue=6;//开始
		case(6):Msg(2,0);return 0;//退出 
	}
	return 1;
}