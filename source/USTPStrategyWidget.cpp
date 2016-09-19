#include "USTPStrategyWidget.h"
#include "USTPConfig.h"
#include <QtGui/QSpinBox>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>
#include "USTPCtpLoader.h"
#include "USTPLogger.h"
#include "USTPMutexId.h"
#include "USTPProfile.h"


USTPStrategyWidget::USTPStrategyWidget(const int& index, USTPMarketWidget* pMarketWidget, USTPOrderWidget* pOrderWidget,
									   USTPCancelWidget* pCancelWidget, USTPTabWidget* pTabWidget, QWidget *parent) :
QTableWidget(parent)
{		
	mCurrentIndex = -1;
	mCurrentTabIndex = index;
	mOrderWidget = pOrderWidget;
	mCancelWidget = pCancelWidget;
	setColumnCount(STRATEGYHEAD_LENGTH);
	QStringList headers;
	headers << STRATEGY_WIDGET_TAB_HEAD_0 << STRATEGY_WIDGET_TAB_HEAD_1<< STRATEGY_WIDGET_TAB_HEAD_2 << STRATEGY_WIDGET_TAB_HEAD_3 << STRATEGY_WIDGET_TAB_HEAD_4
		<< STRATEGY_WIDGET_TAB_HEAD_5 << STRATEGY_WIDGET_TAB_HEAD_6 << STRATEGY_WIDGET_TAB_HEAD_7 << STRATEGY_WIDGET_TAB_HEAD_8 << STRATEGY_WIDGET_TAB_HEAD_9;
	setHorizontalHeaderLabels(headers);
	verticalHeader()->setVisible(false);
	horizontalHeader()->setMovable(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setEditTriggers(QAbstractItemView::NoEditTriggers); 
	setShowGrid(true);
	initConnect(pTabWidget, pMarketWidget, parent);
	setMinimumWidth(MARKET_WINDOW_MIN_WIDTH);
}

USTPStrategyWidget::~USTPStrategyWidget()
{	
}

void USTPStrategyWidget::initConnect(USTPTabWidget* pTabWidget, USTPMarketWidget* pMarketWidget, QWidget* pWidget)
{	
	connect(pTabWidget, SIGNAL(onTabIndexChanged(int)), this, SLOT(doTabIndexChanged(int)));
	connect(this, SIGNAL(onGetComplexMarketPrice(const QString&, const QString&, const int&)), pMarketWidget, SLOT(doGetComplexMarketPrice(const QString&, const QString&, const int&)));
	connect(pMarketWidget, SIGNAL(onUpdateMarketInsPrice(const QString&, const QString&, const int&, const double&, const double&, const double&, const double&)),
		this, SLOT(doUpdateMarketInsPrice(const QString&, const QString&, const int&, const double&, const double&, const double&, const double&)));

	connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(doCellClicked(int, int)));
}

bool USTPStrategyWidget::loadList(const QString& name)
{	
	QStringList orderList = name.split(",");
	for(int nIndex = 0; nIndex < orderList.size(); nIndex++){
		QString eachList = orderList.at(nIndex);
		QStringList cellItems = eachList.split("|");
		if(cellItems.size() < SAVE_ITEM_LENGTH)
			continue;
		doCreateNewOrder(cellItems.at(0), cellItems.at(1), cellItems.at(2), cellItems.at(3), cellItems.at(4), cellItems.at(9), cellItems.at(8).toDouble(), cellItems.at(5).toInt(), 
			cellItems.at(6).toInt(), cellItems.at(7).toInt());
	}
	return true;
}

void  USTPStrategyWidget::doCreateNewOrder(const QString& firstIns, const QString& secIns, const QString& orderStyle, const QString& direction, const QString& offsetFlag, 
										   const QString& hedgeFlag, const double& priceTick, const int& ctpOrderQty, const int& sgitOrderQty, const int& cycleNum)
{
	int nRow = rowCount();
	insertRow(nRow);
	addItem(nRow, 0, firstIns);
	addItem(nRow, 1, secIns);

	QStringList list;
	list << tr("0-条件套利");
	QComboBox* priceTypeCombox = createComboBox(list);
	int nType = priceTypeCombox->findText(orderStyle);
	priceTypeCombox->setCurrentIndex(nType);
	setCellWidget(nRow, 2, priceTypeCombox);


	QStringList bsList;
	bsList << tr("0-买入") << tr("1-卖出");
	QComboBox* bsCombox = createComboBox(bsList);
	bsCombox->setItemIcon(0, QIcon("../image/red.png"));
	bsCombox->setItemIcon(1, QIcon("../image/green.png"));
	int nBS = bsCombox->findText(direction);
	bsCombox->setCurrentIndex(nBS);
	setCellWidget(nRow, 3, bsCombox);

	QStringList offsetList;
	offsetList << tr("0-开仓") << tr("1-平仓") << tr("2-平今") << tr("3-平昨");
	QComboBox* offsetCombox = createComboBox(offsetList);
	offsetCombox->setItemIcon(0, QIcon("../image/red.png"));
	offsetCombox->setItemIcon(1, QIcon("../image/green.png"));
	offsetCombox->setItemIcon(2, QIcon("../image/green.png"));
	offsetCombox->setItemIcon(3, QIcon("../image/green.png"));
	int nOffset = offsetCombox->findText(offsetFlag);
	offsetCombox->setCurrentIndex(nOffset);
	setCellWidget(nRow, 4, offsetCombox);

	QSpinBox *ctpQtySpinBox = new QSpinBox();
	ctpQtySpinBox->setRange(1, 1000); 
	ctpQtySpinBox->setSingleStep(1);
	ctpQtySpinBox->setValue(ctpOrderQty);
	setCellWidget(nRow, 5, ctpQtySpinBox);

	QSpinBox *sgitQtySpinBox = new QSpinBox();
	sgitQtySpinBox->setRange(1, 1000); 
	sgitQtySpinBox->setSingleStep(1);
	sgitQtySpinBox->setValue(sgitOrderQty);
	setCellWidget(nRow, 6, sgitQtySpinBox);

	QDoubleSpinBox *priceSpinBox = new QDoubleSpinBox();
	priceSpinBox->setRange(-10000.0, 10000.0); 
	double tick = USTPMutexId::getInsPriceTick(firstIns);
	priceSpinBox->setDecimals(getPrcision(tick));
	priceSpinBox->setSingleStep(tick);
	priceSpinBox->setValue(priceTick);
	setCellWidget(nRow, 7, priceSpinBox);

	QSpinBox *cycleSpinBox = new QSpinBox();
	cycleSpinBox->setRange(1, 1000); 
	cycleSpinBox->setSingleStep(1);
	cycleSpinBox->setValue(cycleNum);
	setCellWidget(nRow, 8, cycleSpinBox);

	QPushButton* pOrderButton = createButton(tr("提 交"), SLOT(doSubmitOrder()));
	setCellWidget(nRow, 9, pOrderButton);

	QStringList hedgeList;
	hedgeList << tr("0-投机") << tr("1-套利");
	QComboBox* hedgeCombox = createComboBox(hedgeList);
	int nHedge = hedgeCombox->findText(hedgeFlag);
	hedgeCombox->setCurrentIndex(nHedge);
	setCellWidget(nRow, 10, hedgeCombox);

	QPushButton* pCancelBtn = createButton(tr("删 除"), SLOT(doBtnTableDeleteClicked()));
	setCellWidget(nRow, 11, pCancelBtn);
}

void USTPStrategyWidget::addItem(const int& row, const int& colume, const QString& text)
{
	QTableWidgetItem* item = new QTableWidgetItem(text);
	setItem(row, colume, item);
}

void USTPStrategyWidget::createNewOrder(const int& row, const double& firstBidPrice, const double& firstAskPrice, const double& secBidPrice, const double& secAskPrice)
{
	QString firstName = item(row, 0)->text();
	QString secName = item(row, 1)->text();

	QComboBox* orderTypeCombox = qobject_cast<QComboBox *>(cellWidget(row, 2));
	QStringList items = orderTypeCombox->currentText().split("-");
	int orderType = items.at(0).toInt();
	QString typeName = items.at(1);

	QComboBox* bsCombox = qobject_cast<QComboBox *>(cellWidget(row, 3));
	QStringList bsItems = bsCombox->currentText().split("-");
	char direction = (bsItems.at(0) == QString(tr("0"))) ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;

	QComboBox* offsetCombox = qobject_cast<QComboBox *>(cellWidget(row, 4));
	QStringList offsetItems = offsetCombox->currentText().split("-");
	int selOffsetFlag = offsetItems.at(0).toInt();
	char offstFlag;
	switch (selOffsetFlag)
	{
	case 0:
		offstFlag = THOST_FTDC_OF_Open;
		break;
	case 1:
		offstFlag = THOST_FTDC_OF_Close;
		break;
	case 2:
		offstFlag = THOST_FTDC_OF_CloseToday;
		break;
	case 3:
		offstFlag = THOST_FTDC_OF_CloseYesterday;
		break;
	default:
		offstFlag = THOST_FTDC_OF_Open;
		break;
	}

	QSpinBox* ctpQtySpin = qobject_cast<QSpinBox *>(cellWidget(row, 5));
	int ctpQty = ctpQtySpin->value();

	QSpinBox* sgitQtySpin = qobject_cast<QSpinBox *>(cellWidget(row, 6));
	int sgitQty = sgitQtySpin->value();

	QDoubleSpinBox* priceSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 7));
	double price = priceSpin->value();

	QSpinBox* cycleSpin = qobject_cast<QSpinBox *>(cellWidget(row, 8));
	int cycleNum = cycleSpin->value();

	QComboBox* hedgeCombox = qobject_cast<QComboBox *>(cellWidget(row, 10));
	QStringList hedgeItems = hedgeCombox->currentText().split("-");
	char hedgeFlag = (hedgeItems.at(0) == tr("0")) ? THOST_FTDC_HF_Speculation : THOST_FTDC_HF_Arbitrage;

	createStrategy(orderType, firstName, secName, price, ctpQty, sgitQty, cycleNum, direction, offstFlag, hedgeFlag, firstBidPrice, firstAskPrice, secBidPrice, secAskPrice);
}

void USTPStrategyWidget::doSubmitOrder()
{
	QPushButton * senderObj = qobject_cast<QPushButton *>(sender());
	if(senderObj == 0)
		return;
	QModelIndex index = indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
	int selRow = index.row();

	QString firstName = item(selRow, 0)->text();
	QString secName = item(selRow, 1)->text();
	QString complexIns = firstName + tr("|") + secName;
	mOrderKey = complexIns + tr("_") + QString::number(USTPMutexId::getMarketIndex());
	emit onGetComplexMarketPrice(complexIns, mOrderKey, selRow);
}


void USTPStrategyWidget::createStrategy(const int& orderType, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& ctpQty, const int& sgitQty, 
										const int& cycleNum, const char& bs,  const char& offset, const char& hedge, const double& firstBidPrice, const double& firstAskPrice,
										const double& secBidPrice, const double& secAskPrice)
{
	switch (orderType){
		case 0:
			{
				QString label = tr("[条件套利]") + QString::number(USTPMutexId::getNewOrderIndex());
				USTPStrategyBase* pStrategy = new USTPConditionArbitrage(label, firstIns, secIns, orderPriceTick, ctpQty, sgitQty, cycleNum, bs, offset, hedge, firstBidPrice, firstAskPrice, 
					secBidPrice, secAskPrice, mOrderWidget, mCancelWidget, this);
				mUserStrategys.insert(label, pStrategy);
			}
			break;
	}
}

void USTPStrategyWidget::doBtnTableDeleteClicked()
{
	QPushButton * senderObj = qobject_cast<QPushButton *>(sender());
	if(senderObj == 0)
		return;
	QModelIndex index = indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
	int row = index.row();
	int nColumeLen = columnCount();
	for(int nIndex = 0; nIndex < nColumeLen; nIndex++){
		QTableWidgetItem* showItem = takeItem(row, nIndex);
		delete showItem;
		showItem = NULL;
	}
	removeRow(row);
}

QPushButton *USTPStrategyWidget::createButton(const QString &text, const char *member)
{
	QPushButton *button = new QPushButton(text);
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}


QComboBox* USTPStrategyWidget::createComboBox(const QStringList &itemList)
{
	QComboBox *comboBox = new QComboBox;
	foreach(QString item, itemList){
		comboBox->addItem(item);
	}
	comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	return comboBox;
}


void USTPStrategyWidget::doOrderFinished(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset, const char& hedge,
										 bool isAllTraded, const int& orderType, const double& firstMarketBidPrice, const double& firstMarketAskPrice, const double& secMarketBidPrice, const double& secMarketAskPrice)
{
	if(mUserStrategys.find(orderLabel) != mUserStrategys.end()){
		USTPStrategyBase* pStrategy = mUserStrategys[orderLabel];
		delete pStrategy;
		pStrategy = NULL;
		mUserStrategys.remove(orderLabel);
		USTPLogger::saveData(QString(tr("[Del Order]  ")) + orderLabel);
		//////////////////////////////////////////////////////////////////////////
		///创建新的循环策略
		//////////////////////////////////////////////////////////////////////////
		//if(isAllTraded && isCycle){
		//	if((THOST_FTDC_D_Buy == bs) && (THOST_FTDC_OF_Open == offset)){
		//		double nextOrderPrice = orderPriceTick + cycleStall * USTPMutexId::getInsPriceTick(firstIns);
		//		QString exh = USTPMutexId::getInsExchangeId(firstIns);
		//		char nextOffsetFlag = (exh == EXH_SHFE) ? THOST_FTDC_OF_CloseToday : THOST_FTDC_OF_Close;
		//		createStrategy(orderType, firstIns, secIns, nextOrderPrice, firstPriceSlipPoint, secPriceSlipPoint, qty, THOST_FTDC_D_Sell, nextOffsetFlag, hedge, 
		//			cancelFirstTime, cancelSecTime, cycleStall, actionReferNum, actionSuperNum, isAutoFirstCancel, isAutoSecCancel, isCycle, isOppentPrice,
		//			isDefineOrder, isReferTick, firstMarketBidPrice, firstMarketAskPrice, secMarketBidPrice, secMarketAskPrice);
		//	}else if ((THOST_FTDC_D_Sell == bs) && (THOST_FTDC_OF_Open == offset)){
		//		double nextOrderPrice = orderPriceTick - cycleStall * USTPMutexId::getInsPriceTick(firstIns);
		//		QString exh = USTPMutexId::getInsExchangeId(firstIns);
		//		char nextOffsetFlag = (exh == EXH_SHFE) ? THOST_FTDC_OF_CloseToday : THOST_FTDC_OF_Close;
		//		createStrategy(orderType, firstIns, secIns, nextOrderPrice, firstPriceSlipPoint, secPriceSlipPoint, qty, THOST_FTDC_D_Buy, nextOffsetFlag, hedge, 
		//			cancelFirstTime, cancelSecTime, cycleStall, actionReferNum, actionSuperNum, isAutoFirstCancel, isAutoSecCancel, isCycle, isOppentPrice,
		//			isDefineOrder, isReferTick, firstMarketBidPrice, firstMarketAskPrice, secMarketBidPrice, secMarketAskPrice);
		//	}else if ((THOST_FTDC_D_Buy == bs) && (THOST_FTDC_OF_Close == offset || THOST_FTDC_OF_CloseToday == offset || THOST_FTDC_OF_CloseYesterday == offset)){
		//		double nextOrderPrice = orderPriceTick + cycleStall * USTPMutexId::getInsPriceTick(firstIns);
		//		createStrategy(orderType, firstIns, secIns, nextOrderPrice, firstPriceSlipPoint, secPriceSlipPoint, qty, THOST_FTDC_D_Sell, THOST_FTDC_OF_Open, hedge, 
		//			cancelFirstTime, cancelSecTime, cycleStall, actionReferNum, actionSuperNum, isAutoFirstCancel, isAutoSecCancel, isCycle, isOppentPrice,
		//			isDefineOrder, isReferTick, firstMarketBidPrice, firstMarketAskPrice, secMarketBidPrice, secMarketAskPrice);
		//	}else if ((THOST_FTDC_D_Sell == bs) && (THOST_FTDC_OF_Close == offset || THOST_FTDC_OF_CloseToday == offset || THOST_FTDC_OF_CloseYesterday == offset)){
		//		double nextOrderPrice = orderPriceTick - cycleStall * USTPMutexId::getInsPriceTick(firstIns);
		//		createStrategy(orderType, firstIns, secIns, nextOrderPrice, firstPriceSlipPoint, secPriceSlipPoint, qty, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, hedge, 
		//			cancelFirstTime, cancelSecTime, cycleStall, actionReferNum, actionSuperNum, isAutoFirstCancel, isAutoSecCancel, isCycle, isOppentPrice,
		//			isDefineOrder, isReferTick, firstMarketBidPrice, firstMarketAskPrice, secMarketBidPrice, secMarketAskPrice);
		//	}

		//}
	}
}

QString USTPStrategyWidget::getList()
{
	int nRowLen = rowCount();
	QString szSaveData;
	for(int row = 0; row < nRowLen; row++){
		QString firstName = item(row, 0)->text();
		QString secName = item(row, 1)->text();

		QComboBox* orderTypeCombox = qobject_cast<QComboBox *>(cellWidget(row, 2));
		QString orderType = orderTypeCombox->currentText();

		QComboBox* bsCombox = qobject_cast<QComboBox *>(cellWidget(row, 3));
		QString direction = bsCombox->currentText();

		QComboBox* offsetCombox = qobject_cast<QComboBox *>(cellWidget(row, 4));
		QString offstFlag = offsetCombox->currentText();

		QSpinBox* ctpQtySpin = qobject_cast<QSpinBox *>(cellWidget(row, 5));
		int ctpQty = ctpQtySpin->value();

		QSpinBox* sgitQtySpin = qobject_cast<QSpinBox *>(cellWidget(row, 6));
		int sgitQty = sgitQtySpin->value();

		QDoubleSpinBox* priceSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 7));
		double price = priceSpin->value();

		QSpinBox* cycleSpin = qobject_cast<QSpinBox *>(cellWidget(row, 8));
		int cycleNum = cycleSpin->value();

		QComboBox* hedgeCombox = qobject_cast<QComboBox *>(cellWidget(row, 9));
		QString hedgeFlag = hedgeCombox->currentText();
		//保存数据到文件
		QString szLine = firstName + QString(tr("|")) + secName + QString(tr("|")) + orderType + QString(tr("|")) + direction + QString(tr("|")) + 
			offstFlag + QString(tr("|")) + QString::number(ctpQty) + QString(tr("|")) + QString::number(sgitQty) + QString(tr("|")) + QString::number(cycleNum) + 
			QString(tr("|")) + QString::number(price, 'g', 3) + QString(tr("|")) + hedgeFlag;
		if(row == 0)
			szSaveData = szLine;
		else
			szSaveData = szSaveData + QString(tr(",")) + szLine;
	}
	return szSaveData;
}


int USTPStrategyWidget::getPrcision(const double& value)
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

void USTPStrategyWidget::doCellClicked(int row, int column)
{
	mCurrentIndex = row;
}


void USTPStrategyWidget::doTabIndexChanged(int tabIndex)
{	
	if(mCurrentTabIndex == tabIndex){
		mCancelWidget->installEventFilter(this);
	}else{
		mCancelWidget->removeEventFilter(this);
	}
}

void USTPStrategyWidget::doUpdateMarketInsPrice(const QString& complexIns, const QString& reqKey, const int& rowId, const double& firstMarketBidPrice, const double& firstMarketAskPrice,
												const double& secMarketBidPrice, const double& secMarketAskPrice)
{
	if(mOrderKey == reqKey)
		createNewOrder(rowId, firstMarketBidPrice, firstMarketAskPrice, secMarketBidPrice, secMarketAskPrice);

}

#include "moc_USTPStrategyWidget.cpp"