#ifndef USTP_LOGIN_DIALOG_H
#define USTP_LOGIN_DIALOG_H

#include <QtGui/QtGui>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
QT_END_NAMESPACE


class USTPLoginDialog : public QDialog
{
    Q_OBJECT

public:
	USTPLoginDialog(QWidget* parent = 0);

	~USTPLoginDialog();

public slots:
	
	void doUserLogin();
	void doCTPMdRspUserLogin(const QString& brokerId, const QString& userId, const int& errorId, const QString& errorMsg, bool bIsLast);
	void doSgitMdRspUserLogin(const QString& brokerId, const QString& userId, const int& errorId, const QString& errorMsg, bool bIsLast);
	void doCTPTradeRspUserLogin(const QString& tradingDate, const QString& brokerId, const QString& userId, const int& maxLocalId, const int& frontId,
		const int& sessionId, const int& errorId, const QString& errorMsg, bool bIsLast);
	void doSgitTradeRspUserLogin(const QString& tradingDate, const QString& brokerId, const QString& userId, const int& maxLocalId, const int& frontId,
		const int& sessionId, const int& errorId, const QString& errorMsg, bool bIsLast);
	void doCTPTradeSettlementInfoConfirm(const QString& brokerId, const QString& investorId, const int& errorId, const QString& errorMsg);
	void doCTPRspQryInstrument(const QString& exchangeId, const QString& productId, const QString& instrumentId, const double& priceTick, 
		const int& volumeMultiple, const int& maxMarketVolume, bool bIsLast);
	void doSgitRspQryInstrument(const QString& exchangeId, const QString& productId, const QString& instrumentId, const double& priceTick, 
		const int& volumeMultiple, const int& maxMarketVolume, bool bIsLast);
	void doCTPRspQryInvestorPosition(const QString& instrumentId, const char& direction, const int& position, const int& yPosition, const char& hedgeFlag,
		const QString& brokerId, const QString& tradingDay, const QString& investorId, bool bIsLast);
	void doSgitRspQryInvestorPosition(const QString& instrumentId, const char& direction, const int& position, const int& yPosition, const char& hedgeFlag,
		const QString& brokerId, const QString& tradingDay, const QString& investorId, bool bIsLast);

private:
	void initConnect();

protected:
	void paintEvent(QPaintEvent *event);

private:
	
	QLabel* mAUserLabel;
	QLabel* mAPasswordLabel;
	QLineEdit* mAUserEdit;
	QLineEdit* mAPasswordEdit;

	QLabel* mBUserLabel;
	QLabel* mBPasswordLabel;
	QLineEdit* mBUserEdit;
	QLineEdit* mBPasswordEdit;

	QPushButton* mLoginBtn;
	QPushButton* mCancelBtn;
	QGridLayout* mGridLayout;
	QHBoxLayout* mBtnLayout;
	QVBoxLayout* mViewLayout;

	QString mUserABrokerId;
	QString mUserBBrokerId;
	QString mUserAUserId;
	QString mUserBUserId;
	QString mUserAPassword;
	QString mUserBPassword;
};

#endif
