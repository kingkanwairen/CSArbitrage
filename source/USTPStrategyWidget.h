#ifndef USTP_STRATEGY_WIDGET_H
#define USTP_STRATEGY_WIDGET_H

#include <QtGui/QTableWidget>
#include <QtGui/QComboBox>
#include <QtCore/QMap>
#include "USTPUserStrategy.h"
#include "USTPOrderWidget.h"
#include "USTPCancelWidget.h"
#include "USTPMarketWidget.h"
#include "USTPTabWidget.h"

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class USTPStrategyWidget : public QTableWidget
{
	Q_OBJECT
public:
	USTPStrategyWidget(const int& index, USTPMarketWidget* pMarketWidget, USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget, USTPTabWidget* pTabWidget, QWidget* parent = 0);

	~USTPStrategyWidget();
signals:
	void onGetComplexMarketPrice(const QString& complexIns, const QString& reqKey, const int& rowIndex);

	public slots:
		void doCreateNewOrder(const QString& firstIns, const QString& secIns, const QString& orderStyle, const QString& direction, const QString& offsetFlag, 
			const QString& hedgeFlag, const double& priceTick, const int& ctpOrderQty, const int& sgitOrderQty, const int& cycleNum);

		void doSubmitOrder();

		void doBtnTableDeleteClicked();

		void doOrderFinished(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset, const char& hedge,
			bool isAllTraded, const int& orderType, const double& firstMarketBidPrice, const double& firstMarketAskPrice, const double& secMarketBidPrice, const double& secMarketAskPrice);

		void doCellClicked(int row, int column);

		void doTabIndexChanged(int tabIndex);

		void doUpdateMarketInsPrice(const QString& complexIns, const QString& reqKey, const int& rowId, const double& firstMarketBidPrice, const double& firstMarketAskPrice,
			const double& secMarketBidPrice, const double& secMarketAskPrice);


public:

	bool loadList(const QString& name);

	QString getList();

private:

	void initConnect(USTPTabWidget* pTabWidget, USTPMarketWidget* pMarketWidget, QWidget* pWidget);

	QPushButton* createButton(const QString &text, const char *member);

	QComboBox* createComboBox(const QStringList &itemList);

	void addItem(const int& row, const int& colume, const QString& text);

	int getPrcision(const double& value);

	void createNewOrder(const int& row, const double& firstBidPrice, const double& firstAskPrice, const double& secBidPrice, const double& secAskPrice);

	void createStrategy(const int& orderType, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& ctpQty, const int& sgitQty, 
		const int& cycleNum, const char& bs,  const char& offset, const char& hedge, const double& firstBidPrice, const double& firstAskPrice,
		const double& secBidPrice, const double& secAskPrice);


private:
	QMap<QString, USTPStrategyBase*> mUserStrategys;
	USTPOrderWidget* mOrderWidget;
	USTPCancelWidget* mCancelWidget;
	USTPMarketWidget* mMarketWidget;
	int mCurrentIndex;
	QString mMarketKey;
	QString mOrderKey;
	int mCurrentTabIndex;
};

#endif