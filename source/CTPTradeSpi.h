#ifndef CTP_OMS_SPI_H
#define CTP_OMS_SPI_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QThread>
#include "ThostFtdcTraderApi.h"

class CTPTradeSpi : public QObject, public CThostFtdcTraderSpi
{
	Q_OBJECT
public:

	CTPTradeSpi();

	virtual~CTPTradeSpi();

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason);

	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);

	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

signals:

	void onCTPTradeFrontConnected();

	void onCTPTradeFrontDisconnected(int reason);

	void onCTPTradeRspUserLogin(const QString& tradingDate, const QString& brokerId, const QString& userId, const int& maxLocalId, const int& frontId,
		const int& sessionId, const int& errorId, const QString& errorMsg, bool bIsLast);

	void onCTPTradeRspUserLogout(const QString& brokerId, const QString& userId, const int& errorId, const QString& errorMsg);
	
	void onCTPTradeSettlementInfoConfirm(const QString& brokerId, const QString& investorId, const int& errorId, const QString& errorMsg);

	void onCTPRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
		const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
		const QString& localId, const QString& orderRef, const QString& tradeTime);

	void onCTPRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
		const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
		const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId);

	void onCTPErrRtnOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
		const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
		const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId);

	void onCTPErrRtnOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
		const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
		const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId);

	void onCTPRtnInstrumentStatus(const QString& exchangeId, const QString& instrumentId, const char& instrumentStatus);
	
	void onCTPRspQryInstrument(const QString& exchangeId, const QString& productId, const QString& instrumentId, const double& priceTick, 
		const int& volumeMultiple, const int& maxMarketVolume, bool bIsLast);
	
	void onCTPRspQryInvestorPosition(const QString& instrumentId, const char& direction, const int& position, const int& yPosition, const char& hedgeFlag,
		const QString& brokerId, const QString& tradingDay, const QString& investorId, bool bIsLast);	

private:
	QThread mTradeThread;
};

#endif