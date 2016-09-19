#include "USTPMutexId.h"
#include "USTPConfig.h"
#include <QtCore/QDateTime>

USTPMutexId* USTPMutexId::mThis = NULL;

#define BASE_TIME 91500

USTPMutexId::USTPMutexId()
{
	
}

USTPMutexId::~USTPMutexId()
{

}

bool USTPMutexId::initialize()
{
	mThis = new USTPMutexId;	
	mThis->nRequestIndex = INIT_VALUE;
	mThis->nNewOrderIndex = INIT_VALUE;
	mThis->nMarketIndex = 0;
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("hhmmss");
	int nCurrentTime = current_date.toInt();
	if(nCurrentTime >= BASE_TIME)
		mThis->mIsBaseTime = true;
	else
		mThis->mIsBaseTime = false;
	return true;
}

bool USTPMutexId::setUserAInfo(const QString& userId, const QString& psw, int maxId, int frontId, int sessionId)
{
	mThis->mAUserId = userId;
	mThis->mAInvestorId = userId;
	mThis->mAFrontId = frontId;
	mThis->mASessionId = sessionId;
	mThis->nAOrderId = maxId;
	mThis->mAPsw = psw;
	return true;
}

bool USTPMutexId::setUserBInfo(const QString& userId, const QString& psw, int maxId, int frontId, int sessionId)
{
	mThis->mBUserId = userId;
	mThis->mBInvestorId = userId;
	mThis->mBFrontId = frontId;
	mThis->mBSessionId = sessionId;
	mThis->nBOrderId = maxId;
	mThis->mBPsw = psw;
	return true;
}

QString USTPMutexId::getAInvestorId()
{
	return mThis->mAInvestorId;
}

QString USTPMutexId::getBInvestorId()
{
	return mThis->mBInvestorId;
}

void USTPMutexId::setAInvestorId(const QString& investorId)
{
	mThis->mAInvestorId = investorId;
}

void USTPMutexId::setBInvestorId(const QString& investorId)
{
	mThis->mBInvestorId = investorId;
}

int USTPMutexId::getOrderRef(const QString& userId)
{
	QMutexLocker locker(&mThis->mOrderRefMutex);
	int nOrderRf = 0;
	if(userId == mThis->mAUserId)
		nOrderRf = ++mThis->nAOrderId;
	else
		nOrderRf = ++mThis->nBOrderId;
	return nOrderRf;
}

int USTPMutexId::getMutexId()
{
	QMutexLocker locker(&mThis->mRequestMutex);
	mThis->nRequestIndex++;
	return mThis->nRequestIndex;
}

int USTPMutexId::getNewOrderIndex()
{
	QMutexLocker locker(&mThis->mNewOrderMutex);
	mThis->nNewOrderIndex++;
	return mThis->nNewOrderIndex;
}

QString USTPMutexId::getAUserId()
{
	return mThis->mAUserId;
}

QString USTPMutexId::getBUserId()
{
	return mThis->mBUserId;
}

QString USTPMutexId::getALoginPsw()
{
	return mThis->mAPsw;
}

QString USTPMutexId::getBLoginPsw()
{
	return mThis->mBPsw;
}

int USTPMutexId::getFrontId(const QString& userId)
{	
	int nFront = 0;
	if(userId == mThis->mAUserId)
		nFront = mThis->mAFrontId;
	else
		nFront = mThis->mBFrontId;
	return nFront;
}

int USTPMutexId::getSessionId(const QString& userId)
{
	int nSession = 0;
	if(userId == mThis->mAUserId)
		nSession = mThis->mASessionId;
	else
		nSession = mThis->mBSessionId;
	return nSession;
}

bool USTPMutexId::setInsBidPosition(const QString& userId, const QString& ins, const int& qty)
{	
	if(userId == mThis->mAUserId){
		if(mThis->mABidQtys.find(ins) != mThis->mABidQtys.end()){
			mThis->mABidQtys[ins] = qty;
		}else{
			mThis->mABidQtys.insert(ins, qty);
		}
	}else{
		if(mThis->mBBidQtys.find(ins) != mThis->mBBidQtys.end()){
			mThis->mBBidQtys[ins] = qty;
		}else{
			mThis->mBBidQtys.insert(ins, qty);
		}
	}
	return true;
}


bool USTPMutexId::setInsAskPosition(const QString& userId, const QString& ins, const int& qty)
{	
	if(userId == mThis->mAUserId){
		if(mThis->mAAskQtys.find(ins) != mThis->mAAskQtys.end()){
			mThis->mAAskQtys[ins] = qty;
		}else{
			mThis->mAAskQtys.insert(ins, qty);
		}
	}else{
		if(mThis->mBAskQtys.find(ins) != mThis->mBAskQtys.end()){
			mThis->mBAskQtys[ins] = qty;
		}else{
			mThis->mBAskQtys.insert(ins, qty);
		}
	}

	return true;
}


bool USTPMutexId::addBidPosition(const QString& userId, const QString& ins, const int& qty)
{	
	if(userId == mThis->mAUserId){
		if(mThis->mABidQtys.find(ins) != mThis->mABidQtys.end()){
			mThis->mABidQtys[ins] += qty;
		}else{
			mThis->mABidQtys.insert(ins, qty);
		}
	}else{
		if(mThis->mBBidQtys.find(ins) != mThis->mBBidQtys.end()){
			mThis->mBBidQtys[ins] += qty;
		}else{
			mThis->mBBidQtys.insert(ins, qty);
		}
	}
	return true;
}

bool USTPMutexId::addAskPosition(const QString& userId, const QString& ins, const int& qty)
{	
	if(userId == mThis->mAUserId){
		if(mThis->mAAskQtys.find(ins) != mThis->mAAskQtys.end()){
			mThis->mAAskQtys[ins] += qty;
		}else{
			mThis->mAAskQtys.insert(ins, qty);
		}
	}else{
		if(mThis->mBAskQtys.find(ins) != mThis->mBAskQtys.end()){
			mThis->mBAskQtys[ins] += qty;
		}else{
			mThis->mBAskQtys.insert(ins, qty);
		}
	}
	return true;
}


int USTPMutexId::getInsBidPosition(const QString& userId, const QString& ins)
{
	if(userId == mThis->mAUserId){
		if(mThis->mABidQtys.find(ins) != mThis->mABidQtys.end()){
			return mThis->mABidQtys[ins];
		}
	}else{
		if(mThis->mBBidQtys.find(ins) != mThis->mBBidQtys.end()){
			return mThis->mBBidQtys[ins];
		}
	}	
	return 0;
}

int USTPMutexId::getInsAskPosition(const QString& userId, const QString& ins)
{
	if(userId == mThis->mAUserId){
		if(mThis->mAAskQtys.find(ins) != mThis->mAAskQtys.end()){
			return mThis->mAAskQtys[ins];
		}
	}else{
		if(mThis->mBAskQtys.find(ins) != mThis->mBAskQtys.end()){
			return mThis->mBAskQtys[ins];
		}
	}
	return 0;
}

bool USTPMutexId::setInsPriceTick(const QString& ins, const QString& exchangeId, const double& tick, const int& volumeMultiple)
{
	if(mThis->mInsTicks.find(ins) != mThis->mInsTicks.end()){
		return false;
	}
	mThis->mInsTicks.insert(ins, tick);
	mThis->mInsExhs.insert(ins, exchangeId);
	mThis->mInsMultiple.insert(ins, volumeMultiple);
	return true;
}

bool USTPMutexId::setInsMarketMaxVolume(const QString& ins, const int& volume)
{
	if(mThis->mMaxMarketQtys.find(ins) != mThis->mMaxMarketQtys.end()){
		return false;
	}
	mThis->mMaxMarketQtys.insert(ins, volume);
	return true;
}

int USTPMutexId::getInsMarketMaxVolume(const QString& ins)
{
	if(mThis->mMaxMarketQtys.find(ins) != mThis->mMaxMarketQtys.end()){
		return mThis->mMaxMarketQtys[ins];
	}
	return 0;
}

double USTPMutexId::getInsPriceTick(const QString& ins)
{
	if(mThis->mInsTicks.find(ins) != mThis->mInsTicks.end()){
		return mThis->mInsTicks[ins];
	}
	return 0.0;
}

QString USTPMutexId::getInsExchangeId(const QString& ins)
{
	if(mThis->mInsExhs.find(ins) != mThis->mInsExhs.end()){
		return mThis->mInsExhs[ins];
	}
	return QString(tr(""));
}

int USTPMutexId::getInsMultiple(const QString& ins)
{
	if(mThis->mInsMultiple.find(ins) != mThis->mInsMultiple.end()){
		return mThis->mInsMultiple[ins];
	}
	return 0;
}

bool USTPMutexId::getTotalBidPosition(const QString& userId, QMap<QString, int>& bidPostions)
{	
	if(userId == mThis->mAUserId){
		QMapIterator<QString, int> i(mThis->mABidQtys);
		while (i.hasNext()){
			i.next();
			bidPostions.insert(i.key(), i.value());
		}
	}else{
		QMapIterator<QString, int> i(mThis->mBBidQtys);
		while (i.hasNext()){
			i.next();
			bidPostions.insert(i.key(), i.value());
		}
	}
	
	return true;
}

bool USTPMutexId::getTotalAskPosition(const QString& userId, QMap<QString, int>& askPostions)
{	
	if(userId == mThis->mAUserId){
		QMapIterator<QString, int> i(mThis->mAAskQtys);
		while (i.hasNext()){
			i.next();
			askPostions.insert(i.key(), i.value());
		}
	}else{
		QMapIterator<QString, int> i(mThis->mBAskQtys);
		while (i.hasNext()){
			i.next();
			askPostions.insert(i.key(), i.value());
		}
	}
	return true;
}


int USTPMutexId::getMarketIndex()
{
	QMutexLocker locker(&mThis->mMarketMutex);
	mThis->nMarketIndex++;
	return mThis->nMarketIndex;
}

bool USTPMutexId::initActionNum(const QString& ins, const int& num)
{
	if(mThis->mInsActionNums.find(ins) != mThis->mInsActionNums.end()){
		return false;
	}
	if(mThis->mIsBaseTime)
		mThis->mInsActionNums.insert(ins, num);
	else
		mThis->mInsActionNums.insert(ins, 0);
	return true;
}

bool USTPMutexId::updateActionNum(const QString& ins)
{	
	QMutexLocker locker(&mThis->mInsActionMutex);
	if(mThis->mInsActionNums.find(ins) != mThis->mInsActionNums.end())
		mThis->mInsActionNums[ins] += 1;
	else
		mThis->mInsActionNums.insert(ins, 1);
	return true;
}

int USTPMutexId::getActionNum(const QString& ins)
{
	if(mThis->mInsActionNums.find(ins) != mThis->mInsActionNums.end()){
		return mThis->mInsActionNums[ins];
	}
	return 0;
}

bool USTPMutexId::getTotalActionNum(QMap<QString, int>& actionNums)
{
	QMapIterator<QString, int> i(mThis->mInsActionNums);
	while (i.hasNext()){
		i.next();
		actionNums.insert(i.key(), i.value());
	}
	return true;
}

bool USTPMutexId::finalize()
{
	if (mThis != NULL) {
		delete mThis;
		mThis = NULL;
	}
	return true;
}


bool USTPMutexId::setUpperLowerPrice(const QString& ins, const double& upperPrice, const double& lowerPrice)
{
	if(mThis->mUpperPrices.find(ins) != mThis->mUpperPrices.end()){
		return false;
	}
	mThis->mUpperPrices.insert(ins, upperPrice);

	if(mThis->mLowerPrices.find(ins) != mThis->mLowerPrices.end()){
		return false;
	}
	mThis->mLowerPrices.insert(ins, lowerPrice);
	return true;
}

double USTPMutexId::getUpperPrice(const QString& ins)
{
	if(mThis->mUpperPrices.find(ins) != mThis->mUpperPrices.end()){
		return mThis->mUpperPrices[ins];
	}
	return 0.0;
}

double USTPMutexId::getLowerPrice(const QString& ins)
{
	if(mThis->mLowerPrices.find(ins) != mThis->mLowerPrices.end()){
		return mThis->mLowerPrices[ins];
	}
	return 0.0;
}

#include "moc_USTPMutexId.cpp"