#ifndef __PARTICLE_CLOUD_H_
#define __PARTICLE_CLOUD_H_

#include "Global.h"

class ParticleCloud
{
  public:
    ParticleCloud();
    virtual ~ParticleCloud();
    void begin();

    bool connect();
    void disconnect();

  private:
    unsigned long _startedConnecting = 0;
    bool _particleConnecting = false;
};

extern ParticleCloud _particleCloud;

#endif
