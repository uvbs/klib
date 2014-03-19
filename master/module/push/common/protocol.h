#pragma once


/*
 协议定义文件：
    协议的描述，服务器与客户端，服务器与服务器的通信说明

    可以使用自动生成协议结构体来弄这个。
*/


// 定义迷你首页系统的版本号
#define MINISYS_VERSION  (0x01)

// 加密算法类型定义 
enum ENCRYPTION {
	E_None,
	E_MD5 = 1,
	E_CRC,
	E_RC4,
	E_RC6,
	E_SHA1,
	E_SELF //自定义加密算法
};

// 操作结果的枚举定义
enum OPERATION_RESULT {
	OP_SUCCESS,
	OP_FAILED,
};

// 控制端到服务器的控制命令
typedef enum C2S {

    // 查询逻辑服务器的地址
    CMD_QUERY_LOGIC_SERVER = 10,
    /*
    UINT       :     uuid;
    */

    // 返回的空闲的逻辑服务器的IP+PORT
    CMD_QUERY_LOGIC_SERVER_ACK,
    /*
    UINT       :     uLogicIP;
    USHORT     :     uLogicUdpPort;
    USHORT     :     uLogicTcpPort;
    */

    // 查询当前最新版本
    CMD_QUERY_CURRENT_VERSION,
    /*
    UINT             :     uCurVerValue;    // 当前版本值
    std::string      :     strVersion;       // 版本的字符串
    */

    // 返回当前的版本号
    CMD_QUERY_CURRENT_VERSION_ACK,
    /*
    std::string      :    strVersion;        // 版本的字符串值
    UINT             :    uVersionValue;     // 版本的数字值
    std::string      :    strUpdateUrl;      // 更新包的地址
    std::string      :    strUpdateSign;     // 数字签名
    BOOL             :    bForceUpdate;      // 强制更新
    BOOL             :    bClientRequest;    // 是否是客户端主动请求的
    */
    
    // 客衣端通知服务器在线消息
    CMD_ONLINE,
    /*
    UINT64         :   uid;
    std::string    :   account;          // 用户的帐号(注册的用户)
    std::string    :   mac;              // mac地址
    std::string    :   channel;          // 渠道
    UINT64         :   last_msg_id;          // 最后收到的消息ID
    std::string    :   login_name;        // 电脑的登陆名
    UINT           :   version;            // 版本值，用数值比较更好些
    */

    // 在线消息的回复消息
    CMD_ONLINE_ACK,
    /*
    BOOL          :    bStartMini;   // 打开迷你首页
    UINT64        :    uuid;         // 生成的唯一标识的ID号
    UINT64        :    msgId;        // 需要展示的消息ID
    */

    // 离线消息，客户端退出的时候发送
    CMD_OFFLINE,
    /*
    UINT64        :   uuid;         // 生成的唯一标识的ID号
    */

    // 离线消息的回复消息，服务器可以选择不回该消息
    CMD_OFFLINE_ACK,
    /*
    */

    // 消息通知协议
    CMD_MESSAGE_NOTIFY,             
    /*
    UINT64        :   msgID;        // 消息ID
    */
	
    // 消息内容协议
    CMD_MESSAGE_CONTENT,
    /*
    UINT64         :   uMsgID;              // 消息ID
    UINT32         :   uMsgType;            // 消息类型
    UINT32         :   uDelayFetch;         // 延迟多长时间获取消息
    UINT32         :   uDelayShow;          // 显示延迟时间（即指需要延迟多少时间显示）
    UINT32         :   uShowTime;           // 显示的时长（如果为0则一直显示）
    std::string    :   strSign;             // 消息内容的签名（计算出内容的md5值,使用签名算法签名）
    std::string    :   strMsgContent;       // 消息内容
    */

    // 消息确认
    CMD_MESSAGE_CONTENT_ACK,
    /*
    UINT64         :       uMsgID;          // 要确认的消息ID
    UINT           :       emOpResult;      // 显示结果
    */


} COMMAND_CLIENT_TO_SERVER;
