#ifndef _CC1100_GDO_H
#define _CC1100_GDO_H



#define GDO0_PIN (1<<2)
#define GDO2_PIN (1<<3)

enum {
	LOW_EDGE=0,
	HIGH_EDGE
};



/*

#define GDO_INIT() do \
{ \
  PCSEL &= ~(GDO0_PIN | GDO2_PIN); \
  PCDDR &= ~(GDO0_PIN | GDO2_PIN); \
  P1IE  &= ~(GDO0_PIN | GDO2_PIN); \
} while (0)

*/


/*
#define GDO_INIT() do \
{ \
  PINC &= ~(GDO0_PIN | GDO2_PIN); \
  DDRC &= ~(GDO0_PIN | GDO2_PIN); \
  PCMSK1  |= (1<<PCINT10); \
  PCMSK1  |= (1<<PCINT11); \
} while (0)
*/



#define GDO_INIT() do \
{ \
  DDRC &= ~(GDO0_PIN | GDO2_PIN); \
  PCMSK1  |= (1<<PCINT10); \
  PCMSK1  |= (1<<PCINT11); \
} while (0)




//#define GDO0_INT_ENABLE() P1IE |= GDO0_PIN

#define GDO0_INT_ENABLE()   PCMSK1  |= (1<<PCINT10)

//#define GDO2_INT_ENABLE() P1IE |= GDO2_PIN

#define GDO2_INT_ENABLE()   PCMSK1  |= (1<<PCINT11)

//#define GDO0_INT_DISABLE() P1IE &= ~GDO0_PIN

#define GDO0_INT_DISABLE()  PCMSK1  &= ~(1<<PCINT10)

//#define GDO2_INT_DISABLE() P1IE &= ~GDO2_PIN

#define GDO2_INT_DISABLE()  PCMSK1  &= ~(1<<PCINT11)




//#define GDO0_INT_CLEAR() PCIFR &= ~(1<<PCIF1)
#define GDO0_INT_CLEAR() PCIFR |= (1<<PCIF1)

//#define GDO2_INT_CLEAR() PCIFR &= ~(1<<PCIF1)
#define GDO2_INT_CLEAR() PCIFR |= (1<<PCIF1)



#define SENSOR_INT1_PIN (1<<3)


#define SENSOR_INIT() do \
{ \
  DDRD &= ~(SENSOR_INT1_PIN); \
  EIMSK  |= (1<<INT1); \
} while (0)


#define SENSOR_INT1_ENABLE()  EIMSK  |= (1<<INT1)
#define SENSOR_INT1_DISABLE()  EIMSK  &= ~(1<<INT1)




#define RTC_INT0_PIN (1<<2)

#define RTC_INIT() do \
{ \
  DDRD &= ~(RTC_INT0_PIN); \
  EIMSK  |= (1<<INT0); \
} while (0)


#define RTC_INT0_ENABLE()  EIMSK  |= (1<<INT0)
#define RTC_INT0_DISABLE() EIMSK  &= ~(1<<INT0)






#define RTC_INT0_CLEAR() EIFR &= ~(1<<INTF0)
#define SENSOR_INT1_CLEAR() EIFR &= ~(1<<INTF1)


#define rtc_int0_int_clear() \
	RTC_INT0_CLEAR()

#define sensor_int1_int_clear() \
	SENSOR_INT1_CLEAR()


#define RTC_INT0_SET_RISING() do \
{ \
	EICRA &= ~(1<<ISC00); \
	EICRA |= (1<<ISC01);  \
} while(0)


#define RTC_INT0_SET_FALLING() do \
{ \
	EICRA |= (1<<ISC00);  \
	EICRA |= (1<<ISC01);  \
} while(0)


#define SENSOR_INT1_SET_RISING() do \
{ \
	EICRA &= ~(1<<ISC10); \
	EICRA |= (1<<ISC11);  \
} while(0)


#define SENSOR_INT1_SET_FALLING() do \
{ \
	EICRA |= (1<<ISC10);  \
	EICRA |= (1<<ISC11);  \
} while(0)


#define rtc_int0_int_set_rising_edge() \
	RTC_INT0_SET_RISING()

#define rtc_int0_int_set_falling_edge() \
    RTC_INT0_SET_RISING()



#define sensor_int1_int_set_rising_edge() \
	SENSOR_INT1_SET_RISING()


#define sensor_int1_int_set_falling_edge() \
	SENSOR_INT1_SET_FALLING()






#endif




