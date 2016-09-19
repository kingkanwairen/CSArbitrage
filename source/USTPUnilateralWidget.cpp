#include "USTPUnilateralWidget.h"
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


USTPUnilateralWidget::USTPUnilateralWidget(USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget, USTPSpeMarketWidget* pSpeMarketWidget, QWidget *parent) :
QTableWidget(parent)
{	
	mTradeBidPrice = 0.0;
	mTradeAskPrice = 0.0;
	mReferBidPrice = 0.0;
	mReferAskPrice = 0.0;
	mOrderWidget = pOrderWidget;
	mCancelWidget = pCancelWidget;
	setColumnCount(UNILATERAL_HEAD_LENGTH);
	QStringList headers;
	headers << UNILATERAL_WIDGET_TAB_HEAD_0 << UNILATERAL_WIDGET_TAB_HEAD_1<< UNILATERAL_WIDGET_TAB_HEAD_2 << UNILATERAL_WIDGET_TAB_HEAD_3<< UNILATERAL_WIDGET_TAB_HEAD_4
		<< UNILATERAL_WIDGET_TAB_HEAD_5 << UNILATERAL_WIDGET_TAB_HEAD_6 << UNILATERAL_WIDGET_TAB_HEAD_7 << UNILATERAL_WIDGET_TAB_HEAD_8 << UNILATERAL_WIDGET_TAB_HEAD_9
		<< UNILATERAL_WIDGET_TAB_HEAD_10 << UNILATERAL_WIDGET_TAB_HEAD_11 << UNILATERAL_WIDGET_TAB_HEAD_12<< UNILATERAL_WIDGET_TAB_HEAD_13 << UNILATERAL_WIDGET_TAB_HEAD_14
		<< UNILATERAL_WIDGET_TAB_HEAD_15;
	setHorizontalHeaderLabels(headers);
	verticalHeader()->setVisible(false);
	horizontalHeader()->setMovable(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setEditTriggers(QAbstractItemView::NoEditTriggers); 
	setShowGrid(true);
	initConnect(pSpeMarketWidget, this);
	setMinimumWidth(MARKET_WINDOW_MIN_WIDTH);
}

USTPUnilateralWidget::~USTPUnilateralWidget()
{	
}

void USTPUnilateralWidget::initConnect(USTPSpeMarketWidget* pSpeMarketWidget, QWidget* pWidget)
{	
	connect(this, SIGNAL(onGetUnilateralInsPrice(const QString&, const QString&, const int&)), pSpeMarketWidget, SLOT(doGetUnilateralInsPrice(const QString&, const QString&, const int&)));
	connect(pSpeMarketWidget, SIGNAL(onUpdateUnilateralPrice(const QString&, const QString&, const int&, const double&, const double&)), this, SLOT(doUpdateUnilateralPrice(const QString&,
		const QString&, const int&, const double&, const double&)));
}

bool USTPUnilateralWidget::loadList(const QString& name)
{	
	QStringList orderList = name.split(",");
	for(int nIndex = 0; nIndex < orderList.size(); nIndex++){
		QString eachList = orderList.at(nIndex);
		QStringList cellItems = eachList.split("|");
		if(cellItems.size() < SAVE_UNILATERAL_LENGTH)
			continue;
		doCreateUnilateralOrder(cellItems.at(0), cellItems.at(1), cellItems.at(2), cellItems.at(3), cellItems.at(4), cellItems.at(5).toDouble(), cellItems.at(6).toDouble(), cellItems.at(7).toDouble(),
			cellItems.at(8).toDouble(), cellItems.at(9).toInt(), cellItems.at(10).toInt(), cellItems.at(11).toInt(), cellItems.at(12).toInt(), cellItems.at(13).toInt());
	}
	return true;
}

void  USTPUnilateralWidget::doCreateUnilateralOrder(const QString& instrumentId, const QString& referInsId, const QString& orderStyle, const QString& direction, const QString& offsetFlag, const double& openPriceSpread, 
													const double& closePriceSpread, const double& opponentPriceSpread, const double& referPriceSpread, const int& orderQty, const int& actionTimeNum, const int& superPriceStall,
													bool isActionTime, bool isCylce)
{
	int nRow = rowCount();
	insertRow(nRow);
	addItem(nRow, 0, instrumentId);
	
	addItem(nRow, 1, referInsId);

	QStringList list;
	list << tr("0-开开") << tr("1-平平") << tr("2-开平") << tr("3-平开");
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
	offsetList << tr("0-平仓") << tr("1-平今") << tr("2-平昨");
	QComboBox* offsetCombox = createComboBox(offsetList);
	offsetCombox->setItemIcon(0, QIcon("../image/green.png"));
	offsetCombox->setItemIcon(1, QIcon("../image/green.png"));
	offsetCombox->setItemIcon(2, QIcon("../image/green.png"));
	int nOffset = offsetCombox->findText(offsetFlag);
	offsetCombox->setCurrentIndex(nOffset);
	setCellWidget(nRow, 4, offsetCombox);

	QSpinBox *qtySpinBox = new QSpinBox();
	qtySpinBox->setRange(1, 1000); 
	qtySpinBox->setSingleStep(1);
	qtySpinBox->setValue(orderQty);
	setCellWidget(nRow, 5, qtySpinBox);

	QDoubleSpinBox *openSpinBox = new QDoubleSpinBox();
	openSpinBox->setRange(0.0, 10000.0); 
	double tick = USTPMutexId::getInsPriceTick(instrumentId);
	openSpinBox->setDecimals(getPrcision(tick));
	openSpinBox->setSingleStep(tick);
	openSpinBox->setValue(openPriceSpread);
	setCellWidget(nRow, 6, openSpinBox);

	QPushButton* pOrderButton = createButton(tr("提 交"), SLOT(doSubmitOrder()));
	setCellWidget(nRow, 7, pOrderButton);

	QDoubleSpinBox *closeSpinBox = new QDoubleSpinBox();
	closeSpinBox->setRange(-10000.0, 10000.0); 
	closeSpinBox->setSingleStep(tick);
	closeSpinBox->setValue(closePriceSpread);
	setCellWidget(nRow, 8, closeSpinBox);

	QDoubleSpinBox *opponentSpinBox = new QDoubleSpinBox();
	opponentSpinBox->setRange(-10000.0, 10000.0); 
	opponentSpinBox->setSingleStep(tick);
	opponentSpinBox->setValue(closePriceSpread);
	setCellWidget(nRow, 9, opponentSpinBox);

	QDoubleSpinBox *referPriceSpinBox = new QDoubleSpinBox();
	referPriceSpinBox->setRange(-10000.0, 10000.0); 
	referPriceSpinBox->setSingleStep(tick);
	referPriceSpinBox->setValue(referPriceSpread);
	setCellWidget(nRow, 10, referPriceSpinBox);

	QSpinBox *actionTimeSpinBox = new QSpinBox();
	actionTimeSpinBox->setRange(1, 60000); 
	actionTimeSpinBox->setSingleStep(1);
	actionTimeSpinBox->setValue(actionTimeNum);
	setCellWidget(nRow, 11, actionTimeSpinBox);

	QSpinBox *superPriceSpinBox = new QSpinBox();
	superPriceSpinBox->setRange(1, 60000); 
	superPriceSpinBox->setSingleStep(1);
	superPriceSpinBox->setValue(superPriceStall);
	setCellWidget(nRow, 12, superPriceSpinBox);

	QCheckBox *actionTimeCheckBox = new QCheckBox();
	actionTimeCheckBox->setChecked(isActionTime);
	setCellWidget(nRow, 13, actionTimeCheckBox);

	QCheckBox *cycleCheckBox = new QCheckBox();
	cycleCheckBox->setChecked(isCylce);
	setCellWidget(nRow, 14, cycleCheckBox);

	QPushButton* pCancelBtn = createButton(tr("删 除"), SLOT(doBtnTableDeleteClicked()));
	setCellWidget(nRow, 15, pCancelBtn);
}

void USTPUnilateralWidget::addItem(const int& row, const int& colume, const QString& text)
{
	QTableWidgetItem* item = new QTableWidgetItem(text);
	setItem(row, colume, item);
}

void USTPUnilateralWidget::createNewOrder(const int& row, const double& insBidPrice, const double insAskPrice, const double& referBidPrice, const double& referAskPrice)
{
	QString insName = item(row, 0)->text();
	QString referInsName = item(row, 1)->text();
	QComboBox* orderTypeCombox = qobject_cast<QComboBox *>(cellWidget(row, 2));
	QStringList items = orderTypeCombox->currentText().split("-");
	int orderType = items.at(0).toInt();
	QString typeName = items.at(1);

	QComboBox* bsCombox = qobject_cast<QComboBox *>(cellWidget(row, 3));
	QStringList bsItems = bsCombox->currentText().split("-");
	char direction = (bsItems.at(0) == tr("0")) ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;

	QComboBox* offsetCombox = qobject_cast<QComboBox *>(cellWidget(row, 4));
	QStringList offsetItems = offsetCombox->currentText().split("-");
	int selOffsetFlag = offsetItems.at(0).toInt();
	char offstFlag;
	switch (selOffsetFlag)
	{
	case 0:
		offstFlag = THOST_FTDC_OF_Close;
		break;
	case 1:
		offstFlag = THOST_FTDC_OF_CloseToday;
		break;
	case 2:
		offstFlag = THOST_FTDC_OF_CloseYesterday;
		break;
	default:
		offstFlag = THOST_FTDC_OF_Open;
		break;
	}

	QSpinBox* qtySpin = qobject_cast<QSpinBox *>(cellWidget(row, 5));
	int qty = qtySpin->value();

	QDoubleSpinBox* openPriceSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 6));
	double openPrice = openPriceSpin->value();

	QDoubleSpinBox* closePriceSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 8));
	double closePrice = closePriceSpin->value();

	QDoubleSpinBox* opponentPriceSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 9));
	double opponentPrice = opponentPriceSpin->value();

	QDoubleSpinBox* referPriceSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 10));
	double referPrice = referPriceSpin->value();

	QSpinBox* actionTimeSpin = qobject_cast<QSpinBox *>(cellWidget(row, 11));
	int actionTimeNum = actionTimeSpin->value();

	QSpinBox* superPriceSpin = qobject_cast<QSpinBox *>(cellWidget(row, 12));
	int superPriceTick = superPriceSpin->value();

	QCheckBox* actionTimeCheck = qobject_cast<QCheckBox *>(cellWidget(row, 13));
	bool isActionTime = actionTimeCheck->isChecked();

	QCheckBox* cycleCheck = qobject_cast<QCheckBox *>(cellWidget(row, 14));
	bool isCycle = cycleCheck->isChecked();

	QString label = tr("[单边循环]") + QString::number(USTPMutexId::getNewOrderIndex());
	USTPStrategyBase* pStrategy = new USTPUnilateralOrder(label,  insName, referInsName, orderType, direction, offstFlag, openPrice, closePrice, 
	opponentPrice, referPrice, 0.0, insBidPrice, insAskPrice, referBidPrice, referAskPrice, qty, actionTimeNum, superPriceTick, isActionTime, 
	isCycle, true, mOrderWidget, mCancelWidget, this);
	mUSTPUnilateralStrategyMap.insert(label, pStrategy);
}

void USTPUnilateralWidget::doSubmitOrder()
{
	QPushButton * senderObj = qobject_cast<QPushButton *>(sender());
	if(senderObj == 0)
		return;
	QModelIndex index = indexAt(QPoint(senderObj->frameGeometry().x(), senderObj->frameGeometry().y()));
	int selRow = index.row();
	mTradeBidPrice = 0.0;
	mTradeAskPrice = 0.0;
	mReferBidPrice = 0.0;
	mReferAskPrice = 0.0;
	QString insName = item(selRow, 0)->text();
	QString referInsName = item(selRow, 1)->text();
	mTradeInsKey = insName + tr("_") + QString::number(USTPMutexId::getMarketIndex());
	emit onGetUnilateralInsPrice(insName, mTradeInsKey, selRow);
	mReferInsKey = referInsName + tr("_") + QString::number(USTPMutexId::getMarketIndex());
	emit onGetUnilateralInsPrice(referInsName, mReferInsKey, selRow);
	
}

void USTPUnilateralWidget::doBtnTableDeleteClicked()
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

QPushButton *USTPUnilateralWidget::createButton(const QString &text, const char *member)
{
	QPushButton *button = new QPushButton(text);
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}


QComboBox* USTPUnilateralWidget::createComboBox(const QStringList &itemList)
{
	QComboBox *comboBox = new QComboBox;
	foreach(QString item, itemList){
		comboBox->addItem(item);
	}
	comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	return comboBox;
}

void USTPUnilateralWidget::doUnilateralFinished(const QString& orderLabel, const QString& insName, const QString& referIns, const int& orderType, const char& direction, const char& offsetFlag, 
												const double& openPrice, const double& closePrice, const double& opponentPrice, const double& referPrice, const double& orderBasePrice, const double& insBidPrice,
												const double insAskPrice, const double& referBidPrice, const double& referAskPrice, const int& qty, const int& cancelTime, const int& superPriceTick, bool isAutoCancel,
												bool isCycle, bool isFirstIns, bool isAllTraded)
{
	if(mUSTPUnilateralStrategyMap.find(orderLabel) != mUSTPUnilateralStrategyMap.end()){
		USTPStrategyBase* pStrategy = mUSTPUnilateralStrategyMap[orderLabel];
		delete pStrategy;
		pStrategy = NULL;
		mUSTPUnilateralStrategyMap.remove(orderLabel);
		USTPLogger::saveData(QString(tr("[Del Order]  ")) + orderLabel);

		//////////////////////////////////////////////////////////////////////////
		///创建新的循环策略
		//////////////////////////////////////////////////////////////////////////
		if(isAllTraded && isCycle && isFirstIns){
			QString label = tr("[单边循环]") + QString::number(USTPMutexId::getNewOrderIndex());
			char bs = (THOST_FTDC_D_Sell == direction) ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;

			USTPStrategyBase* pStrategy = new USTPUnilateralOrder(label,  insName, referIns, orderType, bs, offsetFlag, closePrice, closePrice, opponentPrice, referPrice, orderBasePrice,
				insBidPrice, insAskPrice, referBidPrice, referAskPrice, qty, cancelTime, superPriceTick, isAutoCancel, isCycle, false, mOrderWidget, mCancelWidget, this);
			mUSTPUnilateralStrategyMap.insert(label, pStrategy);
		}
	}
}

void USTPUnilateralWidget::doUpdateUnilateralPrice(const QString& selIns, const QString& key, const int& indexRow, const double& bidPrice, const double& askPrice)
{
	if(key == mTradeInsKey){
		mTradeBidPrice = bidPrice;
		mTradeAskPrice = askPrice;
	}else if(key == mReferInsKey){
		mReferBidPrice = bidPrice;
		mReferAskPrice = askPrice;
	}
	if(mReferAskPrice > 0.0 && mReferBidPrice > 0.0 && mTradeAskPrice > 0.0 && mTradeBidPrice > 0.0)
		createNewOrder(indexRow, mTradeBidPrice, mTradeAskPrice, mReferBidPrice, mReferAskPrice);
}

QString USTPUnilateralWidget::getList()
{
	int nRowLen = rowCount();
	QString szSaveData;
	for(int row = 0; row < nRowLen; row++){
		QString insName = item(row, 0)->text();
		QString referInsName = item(row, 1)->text();
		QComboBox* orderTypeCombox = qobject_cast<QComboBox *>(cellWidget(row, 2));
		QString orderType = orderTypeCombox->currentText();

		QComboBox* bsCombox = qobject_cast<QComboBox *>(cellWidget(row, 3));
		QString direction = bsCombox->currentText();

		QComboBox* offsetCombox = qobject_cast<QComboBox *>(cellWidget(row, 4));
		QString offstFlag = offsetCombox->currentText();

		QSpinBox* qtySpin = qobject_cast<QSpinBox *>(cellWidget(row, 5));
		int qty = qtySpin->value();

		QDoubleSpinBox* openSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 6));
		double openPrice = openSpin->value();

		QDoubleSpinBox* closeSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 8));
		double closePrice = closeSpin->value();

		QDoubleSpinBox* opponentSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 9));
		double opponentPrice = opponentSpin->value();

		QDoubleSpinBox* referPriceSpin = qobject_cast<QDoubleSpinBox *>(cellWidget(row, 10));
		double referPrice = referPriceSpin->value();

		QSpinBox* actionNumSpin = qobject_cast<QSpinBox *>(cellWidget(row, 11));
		int actionNum = actionNumSpin->value();

		QSpinBox* superPriceSpin = qobject_cast<QSpinBox *>(cellWidget(row, 12));
		int superPriceNum = superPriceSpin->value();

		QCheckBox* actionCheck = qobject_cast<QCheckBox *>(cellWidget(row, 13));
		bool isActionTime = actionCheck->isChecked();

		QCheckBox* cycleCheck = qobject_cast<QCheckBox *>(cellWidget(row, 14));
		bool isCycle = cycleCheck->isChecked();
		//保存数据到文件
		QString szLine = insName + tr("|") + referInsName + tr("|")  + orderType + tr("|") + direction + tr("|") + offstFlag + tr("|") + QString::number(openPrice, 'g', 3) + tr("|") + 
			QString::number(closePrice, 'g', 3) + tr("|") + QString::number(opponentPrice, 'g', 3) + tr("|") + QString::number(referPrice, 'g', 3) + tr("|") + QString::number(qty) + tr("|") + 
			QString::number(actionNum) + tr("|") + QString::number(superPriceNum) + tr("|") + QString::number(isActionTime ? 1 : 0) + tr("|") + QString::number(isCycle ? 1 : 0);
		if(row == 0)
			szSaveData = szLine;
		else
			szSaveData = szSaveData + QString(tr(",")) + szLine;
	}
	return szSaveData;
}

int USTPUnilateralWidget::getPrcision(const double& value)
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


#include "moc_USTPUnilateralWidget.cpp"