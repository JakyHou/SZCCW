#include "Ram.h"
#include "MC_Globals.h"


void ProcSetKey()
{
		//usSetSpd=usSetSpdBk;//��ȷ�ϼ��ٶȿ��Ʋ���Ч
}

void ProcFunKey()
{
	if (fgRun==0)
	{
	 State= INIT;
	 fgRun=1;
	 }
	else
	{
		fgRun=0;
		State= STOP;
	}
}
void ProcLeftKey()
{
	hTorque_Reference +=100;
}

void ProcRightKey()
{
	
	hTorque_Reference -=100;
}
void ProcUpKey()
{
	hSpeed_Reference += 10;
}
void ProcDownKey()
{
	
	hSpeed_Reference -= 10;
}

/*
*********************************************************************************************************
* �������ƣ�keyScan
* �������ܣ�����ɨ�����
* ��ڲ�����Null
* ���ڲ�����Null
*********************************************************************************************************
*/
	void KeyProc()
	{
		U8 i;
			if (fgKeyDeal == 0)//8ms ���һ��
			{
				return;
			}
			
			fgKeyDeal =0;
			if (ucKeyPressTime != 0)//������ʱ�䶨��
			{
				ucKeyPressTime--;
			}	
			fgKeyDeal = 0;
			switch (ucKeyDat)  //�ж��Ǹ�������
			{
				case 0:
					i = 0;
					break;
				case SetKey:
					i = SetKey;
					break;
				
				case FunKey:
					i = FunKey;
					break;
				case LeftKey:
					i = LeftKey;
					break;
				case RightKey:
					i = RightKey;
					break;
				case UpKey:
					i = UpKey;
					break;
				//case 34:
				//	i = Fun_SetCombKey;
				//	break;
				case DownKey:
					i =  DownKey;
					break;
				
				
				
				
				default:
					i = OtherKey;
					break;
			}
			if (i == ucPreKey)//�ж��Ƿ���ϸ���һ��
			{
				if (ucKeyDebouce != 0)//ȥ����,ʱ��6*8
				{
					ucKeyDebouce--;
					return;
				}
				if (fgBusy)
				{
					if (i == NoKey)
					{
						fgBusy = 0; 			// �˴������ͷ���Ч��
		//				checkFan2();
						ucKeyNew = 0;
						ucKeyOld = 0;
					}
				}
				else
				{
					if ((i != NoKey) && (i != OtherKey))
					{
						fgBusy = 1;
						ucKeyNew = i;
						ucKeyOld = i;
					}
					
				}
			}
			else
			{	
				ucPreKey = i;
				ucKeyDebouce = KeyDebouce2; 	// 72ms
			}
		///////////////////////////////////////////////////
			if (ucKeyNew != 0)
			{
		
				if (ucKeyOld != 0)
				{
					ucKeyOld = 0;
					switch (ucKeyNew)
						{
							case SetKey:		ProcSetKey(); 	  //��ͬ�İ�����ͬ�Ĵ������//���ذ���
								break;		
							
							case FunKey:	
								//������ʱ���ܰ����������ֶ���ʱ��Ҳ����
								ProcFunKey();
								break;
								case LeftKey:		ProcLeftKey();	//�¼��������		  
									break;
								case RightKey:		ProcRightKey(); //�Ҽ��������
									break;
								case UpKey: 		ProcUpKey();	//ȷ�ϼ��������	
									break;
								case DownKey:		ProcDownKey();	//����������	
									break;
							default:
								break;
						}
					
					
					
				}
				else
				{
					
				}
			}
}




