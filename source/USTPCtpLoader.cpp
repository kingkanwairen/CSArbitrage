#include "USTPCtpLoader.h"
#include "USTPConfig.h"
#include <QtXml/QDomDocument>
#include <QtCore/QDateTime>
#include "CTPMdApi.h"
#include "SgitMdApi.h"
#include "CTPTradeApi.h"
#include "SgitTradeApi.h"
#include "USTPMutexId.h"
#include "USTPLogger.h"
#include "USTPProfile.h"


USTPCtpLoader* USTPCtpLoader::mThis = NULL;

bool USTPCtpLoader::initialize()
{	
	mThis = new USTPCtpLoader();
	if (!mThis->m_pBase64->decodeLicense(SYSTEM_LICESE_PATH)){
		return false;
	}
	if(!mThis->m_pBase64->getDateIsValid(mThis->mDateTime, mThis->mValidDateTime)){
		return false;
	}
//	mThis->mAuthUser = mThis->m_pBase64->getUserId();
	if (!mThis->openTradeLog()){
		return false;
	}
	if (!mThis->openProfileFile()){
		return false;
	}
	if (!mThis->loadXMLFile(SYSTEM_FILE_PATH)){
		return false;
	}
	return true;
}

bool USTPCtpLoader::finalize()
{
	if (!mThis->closeTradeLog()){
		return false;
	}

	if (!mThis->closeProfileFile()){
		return false;
	}

	if (mThis) {
		delete mThis;
		mThis = NULL;
	}
	return true;
}

bool USTPCtpLoader::start()
{
	if (!mThis->startCtpMdThread()){
		return false;
	}
	if (!mThis->startCtpTradeThread()){
		return false;
	}

	if (!mThis->startSgitMdThread()){
		return false;
	}
	if (!mThis->startSgitTradeThread()){
		return false;
	}

	if (!CTPMdApi::initilize(mThis->m_pAMdApi)){
		return false;
	}

	if (!SgitMdApi::initilize(mThis->m_pBMdApi)){
		return false;
	}

	if(!CTPTradeApi::initialize(mThis->m_pUserATradeApi)){
		return false;
	}
	if(!SgitTradeApi::initialize(mThis->m_pUserBTradeApi)){
		return false;
	}
	return true;
}

bool USTPCtpLoader::stop()
{	
	if (!CTPMdApi::finalize()){
		return false;
	}
	if(!CTPTradeApi::finalize()){
		return false;
	}

	if (!SgitMdApi::finalize()){
		return false;
	}
	if(!SgitTradeApi::finalize()){
		return false;
	}

	if(!USTPMutexId::finalize()){
		return false;
	}

	if (mThis->m_pAMdApi != NULL){
		mThis->m_pAMdApi->Release();
		mThis->m_pAMdApi = NULL;
	}

	if (mThis->m_pBMdApi != NULL){
		mThis->m_pBMdApi->Release();
		mThis->m_pBMdApi = NULL;
	}

	if (mThis->m_pUserATradeApi != NULL){
		mThis->m_pUserATradeApi->Release();
		mThis->m_pUserATradeApi = NULL;
	}
	if (mThis->m_pUserBTradeApi != NULL){
		mThis->m_pUserBTradeApi->Release();
		mThis->m_pUserBTradeApi = NULL;
	}
	return true;
}

USTPCtpLoader::USTPCtpLoader()
{	
	m_pBase64 = new USTPBase64;
}

USTPCtpLoader::~USTPCtpLoader()
{
	delete m_pAMdSpi;
	delete m_pBMdSpi;
	delete m_pUserATradeSpi;
	delete m_pUserBTradeSpi;
	delete m_pBase64;
}

bool USTPCtpLoader::loadXMLFile(const QString& path)
{
	QFile file(path);  
	if (!file.open(QFile::ReadOnly | QFile::Text)) {  
		return false; 
	}  

	QDomDocument domDocument;  
	if (!domDocument.setContent(&file, true)) {  
		file.close();  
		return false;  
	}  
	QDomElement tree = domDocument.documentElement();
	QDomNodeList usersItemlist = tree.childNodes(); 
	//AÕËºÅ
	QString name = usersItemlist.at(0).nodeName();
	QDomNodeList userAItemlist = usersItemlist.at(0).childNodes();  
	QDomNode nodeBroker = userAItemlist.at(0);
	QString broker = nodeBroker.toElement().attribute("name");
	if (broker.compare("broker") == 0){
		mThis->mABrokerId = nodeBroker.toElement().attribute("value");
	}
	QDomNode nodeOms = userAItemlist.at(1);
	QString omsPath = nodeOms.toElement().attribute("name");
	if (omsPath.compare("oms") == 0){
		mThis->mATradePath = nodeOms.toElement().attribute("value");
	}

	QDomNode nodeAMd = userAItemlist.at(2);
	QString mdAPath = nodeAMd.toElement().attribute("name");
	if (mdAPath.compare("md") == 0){
		mThis->mAMdPath = nodeAMd.toElement().attribute("value");
	}

	//BÕËºÅ
	QDomNodeList userBItemlist = usersItemlist.at(1).childNodes();  
	QDomNode nodeBBroker = userBItemlist.at(0);
	QString brokerB = nodeBBroker.toElement().attribute("name");
	if (brokerB.compare("broker") == 0){
		mThis->mBBrokerId = nodeBBroker.toElement().attribute("value");
	}
	QDomNode nodeBOms = userBItemlist.at(1);
	QString omsBPath = nodeBOms.toElement().attribute("name");
	if (omsBPath.compare("oms") == 0){
		mThis->mBTradePath = nodeBOms.toElement().attribute("value");
	}
	QDomNode nodeBMd = userBItemlist.at(2);
	QString mdBPath = nodeBMd.toElement().attribute("name");
	if (mdBPath.compare("md") == 0){
		mThis->mBMdPath = nodeBMd.toElement().attribute("value");
	}

	if (mThis->mABrokerId.isEmpty() || mThis->mAMdPath.isEmpty() || mThis->mBMdPath.isEmpty() || mThis->mATradePath.isEmpty() || mThis->mBBrokerId.isEmpty() || mThis->mBTradePath.isEmpty()){
		return false;
	}
	return true;  
}

bool USTPCtpLoader::openTradeLog()
{
	if (USTPLogger::initialize()){
		QDateTime current_date_time = QDateTime::currentDateTime();
		QString current_date = current_date_time.toString("yyyy-MM-dd");
		QString logName = tr(LOG_FILE) + current_date + tr(".txt");
		USTPLogger::setFilePath(true, logName);
		return true;
	}
	return false;
}

bool USTPCtpLoader::closeTradeLog()
{
	if (USTPLogger::finalize()){
		return true;
	}
	return false;
}

bool USTPCtpLoader::openProfileFile()
{
	if (USTPProfile::initialize())
		return true;
	return false;
}

bool USTPCtpLoader::closeProfileFile()
{
	if (USTPProfile::finalize()){
		return true;
	}
	return false;
}

bool USTPCtpLoader::startCtpMdThread()
{
	mThis->m_pAMdApi = CThostFtdcMdApi::CreateFtdcMdApi("../log/CTP_Md/");
	m_pAMdSpi = new CTPMdSpi();
	mThis->m_pAMdApi->RegisterFront(mThis->mAMdPath.toLatin1().data());
	mThis->m_pAMdApi->RegisterSpi(mThis->m_pAMdSpi);
	mThis->m_pAMdApi->Init();
	return true;
}

bool  USTPCtpLoader::startSgitMdThread()
{
	mThis->m_pBMdApi = CSgitFtdcMdApi::CreateFtdcMdApi("../log/Sgit_Md/");
	m_pBMdSpi = new SgitMdSpi(mThis->m_pBMdApi);
	mThis->m_pBMdApi->SubscribeMarketTopic (Sgit_TERT_QUICK);
	mThis->m_pBMdApi->RegisterFront(mThis->mBMdPath.toLatin1().data());
	mThis->m_pBMdApi->RegisterSpi(mThis->m_pBMdSpi);
	mThis->m_pBMdApi->Init(false);
	return true;
}

bool USTPCtpLoader::startCtpTradeThread()
{
	mThis->m_pUserATradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi("../log/CTP_Trade/");	
	m_pUserATradeSpi = new CTPTradeSpi();
	mThis->m_pUserATradeApi->SubscribePublicTopic(THOST_TERT_QUICK);
	mThis->m_pUserATradeApi->SubscribePrivateTopic(THOST_TERT_QUICK);
	mThis->m_pUserATradeApi->RegisterFront(mThis->mATradePath.toLatin1().data());	
	mThis->m_pUserATradeApi->RegisterSpi(m_pUserATradeSpi);
	mThis->m_pUserATradeApi->Init();
	return true;
}

bool USTPCtpLoader::startSgitTradeThread()
{
	mThis->m_pUserBTradeApi = CSgitFtdcTraderApi::CreateFtdcTraderApi("../log/Sgit_Trade/");
	m_pUserBTradeSpi = new SgitTradeSpi(mThis->m_pUserBTradeApi);
	mThis->m_pUserBTradeApi->SubscribePublicTopic(Sgit_TERT_QUICK);
	mThis->m_pUserBTradeApi->SubscribePrivateTopic(Sgit_TERT_QUICK);
	mThis->m_pUserBTradeApi->RegisterFront(mThis->mBTradePath.toLatin1().data());	
	mThis->m_pUserBTradeApi->RegisterSpi(m_pUserBTradeSpi);
	mThis->m_pUserBTradeApi->Init(false);
	return true;
}

QString USTPCtpLoader::getUserABrokerId()
{
	return mThis->mABrokerId;
}

QString USTPCtpLoader::getUserBBrokerId()
{
	return mThis->mBBrokerId;
}


QString USTPCtpLoader::getAMdPath()
{
	return mThis->mAMdPath;
}

QString USTPCtpLoader::getBMdPath()
{
	return mThis->mBMdPath;
}

QString USTPCtpLoader::getUserATradePath()
{
	return mThis->mATradePath;
}

QString USTPCtpLoader::getUserBTradePath()
{
	return mThis->mBTradePath;
}

CThostFtdcMdApi* USTPCtpLoader::getAMdApi()
{
	return mThis->m_pAMdApi;
}

CSgitFtdcMdApi* USTPCtpLoader::getBMdApi()
{
	return mThis->m_pBMdApi;
}

CThostFtdcTraderApi* USTPCtpLoader::getUserATradeApi()
{
	return mThis->m_pUserATradeApi;
}

CSgitFtdcTraderApi* USTPCtpLoader::getUserBTradeApi()
{
	return mThis->m_pUserBTradeApi;
}

CTPMdSpi* USTPCtpLoader::getAMdSpi()
{
	return mThis->m_pAMdSpi;
}

SgitMdSpi* USTPCtpLoader::getBMdSpi()
{
	return mThis->m_pBMdSpi;
}

CTPTradeSpi* USTPCtpLoader::getUserATradeSpi()
{
	return mThis->m_pUserATradeSpi;
}

SgitTradeSpi* USTPCtpLoader::getUserBTradeSpi()
{
	return mThis->m_pUserBTradeSpi;
}

QString USTPCtpLoader::getDateTime()
{
	return mThis->mDateTime;
}

QString USTPCtpLoader::getValidDateTime()
{
	return mThis->mValidDateTime;
}