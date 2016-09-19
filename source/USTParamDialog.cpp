#include "USTParamDialog.h"
#include "USTPConfig.h"
#include "USTPStrategyWidget.h"

USTParamDialog::USTParamDialog(USTPStrategyWidget* pStrategyWidget, QWidget* parent)
:QDialog(parent)
{
	mCancelBtn = createButton(tr("取 消"), SLOT(close()));;
	mConfirmBtn = createButton(tr("确 定"), SLOT(createOrder()));
	mBtnLayout = new QHBoxLayout;
	mBtnLayout->addWidget(mConfirmBtn);
	mBtnLayout->addWidget(mCancelBtn);

	mOrderGroup = createOrderGroup();

	mOrderLayout = new QVBoxLayout;
	mOrderLayout->addWidget(mOrderGroup);

	mViewLayout = new QVBoxLayout;
	mViewLayout->addLayout(mOrderLayout);
	mViewLayout->addStretch(1);
	mViewLayout->addSpacing(12);
	mViewLayout->addLayout(mBtnLayout);	
	setLayout(mViewLayout);
	initConnect(pStrategyWidget);
	setWindowTitle(STRATERGY_WINDOW_TITLE);
}

USTParamDialog::~USTParamDialog()
{

}

QGroupBox* USTParamDialog::createOrderGroup()
{	
	QGroupBox *groupBox = new QGroupBox(tr("订单设置"));

	mFirstInsLabel = new QLabel(tr("CTP合约:"));
	mFirstInsEdit = new QLineEdit;
	mFirstInsLabel->setBuddy(mFirstInsEdit);

	mSecondInsLabel = new QLabel(tr("SGIT合约:"));
	mSecondInsEdit = new QLineEdit;
	mSecondInsLabel->setBuddy(mSecondInsEdit);

	mStyleLabel = new QLabel(tr("订单类型:"));
	QStringList list;
	list <<tr("0-条件套利");
	mStyleComboBox = createComboBox(list);
	mStyleComboBox->setCurrentIndex(0);
	mStyleLabel->setBuddy(mStyleComboBox);

	mDirectionLabel = new QLabel(tr("买/卖:"));
	QStringList bsList;
	bsList << tr("0-买入") << tr("1-卖出");
	mBSComboBox = createComboBox(bsList);
	mDirectionLabel->setBuddy(mBSComboBox);

	mOffsetLabel = new QLabel(tr("开/平:"));
	QStringList offsetList;
	offsetList << tr("0-开仓") << tr("1-平仓") << tr("2-平今") << tr("3-平昨");
	mOffsetComboBox = createComboBox(offsetList);
	mOffsetLabel->setBuddy(mOffsetComboBox);

	mHedgeLabel = new QLabel(tr("投机/套保:"));
	QStringList hedgeList;
	hedgeList << tr("0-投机") << tr("1-套利");
	mHedgeComboBox = createComboBox(hedgeList);
	mHedgeComboBox->setCurrentIndex(1);
	mHedgeLabel->setBuddy(mHedgeComboBox);

	mPriceLabel = new QLabel(tr("委托价差:"));
	mPriceSpinBox = new QDoubleSpinBox;
	mPriceSpinBox->setRange(-10000.0, 10000.0);
	mPriceSpinBox->setDecimals(3);
	mPriceSpinBox->setSingleStep(1.0);
	mPriceSpinBox->setValue(20.0);
	mPriceLabel->setBuddy(mPriceSpinBox);

	mQtyLabel = new QLabel(tr("委托量(CTP/SGIT):"));
	mDivisionLabel = new QLabel(tr("   /"));
	mCtpQtySpinBox = new QSpinBox;
	mCtpQtySpinBox->setRange(1, 1000);
	mCtpQtySpinBox->setSingleStep(1);
	mCtpQtySpinBox->setValue(1);

	mSgitQtySpinBox = new QSpinBox;
	mSgitQtySpinBox->setRange(1, 1000);
	mSgitQtySpinBox->setSingleStep(1);
	mSgitQtySpinBox->setValue(1);

	mCycleLabel = new QLabel(tr("循环档:"));
	mCycleSpinBox = new QSpinBox;
	mCycleSpinBox->setRange(1, 1000);
	mCycleSpinBox->setSingleStep(1);
	mCycleSpinBox->setValue(10);
	mCycleLabel->setBuddy(mCycleSpinBox);


	QGridLayout* vBox = new QGridLayout;
	vBox->addWidget(mFirstInsLabel, 0, 0, 1, 1);
	vBox->addWidget(mFirstInsEdit, 0, 1, 1, 2);
	vBox->addWidget(mSecondInsLabel, 0, 3, 1, 1);
	vBox->addWidget(mSecondInsEdit, 0, 4, 1, 2);

	vBox->addWidget(mStyleLabel, 1, 0, 1, 1);
	vBox->addWidget(mStyleComboBox, 1, 1, 1, 2);
	vBox->addWidget(mDirectionLabel, 1, 3, 1, 1);
	vBox->addWidget(mBSComboBox, 1, 4, 1, 2);

	vBox->addWidget(mOffsetLabel, 2, 0, 1, 1);
	vBox->addWidget(mOffsetComboBox, 2, 1, 1, 2);
	vBox->addWidget(mHedgeLabel, 2, 3, 1, 1);
	vBox->addWidget(mHedgeComboBox, 2, 4, 1, 2);

	vBox->addWidget(mQtyLabel, 3, 0, 1, 1);
	vBox->addWidget(mCtpQtySpinBox, 3, 1, 1, 2);
	vBox->addWidget(mDivisionLabel, 3, 3, 1, 1);
	vBox->addWidget(mSgitQtySpinBox, 3, 4, 1, 2);

	vBox->addWidget(mPriceLabel, 4, 0, 1, 1);
	vBox->addWidget(mPriceSpinBox, 4, 1, 1, 2);
	vBox->addWidget(mCycleLabel, 4, 3, 1, 1);
	vBox->addWidget(mCycleSpinBox, 4, 4, 1, 2);
	groupBox->setLayout(vBox);
	return groupBox;
}


QComboBox* USTParamDialog::createComboBox(const QStringList &itemList)
{
	QComboBox *comboBox = new QComboBox;
	foreach(QString item, itemList){
		comboBox->addItem(item);
	}
	comboBox->setEditable(true);
	comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	return comboBox;
}

QPushButton *USTParamDialog::createButton(const QString &text, const char *member)
{
	QPushButton *button = new QPushButton(text);
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}

void USTParamDialog::initConnect(USTPStrategyWidget* pStrategyWidget)
{	
	connect(mStyleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(doEnableBox(int)));

	connect(this, SIGNAL(onCreateNewOrder(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const double&, 
		const int&, const int&, const int&)), pStrategyWidget, SLOT(doCreateNewOrder(const QString&, const QString&, const QString&, const QString&, 
		const QString&, const QString&, const double&, const int&, const int&, const int&)));
}


void USTParamDialog::createOrder()
{
	QString firstName, secondName;
	firstName = mFirstInsEdit->text();
	secondName = mSecondInsEdit->text();
	if(firstName.isEmpty() || secondName.isEmpty()){
		ShowWarning(tr("合约设置不能为空."));
	}else{
		QString style = mStyleComboBox->currentText();
		QString bs = mBSComboBox->currentText();
		QString offset = mOffsetComboBox->currentText();
		QString hedge = mHedgeComboBox->currentText();
		double price = mPriceSpinBox->value();
		int ctpQty = mCtpQtySpinBox->value();
		int sgitQty = mSgitQtySpinBox->value();
		int cycleNum = mCycleSpinBox->value();
		emit onCreateNewOrder(firstName, secondName, style, bs, offset, hedge, price, ctpQty, sgitQty, cycleNum);
		accept();
	}
}

#include "moc_USTParamDialog.cpp"