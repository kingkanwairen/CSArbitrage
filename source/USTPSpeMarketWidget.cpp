#include "USTPSpeMarketWidget.h"
#include <QtGui/QHeaderView>
#include "USTPMutexId.h"
#include "USTPConfig.h"

USTPSpeMarketWidget::USTPSpeMarketWidget(QWidget* parent)
:QTableWidget(parent)
{	
	setColumnCount(SPEC_DEPTH_HEAD_LENGTH);
	mHeaders << SPEC_DEPTH_WIDGET_TAB_HEAD_0 << SPEC_DEPTH_WIDGET_TAB_HEAD_1 << SPEC_DEPTH_WIDGET_TAB_HEAD_2 << SPEC_DEPTH_WIDGET_TAB_HEAD_3
		<< SPEC_DEPTH_WIDGET_TAB_HEAD_4 << SPEC_DEPTH_WIDGET_TAB_HEAD_5 << SPEC_DEPTH_WIDGET_TAB_HEAD_6;
	setHorizontalHeaderLabels(mHeaders);
	setSelectionBehavior(QAbstractItemView::SelectItems);
	verticalHeader()->setVisible(false);
	verticalHeader()->setDefaultSectionSize(22);
	horizontalHeader()->setMovable(true);
	horizontalHeader()->setHighlightSections(false); 
	horizontalHeader()->setFont(QFont("Times", 11, QFont::Bold));

	setStyleSheet(  
		"QTableWidget {"  
		"background-color: rgb(20, 20, 20) ;"  
		"}"  
		"QHeaderView::section {"  
		"color: white;"  
		"}"  
		"QHeaderView::section {"  
		"background-color: rgb(80, 80, 6);" 
		"}"  
		);  

	setFont(QFont("Times", 11, QFont::DemiBold));
	setEditTriggers(QAbstractItemView::NoEditTriggers); 
	setShowGrid(true);
	setColumnWidth(0, 80);
	for(int nIndex = 1; nIndex < SPEC_DEPTH_HEAD_LENGTH; nIndex++){
		setColumnWidth(nIndex, 100);
	}
	initConnect();
}

USTPSpeMarketWidget::~USTPSpeMarketWidget()
{
	foreach (USTPSpeComplexMd* widget, mUSTPSpeMdMap){
		delete widget;
		widget = NULL;
	}
	mUSTPSpeMdMap.clear();
}

void USTPSpeMarketWidget::initConnect()
{
	connect(this, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(doSelectItem(QTableWidgetItem*)));
}

void USTPSpeMarketWidget::doSubscribeMd(const QStringList& inss)
{	
	foreach (USTPSpeComplexMd* widget, mUSTPSpeMdMap){
		delete widget;
		widget = NULL;
	}
	mUSTPSpeMdMap.clear();
	mCTPInss.clear();
	mSgitInss.clear();

	int nRowLen = rowCount();
	int nColumeLen = columnCount();
	for(int row = 0; row < nRowLen; row++){
		for(int colume = 0; colume < nColumeLen; colume++){
			QTableWidgetItem *rItem = takeItem(row, colume);
			delete rItem;
			rItem = NULL;
		}
	}
	setRowCount(0);
	clearContents();

	foreach(QString ins, inss){
		QStringList items = ins.split("|");
		if (items.size() == 2){
			QString oneIns = items.at(0);
			QString secIns = items.at(1);
			QMap<QString, QString>::const_iterator iter = mCTPInss.find(oneIns);
			if(iter == mCTPInss.end()){
				doUSTPSpeComplexMd(tr("CTP|") + oneIns, "0.0", "0", "0.0", "0", "0", "0.0");
				USTPSpeComplexMd* wdg = new USTPSpeComplexMd(oneIns, this);
				mUSTPSpeMdMap.insert(oneIns, wdg);
				mCTPInss.insert(oneIns, oneIns);
			}
			iter = mSgitInss.find(secIns);
			if(iter == mSgitInss.end()){
				doUSTPSpeComplexMd(tr("SGIT|") + secIns, "0.0", "0", "0.0", "0", "0", "0.0");
				USTPSpeComplexMd* wdg = new USTPSpeComplexMd(secIns, this);
				mUSTPSpeMdMap.insert(secIns, wdg);
				mSgitInss.insert(secIns, secIns);
			}
		}
	}

}

void USTPSpeMarketWidget::doUSTPSpeComplexMd(const QString& selIns, const QString& bidPrice, const QString& bidVolume, const QString& askPrice, const QString& askVolume, 
											 const QString& tradeVolume, const QString& lastPrice)

{
	QList<QTableWidgetItem*> itemList = findItems(selIns, Qt::MatchRecursive);
	if (itemList.size() == 0){
		int nRow = rowCount();
		insertRow(nRow);		
		addItem(nRow, 0, selIns);
		addItem(nRow, 1, lastPrice);	
		addItem(nRow, 2, bidPrice);
		addItem(nRow, 3, bidVolume);	
		addItem(nRow, 4, askPrice);
		addItem(nRow, 5, askVolume);	
		addItem(nRow, 6, tradeVolume);
	}
	else{
		QTableWidgetItem* it = itemList.at(0);
		int nSelRow = it->row();
		updateItemPrice(nSelRow, 1, lastPrice);	
		updateItemPrice(nSelRow, 2, bidPrice);
		updateItemVolume(nSelRow, 3, bidVolume);
		updateItemPrice(nSelRow, 4, askPrice);
		updateItemVolume(nSelRow, 5, askVolume);	
		updateItemVolume(nSelRow, 6, tradeVolume);
	}
}

void USTPSpeMarketWidget::updateItemPrice(const int& row, const int& colume, const QString& price)
{
	QString szOldPrice = item(row, colume)->text();
	double oldPrice = szOldPrice.toDouble();
	double newPrice = price.toDouble();
	if(newPrice >= oldPrice){
		item(row, colume)->setTextColor(QColor(230, 0, 0)); 
	}else{
		item(row, colume)->setTextColor(QColor(0, 230, 0)); 
	}
	item(row, colume)->setText(price);
}

void USTPSpeMarketWidget::updateItemVolume(const int& row, const int& colume, const QString& volume)
{
	QString szOldVolume = item(row, colume)->text();
	int oldVolume = szOldVolume.toInt();
	int newVolume = volume.toInt();
	if(newVolume >= oldVolume){
		item(row, colume)->setTextColor(QColor(230, 0, 0)); 
	}else{
		item(row, colume)->setTextColor(QColor(0, 230, 0)); 
	}
	item(row, colume)->setText(volume);
}

void USTPSpeMarketWidget::addItem(const int& row, const int& colume, const QString& text)
{
	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setTextColor(QColor(250, 250, 250)); 
	setItem(row, colume, item);
}

void USTPSpeMarketWidget::doGetSpeInsPrice(const QString& selIns, const int& direction)
{
	emit onGetSpeDepthMd(selIns, direction);
}

void USTPSpeMarketWidget::doUpdateSpeDepthMd(const QString& selIns, const int& direction, const double& orderPrice)
{
	emit onUpdateSpePrice(selIns, direction, orderPrice);
}

void USTPSpeMarketWidget::doGetUnilateralInsPrice(const QString& insName, const QString& key, const int& indexRow)
{
	emit onGetUnilateralSpeDepthMd(insName, key, indexRow);
}

void USTPSpeMarketWidget::doUpdateUnilateralMd(const QString& selIns, const QString& key, const int& indexRow, const double& bidPrice, const double& askPrice)
{
	emit onUpdateUnilateralPrice(selIns, key, indexRow, bidPrice, askPrice);
}

void USTPSpeMarketWidget::doSelectItem(QTableWidgetItem* it)
{
	int nSelRow = it->row();
	int nSelColume = it->column();
	QString selItem = item(nSelRow, 0)->text();
	if(nSelColume == 2 || nSelColume == 3){
		QString selPrice = item(nSelRow, 2)->text();
		emit onClickSelIns(selItem, 1, selPrice.toDouble());
	}else if(nSelColume == 4 || nSelColume == 5){
		QString selPrice = item(nSelRow, 4)->text();
		emit onClickSelIns(selItem, 0, selPrice.toDouble());
	}else{
		emit onClickSelIns(selItem, -1, 0.0);
	}
}

#include "moc_USTPSpeMarketWidget.cpp"