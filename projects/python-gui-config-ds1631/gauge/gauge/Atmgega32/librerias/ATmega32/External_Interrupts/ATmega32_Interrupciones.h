
#define ATmega32_Interrupts_Set_Int0_Request_Low_Level  MCUCR &= ~_BV( ISC00) & ~_BV( ISC01)
#define ATmega32_Interrupts_Set_Int0_Any_Logical_Change MCUCR |=  _BV( ISC00)
#define ATmega32_Interrupts_Set_Int0_Falling_Edge	  MCUCR |=  _BV( ISC01)
#define ATmega32_Interrupts_Set_Int0_Rising_Edge	  MCUCR |=  _BV( ISC00) | _BV( ISC01)

#define ATmega32_Interrupts_Set_Int1_Request_Low_Level  MCUCR &= ~_BV( ISC10) & ~_BV( ISC11)
#define ATmega32_Interrupts_Set_Int1_Any_Logical_Change MCUCR |=  _BV( ISC10)
#define ATmega32_Interrupts_Set_Int1_Falling_Edge	  MCUCR |=  _BV( ISC11)
#define ATmega32_Interrupts_Set_Int1_Rising_Edge	  MCUCR |=  _BV( ISC10) | _BV( ISC11)

#define ATmega32_Interrupts_Set_Int2_Falling_Edge	  MCUCSR &= ~_BV( ISC2)
#define ATmega32_Interrupts_Set_Int2_Rising_Edge	  MCUCSR |=  _BV( ISC2)

#define ATmega32_Interrupts_Int0_Set_Disabled		GICR &= ~_BV( INT0)
#define ATmega32_Interrupts_Int1_Set_Disabled		GICR &= ~_BV( INT1)
#define ATmega32_Interrupts_Int2_Set_Disabled		GICR &= ~_BV( INT2)

#define ATmega32_Interrupts_Int0_Set_Enabled		GICR |= _BV( INT0)
#define ATmega32_Interrupts_Int1_Set_Enabled		GICR |= _BV( INT1)
#define ATmega32_Interrupts_Int2_Set_Enabled		GICR |= _BV( INT2)

