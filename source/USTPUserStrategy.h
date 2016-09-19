#ifndef USTP_USER_STRATEGY_H
#define USTP_USER_STRATEGY_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QStringList>
#include <QtCore/QMap>

class USTPStrategyWidget;
class USTPOrderWidget;
class USTPCancelWidget;
class USTPSubmitWidget;

typedef struct
{
	char status;
	QString orderRef;
}OrderStatus;

class USTPStrategyBase : public QObject
{
	Q_OBJECT
public:
	USTPStrategyBase(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset, const char& hedge);
	virtual~USTPStrategyBase();

protected:
	virtual void createOrder(){};

	virtual void updateInitShow(){};

	virtual bool isCTPInMarket(const char& status);

	virtual bool isSgitInMarket(const char& status);

	virtual void orderSecondIns(bool isInit, const int& qty, const double& bidPrice, const double& askPrice){};

	virtual void submitCTPAction(const QString& insLabel, const QString& orderLocalId, const QString& instrument){};

	virtual void submitSgitAction(const QString& insLabel, const QString& orderLocalId, const QString& ordersysId, const QString& instrument){};

	virtual void submitCTPOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns){};
	
	virtual void submitSgitOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns){};

	virtual void orderCTPInsert(const int& reqId, const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns){};

	virtual void orderSgitInsert(const int& reqId, const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns){};


protected:
	virtual int getInsPrcision(const double& value);
protected:

	QThread mStrategyThread;
	QString mFirstIns;
	QString mSecIns;
	QString mOrderLabel;
	QString mUserCustomLabel;
	QString mABrokerId;
	QString mAUserId;
	QString mAInvestorId;
	QString mBBrokerId;
	QString mBUserId;
	QString mBInvestorId;

	QString mStrategyLabel;

	QString mFirstOrderRef;
	QString mSecOrderRef;
	QString mInsComplex;
	QString mFirstOrderSysId;
	QString mExh;

	double mOrderPrice;
	double mPriceTick;
	double mFirstMarketBasePrice;
	double mSecondMarketBasePrice;
	double mSecondOrderBasePrice;
	double mFirstOrderPrice;
	double mFirstBidMarketPrice;
	double mFirstAskMarketPrice;
	double mSecondBidMarketPrice;
	double mSecondAskMarketPrice;
	double mFirstOrderBasePrice;
	double mLimitSpread;

	double mSecInsUpperPrice;
	double mSecInsLowerPrice;

	int mRequestId;
	int mInsPrecision;
	int mOrderQty;
	int mCycleStall;
	int mOrderType;
	int mCurrentReferIndex;
	int mFirstRemainQty;
	int mFirstTradeQty;
	int mSecondRemainQty;
	int mSecondTradeQty;
	int mAFrontId;
	int mASessionId;

	int mBFrontId;
	int mBSessionId;


	int mFirstBidMarketVolume;
	int mFirstAskMarketVolume;

	char mBS;
	char mOffsetFlag;
	char mHedgeFlag;
	char mFirstInsStatus;
	char mSecInsStatus;
	bool mIsCycle;
	bool mIsCanMarket;
	bool mIsDeleted;
	QStringList mSecActionList;
	QMap<QString, OrderStatus> mSecOrderRefMap;
	QMap<QString, QString> mReqMap;
private:
};


class USTPConditionArbitrage : public USTPStrategyBase
{
	Q_OBJECT
public:
	USTPConditionArbitrage(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& ctpQty, const int& sgitQty, 
		const int& cycleNum, const char& bs,  const char& offset, const char& hedge, const double& firstMarketBidPrice, const double& firstMarketAskPrice,
		const double& secMarketBidPrice, const double& secMarketAskPrice, USTPOrderWidget* pOrderWidget,USTPCancelWidget* pCancelWidget, USTPStrategyWidget* pStrategyWidget);

	virtual~USTPConditionArbitrage();

signals:

	void onOrderFinished(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset, const char& hedge,
		bool isAllTraded, const int& orderType, const double& firstMarketBidPrice, const double& firstMarketAskPrice, const double& secMarketBidPrice, const double& secMarketAskPrice);

	void onUpdateOrderShow(const QString& userLocalId, const QString& instrument, const QString& orderStyle, const char& status, const char& bs, const double& price, const int&
		orderVolume, const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedge, const double& orderPrice);

	protected slots:
		void doCTPRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
			const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
			const double& lowestPrice, const int& volume);

		void doSgitRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
			const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
			const double& lowestPrice, const int& volume);

		void doCTPRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
			const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
			const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId);

		void doCTPErrRtnOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
			const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
			const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId);

		void doCTPErrRtnOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
			const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
			const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId);

		void doCTPRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
			const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
			const QString& localId, const QString& orderRef, const QString& tradeTime);


		void doSgitRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
			const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
			const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId);

		void doSgitRspOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
			const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
			const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId);

		void doSgitRspOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
			const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
			const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId);

		void onSgitRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
			const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
			const QString& localId, const QString& orderRef, const QString& tradeTime);

		void doDelOrder(const QString& orderStyle);

protected:
	void updateInitShow();

	void orderSecondIns(bool isInit, const int& qty, const double& bidPrice, const double& askPrice);

	virtual void submitCTPAction(const QString& insLabel, const QString& orderLocalId, const QString& instrument);

	virtual void submitSgitAction(const QString& insLabel, const QString& orderLocalId, const QString& ordersysId, const QString& instrument);

	virtual void submitCTPOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns);

	virtual void submitSgitOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns);

	virtual void orderCTPInsert(const int& reqId, const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns);

	virtual void orderSgitInsert(const int& reqId, const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns);

private:

	void cancelFirstIns();

	void initConnect(USTPStrategyWidget* pStrategyWidget, USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget);

	void noOpponentPriceOrder(const QString& instrument, const double& basePrice);

	void switchFirstInsOrder(const char& tCondition);

	void defineTimeOrder(const double& basePrice);

private:
	int mCtpOrderQty;
	int mSgitOrderQty;
	int mCycleNum;
};

class USTPSpeculateOrder : public USTPStrategyBase
{
	Q_OBJECT
public:

	USTPSpeculateOrder(const QString& orderLabel, const QString& speLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset,
		const char& hedge, USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget, USTPSubmitWidget* pSubmitWidget);

	virtual~USTPSpeculateOrder();

signals:

	void onOrderFinished(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset, const char& hedge,
		bool isAllTraded, const int& orderType, const double& firstMarketBidPrice, const double& firstMarketAskPrice, const double& secMarketBidPrice, const double& secMarketAskPrice);

	void onUpdateOrderShow(const QString& userLocalId, const QString& instrument, const QString& orderStyle, const char& status, const char& bs, const double& price, const int&
		orderVolume, const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedge, const double& orderPrice);

protected slots:
		void doSubmitOrder(const QString& orderLabel, const QString& speLabel, const QString& ins, const char& direction, const char& offsetFlag, const int& volume, 
			const double& orderPrice, bool isAOrder);

		void doCTPRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
			const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
			const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId);

		void doCTPErrRtnOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
			const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
			const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId);

		void doCTPErrRtnOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
			const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
			const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId);

		void doSgitRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
			const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
			const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId);

		void doSgitRspOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
			const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
			const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId);

		void doSgitRspOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
			const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
			const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId);

		void doDelOrder(const QString& orderStyle);

protected:
	void submitCTPAction(const QString& insLabel, const QString& orderLocalId, const QString& instrument);

	void submitSgitAction(const QString& insLabel, const QString& orderLocalId, const QString& ordersysId, const QString& instrument);

	void submitCTPOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns);
	
	void submitSgitOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns);


private:

	void initConnect(USTPSubmitWidget* pSubmitWidget, USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget);

private:

	QString mBrokerId;
	QString mUserId;
	QString mInvestorId;
	int mFrontId;
	int mSessionId;

	QString mTempBrokerId;
	QString mTempUserId;
	QString mTempInvestorId;
	int mTempFrontId;
	int mTempSessionId;

	QString mTempOrderLabel;
	char mTempOffsetFlag;
	char mTempBS;
	QString mTempFirstIns;
	int mTempOrderQty;
	double mTempOrderPrice;
	QString mSpeOrderLabel;
	bool mIsCtpOrder;
};


#endif