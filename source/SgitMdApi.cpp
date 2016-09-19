#include "SgitMdApi.h"
#define CHAR_BUF_SIZE 128

SgitMdApi* SgitMdApi::mThis = NULL;

SgitMdApi::SgitMdApi()
{
	m_pMdApi = NULL;
}

SgitMdApi::~ SgitMdApi()
{
	
}

bool SgitMdApi::initilize(CSgitFtdcMdApi* pMdApi)
{	
	mThis = new SgitMdApi();
	mThis->m_pMdApi = pMdApi;
	return true;
}

bool  SgitMdApi:: finalize()
{
	if (mThis != NULL){
		delete mThis;
		mThis = NULL;
	}
	return true;
}

int SgitMdApi::reqUserLogin(const QString& brokerId, const QString& userId, const QString& password)
{
	if (mThis->m_pMdApi != NULL){
		CSgitFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
	//	strcpy(req.BrokerID, brokerId.toStdString().data());
		strcpy(req.UserID, userId.toStdString().data());
		strcpy(req.Password, password.toStdString().data());
		int nResult = mThis->m_pMdApi->ReqUserLogin(&req, ++mThis->nRequestId);
		return nResult;
	}
	return -1;
}

int SgitMdApi::subMarketData(const QString& instrumentId)
{
	if(mThis->m_pMdApi != NULL){
		CSgitSubQuotField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.ContractID, instrumentId.toStdString().data());
		int nResult = mThis->m_pMdApi->SubQuot(&req);
		return nResult;
	}
	return -1;
}

#include "moc_SgitMdApi.cpp"