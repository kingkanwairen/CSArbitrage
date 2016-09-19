#ifndef USTP_UNILATERAL_DIALOG_H
#define USTP_UNILATERAL_DIALOG_H

#include <QtGui/QtGui>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QLabel;
class QSpinBox;
class QCheckBox;
class QGroupBox;
QT_END_NAMESPACE

class USTPUnilateralWidget;

class USTPUnilateralDialog : public QDialog
{
	Q_OBJECT

public:
	USTPUnilateralDialog(USTPUnilateralWidget* pUnilateralWidget, QWidget* parent = 0);

	~USTPUnilateralDialog();

private:
	QGroupBox* createOrderGroup();
	QComboBox* createComboBox(const QStringList &itemList);
	QPushButton* createButton(const QString &text, const char *member);
	void initConnect(USTPUnilateralWidget* pUnilateralWidget);

signals:
	void onCreateUnilateralOrder(const QString& instrumentId, const QString& referInsId, const QString& orderStyle, const QString& direction, const QString& offsetFlag, const double& openPriceSpread, 
		const double& closePriceSpread, const double& opponentPriceSpread, const double& referPriceSpread, const int& orderQty, const int& actionTimeNum, const int& superPriceStall, bool isActionTime, bool isCylce);
	protected slots:
		void createOrder();

private:

	QLabel* mInsLabel;
	QLabel* mReferInsLabel;
	QLabel* mStyleLabel;
	QLabel* mDirectionLabel;
	QLabel* mOpenPriceLabel;
	QLabel* mClosePriceLabel;
	QLabel* mReferPriceLabel;
	QLabel* mOffsetLabel;
	QLabel* mQtyLabel;
	QLabel* mOpponentPriceLabel;
	QLabel* mSuperPriceLabel;
	QLabel* mCycleLabel;
	QLabel* mActionTimeLabel;


	QLineEdit* mInsEdit;
	QLineEdit* mReferInsEdit;
	QComboBox* mStyleComboBox;
	QComboBox* mBSComboBox;
	QComboBox* mOffsetComboBox;
	QDoubleSpinBox* mOpenPriceSpinBox;
	QDoubleSpinBox* mClosePriceSpinBox;
	QDoubleSpinBox* mOpponentPriceSpinBox;
	QDoubleSpinBox* mReferPriceSpinBox;
	QSpinBox* mQtySpinBox;
	QSpinBox* mSuperPriceSpinBox;	
	QSpinBox* mActionTimeSpinBox;
	QCheckBox* mCycleCheckBox;
	QCheckBox* mActionTimeCheckBox;

	QPushButton* mConfirmBtn;
	QPushButton* mCancelBtn;

	QGroupBox* mOrderGroup;

	QHBoxLayout* mBtnLayout;
	QVBoxLayout* mViewLayout;
};

#endif