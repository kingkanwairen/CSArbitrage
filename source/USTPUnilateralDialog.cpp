#include "USTPUnilateralDialog.h"
#include "USTPConfig.h"
#include "USTPUnilateralWidget.h"

USTPUnilateralDialog::USTPUnilateralDialog(USTPUnilateralWidget* pUnilateralWidget, QWidget* parent)
:QDialog(parent)
{
	mCancelBtn = createButton(tr("取 消"), SLOT(close()));;
	mConfirmBtn = createButton(tr("确 定"), SLOT(createOrder()));
	mBtnLayout = new QHBoxLayout;
	mBtnLayout->addWidget(mConfirmBtn);
	mBtnLayout->addWidget(mCancelBtn);

	mOrderGroup = createOrderGroup();

	mViewLayout = new QVBoxLayout;
	mViewLayout->addWidget(mOrderGroup);
	mViewLayout->addStretch(1);
	mViewLayout->addSpacing(12);
	mViewLayout->addLayout(mBtnLayout);	
	setLayout(mViewLayout);
	initConnect(pUnilateralWidget);
	setWindowTitle(UNILATERAL_WINDOW_TITLE);
}

USTPUnilateralDialog::~USTPUnilateralDialog()
{

}

QGroupBox* USTPUnilateralDialog::createOrderGroup()
{

	QGroupBox *groupBox = new QGroupBox(tr("订单设置"));
	mInsLabel = new QLabel(tr("单边合约:"));
	mInsEdit = new QLineEdit;
	mInsLabel->setBuddy(mInsEdit);

	mReferInsLabel = new QLabel(tr("参考合约:"));
	mReferInsEdit = new QLineEdit;
	mReferInsLabel->setBuddy(mReferInsEdit);

	mStyleLabel = new QLabel(tr("订单类型:"));
	QStringList list;
	list << tr("0-开开") << tr("1-平平") << tr("2-开平") << tr("3-平开");
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
	offsetList << tr("0-平仓") << tr("1-平今") << tr("2-平昨");
	mOffsetComboBox = createComboBox(offsetList);
	mOffsetLabel->setBuddy(mOffsetComboBox);

	mOpenPriceLabel = new QLabel(tr("开仓点数:"));
	mOpenPriceSpinBox = new QDoubleSpinBox;
	mOpenPriceSpinBox->setRange(0.0, 10000.0);
	mOpenPriceSpinBox->setDecimals(3);
	mOpenPriceSpinBox->setSingleStep(1.0);
	mOpenPriceSpinBox->setValue(20.0);
	mOpenPriceLabel->setBuddy(mOpenPriceSpinBox);

	mQtyLabel = new QLabel(tr("委托量:"));
	mQtySpinBox = new QSpinBox;
	mQtySpinBox->setRange(1, 1000);
	mQtySpinBox->setSingleStep(1);
	mQtySpinBox->setValue(1);
	mQtyLabel->setBuddy(mQtySpinBox);

	mActionTimeLabel = new QLabel(tr("定时撤单:"));
	mActionTimeSpinBox = new QSpinBox;
	mActionTimeSpinBox->setRange(1, 60000);
	mActionTimeSpinBox->setSingleStep(1);
	mActionTimeSpinBox->setValue(400);
	mActionTimeLabel->setBuddy(mActionTimeSpinBox);

	mSuperPriceLabel = new QLabel(tr("超价档:"));
	mSuperPriceSpinBox = new QSpinBox;
	mSuperPriceSpinBox->setRange(-1000, 1000);
	mSuperPriceSpinBox->setSingleStep(1);
	mSuperPriceSpinBox->setValue(1);
	mSuperPriceLabel->setBuddy(mSuperPriceSpinBox);

	mClosePriceLabel = new QLabel(tr("平仓点数:"));
	mClosePriceSpinBox = new QDoubleSpinBox;
	mClosePriceSpinBox->setRange(-10000.0, 10000.0);
	mClosePriceSpinBox->setDecimals(3);
	mClosePriceSpinBox->setSingleStep(1.0);
	mClosePriceSpinBox->setValue(10.0);
	mClosePriceLabel->setBuddy(mClosePriceSpinBox);

	mOpponentPriceLabel = new QLabel(tr("对价点数:"));
	mOpponentPriceSpinBox = new QDoubleSpinBox;
	mOpponentPriceSpinBox->setRange(-10000.0, 10000.0);
	mOpponentPriceSpinBox->setDecimals(3);
	mOpponentPriceSpinBox->setSingleStep(1.0);
	mOpponentPriceSpinBox->setValue(10.0);
	mOpponentPriceLabel->setBuddy(mOpponentPriceSpinBox);

	mReferPriceLabel = new QLabel(tr("参考点数:"));
	mReferPriceSpinBox = new QDoubleSpinBox;
	mReferPriceSpinBox->setRange(-10000.0, 10000.0);
	mReferPriceSpinBox->setDecimals(3);
	mReferPriceSpinBox->setSingleStep(1.0);
	mReferPriceSpinBox->setValue(100.0);
	mReferPriceLabel->setBuddy(mReferPriceSpinBox);

	mCycleCheckBox = new QCheckBox(tr("循环"));
	mCycleCheckBox->setChecked(false);

	mActionTimeCheckBox = new QCheckBox(tr("定时撤单"));
	mActionTimeCheckBox->setChecked(true);

	QGridLayout* vBox = new QGridLayout;
	vBox->addWidget(mInsLabel, 0, 0, 1, 1);
	vBox->addWidget(mInsEdit, 0, 1, 1, 2);

	vBox->addWidget(mReferInsLabel, 0, 3, 1, 1);
	vBox->addWidget(mReferInsEdit, 0, 4, 1, 2);

	vBox->addWidget(mStyleLabel, 1, 0, 1, 1);
	vBox->addWidget(mStyleComboBox, 1, 1, 1, 2);

	vBox->addWidget(mDirectionLabel, 1, 3, 1, 1);
	vBox->addWidget(mBSComboBox, 1, 4, 1, 2);

	vBox->addWidget(mQtyLabel, 2, 0, 1, 1);
	vBox->addWidget(mQtySpinBox, 2, 1, 1, 2);

	vBox->addWidget(mOffsetLabel, 2, 3, 1, 1);
	vBox->addWidget(mOffsetComboBox, 2, 4, 1, 2);

	vBox->addWidget(mSuperPriceLabel, 3, 0, 1, 1);
	vBox->addWidget(mSuperPriceSpinBox, 3, 1, 1, 2);

	vBox->addWidget(mClosePriceLabel, 3, 3, 1, 1);
	vBox->addWidget(mClosePriceSpinBox, 3, 4, 1, 2);

	vBox->addWidget(mOpponentPriceLabel, 4, 0, 1, 1);
	vBox->addWidget(mOpponentPriceSpinBox, 4, 1, 1, 2);

	vBox->addWidget(mOpenPriceLabel, 4, 3, 1, 1);
	vBox->addWidget(mOpenPriceSpinBox, 4, 4, 1, 2);

	vBox->addWidget(mReferPriceLabel, 5, 0, 1, 1);
	vBox->addWidget(mReferPriceSpinBox, 5, 1, 1, 2);

	vBox->addWidget(mActionTimeLabel, 5, 3, 1, 1);
	vBox->addWidget(mActionTimeSpinBox, 5, 4, 1, 2);
	
	vBox->addWidget(mActionTimeCheckBox, 6, 0, 1, 2);
	vBox->addWidget(mCycleCheckBox, 6, 2, 1, 2);

	groupBox->setLayout(vBox);
	return groupBox;
}


QComboBox* USTPUnilateralDialog::createComboBox(const QStringList &itemList)
{
	QComboBox *comboBox = new QComboBox;
	foreach(QString item, itemList){
		comboBox->addItem(item);
	}
	comboBox->setEditable(true);
	comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	return comboBox;
}

QPushButton *USTPUnilateralDialog::createButton(const QString &text, const char *member)
{
	QPushButton *button = new QPushButton(text);
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}

void USTPUnilateralDialog::initConnect(USTPUnilateralWidget* pUnilateralWidget)
{	
	connect(this, SIGNAL(onCreateUnilateralOrder(const QString&, const QString&, const QString&, const QString&, const QString&, const double&, const double&, const double&, 
		const double&, const int&, const int&, const int&, bool, bool)), pUnilateralWidget, SLOT(doCreateUnilateralOrder(const QString&, const QString&, const QString&, 
		const QString&, const QString&, const double&, const double&, const double&, const double&, const int&, const int&, const int&, bool, bool)));
}


void USTPUnilateralDialog::createOrder()
{
	QString insName = mInsEdit->text();
	QString referInsName = mReferInsEdit->text();
	if(insName.isEmpty()){
		ShowWarning(tr("合约设置不能为空."));
	}else{
		QString style = mStyleComboBox->currentText();
		QString bs = mBSComboBox->currentText();
		QString offset = mOffsetComboBox->currentText();
		double openPrice = mOpenPriceSpinBox->value();
		double closePrice = mClosePriceSpinBox->value();
		double opponentPrice = mOpponentPriceSpinBox->value();
		double referPrice = mReferPriceSpinBox->value();
		int qty = mQtySpinBox->value();
		int actionTimeNum = mActionTimeSpinBox->value();
		int superTickNum = mSuperPriceSpinBox->value();
		bool isActionTime = mActionTimeCheckBox->isChecked();
		bool isCycle = mCycleCheckBox->isChecked();
		emit onCreateUnilateralOrder(insName, referInsName, style, bs, offset, openPrice, closePrice, opponentPrice, referPrice, qty, actionTimeNum, superTickNum, isActionTime, isCycle);
		accept();
	}
}

#include "moc_USTPUnilateralDialog.cpp"