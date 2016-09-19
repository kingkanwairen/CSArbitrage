#include "USTPMarketWidget.h"
#include "CTPMdApi.h"
#include "SgitMdApi.h"
#include "USTPConfig.h"
#include "USTPCtpLoader.h"
#include "USTPMutexId.h"
#include <QtGui/QHeaderView>
#include <QtCore/QTime>
#include <Windows.h>

USTPMarketWidget::USTPMarketWidget( QWidget* parent)
:QTableWidget(parent)
{	
	setColumnCount(DEPTH_HEAD_LENGTH);
	mHeaders << DEPTH_WIDGET_TAB_HEAD_0 << DEPTH_WIDGET_TAB_HEAD_1 << DEPTH_WIDGET_TAB_HEAD_2 << DEPTH_WIDGET_TAB_HEAD_3
		<< DEPTH_WIDGET_TAB_HEAD_4 << DEPTH_WIDGET_TAB_HEAD_5 << DEPTH_WIDGET_TAB_HEAD_6 << DEPTH_WIDGET_TAB_HEAD_7 
		<< DEPTH_WIDGET_TAB_HEAD_8 << DEPTH_WIDGET_TAB_HEAD_9 << DEPTH_WIDGET_TAB_HEAD_10;
	setHorizontalHeaderLabels(mHeaders);
	setSelectionBehavior(QAbstractItemView::SelectRows);
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
	setColumnHidden(0, true);
	setEditTriggers(QAbstractItemView::NoEditTriggers); 
	setShowGrid(true);
	setMinimumWidth(MARKET_WINDOW_MIN_WIDTH);
	initConnect();
	setColumnWidth(0, 80);
	setColumnWidth(1, 80);
	for(int nIndex = 2; nIndex < DEPTH_HEAD_LENGTH; nIndex++){
		setColumnWidth(nIndex, 100);
	}
}

USTPMarketWidget::~USTPMarketWidget()
{

}

void USTPMarketWidget::initConnect()
{
	connect(USTPCtpLoader::getAMdSpi(), SIGNAL(onCTPMdFrontConnected()), this, SLOT(doCTPMdFrontConnected()));
	connect(USTPCtpLoader::getBMdSpi(), SIGNAL(onSgitMdFrontConnected()), this, SLOT(doSgitMdFrontConnected()));
}

void USTPMarketWidget::doCTPMdFrontConnected()
{
	mCTPInss.clear();
	mSgitInss.clear();
}

void USTPMarketWidget::doSgitMdFrontConnected()
{
	mCTPInss.clear();
	mSgitInss.clear();
}

void USTPMarketWidget::doSubscribeMd(const QStringList& inss)
{	
	foreach (USTPComplexMd* widget, mUSTPMdMap){
		delete widget;
		widget = NULL;
	}
	mUSTPMdMap.clear();
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
			doUSTPComplexMd(ins, oneIns, secIns, "0.0", "0.0", "0.0", "0.0", "0", "0", "0", "0");
			USTPComplexMd* wdg = new USTPComplexMd(oneIns, secIns, this);
			mUSTPMdMap.insert(ins, wdg);
			QMap<QString, QString>::const_iterator iter = mCTPInss.find(oneIns);
			if(iter == mCTPInss.end()){
				CTPMdApi::subMarketData(oneIns);
				mCTPInss.insert(oneIns, oneIns);
			}
			iter = mSgitInss.find(secIns);
			if(iter == mSgitInss.end()){
				SgitMdApi::subMarketData(secIns);
				mSgitInss.insert(secIns, secIns);
			}
		}
	}

}

void USTPMarketWidget::doUSTPComplexMd(const QString& key, const QString& firstIns, const QString& secIns, const QString& bbPrice, const QString& bsPrice, const QString& sbPrice,
									   const QString& ssPrice, const QString& bbQty, const QString& bsQty, const QString& sbQty, const QString& ssQty)
{
	QList<QTableWidgetItem*> itemList = findItems(key, Qt::MatchRecursive);
	if (itemList.size() == 0){
		int nRow = rowCount();
		insertRow(nRow);		
		addItem(nRow, 0, key);
		addItem(nRow, 1, firstIns);
		addItem(nRow, 2, secIns);	
		addItem(nRow, 3, bsPrice);
		addItem(nRow, 4, sbPrice);	
		addItem(nRow, 5, bbPrice);
		addItem(nRow, 6, ssPrice);	
		addItem(nRow, 7, bbQty);
		addItem(nRow, 8, bsQty);
		addItem(nRow, 9, sbQty);
		addItem(nRow, 10, ssQty);
	}
	else{
		QTableWidgetItem* it = itemList.at(0);
		int nSelRow = it->row();
		item(nSelRow, 1)->setText(firstIns);
		item(nSelRow, 2)->setText(secIns);	
		updateItemPrice(nSelRow, 3, bsPrice);
		updateItemPrice(nSelRow, 4, sbPrice);	
		updateItemPrice(nSelRow, 5, bbPrice);
		updateItemPrice(nSelRow, 6, ssPrice);
		updateItemVolume(nSelRow, 7, bbQty);
		updateItemVolume(nSelRow, 8, bsQty);
		updateItemVolume(nSelRow, 9, sbQty);
		updateItemVolume(nSelRow, 10, ssQty);
	}
}

void USTPMarketWidget::updateItemPrice(const int& row, const int& colume, const QString& price)
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

void USTPMarketWidget::updateItemVolume(const int& row, const int& colume, const QString& volume)
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

void USTPMarketWidget::addItem(const int& row, const int& colume, const QString& text)
{
	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setTextColor(QColor(250, 250, 250)); 
	setItem(row, colume, item);
}


void  USTPMarketWidget::doGetComplexMarketPrice(const QString& complexIns, const QString& reqKey, const int& rowIndex)
{
	emit onGetComplexDepthPrice(complexIns, reqKey, rowIndex);
}


void USTPMarketWidget::doUpdateComplexDepthPrice(const QString& complexIns, const QString& reqKey, const int& rowId, const double& firstMarketBidPrice, const double& firstMarketAskPrice,
							   const double& secMarketBidPrice, const double& secMarketAskPrice)
{
	emit onUpdateMarketInsPrice(complexIns, reqKey, rowId, firstMarketBidPrice, firstMarketAskPrice, secMarketBidPrice, secMarketAskPrice);
}

#include "moc_USTPMarketWidget.cpp"