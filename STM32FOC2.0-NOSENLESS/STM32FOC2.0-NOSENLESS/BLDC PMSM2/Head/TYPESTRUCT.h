
/*************************************************************/
/*                                                           */
/*   Variable Type and Function Type		                     */
/*                                                           */
/*   Written by CZQ     2011.8.11    									    */
/*                                                           */
/*************************************************************/

#ifndef TYPESTRUCT
#define TYPESTRUCT

#define   U8    unsigned char      //�޷���8λ
#define   U16   unsigned short       //�޷���16λ
#define   U32   unsigned long  //�޷���32λ
#define   U64   unsigned long long  //�޷���64λ


#define   S8    signed char               //�з���8λ
#define   S16   signed short                //�з���16λ
#define   S32   signed long            //�з���32λ
#define   S64   signed long	long            //�з���64λ


#define FirstBit 0    //0/1:0means the first bit is 0th  bit ,1 means the first bit is 7th bit
#if  FirstBit
typedef struct {			/*�����ֽڡ�λѰַ�ṹ*/
	unsigned char b7:1;
	unsigned char b6:1;
	unsigned char b5:1;
	unsigned char b4:1;
	unsigned char b3:1;
	unsigned char b2:1;
	unsigned char b1:1;
	unsigned char b0:1;
} BYTE_FIELD;

typedef struct{			/*�����֡�λѰַ�ṹ*/
	unsigned short int b15:1;
	unsigned short int b14:1;
	unsigned short int b13:1;
	unsigned short int b12:1;
	unsigned short int b11:1;
	unsigned short int b10:1;
	unsigned short int b9:1;
	unsigned short int b8:1;
	unsigned short int b7:1;
	unsigned short int b6:1;
	unsigned short int b5:1;
	unsigned short int b4:1;
	unsigned short int b3:1;
	unsigned short int b2:1;
	unsigned short int b1:1;
	unsigned short int b0:1;
} WORD_FIELD;

typedef struct{           //*����˫�֡�λѰַ�ṹ  //Add: 2010.01.25 
	unsigned long int  b31:1;
	unsigned long int  b30:1;
	unsigned long int  b29:1;
	unsigned long int  b28:1;
	unsigned long int  b27:1;	
	unsigned long int  b26:1;
	unsigned long int  b25:1;
	unsigned long int  b24:1;
	unsigned long int  b23:1;
	unsigned long int  b22:1;		
	unsigned long int  b21:1;
	unsigned long int  b20:1;
	unsigned long int  b19:1;
	unsigned long int  b18:1;
	unsigned long int  b17:1;
	unsigned long int  b16:1;				
	unsigned long int  b15:1;
	unsigned long int  b14:1;
	unsigned long int  b13:1;
	unsigned long int  b12:1;
	unsigned long int  b11:1;
	unsigned long int  b10:1;
	unsigned long int  b9:1;
	unsigned long int  b8:1;
	unsigned long int  b7:1;
	unsigned long int  b6:1;
	unsigned long int  b5:1;
	unsigned long int  b4:1;
	unsigned long int  b3:1;
	unsigned long int  b2:1;
	unsigned long int  b1:1;
	unsigned long int  b0:1;
} DWORD_FIELD;	

typedef struct {
	unsigned long int PRI:3;
	unsigned long int FUN :5; 
	unsigned long int PADDR :5; 
	unsigned long int SADDR :5; 
	unsigned long int GID :2;
	unsigned long int FNUM :4; 
	unsigned long int FID :4; 
	unsigned long int FT :1;			
}BYTE_STRU;


#else
typedef struct {
	unsigned long int FT :1;
	unsigned long int FID :4; 
	unsigned long int FNUM :4;
	unsigned long int GID :2;
	unsigned long int SADDR :5; 
	unsigned long int PADDR :5; 
	unsigned long int FUN :5; 
	unsigned long int PRI:3;
}BYTE_STRU;

typedef struct {			/*�����ֽڡ�λѰַ�ṹ*/
	unsigned char b0:1;
	unsigned char b1:1;
	unsigned char b2:1;
	unsigned char b3:1;
	unsigned char b4:1;
	unsigned char b5:1;
	unsigned char b6:1;
	unsigned char b7:1;
} BYTE_FIELD;

typedef struct{			/*�����֡�λѰַ�ṹ*/
	unsigned short int b0:1;
	unsigned short int b1:1;
	unsigned short int b2:1;
	unsigned short int b3:1;
	unsigned short int b4:1;
	unsigned short int b5:1;
	unsigned short int b6:1;
	unsigned short int b7:1;
	unsigned short int b8:1;
	unsigned short int b9:1;
	unsigned short int b10:1;
	unsigned short int b11:1;
	unsigned short int b12:1;
	unsigned short int b13:1;
	unsigned short int b14:1;
	unsigned short int b15:1;
} WORD_FIELD;

typedef struct{           //*����˫�֡�λѰַ�ṹ  //Add: 2010.01.25 
	unsigned long int  b0:1;
	unsigned long int  b1:1;
	unsigned long int  b2:1;
	unsigned long int  b3:1;
	unsigned long int  b4:1;	
	unsigned long int  b5:1;
	unsigned long int  b6:1;
	unsigned long int  b7:1;
	unsigned long int  b8:1;
	unsigned long int  b9:1;		
	unsigned long int  b10:1;
	unsigned long int  b11:1;
	unsigned long int  b12:1;
	unsigned long int  b13:1;
	unsigned long int  b14:1;
	unsigned long int  b15:1;				
	unsigned long int  b16:1;
	unsigned long int  b17:1;
	unsigned long int  b18:1;
	unsigned long int  b19:1;
	unsigned long int  b20:1;
	unsigned long int  b21:1;
	unsigned long int  b22:1;
	unsigned long int  b23:1;
	unsigned long int  b24:1;
	unsigned long int  b25:1;
	unsigned long int  b26:1;
	unsigned long int  b27:1;
	unsigned long int  b28:1;
	unsigned long int  b29:1;
	unsigned long int  b30:1;
	unsigned long int  b31:1;
} DWORD_FIELD;	
#endif

typedef union{			/*�����ֽڡ�λѰַ��������*/
	unsigned int Data;
	BYTE_STRU Grp; //can Э��ͷ��ṹ
} CanTYPE_BYTE;



typedef union{			/*�����ֽڡ�λѰַ��������*/
	unsigned char byte;
	BYTE_FIELD bit;
} TYPE_BYTE;

typedef union{		/*�����֡�λѰַ��������*/
	unsigned int word;
	unsigned char byte[2];
	WORD_FIELD bit;
} TYPE_WORD;


typedef union{    /*����˫�֡�λѰַ��������*/	//Add:2010.01.25
	unsigned long int dword;
	unsigned short int word[2];
	unsigned char byte[4];
	DWORD_FIELD bit;
} TYPE_DWORD;

typedef union{    /*����˫�ֽ���������*/
	unsigned short int word[2];
	unsigned char byte[4];
} DWORD;

typedef union{                  //��������������
     unsigned short int word;
	 unsigned char byte [2];
} WORD;

typedef struct
{
	U8 Buf[9];
	U8 Fuhao;
}NumToArrayBufDef;

typedef struct
{
    volatile unsigned char State;
    volatile unsigned char Step;
    volatile unsigned char FlagSwitchStep;      //�����־
    volatile unsigned char FlagBEMF;        //�����־

    volatile unsigned short PWMTicks;

    volatile unsigned short PWMTicksPre;

    unsigned short ObjectDutyCycle;
    unsigned short NonceDutyCycle;

    unsigned short PWM_Freq;
    unsigned short TimerPeriod;

    unsigned short NonceBEMF;
    unsigned short MaxBEMF;
    unsigned short MinBEMF;

    unsigned short CommutationTimeMax;    //�����ʱ��
    unsigned short CommutationTimeMin;       //��С����ʱ��
    unsigned short CommutationTimePre;       //�ϴλ���ʱ��


    unsigned short Count1;
    unsigned short Count2;
    unsigned char ADC_BEMF_Filter;

} MotorParam_TypeDef;

#endif

