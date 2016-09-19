#include "CTPTradeSpi.h"
#include "USTPConfig.h"
#include "USTPLogger.h"
#include <QtCore/QThread>
#include <QtCore/QDebug>

CTPTradeSpi::CTPTradeSpi()
{
	moveToThread(&mTradeThread);
	mTradeThread.start();
}

CTPTradeSpi::~CTPTradeSpi()
{
	mTradeThread.quit();
	mTradeThread.wait();
}

void CTPTradeSpi::OnFrontConnected()
{	
	emit onCTPTradeFrontConnected();
	QString data = QString(tr("[CTP| Trade-Connected]  "));
	USTPLogger::saveData(data);
}

void CTPTradeSpi::OnFrontDisconnected(int nReason)
{	
	emit onCTPTradeFrontDisconnected(nReason);
	QString data = QString(tr("[CTP| Trade-Disconneted]  ")) +  QString::number(nReason);
	USTPLogger::saveData(data);
}

void CTPTradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
#ifdef _DEBUG
	QString data = QString(tr("[CTP| Trade-RspError]  ErrorId:")) + QString::number(pRspInfo->ErrorID) + tr("  ErrorMsg: ") + QString::fromLocal8Bit(pRspInfo->ErrorMsg);
	USTPLogger::saveData(data);
#endif
}

void CTPTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspUserLogin != NULL && pRspInfo != NULL){
		emit onCTPTradeRspUserLogin(QString(pRspUserLogin->TradingDay), QString(pRspUserLogin->BrokerID), QString(pRspUserLogin->UserID),
			atoi(pRspUserLogin->MaxOrderRef), pRspUserLogin->FrontID, pRspUserLogin->SessionID, pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg), bIsLast);
#ifdef _DEBUG
		QString data = QString(tr("[CTP| Trade-RspUserLogin]  UserId: ")) + QString(pRspUserLogin->UserID) + tr("  TradingDate: ") + QString(pRspUserLogin->TradingDay)
			+ tr("  ErrorMsg: ") + QString::fromLocal8Bit(pRspInfo->ErrorMsg);
		USTPLogger::saveData(data);
#endif
	}
}

void CTPTradeSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pUserLogout != NULL && pRspInfo != NULL){
		emit onCTPTradeRspUserLogout(QString(pUserLogout->BrokerID), QString(pUserLogout->UserID), pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg));
	}
}

void CTPTradeSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pSettlementInfoConfirm != NULL && pRspInfo != NULL){
		emit onCTPTradeSettlementInfoConfirm(QString(pSettlementInfoConfirm->BrokerID), QString(pSettlementInfoConfirm->InvestorID), pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg));
	}
}

void CTPTradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if (pTrade != NULL){
		emit onCTPRtnTrade(QString(pTrade->TradeID), QString(pTrade->InstrumentID), pTrade->Direction, pTrade->Volume, pTrade->Price,
			pTrade->OffsetFlag, pTrade->HedgeFlag, QString(pTrade->BrokerID), QString(pTrade->ExchangeID), QString(pTrade->InvestorID), 
			QString(pTrade->OrderSysID), QString(pTrade->OrderLocalID), QString(pTrade->OrderRef), QString(pTrade->TradeTime));
#ifdef _DEBUG
		QString data = QString(tr("[CTP| OnRtnTrade]  InvestorId: ")) + QString(pTrade->InvestorID) + tr("  OrderSysID: ") + QString(pTrade->OrderSysID)
			+ tr("  InstrumentId: ") + QString(pTrade->InstrumentID) + tr("  Direction: ") + QString(pTrade->Direction) + tr("  TradePrice: ") + 
			QString::number(pTrade->Price) + tr("  TradeVolume: ") + QString::number(pTrade->Volume) + tr("  TradeTime: ") + QString(pTrade->TradeTime)
			+ tr("  OrderRef: ") + QString(pTrade->OrderRef);
		USTPLogger::saveData(data);
#endif
	}
}

void CTPTradeSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{	
	if (pOrder != NULL){
		emit onCTPRtnOrder(QString(pOrder->OrderLocalID), QString(pOrder->OrderRef), QString(pOrder->InstrumentID), pOrder->Direction, pOrder->LimitPrice, pOrder->VolumeTotalOriginal,
			pOrder->VolumeTotal, pOrder->VolumeTraded, pOrder->CombOffsetFlag[0], pOrder->OrderPriceType, pOrder->CombHedgeFlag[0], pOrder->OrderStatus,
			QString(pOrder->BrokerID), QString(pOrder->ExchangeID), QString(pOrder->InvestorID), QString(pOrder->OrderSysID), QString(pOrder->StatusMsg), 
			pOrder->TimeCondition, pOrder->RequestID);
#ifdef _DEBUG
		QString data = QString(tr("[CTP| OnRtnOrder]  InvestorId: ")) + QString(pOrder->InvestorID) + tr("  OrderRef: ") + QString(pOrder->OrderRef)
			+ tr("  InstrumentId: ") + QString(pOrder->InstrumentID) + tr("  Direction: ") + QString(pOrder->Direction) + tr("  OrderPrice: ") + 
			QString::number(pOrder->LimitPrice) + tr("  OrderStatus: ") + QString(pOrder->OrderStatus) + tr("  OrderSysID: ") + QString(pOrder->OrderSysID);
		USTPLogger::saveData(data);
#endif
	}
}

void CTPTradeSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if (pInputOrder != NULL && pRspInfo != NULL){
		emit onCTPErrRtnOrderInsert(QString(pInputOrder->UserID), QString(pInputOrder->BrokerID), pInputOrder->Direction, QString(pInputOrder->GTDDate), pInputOrder->CombHedgeFlag[0],
			QString(pInputOrder->InstrumentID), QString(pInputOrder->InvestorID), pInputOrder->CombOffsetFlag[0], pInputOrder->OrderPriceType, pInputOrder->TimeCondition,
			QString(pInputOrder->OrderRef), pInputOrder->LimitPrice, pInputOrder->VolumeTotalOriginal, pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg), pInputOrder->RequestID);
#ifdef _DEBUG
		QString data = QString(tr("[CTP| ErrRtnOrderInsert]  InvestorId: ")) + QString(pInputOrder->InvestorID) + tr("  OrderRef: ") + QString(pInputOrder->OrderRef)
			+ tr("  InstrumentId: ") + QString(pInputOrder->InstrumentID) + tr("  Direction: ") + QString(pInputOrder->Direction) + tr("  OrderPrice: ") + 
			QString::number(pInputOrder->LimitPrice) + tr("  OrderVolume: ") + QString::number(pInputOrder->VolumeTotalOriginal) + tr("  ErrorMsg: ") +  QString::fromLocal8Bit(pRspInfo->ErrorMsg);
		USTPLogger::saveData(data);
#endif
	}
}

void CTPTradeSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	if (pOrderAction != NULL && pRspInfo != NULL){
		emit onCTPErrRtnOrderAction(pOrderAction->ActionFlag, QString(pOrderAction->BrokerID), QString(pOrderAction->ExchangeID), QString(pOrderAction->InvestorID),
			QString(pOrderAction->OrderSysID), QString(pOrderAction->ActionLocalID), QString(pOrderAction->OrderRef), pOrderAction->LimitPrice, 
			pOrderAction->VolumeChange, pRspInfo->ErrorID, QString::fromLocal8Bit(pRspInfo->ErrorMsg), pOrderAction->RequestID);
#ifdef _DEBUG
		QString data = QString(tr("[CTP| ErrRtnOrderAction]  InvestorId: ")) + QString(pOrderAction->InvestorID) + tr("  OrderSysID: ") + QString(pOrderAction->OrderSysID)
			+ tr("  ActionLocalID: ") + QString(pOrderAction->ActionLocalID) + tr("  OrderRef: ") + QString(pOrderAction->OrderRef) + tr("  ErrorMsg: ") + 
			QString::fromLocal8Bit(pRspInfo->ErrorMsg);
		USTPLogger::saveData(data);
#endif
	}
}

void CTPTradeSpi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus)
{
	if (pInstrumentStatus != NULL){
		emit onCTPRtnInstrumentStatus(QString(pInstrumentStatus->ExchangeID), QString(pInstrumentStatus->InstrumentID), pInstrumentStatus->InstrumentStatus);
	}
}

void CTPTradeSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInstrument != NULL ){
		emit onCTPRspQryInstrument((pInstrument->ExchangeID), QString(pInstrument->ProductID), QString(pInstrument->InstrumentID),
			pInstrument->PriceTick, pInstrument->VolumeMultiple, pInstrument->MaxMarketOrderVolume, bIsLast);
	}

}

void CTPTradeSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(pInvestorPosition == NULL){
		emit onCTPRspQryInvestorPosition("", -1, 0, 0, '1', "", "","", bIsLast);
		return;
	}
	if (pInvestorPosition != NULL){	
		emit onCTPRspQryInvestorPosition(QString(pInvestorPosition->InstrumentID), pInvestorPosition->PosiDirection, pInvestorPosition->Position, 
			pInvestorPosition->YdPosition, pInvestorPosition->HedgeFlag, QString(pInvestorPosition->BrokerID), QString(pInvestorPosition->TradingDay),
			QString(pInvestorPosition->InvestorID), bIsLast);
#ifdef _DEBUG
		QString data = QString(tr("[CTP| RspQryInvestorPosition]  InvestorId: ")) + QString(pInvestorPosition->InvestorID) + tr("  InstrumentId: ") + QString(pInvestorPosition->InstrumentID)
			+ tr("  Direction: ") + QString(pInvestorPosition->PosiDirection) + tr("  Postion: ") + QString::number(pInvestorPosition->Position) + tr("  YPostion: ") +
			QString::number(pInvestorPosition->YdPosition);
		USTPLogger::saveData(data);
#endif
	}
}

#include "moc_CTPTradeSpi.cpp"