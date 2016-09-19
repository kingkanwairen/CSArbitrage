#include "USTPUserStrategy.h"
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <Windows.h>
#include "USTPConfig.h"
#include "USTPLogger.h"
#include "USTPCtpLoader.h"
#include "USTPMutexId.h"
#include "CTPTradeApi.h"
#include "SgitTradeApi.h"
#include "USTPStrategyWidget.h"
#include "USTPSubmitWidget.h"


#define USTP_FTDC_OS_ORDER_SUBMIT 'O'
#define USTP_FTDC_OS_CANCEL_SUBMIT 'C'
#define USTP_FTDC_OS_ORDER_NO_ORDER 'N'
#define USTP_FTDC_OS_ORDER_ERROR 'E'

#define FIRST_INSTRUMENT tr("FirstInstrument")
#define SECOND_INSTRUMENT tr("SecondInstrument")
#define DEFINE_ORDER_TIME 10

USTPStrategyBase::USTPStrategyBase(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset, const char& hedge)
{	
	mOrderLabel = orderLabel;
	mOrderPrice = orderPriceTick;
	mOrderQty = qty;
	mFirstIns = firstIns;
	mSecIns = secIns;
	mOffsetFlag = offset;
	mHedgeFlag = hedge;
	mBS = bs;
	mIsDeleted = false;

	mABrokerId = USTPCtpLoader::getUserABrokerId();
	mAUserId = USTPMutexId::getAUserId();
	mAInvestorId = USTPMutexId::getAInvestorId();

	mBBrokerId = USTPCtpLoader::getUserBBrokerId();
	mBUserId = USTPMutexId::getBUserId();
	mBInvestorId = USTPMutexId::getBInvestorId();

	mAFrontId = USTPMutexId::getFrontId(mAUserId);
	mASessionId = USTPMutexId::getSessionId(mAUserId);

	mBFrontId = USTPMutexId::getFrontId(mBUserId);
	mBSessionId = USTPMutexId::getSessionId(mBUserId);

	mInsComplex = mFirstIns + tr("|") + mSecIns;
	mExh = USTPMutexId::getInsExchangeId(secIns);
	mPriceTick = USTPMutexId::getInsPriceTick(secIns);
	mInsPrecision = getInsPrcision(mPriceTick);
	mSecInsUpperPrice = USTPMutexId::getUpperPrice(secIns);
	mSecInsLowerPrice = USTPMutexId::getLowerPrice(secIns);
}

USTPStrategyBase::~USTPStrategyBase()
{

}

bool USTPStrategyBase::isCTPInMarket(const char& status)
{
	if (THOST_FTDC_OST_PartTradedQueueing == status || THOST_FTDC_OST_PartTradedNotQueueing == status ||
		THOST_FTDC_OST_NoTradeQueueing == status || THOST_FTDC_OST_NoTradeNotQueueing == status){
			return true;
	}
	return false;
}

bool USTPStrategyBase::isSgitInMarket(const char& status)
{
	if (Sgit_FTDC_OST_PartTradedQueueing == status || Sgit_FTDC_OST_PartTradedNotQueueing == status ||
		Sgit_FTDC_OST_NoTradeQueueing == status || Sgit_FTDC_OST_NoTradeNotQueueing == status){
			return true;
	}
	return false;
}

int USTPStrategyBase::getInsPrcision(const double& value)
{
	if (value >= VALUE_1){
		return 0;
	}else if(value * 10 >= VALUE_1){
		return 1;
	}else if(value * 100 >= VALUE_1){
		return 2;
	}else if(value * 1000 >= VALUE_1){
		return 3;
	}
	return 0;
}


USTPConditionArbitrage::USTPConditionArbitrage(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& ctpQty, const int& sgitQty, 
											   const int& cycleNum, const char& bs, const char& offset, const char& hedge, const double& firstMarketBidPrice, const double& firstMarketAskPrice,
											   const double& secMarketBidPrice, const double& secMarketAskPrice, USTPOrderWidget* pOrderWidget,USTPCancelWidget* pCancelWidget, USTPStrategyWidget* pStrategyWidget)
											   : USTPStrategyBase(orderLabel, firstIns, secIns, orderPriceTick, 0, bs, offset, hedge)
{	
	moveToThread(&mStrategyThread);
	mStrategyThread.start();
	mOrderType = 0;
	mRequestId = -1;
	mFirstTradeQty = 0;
	mFirstRemainQty = mOrderQty;
	mSecondTradeQty = 0;
	mFirstBidMarketVolume = 0;
	mFirstAskMarketVolume = 0;
	mCurrentReferIndex = 0;
	if(THOST_FTDC_D_Buy == bs){
		mFirstMarketBasePrice = firstMarketBidPrice;	
		mSecondMarketBasePrice = secMarketBidPrice;
	}else{
		mFirstMarketBasePrice = firstMarketAskPrice;	
		mSecondMarketBasePrice = secMarketAskPrice;
	}
	mFirstBidMarketPrice = firstMarketBidPrice;
	mFirstAskMarketPrice = firstMarketAskPrice;
	mSecondBidMarketPrice = secMarketBidPrice;
	mSecondAskMarketPrice = secMarketAskPrice;
	mFirstOrderPrice = 0.0;
	mSecondOrderBasePrice = 0.0;
	mFirstInsStatus = USTP_FTDC_OS_ORDER_NO_ORDER;
	mSecInsStatus = USTP_FTDC_OS_ORDER_NO_ORDER;

	mIsCanMarket = (USTPMutexId::getInsMarketMaxVolume(secIns) > 0) ? true : false;
	initConnect(pStrategyWidget, pOrderWidget, pCancelWidget);	
	updateInitShow();
}

USTPConditionArbitrage::~USTPConditionArbitrage()
{
	mStrategyThread.quit();
	mStrategyThread.wait();
}

void USTPConditionArbitrage::initConnect(USTPStrategyWidget* pStrategyWidget, USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget)
{
	connect(USTPCtpLoader::getAMdSpi(), SIGNAL(onCTPRtnDepthMarketData(const QString&, const double&, const double&, 
		const double&,const double&, const int&, const double&, const int&, const double&, const double&, const int&)), 
		this, SLOT(doCTPRtnDepthMarketData(const QString&, const double&, const double&, 
		const double&,const double&, const int&, const double&, const int&, const double&, const double&, const int&)), Qt::QueuedConnection);
	
	connect(USTPCtpLoader::getBMdSpi(), SIGNAL(onSgitRtnDepthMarketData(const QString&, const double&, const double&, 
		const double&,const double&, const int&, const double&, const int&, const double&, const double&, const int&)), 
		this, SLOT(doSgitRtnDepthMarketData(const QString&, const double&, const double&, 
		const double&,const double&, const int&, const double&, const int&, const double&, const double&, const int&)), Qt::QueuedConnection);


	//A账号信号
	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPRtnOrder(const QString&, const QString&, const QString&, const char&, const double&, const int&, const int&, const int&, const char&, const char&, const char&, const char&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const char&, const int&)),
		this, SLOT(doCTPRtnOrder(const QString&, const QString&, const QString&, const char&, const double&, const int&, const int&, const int&, const char&, const char&, const char&, const char&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const char&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPErrRtnOrderInsert(const QString&, const QString&, const char&, const QString&, const char&, const QString&, const QString&, const char&, const char&, const char&,
		const QString&, const double&, const int&, const int&, const QString&, const int&)),
		this, SLOT(doCTPErrRtnOrderInsert(const QString&, const QString&, const char&, const QString&, const char&, const QString&, const QString&, const char&, const char&, const char&,
		const QString&, const double&, const int&, const int&, const QString&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPErrRtnOrderAction(const char&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const double&, const int&, const int&, const QString&, const int&)),
		this, SLOT(doCTPErrRtnOrderAction(const char&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const double&, const int&, const int&, const QString&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPRtnTrade(const QString&, const QString&, const char&, const int&, const double&,
		const char&, const char&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)),
		this, SLOT(doCTPRtnTrade(const QString&, const QString&, const char&, const int&, const double&,
		const char&, const char&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)), Qt::QueuedConnection);

	//B账号信号
	connect(USTPCtpLoader::getUserBTradeSpi(), SIGNAL(onSgitRtnOrder(const QString&, const QString&, const QString&, const char&, const double&, const int&, const int&, const int&, const char&, const char&, const char&, const char&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const char&, const int&)),
		this, SLOT(doSgitRtnOrder(const QString&, const QString&, const QString&, const char&, const double&, const int&, const int&, const int&, const char&, const char&, const char&, const char&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const char&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserBTradeSpi(), SIGNAL(onSgitRspOrderInsert(const QString&, const QString&, const char&, const QString&, const char&, const QString&, const QString&, const char&, const char&, const char&,
		const QString&, const double&, const int&, const int&, const QString&, const int&)),
		this, SLOT(doSgitRspOrderInsert(const QString&, const QString&, const char&, const QString&, const char&, const QString&, const QString&, const char&, const char&, const char&,
		const QString&, const double&, const int&, const int&, const QString&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserBTradeSpi(), SIGNAL(onSgitRspOrderAction(const char&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const double&, const int&, const int&, const QString&, const int&)),
		this, SLOT(doSgitRspOrderAction(const char&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const double&, const int&, const int&, const QString&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onSgitRtnTrade(const QString&, const QString&, const char&, const int&, const double&,
		const char&, const char&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)),
		this, SLOT(doSgitRtnTrade(const QString&, const QString&, const char&, const int&, const double&,
		const char&, const char&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)), Qt::QueuedConnection);



	connect(this, SIGNAL(onUpdateOrderShow(const QString&, const QString&, const QString&, const char&, const char&, const double& , const int&, const int&, const int&, const char&, const char&, const char&, const double&)), 
		pOrderWidget, SLOT(doUpdateOrderShow(const QString&, const QString&, const QString&, const char&, const char&, const double& , const int&, const int&, const int&, const char&, const char&, const char&, const double&)), Qt::QueuedConnection);

	connect(this, SIGNAL(onUpdateOrderShow(const QString&, const QString&, const QString&, const char&, const char&, const double& , const int&, const int&, const int&, const char&, const char&, const char&, const double&)), 
		pCancelWidget, SLOT(doUpdateOrderShow(const QString&, const QString&, const QString&, const char&, const char&, const double& , const int&, const int&, const int&, const char&, const char&, const char&, const double&)), Qt::QueuedConnection);

	connect(pCancelWidget, SIGNAL(onDelOrder(const QString& )), this, SLOT(doDelOrder(const QString& )), Qt::QueuedConnection);

	connect(this, SIGNAL(onOrderFinished(const QString&, const QString&, const QString&, const double&, const int&, const char&,  const char&, const char&, bool, const int&, const double&, const double&, const double&, const double&)),
		pStrategyWidget, SLOT(doOrderFinished(const QString&, const QString&, const QString&, const double&, const int&, const char&,  const char&, const char&, bool, const int&, const double&, const double&, const double&, const double&)), Qt::QueuedConnection);

	connect(this, SIGNAL(onOrderFinished(const QString&, const QString&, const QString&, const double&, const int&, const char&,  const char&, const char&, bool, const int&, const double&, const double&, const double&, const double&)), pCancelWidget,
		SLOT(doOrderFinished(const QString&, const QString&, const QString&, const double&, const int&, const char&,  const char&, const char&, bool, const int&, const double&, const double&, const double&, const double&)), Qt::QueuedConnection);
}

void USTPConditionArbitrage::updateInitShow()
{	
	if(mIsDeleted)	//如果手工撤单，则不更新
		return;
	mFirstInsStatus = USTP_FTDC_OS_ORDER_NO_ORDER;
	mRequestId = USTPMutexId::getMutexId();
	QString szReqId = QString::number(mRequestId);
	emit onUpdateOrderShow(szReqId, mFirstIns, mOrderLabel, 'N', mBS, 0.0, mFirstRemainQty, mFirstRemainQty, mFirstTradeQty, mOffsetFlag, THOST_FTDC_OPT_LimitPrice, mHedgeFlag, mOrderPrice);
}

void USTPConditionArbitrage::doCTPRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
													  const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
													  const double& lowestPrice, const int& volume)
{	
	if (mFirstIns != instrumentId)	//监听第一，二腿行情
		return;
	if(THOST_FTDC_D_Buy == mBS)
		mFirstMarketBasePrice = bidPrice;
	else
		mFirstMarketBasePrice = askPrice;

	mFirstBidMarketPrice = bidPrice;
	mFirstAskMarketPrice = askPrice;
	mFirstBidMarketVolume = bidVolume;
	mFirstAskMarketVolume = askVolume;
	if((mFirstMarketBasePrice < VALUE_ACCURACY) || (mSecondMarketBasePrice < VALUE_ACCURACY) || (mFirstBidMarketPrice < VALUE_ACCURACY) || (mFirstAskMarketPrice < VALUE_ACCURACY))	
		return;
	if((mFirstMarketBasePrice > INIT_VALUE) || (mSecondMarketBasePrice > INIT_VALUE) || (mFirstBidMarketPrice > INIT_VALUE) || (mFirstAskMarketPrice > INIT_VALUE))
		return;

	double fSecBasePrice = mSecondMarketBasePrice + mOrderPrice;
	noOpponentPriceOrder(instrumentId, fSecBasePrice);
		
}

void USTPConditionArbitrage::doSgitRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
													 const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
													 const double& lowestPrice, const int& volume)
{	
	if (mSecIns != instrumentId)	//监听第一，二腿行情
		return;
	if(THOST_FTDC_D_Buy == mBS)
		mSecondMarketBasePrice = bidPrice;
	else
		mSecondMarketBasePrice = askPrice;
	mSecondBidMarketPrice = bidPrice;
	mSecondAskMarketPrice = askPrice;
	if((mFirstMarketBasePrice < VALUE_ACCURACY) || (mSecondMarketBasePrice < VALUE_ACCURACY) || (mFirstBidMarketPrice < VALUE_ACCURACY) || (mFirstAskMarketPrice < VALUE_ACCURACY))	
		return;
	if((mFirstMarketBasePrice > INIT_VALUE) || (mSecondMarketBasePrice > INIT_VALUE) || (mFirstBidMarketPrice > INIT_VALUE) || (mFirstAskMarketPrice > INIT_VALUE))
		return;

	double fSecBasePrice = mSecondMarketBasePrice + mOrderPrice;
	noOpponentPriceOrder(instrumentId, fSecBasePrice);

}

void USTPConditionArbitrage::noOpponentPriceOrder(const QString& instrument, const double& basePrice)
{
	if(((USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus) || (THOST_FTDC_OST_Canceled == mFirstInsStatus))){//第一腿没有下单或者已撤单

		if(THOST_FTDC_D_Buy == mBS && mFirstMarketBasePrice <= basePrice){	//条件单买委托满足下单条件
			mFirstOrderPrice = mFirstMarketBasePrice + mPriceTick;
			if(mFirstOrderPrice > basePrice)
				mFirstOrderPrice = basePrice;
			mSecondOrderBasePrice = mSecondMarketBasePrice;
			switchFirstInsOrder(THOST_FTDC_TC_GFD);

		}else if(THOST_FTDC_D_Sell == mBS && mFirstMarketBasePrice >= basePrice){//条件单卖委托满足下单条件
			mFirstOrderPrice = mFirstMarketBasePrice - mPriceTick;
			if(mFirstOrderPrice < basePrice)
				mFirstOrderPrice = basePrice;
			mSecondOrderBasePrice = mSecondMarketBasePrice;
			switchFirstInsOrder(THOST_FTDC_TC_GFD);

		}else{
			if(THOST_FTDC_OST_Canceled == mFirstInsStatus){
				updateInitShow();
			}
		}
	}else if(USTPStrategyBase::isCTPInMarket(mFirstInsStatus)){//1.非设定时间撤单的情况，第一腿委托成功，第一腿行情发生变化，腿一撤单重发;2.设定时间撤单的情况，定时超时，根据行情触发。

//#ifdef _DEBUG
//		QString data = tr("[") + mOrderLabel + tr("-RealData]   Instrument: ") + instrument +  QString(tr("  FirstOrderPrice: ")) + QString::number(mFirstOrderPrice) + QString(tr("  FirstMarketBasePrice: ")) +
//			QString::number(mFirstMarketBasePrice) + QString(tr("  BasePrice: ")) + QString::number(basePrice);
//		USTPLogger::saveData(data);
//#endif

		if((THOST_FTDC_D_Buy == mBS && (mFirstMarketBasePrice > mFirstOrderPrice + VALUE_ACCURACY)) || (THOST_FTDC_D_Sell == mBS && (mFirstMarketBasePrice < mFirstOrderPrice - VALUE_ACCURACY))){
			cancelFirstIns();
		}
	}
}

void USTPConditionArbitrage::defineTimeOrder(const double& basePrice)
{
	if(((USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus) || (THOST_FTDC_OST_Canceled == mFirstInsStatus))){//第一腿没有下单或者已撤单
		if(THOST_FTDC_D_Buy == mBS && mFirstMarketBasePrice <= basePrice){	//条件单买委托满足下单条件
			mFirstOrderPrice = mFirstMarketBasePrice + mPriceTick;
			if(mFirstOrderPrice > basePrice)
				mFirstOrderPrice = basePrice;
			mSecondOrderBasePrice = mSecondMarketBasePrice;
			switchFirstInsOrder(THOST_FTDC_TC_GFD);

		}else if(THOST_FTDC_D_Sell == mBS && mFirstMarketBasePrice >= basePrice){//条件单卖委托满足下单条件
			mFirstOrderPrice = mFirstMarketBasePrice - mPriceTick;
			if(mFirstOrderPrice < basePrice)
				mFirstOrderPrice = basePrice;
			mSecondOrderBasePrice = mSecondMarketBasePrice;
			switchFirstInsOrder(THOST_FTDC_TC_GFD);
		}else{
			if(THOST_FTDC_OST_Canceled == mFirstInsStatus){		
				updateInitShow();
			}
		}
	}
}

void USTPConditionArbitrage::switchFirstInsOrder(const char& tCondition)
{
	if(USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus){
		if(mIsDeleted)
			return;
		orderCTPInsert(mRequestId, FIRST_INSTRUMENT, mFirstIns, mFirstOrderPrice, mBS, mFirstRemainQty, THOST_FTDC_OPT_LimitPrice, tCondition, true);
	}else
		submitCTPOrder(FIRST_INSTRUMENT, mFirstIns, mFirstOrderPrice, mBS, mFirstRemainQty, THOST_FTDC_OPT_LimitPrice, tCondition, true);
}

void USTPConditionArbitrage::submitCTPOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns)
{	
	if(isFirstIns && mIsDeleted)	//撤掉报单，合约一禁止下新单
		return;
	mRequestId = USTPMutexId::getMutexId();
	orderCTPInsert(mRequestId, insLabel, instrument, orderPrice, direction, qty, priceType, timeCondition, isFirstIns);	
}

void USTPConditionArbitrage::submitSgitOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns)
{	
	if(isFirstIns && mIsDeleted)
		return;
	mRequestId = USTPMutexId::getMutexId();
	orderSgitInsert(mRequestId, insLabel, instrument, orderPrice, direction, qty, priceType, timeCondition, isFirstIns);	
}

void USTPConditionArbitrage::orderCTPInsert(const int& reqId, const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns)
{	
	char offsetFlag = mOffsetFlag;
	double adjustPrice = (priceType == THOST_FTDC_OPT_LimitPrice) ? orderPrice : 0.0;
	QString orderRef;
	mFirstInsStatus = USTP_FTDC_OS_ORDER_SUBMIT;
	CTPTradeApi::reqOrderInsert(reqId, orderRef, mABrokerId, mAUserId, mAInvestorId, instrument, priceType, timeCondition, adjustPrice, qty, direction, offsetFlag, mHedgeFlag, THOST_FTDC_VC_AV);
	mFirstOrderRef = orderRef;
	mCurrentReferIndex = 0;
	QString szReqId = QString::number(reqId);
	mReqMap.insert(orderRef, szReqId);
	emit onUpdateOrderShow(szReqId, instrument, mOrderLabel, 'N', direction, 0.0, qty, qty, 0, offsetFlag, priceType, mHedgeFlag, mOrderPrice);	

#ifdef _DEBUG
	QString accountId = (instrument == mFirstIns) ? mAUserId : mBUserId;
	QString data = mOrderLabel + QString(tr("  [CTP| ")) + insLabel + QString(tr("-OrderInsert]   Instrument: ")) + instrument +  QString(tr("  RequestId: ")) + szReqId + QString(tr("  OrderRef: ")) + orderRef + 
		QString(tr("  UserId: ")) + accountId + QString(tr("  PriceType: ")) + QString(priceType) + QString(tr("  OrderPrice: ")) + QString::number(adjustPrice) + QString(tr("  OrderVolume: ")) + 
		QString::number(qty) + QString(tr("  Direction: ")) + QString(direction) + QString(tr("  OffsetFlag: ")) + QString(offsetFlag);
	USTPLogger::saveData(data);
#endif	
}

void USTPConditionArbitrage::orderSgitInsert(const int& reqId, const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns)
{	
	char offsetFlag = mOffsetFlag;
	double adjustPrice = (priceType == Sgit_FTDC_OPT_LimitPrice) ? orderPrice : 0.0;
	QString orderRef;
	OrderStatus state;
	state.status = USTP_FTDC_OS_ORDER_SUBMIT;
	if(Sgit_FTDC_OF_Open == mOffsetFlag)
		offsetFlag = Sgit_FTDC_OF_CloseToday;
	else
		offsetFlag = Sgit_FTDC_OF_Open;
	SgitTradeApi::reqOrderInsert(reqId, orderRef, mBBrokerId, mBUserId, mBInvestorId, instrument, priceType, timeCondition, adjustPrice, qty, direction, offsetFlag, mHedgeFlag, Sgit_FTDC_VC_AV);
	state.orderRef = orderRef;
	mSecOrderRefMap.insert(orderRef, state);
	QString szReqId = QString::number(reqId);
	mReqMap.insert(orderRef, szReqId);
	emit onUpdateOrderShow(szReqId, instrument, mOrderLabel, 'N', direction, 0.0, qty, qty, 0, offsetFlag, priceType, mHedgeFlag, mOrderPrice);	

#ifdef _DEBUG
	QString accountId = (instrument == mFirstIns) ? mAUserId : mBUserId;
	QString data = mOrderLabel + QString(tr("  [")) + insLabel + QString(tr("-OrderInsert]   Instrument: ")) + instrument +  QString(tr("  RequestId: ")) + szReqId + QString(tr("  OrderRef: ")) + orderRef + 
		QString(tr("  UserId: ")) + accountId + QString(tr("  PriceType: ")) + QString(priceType) + QString(tr("  OrderPrice: ")) + QString::number(adjustPrice) + QString(tr("  OrderVolume: ")) + 
		QString::number(qty) + QString(tr("  Direction: ")) + QString(direction) + QString(tr("  OffsetFlag: ")) + QString(offsetFlag);
	USTPLogger::saveData(data);
#endif	
}

void USTPConditionArbitrage::doCTPRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
											const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
											const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId)
{	
	if(mReqMap.find(orderRef) == mReqMap.end())
		return;
	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, orderStatus, direction, orderPrice, orderVolume, remainVolume, tradeVolume, offsetFlag, priceType, hedgeFlag, mOrderPrice);
	if(mFirstOrderRef == orderRef){
		mFirstInsStatus = orderStatus;
		mFirstRemainQty = remainVolume;

		if (THOST_FTDC_OST_Canceled == orderStatus){
			double fSecBasePrice = mSecondMarketBasePrice + mOrderPrice;
			defineTimeOrder(fSecBasePrice);
		}
		
		if (THOST_FTDC_OST_Canceled == orderStatus && mIsDeleted){
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, false, mOrderType, mFirstBidMarketPrice, mFirstAskMarketPrice,
				mSecondBidMarketPrice, mSecondAskMarketPrice);
		}
		if(THOST_FTDC_OST_Canceled == orderStatus)
			USTPMutexId::updateActionNum(mInsComplex);
	}
}


void USTPConditionArbitrage::doCTPRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
											const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
											const QString& localId, const QString& orderRef, const QString& tradeTime)
{
	if(mFirstOrderRef == orderRef && mAUserId == investorId){
		mFirstTradeQty += tradeVolume;
		orderSecondIns(true, tradeVolume, 0.0, 0.0);
	}
	if(mFirstTradeQty >= mOrderQty && mSecondTradeQty >= mOrderQty){
		emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, true, mOrderType, mFirstBidMarketPrice, mFirstAskMarketPrice, 
			mSecondBidMarketPrice, mSecondAskMarketPrice);
	}
}

void USTPConditionArbitrage::orderSecondIns(bool isInit, const int& qty, const double& bidPrice, const double& askPrice)
{	
	if (mIsCanMarket){
		if (Sgit_FTDC_D_Buy == mBS)
			submitSgitOrder(SECOND_INSTRUMENT, mSecIns, 0.0, Sgit_FTDC_D_Sell, qty, Sgit_FTDC_OPT_AnyPrice, Sgit_FTDC_TC_IOC, false);
		else
			submitSgitOrder(SECOND_INSTRUMENT, mSecIns, 0.0, Sgit_FTDC_D_Buy, qty, Sgit_FTDC_OPT_AnyPrice, Sgit_FTDC_TC_IOC, false);
	}else{
		if(Sgit_FTDC_D_Buy == mBS)
			submitSgitOrder(SECOND_INSTRUMENT, mSecIns, mSecInsLowerPrice, Sgit_FTDC_D_Sell, qty, Sgit_FTDC_OPT_LimitPrice, Sgit_FTDC_TC_GFD, false);
		else
			submitSgitOrder(SECOND_INSTRUMENT, mSecIns, mSecInsUpperPrice, Sgit_FTDC_D_Buy, qty, Sgit_FTDC_OPT_LimitPrice, Sgit_FTDC_TC_GFD, false);
	}
}

void USTPConditionArbitrage::doCTPErrRtnOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
													 const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
													 const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId)
{	
	if(mReqMap.find(orderRef) == mReqMap.end())
		return;
	mFirstInsStatus = USTP_FTDC_OS_ORDER_ERROR;	
	switch (errorId){
	case 22:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'D', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice); //重复的报单
		break;
	case 31:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'Z', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//	资金不足
		break;
	case 42:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'S', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//	结算结果未确认
		break;
	case 50:
	case 51:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'P', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//平仓位不足
		break;
	default:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'W', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);
		break;
	}
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [CTP| ErrRtnOrderInsert] orderRef: ")) + orderRef + QString(tr("  RequestId: ")) + mReqMap[orderRef] + QString(tr("  InstrumentId: ")) + instrumentId + 
		QString(tr("  ErrorId: ")) + QString::number(errorId) + QString(tr("  ErrorMsg: ")) + errorMsg;
	USTPLogger::saveData(data);
#endif
}

void USTPConditionArbitrage::doCTPErrRtnOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
													 const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
													 const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId)
{
#ifdef _DEBUG
	if(mReqMap.find(orderActionRef) == mReqMap.end())
		return;
	QString data = mOrderLabel + QString(tr("  [CTP| ErrRtnOrderAction] orderSysId: ")) + orderSysId + 
		QString(tr("  UserActionLocalId: ")) + userActionLocalId  + QString(tr("  ErrorId: ")) + QString::number(errorId) + QString(tr("  ErrorMsg: ")) + errorMsg;
	USTPLogger::saveData(data);
#endif
}


void USTPConditionArbitrage::cancelFirstIns()
{
	mFirstInsStatus = USTP_FTDC_OS_CANCEL_SUBMIT;
	submitCTPAction(FIRST_INSTRUMENT, mFirstOrderRef, mFirstIns);
}


void USTPConditionArbitrage::doDelOrder(const QString& orderStyle)
{
	if(orderStyle != mOrderLabel)
		return;
	mIsDeleted = true;
	QStringList items = orderStyle.split("|");
	QString severType = items.at(0);
	if(severType == CTP_SERVER){
		if(isCTPInMarket(mFirstInsStatus)){		
			mFirstInsStatus = USTP_FTDC_OS_CANCEL_SUBMIT;
			submitCTPAction(FIRST_INSTRUMENT, mFirstOrderRef, mFirstIns);
		}else if(USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus || USTP_FTDC_OS_ORDER_ERROR == mFirstInsStatus || THOST_FTDC_OST_Canceled == mFirstInsStatus){
			emit onUpdateOrderShow(QString::number(mRequestId), mFirstIns, mOrderLabel, THOST_FTDC_OST_Canceled, mBS, 0.0, mOrderQty, mOrderQty, 0, mOffsetFlag, THOST_FTDC_OPT_LimitPrice, mHedgeFlag, mOrderPrice);
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice,  mOrderQty, mBS, mOffsetFlag, mHedgeFlag, false, mOrderType, mFirstBidMarketPrice, mFirstAskMarketPrice,
				mSecondBidMarketPrice, mSecondAskMarketPrice);
		}
		QString data = mOrderLabel + QString(tr("  [CTP| DoDelOrder]   mFirstInsStatus: ")) + QString(mFirstInsStatus);
		USTPLogger::saveData(data);
	}else{
		if(isSgitInMarket(mFirstInsStatus)){
			mFirstInsStatus = USTP_FTDC_OS_CANCEL_SUBMIT;
			submitSgitAction(FIRST_INSTRUMENT, mFirstOrderRef, mFirstOrderSysId, mFirstIns);
		}else if(USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus || USTP_FTDC_OS_ORDER_ERROR == mFirstInsStatus || Sgit_FTDC_OST_Canceled == mFirstInsStatus){
			emit onUpdateOrderShow(QString::number(mRequestId), mFirstIns, mOrderLabel, Sgit_FTDC_OST_Canceled, mBS, 0.0, mOrderQty, mOrderQty, 0, mOffsetFlag, Sgit_FTDC_OPT_LimitPrice, mHedgeFlag, mOrderPrice);
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, false, mOrderType, mFirstBidMarketPrice, mFirstAskMarketPrice,
				mSecondBidMarketPrice, mSecondAskMarketPrice);
		}
		QString data = mOrderLabel + QString(tr("  [SGIT| DoDelOrder]   mFirstInsStatus: ")) + QString(mFirstInsStatus);
		USTPLogger::saveData(data);
	}
}


void USTPConditionArbitrage::submitCTPAction(const QString& insLabel, const QString& orderLocalId, const QString& instrument)
{	
	CTPTradeApi::reqOrderAction(USTPMutexId::getMutexId(), mABrokerId, mAUserId, mAInvestorId, instrument, orderLocalId, mAFrontId, mASessionId);
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [CTP| ")) + insLabel + QString(tr("-OrderAction]   OrderRef: ")) + orderLocalId + QString(tr("  InstrumentId: ")) + instrument;
	USTPLogger::saveData(data);
#endif	
}

void USTPConditionArbitrage::submitSgitAction(const QString& insLabel, const QString& orderLocalId, const QString& ordersysId, const QString& instrument)
{	
	SgitTradeApi::reqOrderAction(USTPMutexId::getMutexId(), mBBrokerId, mBUserId, mBInvestorId, instrument, orderLocalId, ordersysId, USTPMutexId::getInsExchangeId(instrument));
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [SGIT| ")) + insLabel + QString(tr("-OrderAction]   OrderRef: ")) + orderLocalId + QString(tr("  InstrumentId: ")) + instrument;
	USTPLogger::saveData(data);
#endif	
}

void USTPConditionArbitrage::doSgitRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
										const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
										const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId)
{
	if(mReqMap.find(orderRef) == mReqMap.end())
		return;
	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, orderStatus, direction, orderPrice, orderVolume, remainVolume, tradeVolume, offsetFlag, priceType, hedgeFlag, mOrderPrice);
}

void USTPConditionArbitrage::doSgitRspOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
											  const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
											  const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId)
{
	if(mReqMap.find(orderRef) == mReqMap.end())
		return;

	//if(mFirstIns == instrumentId)
	//	mFirstInsStatus = USTP_FTDC_OS_ORDER_ERROR;

	//switch (errorId){
	//case 22:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'D', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice); //重复的报单
	//	break;
	//case 31:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'Z', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//	资金不足
	//	break;
	//case 42:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'S', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//	结算结果未确认
	//	break;
	//case 50:
	//case 51:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'P', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//平仓位不足
	//	break;
	//default:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'W', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);
	//	break;
	//}
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [SGIT| RspOrderInsert] orderRef: ")) + orderRef + QString(tr("  RequestId: ")) + mReqMap[orderRef] + QString(tr("  InstrumentId: ")) + instrumentId + 
		QString(tr("  ErrorId: ")) + QString::number(errorId) + QString(tr("  ErrorMsg: ")) + errorMsg;
	USTPLogger::saveData(data);
#endif
}

void USTPConditionArbitrage::doSgitRspOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
											  const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
											  const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId)
{
#ifdef _DEBUG
	if(mReqMap.find(orderActionRef) == mReqMap.end())
		return;
	QString data = mOrderLabel + QString(tr("  [SGIT| RspOrderAction] orderSysId: ")) + orderSysId + 
		QString(tr("  UserActionLocalId: ")) + userActionLocalId  + QString(tr("  ErrorId: ")) + QString::number(errorId) + QString(tr("  ErrorMsg: ")) + errorMsg;
	USTPLogger::saveData(data);
#endif
}

void USTPConditionArbitrage::onSgitRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
					const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
					const QString& localId, const QString& orderRef, const QString& tradeTime)
{
	if (mSecOrderRefMap.contains(orderRef)){
		mSecondTradeQty += tradeVolume;
	}
	if(mFirstTradeQty >= mOrderQty && mSecondTradeQty >= mOrderQty){
		emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, true, mOrderType, mFirstBidMarketPrice, mFirstAskMarketPrice, 
			mSecondBidMarketPrice, mSecondAskMarketPrice);
	}
}



USTPSpeculateOrder::USTPSpeculateOrder(const QString& orderLabel, const QString& speLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset,
									   const char& hedge, USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget, USTPSubmitWidget* pSubmitWidget)
									   :USTPStrategyBase(orderLabel, firstIns, secIns, orderPriceTick, qty, bs, offset, hedge)
{		
	moveToThread(&mStrategyThread);
	mStrategyThread.start();
	mOrderType = 1;
	mRequestId = -1;
	mTempOrderQty = 0;
	mTempOrderPrice = 0.0;
	mSpeOrderLabel = speLabel;
	mFirstInsStatus = USTP_FTDC_OS_ORDER_NO_ORDER;
	QStringList items = speLabel.split("|");
	QString severType = items.at(0);
	if(severType == CTP_SERVER)
		mIsCtpOrder = true;
	else
		mIsCtpOrder = false;
	initConnect(pSubmitWidget, pOrderWidget, pCancelWidget);
}

USTPSpeculateOrder::~USTPSpeculateOrder()
{
	mStrategyThread.quit();
	mStrategyThread.wait();
}

void USTPSpeculateOrder::initConnect(USTPSubmitWidget* pSubmitWidget, USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget)
{
	connect(pSubmitWidget, SIGNAL(onSubmitOrder(const QString&, const QString&, const QString&, const char&, const char&, const int&, const double&, bool)), 
		this, SLOT(doSubmitOrder(const QString&, const QString&, const QString&, const char&, const char&, const int&, const double&, bool)), Qt::QueuedConnection);

	//A账号信号
	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPRtnOrder(const QString&, const QString&, const QString&, const char&, const double&, const int&, const int&, const int&, const char&, const char&, const char&, const char&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const char&, const int&)),
		this, SLOT(doCTPRtnOrder(const QString&, const QString&, const QString&, const char&, const double&, const int&, const int&, const int&, const char&, const char&, const char&, const char&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const char&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPErrRtnOrderInsert(const QString&, const QString&, const char&, const QString&, const char&, const QString&, const QString&, const char&, const char&, const char&,
		const QString&, const double&, const int&, const int&, const QString&, const int&)),
		this, SLOT(doCTPErrRtnOrderInsert(const QString&, const QString&, const char&, const QString&, const char&, const QString&, const QString&, const char&, const char&, const char&,
		const QString&, const double&, const int&, const int&, const QString&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPErrRtnOrderAction(const char&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const double&, const int&, const int&, const QString&, const int&)),
		this, SLOT(doCTPErrRtnOrderAction(const char&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const double&, const int&, const int&, const QString&, const int&)), Qt::QueuedConnection);

	//B账号信号
	connect(USTPCtpLoader::getUserBTradeSpi(), SIGNAL(onSgitRtnOrder(const QString&, const QString&, const QString&, const char&, const double&, const int&, const int&, const int&, const char&, const char&, const char&, const char&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const char&, const int&)),
		this, SLOT(doSgitRtnOrder(const QString&, const QString&, const QString&, const char&, const double&, const int&, const int&, const int&, const char&, const char&, const char&, const char&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const char&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserBTradeSpi(), SIGNAL(onSgitRspOrderInsert(const QString&, const QString&, const char&, const QString&, const char&, const QString&, const QString&, const char&, const char&, const char&,
		const QString&, const double&, const int&, const int&, const QString&, const int&)),
		this, SLOT(doSgitRspOrderInsert(const QString&, const QString&, const char&, const QString&, const char&, const QString&, const QString&, const char&, const char&, const char&,
		const QString&, const double&, const int&, const int&, const QString&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getUserBTradeSpi(), SIGNAL(onSgitRspOrderAction(const char&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const double&, const int&, const int&, const QString&, const int&)),
		this, SLOT(doSgitRspOrderAction(const char&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const double&, const int&, const int&, const QString&, const int&)), Qt::QueuedConnection);


	connect(this, SIGNAL(onUpdateOrderShow(const QString&, const QString&, const QString&, const char&, const char&, const double& , const int&, const int&, const int&, const char&, const char&, const char&, const double&)), 
		pOrderWidget, SLOT(doUpdateOrderShow(const QString&, const QString&, const QString&, const char&, const char&, const double& , const int&, const int&, const int&, const char&, const char&, const char&, const double&)), Qt::QueuedConnection);

	connect(this, SIGNAL(onUpdateOrderShow(const QString&, const QString&, const QString&, const char&, const char&, const double& , const int&, const int&, const int&, const char&, const char&, const char&, const double&)), 
		pCancelWidget, SLOT(doUpdateOrderShow(const QString&, const QString&, const QString&, const char&, const char&, const double& , const int&, const int&, const int&, const char&, const char&, const char&, const double&)), Qt::QueuedConnection);

	connect(pCancelWidget, SIGNAL(onDelOrder(const QString& )), this, SLOT(doDelOrder(const QString& )), Qt::QueuedConnection);

	connect(this, SIGNAL(onOrderFinished(const QString&, const QString&, const QString&, const double&, const int&, const char&,  const char&, const char&, bool, const int&, const double&, const double&, const double&, const double&)),
		pCancelWidget, SLOT(doOrderFinished(const QString&, const QString&, const QString&, const double&, const int&, const char&,  const char&, const char&, bool, const int&, const double&, const double&, const double&, const double&)), Qt::QueuedConnection);
}

void USTPSpeculateOrder::doSubmitOrder(const QString& orderLabel, const QString& speLabel, const QString& ins, const char& direction, const char& offsetFlag, const int& volume, 
									   const double& orderPrice, bool isAOrder)
{	
	if(mSpeOrderLabel != speLabel)
		return;

	if(mIsCtpOrder){
		if(USTPStrategyBase::isCTPInMarket(mFirstInsStatus)){
			mTempOrderLabel = orderLabel;
			mTempOffsetFlag = offsetFlag;
			mTempBS = direction;
			mTempFirstIns = ins;
			mTempOrderQty = volume;
			mTempOrderPrice = orderPrice;
			mTempBrokerId = mABrokerId;
			mTempUserId = mAUserId;
			mTempInvestorId = mAInvestorId;
			mTempFrontId = mAFrontId;
			mTempSessionId = mASessionId;
			mFirstInsStatus = USTP_FTDC_OS_CANCEL_SUBMIT;
			submitCTPAction(FIRST_INSTRUMENT, mFirstOrderRef, mFirstIns);
		}else if(USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus || THOST_FTDC_OST_Canceled == mFirstInsStatus){
			mOrderLabel = orderLabel;
			mOffsetFlag = offsetFlag;
			mBS = direction;
			mFirstIns = ins;
			mOrderQty = volume;
			mOrderPrice = orderPrice;
			mFirstRemainQty = volume;
			mBrokerId = mABrokerId;
			mUserId = mAUserId;
			mInvestorId = mAInvestorId;
			mFrontId = mAFrontId;
			mSessionId = mASessionId;
			submitCTPOrder(FIRST_INSTRUMENT, ins, orderPrice, direction, volume, THOST_FTDC_OPT_LimitPrice, THOST_FTDC_TC_GFD, true);
		}
	}else{
		if(USTPStrategyBase::isSgitInMarket(mFirstInsStatus)){
			mTempOrderLabel = orderLabel;
			mTempOffsetFlag = offsetFlag;
			mTempBS = direction;
			mTempFirstIns = ins;
			mTempOrderQty = volume;
			mTempOrderPrice = orderPrice;
			mTempBrokerId = mBBrokerId;
			mTempUserId = mBUserId;
			mTempInvestorId = mBInvestorId;
			mTempFrontId = mBFrontId;
			mTempSessionId = mBSessionId;
			mFirstInsStatus = USTP_FTDC_OS_CANCEL_SUBMIT;
			submitSgitAction(FIRST_INSTRUMENT, mFirstOrderRef, mFirstOrderSysId, mFirstIns);
		}else if(USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus || Sgit_FTDC_OST_Canceled == mFirstInsStatus){
			mOrderLabel = orderLabel;
			mOffsetFlag = offsetFlag;
			mBS = direction;
			mFirstIns = ins;
			mOrderQty = volume;
			mOrderPrice = orderPrice;
			mFirstRemainQty = volume;
			mBrokerId = mBBrokerId;
			mUserId = mBUserId;
			mInvestorId = mBInvestorId;
			mFrontId = mBFrontId;
			mSessionId = mBSessionId;
			submitSgitOrder(FIRST_INSTRUMENT, ins, orderPrice, direction, volume, Sgit_FTDC_OPT_LimitPrice, Sgit_FTDC_TC_GFD, true);
		}
	}
}

void USTPSpeculateOrder::submitCTPOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns)
{
	double adjustPrice = (priceType == THOST_FTDC_OPT_LimitPrice) ? orderPrice : 0.0;
	QString orderRef;
	mFirstInsStatus = USTP_FTDC_OS_ORDER_SUBMIT;
	int reqId = USTPMutexId::getMutexId();
	CTPTradeApi::reqOrderInsert(reqId, orderRef, mBrokerId, mUserId, mInvestorId, instrument, priceType, timeCondition, adjustPrice, qty, direction, mOffsetFlag, mHedgeFlag, THOST_FTDC_VC_AV);
	mFirstOrderRef = orderRef;
	QString szReqId = QString::number(reqId);
	mReqMap.insert(orderRef, szReqId);
	emit onUpdateOrderShow(szReqId, instrument, mOrderLabel, 'N', direction, 0.0, qty, qty, 0, mOffsetFlag, priceType, mHedgeFlag, mOrderPrice);
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [CTP| ")) + insLabel + QString(tr("-OrderInsert]   Instrument: ")) + instrument +  QString(tr("  RequestId: ")) + szReqId + QString(tr("  OrderRef: ")) + orderRef + 
		QString(tr("  UserId: ")) + mUserId + QString(tr("  PriceType: ")) + QString(priceType) + QString(tr("  OrderPrice: ")) + QString::number(adjustPrice) + QString(tr("  OrderVolume: ")) + 
		QString::number(qty) + QString(tr("  Direction: ")) + QString(direction);
	USTPLogger::saveData(data);
#endif	
}

void USTPSpeculateOrder::submitSgitOrder(const QString& insLabel, const QString& instrument, const double& orderPrice, const char& direction, const int& qty, const char& priceType, const char& timeCondition, bool isFirstIns)
{	
	double adjustPrice = (priceType == Sgit_FTDC_OPT_LimitPrice) ? orderPrice : 0.0;
	QString orderRef;
	mFirstInsStatus = USTP_FTDC_OS_ORDER_SUBMIT;
	int reqId = USTPMutexId::getMutexId();
	SgitTradeApi::reqOrderInsert(reqId, orderRef, mBrokerId, mUserId, mInvestorId, instrument, priceType, timeCondition, adjustPrice, qty, direction, mOffsetFlag, mHedgeFlag, Sgit_FTDC_VC_AV);
	mFirstOrderRef = orderRef;
	QString szReqId = QString::number(reqId);
	mReqMap.insert(orderRef, szReqId);
	emit onUpdateOrderShow(szReqId, instrument, mOrderLabel, 'N', direction, 0.0, qty, qty, 0, mOffsetFlag, priceType, mHedgeFlag, mOrderPrice);
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [SGIT| ")) + insLabel + QString(tr("-OrderInsert]   Instrument: ")) + instrument +  QString(tr("  RequestId: ")) + szReqId + QString(tr("  OrderRef: ")) + orderRef + 
		QString(tr("  UserId: ")) + mUserId + QString(tr("  PriceType: ")) + QString(priceType) + QString(tr("  OrderPrice: ")) + QString::number(adjustPrice) + QString(tr("  OrderVolume: ")) + 
		QString::number(qty) + QString(tr("  Direction: ")) + QString(direction);
	USTPLogger::saveData(data);
#endif	
}



void USTPSpeculateOrder::doCTPRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
										const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
										const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId)
{	
	if(mReqMap.find(orderRef) == mReqMap.end())
		return;
	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, orderStatus, direction, orderPrice, orderVolume, remainVolume, tradeVolume, offsetFlag, priceType, hedgeFlag, orderPrice);
	if(mFirstOrderRef == orderRef){
		mFirstOrderSysId = orderSysId;
		mFirstInsStatus = orderStatus;
		mFirstRemainQty = remainVolume;
		if (THOST_FTDC_OST_Canceled == orderStatus){
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, false, mOrderType, 0.0, 0.0, 0.0, 0.0);
			if(mTempOrderLabel == "")
				return;
			mOrderLabel = mTempOrderLabel;
			mFirstIns = mTempFirstIns;
			mOrderPrice = mTempOrderPrice;
			mBS = mTempBS;
			mOrderQty = mTempOrderQty;
			mOffsetFlag = mTempOffsetFlag;

			mBrokerId = mTempBrokerId;
			mUserId = mTempUserId;
			mInvestorId = mTempInvestorId;
			mFrontId = mTempFrontId;
			mSessionId = mTempSessionId;
			submitCTPOrder(FIRST_INSTRUMENT, mFirstIns, mOrderPrice, mBS, mOrderQty, THOST_FTDC_OPT_LimitPrice, THOST_FTDC_TC_GFD, true);
		}else if(THOST_FTDC_OST_AllTraded == orderStatus){
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, true, mOrderType, 0.0, 0.0, 0.0, 0.0);
			mFirstInsStatus = USTP_FTDC_OS_ORDER_NO_ORDER;
		}
	}
}


void USTPSpeculateOrder::doCTPErrRtnOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
												 const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
												 const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId)
{	
	if(mReqMap.find(orderRef) == mReqMap.end())
		return;

	if(mFirstIns == instrumentId)
		mFirstInsStatus = USTP_FTDC_OS_ORDER_ERROR;

	switch (errorId){
	case 22:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'D', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice); //重复的报单
		break;
	case 31:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'Z', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//	资金不足
		break;
	case 42:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'S', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//	结算结果未确认
		break;
	case 50:
	case 51:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'P', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//平仓位不足
		break;
	default:
		emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'W', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);
		break;
	}
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [CTP| ErrRtnOrderInsert] orderRef: ")) + orderRef + QString(tr("  RequestId: ")) + mReqMap[orderRef] + QString(tr("  InstrumentId: ")) + instrumentId + 
		QString(tr("  ErrorId: ")) + QString::number(errorId) + QString(tr("  ErrorMsg: ")) + errorMsg;
	USTPLogger::saveData(data);
#endif
}

void USTPSpeculateOrder::doCTPErrRtnOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
												 const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
												 const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId)
{
#ifdef _DEBUG
	if(mReqMap.find(orderActionRef) == mReqMap.end())
		return;
	QString data = mOrderLabel + QString(tr("  [CTP| ErrRtnOrderAction] orderSysId: ")) + orderSysId + 
		QString(tr("  UserActionLocalId: ")) + userActionLocalId  + QString(tr("  ErrorId: ")) + QString::number(errorId) + QString(tr("  ErrorMsg: ")) + errorMsg;
	USTPLogger::saveData(data);
#endif
}

void USTPSpeculateOrder::doDelOrder(const QString& orderStyle)
{	
	if(orderStyle != mOrderLabel)
		return;

	QStringList items = orderStyle.split("|");
	QString severType = items.at(0);
	if(severType == CTP_SERVER){
		if(isCTPInMarket(mFirstInsStatus)){
			mTempOrderLabel = "";
			mFirstInsStatus = USTP_FTDC_OS_CANCEL_SUBMIT;
			submitCTPAction(FIRST_INSTRUMENT, mFirstOrderRef, mFirstIns);
		}else if(USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus || USTP_FTDC_OS_ORDER_ERROR == mFirstInsStatus || THOST_FTDC_OST_Canceled == mFirstInsStatus){
			emit onUpdateOrderShow(QString::number(mRequestId), mFirstIns, mOrderLabel, THOST_FTDC_OST_Canceled, mBS, 0.0, mOrderQty, mOrderQty, 0, mOffsetFlag, THOST_FTDC_OPT_LimitPrice, mHedgeFlag, mOrderPrice);
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, false, mOrderType, 0.0, 0.0, 0.0, 0.0);
			mFirstInsStatus = USTP_FTDC_OS_ORDER_NO_ORDER;
		}
		QString data = mOrderLabel + QString(tr("  [CTP| DoDelOrder]   mFirstInsStatus: ")) + QString(mFirstInsStatus);
		USTPLogger::saveData(data);
	}else{
		if(isSgitInMarket(mFirstInsStatus)){
			mTempOrderLabel = "";
			mFirstInsStatus = USTP_FTDC_OS_CANCEL_SUBMIT;
			submitSgitAction(FIRST_INSTRUMENT, mFirstOrderRef, mFirstOrderSysId, mFirstIns);
		}else if(USTP_FTDC_OS_ORDER_NO_ORDER == mFirstInsStatus || USTP_FTDC_OS_ORDER_ERROR == mFirstInsStatus || Sgit_FTDC_OST_Canceled == mFirstInsStatus){
			emit onUpdateOrderShow(QString::number(mRequestId), mFirstIns, mOrderLabel, Sgit_FTDC_OST_Canceled, mBS, 0.0, mOrderQty, mOrderQty, 0, mOffsetFlag, Sgit_FTDC_OPT_LimitPrice, mHedgeFlag, mOrderPrice);
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, false, mOrderType, 0.0, 0.0, 0.0, 0.0);
			mFirstInsStatus = USTP_FTDC_OS_ORDER_NO_ORDER;
		}
		QString data = mOrderLabel + QString(tr("  [SGIT| DoDelOrder]   mFirstInsStatus: ")) + QString(mFirstInsStatus);
		USTPLogger::saveData(data);
	}
}

void USTPSpeculateOrder::submitCTPAction(const QString& insLabel, const QString& orderLocalId, const QString& instrument)
{	
	CTPTradeApi::reqOrderAction(USTPMutexId::getMutexId(), mBrokerId, mUserId, mInvestorId, instrument, orderLocalId, mFrontId, mSessionId);
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [CTP| ")) + insLabel + QString(tr("-OrderAction]   OrderRef: ")) + orderLocalId + QString(tr("  InstrumentId: ")) + instrument;
	USTPLogger::saveData(data);
#endif	
}

void USTPSpeculateOrder::submitSgitAction(const QString& insLabel, const QString& orderLocalId, const QString& ordersysId, const QString& instrument)
{	
	SgitTradeApi::reqOrderAction(USTPMutexId::getMutexId(), mBrokerId, mUserId, mInvestorId, instrument, orderLocalId, ordersysId, USTPMutexId::getInsExchangeId(instrument));
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [SGIT| ")) + insLabel + QString(tr("-OrderAction]   OrderRef: ")) + orderLocalId + QString(tr("  InstrumentId: ")) + instrument;
	USTPLogger::saveData(data);
#endif	
}


void USTPSpeculateOrder::doSgitRtnOrder(const QString& localId, const QString& orderRef, const QString& instrumentId, const char& direction, const double& orderPrice, const int& orderVolume,
					const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedgeFlag, const char& orderStatus,
					const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId, const QString& statusMsg, const char& timeCondition, const int& reqId)
{
	if(mReqMap.find(orderRef) == mReqMap.end())
		return;
	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, orderStatus, direction, orderPrice, orderVolume, remainVolume, tradeVolume, offsetFlag, priceType, hedgeFlag, orderPrice);
	if(mFirstOrderRef == orderRef){
		mFirstOrderSysId = orderSysId;
		mFirstInsStatus = orderStatus;
		mFirstRemainQty = remainVolume;
		if (Sgit_FTDC_OST_Canceled == orderStatus){
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, false, mOrderType, 0.0, 0.0, 0.0, 0.0);
			if(mTempOrderLabel == "")
				return;
			mOrderLabel = mTempOrderLabel;
			mFirstIns = mTempFirstIns;
			mOrderPrice = mTempOrderPrice;
			mBS = mTempBS;
			mOrderQty = mTempOrderQty;
			mOffsetFlag = mTempOffsetFlag;

			mBrokerId = mTempBrokerId;
			mUserId = mTempUserId;
			mInvestorId = mTempInvestorId;
			mFrontId = mTempFrontId;
			mSessionId = mTempSessionId;
			submitSgitOrder(FIRST_INSTRUMENT, mFirstIns, mOrderPrice, mBS, mOrderQty, Sgit_FTDC_OPT_LimitPrice, Sgit_FTDC_TC_GFD, true);
		}else if(Sgit_FTDC_OST_AllTraded == orderStatus){
			emit onOrderFinished(mOrderLabel, mFirstIns, mSecIns, mOrderPrice, mOrderQty, mBS, mOffsetFlag, mHedgeFlag, true, mOrderType, 0.0, 0.0, 0.0, 0.0);
			mFirstInsStatus = USTP_FTDC_OS_ORDER_NO_ORDER;
		}
	}
}

void USTPSpeculateOrder::doSgitRspOrderInsert(const QString& userId, const QString& brokerId, const char& direction, const QString& gtdDate, const char& hedgeFlag,
						  const QString& instrumentId, const QString& investorId, const char& offsetFlag, const char& priceType, const char& timeCondition,
						  const QString& orderRef, const double& orderPrice, const int& volume, const int& errorId, const QString& errorMsg, const int& reqId)
{
	if(mReqMap.find(orderRef) == mReqMap.end())
		return;

	//if(mFirstIns == instrumentId)
	//	mFirstInsStatus = USTP_FTDC_OS_ORDER_ERROR;

	//switch (errorId){
	//case 22:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'D', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice); //重复的报单
	//	break;
	//case 31:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'Z', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//	资金不足
	//	break;
	//case 42:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'S', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//	结算结果未确认
	//	break;
	//case 50:
	//case 51:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'P', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);	//平仓位不足
	//	break;
	//default:
	//	emit onUpdateOrderShow(mReqMap[orderRef], instrumentId, mOrderLabel, 'W', direction, orderPrice, volume, volume, 0, offsetFlag, priceType, hedgeFlag, mOrderPrice);
	//	break;
	//}
#ifdef _DEBUG
	QString data = mOrderLabel + QString(tr("  [SGIT| RspOrderInsert] orderRef: ")) + orderRef + QString(tr("  RequestId: ")) + mReqMap[orderRef] + QString(tr("  InstrumentId: ")) + instrumentId + 
		QString(tr("  ErrorId: ")) + QString::number(errorId) + QString(tr("  ErrorMsg: ")) + errorMsg;
	USTPLogger::saveData(data);
#endif
}

void USTPSpeculateOrder::doSgitRspOrderAction(const char& actionFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId,
						  const QString& orderSysId, const QString& userActionLocalId, const QString& orderActionRef, const double& orderPrice, 
						  const int& volumeChange, const int& errorId, const QString& errorMsg, const int& reqId)
{
#ifdef _DEBUG
	if(mReqMap.find(orderActionRef) == mReqMap.end())
		return;
	QString data = mOrderLabel + QString(tr("  [SGIT| RspOrderAction] orderSysId: ")) + orderSysId + 
		QString(tr("  UserActionLocalId: ")) + userActionLocalId  + QString(tr("  ErrorId: ")) + QString::number(errorId) + QString(tr("  ErrorMsg: ")) + errorMsg;
	USTPLogger::saveData(data);
#endif
}


#include "moc_USTPUserStrategy.cpp"