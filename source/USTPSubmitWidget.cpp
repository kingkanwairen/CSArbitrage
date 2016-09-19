#include "USTPSubmitWidget.h"
#include <QtGui/QKeyEvent>
#include "USTPCtpLoader.h"
#include "USTPMutexId.h"
#include "USTPConfig.h"


USTPSubmitWidget::USTPSubmitWidget(USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget, 
								   USTPSpeMarketWidget* pSpeMarketWidget, QWidget* parent) : QWidget(parent)
{	
	mOrderWidget = pOrderWidget;
	mCancelWidget = pCancelWidget;
	mSpeMarketWidget = pSpeMarketWidget;
	QFont serifFont("Times", 11, QFont::Bold);
	setFont(serifFont);

	mInsLabel = new QLabel(tr("合约代码"));
	mInsLineEdit = new QLineEdit;
	mInsLabel->setBuddy(mInsLineEdit);

	mDirectionLabel = new QLabel(tr("买/卖"));
	QStringList bsList;
	bsList << tr("0-买入") << tr("1-卖出");
	mBSComboBox = createComboBox(bsList);
	mBSComboBox->setItemIcon(0, QIcon("../image/red.png"));
	mBSComboBox->setItemIcon(1, QIcon("../image/green.png"));
	mDirectionLabel->setBuddy(mBSComboBox);

	mOffsetFlagLabel = new QLabel(tr("开/平"));
	QStringList offsetList;
	offsetList << tr("0-开仓") << tr("1-平仓") << tr("2-平今") << tr("3-平昨");
	mOffsetComboBox = createComboBox(offsetList);
	mOffsetComboBox->setItemIcon(0, QIcon("../image/red.png"));
	mOffsetComboBox->setItemIcon(1, QIcon("../image/green.png"));
	mOffsetComboBox->setItemIcon(2, QIcon("../image/green.png"));
	mOffsetComboBox->setItemIcon(3, QIcon("../image/green.png"));
	mOffsetFlagLabel->setBuddy(mOffsetComboBox);
	
	mVolumeLabel = new QLabel(tr("委托数量"));
	mVolumeSpinBox = new QSpinBox;
	mVolumeSpinBox->setRange(1, 10000);
	mVolumeSpinBox->setSingleStep(1);
	mVolumeSpinBox->setValue(1);
	mVolumeLabel->setBuddy(mVolumeSpinBox);
	
	mPriceLabel = new QLabel(tr("委托价格"));
	mPriceSpinBox = new QDoubleSpinBox;
	mPriceSpinBox->setRange(0.0, 100000.0);
	mPriceSpinBox->setSingleStep(1.0);
	mPriceSpinBox->setValue(1000.0);
	mPriceLabel->setBuddy(mPriceSpinBox);

	mConfirmBtn = createButton(tr("提 交"), SLOT(createOrder()));

	mGridLayout = new QGridLayout;
	mGridLayout->addWidget(mInsLabel, 0, 1, 1, 1);	
	mGridLayout->addWidget(mDirectionLabel, 0, 2, 1, 1);
	mGridLayout->addWidget(mOffsetFlagLabel, 0, 3, 1, 1);
	mGridLayout->addWidget(mVolumeLabel, 0, 4, 1, 1);
	mGridLayout->addWidget(mPriceLabel, 0, 5, 1, 1);

	mGridLayout->addWidget(mInsLineEdit, 1, 1, 1, 1);
	mGridLayout->addWidget(mBSComboBox, 1, 2, 1, 1);
	mGridLayout->addWidget(mOffsetComboBox, 1, 3, 1, 1);
	mGridLayout->addWidget(mVolumeSpinBox, 1, 4, 1, 1);
	mGridLayout->addWidget(mPriceSpinBox, 1, 5, 1, 1);
	mGridLayout->addWidget(mConfirmBtn, 1, 6, 1, 1);
	setLayout(mGridLayout);
	initConnect(pSpeMarketWidget, parent);
}

USTPSubmitWidget::~USTPSubmitWidget()
{
	foreach (USTPStrategyBase* pStrategy, mUSTPSpeStrategyMap){
		delete pStrategy;
		pStrategy = NULL;
	}
	mUSTPSpeStrategyMap.clear();
}

void USTPSubmitWidget::initConnect(USTPSpeMarketWidget* pSpeMarketWidget, QWidget* pWidget)
{	
	connect(this, SIGNAL(onGetSpeInsPrice(const QString&, const int&)), pSpeMarketWidget, SLOT(doGetSpeInsPrice(const QString&, const int&)));
	connect(pSpeMarketWidget, SIGNAL(onClickSelIns(const QString&, const int&, const double&)), this, SLOT(doClickSelIns(const QString&, const int&, const double&)));
	connect(pSpeMarketWidget, SIGNAL(onUpdateSpePrice(const QString&, const int&, const double&)), this, SLOT(doUpdateSpePrice(const QString&, const int&, const double&)));
}

QComboBox* USTPSubmitWidget::createComboBox(const QStringList &itemList)
{
	QComboBox *comboBox = new QComboBox;
	foreach(QString item, itemList){
		comboBox->addItem(item);
	}
	comboBox->setEditable(true);
	comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	return comboBox;
}

QPushButton *USTPSubmitWidget::createButton(const QString &text, const char *member)
{
	QPushButton *button = new QPushButton(text);
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}

void USTPSubmitWidget::createOrder()
{	
	QString insId = mInsLineEdit->text();
	if(insId == "")
		return;

	QStringList items = insId.split("|");
	QString severType = items.at(0);
	QString insName = items.at(1);
	if(severType == CTP_SERVER)
		submitCtpOrder(insName);
	else
		submitSgitOrder(insName);
		
}

void USTPSubmitWidget::submitCtpOrder(const	QString& selIns)
{
	QString label = tr("CTP|") + QString::number(USTPMutexId::getNewOrderIndex());
	QStringList bsItems = mBSComboBox->currentText().split("-");
	char direction = (bsItems.at(0) == tr("0")) ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	QStringList offsetItems = mOffsetComboBox->currentText().split("-");
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
	int qty = mVolumeSpinBox->value();
	double price = mPriceSpinBox->value();
	QString speLabel;
	if(THOST_FTDC_D_Buy == direction)
		speLabel = tr("CTP|") + selIns + tr("_B");
	else
		speLabel = tr("CTP|") + selIns + tr("_S");
	emit onSubmitOrder(label, speLabel, selIns, direction, offstFlag, qty, price, false);
}

void USTPSubmitWidget::submitSgitOrder(const QString& selIns)
{
	QString label = tr("SGIT|") + QString::number(USTPMutexId::getNewOrderIndex());
	QStringList bsItems = mBSComboBox->currentText().split("-");
	char direction = (bsItems.at(0) == tr("0")) ? Sgit_FTDC_D_Buy : Sgit_FTDC_D_Sell;
	QStringList offsetItems = mOffsetComboBox->currentText().split("-");
	int selOffsetFlag = offsetItems.at(0).toInt();
	char offstFlag;
	switch (selOffsetFlag)
	{
		case 0:
			offstFlag = Sgit_FTDC_OF_Open;
			break;
		case 1:
			offstFlag = Sgit_FTDC_OF_Close;
			break;
		case 2:
			offstFlag = Sgit_FTDC_OF_CloseToday;
			break;
		case 3:
			offstFlag = Sgit_FTDC_OF_CloseYesterday;
			break;
		default:
			offstFlag = Sgit_FTDC_OF_Open;
			break;
	}
	int qty = mVolumeSpinBox->value();
	double price = mPriceSpinBox->value();
	QString speLabel;
	if(Sgit_FTDC_D_Buy == direction)
		speLabel = tr("SGIT|") + selIns + tr("_B");
	else
		speLabel = tr("SGIT|") + selIns + tr("_S");
	emit onSubmitOrder(label, speLabel, selIns, direction, offstFlag, qty, price, false);
}

void USTPSubmitWidget::doClickSelIns(const	QString& selIns, const int& direction, const double& showPrice)
{	
	mInsLineEdit->setText(selIns);
	double tick = USTPMutexId::getInsPriceTick(selIns);
	mPriceSpinBox->setDecimals(getPrcision(tick));
	mPriceSpinBox->setSingleStep(tick);
	if(direction == -1)
		return;
	mBSComboBox->setCurrentIndex(direction);
	mPriceSpinBox->setValue(showPrice);
}

void USTPSubmitWidget::doUpdateSpePrice(const QString& selIns, const int& direction, const double& orderPrice)
{
	if(mInsLineEdit->text() == selIns){
		mBSComboBox->setCurrentIndex(direction);
		double adjustPrice = 0.0;
		double tick = USTPMutexId::getInsPriceTick(selIns);
		if(direction == 0)
			adjustPrice = orderPrice + tick;
		else
			adjustPrice = orderPrice - tick;
		mPriceSpinBox->setValue(adjustPrice);
		createOrder();
	}
}


int USTPSubmitWidget::getPrcision(const double& value)
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

void USTPSubmitWidget::doSubscribeMd(const QStringList& inss)
{	
	foreach (USTPStrategyBase* pStrategy, mUSTPSpeStrategyMap){
		delete pStrategy;
		pStrategy = NULL;
	}
	mUSTPSpeStrategyMap.clear();
	mCTPInss.clear();
	mSgitInss.clear();
	foreach(QString ins, inss){
		QStringList items = ins.split("|");
		if (items.size() == 2){
			QString oneIns = items.at(0);
			QString secIns = items.at(1);
			QMap<QString, QString>::const_iterator iter = mCTPInss.find(oneIns);
			if(iter == mCTPInss.end()){
				QString label_B = tr("CTP|") + oneIns + tr("_B");
				QString label_S = tr("CTP|") + oneIns + tr("_S");

				USTPStrategyBase* pStrategy_B = new USTPSpeculateOrder("", label_B, "", "", 0.0, 0, THOST_FTDC_D_Buy,  THOST_FTDC_OF_Open,
					THOST_FTDC_HF_Speculation, mOrderWidget, mCancelWidget, this);
				USTPStrategyBase* pStrategy_S = new USTPSpeculateOrder("", label_S, "", "", 0.0, 0, THOST_FTDC_D_Sell,  THOST_FTDC_OF_Open,
					THOST_FTDC_HF_Speculation, mOrderWidget, mCancelWidget, this);
				mUSTPSpeStrategyMap.insert(label_B, pStrategy_B);
				mUSTPSpeStrategyMap.insert(label_S, pStrategy_S);
				mCTPInss.insert(oneIns, oneIns);
			}
			iter = mSgitInss.find(secIns);
			if(iter == mSgitInss.end()){
				QString label_B = tr("SGIT|") + secIns + tr("_B");
				QString label_S = tr("SGIT|") + secIns + tr("_S");
				USTPStrategyBase* pStrategy_B = new USTPSpeculateOrder("", label_B, "", "", 0.0, 0, Sgit_FTDC_D_Buy,  Sgit_FTDC_OF_Open,
					Sgit_FTDC_ECIDT_Speculation, mOrderWidget, mCancelWidget, this);
				USTPStrategyBase* pStrategy_S = new USTPSpeculateOrder("", label_S, "", "", 0.0, 0, Sgit_FTDC_D_Sell,  Sgit_FTDC_OF_Open,
					Sgit_FTDC_ECIDT_Speculation, mOrderWidget, mCancelWidget, this);
				mUSTPSpeStrategyMap.insert(label_B, pStrategy_B);
				mUSTPSpeStrategyMap.insert(label_S, pStrategy_S);
				mSgitInss.insert(secIns, secIns);
			}
		}
	}

}

#include "moc_USTPSubmitWidget.cpp"
