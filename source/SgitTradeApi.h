#ifndef SGIT_TRADE_API_H
#define SGIT_TRADE_API_H

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include "SgitFtdcTraderApi.h"

class SgitTradeApi : public QObject
{
	Q_OBJECT
public:

	SgitTradeApi();

	virtual~SgitTradeApi();

public:

	static bool initialize(CSgitFtdcTraderApi* pTradeApi);

	static bool finalize();

	static int reqUserLogin(const int& reqId, const QString& brokerId, const QString& userId, const QString& password);

	static int reqUserLogout(const int& reqId, const QString& brokerId, const QString& userId);

	static int reqOrderInsert(const int& reqId, QString& orderRef, const QString& brokerId, const QString& userId, const QString& investorId, const QString& instrumentId, const char& priceType, 
		const char& timeCondition, const double& orderPrice, const int& volume, const char& direction, const char& offsetFlag, const char& hedgeFlag, const char& volumeCondition);

	static int reqOrderAction(const int& reqId, const QString& brokerId, const QString& userId, const QString& investorId, const QString& instrumentId, const QString& orderRef,
		const QString& orderSysId, const QString& exchangeId);

	static int reqQryInstrument(const int& reqId, const QString& instrumentId, const QString& exchangeId, const QString& productId);

	static int reqQryInvestorPosition(const int& reqId, const QString& brokerId, const QString& investorId, const QString& instrumentId);

private:
	static SgitTradeApi* mThis;
	CSgitFtdcTraderApi* m_pTradeApi;
	QString mUserId;
	int mUserLocalId;
	QMutex mOrderMutex;
};

#endif