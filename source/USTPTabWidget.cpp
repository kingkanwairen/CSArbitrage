#include "USTPTabWidget.h"
#include <QtGui/QMenu>
#include <QtGui/QInputDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QDebug>

USTPTabWidget::USTPTabWidget(QWidget *parent)
:QTabWidget(parent)
{	
	mReNameAct = new QAction(QIcon("../image/green.png"), QString(tr("重置标签(&R)")), this);
	mReNameAct->setShortcut(QKeySequence(QObject::tr("Ctrl+R")));
	connect(mReNameAct, SIGNAL(triggered()), this, SLOT(doReNameTabHead()));
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(doCurrentChanged(int)));
}

USTPTabWidget::~USTPTabWidget()
{

}

void USTPTabWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.addAction(mReNameAct);
	menu.exec(event->globalPos());
}

void USTPTabWidget::doReNameTabHead()
{	
	QString tabName;
	int nCurrentIndex = currentIndex();
	if(nCurrentIndex < 0)
		tabName = tr("Tab");
	else
		tabName = tabText(nCurrentIndex);

	bool ok;
	QString groupName = QInputDialog::getText(this, tr("重置标签"),
		tr("组名:"), QLineEdit::Normal, tabName, &ok);
	if (ok && !groupName.isEmpty()){
		
		setTabText(nCurrentIndex, groupName);
	}
}

void USTPTabWidget::doCurrentChanged(int index)
{
	emit onTabIndexChanged(index);
}

#include "moc_USTPTabWidget.cpp"