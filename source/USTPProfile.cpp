#include "USTPProfile.h"
#include "USTPConfig.h"
#include <QtCore/QTextStream>

USTPProfile *USTPProfile::mThis = NULL;

bool USTPProfile::initialize()
{
	mThis = new USTPProfile();
	return true;
}


bool USTPProfile::finalize()
{	
	if (mThis) {
		delete mThis;
		mThis = NULL;
	}
	return true;
}

bool USTPProfile::readItem(const QString& itemName, QString& data)
{
	QFile file(PARAM_FILE_PATH);  
	if (!file.open(QFile::ReadOnly)) { 
		return false;
	}
	QTextStream stream(&file);
	do {
		data = stream.readLine();
		if(data == itemName){
			data = stream.readLine();
			break;
		}
	} while (!data.isNull());
	file.close();
	if (data.isEmpty())
		return false;
	return true;
}

bool USTPProfile::writeItem(const QString& itemName, QString& data)
{
	QFile file(PARAM_FILE_PATH);  
	if (!file.open(QIODevice::ReadOnly)) { 
		return false;
	}
	QTextStream stream(&file);
	QString saveLine;
	QString line;
	do {
		line = stream.readLine();
		if(line == itemName){
			saveLine += line;
			saveLine += tr("\n");
			line = stream.readLine();
			line.replace(line, data, Qt::CaseSensitive);
		}
		saveLine += line;
		saveLine += tr("\n");
	} while (!line.isNull());
	file.close();

	QFile saveFile(PARAM_FILE_PATH); 
	if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) { 
		return false;
	}
	QTextStream saveStream(&saveFile);
	saveStream << saveLine;
	saveFile.close();
	return true;
}

#include "moc_USTPProfile.cpp"