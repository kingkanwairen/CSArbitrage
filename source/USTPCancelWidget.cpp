#include "USTPCancelWidget.h"
#include <QtGui/QHeaderView>
#include "USTPConfig.h"
#include "USTPCtpLoader.h"

USTPCancelWidget::USTPCancelWidget(QWidget* parent)
:QTableWidget(parent)
{	
	setColumnCount(CANCEL_HEAD_LENGTH);
	mHeaders << CANCEL_WIDGET_TAB_HEAD_0 << CANCEL_WIDGET_TAB_HEAD_1 << CANCEL_WIDGET_TAB_HEAD_2
		<< CANCEL_WIDGET_TAB_HEAD_3 << CANCEL_WIDGET_TAB_HEAD_4 << CANCEL_WIDGET_TAB_HEAD_5 
		<< CANCEL_WIDGET_TAB_HEAD_6 << CANCEL_WIDGET_TAB_HEAD_7 << CANCEL_WIDGET_TAB_HEAD_8;
	setHorizontalHeaderLabels(mHeaders);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setAlternatingRowColors(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers); 
	horizontalHeader()->setMovable(true);
	setColumnHidden(1, true);
	setShowGrid(true);
	horizontalHeader()->setFont(QFont("Times", 10, QFont::DemiBold));
}

USTPCancelWidget::~USTPCancelWidget()
{

}


QString USTPCancelWidget::getBSText(const char& bs)
{	
	QString rtnBS = "--";
	if (bs == '0'){
		rtnBS = QString(tr("买"));
	}else{
		rtnBS = QString(tr("卖"));
	}
	return rtnBS;
}

QString USTPCancelWidget::getOffsetFlagText(const char& offsetFlag)
{
	QString rtnOffset = "--";
	if (offsetFlag == '0'){
		rtnOffset = QString(tr("开仓"));
	}else if (offsetFlag == '1'){
		rtnOffset = QString(tr("平仓"));
	}else if (offsetFlag == '2'){
		rtnOffset = QString(tr("强平"));
	}else if (offsetFlag == '3'){
		rtnOffset = QString(tr("平今"));
	}else if (offsetFlag == '4'){
		rtnOffset = QString(tr("平昨"));
	}
	return rtnOffset;
}

void USTPCancelWidget::doUpdateOrderShow(const QString& userLocalId, const QString& instrument, const QString& orderStyle, const char& status, const char& bs, const double& price, const int& orderVolume, 
										 const int& remainVolume, const int& tradeVolume, const char& offsetFlag, const char& priceType, const char& hedge, const double& orderPrice)
{	
	QList<QTableWidgetItem*> itemList = findItems(orderStyle, Qt::MatchRecursive);
	if (itemList.size() == 0){
		if(status == THOST_FTDC_OST_AllTraded || status == THOST_FTDC_OST_Canceled)
			return;
		setSortingEnabled(false);
		int nRow = rowCount();	
		insertRow(nRow);
		QPushButton* pDeleteBtn = createButton(tr("删 除"), SLOT(doBtnDeleteClicked()));
		QPushButton* pCancelBtn = createButton(tr("清除列表"), SLOT(doBtnCancelClicked()));
		setCellWidget(nRow, 0, pDeleteBtn);
		int splitChar = orderStyle.indexOf(tr("]"));
		QString numStr = orderStyle.mid(splitChar + 1);
		addItem(nRow, 1, numStr);
		addItem(nRow, 2, orderStyle);
		addItem(nRow, 3, instrument);
		addItem(nRow, 4, getBSText(bs));
		addItem(nRow, 5, QString::number(orderVolume));
		addItem(nRow, 6, getOffsetFlagText(offsetFlag));
		addItem(nRow, 7, QString::number(orderPrice));
		setCellWidget(nRow, 8, pCancelBtn);
		setSortingEnabled(true);
		sortItems(1, Qt::DescendingOrder);
	}
}

void USTPCancelWidget::doOrderFinished(const QString& orderLabel, const QString& firstIns, const QString& secIns, const double& orderPriceTick, const int& qty, const char& bs,  const char& offset, const char& hedge,
									   bool isAllTraded, const int& orderType, const double& firstMarketBidPrice, const double& firstMarketAskPrice, const double& secMarketBidPrice, const double& secMarketAskPrice)
{	
	QList<QTableWidgetItem*> itemList = findItems(orderLabel, Qt::MatchRecursive);
	if(itemList.size() > 0){
		QTableWidgetItem* it = itemList.at(0);
		int nSelRow = it->row();
		int nColumeLen = columnCount();
		for(int nIndex = 0; nIndex < nColumeLen; nIndex++){
			QTableWidgetItem* showItem = takeItem(nSelRow, nIndex);
			delete showItem;
			showItem = NULL;
		}
		removeRow(nSelRow);
	}
}


void USTPCancelWidget::addItem(const int& row, const int& colume, const QString& text)
{
	QTableWidgetItem* item = new QTableWidgetItem(text);
	setItem(row, colume, item);
}

QPushButton *USTPCancelWidget::createButton(const QString &text, const char *member)
{
	QPushButton *button = new QPushButton(text);
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}

void USTPCancelWidget::doBtnDeleteClicked()
{	
	QPushButton * senderObj = qobject_cast<QPushButton *>(sender());
	if(senderObj == 0)
		return;
	setFocus();
	QModelIndex index = indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
	int row = index.row();
	QString orderName = item(row, 2)->text();
	emit onDelOrder(orderName);
}

void USTPCancelWidget::doBtnCancelClicked()
{	
	QPushButton * senderObj = qobject_cast<QPushButton *>(sender());
	if(senderObj == 0)
		return;
	setFocus();
	QModelIndex index = indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
	int nSelRow = index.row();
	int nColumeLen = columnCount();
	for(int nIndex = 0; nIndex < nColumeLen; nIndex++){
		QTableWidgetItem* showItem = takeItem(nSelRow, nIndex);
		delete showItem;
		showItem = NULL;
	}
	removeRow(nSelRow);
}

#include "moc_USTPCancelWidget.cpp"