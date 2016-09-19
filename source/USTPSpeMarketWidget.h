#ifndef USTP_SPE_MARKET_WIDGET_H
#define USTP_SPE_MARKET_WIDGET_H

#include <QtGui/QTableWidget>
#include <QtCore/QMap>
#include "USTPSpeComplexMd.h"


class USTPSpeMarketWidget : public  QTableWidget
{
	Q_OBJECT

public:
	USTPSpeMarketWidget(QWidget* parent = 0);
	~USTPSpeMarketWidget();
public slots:
	void doUSTPSpeComplexMd(const QString& selIns, const QString& bidPrice, const QString& bidVolume, const QString& askPrice, const QString& askVolume, const QString& tradeVolume, const QString& lastPrice);
	void doSubscribeMd(const QStringList& inss);
	void doGetSpeInsPrice(const QString& selIns, const int& direction);
	void doGetUnilateralInsPrice(const QString& insName, const QString& key, const int& indexRow);
	void doUpdateSpeDepthMd(const QString& selIns, const int& direction, const double& orderPrice);
	void doUpdateUnilateralMd(const QString& selIns, const QString& key, const int& indexRow, const double& bidPrice, const double& askPrice);
	void doSelectItem(QTableWidgetItem* it);
signals:
	void onGetSpeDepthMd(const QString& selIns, const int& direction);
	void onGetUnilateralSpeDepthMd(const QString& insName, const QString& key, const int& indexRow);
	void onUpdateSpePrice(const QString& selIns, const int& direction, const double& orderPrice);
	void onUpdateUnilateralPrice(const QString& selIns, const QString& key, const int& indexRow, const double& bidPrice, const double& askPrice);
	void onClickSelIns(const QString& selIns, const int& direction, const double& showPrice);
private:
	void initConnect();
	void addItem(const int& row, const int& colume, const QString& text);
	void updateItemPrice(const int& row, const int& colume, const QString& price);
	void updateItemVolume(const int& row, const int& colume, const QString& volume);
private:
	QStringList mHeaders;
	QMap<QString, USTPSpeComplexMd*> mUSTPSpeMdMap;
	QMap<QString, QString> mCTPInss;
	QMap<QString, QString> mSgitInss;
};
#endif