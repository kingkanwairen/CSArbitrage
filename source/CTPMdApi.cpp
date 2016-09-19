#include "CTPMdApi.h"
#define CHAR_BUF_SIZE 128

CTPMdApi* CTPMdApi::mThis = NULL;

CTPMdApi::CTPMdApi()
{
	m_pMdApi = NULL;
}

CTPMdApi::~ CTPMdApi()
{
	
}

bool CTPMdApi::initilize(CThostFtdcMdApi* pMdApi)
{	
	mThis = new CTPMdApi();
	mThis->m_pMdApi = pMdApi;
	return true;
}

bool  CTPMdApi:: finalize()
{
	if (mThis != NULL){
		delete mThis;
		mThis = NULL;
	}
	return true;
}

int CTPMdApi::reqUserLogin(const QString& brokerId, const QString& userId, const QString& password)
{
	if (mThis->m_pMdApi != NULL){
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, brokerId.toStdString().data());
		strcpy(req.UserID, userId.toStdString().data());
		strcpy(req.Password, password.toStdString().data());
		int nResult = mThis->m_pMdApi->ReqUserLogin(&req, ++mThis->nRequestId);
		return nResult;
	}
	return -1;
}

int CTPMdApi::subMarketData(const QString& instrumentId)
{
	if(mThis->m_pMdApi != NULL){
		char* pInstrument = new char[CHAR_BUF_SIZE];
		char* ppInstrumentID[] = {pInstrument};
		strcpy_s(pInstrument, CHAR_BUF_SIZE, instrumentId.toStdString().data());
		int nResult = mThis->m_pMdApi->SubscribeMarketData(ppInstrumentID,  1);
		delete[]pInstrument;
		pInstrument = NULL;
		return nResult;
	}
	return -1;
}

#include "moc_CTPMdApi.cpp"