#include "CTPTradeApi.h"
#include <QtCore/QDebug>
#include<math.h>
#include "USTPMutexId.h"

#define CHAR_BUF_SIZE 128
#define INS_TICK 0.2
CTPTradeApi* CTPTradeApi::mThis = NULL;

bool CTPTradeApi::initialize(CThostFtdcTraderApi* pTradeApi)
{	
	mThis = new CTPTradeApi();
	mThis->m_pTradeApi = pTradeApi;
	return true;
}

bool CTPTradeApi::finalize()
{
	if(mThis != NULL){
		delete mThis;
		mThis = NULL;
	}
	return true;
}

CTPTradeApi::CTPTradeApi()
{
	m_pTradeApi = NULL;
}

CTPTradeApi::~CTPTradeApi()
{

}

int CTPTradeApi::reqUserLogin(const int& reqId, const QString& brokerId, const QString& userId, const QString& password)
{
	if (mThis->m_pTradeApi != NULL){
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, brokerId.toStdString().data());
		strcpy(req.UserID, userId.toStdString().data());
		strcpy(req.Password, password.toStdString().data());
		int nResult =mThis->m_pTradeApi->ReqUserLogin(&req, reqId);
		mThis->mUserId = userId;
		return nResult;
	}
	return -1;
}


int CTPTradeApi::reqUserLogout(const int& reqId, const QString& brokerId, const QString& userId)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;

	CThostFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, brokerId.toStdString().data());
	strcpy(req.UserID, userId.toStdString().data());
	int nResult = mThis->m_pTradeApi->ReqUserLogout(&req, reqId);
	return nResult;
}

int CTPTradeApi::reqSettlementInfoConfirm(const int& reqId, const QString& brokerId, const QString& investorId)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, brokerId.toStdString().data());
	strcpy(req.InvestorID, investorId.toStdString().data());
	int nResult = mThis->m_pTradeApi->ReqSettlementInfoConfirm(&req, reqId);
	return nResult;
}

int CTPTradeApi::reqOrderInsert(const int& reqId, QString& orderRef, const QString& brokerId, const QString& userId, const QString& investorId, const QString& instrumentId, const char& priceType, 
								 const char& timeCondition, const double& orderPrice, const int& volume, const char& direction, const char& offsetFlag, const char& hedgeFlag, const char& volumeCondition)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;

	QMutexLocker locker(&mThis->mOrderMutex);
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, brokerId.toStdString().data());
	req.Direction = direction;
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
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
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	req.RequestID = reqId;
	int nResult = mThis->m_pTradeApi->ReqOrderInsert(&req, reqId);
	return nResult;
}

int CTPTradeApi::reqOrderAction(const int& reqId, const QString& brokerId, const QString& userId, const QString& investorId, const QString& instrumentId, const QString& orderRef,
								 const int& frontId, const int& sessionId)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;

	QMutexLocker locker(&mThis->mOrderMutex);
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, brokerId.toStdString().data());
	strcpy(req.UserID, userId.toStdString().data());
	strcpy(req.InvestorID, investorId.toStdString().data());
	strcpy(req.OrderRef, orderRef.toStdString().data());
	strcpy(req.InstrumentID, instrumentId.toStdString().data());
	req.FrontID = frontId;
	req.SessionID = sessionId;
	req.ActionFlag = THOST_FTDC_AF_Delete;
	int nResult = mThis->m_pTradeApi->ReqOrderAction(&req, reqId);
	return nResult;
}

int CTPTradeApi::reqQryInstrument(const int& reqId, const QString& instrumentId, const QString& exchangeId, const QString& productId)
{	
	if (mThis->m_pTradeApi != NULL){
		CThostFtdcQryInstrumentField req;
		memset(&req, 0,sizeof(req));
		strcpy(req.InstrumentID, instrumentId.toStdString().data());
		strcpy(req.ExchangeID, exchangeId.toStdString().data());
		strcpy(req.ProductID, productId.toStdString().data());
		int nResult = mThis->m_pTradeApi->ReqQryInstrument(&req, reqId);
		return nResult;
	}
	return -1;
}

int CTPTradeApi::reqQryInvestorPosition(const int& reqId, const QString& brokerId, const QString& investorId, const QString& instrumentId)
{	
	if( mThis->m_pTradeApi == NULL)
		return -1;
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, brokerId.toStdString().data());
	strcpy(req.InvestorID, investorId.toStdString().data());
	strcpy(req.InstrumentID, instrumentId.toStdString().data());	
	int nResult = mThis->m_pTradeApi->ReqQryInvestorPosition(&req, reqId);
	return nResult;
}

#include "moc_CTPTradeApi.cpp"