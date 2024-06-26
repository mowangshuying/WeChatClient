#include "WSClientMgr.h"
#include <QDebug>
#include <QThread>

WSClientMgr* WSClientMgr::m_WsClientMgr = nullptr;

WSClientMgr::WSClientMgr()
{
    m_bConn = false;
    m_webSock = new QWebSocket();

    // 创建一个定时器
    m_timer = new QTimer();
    m_timer->start(1000);

    connect(m_timer, &QTimer::timeout, this, &WSClientMgr::slotTimer);
    connect(m_webSock, &QWebSocket::connected, this, &WSClientMgr::slotConnected);
    connect(m_webSock, &QWebSocket::disconnected, this, &WSClientMgr::slotDisconnected);
    connect(m_webSock, &QWebSocket::textMessageReceived, this, &WSClientMgr::slotRecvMsg);

    // 连接远端服务器
    m_webSock->open(QUrl(CHAT_SERVER_ADDR));
    // m_webSock->open(QUrl("ws://127.0.0.1:5000"));
    LogDebug << "threadId:" << QThread::currentThread()->currentThreadId();
}

WSClientMgr::~WSClientMgr()
{
    if (m_webSock)
        delete m_webSock;

    if (m_timer)
    {
        m_timer->stop();
        delete m_timer;
    }
}

void WSClientMgr::initMgr()
{
    getMgr();
}

void WSClientMgr::exitMgr()
{
    if (m_WsClientMgr)
    {
        delete m_WsClientMgr;
    }
}

WSClientMgr* WSClientMgr::getMgr()
{
    if (m_WsClientMgr == nullptr)
    {
        m_WsClientMgr = new WSClientMgr();
    }
    return m_WsClientMgr;
}

void WSClientMgr::sendMsg(const QString& message)
{
    m_webSock->sendTextMessage(message);
}

void WSClientMgr::regMsgCall(QString cmd, NetEventCall netEventCall)
{
    m_Msg2CallbackMap[cmd] = netEventCall;
    // LogDebug << "threadId = " << QThread::currentThread()->currentThreadId();
}

void WSClientMgr::transfer(neb::CJsonObject& msg)
{
    QString str = msg.ToString().c_str();
    sendMsg(str);
}

void WSClientMgr::request(const std::string& cmd, neb::CJsonObject& data)
{
    neb::CJsonObject msg;
    msg.Add("cmd", cmd);
    msg.Add("type", msg_type_req);
    msg.Add("msgId", ++m_MsgId);

    // 这里时间是不准确的，需要和服务同步
    int time = QTime::currentTime().msec();

    msg.Add("time", time);
    msg.Add("data", data);

    transfer(msg);
}

void WSClientMgr::request(const std::string& cmd, neb::CJsonObject& json, NetEventCall call)
{
    request(cmd, json);
    if (call != nullptr)
    {
        NetEventCallData calldata;
        calldata.callFun = call;
        calldata.dt = QTime::currentTime().msec();
        m_RequestMsg2CallbackMap[m_MsgId] = calldata;
    }
}

void WSClientMgr::onNetMsgDo(int64 msgId, neb::CJsonObject& msgJson)
{
    auto itf = m_RequestMsg2CallbackMap.find(msgId);
    if (itf != m_RequestMsg2CallbackMap.end())
    {
        itf->second.callFun(msgJson);
        m_RequestMsg2CallbackMap.erase(itf);
        return;
    }
    LogDebug << "can not find msgid = " << msgId << " in onNetMsgDo ";
}

void WSClientMgr::onNetMsgDo(std::string cmd, neb::CJsonObject& msgJson)
{
    QString qcmdStr = cmd.c_str();
    LogDebug << "cmd:" << qcmdStr;
    auto itf = m_Msg2CallbackMap.find(qcmdStr);
    if (itf != m_Msg2CallbackMap.end())
    {
        itf->second(msgJson);
        return;
    }
    LogDebug << "can not find cmd = " << qcmdStr << " in onNetMsgDo ";
}

void WSClientMgr::slotConnected()
{
    LogDebug << "slot_connected()...";
    // LogDebug << "threadId = " << QThread::currentThread()->currentThreadId();
    // 向远端服务器发送一个注册消息
    neb::CJsonObject json;
    json.Add("type", "Client");
    json.Add("cckey", "ccmm00@123456");
    request("cs_reg_client", json, [this](neb::CJsonObject& msg) {
        LogDebug << "reg client suc";
        m_bConn = true;
    });
}

void WSClientMgr::slotDisconnected()
{
    LogDebug << "slot_disconnected...";
    m_bConn = false;
}

void WSClientMgr::slotRecvMsg(const QString& message)
{
    // LogDebug << "threadId = " << QThread::currentThread()->currentThreadId();
    neb::CJsonObject json;
    if (!json.Parse(message.toStdString()))
    {
        LogDebug << "json parse failed in slot_recvMsg";
        return;
    }

    //
    int msgType = 0;
    if (!json.Get("type", msgType))
    {
        LogDebug << "can not find msgType in slot_recvMsg";
        return;
    }

    if (msgType == msg_type_resp)
    {
        int msgId = 0;
        if (!json.Get("msgId", msgId))
        {
            LogDebug << "can not find msgid in slot_recvMsg";
            return;
        }
        onNetMsgDo(msgId, json);
        return;
    }

    if (msg_type_push == msgType || msg_type_broadcast == msgType)
    {
        std::string cmd;
        if (!json.Get("cmd", cmd))
        {
            LogDebug << "can not find cmd in slot_recvMsg";
            return;
        }

        onNetMsgDo(cmd, json);
        return;
    }
}

void WSClientMgr::slotTimer()
{
    m_time++;
    if (m_time % 5 == 0)
    {
        // 5秒向网关服务器发送心跳
        neb::CJsonObject json;
        request("cs_msg_heart", json);
    }

    if (!m_bConn)
    {
        m_webSock->open(QUrl(CHAT_SERVER_ADDR));
        LogDebug << "Retry to connect server";
    }
}
