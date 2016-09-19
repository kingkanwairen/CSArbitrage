#ifndef USTP_RSP_KEYDOWN_H
#define USTP_RSP_KEYDOWN_H

#include "HockDll.h"
#include <QtCore/QObject>


class USTPRspKeyDown : public QObject, public KeyHockSpi
{
	Q_OBJECT

public:
	USTPRspKeyDown(void);
	virtual ~USTPRspKeyDown(void);

	virtual void OnKeyDown(int key);


signals:
	void onKeyDownHock(const int& key);
};

#endif