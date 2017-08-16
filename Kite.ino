#include "Global.h"
#include "neopixel.h"
#include "Cv.h"
#include "GridEye.h"
#include "AbstractState.h"
#include "States.h"
#include "photon-wdgs.h"

//TODO comment in and update
PRODUCT_ID(P_ID);
PRODUCT_VERSION(P_VERSION);

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);
STARTUP(startUp());

Adafruit_NeoPixel* _strip = NULL;
Cv* _cv = NULL;
GridEye* _gridEye = NULL;
AbstractState* _state = NULL;

void startUp()
{
    _strip = new Adafruit_NeoPixel(1, A0, WS2811);
    _strip->begin();
    _strip->setBrightness(64);
    RGB.onChange(ledUpdateHandler);
}

void setup()
{
    Serial.begin(115200);

    Particle.connect();
    if(!waitFor(Particle.connected, CLOUD_CONNECTION_TIMEOUT)) System.reset();

    PhotonWdgs::begin(true, true, 60000, TIMER7);

    _logger.begin();
    _particleCloud.begin();
    _animusCloud.begin();
    _logger.checkSd();
    _externalEeprom.begin();
    _settings.begin();
    _storage.begin();

    _cv = new Cv(8, 4);
    _gridEye = new GridEye();

    if(0 == strcmp(System.version().c_str(), PARTICLE_FIRMWARE_VERSION)) _logger.logMessage(INFO_LOG, "System firmware version matches: " + System.version());
    else _logger.logMessage(ERROR_LOG, "System firmware version does not match: " + System.version());

    pinMode(A1, INPUT_PULLDOWN);
    attachInterrupt(A1, pirTriggeredHandler, RISING);

    _state = new StartingState();
    _state->enter();

    _logger.logMessage(INFO_LOG, "Finished setup");
    _logger.logMessage(INFO_LOG, "Starting flash time");
}

void loop()
{
    PhotonWdgs::tickle();

    //Allow one minute for flashing when first run
    static unsigned long flashingStartTime = millis();
    if(abs(millis() - flashingStartTime) < (60 * 1000))
    {
        Particle.process();
        delay(100);
    }
    else
    {
        //Loop timing
        /*static unsigned long loopTimerOldStartTime = 0;
        unsigned long loopTimerNewStartTime = millis();
        if(abs(loopTimerNewStartTime - loopTimerOldStartTime) >= 100) _logger.logMessage(ERROR_LOG, "Over time: " + String(loopTimerNewStartTime - loopTimerOldStartTime));
        loopTimerOldStartTime = loopTimerNewStartTime;*/

        static unsigned long cvStartTime = 0;
        unsigned long interval = abs(millis() - cvStartTime);
        if(interval >= 105) _logger.logMessage(ERROR_LOG, "Over time: " + String(interval));
        if(interval >= 100)
        {
            cvStartTime = millis();

            //unsigned long startTest = millis();

            //Read frame
            QSMatrix<float> frame(8, 8, 0.0);
            _gridEye->readFrame(frame);

            //Write frame to SD
            //_logger.logFrame(frame);

            //Read thermistor temp
            //int16_t thermistor = _gridEye->readThermistor();

            //Write thermistor to SD
            //_logger.logThermistor(thermistor);

            //Process frame
            _cv->processFrame(frame);

            //Get count
            _storage.addGateCount(_cv->gateCount());

            //Serial.printf("Time taken: %lu\r\n", millis() - startTest);
        }

        static unsigned long stateMachineStartTime = millis();
        if(abs(millis() - stateMachineStartTime) >= 1000)
        {
            AbstractState* state = _state->update();
            if (state != NULL)
            {
                _state->exit();
                delete _state;
                _state = state;
                _state->enter();
            }
            stateMachineStartTime = millis();

            _storage.saveGateCount();

            uint32_t freemem = System.freeMemory();
            if(freemem < 20000) _logger.logMessage(ERROR_LOG, "Free memory low : " + String(freemem));
        }
    }
}

void serialEvent()
{
    char c = Serial.read();
    switch(c)
    {
        case 'c':
            if(Particle.connected()) Particle.disconnect();
            else Particle.connect();
            break;
        case 'a':
            _storage.addGateCount(1);
            break;
        case 's':
            _storage.addGateCount(-1);
            break;
        case 'x':
            _settings.setDebugPrint(!_settings.getDebugPrint());
            break;
        case 'z':
            _settings.print();
            break;
        case 'p':
            _storage.clear();
            break;
        default:
           break;
    }
}

void ledUpdateHandler(int red, int green, int blue)
{
    _strip->setPixelColor(0, red, green, blue);
    _strip->show();
}

void pirTriggeredHandler()
{
    //if(_settings.getDebugPrint()) Serial.printf("PIR triggered\r\n");
    _storage.pirTriggered();
}
