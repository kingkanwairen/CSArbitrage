#ifndef SGIT_MD_SPI_H
#define SGIT_MD_SPI_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QThread>
#include "SgitFtdcMdApi.h"

class SgitMdSpi : public QObject, public CSgitFtdcMdSpi
{
	Q_OBJECT
public:

	SgitMdSpi(CSgitFtdcMdApi* pMdApi);

	virtual~SgitMdSpi();

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(char *pErrMsg);

	virtual void OnRspUserLogin(CSgitFtdcRspUserLoginField *pRspUserLogin, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnDepthMarketData(CSgitFtdcDepthMarketDataField *pMarketData);

signals:
	void onSgitMdFrontConnected();
	void onSgitMdFrontDisconnected(const QString& reason);
	void onSgitMdRspUserLogin(const QString& brokerId, const QString& userId, const int& errorId, const QString& errorMsg, bool bIsLast);
	void onSgitRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
		const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
		const double& lowestPrice, const int& volume);

private:
	QThread mMdThread;
	CSgitFtdcMdApi* m_pMdApi;
};

#endif