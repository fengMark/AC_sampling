#include<Solve.h>
/*----------------����һ����·�ĵ�ѹ�͵�����A,B,C���ĵ�ַ(VarrayA,CarrayA,VarrayB,CarrayB,VarrayC,CarrayC)---------*/
uint  code jacqu_addr[6]={0x0200,0x0300,0x0400,0x0500,0x0600,0x0700};
int  xdata *  jacqu_volutage_arrayAA=0;		//0
int  xdata *  jacqu_volutage_arrayBB=0;		//2
int  xdata *  jacqu_volutage_arrayCC=0;		//4
int  xdata *  jacqu_current_arrayAA=0;		//1
int  xdata *  jacqu_current_arrayBB=0;		//3
int  xdata *  jacqu_current_arrayCC=0;		//5
static uint sqrt(unsigned long M){
	unsigned int N, i;
	unsigned long tmp, ttp; // �����ѭ������
	if(M == 0) // �����������������ҲΪ0
		return 0;
	N = 0;
	tmp = (M >> 30); // ��ȡ���λ��B[m-1]
	M<<=2;
	if(tmp > 1) // ���λΪ1
	{
		N ++; // �����ǰλΪ1������ΪĬ�ϵ�0
		tmp -= N;
	}
	for(i=15; i>0; i--) // ��ʣ���15λ
	{
		N <<= 1; // ����һλ
		tmp <<= 2;
		tmp += (M >> 30); // ����
		ttp = N;
		ttp = (ttp<<1)+1;
		M <<= 2;
		if(tmp >= ttp) // �������
		{
			tmp -= ttp;
			N++;
		}
	}
	return N;
}
uint JiSuan_U_I(int  *array){
	uchar i;
	uint value;
	unsigned long sum=0;
	for(i=0;i<32;i++){
		sum+=(*array)*(*array);
		array++;
	}
	value=sqrt(sum/32);
	return value;
}
long JiSuan_P(){		 
	uchar i;
	long Pa,Pb,Pc,P;
	jacqu_volutage_arrayAA=(int xdata *)jacqu_addr[0];
	jacqu_current_arrayAA=(int xdata *)jacqu_addr[1];
	jacqu_volutage_arrayBB=(int xdata *)jacqu_addr[2];
	jacqu_current_arrayBB=(int xdata *)jacqu_addr[3];
	jacqu_volutage_arrayBB=(int xdata *)jacqu_addr[4];
	jacqu_current_arrayBB =(int xdata *)jacqu_addr[5];
	for(i=0;i<32;i++){
		Pa+=(jacqu_current_arrayAA[i]*jacqu_volutage_arrayAA[i]);
	}
	Pa=Pa/32;
	for(i=0;i<32;i++){
		Pb+=(jacqu_current_arrayBB[i]*jacqu_volutage_arrayBB[i]);
	}
	Pb=Pb/32;
	for(i=0;i<32;i++){
		Pc+=(jacqu_current_arrayCC[i]*jacqu_volutage_arrayCC[i]);
	}
	Pc=Pc/32;
	P=Pa+Pb+Pc;
	return P;
}
long JiSuan_Q(){		 
	uchar i;
	int Q;
	signed long Qa,Qb,Qc;
	jacqu_volutage_arrayAA=(int xdata *)jacqu_addr[0];
	jacqu_current_arrayAA=(int xdata *)jacqu_addr[1];
	jacqu_volutage_arrayBB=(int xdata *)jacqu_addr[2];
	jacqu_current_arrayBB=(int xdata *)jacqu_addr[3];
	jacqu_volutage_arrayBB=(int xdata *)jacqu_addr[4];
	jacqu_current_arrayBB =(int xdata *)jacqu_addr[5];
	Qa=(jacqu_current_arrayAA[0]*jacqu_volutage_arrayAA[31]-jacqu_current_arrayAA[31]*jacqu_volutage_arrayAA[0])/2;
	Qb=(jacqu_current_arrayBB[0]*jacqu_volutage_arrayBB[31]-jacqu_current_arrayBB[31]*jacqu_volutage_arrayBB[0])/2;
	Qc=(jacqu_current_arrayCC[0]*jacqu_volutage_arrayCC[31]-jacqu_current_arrayCC[31]*jacqu_volutage_arrayCC[0])/2;
	for(i=1;i<32;i++){
		Qa+=((jacqu_current_arrayAA[i+1]*jacqu_volutage_arrayAA[i]-jacqu_current_arrayAA[i]*jacqu_volutage_arrayAA[i+1])/2);
	}
	Qa/=32;
	for(i=1;i<32;i++){
		Qb+=((jacqu_current_arrayBB[i+1]*jacqu_volutage_arrayBB[i]-jacqu_current_arrayBB[i]*jacqu_volutage_arrayBB[i+1])/2);
	}
	Qb/=32;
	for(i=1;i<32;i++){
		Qc+=((jacqu_current_arrayCC[i+1]*jacqu_volutage_arrayCC[i]-jacqu_current_arrayCC[i]*jacqu_volutage_arrayCC[i+1])/2);
	}
	Qc/=32;
	Q=Qa+Qb+Qc;
	return Q;
}