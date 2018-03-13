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
#define adio (1000*10/4095)//ת�����ʣ�12λ����(2^12-1=4095)��������Ϊ10V:ת��Ϊ�ɴ洢�ĵ�ѹֵ
/*----------62256�洢����ַ-------------------------*/
uint code UIaddr[5]={0x0000,0x0010,0x0020,0x0030,0x0040};
uint code PQaddr[5]={0x0100,0x0110,0x0120,0x0130,0x0140}; 
uint xdata *  U_I_Data;//���ﶨ����һ��Ҫע���������ͣ������������ָ��ַ���Ua,Ub,Uc,Ia,Ib,Ic
long xdata *  P_Q_Data;
/*----------8155�˿ں�����״̬�Ĵ�����ַ-------------*/
#define COM8155 XBYTE[0X7f00]
#define PA8155  XBYTE[0X7f01]
#define PB8155  XBYTE[0X7f02]
#define PC8155  XBYTE[0X7f03]
#define LT8155  XBYTE[0X7F04]
#define HT8155  XBYTE[0x7f05]
/*--------------------38������оƬ����-----------------------*/
sbit G1 = P1^7;  
sbit G2 = P1^6;
sbit G3 = P1^5;
sbit ADSTART  = P1^4;           
sbit ADINT  = P1^3;             
sbit FREINT = P1^2;             
sbit PS2 = P1^1; 
sbit PS1 = P1^0;
/*----------------8155��ʱ���ĸߵ�λ�ļ�������---------------------*/
uchar HTime;
uchar LTime;
/*---------------------Ƶ������-------------------------------*/
uint fre[3],real_fre;
uint x0,x1;
uchar times=0;
uchar flage;
/*--------------------��ѹ�͵����Ļ�׼ֵ-----------------------*/ 
uchar biaozhiwei;
int u,i;
/*------------------------��ʾ����---------------------------*/
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
/*---------------------������-------------------------------------------------*/
void main(void){
	uchar flage=1;
	uchar choice=0;
	ADINT=1;
	FREINT=1;
	config_8155_A();//ѡ��12864�ͼ���
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

/*---------------------����8155(����12864�ͼ���)оƬ--------------------------*/
void config_8155_A(){
	G1=1;
	G2=1;
	G3=0;
	COM8155_A=0X0f;
}
/*---------------------����8155(���ƶ�·�л�)оƬ-----------------------------*/
void config_8155_B(){ //��·�л�
			G1=0;
			G2=0;
			G3=1;
			COM8155=0X0f;
}
/*---------------------��62256�洢���в�������--------------------------------*/
void Read62256(uchar channel){
	uchar i;
	U_I_Data=(uint  *)(UIaddr[channel-1]);
	P_Q_Data=(long  *)(PQaddr[channel-1]);
	G1=1;G2=0;G3=1;//ѡ��62256����62256��������
	for(i=0;i<6;i++){
		disBuf_U_I[i]=*U_I_Data; //ȡ����
		U_I_Data++;
	}
	for(i=0;i<2;i++){
		disBuf_P_Q[i]=*P_Q_Data; //ȡ����
		P_Q_Data++;
	}
}
/*---------------------��ʾ��ѹ�͵���ֵ---------------------------------------*/
void DisPlay_uint(uint value,uchar i){
	uchar wei;
	config_8155_A();//ѡ��8155��չ��12864�ͼ���
	temp&=0xEf;	  					//Mcs = 0;Scs = 1;
	PC8155=temp;
	temp|=0x20;
	PC8155=temp;
	wei=value/1000;		  //������ʾ��ѹ��Чֵ�͵�����Чֵ
	other_disp16(2*i,0,num[wei]);//��λ����	0.000
	other_disp16(2*i,8,dot);	 //С����
	wei=value%1000/100;
	other_disp16(2*i,16,num[wei]);
	wei=value%1000%100/10;
	other_disp16(2*i,24,num[wei]);
	wei=value%1000%100%10;
	other_disp16(2*i,32,num[wei]);
}
/*---------------------��ʾ��Ч����Ч�Ĺ���ֵ---------------------------------*/
void DisPlay_long(long value,uchar i){
	uchar wei;
	config_8155_A();
	temp|=0x10;						//Mcs = 1;Scs = 0;
	PC8155=temp;
	temp&=0xDf;
	PC8155=temp;
	if(value<0){			//���ź�����������ߵ�����ʾ
		other_disp16(2*(i+2),40,fuhao);	
	}
	wei=value/10000000;
	other_disp16(2*(i+2),48,num[wei]);
	wei=value%10000000/1000000;
	other_disp16(2*(i+2),56,num[wei]);
	temp&=0xEf;	  					//Mcs = 0;Scs = 1;
	PC8155=temp;
	temp|=0x20;
	PC8155=temp;			 //С�����С������������ʾ
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
/*---------------------��Ϣ��ʾ:������Ϣ���˳�ҳ��----------------------------*/
void Msg(int page,uchar channel){			 
	uchar i; 
	Clr_Scr();                                                                                              //����                                                                                                  //��ʾ�߿�
	if(page == 0)    //��Ϣһҳ
	{                                                                                                               //��ʾ�����Կ�ʼ��
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
	else if(page == 1){     //��Ϣ��ҳ                                                                                                               //��ʾ��������ϡ�
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
	else if(page==2){//�˳�ҳ
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
/*---------------------��ʱ������---------------------------------------------*/
void TimerConfig(void){
	TMOD=0x01;           //ģʽ���ã�00000001���ɼ����õ��Ƕ�ʱ��0��������ģʽ1��M1=0��M0=1����
	TR0=1;                   //�򿪶�ʱ��
	TH0=(65536-50000)/256;// ���ھ���Ϊ12Mhz,�����Ǵ���Ӧ50000����ʱ��ÿ��50000΢�뷢��һ���жϡ�
	TL0=(65536-50000)%256;//50000��������Ϊ50000*11.0592/12
	ET0=1;                   //����ʱ��0�ж�
	EA=1;                    //�����ж�
}
/*---------------------��ʱ���жϷ���-----------------------------------------*/
void time0(void) interrupt 1{   
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;  
}
/*---------------------��ʼ���ⲿ�ж�0----------------------------------------*/
void Outside_Init(void){
	EX0=1; //���ⲿ�ж�0
	IT0=1; //�½��ش�����ʽ
	EA=1;  //�������
}
/*---------------------�ⲿ�жϴ������---------------------------------------*/
int ReadAD1674(int U_I_flag){
	int value;
	uint d;
	uchar H_value,L_value;
	if(U_I_flag==CURRENT){
		G1=0;G2=1;G3=0;//ѡ���8λ����
		H_value=P0;
		G1=0;G2=0;G3=1;//ѡ�������4λ����
		L_value=P0;	//p0�ڵ���λ
		d=(H_value<<8)|(L_value<<4);//��λ�͵�λ�ϳɣ�L_valueΪP0�ڵĵ���λ
	}
	if(U_I_flag==VOLUTAGE){
		G1=0;G2=0;G3=0;//ѡ���8λ��ѹ
		H_value=P0;
		G1=0;G2=0;G3=1;//ѡ�������4λ��ѹ
		L_value=P0;
		d=(H_value<<8)|L_value;//��λ�͵�λ�ϳɣ�temp2ΪP0�ڵ��Ļ
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
	value=(int)(d*adio);//ת��Ϊ�ɴ洢�ĵ�ѹ���ߵ���
	if(biaozhiwei==0){//Ϊ����ʱ��
		value=-value;
	}
	return value;
}
void Outside_process(void)interrupt  0{
	//�ж��жϵķ�ʽ
	if(ADINT==0){//AD�����ж�
		if(flage==1){
			ADSTART=1;//�������ݲ������
			u=ReadAD1674(VOLUTAGE);
			i=ReadAD1674(CURRENT);
			flage=0;                                                        
		}
	}
	if(FREINT==0){//Ƶ���ж�
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
/*---------------------����һ����·��Ƶ��-------------------------------------*/
void Measure_Fre(uchar s){
	uchar i,j;
	uint sum=0;
	ADINT=1;
	FREINT=0;//��ͨƵ���ж�
	config_8155_B();//ѡ��8155���ſ��ƶ�·�л���оƬ��
	for(i=s,j=0;i<s+3;i++,j++){
		PA8155=i;       //��ͨһ����·��һ�ֻ࣬���ѹ��Ƶ�ʣ�������Ƶ�ʺ͵�ѹ��Ƶ����ͬ
		TimerConfig();//������ʱ��
		Outside_Init();//�����ⲿ�ж� �����ⲿ�жϣ��½����źţ��������Σ��ⲿ�жϷ����м�¼���ε��ж�ʱ��
		while(times!=2);
		EX0=0;           //���ⲿ�ж�0
		ET0=0;                   //�ض�ʱ��0�ж�
		EA=0;                    //�����ж�
		fre[j]=x1-x0;    //������źŵ�Ƶ������
		sum+=fre[j];
		times=0;                 //�һ�⿶������ʼ��
	}
	real_fre=sum/3;//�����һ����·���ź�Ƶ��
}
/*---------------------����һ����·��ѹ�͵�����32�㣩-------------------------*/
void Measure_U_I(uchar s){
	uchar temp;
	uchar j;
	uint Tss=((real_fre/96)-25)*16;//8155�����������ȥ25us�൱������25us����12λADת�� (1/16)us�ⲿ��Դ�����ʱ������
	//����16��Ts/(1/16)==�����������
	ADINT=0;
	FREINT=1;//�ر�Ƶ���ж�,����AD�ж�
	for(j=0;j<32;j++){       //ÿ��32�β�����͵�ѹ
       for(temp=s;temp<s+3;temp++){ //��������
			config_8155_B();//ѡ�ж�·�л�8155
			PA8155=temp;//��ѹ��ͨ
			PB8155=temp;//������ͨ
			LT8155=Tss%256;//�趨������ѹƵ��50Hz ����Ϊ0.02s������Tss��Լ����208.3333....΢�� ��8155����16MHz��Ƶ-����208.333/(1/16)=3333(0d05)
			HT8155=Tss/256;//�������
			COM8155=0X0F|0XC0;//��������
			while(COM8155&0X40); //��������
			flage=1;
			ADSTART=1;//����ADת��
			while(flage);
			//��62256���ݴ洢����д��������
			G1=1;G2=0;G3=1;//ѡ��62256����62256���ݴ洢����д��������
			if(temp==s){            //A��
				jacqu_volutage_arrayAA=(int xdata *)jacqu_addr[0];
				jacqu_current_arrayAA=(int xdata *)jacqu_addr[1];
				*jacqu_volutage_arrayAA=u;
				*jacqu_current_arrayAA=i;
				jacqu_volutage_arrayAA++;
				jacqu_current_arrayAA++;
			}
			if(temp==(s+1)){        //B��
				jacqu_volutage_arrayBB=(int xdata *)jacqu_addr[2];
				jacqu_current_arrayBB=(int xdata *)jacqu_addr[3];
				*jacqu_volutage_arrayBB=u;
				*jacqu_current_arrayBB=i;
				jacqu_volutage_arrayBB++;
				jacqu_current_arrayBB++;
			}
			if(temp==(s+2)){        //C��
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
	other_disp16(4,0,num[channel]);//��ʾ��·��
	for(i=0x08,j=0;i<0x17;i+=3,j++){
		Measure_Fre(i);//����һ����·��Ƶ��
		Measure_U_I(i);//����һ����·�ĵ�ѹ�͵���
		G1=1;G2=0;G3=1;//62256оƬѡ��
		U_I_Data=(uint  *)(UIaddr[j]);
		P_Q_Data=(long  *)(PQaddr[j]);
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayAA);U_I_Data++;		 //���j+1����·��Ua,Ub,Uc,Ia,Ib,Ic,P,Q
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayBB);U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayCC);U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayAA); U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayBB); U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayCC); U_I_Data++;
		*P_Q_Data=JiSuan_P();P_Q_Data++;
		*P_Q_Data=JiSuan_Q();
	}
	config_8155_A(); //��ʾ��������
	Msg(0,channel);   //��·��Ϣ��ʾ����һҳ
	choice= KeyBoardScan();//��һҳ
	while(choice!=7){
		choice= KeyBoardScan();
	}
	Msg(1,channel);	  //����Ϣ��ʾ  �ڶ�ҳ
}
uchar operate(uchar KeyValue){
	switch(KeyValue){
		case(9):other_disp16(4,0,num[0]);delay(10000);Acs_working();KeyValue=6;//��ʼ
		case(6):Msg(2,0);return 0;//�˳� 
	}
	return 1;
}