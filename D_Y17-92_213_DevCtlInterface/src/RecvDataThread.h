#ifndef RECVDATATHREAD_H
#define RECVDATATHREAD_H

#include <QDateTime>
#include <QThread>
#include <QUdpSocket>
#include "../../Common/LxTsspCommonDefinition.h"
#include "../../Common/TypeDefine.h"
#include "../../Common/DataStruct2311.h"
#include "DataStructTTCDevCtlInterface.h"
#include "platformresource.h"
#include "cbaseband.h"
#include "devices.h"
#include "alter.h"
#include "adjustalter.h"
#include "CRM.h"
#include "../../Common/SMCUSSB2_BBE_CDMAStruct.h"
#include "../../Common/CommonFunction.h"
#define MAX_RECV_BUFFER  (1024 * 8)     //最大数据长度 存在数据协议中有个包大概在7K左右

//通道处理以及函数结构体
typedef struct _tag_channelProcessor
{
    QString chlId;
    ICommunicationChannel *pSendChl;
    void (*fpProcessFunction)(QByteArray&);
}CHLPRSOR;

class RecvDataThread : public QThread
{
    Q_OBJECT
public:
    explicit RecvDataThread(QObject *parent = 0);
    ~RecvDataThread();

    void run();
    void setStop();
    bool init(PlatformResource& res);
    PlatformResource* m_pPlatformRes;
public slots:
    void ReadData();

public:
    CBaseband m_bbe1;
    CBaseband m_bbe2;
    CBaseband m_bbe3;
    CNetSwitch m_netSwitchUp;
    CNetSwitch m_netSwitchDown;
    CAlter m_alterDown1;
    CAlter m_alterDown2;
    CAlter m_alterUp1;
    CAlter m_alterUp2;
    CAdjustAlter m_adjustAlter;
    CTC m_safeCtrl1;
    CTC m_safeCtrl2;
    CRM m_crm1;
    CRM m_crm2;
    CDevices m_devs;
private:
    //区分具体的协议 判断是协议1还是协议2
    int checkProcotol(QByteArray& datagram);
    QVector<ICommunicationChannel*> m_ComChannelVec;
    QUdpSocket * m_pSocket;     //测试使用的UDP通道
    BOOL m_bStop;   //停止线程变量使用
    UCHAR *m_pDataBuffer;   //用于接收数据使用的临时缓存
    SMCU2::MCSSSTCStatus4* m_pTC;
public:
    //初始化相关的通道
    bool initChlProcessor();
    //

    void processDatagram(QByteArray& datagram, QString chlID);
    //处理第1中协议数据
    void proceProcotol(QByteArray& datagram);
    int getSN(int devID);
};

#endif // RECVDATATHREAD_H
