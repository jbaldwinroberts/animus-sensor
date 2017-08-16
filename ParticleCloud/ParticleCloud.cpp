#ifndef __PARTICLE_CLOUD_CPP_
#define __PARTICLE_CLOUD_CPP_

#include "ParticleCloud.h"

ParticleCloud::ParticleCloud() {}

ParticleCloud::~ParticleCloud() {}

void ParticleCloud::begin()
{
    _logger.logMessage(INFO_LOG, "Started Particle Cloud initialisation");

    _logger.logMessage(INFO_LOG, "Finished Particle Cloud initialisation");
}

bool ParticleCloud::connect()
{
    _logger.logMessage(INFO_LOG, "Connecting to Particle cloud BLOCKING");
    WiFi.on();
    WiFi.connect();
    Particle.connect();
    if(waitFor(Particle.connected, CLOUD_CONNECTION_TIMEOUT))
    {
        _logger.logMessage(INFO_LOG, "Connected to Particle cloud BLOCKING");
        return true;
    }
    else
    {
        _logger.logMessage(WARN_LOG, "Connecting timed out");
        disconnect();
        return false;
    }
}

void ParticleCloud::disconnect()
{
    _logger.logMessage(INFO_LOG, "Disconnecting");
    Particle.disconnect();
    WiFi.disconnect();
    WiFi.off();
    _logger.logMessage(INFO_LOG, "Disconnected");
    _startedConnecting = 0;
    _particleConnecting = false;
}

ParticleCloud _particleCloud;

#endif
