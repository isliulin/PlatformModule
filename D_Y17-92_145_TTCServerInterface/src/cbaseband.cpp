#include "headers.h"
#include "../../Common/Log.h"
#include "DataStructTTCServerInterface.h"


CBaseband::CBaseband()
{
    m_connect = false;
}
void CBaseband::set(SBBE& bbe)
{
    m_ddt.set(bbe.ddt);
    m_ttc.set(bbe.ttc);
    m_ss1.set(bbe.ss1);
    m_ss2.set(bbe.ss2);
    m_tc.set(bbe.tc);
}

void CBaseband::get(SBBE &bbe)
{
    m_ddt.get(bbe.ddt);
    m_ttc.get(bbe.ttc);
    m_ss1.get(bbe.ss1);
    m_ss2.get(bbe.ss2);
    m_tc.get(bbe.tc);
}
bool CBaseband::init(USHORT devID, PlatformResource* pRes)
{
    m_devID = devID;
    m_pPlatformRes = pRes;
    bool isOK = false;
    do
    {
        if(!m_ddt.init(m_devID, m_pPlatformRes))
            break;
        if(!m_ttc.init(devID,m_pPlatformRes))
            break;
        if(!m_ss1.init(m_devID, m_pPlatformRes))
            break;
        //if(!m_ss2.init(m_devID, m_pPlatformRes))
          //  break;
//        if(!m_tc.init(m_devID, m_pPlatformRes))
//            break;
        isOK = true;
    }while(false);
    return isOK;
}

void CBaseband::processBaseBand(QByteArray &datagram)
{
    // 基带
    NETSDFRM netSDFrm;
    int nNetSDFRMLen = sizeof(NETSDFRM);
    if (datagram.size() <= nNetSDFRMLen)
    {
        CLog::addLog("接收到的基带协议报文有错.");
        return;
    }
    memcpy(&netSDFrm, datagram.data(), nNetSDFRMLen);
//    if(netSDFrm.DataLen != datagram.size() - nNetSDFRMLen)
//    {
//        CLog::addLog("接收到的基带协议报文有错.");
//        return;
//    }状态查询命令报文长度不对, 所以无法用上述方法检测
    QMap<int, QPair<QString, QString> >& sidSys = m_pPlatformRes->m_devMap.m_mpSysSid;
    QMap<int, QPair<QString, QString> >::iterator iter = sidSys.find(netSDFrm.SID);
    if(iter == sidSys.end())
        return;
    if(iter.value().first == "DDT")
    {
        m_sysMod = DDT;
        m_ddt.processBaseBand(datagram);
    }
    else if(iter.value().first == "USB")
    {
        m_sysMod = USB;
        m_ttc.processBaseBand(datagram);
    }
    else if(iter.value().first == "SS1")
    {
        m_sysMod = SS1;
        m_ss1.processBaseBand(datagram);
    }
    else if(iter.value().first == "SS2")
    {
        m_sysMod = SS2;
        m_ss2.processBaseBand(datagram);
    }
    else if(iter.value().first == "TC")
    {
        m_sysMod = TC;
        m_tc.processBaseBand(datagram);
    }
    m_connect = (char)1;
}

void CBaseband::getDevInfo(char &bbe, char &bbeControl)
{
    bbe = (char)0;
    bbeControl = (char)0;
    if(m_connect == (char)0) return;
    m_connect = (char)0;
    switch (m_sysMod) {
    case DDT:
        bbe = (char)m_ddt.m_pMCSSysStatus->ucSystemMode;
        bbeControl = (char)m_ddt.m_pMCSSysStatus->ucMonitorMode;
        break;
    case USB:
        bbe = (char)m_ttc.m_pMCSSysDataStatusTTC1->ucSystemMode;
        bbeControl = (char)m_ttc.m_pMCSSysDataStatusTTC1->ucMonitorMode;
        break;
    case SS1:
        bbe = (char)m_ss1.m_pMCSSysTarget4Status1->sysStatu.ucSystemMode;
        bbeControl = (char)m_ss1.m_pMCSSysTarget4Status1->sysStatu.ucMonitorMode;
        break;
    case SS2:
        bbe = (char)m_ss2.m_pMCSSysTarget4Status1->MCSSysStatu.ucSystemMode;
        bbeControl = (char)m_ss2.m_pMCSSysTarget4Status1->MCSSysStatu.ucMonitorMode;
        break;
    case TC:
        bbe = 5;
        bbeControl = (char)0;
        break;
    }
}

