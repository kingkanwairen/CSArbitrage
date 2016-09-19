#include "USTPComplexMd.h"
#include <QtCore/QThread>
#include "USTPMutexId.h"
#include "USTPCtpLoader.h"
#include "USTPMarketWidget.h"
#include "USTPConfig.h"

#define BS_VALUE -100000.0

USTPComplexMd::USTPComplexMd(const QString& firstIns, const QString& secondIns, USTPMarketWidget* pMarketWidget)
{	
	mFirstBidPrice = 0.0;
	mFirstAskPrice = 0.0;
	mFirstBidQty = 0;
	mFirstAskQty = 0;
	mSecondBidPrice = 0.0;
	mSecondAskPrice = 0.0;

	mFirstBidPriceDiff = BS_VALUE;
	mSecondBidPriceDiff = BS_VALUE;
	mThirdBidPriceDiff = BS_VALUE;

	mFirstAskPriceDiff = BS_VALUE;
	mSecondAskPriceDiff = BS_VALUE;
	mThirdAskPriceDiff = BS_VALUE;
	
	mFirstIsInit = true;
	mSecIsInit = true;
	mSecondBidQty = 0;
	mSecondAskQty = 0;
	mFirstInstrument = firstIns;
	mSecondInstrument = secondIns;
	mComplexKey = mFirstInstrument + tr("|") + mSecondInstrument;
	int nFirstPrecise = getInsPrcision(USTPMutexId::getInsPriceTick(firstIns));
	int nSecPrecise = getInsPrcision(USTPMutexId::getInsPriceTick(secondIns));
	mInsPrecision = nFirstPrecise > nSecPrecise ? nFirstPrecise : nSecPrecise;
	initConnect(pMarketWidget);
	moveToThread(&mComplexMdThread);	
	mComplexMdThread.start();
}

USTPComplexMd::~USTPComplexMd()
{
	mComplexMdThread.quit();
	mComplexMdThread.wait();
}

void USTPComplexMd::initConnect(USTPMarketWidget* pMarketWidget)
{	
	connect(USTPCtpLoader::getAMdSpi(), SIGNAL(onCTPRtnDepthMarketData(const QString&, const double&, const double&, const double&,const double&, const int&, 
		const double&, const int&, const double&, const double&, const int&)), this, SLOT(doCTPRtnDepthMarketData(const QString&, const double&, const double&, 
		const double&,const double&, const int&, const double&, const int&, const double&, const double&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getBMdSpi(), SIGNAL(onSgitRtnDepthMarketData(const QString&, const double&, const double&, const double&,const double&, const int&, 
		const double&, const int&, const double&, const double&, const int&)), this, SLOT(doSgitRtnDepthMarketData(const QString&, const double&, const double&, 
		const double&,const double&, const int&, const double&, const int&, const double&, const double&, const int&)), Qt::QueuedConnection);

	connect(this, SIGNAL(onUSTPComplexMd(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&)), pMarketWidget, SLOT(doUSTPComplexMd(const QString&, const QString&, const QString&, const QString&, const QString&,
		const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)), Qt::QueuedConnection);

	connect(pMarketWidget, SIGNAL(onGetComplexDepthPrice(const QString&, const QString&, const int&)), this, SLOT(doGetComplexDepthPrice(const QString&, const QString&, const int&)), Qt::QueuedConnection);

	connect(this, SIGNAL(onUpdateComplexDepthPrice(const QString&, const QString&, const int&, const double&, const double&,const double&, const double&)), 
		pMarketWidget, SLOT(doUpdateComplexDepthPrice(const QString&, const QString&, const int&, const double&, const double&,const double&, const double&)), Qt::QueuedConnection);
}

void USTPComplexMd::doCTPRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
											 const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
											 const double& lowestPrice, const int& volume)
{	
	if(instrumentId != mFirstInstrument)
		return;
	mFirstBidPrice = bidPrice;
	mFirstAskPrice = askPrice;
	mFirstBidQty = bidVolume;
	mFirstAskQty = askVolume;
	calculateComplexMd();
	if(mFirstIsInit && highestPrice > 0.0 && lowestPrice > 0.0){
		mFirstIsInit = false;
		USTPMutexId::setUpperLowerPrice(mFirstInstrument, highestPrice, lowestPrice);
	}
}

void USTPComplexMd::doSgitRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
											const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
											const double& lowestPrice, const int& volume)
{	
	if(instrumentId != mSecondInstrument)
		return;
	mSecondBidPrice = bidPrice;
	mSecondAskPrice = askPrice;
	mSecondBidQty = bidVolume;
	mSecondAskQty = askVolume;
	calculateComplexMd();
	if(mSecIsInit && highestPrice > 0.0 && lowestPrice > 0.0){
		mFirstIsInit = false;
		USTPMutexId::setUpperLowerPrice(mSecondInstrument, highestPrice, lowestPrice);
	}
}

void USTPComplexMd::calculateComplexMd()
{
	double bbPrice = mFirstBidPrice - mSecondBidPrice;
	double bsPrice = mFirstBidPrice - mSecondAskPrice;
	double sbPrice = mFirstAskPrice - mSecondBidPrice;
	double ssPrice = mFirstAskPrice - mSecondAskPrice;
	int bbQty = mFirstBidQty > mSecondBidQty ? mSecondBidQty : mFirstBidQty;
	int bsQty = mFirstBidQty > mSecondAskQty ? mSecondAskQty : mFirstBidQty;
	int sbQty = mFirstAskQty > mSecondBidQty ? mSecondBidQty : mFirstAskQty;
	int ssQty = mFirstAskQty > mSecondAskQty ? mSecondAskQty : mFirstAskQty;
	
	mThirdBidPriceDiff = mSecondBidPriceDiff;
	mSecondBidPriceDiff = mFirstBidPriceDiff;
	mFirstBidPriceDiff = bbPrice;

	mThirdAskPriceDiff = mSecondAskPriceDiff;
	mSecondAskPriceDiff = mFirstAskPriceDiff;
	mFirstAskPriceDiff = ssPrice;

	if(mSecondBidPriceDiff <= BS_VALUE)
		mSecondBidPriceDiff = mFirstBidPriceDiff;
	if(mThirdBidPriceDiff <= BS_VALUE)
		mThirdBidPriceDiff = mSecondBidPriceDiff;
	if(mSecondAskPriceDiff <= BS_VALUE)
		mSecondAskPriceDiff = mFirstAskPriceDiff;
	if(mThirdAskPriceDiff <= BS_VALUE)
		mThirdAskPriceDiff = mSecondAskPriceDiff;

	QString szBBPrice = QString("%1").arg(bbPrice, 0, 'f', mInsPrecision);
	QString szBSPrice = QString("%1").arg(bsPrice, 0, 'f', mInsPrecision);
	QString szSBPrice = QString("%1").arg(sbPrice, 0, 'f', mInsPrecision);
	QString szSSPrice = QString("%1").arg(ssPrice, 0, 'f', mInsPrecision);
	QString szBBQty = QString::number(bbQty);
	QString szBSQty = QString::number(bsQty);
	QString szSBQty = QString::number(sbQty);
	QString szSSQty = QString::number(ssQty);
	emit onUSTPComplexMd(mComplexKey, mFirstInstrument, mSecondInstrument, szBBPrice, szBSPrice, szSBPrice, szSSPrice, szBBQty, szBSQty, szSBQty, szSSQty);
}

int USTPComplexMd::getInsPrcision(const double& value)
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

void USTPComplexMd::doGetComplexDepthPrice(const QString& complexIns, const QString& reqKey, const int& rowIndex)
{
	if(mComplexKey == complexIns)
		onUpdateComplexDepthPrice(complexIns, reqKey, rowIndex, mFirstBidPrice, mFirstAskPrice, mSecondBidPrice, mSecondAskPrice);
}

#include "moc_USTPComplexMd.cpp"