#ifndef _SOLVE_H_
#define _SOLVE_H_
#define uint unsigned int
#define uchar unsigned char
extern uint code jacqu_addr[6];
extern int  xdata *  jacqu_volutage_arrayAA;
extern int  xdata *  jacqu_volutage_arrayBB;
extern int  xdata *  jacqu_volutage_arrayCC;
extern int  xdata *  jacqu_current_arrayAA;
extern int  xdata *  jacqu_current_arrayBB;
extern int  xdata *  jacqu_current_arrayCC;  
//extern int  jacqu_volutage_arrayAA[32],jacqu_volutage_arrayBB[32],jacqu_volutage_arrayCC[32];//第二数组	   	计算程序读出
//extern int  jacqu_current_arrayAA[32],jacqu_current_arrayBB[32],jacqu_current_arrayCC[33];
extern uint JiSuan_U_I(int array[]);
extern long JiSuan_P();
extern long JiSuan_Q();
#endif