#include <QtCore/QByteArray>
#include "USTPConfig.h"
#include "USTPMainWindow.h"
#include "USTPMdDialog.h"
#include "USTParamDialog.h"
#include "USTPCtpLoader.h"
#include "USTPMutexId.h"
#include "CTPMdApi.h"
#include "SgitMdApi.h"
#include "CTPTradeApi.h"
#include "SgitTradeApi.h"
#include "USTPProfile.h"


USTPMainWindow::USTPMainWindow(QWidget * parent)
: QMainWindow(parent)
{	
	mTabIndex = -1;
	mABrokerId = USTPCtpLoader::getUserABrokerId();
	mBBrokerId = USTPCtpLoader::getUserBBrokerId();
	mAUserId = USTPMutexId::getAUserId();
	mBUserId = USTPMutexId::getBUserId();
	mAPsw = USTPMutexId::getALoginPsw();
	mBPsw = USTPMutexId::getBLoginPsw();
	createPic();
	createActions();
	createMenus();
	createToolBar();
	createStatusBar();

	mTabIcon = QIcon("../image/tab.png");
	mMarketWidget = new USTPMarketWidget(this);
	mSpeMarketWidget = new USTPSpeMarketWidget(this);
	mOrderWidget = new USTPOrderWidget(this);
	mTradeWidget = new USTPTradeWidget(this);
	mAPositionWidget = new USTPositionWidget(mAUserId, this);
	mBPositionWidget = new USTPositionWidget(mBUserId, this);
	mCancelWidget = new USTPCancelWidget(this);
	mArbitrageTabWidget = new USTPTabWidget(this);
	mArbitrageTabWidget->setTabsClosable(true);
	
	QString group, order, key, spread, actinNum;
	USTPProfile::readItem(tr("[ActionNum]"), actinNum);
	QStringList actionList = actinNum.split(",");
	for(int actionIndex = 0; actionIndex < actionList.size(); actionIndex++){
		QString itemAction = actionList.at(actionIndex);
		QStringList actionItem = itemAction.split(":");
		USTPMutexId::initActionNum(actionItem.at(0), actionItem.at(1).toInt());
	}

	int nIndex = 0;
	if(USTPProfile::readItem(tr("[GroupItem]"), group)){
		USTPProfile::readItem(tr("[TradeItem]"), order);
		QStringList groupList = group.split(",");
		QStringList orderList = order.split(":");	
		for(nIndex = 0; nIndex < groupList.size(); nIndex++){
			USTPStrategyWidget* pWidget = new USTPStrategyWidget(nIndex, mMarketWidget, mOrderWidget, mCancelWidget, mArbitrageTabWidget, this);
			mArbitrageTabWidget->addTab(pWidget, mTabIcon, groupList.at(nIndex));
			mArbitrageTabWidget->setTabEnabled(nIndex, true);
			mTabIndex = nIndex;
			if(orderList.size() > nIndex){
				QString subTab = orderList.at(nIndex);
				pWidget->loadList(subTab);
			}
		}
	}
	mSubmitWidget = new USTPSubmitWidget(mOrderWidget, mCancelWidget, mSpeMarketWidget, this);
	mMarketDockWidget = createDockWidget(MARKET_DOCK_WIDGET, mArbitrageTabWidget, Qt:: AllDockWidgetAreas, Qt::TopDockWidgetArea);
	mOrderDockWidget = createDockWidget(ORDER_DOCK_WIDGET, mOrderWidget,Qt:: AllDockWidgetAreas, Qt::TopDockWidgetArea);

	mCancelDockWidget = createDockWidget(CANCEL_DOCK_WIDGET, mCancelWidget,Qt:: AllDockWidgetAreas, Qt::BottomDockWidgetArea);		
	mAPositionDockWidget = createDockWidget(A_POSITION_DOCK_WIDGET, mAPositionWidget,Qt:: AllDockWidgetAreas, Qt::BottomDockWidgetArea);
	mBPositionDockWidget = createDockWidget(B_POSITION_DOCK_WIDGET, mBPositionWidget,Qt:: AllDockWidgetAreas, Qt::BottomDockWidgetArea);
	mTradeDockWidget = createDockWidget(TRADE_DOCK_WIDGET, mTradeWidget,Qt:: AllDockWidgetAreas, Qt::BottomDockWidgetArea);
	
	QWidget* pWidget = new QWidget(this);
	QGridLayout* pLayOut = new QGridLayout;
	pLayOut->addWidget(mSpeMarketWidget, 0, 0, 8, 1);	
	pLayOut->addWidget(mSubmitWidget, 8, 0, 1, 1);
	pWidget->setLayout(pLayOut);
	mComboDockWidget = createDockWidget(MANUAL_ORDER_DOCK_WIDGET, pWidget, Qt:: AllDockWidgetAreas, Qt::LeftDockWidgetArea);
	
	setCentralWidget(mMarketWidget);
	initConnect();
	setWindowIcon(QIcon("../image/title.png"));
	QString titleName = MAIN_WINDOW_TILTE + tr("     CTP账号: ") + mAUserId + tr("   CTP席位: ") + mABrokerId + tr("     SGIT账号: ") +
		mBUserId + tr("   SGIT席位: ") + mBBrokerId;
	setWindowTitle(titleName);
	showMaximized(); 
}

USTPMainWindow::~USTPMainWindow()
{	
	QString actionList;
	QMap<QString, int> actionNms;
	if (USTPMutexId::getTotalActionNum(actionNms)){
		QMapIterator<QString, int> i(actionNms);
		int nIndex = 0;
		while (i.hasNext()){
			i.next();
			QString item = i.key() + tr(":") + QString::number(i.value());
			if (nIndex == 0)
				actionList = item;
			else{
				actionList += tr(",");
				actionList += item;
			}
			nIndex++;
		}
		USTPProfile::writeItem(tr("[ActionNum]"), actionList);
	}

	if(mMarketWidget){
		delete mMarketWidget;
		mMarketWidget = NULL;
	}


	if(mOrderWidget){
		delete mOrderWidget;
		mOrderWidget = NULL;
	}

	if(mOrderDockWidget){
		delete mOrderDockWidget;
		mOrderDockWidget = NULL;
	}

	if(mTradeWidget){
		delete mTradeWidget;
		mTradeWidget = NULL;
	}

	if(mTradeDockWidget){
		delete mTradeDockWidget;
		mTradeDockWidget = NULL;
	}

	if(mAPositionWidget){
		delete mAPositionWidget;
		mAPositionWidget = NULL;
	}

	if(mBPositionWidget){
		delete mBPositionWidget;
		mBPositionWidget = NULL;
	}

	if(mAPositionDockWidget){
		delete mAPositionDockWidget;
		mAPositionDockWidget = NULL;
	}

	if(mBPositionDockWidget){
		delete mBPositionDockWidget;
		mBPositionDockWidget = NULL;
	}
	
	int nIndex = 0;
	for(nIndex = 0; nIndex < mArbitrageTabWidget->count() - 1; nIndex++){
		USTPStrategyWidget* pWidget = qobject_cast<USTPStrategyWidget*>(mArbitrageTabWidget->widget(nIndex));
		delete pWidget;
		pWidget = NULL;
	}

	if (mArbitrageTabWidget){	
		delete mArbitrageTabWidget;
		mArbitrageTabWidget = NULL;
	}

	if(mSpeMarketWidget){
		delete mSpeMarketWidget;
		mSpeMarketWidget = NULL;
	}

	if(mSubmitWidget){
		delete mSubmitWidget;
		mSubmitWidget = NULL;
	}

	if(mCancelWidget){
		delete mCancelWidget;
		mCancelWidget = NULL;
	}

	if(mCancelDockWidget){
		delete mCancelDockWidget;
		mCancelDockWidget = NULL;
	}

	if (mMarketDockWidget){
		delete mMarketDockWidget;
		mMarketDockWidget = NULL;
	}

	if(mComboDockWidget){
		delete mComboDockWidget;
		mComboDockWidget = NULL;
	}
}

void USTPMainWindow::initConnect()
{	
	connect(USTPCtpLoader::getUserATradeSpi(), SIGNAL(onCTPTradeFrontDisconnected(int)), this, SLOT(doCTPTradeFrontDisconnected(int)));
	connect(USTPCtpLoader::getUserBTradeSpi(), SIGNAL(onSgitTradeFrontDisconnected(int)), this, SLOT(doSgitTradeFrontDisconnected(int)));
	connect(USTPCtpLoader::getAMdSpi(), SIGNAL(onCTPMdFrontDisconnected(int)), this, SLOT(doCTPMdFrontDisconnected(int)));
	connect(USTPCtpLoader::getBMdSpi(), SIGNAL(onSgitMdFrontDisconnected(const QString&)), this, SLOT(doSgitMdFrontDisconnected(const QString&)));
	connect(mArbitrageTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(doRemoveSubTab(int))); 
}

void USTPMainWindow::createPic()
{	
	QImage image(LINK_LABEL_WIDTH, LINK_LABEL_HEIGHT, QImage::Format_RGB32);
	image.load(QString("../image/red.png"));
	QImage scaleImage;
	if(image.width() > LINK_LABEL_WIDTH  || image.height() > LINK_LABEL_HEIGHT){
		scaleImage = image.scaled(QSize(LINK_LABEL_WIDTH, LINK_LABEL_HEIGHT));
	}
	mDisconnectPic = QPixmap::fromImage(scaleImage);
	
	image.load(QString("../image/green.png"));
	if(image.width() > LINK_LABEL_WIDTH  || image.height() > LINK_LABEL_HEIGHT){
		scaleImage = image.scaled(QSize(LINK_LABEL_WIDTH, LINK_LABEL_HEIGHT));
	}
	mLinkPic = QPixmap::fromImage(scaleImage);
}

void USTPMainWindow::createToolBar()
{	
	mToolBar = new QToolBar(this);
	mToolBar->setAllowedAreas(Qt::TopToolBarArea);
	mToolBar->setOrientation(Qt::Horizontal);
	mToolBar->setMovable(false);
	mToolBar->addAction(mMarketAction);
	mToolBar->addAction(mStrategyAction);	
	mToolBar->addAction(mNewTabAction);
	mToolBar->addAction(mSaveAction);
	addToolBar(Qt::TopToolBarArea, mToolBar);
}	

void USTPMainWindow::createActions()
{
	mHelpAction = new QAction(QString(tr("关于(&A)")), this);
	mHelpAction->setShortcut(QKeySequence(QObject::tr("Ctrl+A")));
	
	mExitAction = new QAction(QIcon("../image/exit.png"), QString(tr("退出(&E)")), this);
	mExitAction->setShortcuts(QKeySequence::Quit);

	mStrategyAction = new QAction(QIcon("../image/event.png"), QString(tr("创建组合套利策略(&D)")), this);
	mStrategyAction->setShortcut(QKeySequence(QObject::tr("Ctrl+D")));

	mMarketAction = new QAction(QIcon("../image/md.png"), QString(tr("注册行情(&N)")), this);
	mMarketAction->setShortcut(QKeySequence(QObject::tr("Ctrl+N")));

	mNewTabAction = new QAction(QIcon("../image/subtab.png"), QString(tr("添加组合套利标签(&T)")), this);
	mNewTabAction->setShortcut(QKeySequence(QObject::tr("Ctrl+T")));

	mSaveAction = new QAction(QIcon("../image/save.png"), QString(tr("保存设置(&S)")), this);
	mSaveAction->setShortcut(QKeySequence(QObject::tr("Ctrl+S")));

	connect(mExitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
	connect(mHelpAction, SIGNAL(triggered()), this, SLOT(about()));
	connect(mStrategyAction, SIGNAL(triggered()), this, SLOT(doCreateStrategy()));
	connect(mMarketAction, SIGNAL(triggered()), this, SLOT(doSubscribeMarket()));
	connect(mNewTabAction, SIGNAL(triggered()), this, SLOT(doNewTab()));
	connect(mSaveAction, SIGNAL(triggered()), this, SLOT(doSaveStrategy()));	
}

void USTPMainWindow::createMenus()
{
	mFileMenu = menuBar()->addMenu(tr("文件(&F)"));
	mFileMenu->addAction(mExitAction);

	mOperMenu = menuBar()->addMenu(tr("操作(&0)"));
	menuBar()->addSeparator();

	mHelpMenu = menuBar()->addMenu(tr("帮助(&H)"));
	mHelpMenu->addAction(mHelpAction);
}

void USTPMainWindow::createStatusBar()
{
	QStatusBar* bar = statusBar();
	mMessageLabel = new QLabel;
	mTradeLinkLabel = new QLabel;
	mTradeLinkLabel->setFixedSize(LINK_LABEL_WIDTH, LINK_LABEL_WIDTH);
	mTradeLinkLabel->setPixmap(mLinkPic);

	mMdLinkLabel = new QLabel;
	mMdLinkLabel->setFixedSize(LINK_LABEL_WIDTH, LINK_LABEL_WIDTH);
	mMdLinkLabel->setPixmap(mLinkPic);

	bar->addWidget(mMessageLabel, width() - 2 * LINK_LABEL_WIDTH);
	bar->addWidget(mMdLinkLabel);
	bar->addWidget(mTradeLinkLabel);
}

QDockWidget* USTPMainWindow::createDockWidget(const QString& title, QWidget* pWidget, Qt::DockWidgetAreas allowAreas, Qt::DockWidgetArea initArea)
{
	QDockWidget *dock = new QDockWidget(title, this);
	dock->setAllowedAreas(allowAreas);
	dock->setWidget(pWidget);
	addDockWidget(initArea, dock);
	mOperMenu->addAction(dock->toggleViewAction());
	return dock;
}

void USTPMainWindow::about()
{
	QMessageBox::about(this, tr("关于 套利交易系统"),
		tr("<b>程序化套利交易</b> 为一款基于CTP,飞鼠柜台的快速套利程序化交易系统。"));
}


void USTPMainWindow::doSubscribeMarket()
{	
	QString mdItem;
	USTPProfile::readItem(tr("[MdItem]"), mdItem);
	USTPMdDialog dlg(mMarketWidget, mSpeMarketWidget, mSubmitWidget, mdItem, this);
	dlg.exec();
}

void USTPMainWindow::doCreateStrategy()
{	
	USTPStrategyWidget* pWidget = qobject_cast<USTPStrategyWidget*>(mArbitrageTabWidget->currentWidget());  
	if(pWidget != NULL){
		USTParamDialog dlg(pWidget, this);
		dlg.exec();
	}
}


void USTPMainWindow::doSaveStrategy()
{	
	int nMaxIndex = mArbitrageTabWidget->count();
	if(nMaxIndex <= 0)
		return;
	QString groupList;
	int nIndex = 0;
	for(nIndex = 0; nIndex < nMaxIndex; nIndex++){
		QString tabName = mArbitrageTabWidget->tabText(nIndex); 
		if (nIndex == 0){
			groupList = tabName;
		}else{
			groupList += tr(",");
			groupList += tabName;
		}
	}
	if(!USTPProfile::writeItem(tr("[GroupItem]"), groupList))
		ShowWarning(tr("套利组信息保存失败!"));
	QString itemList;
	for(nIndex = 0; nIndex < nMaxIndex; nIndex++){
		USTPStrategyWidget* pWidget = qobject_cast<USTPStrategyWidget*>(mArbitrageTabWidget->widget(nIndex));
		QString sublist = pWidget->getList();
		if(nIndex == 0){
			itemList = sublist;
		}else{
			itemList += tr(":");
			itemList += sublist;
		}
	}
	if(!USTPProfile::writeItem(tr("[TradeItem]"), itemList))
		ShowWarning(tr("套利报单列表项保存失败!"));

	ShowInfo(tr("列表信息保存成功."));
}

void  USTPMainWindow::doNewTab()
{	
	bool ok;
	QString groupName = QInputDialog::getText(this, tr("设置组合套利标签"),
		tr("组名:"), QLineEdit::Normal, tr("Tab"), &ok);
	if (ok && !groupName.isEmpty()){
		USTPStrategyWidget* pWidget = new USTPStrategyWidget(++mTabIndex, mMarketWidget, mOrderWidget, mCancelWidget, mArbitrageTabWidget, this);
		mArbitrageTabWidget->addTab(pWidget, mTabIcon, groupName);
	}
}

void USTPMainWindow::doCTPTradeFrontDisconnected(int reason)
{
	mTradeLinkLabel->setPixmap(mDisconnectPic);
}


void USTPMainWindow::doCTPMdFrontDisconnected(int reason)
{
	mMdLinkLabel->setPixmap(mDisconnectPic);
}

void USTPMainWindow::doSgitTradeFrontDisconnected(const QString& reason)
{
	mTradeLinkLabel->setPixmap(mDisconnectPic);
}

void USTPMainWindow::doSgitMdFrontDisconnected(const QString& reason)
{
	mMdLinkLabel->setPixmap(mDisconnectPic);
}

void USTPMainWindow::doRemoveSubTab(int index)
{	
	USTPStrategyWidget* pWidget = qobject_cast<USTPStrategyWidget*>(mArbitrageTabWidget->widget(index));
	int nCount = pWidget->rowCount();
	if(nCount > 0){
		ShowWarning(tr("当前组合套利标签报单列表数量不为0，不可移除."));
		return;
	}
	mArbitrageTabWidget->removeTab(index);
	delete pWidget;
	pWidget = NULL;
}

#include "moc_USTPMainWindow.cpp"