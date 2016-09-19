#ifndef USTP_COMPLEX_MD_H
#define USTP_COMPLEX_MD_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QThread>
class USTPMarketWidget;

class USTPComplexMd : public QObject
{
	Q_OBJECT
public:

	USTPComplexMd(const QString& firstIns, const QString& secondIns, USTPMarketWidget* pMarketWidget);

	virtual~USTPComplexMd();

	public slots:
		void doCTPRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
			const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
			const double& lowestPrice, const int& volume);

		void doSgitRtnDepthMarketData(const QString& instrumentId, const double& preSettlementPrice, const double& openPrice, const double& lastPrice,
			const double& bidPrice, const int& bidVolume, const double& askPrice, const int& askVolume, const double& highestPrice, 
			const double& lowestPrice, const int& volume);


		void doGetComplexDepthPrice(const QString& complexIns, const QString& reqKey, const int& rowIndex);

signals:
		void onUSTPComplexMd(const QString& key, const QString& firstIns, const QString& secIns, const QString& bbPrice, const QString& bsPrice, const QString& sbPrice,
			const QString& ssPrice, const QString& bbQty, const QString& bsQty, const QString& sbQty, const QString& ssQty);
		
		void onUpdateComplexDepthPrice(const QString& complexIns, const QString& reqKey, const int& rowId, const double& firstMarketBidPrice, const double& firstMarketAskPrice,
			const double& secMarketBidPrice, const double& secMarketAskPrice);

private:
	void initConnect(USTPMarketWidget* pMarketWidget);

	void calculateComplexMd();

	int getInsPrcision(const double& value);

private:
	QThread mComplexMdThread;
	QString mFirstInstrument;
	QString mSecondInstrument;
	QString mComplexKey;
	double mFirstBidPrice;
	double mFirstAskPrice;
	int mFirstBidQty;
	int mFirstAskQty;
	double mSecondBidPrice;
	double mSecondAskPrice;
	int mSecondBidQty;
	int mSecondAskQty;
	int mInsPrecision;

	double mFirstBidPriceDiff;
	double mSecondBidPriceDiff;
	double mThirdBidPriceDiff;

	double mFirstAskPriceDiff;
	double mSecondAskPriceDiff;
	double mThirdAskPriceDiff;
	bool mFirstIsInit;
	bool mSecIsInit;
};

#endif