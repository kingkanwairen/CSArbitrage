#include "SgitTradeSpi.h"
#include "USTPConfig.h"
#include "USTPLogger.h"
#include <QtCore/QThread>
#include <QtCore/QDebug>

SgitTradeSpi::SgitTradeSpi(CSgitFtdcTraderApi* pTradeApi)
{
	moveToThread(&mTradeThread);
	mTradeThread.start();
	m_pTradeApi = pTradeApi;
}

SgitTradeSpi::~SgitTradeSpi()
{
	mTradeThread.quit();
	mTradeThread.wait();
}

void SgitTradeSpi::OnFrontConnected()
{	
	emit onSgitTradeFrontConnected();
	QString data = QString(tr("[SGIT| SgitTrade-Connected]  "));
	USTPLogger::saveData(data);
}

void SgitTradeSpi::OnFrontDisconnected(char *pErrMsg)
{	
	emit onSgitTradeFrontDisconnected(QString(pErrMsg));
	QString data = QString(tr("[SGIT| Trade-Disconneted]  ")) +  QString(pErrMsg);
	USTPLogger::saveData(data);
}


void SgitTradeSpi::OnRspUserLogin(CSgitFtdcRspUserLoginField *pRspUserLogin, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspUserLogin != NULL && pRspInfo != NULL){
		emit onSgitTradeRspUserLogin(QString(pRspUserLogin->TradingDay), QString(pRspUserLogin->BrokerID), QString(pRspUserLogin->UserID),
			atoi(pRspUserLogin->MaxOrderRef), pRspUserLogin->FrontID, pRspUserLogin->SessionID, pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg), bIsLast);
		if (pRspInfo->ErrorID == 0){
			m_pTradeApi->Ready();
		}
#ifdef _DEBUG
		QString data = QString(tr("[SGIT| Trade-RspUserLogin]  UserId: ")) + QString(pRspUserLogin->UserID) + tr("  TradingDate: ") + QString(pRspUserLogin->TradingDay)
			+ tr("  ErrorMsg: ") + QString::fromLocal8Bit(pRspInfo->ErrorMsg);
		USTPLogger::saveData(data);
#endif
	}
}

void SgitTradeSpi::OnRspUserLogout(CSgitFtdcUserLogoutField *pUserLogout, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pUserLogout != NULL && pRspInfo != NULL){
		emit onSgitTradeRspUserLogout(QString(pUserLogout->BrokerID), QString(pUserLogout->UserID), pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg));
	}
}

void SgitTradeSpi::OnRtnTrade(CSgitFtdcTradeField *pTrade)
{
	if (pTrade != NULL){
		emit onSgitRtnTrade(QString(pTrade->TradeID), QString(pTrade->InstrumentID), pTrade->Direction, pTrade->Volume, pTrade->Price,
			pTrade->OffsetFlag, pTrade->HedgeFlag, QString(pTrade->BrokerID), QString(pTrade->ExchangeID), QString(pTrade->InvestorID), 
			QString(pTrade->OrderSysID), QString(pTrade->OrderLocalID), QString(pTrade->OrderRef), QString(pTrade->TradeTime));
#ifdef _DEBUG
		QString data = QString(tr("[SGIT| OnRtnTrade]  InvestorId: ")) + QString(pTrade->InvestorID) + tr("  OrderSysID: ") + QString(pTrade->OrderSysID)
			+ tr("  InstrumentId: ") + QString(pTrade->InstrumentID) + tr("  Direction: ") + QString(pTrade->Direction) + tr("  TradePrice: ") + 
			QString::number(pTrade->Price) + tr("  TradeVolume: ") + QString::number(pTrade->Volume) + tr("  TradeTime: ") + QString(pTrade->TradeTime)
			+ tr("  OrderRef: ") + QString(pTrade->OrderRef);
		USTPLogger::saveData(data);
#endif
	}
}

void SgitTradeSpi::OnRtnOrder(CSgitFtdcOrderField *pOrder,CSgitFtdcRspInfoField *pRspInfo)
{	
	if (pOrder != NULL){
		emit onSgitRtnOrder(QString(pOrder->OrderLocalID), QString(pOrder->OrderRef), QString(pOrder->InstrumentID), pOrder->Direction, pOrder->LimitPrice, pOrder->VolumeTotalOriginal,
			pOrder->VolumeTotal, pOrder->VolumeTraded, pOrder->CombOffsetFlag[0], pOrder->OrderPriceType, pOrder->CombHedgeFlag[0], pOrder->OrderStatus,
			QString(pOrder->BrokerID), QString(pOrder->ExchangeID), QString(pOrder->InvestorID), QString(pOrder->OrderSysID), QString(pRspInfo->ErrorMsg), 
			pOrder->TimeCondition, pOrder->RequestID);
#ifdef _DEBUG
		QString data = QString(tr("[SGIT| OnRtnOrder]  InvestorId: ")) + QString(pOrder->InvestorID) + tr("  OrderRef: ") + QString(pOrder->OrderRef)
			+ tr("  InstrumentId: ") + QString(pOrder->InstrumentID) + tr("  Direction: ") + QString(pOrder->Direction) + tr("  OrderPrice: ") + 
			QString::number(pOrder->LimitPrice) + tr("  OrderStatus: ") + QString(pOrder->OrderStatus) + tr("  OrderSysID: ") + QString(pOrder->OrderSysID) +
			tr("  StatusMsg: ") + QString(pOrder->StatusMsg) + tr("  ErrorId: ") + QString::number(pRspInfo->ErrorID) + tr("  Msg: ") + QString(pRspInfo->ErrorMsg);
		USTPLogger::saveData(data);
#endif
	}
}

void SgitTradeSpi::OnRspOrderInsert(CSgitFtdcInputOrderField *pInputOrder, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder != NULL && pRspInfo != NULL){
		emit onSgitRspOrderInsert(QString(pInputOrder->UserID), QString(pInputOrder->BrokerID), pInputOrder->Direction, QString(pInputOrder->GTDDate), pInputOrder->CombHedgeFlag[0],
			QString(pInputOrder->InstrumentID), QString(pInputOrder->InvestorID), pInputOrder->CombOffsetFlag[0], pInputOrder->OrderPriceType, pInputOrder->TimeCondition,
			QString(pInputOrder->OrderRef), pInputOrder->LimitPrice, pInputOrder->VolumeTotalOriginal, pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg), pInputOrder->RequestID);
#ifdef _DEBUG
		QString data = QString(tr("[SGIT| RspOrderInsert]  InvestorId: ")) + QString(pInputOrder->InvestorID) + tr("  OrderRef: ") + QString(pInputOrder->OrderRef)
			+ tr("  InstrumentId: ") + QString(pInputOrder->InstrumentID) + tr("  Direction: ") + QString(pInputOrder->Direction) + tr("  OrderPrice: ") + 
			QString::number(pInputOrder->LimitPrice) + tr("  OrderVolume: ") + QString::number(pInputOrder->VolumeTotalOriginal) + tr("  ErrorMsg: ") +  QString::fromLocal8Bit(pRspInfo->ErrorMsg);
		USTPLogger::saveData(data);
#endif
	}
}

void SgitTradeSpi::OnRspOrderAction(CSgitFtdcInputOrderActionField *pInputOrderAction, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction != NULL && pRspInfo != NULL){
		emit onSgitRspOrderAction(pInputOrderAction->ActionFlag, QString(pInputOrderAction->BrokerID), QString(pInputOrderAction->ExchangeID), QString(pInputOrderAction->InvestorID),
			QString(pInputOrderAction->OrderSysID), QString(pInputOrderAction->OrderActionRef), QString(pInputOrderAction->OrderRef), pInputOrderAction->LimitPrice, 
			pInputOrderAction->VolumeChange, pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg), pInputOrderAction->RequestID);
#ifdef _DEBUG
		QString data = QString(tr("[SGIT| RspOrderAction]  InvestorId: ")) + QString(pInputOrderAction->InvestorID) + tr("  OrderSysID: ") + QString(pInputOrderAction->OrderSysID)
			+ tr("  ActionLocalID: ") + QString(pInputOrderAction->OrderActionRef) + tr("  OrderRef: ") + QString(pInputOrderAction->OrderRef) + tr("  ErrorMsg: ") + 
			QString::fromLocal8Bit(pRspInfo->ErrorMsg);
		USTPLogger::saveData(data);
#endif
	}
}

void SgitTradeSpi::OnRtnInstrumentStatus(CSgitFtdcInstrumentStatusField *pInstrumentStatus)
{
	if (pInstrumentStatus != NULL){
		emit onSgitRtnInstrumentStatus(QString(pInstrumentStatus->ExchangeID), QString(pInstrumentStatus->InstrumentID), pInstrumentStatus->InstrumentStatus);
	}
}

void SgitTradeSpi:: OnRspQryInstrument(CSgitFtdcInstrumentField *pInstrument, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInstrument != NULL ){
		emit onSgitRspQryInstrument((pInstrument->ExchangeID), QString(pInstrument->ProductID), QString(pInstrument->InstrumentID),
			pInstrument->PriceTick, pInstrument->VolumeMultiple, pInstrument->MaxMarketOrderVolume, bIsLast);
	}

}

void SgitTradeSpi::OnRspQryInvestorPosition(CSgitFtdcInvestorPositionField *pInvestorPosition, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(pInvestorPosition == NULL){
		emit onSgitRspQryInvestorPosition("", -1, 0, 0, '1', "", "","", bIsLast);
		return;
	}
	if (pInvestorPosition != NULL){	
		emit onSgitRspQryInvestorPosition(QString(pInvestorPosition->InstrumentID), pInvestorPosition->PosiDirection, pInvestorPosition->Position, 
			pInvestorPosition->YdPosition, pInvestorPosition->HedgeFlag, QString(pInvestorPosition->BrokerID), QString(pInvestorPosition->TradingDay),
			QString(pInvestorPosition->InvestorID), bIsLast);
#ifdef _DEBUG
		QString data = QString(tr("[SGIT| RspQryInvestorPosition]  InvestorId: ")) + QString(pInvestorPosition->InvestorID) + tr("  InstrumentId: ") + QString(pInvestorPosition->InstrumentID)
			+ tr("  Direction: ") + QString(pInvestorPosition->PosiDirection) + tr("  Postion: ") + QString::number(pInvestorPosition->Position) + tr("  YPostion: ") +
			QString::number(pInvestorPosition->YdPosition);
		USTPLogger::saveData(data);
#endif
	}
}

#include "moc_SgitTradeSpi.cpp"