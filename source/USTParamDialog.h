#ifndef USTP_PARAM_DIALOG_H
#define USTP_PARAM_DIALOG_H

#include <QtGui/QtGui>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QLabel;
class QSpinBox;
class QCheckBox;
class QGroupBox;
QT_END_NAMESPACE

class USTPStrategyWidget;

class USTParamDialog : public QDialog
{
	Q_OBJECT

public:
	USTParamDialog(USTPStrategyWidget* pStrategyWidget, QWidget* parent = 0);

	~USTParamDialog();

private:
	QGroupBox* createOrderGroup();
	QGroupBox* createSpotGroup();
	QComboBox* createComboBox(const QStringList &itemList);
	QPushButton* createButton(const QString &text, const char *member);
	void initConnect(USTPStrategyWidget* pStrategyWidget);

signals:
	void onCreateNewOrder(const QString& firstIns, const QString& secIns, const QString& orderStyle, const QString& direction, const QString& offsetFlag, 
		const QString& hedgeFlag, const double& priceTick, const int& ctpOrderQty, const int& sgitOrderQty, const int& cycleNum);
	protected slots:
		void createOrder();

private:

	QLabel* mFirstInsLabel;
	QLabel* mSecondInsLabel;
	QLabel* mStyleLabel;
	QLabel* mDirectionLabel;
	QLabel* mOffsetLabel;
	QLabel* mHedgeLabel;
	QLabel* mQtyLabel;
	QLabel* mPriceLabel;
	QLabel* mCycleLabel;
	QLabel* mDivisionLabel;

	QLineEdit* mFirstInsEdit;
	QLineEdit* mSecondInsEdit;
	QComboBox* mStyleComboBox;
	QComboBox* mBSComboBox;
	QComboBox* mOffsetComboBox;
	QComboBox* mHedgeComboBox;

	QDoubleSpinBox* mPriceSpinBox;
	QSpinBox* mCtpQtySpinBox;
	QSpinBox* mSgitQtySpinBox;
	QSpinBox* mCycleSpinBox;

	QPushButton* mConfirmBtn;
	QPushButton* mCancelBtn;

	QGroupBox* mOrderGroup;

	QHBoxLayout* mBtnLayout;
	QVBoxLayout* mOrderLayout;
	QVBoxLayout* mViewLayout;
};

#endif