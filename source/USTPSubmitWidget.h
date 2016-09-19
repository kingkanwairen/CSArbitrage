#ifndef USTP_SUBMIT_WIDGET_H
#define USTP_SUBMIT_WIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QPainter>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include "USTPOrderWidget.h"
#include "USTPCancelWidget.h"
#include "USTPSpeMarketWidget.h"
#include "USTPUserStrategy.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QGridLayout;
QT_END_NAMESPACE

class USTPSpeMarketWidget;

class USTPSubmitWidget : public QWidget
{
	Q_OBJECT

public:
	USTPSubmitWidget(USTPOrderWidget* pOrderWidget, USTPCancelWidget* pCancelWidget, 
		USTPSpeMarketWidget* pSpeMarketWidget, QWidget* parent = 0);

	~USTPSubmitWidget();

signals:

	void onGetSpeInsPrice(const QString& selIns, const int& direction);
	void onSubmitOrder(const QString& orderLabel, const QString& speLabel, const QString& ins, const char& direction, const char& offsetFlag, const int& volume, 
		const double& orderPrice, bool isAOrder);

private:
	QComboBox* createComboBox(const QStringList &itemList);
	QPushButton* createButton(const QString &text, const char *member);

protected slots:
	void createOrder();
	void doSubscribeMd(const QStringList& inss);
	void doClickSelIns(const QString& selIns, const int& direction, const double& showPrice);
	void doUpdateSpePrice(const QString& selIns, const int& direction, const double& orderPrice);

private:
	void initConnect(USTPSpeMarketWidget* pSpeMarketWidget, QWidget* pWidget);
	int getPrcision(const double& value);

	void submitCtpOrder(const	QString& selIns);
	void submitSgitOrder(const	QString& selIns);

private:
	QLabel* mInsLabel;
	QLabel* mDirectionLabel;
	QLabel* mOffsetFlagLabel;
	QLabel* mVolumeLabel;
	QLabel* mPriceLabel;
	QLineEdit* mInsLineEdit;
	QComboBox* mBSComboBox;
	QComboBox* mOffsetComboBox;
	QDoubleSpinBox* mPriceSpinBox;
	QSpinBox* mVolumeSpinBox;
	QPushButton* mConfirmBtn;
	QGridLayout* mGridLayout;
	USTPOrderWidget* mOrderWidget;
	USTPCancelWidget* mCancelWidget;
	USTPSpeMarketWidget* mSpeMarketWidget;
	QMap<QString, QString> mCTPInss;
	QMap<QString, QString> mSgitInss;
	QMap<QString, USTPStrategyBase*> mUSTPSpeStrategyMap;
};

#endif
