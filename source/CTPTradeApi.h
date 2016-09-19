#ifndef CTP_TRADE_API_H
#define CTP_TRADE_API_H

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include "ThostFtdcTraderApi.h"

class CTPTradeApi : public QObject
{
	Q_OBJECT
public:

	CTPTradeApi();

	virtual~CTPTradeApi();

public:

	static bool initialize(CThostFtdcTraderApi* pTradeApi);

	static bool finalize();

	static int reqUserLogin(const int& reqId, const QString& brokerId, const QString& userId, const QString& password);

	static int reqUserLogout(const int& reqId, const QString& brokerId, const QString& userId);

	static int reqSettlementInfoConfirm(const int& reqId, const QString& brokerId, const QString& investorId);

	static int reqOrderInsert(const int& reqId, QString& orderRef, const QString& brokerId, const QString& userId, const QString& investorId, const QString& instrumentId, const char& priceType, 
		const char& timeCondition, const double& orderPrice, const int& volume, const char& direction, const char& offsetFlag, const char& hedgeFlag, const char& volumeCondition);

	static int reqOrderAction(const int& reqId, const QString& brokerId, const QString& userId, const QString& investorId, const QString& instrumentId, const QString& orderRef,
		const int& frontId, const int& sessionId);

	static int reqQryInstrument(const int& reqId, const QString& instrumentId, const QString& exchangeId, const QString& productId);

	static int reqQryInvestorPosition(const int& reqId, const QString& brokerId, const QString& investorId, const QString& instrumentId);

private:
	static CTPTradeApi* mThis;
	CThostFtdcTraderApi* m_pTradeApi;
	QString mUserId;
	int mUserLocalId;
	QMutex mOrderMutex;
};

#endif