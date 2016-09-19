#include "SgitTradeApi.h"
#include <QtCore/QDebug>
#include<math.h>
#include "USTPMutexId.h"

#define CHAR_BUF_SIZE 128
#define INS_TICK 0.2
SgitTradeApi* SgitTradeApi::mThis = NULL;

bool SgitTradeApi::initialize(CSgitFtdcTraderApi* pTradeApi)
{	
	mThis = new SgitTradeApi();
	mThis->m_pTradeApi = pTradeApi;
	return true;
}

bool SgitTradeApi::finalize()
{
	if(mThis != NULL){
		delete mThis;
		mThis = NULL;
	}
	return true;
}

SgitTradeApi::SgitTradeApi()
{
	m_pTradeApi = NULL;
}

SgitTradeApi::~SgitTradeApi()
{

}

int SgitTradeApi::reqUserLogin(const int& reqId, const QString& brokerId, const QString& userId, const QString& password)
{
	if (mThis->m_pTradeApi != NULL){
		CSgitFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.TradingDay, mThis->m_pTradeApi->GetTradingDay());
	//	strcpy(req.BrokerID, brokerId.toStdString().data());
		strcpy(req.UserID, userId.toStdString().data());
		strcpy(req.Password, password.toStdString().data());
		int nResult = mThis->m_pTradeApi->ReqUserLogin(&req, reqId);
		mThis->mUserId = userId;
		return nResult;
	}
	return -1;
}


int SgitTradeApi::reqUserLogout(const int& reqId, const QString& brokerId, const QString& userId)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;
	CSgitFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
//	strcpy(req.BrokerID, brokerId.toStdString().data());
	strcpy(req.UserID, userId.toStdString().data());
	int nResult = mThis->m_pTradeApi->ReqUserLogout(&req, reqId);
	return nResult;
}


int SgitTradeApi::reqOrderInsert(const int& reqId, QString& orderRef, const QString& brokerId, const QString& userId, const QString& investorId, const QString& instrumentId, const char& priceType, 
								 const char& timeCondition, const double& orderPrice, const int& volume, const char& direction, const char& offsetFlag, const char& hedgeFlag, const char& volumeCondition)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;

	QMutexLocker locker(&mThis->mOrderMutex);
	CSgitFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
//	strcpy(req.BrokerID, brokerId.toStdString().data());
	req.Direction = direction;
	req.ForceCloseReason = Sgit_FTDC_FCC_NotForceClose;
	req.CombOffsetFlag[0] = offsetFlag;
	sprintf(req.OrderRef, "%012d", USTPMutexId::getOrderRef(userId));
	strcpy(req.InstrumentID, instrumentId.toStdString().data());
	strcpy(req.InvestorID, investorId.toStdString().data());
	strcpy(req.UserID, userId.toStdString().data());
	orderRef = QString(req.OrderRef);
	req.IsAutoSuspend = 0;
	req.LimitPrice = orderPrice;
	req.MinVolume = 1;
	req.CombHedgeFlag[0] = hedgeFlag;
	req.OrderPriceType = priceType;
	req.TimeCondition = timeCondition;
	req.VolumeTotalOriginal = volume;
	req.VolumeCondition = volumeCondition;
	req.StopPrice = 0;
	req.ContingentCondition = Sgit_FTDC_CC_Immediately;
	req.RequestID = reqId;
	int nResult = mThis->m_pTradeApi->ReqOrderInsert(&req, reqId);
	return nResult;
}

int SgitTradeApi::reqOrderAction(const int& reqId, const QString& brokerId, const QString& userId, const QString& investorId, const QString& instrumentId, const QString& orderRef,
								 const QString& orderSysId, const QString& exchangeId)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;

	QMutexLocker locker(&mThis->mOrderMutex);
	CSgitFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
//	strcpy(req.BrokerID, brokerId.toStdString().data());
	strcpy(req.UserID, userId.toStdString().data());
	strcpy(req.InvestorID, investorId.toStdString().data());
	strcpy(req.OrderRef, orderRef.toStdString().data());
	strcpy(req.OrderSysID, orderSysId.toStdString().data());
	strcpy(req.InstrumentID, instrumentId.toStdString().data());
	strcpy(req.ExchangeID, exchangeId.toStdString().data());
//	req.FrontID = frontId;
//	req.SessionID = sessionId;
	req.ActionFlag = Sgit_FTDC_AF_Delete;
	int nResult = mThis->m_pTradeApi->ReqOrderAction(&req, reqId);
	return nResult;
}

int SgitTradeApi::reqQryInstrument(const int& reqId, const QString& instrumentId, const QString& exchangeId, const QString& productId)
{	
	if (mThis->m_pTradeApi != NULL){
		CSgitFtdcQryInstrumentField req;
		memset(&req, 0,sizeof(req));
		strcpy(req.InstrumentID, instrumentId.toStdString().data());
		strcpy(req.ExchangeID, exchangeId.toStdString().data());
		strcpy(req.ProductID, productId.toStdString().data());
		int nResult = mThis->m_pTradeApi->ReqQryInstrument(&req, reqId);
		return nResult;
	}
	return -1;
}

int SgitTradeApi::reqQryInvestorPosition(const int& reqId, const QString& brokerId, const QString& investorId, const QString& instrumentId)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;
	CSgitFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	//strcpy(req.BrokerID, brokerId.toStdString().data());
	strcpy(req.InvestorID, investorId.toStdString().data());
	strcpy(req.InstrumentID, instrumentId.toStdString().data());	
	int nResult = mThis->m_pTradeApi->ReqQryInvestorPosition(&req, reqId);
	return nResult;
}

#include "moc_SgitTradeApi.cpp"