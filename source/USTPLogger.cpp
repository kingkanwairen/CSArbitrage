#include "USTPLogger.h"
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>


USTPLogger *USTPLogger::mThis = NULL;

USTPLogger::USTPLogger()
{
	mIsSave = false;
}

USTPLogger::~USTPLogger()
{
	
}

bool USTPLogger::initialize()
{
	mThis = new USTPLogger();
	return true;
}


bool USTPLogger::finalize()
{	
	mThis->mLogFile.close();
	if (mThis) {
		delete mThis;
		mThis = NULL;
	}
	return true;
}

void USTPLogger::saveData(const QString& data)
{
	if(mThis->mIsSave){
		QMutexLocker locker(&mThis->mLogMutex);
		QDateTime current_date_time = QDateTime::currentDateTime();
		QString current_date = current_date_time.toString("yyyy-MM-dd  hh:mm:ss zzz ");
		QTextStream out(&mThis->mLogFile);
		QString showMsg = current_date + tr("[INFO]  ") + data + tr("\n");
		out << showMsg;
	}
}

void USTPLogger::setFilePath(bool isSave, const QString& path)
{
	mThis->mIsSave = isSave;
	if(isSave){
		mThis->mLogFile.close();
		mThis->mIsSave = false;
		mThis->mLogFile.setFileName(path);
		if(mThis->mLogFile.open(QFile::Text | QFile::Append)){
			mThis->mIsSave = true;
		}
	}
}

#include "moc_USTPLogger.cpp"