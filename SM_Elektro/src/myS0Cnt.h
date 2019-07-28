/* implements S0 power meter counter
it simply sends 1000 or 10 000 pulse per 1 kWh
1 impulse means 1Wh - or 0.1Wh
for 1000 and 1 kWh it means 1000 pulse per hour 
or 1 pulse every 3.6s - not much
at 100 Wh -> every 36s, 10 Wh -> every 360s = 6 min
generic problem: counting pulse per interval reduces resolution
or does a miss alignment 
you do not have an absolute counter, so not recommended if it is about
absolute values

so we might report an absolute counter, 
and the actual power, calculated by the time difference between last 2 impulse
...sliding average ...??

actual power in W is the 3600/ delta time in s

so we go for an interrupt

if we need multiple S0 counter we would have to create an object class
*/

volatile uint32_t oldtime, deltatime, sumCnt; // in ms

void IRAM_ATTR handle_S0_interrupt () {
    uint32_t newtime = millis();
    deltatime = newtime - oldtime;
    oldtime = newtime;
    sumCnt++;
}

void setup_S0 (uint8_t pin) {
    sumCnt = 0;
    oldtime = millis();
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(pin, handle_S0_interrupt, FALLING);

}

