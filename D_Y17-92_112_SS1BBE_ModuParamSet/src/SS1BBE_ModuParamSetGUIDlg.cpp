﻿#include "SS1BBE_ModuParamSetGUIDlg.h"
#include "ui_SS1BBE_ModuParamSetGUIDlg.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QDateTime>
#include "../../Common/Log.h"
#include "../../Common/CommonFunction.h"
#include <QTimer>
#pragma pack(1)
extern quint64 GetID(ushort usStationID, uchar ucSubSysID, ushort usDeviceID, ushort usType,
                     uchar ucSn);
SS1BBE_ModuParamSetGUIDlg::SS1BBE_ModuParamSetGUIDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SS1BBE_ModuParamSetGUIDlg)
{
    ui->setupUi(this);
    m_pStateMgr = NULL;
    m_pILxTsspMessageManager = NULL;
    m_pILxTsspLogManager = NULL;
    m_pPublicInfo = NULL;
    m_pHAManager = NULL;
    m_targetNum_usedInMacro = -1;
    m_targetNum_usedInParam = -1;
    m_nTargetNum = 0;
    m_pDoppler = NULL;
    m_doppler = false;
    m_MCSSSUTargetTCSSUpModulStatus = NULL;
    m_timer1 = new QTimer(this);
    connect(m_timer1, SIGNAL(timeout()), this, SLOT(updateRadioButton()));

    setAttribute(Qt::WA_DeleteOnClose);
}

SS1BBE_ModuParamSetGUIDlg::~SS1BBE_ModuParamSetGUIDlg()
{
    killTimer(m_TimerID);
    m_timer1->stop();
    delete m_timer1;
    delete ui;
}
int SS1BBE_ModuParamSetGUIDlg::msgBoxFunction(QMessageBox &msgBox, QString msg, BOOL ret)
{
    msgBox.setText(msg);
    msgBox.exec();
    return ret;
}
void SS1BBE_ModuParamSetGUIDlg::setText(const QString& strTitle, ParaStruct xmlsetText[5][60], QString MapMean[50][30], int MapNum[50], uchar Type, ILxTsspParameterMacro* pMacroObj)
{
    m_strTitle = strTitle;
    setWindowTitle(strTitle);

    memcpy(m_DlgParaSetStruct, xmlsetText, sizeof(ParaStruct) * 60 * 5);

    for(int i=0;i<50;i++)
    {
        for(int j=0; j<30; j++)
        {
            m_DlgMapMean[i][j] = MapMean[i][j];
        }
    }

    memset(DlgMapLineNum, 0, 50);
    for(int i=0; i<49; i++)
    {
        DlgMapLineNum[i] = MapNum[i];
    }
    //初始化界面
    InitDLG(Type, pMacroObj);
}

//初始化对话框
void SS1BBE_ModuParamSetGUIDlg::InitDLG(int Type, ILxTsspParameterMacro* pMacroObj)
{
    iParaSetFlag = Type;
    //设置输入格式
    InitInput();
    //显示变量名字和范围
    ShowControls();

    if(Type == 0)//取当前参数
    {
        //取当前设置的参数
        if(m_pStateMgr != NULL)
        {
            DWORD stLen = sizeof(SMCU::MCSSSUTargetTCSSUpModulStatusStruct);
            m_MCSSSUTargetTCSSUpModulStatus  =(SMCU::MCSSSUTargetTCSSUpModulStatusStruct*)m_pStateMgr->FindOneItem(m_StatusParaMCSSSUTargetTCSSUpModulID, stLen);
            if(m_MCSSSUTargetTCSSUpModulStatus == NULL || stLen != sizeof(SMCU::MCSSSUTargetTCSSUpModulStatusStruct))
                return;
        }
        //启动定时器
        m_TimerID = startTimer(500);
        //不使能
        EnabledControls(false);
        EnabledControlsCommon(false);
    }
    else if(Type == 1)//取宏里参数
    {
        ui->pushButton_Modify->setText("");
        ui->pushButton_ModifyCommon->setText("");
        ui->pushButton_Set->setEnabled(false);
        if(pMacroObj!=NULL)
        {
            QByteArray byteArray;
            pMacroObj->GetParameterBlock(m_ParaMCSSSUTargetTCSSUpModulID,byteArray);
            memcpy(&m_MCSSSUTargetTCSSUpModulParam,byteArray.data(),sizeof(m_MCSSSUTargetTCSSUpModulParam));
        }
        ui->pushButton_Modify->setVisible(false);
        ui->pushButton_Set->setVisible(false);
        ui->pushButton_ModifyCommon->hide();
        ui->pushButton_SetCommon->hide();
        // gzl 2017/07/24 恢复多普勒补偿在宏参数设置中的显示
        // sjc 2017/07/24 删除多普勒补偿在宏参数设置中的显示（按甲方要求）
        ui->groupBox_processCtrl->hide();
        ui->groupBox_4->setGeometry(50, 20, 241, 90);
        ui->groupBox_5->setGeometry(330, 20, 221, 90);
        ui->groupBox_6->setGeometry(590, 20, 241, 90);
        ui->groupBox_common->setGeometry(70, 80, 891, 130);
        ui->groupBox_target->setGeometry(70, 221, 891, 351);
        // sjc 2017/07/24
        //end gzl 2017/07/24
        m_timer1->start(500);

        ui->label_BackZeroRate->setParent(ui->groupBox_4);
        ui->lineEdit_BackZeroRate->setParent(ui->groupBox_4);
        ui->label_14->setParent(ui->groupBox_4);
        ui->label_OutputElLevel->setGeometry(20,13,100,30);
        ui->lineEdit_OutputElLevel->setGeometry(110,13,85,30);
        ui->label_2->setGeometry(200,13,30,30);
        ui->label_BackZeroRate->setGeometry(20,50,65,30);
        ui->lineEdit_BackZeroRate->setGeometry(110,50,85,30);
        ui->label_14->setGeometry(200,50,30,30);
        //显示参数
        m_pCommFunc = CCommFunc::getInstance();
        ShowPara(1);
    }
}

//设置输入格式
void SS1BBE_ModuParamSetGUIDlg::InitInput()
{
    //正整数
    QRegExp regExp("[0-9]{1,8}");
    //载波输出电平
    QRegExp regExp_OutputElLevel("-[0-9]{1,2}");
    ui->lineEdit_OutputElLevel->setValidator(new QRegExpValidator(regExp_OutputElLevel, this));
    //输出载噪比
    QRegExp regExp_WaveNoiseRatio("[0-9]{0,3}");
    ui->lineEdit_WaveNoiseRatio->setValidator(new QRegExpValidator(regExp_WaveNoiseRatio, this));
    //回零速率
    QRegExp regExp_BackZeroRate("[0-9]{0,5}");
    ui->lineEdit_BackZeroRate->setValidator(new QRegExpValidator(regExp_BackZeroRate, this));
    //补偿周期
    ui->lineEdit_CompensateTime->setValidator(new QRegExpValidator(regExp, this));
    //多普勒预置频率
    QRegExp regExp_DopplerPredFre("(-)?[0-9]{0,3}");
    ui->lineEdit_DopplerPredFre->setValidator(new QRegExpValidator(regExp_DopplerPredFre, this));
    //输出功率衰减
    QRegExp regExp_OutputPowerDecrease("-[0-9]{0,2}|0");
    ui->lineEdit_OutputPowerDecrease->setValidator(new QRegExpValidator(regExp_OutputPowerDecrease, this));
    //功率比
    QRegExp regExp_IORate("(-)?[0-9]{0,2}");
    ui->lineEdit_IORate->setValidator(new QRegExpValidator(regExp_IORate, this));

    //字母数字
    QRegExp regExp2("[A-Fa-f0-9]{1,8}");
    /*//帧同步码组[HEX]
    ui->lineEdit_ZTMZ->setValidator(new QRegExpValidator(regExp2, this));
    //副帧同步码组[HEX]
    ui->lineEdit_FZTBMZ->setValidator(new QRegExpValidator(regExp2, this));*/
}

void SS1BBE_ModuParamSetGUIDlg::ShowControls()
{
    int i;
    //调制方式
    for(i=0; i<DlgMapLineNum[0];i++)
    {
        ui->comboBox_AdjustType->addItem(m_DlgMapMean[0][i]);
    }
    ui->comboBox_AdjustType->setCurrentIndex(m_DlgParaSetStruct[0][1].InitVal.lval);
    //载波控制
    for(i=0; i<DlgMapLineNum[1];i++)
    {
        ui->comboBox_WaveCtrl->addItem(m_DlgMapMean[1][i]);
    }
    ui->comboBox_WaveCtrl->setCurrentIndex(m_DlgParaSetStruct[0][2].InitVal.lval);
    //噪声源输出
    for(i=0; i<DlgMapLineNum[2];i++)
    {
        ui->comboBox_NoiseSourOutput->addItem(m_DlgMapMean[2][i]);
    }
    ui->comboBox_NoiseSourOutput->setCurrentIndex(m_DlgParaSetStruct[0][3].InitVal.lval);
    //载波输出电平
    ui->lineEdit_OutputElLevel->setText(QString::number(m_DlgParaSetStruct[0][0].InitVal.lval));
    ui->label_OutputElLevel->setText("载波输出电平\n[" + QString::number(m_DlgParaSetStruct[0][0].MinVal.lmin) +
            "~" + QString::number(m_DlgParaSetStruct[0][0].MaxVal.lmax) + "]");
    //输出载噪比
    ui->lineEdit_WaveNoiseRatio->setText(QString::number(m_DlgParaSetStruct[0][4].InitVal.lval));
    ui->label_WaveNoiseRatio->setText("输出载噪比\n[" + QString::number(m_DlgParaSetStruct[0][4].MinVal.lmin) +
            "~" + QString::number(m_DlgParaSetStruct[0][4].MaxVal.lmax) + "]dBHz");
    //补偿方式
    for(i=0; i<DlgMapLineNum[3];i++)
    {
        ui->comboBox_CompensateType->addItem(m_DlgMapMean[3][i]);
    }
    if (m_pDoppler->flag == true)
    {
        ui->checkBox_dopplerSw->setChecked(m_pDoppler->compensateCtrl == 1 ? true : false);
        ui->lineEdit_BackZeroRate->setText(QString::number(m_pDoppler->resetSpeed));
        ui->comboBox_CompensateType->setCurrentIndex(m_DlgParaSetStruct[0][6].InitVal.lval);
        ui->lineEdit_CompensateTime->setText(QString::number(1000 / m_pDoppler->compensatePeriod));
    }
    else
    {
        //多普勒补偿控制
        ui->checkBox_dopplerSw->setChecked(false);
        //回零速率
        ui->lineEdit_BackZeroRate->setText(QString::number(m_DlgParaSetStruct[0][5].InitVal.lval));
        //补偿方式
        ui->comboBox_CompensateType->setCurrentIndex(m_DlgParaSetStruct[0][6].InitVal.lval);
        //补偿周期
        ui->lineEdit_CompensateTime->setText(QString::number(m_DlgParaSetStruct[0][7].InitVal.lval));
    }

    showTargetParam(0);
    //默认选择目标1
    ui->radioButton_Target1->setChecked(true);
}

//显示参数
void SS1BBE_ModuParamSetGUIDlg::ShowPara(int type)
{
    if(type == 0)       //参数设置
    {
        if(m_MCSSSUTargetTCSSUpModulStatus != NULL)
        {
            m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam = m_MCSSSUTargetTCSSUpModulStatus->SSUpModulStatu.tParams;
            for(int i = 0; i < MAX_TARGET_COUNT; i++)
            {
                m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[i] = m_MCSSSUTargetTCSSUpModulStatus->targetTCSSUpModulStatu[i].tParams;
            }
        }
        else
        {
            DWORD stLen = sizeof(SMCU::MCSSSUTargetTCSSUpModulStatusStruct);
            if(m_pStateMgr != NULL)
                m_MCSSSUTargetTCSSUpModulStatus  = (SMCU::MCSSSUTargetTCSSUpModulStatusStruct*)m_pStateMgr->FindOneItem(m_StatusParaMCSSSUTargetTCSSUpModulID, stLen);
        }
    }
    else
    {
        USHORT retZVel = m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[0].usRetZVel;
        ui->lineEdit_BackZeroRate->setText(QString::number(retZVel <= 15000 ? retZVel : 0));
        //解扰使能或禁止
        //副帧方式控件使能
        if (ui->pushButton_Modify->text() != "更改")
        {
            //EnabledControlsSubFrmType(ui->comboBox_SubFrmType->currentIndex());
        }
    }

    /********************************公共参数*********************************/

    //调制方式
    if (m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucModulate > 0)
        ui->comboBox_AdjustType->setCurrentIndex(m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucModulate - 1);
    //载波输出电平
    ui->lineEdit_OutputElLevel->setText(QString::number(m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.cWaveOutLevel));
    //载波控制
    if (m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucWaveOutCtrl > 0)
        ui->comboBox_WaveCtrl->setCurrentIndex(m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucWaveOutCtrl - 1);
    //噪声源输出
    if (m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucNoiseSw > 0)
        ui->comboBox_NoiseSourOutput->setCurrentIndex(m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucNoiseSw - 1);
    //载波输出载噪比
    ui->lineEdit_WaveNoiseRatio->setText(QString::number(m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucCN0));
    on_comboBox_NoiseSourOutput_currentIndexChanged(ui->comboBox_NoiseSourOutput->currentIndex());
    on_checkBox_dopplerSw_clicked();
    processCtrl();
    //显示目标
    showRadioButton();
    //    if (m_MCSSSUTargetTCSSUpModulStatus == NULL)
    //        return;
    showTargetParam(m_nTargetNum, 1);

    /********************************目标参数*********************************/

}

//设置帧长范围
void SS1BBE_ModuParamSetGUIDlg::SetFrameLenRange(int iWordLen)
{
    /* if(iWordLen ==8)
    {
        m_DlgParaSetStruct[1][3].MinVal.lmin = 4;
        m_DlgParaSetStruct[1][3].MaxVal.lmax = 4096;
    }
    else
    {
        m_DlgParaSetStruct[1][3].MinVal.lmin = 4;
        m_DlgParaSetStruct[1][3].MaxVal.lmax = 2048;
    }
    QString strTemp = "帧长["+QString::number(m_DlgParaSetStruct[1][3].MinVal.lmin)+"~"+
            QString::number(m_DlgParaSetStruct[1][3].MaxVal.lmax)+"]字";
    ui->label_FrameLen->setText(strTemp);*/
}

void SS1BBE_ModuParamSetGUIDlg::SetObjInfo(TLxTsspObjectInfo ObjectInfo, DOPPLERCOMPENSATE& m_doppler)
{
    m_pDoppler = &m_doppler;
    memcpy(&m_ObjectInfo, &ObjectInfo, sizeof(TLxTsspObjectInfo));
    //对象初始化
    //查找对象管理器
    LxTsspObjectManager* pObjectManager = LxTsspObjectManager::Instance();

    if(pObjectManager == NULL){
        return;
    }

    m_pHAManager = pObjectManager->GetHAManager();

    bool bRet = m_DeviceMap.loadXML();
    if (!bRet)
    {
        QString strInfo = QString("load device map failed %1");
        CLog::addLog(strInfo);
        return;
    }

    ITEM *pItem = m_DeviceMap.getItem(MODULENAME);
    if (pItem->vSubItem.size() > 0)
    {
        SUBITEM& sItem = *(pItem->getSubItemByDeviceID_SN(m_ObjectInfo.usLocalDID, m_ObjectInfo.ucSN));
        m_usC = sItem.usCMD;
        m_usTID = sItem.usTID;
        m_ucSN = sItem.ucSN;
        /*************************框架**********************************/
        //码同步
        m_ParaMCSSSUTargetTCSSUpModulID = GetID(sItem.usStationID,
                                                sItem.ucSubSysID, sItem.usDeviceID,sItem.usTID,
                                                sItem.ucSN);
        /*************************框架**********************************/

        /*************************状态**********************************/
        //码同步
        m_StatusParaMCSSSUTargetTCSSUpModulID = GetID(sItem.usStationID,
                                                      sItem.ucSubSysID, sItem.usDeviceID,sItem.usStateType,
                                                      sItem.ucSN);
        /*************************状态**********************************/

        //获取目标参数的ID
        ITEM *pItemCom = m_DeviceMap.getItem(COMM_SS1BBE_NAME);
        if (pItemCom != NULL && pItemCom->vSubItem.size() > 0)
        {
            SUBITEM& sItem = *(pItemCom->getSubItemByDeviceID_SN(m_ObjectInfo.usLocalDID, m_ObjectInfo.ucSN));
            /*************************状态**********************************/
            m_CommTargetID = GetID(sItem.usStationID,
                                   sItem.ucSubSysID, sItem.usDeviceID,sItem.usStateType,
                                   sItem.ucSN);
            /*************************状态**********************************/
        }

        //获取发送控制命令的cmd和type
        ITEM *pItemModu = m_DeviceMap.getItem(CMDNAME);
        if (pItemModu != NULL)
        {
            //过程控制命令的实现还需要规划
            int index = 4;
            m_sItem = pItemModu->vSubItem.at(index);
            if(m_sItem.usDeviceID != m_ObjectInfo.usLocalDID)
                m_sItem = pItemModu->vSubItem.at(index+1);
            m_ctlCmd = GetID(m_sItem.usStationID, m_sItem.ucSubSysID, m_sItem.usDeviceID, m_sItem.usStateType,
                             m_sItem.ucSN);
        }

        //日志
        m_pILxTsspLogManager = pObjectManager->GetLogManager();
        m_pStateMgr = pObjectManager->GetStateManager();
        m_pILxTsspMessageManager = pObjectManager->GetMessageManager();
        m_pPublicInfo = pObjectManager->GetPublicInfoManager();
    }

}

//保存参数宏
int SS1BBE_ModuParamSetGUIDlg::SaveToMacro(ILxTsspParameterMacro* pMacroObj)
{
    //判断合法性
    int iRet = CheckPara();
    if(iRet == -1)
    {
        return -1;
    }
    else
    {
        GetPara();
        //宏特殊处理会零速率
        m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[0].usRetZVel = ui->lineEdit_BackZeroRate->text().toInt();
        //取当前设置的参数
        QByteArray byteArray;

        //公共参数
        byteArray.resize(sizeof(m_MCSSSUTargetTCSSUpModulParam));
        memcpy(byteArray.data(), &m_MCSSSUTargetTCSSUpModulParam, sizeof(m_MCSSSUTargetTCSSUpModulParam));
        pMacroObj->UpdateParameterBlock(m_ParaMCSSSUTargetTCSSUpModulID, byteArray);

        //日志信息
        TLxTsspLog tmpLog;
        QString strLog;
        memset(&tmpLog, 0, sizeof(TLxTsspLog));
        tmpLog.ucWarningLevel  = 1;
        strLog += "模式1参数设置: 保存宏成功！";
        strcpy(tmpLog.szContent, strLog.toUtf8().data());
        tmpLog.time = QDateTime::currentDateTime().toTime_t();

        m_pILxTsspLogManager->Add(tmpLog);
    }
}
//从界面取参数
void SS1BBE_ModuParamSetGUIDlg::GetPara()
{

    /***********************************公共参数****************************/
    //调制方式
    m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucModulate = ui->comboBox_AdjustType->currentIndex() + 1;
    //载波输出电平
    m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.cWaveOutLevel = ui->lineEdit_OutputElLevel->text().toInt();
    //载波控制
    m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucWaveOutCtrl = ui->comboBox_WaveCtrl->currentIndex() + 1;
    //噪声源输出
    m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucNoiseSw = ui->comboBox_NoiseSourOutput->currentIndex() + 1;
    //载波输出载噪比
    m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam.ucCN0 = ui->lineEdit_WaveNoiseRatio->text().toInt();

    /***********************************公共参数****************************/
    getParamTarget();
}

//验证参数范围
int SS1BBE_ModuParamSetGUIDlg::CheckPara()
{
    QMessageBox msgBox;
    QString strTmp;
    msgBox.setWindowTitle(MODULENAME);
    /***********************************码同步****************************/

    if(ui->lineEdit_OutputElLevel->text().isEmpty())
    {
        strTmp = "载波输出电平输入为空，请重新输入!";
        goto flag;
    }
    if(ui->lineEdit_DopplerPredFre->text().isEmpty())
    {
        strTmp = "多普勒预置频率输入为空，请重新输入!";
        goto flag;
    }
    if(ui->lineEdit_OutputPowerDecrease->text().isEmpty())
    {
        strTmp = "输出功率衰减输入为空，请重新输入!";
        goto flag;
    }
    if(ui->lineEdit_IORate->text().isEmpty())
    {
        strTmp = "IQ功率比输入为空，请重新输入!";
        goto flag;
    }

    //载波输出电平
    if((ui->lineEdit_OutputElLevel->text().toInt()) < m_DlgParaSetStruct[0][0].MinVal.lmin
            || (ui->lineEdit_OutputElLevel->text().toInt()) > m_DlgParaSetStruct[0][0].MaxVal.lmax)
    {
        strTmp = QString("载波输出电平正确范围:[%1～%2]dBm，请重新输入!")
                .arg(m_DlgParaSetStruct[0][0].MinVal.lmin).arg(m_DlgParaSetStruct[0][0].MaxVal.lmax);
        goto flag;
    }
    //载波输出载噪比
    if((ui->lineEdit_WaveNoiseRatio->text().toInt()) < m_DlgParaSetStruct[0][4].MinVal.lmin
            || (ui->lineEdit_WaveNoiseRatio->text().toInt()) > m_DlgParaSetStruct[0][4].MaxVal.lmax)
    {
        strTmp = QString("载波输出载噪比正确范围：[%1～%2]dBHz，请重新输入!")
                .arg(m_DlgParaSetStruct[0][4].MinVal.lmin).arg(m_DlgParaSetStruct[0][4].MaxVal.lmax);
        goto flag;
    }
    //回零速率
    if(ui->lineEdit_BackZeroRate->text().isEmpty())
    {
        strTmp = QString("回零速率不能为空，请重新输入!");
        goto flag;
    }
    if(ui->lineEdit_BackZeroRate->text().toInt() < m_DlgParaSetStruct[0][5].MinVal.lmin
            || ui->lineEdit_BackZeroRate->text().toInt() > m_DlgParaSetStruct[0][5].MaxVal.lmax)
    {
        strTmp = QString("回零速率范围[%1～%2]Hz/s，请重新输入!")
                .arg(m_DlgParaSetStruct[0][5].MinVal.lmin).arg(m_DlgParaSetStruct[0][5].MaxVal.lmax);
        goto flag;
    }
    //补偿周期
    if(ui->lineEdit_CompensateTime->text().isEmpty())
    {
        strTmp = QString("补偿周期不能为空，请重新输入!");
        goto flag;
    }
    if((UINT)ui->lineEdit_CompensateTime->text().toFloat() < m_DlgParaSetStruct[0][7].MinVal.lmin
            || (UINT)ui->lineEdit_CompensateTime->text().toFloat() > m_DlgParaSetStruct[0][7].MaxVal.lmax)
    {
        strTmp = QString("补偿周期正确范围[%1～%2]ms，请重新输入!")
                .arg(m_DlgParaSetStruct[0][7].MinVal.lmin).arg(m_DlgParaSetStruct[0][7].MaxVal.lmax);
        goto flag;
    }
    //多普勒预置频率
    if((ui->lineEdit_DopplerPredFre->text().toInt()) < m_DlgParaSetStruct[1][0].MinVal.lmin
            || (ui->lineEdit_DopplerPredFre->text().toInt()) > m_DlgParaSetStruct[1][0].MaxVal.lmax)
    {
        strTmp = QString("多普勒预置频率正确范围：[%1～%2]kHz，请重新输入!")
                .arg(m_DlgParaSetStruct[1][0].MinVal.lmin).arg(m_DlgParaSetStruct[1][0].MaxVal.lmax);
        goto flag;
    }
    //输出功率衰减
    if(ui->lineEdit_OutputPowerDecrease->text().toInt() < m_DlgParaSetStruct[1][1].MinVal.lmin
            || ui->lineEdit_OutputPowerDecrease->text().toInt() > m_DlgParaSetStruct[1][1].MaxVal.lmax)
    {
        strTmp = QString("输出功率衰减正确范围：[%1～%2]dB，请重新输入!")
                .arg(m_DlgParaSetStruct[1][1].MinVal.lmin).arg(m_DlgParaSetStruct[1][1].MaxVal.lmax);
        goto flag;
    }
    //IO功率比
    if((ui->lineEdit_IORate->text().toInt()) < m_DlgParaSetStruct[1][4].MinVal.lmin
            || (ui->lineEdit_IORate->text().toInt()) > m_DlgParaSetStruct[1][4].MaxVal.lmax)
    {
        strTmp = QString("IO功率比正确范围：[%1～%2]dB，请重新输入!")
                .arg(m_DlgParaSetStruct[1][4].MinVal.lmin).arg(m_DlgParaSetStruct[1][4].MaxVal.lmax);
        goto flag;
    }
    return 1;
flag:
    msgBox.setText(strTmp);
    msgBox.exec();
    return -1;
}

//定时器
void SS1BBE_ModuParamSetGUIDlg::timerEvent(QTimerEvent *event)
{
    ShowPara(0);
}



void SS1BBE_ModuParamSetGUIDlg::closeEvent(QCloseEvent *event)
{
    event->accept();
}

QSize SS1BBE_ModuParamSetGUIDlg::sizeHint() const
{
    return QSize(this->width(), this->height());
}

void SS1BBE_ModuParamSetGUIDlg::SubFrmEn(bool bFlag)
{
    //副帧长度
    //    ui->label_SubFrmLen->setEnabled(bFlag);
    //    ui->lineEdit_SubFrmLen->setEnabled(bFlag);

    //    ui->label_CFSearchErrBits->setEnabled(bFlag);
    //    ui->comboBox_CFSearchErrBits->setEnabled(bFlag);
    //    ui->label_CFVerifyErrBits->setEnabled(bFlag);
    //    ui->comboBox_CFVerifyErrBits->setEnabled(bFlag);
    //    ui->label_CFLockErrBits->setEnabled(bFlag);
    //    ui->comboBox_CFLockErrBits->setEnabled(bFlag);
    //    ui->label_CFVerifyCheckFrame->setEnabled(bFlag);
    //    ui->comboBox_SubFVerifyFrame->setEnabled(bFlag);
    //    ui->label_CFLockCheckFrame->setEnabled(bFlag);
    //    ui->comboBox_SubFLockFrame->setEnabled(bFlag);
}

//有无ID副帧
void SS1BBE_ModuParamSetGUIDlg::IDSubFrmEn(bool bFlag)
{
    //    ui->groupBox_IDSubFrm->setEnabled(bFlag);
    //    //ID副帧位置
    //    ui->label_IDSubFrmPos->setEnabled(bFlag);
    //    ui->lineEdit_IDSubFrmPos->setEnabled(bFlag);

    //    //ID基值
    //    ui->label_IdBase->setEnabled(bFlag);
    //    ui->comboBox_IdBase->setEnabled(bFlag);

    //    //ID字方向
    //    ui->label_IdDirect->setEnabled(bFlag);
    //    ui->comboBox_IdDirect->setEnabled(bFlag);
}

//有无循环副帧
void SS1BBE_ModuParamSetGUIDlg::CycSubFrmEn(bool bFlag)
{
    //循环副帧
    //    ui->groupBox_CycSubFrm_->setEnabled(bFlag);
    //    ui->label_CycFrmSyncCode->setEnabled(bFlag);
    //    ui->lineEdit_CycFrmSyncCode->setEnabled(bFlag);
    //    ui->label_CycFrmSynCodePos->setEnabled(bFlag);
    //    ui->lineEdit_CycFrmSynCodePos->setEnabled(bFlag);
    //    ui->label_CycFrmSynCodeLen->setEnabled(bFlag);
    //    ui->lineEdit_CycFrmSynCodeLen->setEnabled(bFlag);
}

//有无反码副帧
void SS1BBE_ModuParamSetGUIDlg::ReserveSubFrmEn(bool bFlag)
{

}

void SS1BBE_ModuParamSetGUIDlg::SendMessage(char* pData, unsigned short  usLen,
                                            unsigned short usC, unsigned short usTID, unsigned char ucSN)
{
    static uint seq = 0;
    TLxTsspMessageHeader header = {0};
    TLxTsspSubMsgHeader subHeader = {0};
    header.uiSequence = seq++;
    header.uiHandle = 1;
    header.S = m_pPublicInfo->GetLocalDeviceID();
    header.L = sizeof(TLxTsspMessageHeader) +sizeof(TLxTsspSubMsgHeader) + usLen;
    header.D = m_pPublicInfo->GetLocalDeviceID();//0x09;
    header.T = 1;
    header.M = 1;
    header.P = 1;
    header.A = 0;
    header.R = 0;
    header.C = 0x200;

    subHeader.Len = usLen;
    subHeader.SID = m_pPublicInfo->GetLocalDeviceID();  //m_DObjectInfo.usStationID;
    subHeader.DID = m_ObjectInfo.usLocalDID;
    subHeader.TID = usTID;
    subHeader.SN = m_ObjectInfo.ucSubSysID;
    subHeader.O = ucSN;
    TLxTsspMessage message;
    message.pData =
            new char[sizeof(TLxTsspMessageHeader) + sizeof(TLxTsspSubMsgHeader) + usLen ];
    memcpy(message.pData, &header, sizeof(TLxTsspMessageHeader));
    memcpy(message.pData + sizeof(TLxTsspMessageHeader),
           &subHeader, sizeof(TLxTsspSubMsgHeader));
    memcpy(message.pData + sizeof(TLxTsspMessageHeader) + sizeof(TLxTsspSubMsgHeader),
           pData, usLen);
    message.iDataLen = sizeof(TLxTsspMessageHeader) + sizeof(TLxTsspSubMsgHeader) + usLen;

    int iRet = m_pILxTsspMessageManager->FireMessage(message);
    delete[] message.pData;
}

DWORDLONG SS1BBE_ModuParamSetGUIDlg::StrToSynCode(const char * pStr)
{
    char c,C;
    const char*  p;
    DWORDLONG dwlValue=0;
    char   DigVal=0;
    USHORT nDigCount=0,MAX_DIGITS=16; //The synchronizing code should be not longer  than 16 digitals

    p=pStr;     //Point to the first char of the string
    c = *p++;   // read the first char
    while ((c!=NULL) && (nDigCount<=MAX_DIGITS))//  Not the terminator of a string && total digitals is less than 16
    {
        if(isspace((int)(unsigned char)c))    c = *p++;// skip whitespace
        else if(c>='0'&&c<='9')                        //the char is a number between 1 and 9
        {
            DigVal=c-'0';
            dwlValue=dwlValue*16+DigVal;               //Accumulate the value of the hexal number so far to the current digital
            nDigCount++;
            c = *p++;
        }
        else if((C=toupper(c))>='A'&&C<='F')           //the char is a letter could be a digital of a hexal number
        {
            DigVal=C-'A'+10;
            dwlValue=dwlValue*16+DigVal;               //Accumulate the value of the hexal number so far to the current digital
            nDigCount++;
            c = *p++;
        }
        else    break;                                 // An invalid char that is neither whitespace nor a hexal digital is encounted
        // Conclude the scanning process
    }
    return dwlValue;
}

void SS1BBE_ModuParamSetGUIDlg::showTargetParam(int nIndex, int nType)
{
    //向下拉框控件中写入从xml中读取的内容
    if (nType == 0)
    {
        //扩频码速率
        ui->comboBox_FreExpandRate->clear();
        for(int i=0; i<DlgMapLineNum[nIndex * 5 + 4];i++)
        {
            ui->comboBox_FreExpandRate->addItem(m_DlgMapMean[nIndex * 5 + 4][i]);
        }
        ui->comboBox_FreExpandRate->setCurrentIndex(m_DlgParaSetStruct[nIndex + 1][2].InitVal.lval);
        //通道输出控制
        ui->comboBox_ChannelOutputCtrl->clear();
        for(int i=0; i<DlgMapLineNum[nIndex * 5 + 5];i++)
        {
            ui->comboBox_ChannelOutputCtrl->addItem(m_DlgMapMean[nIndex * 5 + 5][i]);
        }
        ui->comboBox_ChannelOutputCtrl->setCurrentIndex(m_DlgParaSetStruct[nIndex + 1][3].InitVal.lval);
        //I路伪码加调
        ui->comboBox_IVirtualAdd->clear();
        for(int i=0; i<DlgMapLineNum[nIndex * 5 + 6];i++)
        {
            ui->comboBox_IVirtualAdd->addItem(m_DlgMapMean[nIndex * 5 + 6][i]);
        }
        ui->comboBox_IVirtualAdd->setCurrentIndex(m_DlgParaSetStruct[nIndex + 1][5].InitVal.lval);
        //Q路伪码加调
        ui->comboBox_QVirtualAdd->clear();
        for(int i=0; i<DlgMapLineNum[nIndex * 5 + 7];i++)
        {
            ui->comboBox_QVirtualAdd->addItem(m_DlgMapMean[nIndex * 5 + 7][i]);
        }
        ui->comboBox_QVirtualAdd->setCurrentIndex(m_DlgParaSetStruct[nIndex + 1][6].InitVal.lval);
        //数据加调
        ui->comboBox_DataAdd->clear();
        for(int i=0; i<DlgMapLineNum[nIndex * 5 + 8];i++)
        {
            ui->comboBox_DataAdd->addItem(m_DlgMapMean[nIndex * 5 + 8][i]);
        }
        ui->comboBox_DataAdd->setCurrentIndex(m_DlgParaSetStruct[nIndex + 1][7].InitVal.lval);
        //多普勒预置频率
        ui->lineEdit_DopplerPredFre->setText(QString::number(m_DlgParaSetStruct[nIndex + 1][0].InitVal.lval));
        ui->label_DopplerPredFre->setText("多普勒预置频率\n[" + QString::number(m_DlgParaSetStruct[nIndex + 1][0].MinVal.lmin) +
                "~" + QString::number(m_DlgParaSetStruct[nIndex + 1][0].MaxVal.lmax) + "]");
        //输出功率衰减
        ui->lineEdit_OutputPowerDecrease->setText(QString::number(m_DlgParaSetStruct[nIndex + 1][1].InitVal.lval));
        //IQ功率比
        ui->lineEdit_IORate->setText(QString::number(m_DlgParaSetStruct[nIndex + 1][4].InitVal.lval));

    }
    else if (nType == 1)
    {

        //扩频码速率
        if (m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucPNRate > 0)
            ui->comboBox_FreExpandRate->setCurrentIndex(m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex ].ucPNRate - 1);
        //通道输出控制
        if (m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucOutputSwitch > 0)
            ui->comboBox_ChannelOutputCtrl->setCurrentIndex(m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucOutputSwitch - 1);
        //I路伪码加调
        if (m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucPNIModuSw > 0)
            ui->comboBox_IVirtualAdd->setCurrentIndex(m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucPNIModuSw - 1);
        //Q路伪码加调
        if (m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucQPNModuSw > 0)
            ui->comboBox_QVirtualAdd->setCurrentIndex(m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucQPNModuSw - 1);
        //数据加调
        if (m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucTCModuSw > 0)
            ui->comboBox_DataAdd->setCurrentIndex(m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucTCModuSw - 1);
        //多普勒预置频率
        ui->lineEdit_DopplerPredFre->setText(QString::number(m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucPreDplValue));
        //输出功率衰减
        ui->lineEdit_OutputPowerDecrease->setText(QString::number((int)(char)m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].cPowAdjust * (-1)));
        //IQ功率比
        ui->lineEdit_IORate->setText(QString::number((short)(char)m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[nIndex].ucIQRatioP));
        on_comboBox_ChannelOutputCtrl_currentIndexChanged(ui->comboBox_ChannelOutputCtrl->currentIndex());

    }

}

void SS1BBE_ModuParamSetGUIDlg::getParamTarget()
{
    //扩频码速率
    m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[m_nTargetNum].ucPNRate = ui->comboBox_FreExpandRate->currentIndex() + 1;
    //通道输出控制
    m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[m_nTargetNum].ucOutputSwitch = ui->comboBox_ChannelOutputCtrl->currentIndex() + 1;
    //I路伪码加调
    m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[m_nTargetNum].ucPNIModuSw = ui->comboBox_IVirtualAdd->currentIndex() +1;
    //Q路伪码加调
    m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[m_nTargetNum].ucQPNModuSw = ui->comboBox_QVirtualAdd->currentIndex() + 1;
    //数据加调
    m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[m_nTargetNum].ucTCModuSw = ui->comboBox_DataAdd->currentIndex() + 1;
    //多普勒预置频率
    m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[m_nTargetNum].ucPreDplValue = ui->lineEdit_DopplerPredFre->text().toInt();
    //输出功率衰减
    m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[m_nTargetNum].cPowAdjust = ui->lineEdit_OutputPowerDecrease->text().toInt() * (-1);
    //IQ功率比
    m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[m_nTargetNum].ucIQRatioP = ui->lineEdit_IORate->text().toInt();
}

int SS1BBE_ModuParamSetGUIDlg::getSharedMemoryTargetNum()
{
    DWORD stLen = sizeof(SMCU::MCSSysTarget4StatusStruct);
    if(m_pStateMgr != NULL)
        m_pMCSTargetSysStatus  = (SMCU::MCSSysTarget4StatusStruct*)m_pStateMgr->FindOneItem(m_CommTargetID, stLen);

    if (m_pMCSTargetSysStatus != NULL)
        return m_pMCSTargetSysStatus->sysStatu.tParams.ucTargetNum;
    return 0;
}


void SS1BBE_ModuParamSetGUIDlg::showRadioButton()
{
    int nNum = getSharedMemoryTargetNum();
    if (m_targetNum_usedInParam == nNum)
        return;
    m_targetNum_usedInParam = nNum;
    m_nTargetNum = 0;
    set_radioButton_Target();
    ui->radioButton_Target1->hide();
    ui->radioButton_Target2->hide();
    ui->radioButton_Target3->hide();
    ui->radioButton_Target4->hide();
    switch(nNum)
    {
    case 1:
        ui->radioButton_Target1->show();
        break;
    case 2:
        ui->radioButton_Target1->show();
        ui->radioButton_Target2->show();
        break;
    case 3:
        ui->radioButton_Target1->show();
        ui->radioButton_Target2->show();
        ui->radioButton_Target3->show();
        break;
    case 4:
        ui->radioButton_Target1->show();
        ui->radioButton_Target2->show();
        ui->radioButton_Target3->show();
        ui->radioButton_Target4->show();
        break;
    default:
        break;
    }
}

void SS1BBE_ModuParamSetGUIDlg::set_radioButton_Target()
{
    switch(m_nTargetNum)
    {
    case 0:
        ui->radioButton_Target1->setChecked(true);
        break;
    case 1:
        ui->radioButton_Target2->setChecked(true);
        break;
    case 2:
        ui->radioButton_Target3->setChecked(true);
        break;
    case 3:
        ui->radioButton_Target4->setChecked(true);
        break;
    default:
        ui->radioButton_Target1->setChecked(true);
    }
}

void SS1BBE_ModuParamSetGUIDlg::on_radioButton_Target1_clicked()
{
    if(1 != CheckPara())
    {
        set_radioButton_Target();
        return;
    }
    GetPara();
    m_nTargetNum = 0;
    //int nType = ui->pushButton_Set->isEnabled()  ? 0 : 1;
    showTargetParam(0, 1);
}

void SS1BBE_ModuParamSetGUIDlg::on_radioButton_Target2_clicked()
{
    if(1 != CheckPara())
    {
        set_radioButton_Target();
        return;
    }
    GetPara();
    m_nTargetNum = 1;
    //int nType = ui->pushButton_Set->isEnabled()  ? 0 : 1;
    showTargetParam(1, 1);
}

void SS1BBE_ModuParamSetGUIDlg::on_radioButton_Target3_clicked()
{
    if(1 != CheckPara())
    {
        set_radioButton_Target();
        return;
    }
    GetPara();
    m_nTargetNum = 2;
    //int nType = ui->pushButton_Set->isEnabled()  ? 0 : 1;
    showTargetParam(2, 1);
}

void SS1BBE_ModuParamSetGUIDlg::on_radioButton_Target4_clicked()
{
    if(1 != CheckPara())
    {
        set_radioButton_Target();
        return;
    }
    GetPara();
    m_nTargetNum = 3;
    //int nType = ui->pushButton_Set->isEnabled()  ? 0 : 1;
    showTargetParam(3, 1);
}


//使能或不使能控件
void SS1BBE_ModuParamSetGUIDlg::EnabledControls(bool bFlag)
{
    /*************************Group1*************************/

    //多普勒预置频率
    ui->lineEdit_DopplerPredFre->setEnabled(bFlag);
    //输出功率衰减
    ui->lineEdit_OutputPowerDecrease->setEnabled(bFlag);
    //扩频码速率
    ui->comboBox_FreExpandRate->setEnabled(bFlag);
    //通道输出控制
    ui->comboBox_ChannelOutputCtrl->setEnabled(bFlag);
    //IO功率比
    ui->lineEdit_IORate->setEnabled(bFlag);
    //I路伪码加调
    ui->comboBox_IVirtualAdd->setEnabled(bFlag);
    //Q路伪码加调
    ui->comboBox_QVirtualAdd->setEnabled(bFlag);
    //数据加调
    ui->comboBox_DataAdd->setEnabled(bFlag);
    /*************************Group1*************************/

    /*************************帧同步器*************************/
    if (bFlag)
    {
        ui->pushButton_Modify->setText("恢复");
        ui->pushButton_Set->setEnabled(true);
    }
    else
    {
        ui->pushButton_Modify->setText("更改");
        ui->pushButton_Set->setEnabled(false);
    }
}
void SS1BBE_ModuParamSetGUIDlg::EnabledControlsCommon(bool bFlag){
    //载波输出电平
    ui->lineEdit_OutputElLevel->setEnabled(bFlag);
    //调制方式方法
    ui->comboBox_AdjustType->setEnabled(bFlag);
    //载波控制
    ui->comboBox_WaveCtrl->setEnabled(bFlag);
    //噪声源输出
    ui->comboBox_NoiseSourOutput->setEnabled(bFlag);
    //载波输出载噪比
    ui->lineEdit_WaveNoiseRatio->setEnabled(bFlag);
    //回零速率
    ui->lineEdit_BackZeroRate->setEnabled(bFlag);
    //补偿方式
    ui->comboBox_CompensateType->setEnabled(bFlag);
    //补偿周期
    ui->lineEdit_CompensateTime->setEnabled(bFlag);
    //复选框
    ui->checkBox_dopplerSw->setEnabled(bFlag);
    if (bFlag)
    {
        ui->pushButton_ModifyCommon->setText("恢复");
        ui->pushButton_SetCommon->setEnabled(true);
    }
    else
    {
        ui->pushButton_ModifyCommon->setText("更改");
        ui->pushButton_SetCommon->setEnabled(false);
    }
}

//更改参数按钮休息处理函数
void SS1BBE_ModuParamSetGUIDlg::on_pushButton_Modify_clicked()
{
    if(ui->pushButton_Modify->text() == "更改")
    {
        EnabledControls(true);
        killTimer(m_TimerID);
        on_comboBox_ChannelOutputCtrl_currentIndexChanged(ui->comboBox_ChannelOutputCtrl->currentIndex());
        ui->pushButton_ModifyCommon->setEnabled(false);
        ui->pushButton_SetCommon->setEnabled(false);
        return;
    }

    if(ui->pushButton_Modify->text() == "恢复")
    {
        EnabledControls(false);
        m_TimerID = startTimer(500);
        ui->pushButton_ModifyCommon->setEnabled(true);
        //ui->pushButton_SetCommon->setEnabled(true);
        return;
    }
}

void SS1BBE_ModuParamSetGUIDlg::on_pushButton_Set_clicked()
{
    //验证参数范围
    if(CheckPara() == 1)
    {
        //从界面取参数
        GetPara();

        //设置参数
        //取当前设置的参数
        if(m_pHAManager != NULL)
        {
            struct DATA{
                USHORT target;
                struct{
                    USHORT tmp;
                    SMCU::MCSTargetTCSSUpModulParamStruct targetParam;
                }param[MAX_TARGET_COUNT];
            }TargetParam;
            TargetParam.target = 0x02 | 0x04 | 0x08 | 0x10;
            for(int i = 0; i < MAX_TARGET_COUNT; ++i)
            {
                TargetParam.param[i].targetParam = m_MCSSSUTargetTCSSUpModulParam.targetTCSSUpModulParam[i];
            }
            QByteArray byteArray;
            byteArray.resize(sizeof(TargetParam));
            memcpy(byteArray.data(),&TargetParam,sizeof(TargetParam));
            SendMessage((char*)&TargetParam, sizeof(TargetParam), m_usC, m_usTID, m_ucSN);
        }

        //日志信息
        TLxTsspLog tmpLog;
        QString strLog;
        memset(&tmpLog, 0, sizeof(TLxTsspLog));
        tmpLog.ucWarningLevel  = 1;
        strLog += "解调参数设置: 设置参数成功！";
        strcpy(tmpLog.szContent, strLog.toUtf8().data());
        tmpLog.time = QDateTime::currentDateTime().toTime_t();

        m_pILxTsspLogManager->Add(tmpLog);

        //不使能控件
        EnabledControls(false);
        ui->pushButton_ModifyCommon->setEnabled(true);

        m_TimerID = startTimer(500);
    }
}
void SS1BBE_ModuParamSetGUIDlg::on_pushButton_ModifyCommon_clicked()
{
    if(ui->pushButton_ModifyCommon->text() == "更改")
    {
        EnabledControlsCommon(true);
        killTimer(m_TimerID);
        on_comboBox_NoiseSourOutput_currentIndexChanged(ui->comboBox_NoiseSourOutput->currentIndex());
        on_checkBox_dopplerSw_clicked();
        on_comboBox_CompensateType_currentIndexChanged(ui->comboBox_CompensateType->currentIndex());
        ui->pushButton_Modify->setEnabled(false);
        ui->pushButton_Set->setEnabled(false);
    }
    else if(ui->pushButton_ModifyCommon->text() == "恢复")
    {
        EnabledControlsCommon(false);
        m_TimerID = startTimer(500);
        ui->pushButton_Modify->setEnabled(true);
        //ui->pushButton_Set->setEnabled(true);
    }

}

void SS1BBE_ModuParamSetGUIDlg::on_pushButton_SetCommon_clicked()
{
    //验证参数范围
    if(CheckPara() == 1)
    {
        //从界面取参数
        GetPara();

        //设置参数
        //取当前设置的参数
        if(m_pHAManager != NULL)
        {
            struct DATA{
                USHORT target;
                USHORT tmp;
                SMCU::MCSSSUpModulParamStruct SysParam;
            }data;
            data.target = 0x01;
            data.SysParam = m_MCSSSUTargetTCSSUpModulParam.SSUpModulParam;
            QByteArray byteArray;
            byteArray.resize(sizeof(data));
            memcpy(byteArray.data(),&data,sizeof(data));
            SendMessage((char*)&data, sizeof(data), m_usC, m_usTID, m_ucSN);
            byteArray.clear();
            //过程控制命令
            struct{
                UCHAR unitNum;
                UCHAR chl;
                UCHAR compensateCtrl;
                UCHAR compensateType;
                UINT  compensatePeriod;
                USHORT resetSpeed;
            }dopplerCom;
            dopplerCom.unitNum = 41;
            switch (m_nTargetNum) {
            case 0:
                dopplerCom.chl = 1;
                break;
            case 1:
                dopplerCom.chl = 2;
                break;
            case 2:
                dopplerCom.chl = 3;
                break;
            case 3:
                dopplerCom.chl = 4;
            default:
                break;
            }
            dopplerCom.compensateCtrl = ui->checkBox_dopplerSw->checkState() ? 1 : 2;//补偿开关
            dopplerCom.compensateType = ui->comboBox_CompensateType->currentIndex() + 2;//补偿方式
            dopplerCom.compensatePeriod = qRound(1000.0/(UINT)ui->lineEdit_CompensateTime->text().toFloat());//补偿周期
            dopplerCom.resetSpeed = ui->lineEdit_BackZeroRate->text().toInt();//回零速率
            SendMessage((char*)&dopplerCom, sizeof(dopplerCom), m_sItem.usCMD, m_sItem.usTID, m_sItem.ucSN);
            m_doppler = true;
            if (m_pDoppler != NULL)
            {
                m_pDoppler->compensateCtrl = dopplerCom.compensateCtrl;
                m_pDoppler->compensateType = dopplerCom.compensateType;
                m_pDoppler->compensatePeriod = dopplerCom.compensatePeriod;
                m_pDoppler->resetSpeed = dopplerCom.resetSpeed;
                m_pDoppler->flag = true;
            }
        }

        //日志信息
        TLxTsspLog tmpLog;
        QString strLog;
        memset(&tmpLog, 0, sizeof(TLxTsspLog));
        tmpLog.ucWarningLevel  = 1;on_comboBox_CompensateType_currentIndexChanged(ui->comboBox_CompensateType->currentIndex());
        strLog += "解调参数设置: 设置参数成功！";
        strcpy(tmpLog.szContent, strLog.toUtf8().data());
        tmpLog.time = QDateTime::currentDateTime().toTime_t();

        m_pILxTsspLogManager->Add(tmpLog);

        //不使能控件
        EnabledControlsCommon(false);
        ui->pushButton_Modify->setEnabled(true);

        m_TimerID = startTimer(500);
    }

}

void SS1BBE_ModuParamSetGUIDlg::processCtrl()
{
    if(!m_doppler || 0 == m_ctlCmd || m_pStateMgr == NULL)return;//使用过过程控制命令才执行下面
    if(m_pCtlCmd)
    {
        DCCTRL& doppler = m_pCtlCmd->dcCtrl41;
        //        if(doppler. != 1 && doppler.dopplerSwitch != 2)return;//2关，1开
        //        ui->checkBox_dopplerSw->setChecked(doppler.dopplerSwitch == 2 ? false : true);
        //        ui->lineEdit_BackZeroRate->setText(QString::number(doppler.));
        //        ui->lineEdit_DopplerRate->setText(QString::number(doppler.rateOfChange));
        return;
    }
    DWORD stLen = sizeof(PCRSS1);
    m_pCtlCmd  = (PCRSS1*)m_pStateMgr->FindOneItem(m_ctlCmd, stLen);
}

//噪声源输出
void SS1BBE_ModuParamSetGUIDlg::on_comboBox_NoiseSourOutput_currentIndexChanged(int index)
{
    if(ui->pushButton_ModifyCommon->text() == "更改")
        return ;
    if(index ==0){
        ui->lineEdit_WaveNoiseRatio->setEnabled(true);
    }
    else
    {
        ui->lineEdit_WaveNoiseRatio->setEnabled(false);
    }

}
//多普乐补偿on_comboBox_CompensateType_currentIndexChanged(ui->comboBox_CompensateType->currentIndex());

void SS1BBE_ModuParamSetGUIDlg::on_checkBox_dopplerSw_clicked()
{
    if(ui->pushButton_ModifyCommon->text() == "更改")
        return ;
    if(ui->checkBox_dopplerSw->checkState()){
        ui->comboBox_CompensateType->setEnabled(true);
    }
    else{
        ui->comboBox_CompensateType->setEnabled(false);
    }
    on_comboBox_CompensateType_currentIndexChanged(ui->comboBox_CompensateType->currentIndex());
}
//补偿方式
void SS1BBE_ModuParamSetGUIDlg::on_comboBox_CompensateType_currentIndexChanged(int index)
{
    if(ui->pushButton_ModifyCommon->text() == "更改")
        return ;
    if(ui->checkBox_dopplerSw->checkState()){
        if(index == 0)
            ui->lineEdit_CompensateTime->setEnabled(false);
        else
            ui->lineEdit_CompensateTime->setEnabled(true);
    }
    else{
        ui->comboBox_CompensateType->setEnabled(false);
        ui->lineEdit_CompensateTime->setEnabled(false);
    }
}
//通道输出控制
void SS1BBE_ModuParamSetGUIDlg::on_comboBox_ChannelOutputCtrl_currentIndexChanged(int index)
{
    if(ui->pushButton_Modify->text() == "更改")
        return;
    if(index == 0){
        //多普勒预置频率
        ui->lineEdit_DopplerPredFre->setEnabled(true);
        //输出功率衰减
        ui->lineEdit_OutputPowerDecrease->setEnabled(true);
        //扩频码速率
        ui->comboBox_FreExpandRate->setEnabled(true);
        //IO功率比
        ui->lineEdit_IORate->setEnabled(true);
        //I路伪码加调
        ui->comboBox_IVirtualAdd->setEnabled(true);
        //Q路伪码加调
        ui->comboBox_QVirtualAdd->setEnabled(true);
        //数据加调
        ui->comboBox_DataAdd->setEnabled(true);
    }
    else{
        //多普勒预置频率
        ui->lineEdit_DopplerPredFre->setEnabled(false);
        //输出功率衰减
        ui->lineEdit_OutputPowerDecrease->setEnabled(false);
        //扩频码速率
        ui->comboBox_FreExpandRate->setEnabled(false);
        //IO功率比
        ui->lineEdit_IORate->setEnabled(false);
        //I路伪码加调
        ui->comboBox_IVirtualAdd->setEnabled(false);
        //Q路伪码加调
        ui->comboBox_QVirtualAdd->setEnabled(false);
        //数据加调
        ui->comboBox_DataAdd->setEnabled(false);
    }

}

void SS1BBE_ModuParamSetGUIDlg::updateRadioButton()
{
    if(!m_pCommFunc)return;
    if (m_targetNum_usedInMacro == m_pCommFunc->getTargetNum())
        return;
    m_targetNum_usedInMacro = m_pCommFunc->getTargetNum();
    m_nTargetNum = 0;
    set_radioButton_Target();
    ui->radioButton_Target1->hide();
    ui->radioButton_Target2->hide();
    ui->radioButton_Target3->hide();
    ui->radioButton_Target4->hide();
    switch( m_targetNum_usedInMacro - 1)
    {
    case 0:
        ui->radioButton_Target1->show();
        break;
    case 1:
        ui->radioButton_Target1->show();
        ui->radioButton_Target2->show();
        break;
    case 2:
        ui->radioButton_Target1->show();
        ui->radioButton_Target2->show();
        ui->radioButton_Target3->show();
        break;
    case 3:
        ui->radioButton_Target1->show();
        ui->radioButton_Target2->show();
        ui->radioButton_Target3->show();
        ui->radioButton_Target4->show();
        break;
    default:
        break;
    }
}
#pragma pack()
