#ifndef USTP_SPE_COMPLEX_MD_H
#define USTP_SPE_COMPLEX_MD_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QThread>
class USTPSpeMarketWidget;

class USTPSpeComplexMd : public QObject
{
	Q_OBJECT
public:

	USTPSpeComplexMd(const QString& selIns, USTPSpeMarketWidget* pSpeMarketWidget);

	virtual~USTPSpeComplexMd();

public slots:
	void doCTPRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
		const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
		const double& lowestPrice, const int& volume);
	void doSgitRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
		const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
		const double& lowestPrice, const int& volume);
	void doGetSpeDepthMd(const QString& selIns, const int& direction);
	void doGetUnilateralSpeDepthMd(const QString& insName, const QString& key, const int& indexRow);

signals:
		void onUSTPSpeComplexMd(const QString& selIns, const QString& bidPrice, const QString& bidVolume, const QString& askPrice, const QString& askVolume, const QString& tradeVolume, const QString& lastPrice);
		void onUpdateSpeDepthMd(const QString& selIns, const int& direction, const double& orderPrice);
		void onUpdateUnilateralMd(const QString& selIns, const QString& key, const int& indexRow, const double& bidPrice, const double& askPrice);
private:
	void initConnect(USTPSpeMarketWidget* pSpeMarketWidget);

	int getInsPrcision(const double& value);

private:
	QString mInstrument;
	double mBidPrice;
	double mAskPrice;
	int mBidQty;
	int mAskQty;
	int mTradeQty;
	int mInsPrecision;
	bool mIsInit;
	QThread mComplexMdThread;
};

#endif