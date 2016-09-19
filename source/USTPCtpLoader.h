#ifndef USTP_CTP_LOADER_H
#define USTP_CTP_LOADER_H

#include <QtCore/QObject>
#include "CTPMdSpi.h"
#include "SgitMdSpi.h"
#include "CTPTradeSpi.h"
#include "SgitTradeSpi.h"
#include "USTPBase64.h"

class USTPCtpLoader : public QObject
{
public:

	static bool initialize();

	static bool finalize();

	static bool start();

	static bool stop();

	static QString getUserABrokerId();

	static QString getUserBBrokerId();

	static QString getAMdPath();

	static QString getBMdPath();

	static QString getUserATradePath();

	static QString getUserBTradePath();

	static QString getDateTime();

	static QString getValidDateTime();

	static CThostFtdcMdApi* getAMdApi();

	static CSgitFtdcMdApi* getBMdApi();

	static CThostFtdcTraderApi* getUserATradeApi();

	static CSgitFtdcTraderApi* getUserBTradeApi();

	static CTPMdSpi* getAMdSpi();

	static SgitMdSpi* getBMdSpi();

	static CTPTradeSpi* getUserATradeSpi();

	static SgitTradeSpi* getUserBTradeSpi();

protected:

	USTPCtpLoader();

	virtual~ USTPCtpLoader();

private:

	static USTPCtpLoader *mThis;

private:

	bool loadXMLFile(const QString& path);

	bool openTradeLog();

	bool closeTradeLog();

	bool openProfileFile();

	bool closeProfileFile();

	bool startCtpMdThread();

	bool startSgitMdThread();

	bool startCtpTradeThread();

	bool startSgitTradeThread();
	
	QString mABrokerId;
	QString mATradePath;
	QString mAMdPath;

	QString mBBrokerId;
	QString mBTradePath; 	
	QString mBMdPath;

	QString mDateTime;
	QString mValidDateTime;
	CThostFtdcMdApi* m_pAMdApi;
	CSgitFtdcMdApi* m_pBMdApi;
	CThostFtdcTraderApi* m_pUserATradeApi;
	CSgitFtdcTraderApi* m_pUserBTradeApi;
	CTPMdSpi* m_pAMdSpi;
	SgitMdSpi* m_pBMdSpi;
	CTPTradeSpi* m_pUserATradeSpi;
	SgitTradeSpi* m_pUserBTradeSpi;
	USTPBase64* m_pBase64;
};

#endif
