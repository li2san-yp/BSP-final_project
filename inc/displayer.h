/**********************************display V2.0 ˵�� ************************************************************************
displayer���ڿ��ơ�STC-Bѧϰ�塱��8��7������ܣ�Seg7����8��ָʾ�ƣ�Led������.�ṩ��ʾģ����غ�����Ӧ�ú�����

  (1) displayerInit()����ʾģ����غ�����
	
  (2) SetDisplayerArea(char Begin_of_scan,char Ending_of_Scan)������LED��������8������ܴ������ұ�ŷֱ�Ϊ0����7����������
      Begin_of_scan�趨�����������ʼ��ţ�Ending_of_ScanΪ������š��趨��Χ�ڵ�����ܲŹ�������ʾ��
			ע����������£���������ȡֵ��ΧΪ0����7����Ending_of_Scan>Begin_of_scan�������ö�̬ɨ��������Ӿ�Ч���������ó���
			    �÷�Χ�Ĳ�������ʵ��������ʾЧ����������������ʾ���ȣ���ǵ�������ʾ���ȣ�
					
  (3) Seg7Print(char d0,char d1,char d2,char d3,char d4,char d5,char d6,char d7)����8������ֵ�ֱ�������ʾ����Ӧ��������ϡ�
      ��ʾ�����(code char decode_table[])��main.c��,�û������޸ĺ�����.
			
  (4) LedPrint(char led_val)������8��ָʾ�ƿ��ء�����light_val��8��bitλ��Ӧ8��ָʾ�ƵĿ��أ���1������ָʾ�ơ�����									 

��д����ɣ��绰18008400450��   2021��2��26����ƣ�2021��3��15�ո���
*/

#ifndef _displayer_H_
#define _displayer_H_

// 显示解码表，存储在CODE段以节省DATA段内存
extern code char decode_table[];

extern void DisplayerInit();										                                               //������ʾģ�鹤��
extern void SetDisplayerArea(char Begin_of_scan,char Ending_of_Scan);						               //������Ч��ʾ����
extern void Seg7Print(char d0,char d1,char d2,char d3,char d4,char d5,char d6,char d7);        //������������	
extern void LedPrint(char led_val);

#endif  

