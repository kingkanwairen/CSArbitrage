#ifndef USTP_MD_DIALOG_H
#define USTP_MD_DIALOG_H

#include <QtGui/QtGui>

QT_BEGIN_NAMESPACE
class QListWidgetItem;
class QListWidget;
class QLineEdit;
class QLabel;
QT_END_NAMESPACE

class USTPMarketWidget;
class USTPSpeMarketWidget;
class USTPSubmitWidget;


class USTPMdDialog : public QDialog
{
    Q_OBJECT

public:
	USTPMdDialog(USTPMarketWidget* pWidget, USTPSpeMarketWidget* pSpeWidget, USTPSubmitWidget* pSubmitWidget, const QString& inss, QWidget* parent = 0);

	~USTPMdDialog();

signals:

	void onSubscribeMd(const QStringList& inss);

public slots:
	
	void doAddInstrument();

	void doUpInstrument();

	void doDownInstruemnt();

	void doDelInstrument(QListWidgetItem* item);

	void doSelInstrument(QListWidgetItem* item);

	void doDelAllInstruemnt();

	void doSubscribeMarket();

private:
	void initConnect(USTPMarketWidget* pWidget, USTPSpeMarketWidget* pSpeWidget, USTPSubmitWidget* pSubmitWidget);

	void initList(const QString& list);

	bool saveToFile(const QString& list);

private:
	
	QLabel* mFirstInsLabel;
	QLabel* mSecondInsLabel;
	QLineEdit* mFirstInsEdit;
	QLineEdit* mSecondInsEdit;
    QListWidget * mListWidget;
	QPushButton* mAddBtn;
	QPushButton* mDelBtn;
	QPushButton* mUpBtn;
	QPushButton* mDownBtn;
	QPushButton* mConfirmBtn;
	QPushButton* mCancelBtn;
	QHBoxLayout* mHorizonLayout;
	QGridLayout* mGridLayout;
	QVBoxLayout* mViewLayout;
	int mSelIndex;
};

#endif
