#include "USTPTradeWidget.h"
#include "USTPConfig.h"
#include "USTPCtpLoader.h"

USTPTradeWidget::USTPTradeWidget(QWidget* parent)
:QTreeWidget(parent)
{	
	mHeaders << TRADE_WIDGET_TAB_HEAD_0 << TRADE_WIDGET_TAB_HEAD_1 << TRADE_WIDGET_TAB_HEAD_2 
		<< TRADE_WIDGET_TAB_HEAD_3 << TRADE_WIDGET_TAB_HEAD_4 << TRADE_WIDGET_TAB_HEAD_5 
		<< TRADE_WIDGET_TAB_HEAD_6 << TRADE_WIDGET_TAB_HEAD_7 << TRADE_WIDGET_TAB_HEAD_8;
	setHeaderLabels(mHeaders);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setAlternatingRowColors(true);
	setRootIsDecorated(false);
	setSortingEnabled(true);
	sortItems(0, Qt::DescendingOrder);
	setFont(QFont("Times", 10, QFont::DemiBold));
	initConnect();
}

USTPTradeWidget::~USTPTradeWidget()
{

}

void USTPTradeWidget::initConnect()
{
	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPRtnTrade(const QString&, const QString&, const char&, const int&, const double&,
		const char&, const char&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)),
		this, SLOT(doCTPRtnTrade(const QString&, const QString&, const char&, const int&, const double&,
		const char&, const char&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)));

	connect(USTPCtpLoader::getUserBTradeSpi(), SIGNAL(onSgitRtnTrade(const QString&, const QString&, const char&, const int&, const double&,
		const char&, const char&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)),
		this, SLOT(doSgitRtnTrade(const QString&, const QString&, const char&, const int&, const double&,
		const char&, const char&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)));
}

QString USTPTradeWidget::getBSText(const char& bs)
{	
	QString rtnBS = "--";
	if (bs == '0'){
		rtnBS = QString(tr("买"));
	}else{
		rtnBS = QString(tr("卖"));
	}
	return rtnBS;
}

QString USTPTradeWidget::getPriceTypeText(const char& priceType)
{
	QString rtnType = "--";
	if (priceType == '1'){
		rtnType = QString(tr("市价"));
	}else if (priceType == '2'){
		rtnType = QString(tr("限价"));
	}
	return rtnType;
}

QString USTPTradeWidget::getOffsetFlagText(const char& offsetFlag)
{
	QString rtnOffset = "--";
	if (offsetFlag == '0'){
		rtnOffset = QString(tr("开仓"));
	}else if (offsetFlag == '1'){
		rtnOffset = QString(tr("平仓"));
	}else if (offsetFlag == '2'){
		rtnOffset = QString(tr("强平"));
	}else if (offsetFlag == '3'){
		rtnOffset = QString(tr("平今"));
	}else if (offsetFlag == '4'){
		rtnOffset = QString(tr("平昨"));
	}
	return rtnOffset;
}

QString USTPTradeWidget::getHedgeFlagText(const char& hedgeFlag)
{
	QString rtnHedge = "--";
	if (hedgeFlag == '1'){
		rtnHedge = QString(tr("投机"));
	}else if (hedgeFlag == '2'){
		rtnHedge = QString(tr("套利"));
	}else if (hedgeFlag == '3'){
		rtnHedge = QString(tr("套保"));
	}
	return rtnHedge;
}


void USTPTradeWidget::doCTPRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
									 const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
									 const QString& seatId, const QString& userOrderLocalId, const QString& tradeTime)
{	
	QTreeWidgetItem *showItem = new QTreeWidgetItem(this);
	showItem->setText(0, tradeTime);
	showItem->setText(1, investorId);
	showItem->setText(2, instrumentId);
	showItem->setText(3, getBSText(direction));
	showItem->setText(4, getOffsetFlagText(offsetFlag));
	showItem->setText(5, QString::number(tradeVolume));
	showItem->setText(6, QString::number(tradePrice));
	showItem->setText(7, getHedgeFlagText(hedgeFlag));
	showItem->setText(8, tradeId);
	insertTopLevelItem(0, showItem);
}

void USTPTradeWidget::doSgitRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
					const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
					const QString& seatId, const QString& userOrderLocalId, const QString& tradeTime)
{
	QTreeWidgetItem *showItem = new QTreeWidgetItem(this);
	showItem->setText(0, tradeTime);
	showItem->setText(1, investorId);
	showItem->setText(2, instrumentId);
	showItem->setText(3, getBSText(direction));
	showItem->setText(4, getOffsetFlagText(offsetFlag));
	showItem->setText(5, QString::number(tradeVolume));
	showItem->setText(6, QString::number(tradePrice));
	showItem->setText(7, getHedgeFlagText(hedgeFlag));
	showItem->setText(8, tradeId);
	insertTopLevelItem(0, showItem);
}

#include "moc_USTPTradeWidget.cpp"