#ifndef USTP_MAIN_WINDOW_H
#define USTP_MAIN_WINDOW_H
#include <QtGui/QtGui>
#include "USTPOrderWidget.h"
#include "USTPTradeWidget.h"
#include "USTPositionWidget.h"
#include "USTPMarketWidget.h"
#include "USTPStrategyWidget.h"
#include "USTPCancelWidget.h"
#include "USTPSpeMarketWidget.h"
#include "USTPSubmitWidget.h"
#include "USTPTabWidget.h"

QT_BEGIN_NAMESPACE
class QToolBar;
class QAction;
class QTabWidget;
class QDockWidget;
QT_END_NAMESPACE

class USTPMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	USTPMainWindow(QWidget * parent = 0);

	~USTPMainWindow();

private:
	void createPic();
	void createToolBar();
	void createActions();
	void createMenus();
	void createStatusBar();
	QDockWidget * createDockWidget(const QString& title, QWidget* pWidget, 
		Qt::DockWidgetAreas allowAreas = Qt::AllDockWidgetAreas, Qt::DockWidgetArea initArea = Qt::TopDockWidgetArea);

signals:
	void onUpdateKey(const int& bidKey, const int& askKey);

private slots:
	void about();
	void doCreateStrategy();
	void doSubscribeMarket();
	void doSaveStrategy();
	void doNewTab();

private:
	void initConnect();

public slots:

	void doCTPTradeFrontDisconnected(int reason);

	void doCTPMdFrontDisconnected(int reason);

	void doSgitTradeFrontDisconnected(const QString& reason);

	void doSgitMdFrontDisconnected(const QString& reason);

	void doRemoveSubTab(int index);
	
private:
	USTPTabWidget* mArbitrageTabWidget;
	USTPMarketWidget* mMarketWidget;
	USTPOrderWidget* mOrderWidget;
	USTPTradeWidget* mTradeWidget;
	USTPositionWidget* mAPositionWidget;
	USTPositionWidget* mBPositionWidget;
	USTPCancelWidget* mCancelWidget;
	USTPSubmitWidget* mSubmitWidget;
	USTPSpeMarketWidget* mSpeMarketWidget;
	
	QDockWidget* mMarketDockWidget;
	QDockWidget* mOrderDockWidget;
	QDockWidget* mTradeDockWidget;
	QDockWidget* mAPositionDockWidget;
	QDockWidget* mBPositionDockWidget;
	QDockWidget* mCancelDockWidget;
	QDockWidget* mComboDockWidget;

	QToolBar* mToolBar;
	QAction* mHelpAction;
	QAction* mExitAction;
	QAction* mStrategyAction;
	QAction* mMarketAction;
	QAction* mNewTabAction;
	QAction* mSaveAction;

	QMenu* mFileMenu;
	QMenu* mOperMenu;
	QMenu* mHelpMenu;

	QPixmap mLinkPic;
	QPixmap mDisconnectPic;
	QLabel* mTradeLinkLabel;
	QLabel* mMdLinkLabel;
	QLabel* mMessageLabel;

	QString mABrokerId;
	QString mAUserId;
	QString mAPsw;

	QString mBBrokerId;
	QString mBUserId;
	QString mBPsw;
	QIcon mTabIcon;
	int mTabIndex;
};

#endif
