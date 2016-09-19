#include "USTPRspKeyDown.h"
#include <QtCore/QDebug>

USTPRspKeyDown::USTPRspKeyDown(void)
{

}
USTPRspKeyDown:: ~USTPRspKeyDown(void)
{

}

void USTPRspKeyDown::OnKeyDown(int key)
{
	emit onKeyDownHock(key);
}

#include "moc_USTPRspKeyDown.cpp"