#ifndef USTP_TAB_WIDGET_H
#define USTP_TAB_WIDGET_H

#include <QtGui/QTabWidget>

class USTPTabWidget : public  QTabWidget
{
	Q_OBJECT
public:

	USTPTabWidget(QWidget* parent = 0);

	virtual ~USTPTabWidget();

private:
	 QAction* mReNameAct;

 public slots:
		void doReNameTabHead();

		void doCurrentChanged (int index);

signals:
		void onTabIndexChanged(int tabIndex);
	
protected:
	void contextMenuEvent(QContextMenuEvent *event);
};
#endif