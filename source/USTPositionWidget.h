#ifndef USTP_POSITION_WIDGET_H
#define USTP_POSITION_WIDGET_H

#include <QtGui/QTreeWidget>
#include <QtCore/QMap>

class USTPositionWidget : public  QTreeWidget
{
	Q_OBJECT
public:

	USTPositionWidget(const QString& userId, QWidget* parent = 0);

	~USTPositionWidget();

public slots:

	void doCTPRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
		const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
		const QString& localId, const QString& orderRef, const QString& tradeTime);


	void doSgitRtnTrade(const QString& tradeId, const QString& instrumentId, const char& direction, const int& tradeVolume, const double& tradePrice,
		const char& offsetFlag, const char& hedgeFlag, const QString& brokerId, const QString& exchangeId, const QString& investorId, const QString& orderSysId,
		const QString& localId, const QString& orderRef, const QString& tradeTime);

private:
	void initConnect();

	void initPositionShow();

	void updatePositionShow(const QString& ins);

private:

	QStringList mHeaders;
	QString mUserId;
	QMap<QString, int> mBidPositions;
	QMap<QString, int> mAskPositions;
};
#endif