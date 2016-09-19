#ifndef USTP_PROFILE_H
#define USTP_PROFILE_H

#include <QtCore/QFile>

class USTPProfile : public QObject
{
	Q_OBJECT
public:
	USTPProfile(){};

	~USTPProfile(){};

	static bool initialize();

	static bool finalize();

	static bool readItem(const QString& itemName, QString& data);

	static bool writeItem(const QString& itemName, QString& data);

private:
	static USTPProfile *mThis;
	QFile mLogFile;
};

#endif 