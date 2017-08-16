#ifndef __ANIMUS_CLOUD_H_
#define __ANIMUS_CLOUD_H_

#include "Global.h"
#include "Reply.h"
#include "jsmnSpark.h"
#include <queue>
#include <unordered_map>

class AnimusCloud
{
  public:
    AnimusCloud();
    virtual ~AnimusCloud();
    void begin();

    void sendHeartbeat();
    void sendRegister();
    void sendGetAccessKey();
    void sendData();
    void sendInvalidData();
    Reply getReply();
    unsigned long getLastTransmitTime();
    void setLastTransmitTime();
    void checkRestart(bool force = false);

  private:
    int control(String command);
    String extractParameter(const String& reply);
    void replyHandler(const char* event, const char* data);
    void parseReply(String reply);
    void handleSuccessMessage(String reply);
    void handleNotFoundErrors(String reply);
    void handleRegErrors(String reply);
    void handleAuthErrors(String reply);
    void handleDataError(String reply);

    bool _restart = false;
    Reply _reply = NA;
    unsigned long _lastTransmitTime = 0;
    int _msgNo = 0;
};

extern AnimusCloud _animusCloud;

#endif
