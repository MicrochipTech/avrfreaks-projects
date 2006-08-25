
void dataInit(void) __attribute__((naked)) __attribute__ ((section (".init1"))); 

void dataInit(void) 
{ 
	// Break into the C startup so I can clear SRAM to 
	//known values making it easier to see how it is used 
	uint8_t *p; 
	uint16_t i; 

	for (i=0x100; i<RAMEND; i++) 
	{ 
		p = (uint8_t *)i; 
		*p = 0x5A; 
	} 
}

void dummySec(void)
{
	eventAddEvent(EVENT_SENSOR);
	eventAddEvent(EVENT_TIME_SEC);
}

void dummyMin(void)
{
	clock_t *now = clockGet();
	if ((now->second & 0x03) == 0x03)
		clockSet(now->year, now->month, now->day, now->hour, now->minute, 59);
}

void dummyHour(void)
{
	clock_t *now = clockGet();
	if ((now->minute & 0x03) == 0x03) clockSet(now->year, now->month, now->day, now->hour, 59, now->second);
}

void dummyDay(void)
{
	clock_t *now = clockGet();
	if ((now->hour & 0x03) == 0x03) clockSet(now->year, now->month, now->day, 23, now->minute, now->second);
}

	//eventRegisterHandler(dummySec, EVENT_TIME_50MS);
	//eventRegisterHandler(dummyMin, EVENT_TIME_SEC);
	//eventRegisterHandler(dummyHour, EVENT_TIME_MIN);
	//eventRegisterHandler(dummyDay, EVENT_TIME_HOUR);
	
