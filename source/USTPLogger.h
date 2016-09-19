#ifndef USTP_LOGGER_H
#define USTP_LOGGER_H
#include <QtCore/QFile>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

class USTPLogger : public QObject
{
	Q_OBJECT
public:
	USTPLogger();

	~USTPLogger();

	static bool initialize();

	static bool finalize();

	static void saveData(const QString& data);

	static void setFilePath(bool isSave, const QString& path);

private:
	static USTPLogger *mThis;
	bool mIsSave;
	QFile mLogFile;
	QMutex mLogMutex;
};

#endif 