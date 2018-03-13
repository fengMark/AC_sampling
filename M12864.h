#ifndef _M12864_H_
#define _M12864_H_
#define uint unsigned int
#define uchar unsigned char
/*----------8155�˿ں�����״̬�Ĵ�����ַ-------------*/
#define COM8155_A XBYTE[0X7f00]
#define PA8155_A  XBYTE[0X7f01]
#define PB8155_A  XBYTE[0X7f02]
#define PC8155_A  XBYTE[0X7f03]
/*--------------------12864����-----------------------*/
#define Disp_On 0x3f    //��ʾ����
#define Disp_Off 0x3e   //�ر�����
#define Col_Add 0x40    //��ʼ�е�ַ
#define Page_Add 0xb8   //��ʼҳ��ַ
#define Start_Line 0xc0 //��ʼ�е�ַ
extern uchar temp;
extern void delay(unsigned int t);
extern void Read_busy();
extern void write_com(unsigned char cmdcode);
extern void write_data(unsigned char Dispdata);
extern void frame();
extern void Clr_Scr();
//extern void Disp_Img(unsigned char code *img);
//extern void Disp_H_Line();
//extern void Disp_V_Line();
extern void other_disp16(unsigned char pag, unsigned char col, unsigned char code *hzk);
extern void hz_disp16(unsigned char pag, unsigned char col, unsigned char code *hzk);
extern void init_lcd();
extern uchar  KeyBoardScan(void);
#endif