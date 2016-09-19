#ifndef USTP_TRADE_WIDGET_H
#define USTP_TRADE_WIDGET_H

#include <QtGui/QTreeWidget>

class USTPTradeWidget : public  QTreeWidget
{
	Q_OBJECT
public:

	USTPTradeWidget(QWidget* parent = 0);

	~USTPTradeWidget();

public slots:

	void doCTPRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
		const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
		const QString& seatId, const QString& userOrderLocalId, const QString& tradeTime);

	void doSgitRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
		const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
		const QString& seatId, const QString& userOrderLocalId, const QString& tradeTime);

private:
	QString getBSText(const char& bs);
	QString getPriceTypeText(const char& priceType);
	QString getOffsetFlagText(const char& offsetFlag);
	QString getHedgeFlagText(const char& hedgeFlag);
	void initConnect();

private:
	QStringList mHeaders;
};
#endif