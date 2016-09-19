#include "USTPKeyDialog.h"
#include "USTPConfig.h"
#include "USTPStrategyWidget.h"

USTPKeyDialog::USTPKeyDialog(const int& bidK, const int& askK, QWidget* parent)
:QDialog(parent)
{	
	mCancelBtn = createButton(tr("取 消"), SLOT(close()));;
	mConfirmBtn = createButton(tr("确 定"), SLOT(createKey()));
	mBtnLayout = new QHBoxLayout;
	mBtnLayout->addWidget(mConfirmBtn);
	mBtnLayout->addWidget(mCancelBtn);

	mKeyGroup = createKeyGroup(bidK, askK);

	mKeyLayout = new QVBoxLayout;
	mKeyLayout->addWidget(mKeyGroup);
	
	mViewLayout = new QVBoxLayout;
	mViewLayout->addLayout(mKeyLayout);
	mViewLayout->addStretch(1);
	mViewLayout->addSpacing(12);
	mViewLayout->addLayout(mBtnLayout);	
	setLayout(mViewLayout);
	initConnect(parent);
	setWindowTitle(KEY_WINDOW_TITLE);
}

USTPKeyDialog::~USTPKeyDialog()
{

}

QGroupBox* USTPKeyDialog::createKeyGroup(const int& bidK, const int& askK)
{	
	QGroupBox *groupBox = new QGroupBox(tr("快捷键设置"));

	mBidKeyLabel = new QLabel(tr("买快捷键:"));
	QChar bidChar = bidK;
	QChar askChar = askK;
	mBidKeyEdit = new QLineEdit(QString(bidChar));
	mBidKeyLabel->setBuddy(mBidKeyEdit);

	mAskKeyLabel = new QLabel(tr("卖快捷键:"));
	mAskKeyEdit = new QLineEdit(QString(askChar));
	mAskKeyLabel->setBuddy(mAskKeyEdit);

	QGridLayout* vBox = new QGridLayout;
	vBox->addWidget(mBidKeyLabel, 0, 0, 1, 1);
	vBox->addWidget(mBidKeyEdit, 0, 1, 1, 2);
	vBox->addWidget(mAskKeyLabel, 1, 0, 1, 1);
	vBox->addWidget(mAskKeyEdit, 1, 1, 1, 2);
	groupBox->setLayout(vBox);
	return groupBox;
}


QPushButton *USTPKeyDialog::createButton(const QString &text, const char *member)
{
	QPushButton *button = new QPushButton(text);
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}

void USTPKeyDialog::initConnect(QWidget* pMainWindow)
{	
	connect(this, SIGNAL(onCreateNewKey(const int&, const int&)), pMainWindow, SLOT(doCreateNewKey(const int&, const int&)));
}


void USTPKeyDialog::createKey()
{
	QString bidName, askName;
	bidName = mBidKeyEdit->text();
	askName = mAskKeyEdit->text();
	if(bidName.isEmpty() || askName.isEmpty()){
		ShowWarning(tr("快捷键设置不能为空."));
	}else{
		int bidK = bidName.at(0).unicode();
		int askK = askName.at(0).unicode();
		emit onCreateNewKey(bidK, askK);
		accept();
	}
}

#include "moc_USTPKeyDialog.cpp"