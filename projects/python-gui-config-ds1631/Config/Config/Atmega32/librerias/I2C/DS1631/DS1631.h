#define I2C_DS1631_Address_Base	0x90
#define TWI_DS1631_Start_Convert    0x51
#define TWI_DS1631_Stop_Convert     0x22
#define TWI_DS1631_Read_Temperature 0xAA
#define TWI_DS1631_Access_TH        0xA1
#define TWI_DS1631_Access_TL        0xA2
#define TWI_DS1631_Access_Config    0xAC
#define TWI_DS1631_Software_Por     0x54

#define I2C_DS1631_Config_Temperature_High_Flag_Bit 6
#define I2C_DS1631_Config_Temperature_Low_Flag_Bit  5
#define I2C_DS1631_Config_Precision_Bit_1	          3
#define I2C_DS1631_Config_Precision_Bit_0	          2
#define I2C_DS1631_Config_Tout_Polarity_Bit         1
#define I2C_DS1631_Config_Conversion_Mode_Bit       0

#define DS1631_cte_temp 0.0625 //Precisión mínima del sensor.

enum I2C_DS1631_Precision {
		I2C_DS1631_Precision_9_Bits,
		I2C_DS1631_Precision_10_Bits,
		I2C_DS1631_Precision_11_Bits,
		I2C_DS1631_Precision_12_Bits,
};

enum I2C_DS1631_Temperature_Flag {
		I2C_DS1631_Temperature_Low_Flag,  // 0
		I2C_DS1631_Temperature_High_Flag, // 1
};

enum I2C_DS1631_Tout_Polarity {
		I2C_DS1631_Tout_Polarity_Low_Active,  // 0
		I2C_DS1631_Tout_Polarity_High_Active, // 1
};

enum I2C_DS1631_Conversion_Mode {
		I2C_DS1631_Conversion_Mode_Continuous, // 0
		I2C_DS1631_Conversion_Mode_One_Shot,   // 1
};

struct I2C_DS1631_Temperatura {
	int Grados;
	int Decimas;
};

struct I2C_DS1631_Type_Data {
	uint8_t		      		I2C_DS1631_Address;
	float						I2C_DS1631_Temperatura;
	char						I2C_DS1631_Temperatura_String[ 32];
	uint8_t					I2C_DS1631_Config;
	enum I2C_DS1631_Precision		I2C_DS1631_Precision_Set_To;
	enum I2C_DS1631_Temperature_Flag	I2C_DS1631_Temperature_High_Flag_Set_To;
	enum I2C_DS1631_Temperature_Flag	I2C_DS1631_Temperature_Low_Flag_Set_To;
	enum I2C_DS1631_Tout_Polarity		I2C_DS1631_Tout_Polarity_Set_To;
	enum I2C_DS1631_Conversion_Mode	I2C_DS1631_Conversion_Mode_Set_To;
	struct I2C_DS1631_Temperatura 	I2C_DS1631_Temperatura_Actual;
      struct I2C_DS1631_Temperatura 	I2C_DS1631_Termostato_Alta;
	struct I2C_DS1631_Temperatura 	I2C_DS1631_Termostato_Baja;
};

struct I2C_DS1631_Type_Data    I2C_DS1631_TAD;
struct I2C_DS1631_Type_Data *__I2C_DS1631_TAD;

