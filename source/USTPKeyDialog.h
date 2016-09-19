#ifndef USTP_KEY_DIALOG_H
#define USTP_KEY_DIALOG_H

#include <QtGui/QtGui>


QT_BEGIN_NAMESPACE
class QLineEdit;
class QLabel;
class QGroupBox;
QT_END_NAMESPACE

class USTPKeyDialog : public QDialog
{
	Q_OBJECT

public:
	USTPKeyDialog(const int& bidK, const int& askK, QWidget* parent = 0);

	~USTPKeyDialog();

private:
	QGroupBox* createKeyGroup(const int& bidKey, const int& askKey);
	QPushButton* createButton(const QString &text, const char *member);
	void initConnect(QWidget* pMainWindow);

signals:
	void onCreateNewKey(const int& bidKey, const int& askKey);
protected slots:
	void createKey();

private:

	QLabel* mBidKeyLabel;
	QLabel* mAskKeyLabel;
	
	QLineEdit* mBidKeyEdit;
	QLineEdit* mAskKeyEdit;	
	
	QPushButton* mConfirmBtn;
	QPushButton* mCancelBtn;

	QGroupBox* mKeyGroup;

	QHBoxLayout* mBtnLayout;
	QVBoxLayout* mKeyLayout;
	QVBoxLayout* mViewLayout;
};

#endif