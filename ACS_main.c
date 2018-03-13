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
#define adio (1000*10/4095)//×ª»»½øÂÊ£¬12Î»¾«¶È(2^12-1=4095)£¬ÂúÁ¿³ÌÎª10V:×ª»»Îª¿É´æ´¢µÄµçÑ¹Öµ
/*----------62256´æ´¢Æ÷µØÖ·-------------------------*/
uint code UIaddr[5]={0x0000,0x0010,0x0020,0x0030,0x0040};
uint code PQaddr[5]={0x0100,0x0110,0x0120,0x0130,0x0140}; 
uint xdata *  U_I_Data;//ÕâÀï¶¨ÒåÊÇÒ»¶¨Òª×¢ÒâÊı¾İÀàĞÍ£¬ÕâÀïµÄÀàĞÍÊÇÖ¸µØÖ·ÀïµÄUa,Ub,Uc,Ia,Ib,Ic
long xdata *  P_Q_Data;
/*----------8155¶Ë¿ÚºÍÃüÁî×´Ì¬¼Ä´æÆ÷µØÖ·-------------*/
#define COM8155 XBYTE[0X7f00]
#define PA8155  XBYTE[0X7f01]
#define PB8155  XBYTE[0X7f02]
#define PC8155  XBYTE[0X7f03]
#define LT8155  XBYTE[0X7F04]
#define HT8155  XBYTE[0x7f05]
/*--------------------38ÒëÂëÆ÷Ğ¾Æ¬Òı½Å-----------------------*/
sbit G1 = P1^7;  
sbit G2 = P1^6;
sbit G3 = P1^5;
sbit ADSTART  = P1^4;           
sbit ADINT  = P1^3;             
sbit FREINT = P1^2;             
sbit PS2 = P1^1; 
sbit PS1 = P1^0;
/*----------------8155¶¨Ê±Æ÷µÄ¸ßµÍÎ»µÄ¼ÆÊı´ÎÊı---------------------*/
uchar HTime;
uchar LTime;
/*---------------------ÆµÂÊÖÜÆÚ-------------------------------*/
uint fre[3],real_fre;
uint x0,x1;
uchar times=0;
uchar flage;
/*--------------------µçÑ¹ºÍµçÁ÷µÄ»ù×¼Öµ-----------------------*/ 
uchar biaozhiwei;
int u,i;
/*------------------------ÏÔÊ¾Êı×é---------------------------*/
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
/*---------------------Ö÷³ÌĞò-------------------------------------------------*/
void main(void){
	uchar flage=1;
	uchar choice=0;
	ADINT=1;
	FREINT=1;
	config_8155_A();//Ñ¡ÖĞ12864ºÍ¼üÅÌ
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

/*---------------------ÅäÖÃ8155(ÍâÉè12864ºÍ¼üÅÌ)Ğ¾Æ¬--------------------------*/
void config_8155_A(){
	G1=1;
	G2=1;
	G3=0;
	COM8155_A=0X0f;
}
/*---------------------ÅäÖÃ8155(¿ØÖÆ¶àÂ·ÇĞ»»)Ğ¾Æ¬-----------------------------*/
void config_8155_B(){ //¶àÂ·ÇĞ»»
			G1=0;
			G2=0;
			G3=1;
			COM8155=0X0f;
}
/*---------------------´Ó62256´æ´¢Æ÷ÖĞ²éÕÒÊı¾İ--------------------------------*/
void Read62256(uchar channel){
	uchar i;
	U_I_Data=(uint  *)(UIaddr[channel-1]);
	P_Q_Data=(long  *)(PQaddr[channel-1]);
	G1=1;G2=0;G3=1;//Ñ¡ÖĞ62256£¬´Ó62256ÖĞÕÒÊı¾İ
	for(i=0;i<6;i++){
		disBuf_U_I[i]=*U_I_Data; //È¡ÄÚÈİ
		U_I_Data++;
	}
	for(i=0;i<2;i++){
		disBuf_P_Q[i]=*P_Q_Data; //È¡ÄÚÈİ
		P_Q_Data++;
	}
}
/*---------------------ÏÔÊ¾µçÑ¹ºÍµçÁ÷Öµ---------------------------------------*/
void DisPlay_uint(uint value,uchar i){
	uchar wei;
	config_8155_A();//Ñ¡ÖĞ8155À©Õ¹µÄ12864ºÍ¼üÅÌ
	temp&=0xEf;	  					//Mcs = 0;Scs = 1;
	PC8155=temp;
	temp|=0x20;
	PC8155=temp;
	wei=value/1000;		  //ÓÒÆÁÏÔÊ¾µçÑ¹ÓĞĞ§ÖµºÍµçÁ÷ÓĞĞ§Öµ
	other_disp16(2*i,0,num[wei]);//¸öÎ»Êı×Ö	0.000
	other_disp16(2*i,8,dot);	 //Ğ¡Êıµã
	wei=value%1000/100;
	other_disp16(2*i,16,num[wei]);
	wei=value%1000%100/10;
	other_disp16(2*i,24,num[wei]);
	wei=value%1000%100%10;
	other_disp16(2*i,32,num[wei]);
}
/*---------------------ÏÔÊ¾ÓĞĞ§ºÍÎŞĞ§µÄ¹¦ÂÊÖµ---------------------------------*/
void DisPlay_long(long value,uchar i){
	uchar wei;
	config_8155_A();
	temp|=0x10;						//Mcs = 1;Scs = 0;
	PC8155=temp;
	temp&=0xDf;
	PC8155=temp;
	if(value<0){			//¸ººÅºÍÕûÊı²¿·Ö×ó±ßµÄÆÁÏÔÊ¾
		other_disp16(2*(i+2),40,fuhao);	
	}
	wei=value/10000000;
	other_disp16(2*(i+2),48,num[wei]);
	wei=value%10000000/1000000;
	other_disp16(2*(i+2),56,num[wei]);
	temp&=0xEf;	  					//Mcs = 0;Scs = 1;
	PC8155=temp;
	temp|=0x20;
	PC8155=temp;			 //Ğ¡ÊıµãºÍĞ¡Êı²¿·ÖÓÒÆÁÏÔÊ¾
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
/*---------------------ĞÅÏ¢ÏÔÊ¾:¿ÉÓÃĞÅÏ¢ºÍÍË³öÒ³Ãæ----------------------------*/
void Msg(int page,uchar channel){			 
	uchar i; 
	Clr_Scr();                                                                                              //ÇåÆÁ                                                                                                  //ÏÔÊ¾±ß¿ò
	if(page == 0)    //ĞÅÏ¢Ò»Ò³
	{                                                                                                               //ÏÔÊ¾¡°²âÊÔ¿ªÊ¼¡±
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
	else if(page == 1){     //ĞÅÏ¢¶şÒ³                                                                                                               //ÏÔÊ¾¡°²âÊÔÍê±Ï¡±
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
	else if(page==2){//ÍË³öÒ³
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
/*---------------------¶¨Ê±Æ÷ÉèÖÃ---------------------------------------------*/
void TimerConfig(void){
	TMOD=0x01;           //Ä£Ê½ÉèÖÃ£¬00000001£¬¿É¼û²ÉÓÃµÄÊÇ¶¨Ê±Æ÷0£¬¹¤×÷ÓëÄ£Ê½1£¨M1=0£¬M0=1£©¡£
	TR0=1;                   //´ò¿ª¶¨Ê±Æ÷
	TH0=(65536-50000)/256;// ÓÉÓÚ¾§ÕñÎª12Mhz,¹ÊËù¼Ç´ÎÊıÓ¦50000£¬¼ÆÊ±Æ÷Ã¿¸ô50000Î¢Ãë·¢ÆğÒ»´ÎÖĞ¶Ï¡£
	TL0=(65536-50000)%256;//50000µÄÀ´Àú£¬Îª50000*11.0592/12
	ET0=1;                   //¿ª¶¨Ê±Æ÷0ÖĞ¶Ï
	EA=1;                    //¿ª×ÜÖĞ¶Ï
}
/*---------------------¶¨Ê±Æ÷ÖĞ¶Ï·şÎñ-----------------------------------------*/
void time0(void) interrupt 1{   
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;  
}
/*---------------------³õÊ¼»¯Íâ²¿ÖĞ¶Ï0----------------------------------------*/
void Outside_Init(void){
	EX0=1; //¿ªÍâ²¿ÖĞ¶Ï0
	IT0=1; //ÏÂ½µÑØ´¥·¢·½Ê½
	EA=1;  //¿ª×ÜÖĞÏ
}
/*---------------------Íâ²¿ÖĞ¶Ï´¦Àí³ÌĞò---------------------------------------*/
int ReadAD1674(int U_I_flag){
	int value;
	uint d;
	uchar H_value,L_value;
	if(U_I_flag==CURRENT){
		G1=0;G2=1;G3=0;//Ñ¡Ôñ¸ß8Î»µçÁ÷
		H_value=P0;
		G1=0;G2=0;G3=1;//Ñ¡ÔñÊä³öµÍ4Î»µçÁ÷
		L_value=P0;	//p0¿ÚµÍËÄÎ»
		d=(H_value<<8)|(L_value<<4);//¸ßÎ»ºÍµÍÎ»ºÏ³É£¬L_valueÎªP0¿ÚµÄµÍËÄÎ»
	}
	if(U_I_flag==VOLUTAGE){
		G1=0;G2=0;G3=0;//Ñ¡Ôñ¸ß8Î»µçÑ¹
		H_value=P0;
		G1=0;G2=0;G3=1;//Ñ¡ÔñÊä³öµÍ4Î»µçÑ¹
		L_value=P0;
		d=(H_value<<8)|L_value;//¸ßÎ»ºÍµÍÎ»ºÏ³É£¬temp2ÎªP0¿ÚµÄßÄ»
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
	value=(int)(d*adio);//×ª»»Îª¿É´æ´¢µÄµçÑ¹»òÕßµçÁ÷
	if(biaozhiwei==0){//Îª¸ºÊıÊ±ºò
		value=-value;
	}
	return value;
}
void Outside_process(void)interrupt  0{
	//ÅĞ¶ÏÖĞ¶ÏµÄ·½Ê½
	if(ADINT==0){//AD²âÁ¿ÖĞ¶Ï
		if(flage==1){
			ADSTART=1;//¿ªÆôÊı¾İ²¢ĞĞÊä³ö
			u=ReadAD1674(VOLUTAGE);
			i=ReadAD1674(CURRENT);
			flage=0;                                                        
		}
	}
	if(FREINT==0){//ÆµÂÊÖĞ¶Ï
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
/*---------------------²âÁ¿Ò»ÌõÏßÂ·µÄÆµÂÊ-------------------------------------*/
void Measure_Fre(uchar s){
	uchar i,j;
	uint sum=0;
	ADINT=1;
	FREINT=0;//½ÓÍ¨ÆµÂÊÖĞ¶Ï
	config_8155_B();//Ñ¡ÖĞ8155À©ÕÅ¿ØÖÆ¶àÂ·ÇĞ»»µÄĞ¾Æ¬£»
	for(i=s,j=0;i<s+3;i++,j++){
		PA8155=i;       //½ÓÍ¨Ò»ÌõÏßÂ·µÄÒ»Ïà£¬Ö»²âµçÑ¹µÄÆµÂÊ£ºµçÁ÷µÄÆµÂÊºÍµçÑ¹µÄÆµÂÊÏàÍ¬
		TimerConfig();//Æô¶¯¶¨Ê±Æ÷
		Outside_Init();//Æô¶¯Íâ²¿ÖĞ¶Ï µ±ÓĞÍâ²¿ÖĞ¶Ï£¨ÏÂ½µÑØĞÅºÅ£©´¥·¢Á½´Î£¬Íâ²¿ÖĞ¶Ï·şÎñÖĞ¼ÇÂ¼Á½´ÎµÄÖĞ¶ÏÊ±¼ä
		while(times!=2);
		EX0=0;           //¹ØÍâ²¿ÖĞ¶Ï0
		ET0=0;                   //¹Ø¶¨Ê±Æ÷0ÖĞ¶Ï
		EA=0;                    //¹Ø×ÜÖĞ¶Ï
		fre[j]=x1-x0;    //¼ÆËã³öĞÅºÅµÄÆµÂÊÖÜÆÚ
		sum+=fre[j];
		times=0;                 //ÂÒ»´â¿¶¼ÆÊı³õÊ¼»¯
	}
	real_fre=sum/3;//¼ÆËã³öÒ»ÌõÏßÂ·µÄĞÅºÅÆµÂÊ
}
/*---------------------²âÁ¿Ò»ÌõÏßÂ·µçÑ¹ºÍµçÁ÷£¨32µã£©-------------------------*/
void Measure_U_I(uchar s){
	uchar temp;
	uchar j;
	uint Tss=((real_fre/96)-25)*16;//8155Âö³å¼ÆÊı£º¼õÈ¥25usÏàµ±ÓÚÏûºÄ25usÓÃÓÚ12Î»AD×ª»» (1/16)usÍâ²¿ÓĞÔ´¾§ÕñµÄÊ±ÖÓÖÜÆÚ
	//³ËÒÔ16£ºTs/(1/16)==Âö³å¼ÆÊı´ÎÊı
	ADINT=0;
	FREINT=1;//¹Ø±ÕÆµÂÊÖĞ¶Ï,ÔÊĞíADÖĞ¶Ï
	for(j=0;j<32;j++){       //Ã¿Ïà32´Î²âµçÁ÷ºÍµçÑ¹
       for(temp=s;temp<s+3;temp++){ //Èı´ÎÈıÏà
			config_8155_B();//Ñ¡ÖĞ¶àÂ·ÇĞ»»8155
			PA8155=temp;//µçÑ¹½ÓÍ¨
			PB8155=temp;//µçÁ÷½ÓÍ¨
			LT8155=Tss%256;//Éè¶¨¼ÆÊıµçÑ¹ÆµÂÊ50Hz ÖÜÆÚÎª0.02s£¬ËùÒÔTss´óÔ¼µÈÓÚ208.3333....Î¢Ãî ¡£8155½øĞĞ16MHz·ÖÆµ-¼ÆÊı208.333/(1/16)=3333(0d05)
			HT8155=Tss/256;//ÉèÖÃÊä³ö
			COM8155=0X0F|0XC0;//¿ªÆô¼ÆÊı
			while(COM8155&0X40); //¼ÆÊıµ½Êı
			flage=1;
			ADSTART=1;//Æô¶¯AD×ª»»
			while(flage);
			//Ïò62256Êı¾İ´æ´¢Æ÷ÖĞĞ´Èë¼ì²âÊı¾İ
			G1=1;G2=0;G3=1;//Ñ¡ÖĞ62256£¬Ïò62256Êı¾İ´æ´¢Æ÷ÖĞĞ´Èë¼ì²âÊı¾İ
			if(temp==s){            //AÏà
				jacqu_volutage_arrayAA=(int xdata *)jacqu_addr[0];
				jacqu_current_arrayAA=(int xdata *)jacqu_addr[1];
				*jacqu_volutage_arrayAA=u;
				*jacqu_current_arrayAA=i;
				jacqu_volutage_arrayAA++;
				jacqu_current_arrayAA++;
			}
			if(temp==(s+1)){        //BÏà
				jacqu_volutage_arrayBB=(int xdata *)jacqu_addr[2];
				jacqu_current_arrayBB=(int xdata *)jacqu_addr[3];
				*jacqu_volutage_arrayBB=u;
				*jacqu_current_arrayBB=i;
				jacqu_volutage_arrayBB++;
				jacqu_current_arrayBB++;
			}
			if(temp==(s+2)){        //CÏà
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
	other_disp16(4,0,num[channel]);//ÏÔÊ¾ÏßÂ·ºÅ
	for(i=0x08,j=0;i<0x17;i+=3,j++){
		Measure_Fre(i);//²âÁ¿Ò»ÌõÏßÂ·µÄÆµÂÊ
		Measure_U_I(i);//²âÁ¿Ò»ÌõÏßÂ·µÄµçÑ¹ºÍµçÁ÷
		G1=1;G2=0;G3=1;//62256Ğ¾Æ¬Ñ¡ÖĞ
		U_I_Data=(uint  *)(UIaddr[j]);
		P_Q_Data=(long  *)(PQaddr[j]);
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayAA);U_I_Data++;		 //´æµÚj+1ÌõÏßÂ·µÄUa,Ub,Uc,Ia,Ib,Ic,P,Q
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayBB);U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_volutage_arrayCC);U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayAA); U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayBB); U_I_Data++;
		*U_I_Data=JiSuan_U_I(jacqu_current_arrayCC); U_I_Data++;
		*P_Q_Data=JiSuan_P();P_Q_Data++;
		*P_Q_Data=JiSuan_Q();
	}
	config_8155_A(); //ÏÔÊ¾Ö÷»ú½çÃæ
	Msg(0,channel);   //ÏßÂ·ĞÅÏ¢ÏÔÊ¾£ºµÚÒ»Ò³
	choice= KeyBoardScan();//ÏÂÒ»Ò³
	while(choice!=7){
		choice= KeyBoardScan();
	}
	Msg(1,channel);	  //ßÂĞÅÏ¢ÏÔÊ¾  µÚ¶şÒ³
}
uchar operate(uchar KeyValue){
	switch(KeyValue){
		case(9):other_disp16(4,0,num[0]);delay(10000);Acs_working();KeyValue=6;//¿ªÊ¼
		case(6):Msg(2,0);return 0;//ÍË³ö 
	}
	return 1;
}