#ifndef CTP_MD_API_H
#define CTP_MD_API_H

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include "ThostFtdcMdApi.h"

class CTPMdApi : public QObject
{
	Q_OBJECT
public:

	CTPMdApi();

	virtual~ CTPMdApi();

	static bool initilize(CThostFtdcMdApi* pMdApi);

	static bool finalize();

	static int reqUserLogin(const QString& brokerId, const QString& userId, const QString& password);

	static int subMarketData(const QString& instrumentId);

private:
	static CTPMdApi* mThis;
	CThostFtdcMdApi* m_pMdApi;
	int nRequestId;
};

#endif