#ifndef CTP_MD_SPI_H
#define CTP_MD_SPI_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QThread>
#include "ThostFtdcMdApi.h"

class CTPMdSpi : public QObject, public CThostFtdcMdSpi
{
	Q_OBJECT
public:

	CTPMdSpi();

	virtual~CTPMdSpi();

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason);

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

signals:
	void onCTPMdFrontConnected();
	void onCTPMdFrontDisconnected(int reason);
	void onCTPMdRspUserLogin(const QString& brokerId, const QString& userId, const int& errorId, const QString& errorMsg, bool bIsLast);
	void onCTPRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
		const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
		const double& lowestPrice, const int& volume);

private:
	QThread mMdThread;
};

#endif