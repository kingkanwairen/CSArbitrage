#ifndef SGIT_OMS_SPI_H
#define SGIT_OMS_SPI_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QThread>
#include "SgitFtdcTraderApi.h"

class SgitTradeSpi : public QObject, public CSgitFtdcTraderSpi
{
	Q_OBJECT
public:

	SgitTradeSpi(CSgitFtdcTraderApi* pTradeApi);

	virtual~SgitTradeSpi();

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(char *pErrMsg);

	virtual void OnRspUserLogin(CSgitFtdcRspUserLoginField *pRspUserLogin, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogout(CSgitFtdcUserLogoutField *pUserLogout, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnTrade(CSgitFtdcTradeField *pTrade);

	virtual void OnRtnOrder(CSgitFtdcOrderField *pOrder,CSgitFtdcRspInfoField *pRspInfo);

	virtual void OnRspOrderInsert(CSgitFtdcInputOrderField *pInputOrder, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderAction(CSgitFtdcInputOrderActionField *pInputOrderAction, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnInstrumentStatus(CSgitFtdcInstrumentStatusField *pInstrumentStatus);

	virtual void OnRspQryInstrument(CSgitFtdcInstrumentField *pInstrument, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryInvestorPosition(CSgitFtdcInvestorPositionField *pInvestorPosition, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

signals:

	void onSgitTradeFrontConnected();

	void onSgitTradeFrontDisconnected(const QString& reason);

	void onSgitTradeRspUserLogin(const QString& tradingDate, const QString& brokerId, const QString& userId, const int& maxLocalId, const int& frontId,
		const int& sessionId, const int& errorId, const QString& errorMsg, bool bIsLast);

	void onSgitTradeRspUserLogout(const QString& brokerId, const QString& userId, const int& errorId, const QString& errorMsg);

	void onSgitRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
		const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
		const QString& localId, const QString& orderRef, const QString& tradeTime);

	void onSgitRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
		const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
		const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId);

	void onSgitRspOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
		const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
		const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId);

	void onSgitRspOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
		const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
		const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId);

	void onSgitRtnInstrumentStatus(const QString& exchangeId, const QString& instrumentId, const char& instrumentStatus);
	
	void onSgitRspQryInstrument(const QString& exchangeId, const QString& productId, const QString& instrumentId, const double& priceTick, 
		const int& volumeMultiple, const int& maxMarketVolume, bool bIsLast);
	
	void onSgitRspQryInvestorPosition(const QString& instrumentId, const char& direction, const int& position, const int& yPosition, const char& hedgeFlag,
		const QString& brokerId, const QString& tradingDay, const QString& investorId, bool bIsLast);	

private:
	QThread mTradeThread;
	CSgitFtdcTraderApi* m_pTradeApi;
};

#endif