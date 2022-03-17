
#ifndef __USB_MassStorage_H__
#define __USB_MassStorage_H__


//������USB Mass Storage ���һЩ������ݽṹ

#define SUCCESS 0x00
#define FAIL    0x01
#define ERROR   0x02
//UFI��������
#define Format_Unit                 0x04         //��ʽ����Ԫ
#define Inquiry                     0x12         //��ѯ
#define Star_Stop                   0x1B         //����/ֹͣ
#define Mode_Select                 0x55         //ģʽѡ��
#define Mode_Sense                  0x5A         //ģʽ�б�
#define Medium_Removal              0x1E         //��ֹ/��������Ƴ�
#define Read_10                     0x28         //��10
#define Read_12                     0xA8         //��12
#define Read_Capacity               0x25         //������
#define Read_Format_capacity        0x23         //����ʽ������
#define Request_Sense               0x03         //�����ж�
#define Rezero_Unit                 0x01         //��0
#define Seek_10                     0x2B         //Ѱ��10
#define Send_Diagnostic             0x1D         //�������
#define Test_Unit_Ready             0x00         //���Ե�Ԫ׼��
#define Verify                      0x2F         //��֤
#define Write_10                    0x2A         //д10
#define Write_12                    0xAA         //д12
#define Write_and_Verify            0x2E         //д����֤


//�����������(CBW)
typedef struct _CBW
{
 unsigned long dCBWSignature;                    //CBW��־
 unsigned long dCBWTag;                          //������ǩ
 unsigned long int dCBWDataTransgerLength;        //���ݰ���С
 unsigned char bmCBWFlags;                       //D7λ��ʾ���ݷ���0=out��1=in������λ����Ϊ0
 unsigned char bCBWLUN;                          //���ո�������߼���Ԫ��LUN
 unsigned char bCBWCBLength;                     //��ʾCBWCB�ĳ���
 unsigned char CBWCB[16];                        //��������
}
CBW, * pCBW;

//��������״̬���(CSW)
typedef struct _CSW
{
 unsigned long dCSWSignature;                    //CSW�ı�־
 unsigned long dCSWTag;                          //����״̬��־
 unsigned long dCSWDataResidue;                  //��ʾdCBWDataTransferLenhth�ֶ�������ϣ�������ݳ�����ʵ�ʷ��͵����ݳ��ȵĲ��
 unsigned char bCSWStatus;                       //������ִ�е����       
}
CSW, * pCSW;


#endif
