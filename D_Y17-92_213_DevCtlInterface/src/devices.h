#ifndef CDEVICES_H
#define CDEVICES_H
//标准TTC联试应答机监控  非相干扩频联试应答机监控
//校零信表一体机 射频分路组合监控 射频切换开关监控 测试开关矩阵
//频率基准源监控 铷钟机箱监控
//以上设备简单, 使用同一协议, 对于一个设备只需要一个数据源,一个处理函数,故放在CDevices中集中处理。

//中频开关矩阵 因为有上行与下行, 需要两个, 所以单独定义
#include <QDateTime>
#include <QThread>
#include <QUdpSocket>
#include "../../Common/LxTsspCommonDefinition.h"
#include "../../Common/TypeDefine.h"
#include "../../Common/DataStruct2311.h"
#include "DataStructTTCDevCtlInterface.h"
#include "platformresource.h"
#include "cbaseband.h"
#include "../../Common/Dcu_common_DataStruct.h"
#define MAX_RECV_BUFFER  (1024 * 8)     //最大数据长度 存在数据协议中有个包大概在7K左右

//通道处理以及函数结构体
class CNetSwitch
{
public:
    CNetSwitch(){m_connectOld = m_connect = char(0);m_timeTagID = 0;}
    bool init(USHORT devID, PlatformResource* res, bool bUp = true);
    PlatformResource* m_pPlatformRes;
    quint64 m_timeTagID;
    HK_IFSWMatStatu* m_pIFSWMatStatuResponse;
    //中频开关矩阵模块处理函数
    void processNetSwitch(QByteArray& datagram, const QString &strName);
    //处理响应结果
    void processResponse(QString strDeviceName, UCHAR ucAnswerFlag);
    QString getAnswerFlag(UCHAR ucAnsFlag);

    void getDevInfo(char& connect);
    int m_connect;//不断增加
    int m_connectOld;//保存m_connect;
};
class CDevices
{
public:
    CDevices();
    const char m_const_0;
    const char m_const_1;
    void zeroConn();
    void updateConn();
    bool init(PlatformResource* res);
    PlatformResource* m_pPlatformRes;
    COMBTRANSStatu* m_pCombTransStatuTTC;//标准TTC联试应答机监控
    int m_connStdRSP;
    int m_connStdRSPOld;
    COMBTRANSStatu* m_pCombTransStatuFXG;//非相干扩频联试应答机监控
    int m_connSSRSP;
    int m_connSSRSPOld;
    ZEROCALIBBEACONStatu* m_pZCBeaconStatu;//校零信标一体机监控
    int m_connZEROSTD;
    int m_connZEROSTDOld;
    SLDCDCUStatu* m_pSLDDcuParaStatu;//射频分路组合监控
    int m_connSLDC;
    int m_connSLDCOld;
    SSWITCHDCUStatu* m_pSswitchDcuStatu;//射频切换开关监控
    int m_connSswitch;
    int m_connSswitchOld;
    TEST_SWMATStatu* m_pTstSwMatStatu;//测试开关矩阵监控
    long m_connTestNetSwitch;
    long m_connTestNetSwitchOld;
    quint64 m_tstSwMatID;
    FREQBDCUStatu* m_pFreDcuStatu;//频率基准源监控
    int m_connFREQB;
    int m_connFREQBOld;
    RCLOCKDCUStatu* m_pRCLOCKDCUStatu;//铷钟机箱监控
    int m_connRCLOCK;
    int m_connRCLOCKOld;

    COMBTRANSPara* m_pCombTransParaTTC;
    COMBTRANSPara* m_pCombTransParaFXG;
    void getDevInfo(DevicesConnectStatus* pConnectSt);
public:
    //处理第1中协议数据
    void proceProcotol(QByteArray& datagram);
private:
    //{{如果是状态返回true
    //联试应答机监控模块处理函数
    bool processCombTrans(PROCOTOL_PACK& proPack, bool bTTC = true);
    //校零信标一体机模块处理函数
    bool processZEROSTDDcu(PROCOTOL_PACK& proPack);
    //射频分路组合接口模块处理函数
    bool processSLDDcu(PROCOTOL_PACK& proPack);
    //射频切换开关监控模块处理函数
    bool processSswitch(PROCOTOL_PACK& proPack);
    //测试开关矩阵模块处理函数
    bool processTestSwitch(PROCOTOL_PACK& proPack);
    //频率基准源监控模块处理函数
    bool processFreqBDcu(PROCOTOL_PACK& proPack);
    //铷钟机箱监控模块处理函数
    bool processRCLOCKDcu(PROCOTOL_PACK& proPack);
    //}}
    //解析铷钟机箱监控数据 此协议需要特殊处理
    void analyseRCLOCKDcu(UCHAR *pData, unsigned int nLen);
    //处理响应结果
    void processResponse(QString strDeviceName, UCHAR ucAnswerFlag);

    //获取响应返回的信息内容
    QString getAnswerFlag(UCHAR ucAnsFlag);
};

#endif // RECVDATATHREAD_H
