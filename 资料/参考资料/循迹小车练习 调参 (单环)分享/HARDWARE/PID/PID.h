#ifndef __PID_H
#define __PID_H



typedef struct
{
	float Sv;
	float Kp;
	float Kd;
	float Ki;
	float EK;
	float EK_1;//上次偏差
	float SEK;//历史偏差之和
	float OUT;

}PID;


extern PID pid;

void PID_Calc();
//void  Position_P();
void PID_Init();
void PID_OUT();



//int Position_PID (int Encoder,int Target);   //差速PID控制器//这里Encoder=error，Target=0;




#endif
