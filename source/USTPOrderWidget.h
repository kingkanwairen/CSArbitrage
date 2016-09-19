#ifndef USTP_ORDER_WIDGET_H
#define USTP_ORDER_WIDGET_H

#include <QtGui/QTableWidget>
#include <QtGui/QPushButton>

class USTPOrderWidget : public  QTableWidget
{
	Q_OBJECT
public:

	USTPOrderWidget(QWidget* parent = 0);

	~USTPOrderWidget();

public slots:

	void doUpdateOrderShow(const QString& userLocalId, const QString& instrument, const QString& orderStyle, const char& status, const char& bs, const double& price, const int&
		orderVolume, const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedge, const double& orderPrice);

private:
	void addItem(const int& row, const int& colume, const QString& text);
private:
	QString getBSText(const char& bs);
	QString getPriceTypeText(const char& priceType);
	QString getOffsetFlagText(const char& offsetFlag);
	QString getHedgeFlagText(const char& hedgeFlag);
	QString getStatusText(const char& status);
private:
	QStringList mHeaders;
};
#endif