#ifndef USTP_MUTEX_ID_H
#define USTP_MUTEX_ID_H

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QMap>

class USTPMutexId : public QObject
{
	Q_OBJECT
public:
	USTPMutexId();
	virtual ~USTPMutexId();

	static bool initialize();

	static bool setUserAInfo(const QString& userId, const QString& psw, int maxId, int frontId, int sessionId);

	static bool setUserBInfo(const QString& userId, const QString& psw, int maxId, int frontId, int sessionId);

	static int getOrderRef(const QString& userId);

	static int getMutexId();

	static int getNewOrderIndex();

	static QString getAUserId();

	static QString getALoginPsw();

	static int getFrontId(const QString& userId);
	
	static int getSessionId(const QString& userId);

	static QString getBUserId();

	static QString getBLoginPsw();

	static void setAInvestorId(const QString& investorId);

	static void setBInvestorId(const QString& investorId);

	static QString getAInvestorId();

	static QString getBInvestorId();

	static bool setInsBidPosition(const QString& userId, const QString& ins, const int& qty);

	static bool setInsAskPosition(const QString& userId, const QString& ins, const int& qty);

	static int getInsBidPosition(const QString& userId, const QString& ins);

	static int getInsAskPosition(const QString& userId, const QString& ins);

	static bool addBidPosition(const QString& userId, const QString& ins, const int& qty);

	static bool addAskPosition(const QString& userId, const QString& ins, const int& qty);

	static bool setInsPriceTick(const QString& ins, const QString& exchangeId, const double& tick, const int& volumeMultiple);

	static QString getInsExchangeId(const QString& ins);

	static int getInsMultiple(const QString& ins);

	static bool setInsMarketMaxVolume(const QString& ins, const int& volume);

	static double getInsPriceTick(const QString& ins);

	static bool getTotalBidPosition(const QString& userId, QMap<QString, int>& bidPostions);

	static bool getTotalAskPosition(const QString& userId, QMap<QString, int>& askPostions);

	static bool initActionNum(const QString& ins, const int& num);

	static bool updateActionNum(const QString& ins);

	static int getActionNum(const QString& ins);

	static bool getTotalActionNum(QMap<QString, int>& actionNums);
	
	static int getInsMarketMaxVolume(const QString& ins);

	static int getMarketIndex();

	static bool setUpperLowerPrice(const QString& ins, const double& upperPrice, const double& lowerPrice);

	static double getUpperPrice(const QString& ins);

	static double getLowerPrice(const QString& ins);

	static bool finalize();
protected:
private:
	static USTPMutexId* mThis;
	bool mIsBaseTime;
	int nAOrderId;
	int mAFrontId;
	int mASessionId;

	int nBOrderId;
	int mBFrontId;
	int mBSessionId;

	int nRequestIndex;
	int nNewOrderIndex;
	int nMarketIndex;

	QString mAUserId;
	QString mAPsw;
	QString mAInvestorId;

	QString mBUserId;
	QString mBPsw;
	QString mBInvestorId;

	QMap<QString, int> mABidQtys;
	QMap<QString, int> mAAskQtys;
	QMap<QString, int> mBBidQtys;
	QMap<QString, int> mBAskQtys;
	QMap<QString, double> mInsTicks;
	QMap<QString, QString> mInsExhs;
	QMap<QString, int> mMaxMarketQtys;
	QMap<QString, int> mInsMultiple;
	QMap<QString, int> mInsActionNums;
	QMap<QString, double> mUpperPrices;
	QMap<QString, double> mLowerPrices;
	QMutex mRequestMutex;
	QMutex mOrderRefMutex;
	QMutex mNewOrderMutex;
	QMutex mMarketMutex;
	QMutex mInsActionMutex;
};
#endif