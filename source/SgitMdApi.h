#ifndef SGIT_MD_API_H
#define SGIT_MD_API_H

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include "SgitFtdcMdApi.h"

class SgitMdApi : public QObject
{
	Q_OBJECT
public:

	SgitMdApi();

	virtual~ SgitMdApi();

	static bool initilize(CSgitFtdcMdApi* pMdApi);

	static bool finalize();

	static int reqUserLogin(const QString& brokerId, const QString& userId, const QString& password);

	static int subMarketData(const QString& instrumentId);

private:
	static SgitMdApi* mThis;
	CSgitFtdcMdApi* m_pMdApi;
	int nRequestId;
};

#endif