/******************************* sys Ver3.6 ˵�� ************************************************
  (1) sys.c����STC��������ܹ����ṩ��
	      a, ϵͳ��ʼ��MySTC_Init(),ϵͳ���Ⱥ���MySTC_OS(),�����û��ص�����SetEventCallBack()
				b, ���ɿ����úʹ����ص��������¼���
					          enumEventSys1mS       : 1mS��ʱ��
										enumEventSys10mS      : 10mS��ʱ��
										enumEventSys100mS     : 100mS��ʱ��
										enumEventSys1S        : 1S��ʱ��
										enumEventKey          : �����¼�
										enumEventNav          : ���������¼���						
										enumEventHall         : hall�������¼�
										enumEventVib          : �񶯴������¼�
										enumEventXADC         : ��չ�ӿ����µ�ADֵ�¼�
										enumEventUart1Rxd     : ����1���յ�һ�����ϸ�ʽ��������ݰ��¼�������1��USB����PC��ͨ��
										enumEventUart2Rxd     : ����2���յ�һ�����ϸ�ʽ��������ݰ��¼�������2��485��UART��EXT��UART����485��ModBusЭ�飨2022��5��2��������
										enumEventIrRxd        : �����յ�һ�����ݰ��¼�
        c, ��ȡϵͳ�����������۲���
				
	(2) MySTC_Init()��sys��ʼ������������ִ��һ�Ρ�
  (3) MySTC_OS()��  sys���Ⱥ�����Ӧ��while��1��ѭ���С�	
  (4) SetEventCallBack(char event, void *(user_callback)):����"�¼�"�û��ص�����.
	(5) SysPerF GetSysPerformance(void)  :��ȡϵͳ����������������
	     ������������
			 ��������ֵ���ṹ struct SysPerF���������£�
					typedef struct                   //ϵͳ������������,ÿ�����һ��
						{ unsigned long MainLoops;            //SysPerformance.MainLoops            ÿ����ѭ��������Ӧ����1000���ϣ�
							unsigned char PollingMisses;        //SysPerformance.PollingMisses        ÿ����ѯ��ʧ����������ֵΪ0������з�0�����˵���û�������㷨�������⣩
						} SysPerF;	
			
	(6) ����˵����
	      sys.c ��������ʱ��Ϊ1mS������ռʽ��Ҫ���û�����Ƭ�Σ��䵥��ѭ��ִ��ʱ���ۼ�����ӦС��1mS��
		
��д����ɣ��绰18008400450��   2021��2��26����ƣ�2022��5��2�ո���
*/

#ifndef	_sys_h_
#define _sys_h_

typedef struct                     //ϵͳ������������,ÿ�����һ��
  { unsigned long MainLoops;            //SysPerformance.MainLoops            ÿ����ѭ��������Ӧ����1000���ϣ�
		unsigned char PollingMisses;        //SysPerformance.PollingMisses        ÿ����ѯ��ʧ����������ֵΪ0������з�0�����˵���û�������㷨�������⣩
	} struct_SysPerF;
	
extern void MySTC_Init();             //sys��ʼ������
extern void MySTC_OS();               //sys���Ⱥ�����Ӧ��while��1��ѭ����                                     
extern void SetEventCallBack(char event, void *(user_callback));  //����"�¼�"�û��ص�����.
extern struct_SysPerF GetSysPerformance(void); 
		
enum event{enumEventSys1mS,                      //ϵͳ1mS�¼�
	         enumEventSys10mS,                     //ϵͳ10mS�¼�
           enumEventSys100mS,                    //ϵͳ100mS�¼�
           enumEventSys1S,                       //ϵͳ1S�¼�
           enumEventKey,                         //�����¼�
           enumEventHall,                        //hall�������¼�
           enumEventVib,                         //�񶯴������¼�
           enumEventNav,                         //���������¼���
           enumEventXADC,                        //��չ�ӿ����µ�ADֵ�¼�
           enumEventUart1Rxd,                    //����1���յ�һ�����ϸ�ʽ������¼�
	         enumEventUart2Rxd,                    //����2���յ�һ�����ϸ�ʽ���壨"���ݰ���ʽ"����"ModBusЭ��+���ݰ���ʽ"�����¼���    ��2022��5��2�������� 
	         enumEventIrRxd                        //����Ir���յ�һ�����ݰ�
          };                         
	
#endif