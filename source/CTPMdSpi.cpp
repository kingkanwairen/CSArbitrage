#include "CTPMdSpi.h"
#include "USTPConfig.h"
#include "USTPLogger.h"
#include <QtCore/QThread>

CTPMdSpi::CTPMdSpi()
{
	moveToThread(&mMdThread);
	mMdThread.start();
}

CTPMdSpi::~CTPMdSpi()
{
	mMdThread.quit();
	mMdThread.wait();
}

void CTPMdSpi::OnFrontConnected()
{	
	emit onCTPMdFrontConnected();
	QString data = QString(tr("[CTP| Md-Connected]  "));
	USTPLogger::saveData(data);
}

void CTPMdSpi::OnFrontDisconnected(int nReason)
{	
	emit onCTPMdFrontDisconnected(nReason);
	QString data = QString(tr("[CTP| Md-Disconneted]  ")) +  QString::number(nReason);
	USTPLogger::saveData(data);
}

void CTPMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspUserLogin != NULL && pRspInfo != NULL){
		emit onCTPMdRspUserLogin(QString(pRspUserLogin->BrokerID), QString(pRspUserLogin->UserID), pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg), bIsLast);
	}
#ifdef _MD_DEBUG
	QString data = QString(tr("[CTP| Md-RspUserLogin]  BrokerId: ")) + QString(pRspUserLogin->BrokerID) + tr("  UserId: ") + QString(pRspUserLogin->UserID)
		+ tr("  ErrorMsg: ") + QString::fromLocal8Bit(pRspInfo->ErrorMsg);
	USTPLogger::saveData(data);
#endif
}

void CTPMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	if (pDepthMarketData != NULL){
		emit onCTPRtnDepthMarketData(QString(pDepthMarketData->InstrumentID), pDepthMarketData->PreSettlementPrice, pDepthMarketData->OpenPrice, 
			pDepthMarketData->LastPrice, pDepthMarketData->BidPrice1, pDepthMarketData->BidVolume1, pDepthMarketData->AskPrice1, pDepthMarketData->AskVolume1,
			pDepthMarketData->UpperLimitPrice, pDepthMarketData->LowerLimitPrice, pDepthMarketData->Volume);

#ifdef _MD_DEBUG
		QString data = QString(tr("[CTP| Depth]  InstrumentId: ")) + QString(pDepthMarketData->InstrumentID) + QString(tr("  BidPrice: ")) +
			QString::number(pDepthMarketData->BidPrice1) + QString(tr("  BidVolume: ")) + QString::number(pDepthMarketData->BidVolume1) + QString(tr("  AskPrice: ")) +
			QString::number(pDepthMarketData->AskPrice1) + QString(tr("  AskVolume: ")) + QString::number(pDepthMarketData->AskVolume1);
		USTPLogger::saveData(data);
#endif

	}
}

#include "moc_CTPMdSpi.cpp"