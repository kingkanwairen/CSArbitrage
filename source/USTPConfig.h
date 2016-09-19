#include <QtCore/QString>

#ifndef  _DEBUG
#define _DEBUG
#endif

#ifndef _MD_DEBUG
#define _MD_DEBUG
#endif

#define MARKET_DOCK_WIDGET tr("组合套利")
#define ORDER_DOCK_WIDGET tr("委托信息")
#define TRADE_DOCK_WIDGET tr("成交信息")
#define A_POSITION_DOCK_WIDGET tr("CTP账号持仓")
#define B_POSITION_DOCK_WIDGET tr("SGIT账号持仓")
#define CANCEL_DOCK_WIDGET tr("报单管理")
#define STRATEGY_DOCK_WIDGET tr("套利策略")
#define UNILATERAL_DOCK_WIDGET tr("单边循环")
#define MANUAL_ORDER_DOCK_WIDGET tr("手工报单")

#define MAIN_WINDOW_TILTE tr("套利交易系统")
#define LOGIN_WINDOW_TITLE tr("登录")
#define STRATERGY_WINDOW_TITLE tr("套利设置")
#define UNILATERAL_WINDOW_TITLE tr("单边设置")
#define KEY_WINDOW_TITLE tr("快捷键设置")

#define COMBO_MARKET_WINDOW_TITLE tr("组合合约设置")
#define COMBO_MARKET_WINDOW_WIDTH 400
#define COMBO_MARKET_WINDOW_HEIGHT 300

#define  OMS_ORDER_EVENT tr("OmsOrder")
#define  OMS_TRADE_EVENT tr("OmsTrade")
#define  OMS_POSITION_EVENT tr("OmsPosition")
#define  OMS_LOG_EVENT tr("OmsLog")

#define  STRATEGY_WIDGET_TAB_HEAD_0 tr("合约一")
#define  STRATEGY_WIDGET_TAB_HEAD_1 tr("合约二")
#define  STRATEGY_WIDGET_TAB_HEAD_2 tr("订单类型")
#define  STRATEGY_WIDGET_TAB_HEAD_3 tr("买卖")
#define  STRATEGY_WIDGET_TAB_HEAD_4 tr("开平")
#define  STRATEGY_WIDGET_TAB_HEAD_5 tr("CTP委托量")
#define  STRATEGY_WIDGET_TAB_HEAD_6 tr("SGIT委托量")
#define  STRATEGY_WIDGET_TAB_HEAD_7 tr("价格")
#define  STRATEGY_WIDGET_TAB_HEAD_8 tr("循环档")
#define  STRATEGY_WIDGET_TAB_HEAD_9 tr("报单提交")
#define  STRATEGY_WIDGET_TAB_HEAD_10 tr("投机套保")
#define  STRATEGY_WIDGET_TAB_HEAD_11 tr("删除埋单")
#define  STRATEGYHEAD_LENGTH 12
#define  SAVE_ITEM_LENGTH 10

#define  ORDER_WIDGET_TAB_HEAD_0 tr("本地编号")
#define  ORDER_WIDGET_TAB_HEAD_1 tr("合约")
#define  ORDER_WIDGET_TAB_HEAD_2 tr("订单价格")
#define  ORDER_WIDGET_TAB_HEAD_3 tr("订单类型")
#define  ORDER_WIDGET_TAB_HEAD_4 tr("委托状态")
#define  ORDER_WIDGET_TAB_HEAD_5 tr("买/卖")
#define  ORDER_WIDGET_TAB_HEAD_6 tr("委托价格")
#define  ORDER_WIDGET_TAB_HEAD_7 tr("委托量")
#define  ORDER_WIDGET_TAB_HEAD_8 tr("剩余量")
#define  ORDER_WIDGET_TAB_HEAD_9 tr("成交量")
#define  ORDER_WIDGET_TAB_HEAD_10 tr("开/平")
#define  ORDER_WIDGET_TAB_HEAD_11 tr("价格类型")
#define  ORDER_WIDGET_TAB_HEAD_12 tr("套保/投机")
#define  ORDER_HEAD_LENGTH 13

#define  CANCEL_WIDGET_TAB_HEAD_0 tr("删除报单")
#define  CANCEL_WIDGET_TAB_HEAD_1 tr("订单编号")
#define  CANCEL_WIDGET_TAB_HEAD_2 tr("订单类型")
#define  CANCEL_WIDGET_TAB_HEAD_3 tr("合约")
#define  CANCEL_WIDGET_TAB_HEAD_4 tr("买/卖")
#define  CANCEL_WIDGET_TAB_HEAD_5 tr("委托量")
#define  CANCEL_WIDGET_TAB_HEAD_6 tr("开/平")
#define  CANCEL_WIDGET_TAB_HEAD_7 tr("委托价格")
#define  CANCEL_WIDGET_TAB_HEAD_8 tr("清除列表")
#define  CANCEL_HEAD_LENGTH 9

#define  TRADE_WIDGET_TAB_HEAD_0 tr("成交时间")
#define  TRADE_WIDGET_TAB_HEAD_1 tr("用户账号")
#define  TRADE_WIDGET_TAB_HEAD_2 tr("合约")
#define  TRADE_WIDGET_TAB_HEAD_3 tr("买/卖")
#define  TRADE_WIDGET_TAB_HEAD_4 tr("开/平")
#define  TRADE_WIDGET_TAB_HEAD_5 tr("成交量")
#define  TRADE_WIDGET_TAB_HEAD_6 tr("成交价")
#define  TRADE_WIDGET_TAB_HEAD_7 tr("套保/投机")
#define  TRADE_WIDGET_TAB_HEAD_8 tr("成交编号")
#define  TRADE_HEAD_LENGTH 9

#define  POSITION_WIDGET_TAB_HEAD_0 tr("合约")
#define  POSITION_WIDGET_TAB_HEAD_1 tr("买持仓")
#define  POSITION_WIDGET_TAB_HEAD_2 tr("卖持仓")
#define  POSITION_HEAD_LENGTH 3

#define  DEPTH_WIDGET_TAB_HEAD_0 tr("Key")
#define  DEPTH_WIDGET_TAB_HEAD_1 tr("第一腿")
#define  DEPTH_WIDGET_TAB_HEAD_2 tr("第二腿")
#define  DEPTH_WIDGET_TAB_HEAD_3 tr("买-卖价")
#define  DEPTH_WIDGET_TAB_HEAD_4 tr("卖-买价")
#define  DEPTH_WIDGET_TAB_HEAD_5 tr("买-买价")
#define  DEPTH_WIDGET_TAB_HEAD_6 tr("卖-卖价")
#define  DEPTH_WIDGET_TAB_HEAD_7 tr("买-买量")
#define  DEPTH_WIDGET_TAB_HEAD_8 tr("买-卖量")
#define  DEPTH_WIDGET_TAB_HEAD_9 tr("卖-买量")
#define  DEPTH_WIDGET_TAB_HEAD_10 tr("卖-卖量")
#define  DEPTH_HEAD_LENGTH 11

#define  SPEC_DEPTH_WIDGET_TAB_HEAD_0 tr("合约")
#define  SPEC_DEPTH_WIDGET_TAB_HEAD_1 tr("最新价")
#define  SPEC_DEPTH_WIDGET_TAB_HEAD_2 tr("买价")
#define  SPEC_DEPTH_WIDGET_TAB_HEAD_3 tr("买量")
#define  SPEC_DEPTH_WIDGET_TAB_HEAD_4 tr("卖价")
#define  SPEC_DEPTH_WIDGET_TAB_HEAD_5 tr("卖量")
#define  SPEC_DEPTH_WIDGET_TAB_HEAD_6 tr("成交量")
#define  SPEC_DEPTH_HEAD_LENGTH  7


#define  UNILATERAL_WIDGET_TAB_HEAD_0 tr("单边合约")
#define  UNILATERAL_WIDGET_TAB_HEAD_1 tr("参考合约")
#define  UNILATERAL_WIDGET_TAB_HEAD_2 tr("订单类型")
#define  UNILATERAL_WIDGET_TAB_HEAD_3 tr("买卖")
#define  UNILATERAL_WIDGET_TAB_HEAD_4 tr("平仓类型")
#define  UNILATERAL_WIDGET_TAB_HEAD_5 tr("数量")
#define  UNILATERAL_WIDGET_TAB_HEAD_6 tr("开仓点数")
#define  UNILATERAL_WIDGET_TAB_HEAD_7 tr("报单提交")
#define  UNILATERAL_WIDGET_TAB_HEAD_8 tr("平仓点数")
#define  UNILATERAL_WIDGET_TAB_HEAD_9 tr("对价点数")
#define  UNILATERAL_WIDGET_TAB_HEAD_10 tr("参考点数")
#define  UNILATERAL_WIDGET_TAB_HEAD_11 tr("撤单时间")
#define  UNILATERAL_WIDGET_TAB_HEAD_12 tr("超价档")
#define  UNILATERAL_WIDGET_TAB_HEAD_13 tr("定时撤单")
#define  UNILATERAL_WIDGET_TAB_HEAD_14 tr("循环")
#define  UNILATERAL_WIDGET_TAB_HEAD_15 tr("删除埋单")
#define  UNILATERAL_HEAD_LENGTH 16
#define  SAVE_UNILATERAL_LENGTH 14

#define  EXH_SHFE tr("SHFE")
#define  EXH_CZCE tr("CZCE")

#define  CTP_SERVER tr("CTP")
#define  SGIT_SERVER tr("SGIT")


#define LINK_LABEL_WIDTH 16
#define LINK_LABEL_HEIGHT 16


#define STATUS_LABEL_WIDTH 360
#define STATUS_LABEL_HEIGHT 300
#define STATUS_WINDOW_MAX_WIDTH 360

#define ORDER_WINDOW_MIN_HEIGHT 100
#define TRADE_WINDOW_MIN_HEIGHT 100
#define POSITION_WINDOW_MIN_HEIGHT 100
#define STRAGETY_WINDOW_MIN_HEIGHT 300

#define MARKET_WINDOW_MIN_WIDTH 550
#define ORDER_WINDOW_MIN_WIDTH 550
#define CANCEL_WINDOW_MIN_WIDTH 600
#define TRADE_WINDOW_MIN_WIDTH 500
#define POSITION_WINDOW_MIN_WIDTH 250

#define LOGIN_WINDOW_HEIGHT 350
#define LOGIN_WINDOW_WIDTH 600

#define  ShowInfo(msg) QMessageBox::information(this, tr("提示"), msg)
#define  ShowWarning(msg) QMessageBox::warning(this, tr("提示"), msg)
#define  ShowCritical(msg) QMessageBox::critical(NULL, tr("提示"), msg)


#define LOG_FILE "../log/trade_"
#define SYSTEM_FILE_PATH  "../config/config.xml"
#define SYSTEM_LICESE_PATH  "../config/license.txt"
#define PARAM_FILE_PATH "../config/param.txt"

#define CTP_INITINAL_ERROR -1
#define CTP_LOAD_ERROR -2
#define CTP_STOP_ERROR -3
#define CTP_FINALIZE_ERROR -4

#define VALUE_1 1.0

#define  INIT_VALUE 10000000
#define  VALUE_ACCURACY 0.00001

#define KEY_BID 0
#define KEY_ASK 1
#define KEY_ACTION 2

