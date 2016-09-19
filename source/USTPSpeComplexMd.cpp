#include "USTPSpeComplexMd.h"
#include <QtCore/QThread>
#include "USTPMutexId.h"
#include "USTPCtpLoader.h"
#include "USTPSpeMarketWidget.h"
#include "USTPConfig.h"


USTPSpeComplexMd::USTPSpeComplexMd(const QString& selIns, USTPSpeMarketWidget* pSpeMarketWidget)
{	
	mBidPrice = 0.0;
	mAskPrice = 0.0;
	mBidQty = 0;
	mAskQty = 0;
	mTradeQty = 0;
	mInstrument = selIns;
	mIsInit = true;
	mInsPrecision = getInsPrcision(USTPMutexId::getInsPriceTick(selIns));
	initConnect(pSpeMarketWidget);
	moveToThread(&mComplexMdThread);	
	mComplexMdThread.start();
}

USTPSpeComplexMd::~USTPSpeComplexMd()
{
	mComplexMdThread.quit();
	mComplexMdThread.wait();
}

void USTPSpeComplexMd::initConnect(USTPSpeMarketWidget* pSpeMarketWidget)
{	
	connect(USTPCtpLoader::getAMdSpi(), SIGNAL(onCTPRtnDepthMarketData(const QString&, const double&, const double&, const double&,const double&, 
		const int&, const double&, const int&, const double&, const double&, const int&)), this, SLOT(doCTPRtnDepthMarketData(const QString&, const double&, const double&, 
		const double&,const double&, const int&, const double&, const int&, const double&, const double&, const int&)), Qt::QueuedConnection);

	connect(USTPCtpLoader::getBMdSpi(), SIGNAL(onSgitRtnDepthMarketData(const QString&, const double&, const double&, const double&,const double&, 
		const int&, const double&, const int&, const double&, const double&, const int&)), this, SLOT(doSgitRtnDepthMarketData(const QString&, const double&, const double&, 
		const double&,const double&, const int&, const double&, const int&, const double&, const double&, const int&)), Qt::QueuedConnection);


	connect(this, SIGNAL(onUSTPSpeComplexMd(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)), pSpeMarketWidget, 
		SLOT(doUSTPSpeComplexMd(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)), Qt::QueuedConnection);

	connect(pSpeMarketWidget, SIGNAL(onGetSpeDepthMd(const QString&, const int&)), this, SLOT(doGetSpeDepthMd(const QString&, const int&)), Qt::QueuedConnection);
	connect(pSpeMarketWidget, SIGNAL(onGetUnilateralSpeDepthMd(const QString&, const QString&, const int&)), this, SLOT(doGetUnilateralSpeDepthMd(const QString&, const QString&, const int&)), Qt::QueuedConnection);
	connect(this, SIGNAL(onUpdateSpeDepthMd(const QString&, const int&, const double&)), pSpeMarketWidget, SLOT(doUpdateSpeDepthMd(const QString&, const int&, const double&)), Qt::QueuedConnection);
	connect(this, SIGNAL(onUpdateUnilateralMd(const QString&, const QString&, const int&, const double&, const double&)), pSpeMarketWidget, SLOT(doUpdateUnilateralMd(const QString&, 
		const QString&, const int&, const double&, const double&)), Qt::QueuedConnection);
}

void USTPSpeComplexMd::doCTPRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
												const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
												const double& lowestPrice, const int& volume)
{
	if(mInstrument.compare(instrumentId) == 0){
		mBidPrice = bidPrice;
		mAskPrice = askPrice;
		mBidQty = bidVolume;
		mAskQty = askVolume;
		mTradeQty = volume;
		QString szBidPrice = QString("%1").arg(bidPrice, 0, 'f', mInsPrecision);
		QString szAskPrice = QString("%1").arg(askPrice, 0, 'f', mInsPrecision);
		QString szLastPrice = QString("%1").arg(lastPrice, 0, 'f', mInsPrecision);
		QString szBidQty = QString::number(bidVolume);
		QString szAskQty = QString::number(askVolume);
		QString szTradeQty = QString::number(volume);
		emit onUSTPSpeComplexMd(tr("CTP|") + instrumentId, szBidPrice, szBidQty, szAskPrice, szAskQty, szTradeQty, szLastPrice);
		if(mIsInit && highestPrice > 0.0 && lowestPrice > 0.0){
			mIsInit = false;
			USTPMutexId::setUpperLowerPrice(instrumentId, highestPrice, lowestPrice);
		}
	}
}

void USTPSpeComplexMd::doSgitRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
											   const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
											   const double& lowestPrice, const int& volume)
{
	if(mInstrument.compare(instrumentId) == 0){
		mBidPrice = bidPrice;
		mAskPrice = askPrice;
		mBidQty = bidVolume;
		mAskQty = askVolume;
		mTradeQty = volume;
		QString szBidPrice = QString("%1").arg(bidPrice, 0, 'f', mInsPrecision);
		QString szAskPrice = QString("%1").arg(askPrice, 0, 'f', mInsPrecision);
		QString szLastPrice = QString("%1").arg(lastPrice, 0, 'f', mInsPrecision);
		QString szBidQty = QString::number(bidVolume);
		QString szAskQty = QString::number(askVolume);
		QString szTradeQty = QString::number(volume);
		emit onUSTPSpeComplexMd(tr("SGIT|") + instrumentId, szBidPrice, szBidQty, szAskPrice, szAskQty, szTradeQty, szLastPrice);
		if(mIsInit && highestPrice > 0.0 && lowestPrice > 0.0){
			mIsInit = false;
			USTPMutexId::setUpperLowerPrice(instrumentId, highestPrice, lowestPrice);
		}
	}
}


int USTPSpeComplexMd::getInsPrcision(const double& value)
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

void USTPSpeComplexMd::doGetSpeDepthMd(const QString& selIns, const int& direction)
{
	if(mInstrument == selIns){
		switch (direction)
		{
		case 0:
			emit onUpdateSpeDepthMd(mInstrument, 0, mBidPrice);
			break;
		case 1:
			emit onUpdateSpeDepthMd(mInstrument, 1, mAskPrice);
			break;
		}
	}
}

void USTPSpeComplexMd::doGetUnilateralSpeDepthMd(const QString& insName, const QString& key, const int& indexRow)
{
	if(mInstrument == insName)
		emit onUpdateUnilateralMd(insName, key, indexRow, mBidPrice, mAskPrice);
}

#include "moc_USTPSpeComplexMd.cpp"