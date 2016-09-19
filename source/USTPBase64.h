#ifndef ___BASE64_H___   
#define ___BASE64_H___   

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <string>
using namespace std;

class USTPBase64
{
public:
	/*编码
	DataByte
	[in]输入的数据长度,以字节为单位
	*/
	string Encode(const unsigned char* Data,int DataByte);
	/*解码
	DataByte
	[in]输入的数据长度,以字节为单位
	OutByte
	[out]输出的数据长度,以字节为单位,请不要通过返回值计算
	输出数据的长度
	*/
	string Decode(const char* Data,int DataByte,int& OutByte);

	bool decodeLicense(const QString& path);

	QString getUserId();

	QString getDateTime();

	bool getDateIsValid(QString& dateTime, QString& validTime);

private:
	QStringList mDecodeLicense;

};

#endif // ___BASE64_H___  
