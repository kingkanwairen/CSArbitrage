#include "SgitMdSpi.h"
#include "USTPConfig.h"
#include "USTPLogger.h"
#include <QtCore/QThread>

SgitMdSpi::SgitMdSpi(CSgitFtdcMdApi* pMdApi)
{
	moveToThread(&mMdThread);
	mMdThread.start();
	m_pMdApi = pMdApi;
}

SgitMdSpi::~SgitMdSpi()
{
	mMdThread.quit();
	mMdThread.wait();
}

void SgitMdSpi::OnFrontConnected()
{	
	emit onSgitMdFrontConnected();
	QString data = QString(tr("[SGIT| Md-Connected]  "));
	USTPLogger::saveData(data);
}

void SgitMdSpi::OnFrontDisconnected(char *pErrMsg)
{	
	emit onSgitMdFrontDisconnected(QString(pErrMsg));
	QString data = QString(tr("[SGIT| Md-Disconneted]  ")) +  QString(pErrMsg);
	USTPLogger::saveData(data);
}

void SgitMdSpi::OnRspUserLogin(CSgitFtdcRspUserLoginField *pRspUserLogin, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspUserLogin != NULL && pRspInfo != NULL){
		if (pRspInfo->ErrorID == 0){
			m_pMdApi->Ready();
		}
		emit onSgitMdRspUserLogin(QString(pRspUserLogin->BrokerID), QString(pRspUserLogin->UserID), pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg), bIsLast);
	}
#ifdef _MD_DEBUG
	QString data = QString(tr("[SGIT| Md-RspUserLogin]  BrokerId: ")) + QString(pRspUserLogin->BrokerID) + tr("  UserId: ") + QString(pRspUserLogin->UserID)
		+ tr("  ErrorMsg: ") + QString::fromLocal8Bit(pRspInfo->ErrorMsg);
	USTPLogger::saveData(data);
#endif
}

void SgitMdSpi::OnRtnDepthMarketData(CSgitFtdcDepthMarketDataField *pDepthMarketData)
{
	if (pDepthMarketData != NULL){
		emit onSgitRtnDepthMarketData(QString(pDepthMarketData->InstrumentID), pDepthMarketData->PreSettlementPrice, pDepthMarketData->OpenPrice, 
			pDepthMarketData->LastPrice, pDepthMarketData->BidPrice1, pDepthMarketData->BidVolume1, pDepthMarketData->AskPrice1, pDepthMarketData->AskVolume1,
			pDepthMarketData->UpperLimitPrice, pDepthMarketData->LowerLimitPrice, pDepthMarketData->Volume);

#ifdef _MD_DEBUG
		QString data = QString(tr("[SGIT| Depth]  InstrumentId: ")) + QString(pDepthMarketData->InstrumentID) + QString(tr("  BidPrice: ")) +
			QString::number(pDepthMarketData->BidPrice1) + QString(tr("  BidVolume: ")) + QString::number(pDepthMarketData->BidVolume1) + QString(tr("  AskPrice: ")) +
			QString::number(pDepthMarketData->AskPrice1) + QString(tr("  AskVolume: ")) + QString::number(pDepthMarketData->AskVolume1);
		USTPLogger::saveData(data);
#endif

	}
}

#include "moc_SgitMdSpi.cpp"