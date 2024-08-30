#include "DHT11.h"

#define Delay_us(X)  delay((X)*80/5)

void delay(unsigned int n)
{
    while(n--);
}

void DHT11_PIN_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /**/
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PIN_PORT, &GPIO_InitStruct);
}
void DHT11_PIN_INPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    DHT11_PIN_CLOCK;
    /**/
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_PIN_PORT, &GPIO_InitStruct);
}
//شλDHT11
void DHT11_Rst(void)
{
    DHT11_PIN_OUT(); 	//ʨ׃ΪˤԶ
    DHT11_PIN_OUT_L; 	//֍֧ƽ
    HAL_Delay(20);	//-֍ׁʙ18ms
    DHT11_PIN_OUT_H; 	//ٟ֧ƽ
    Delay_us(60);     	//׷ܺ-ٟ20~40us
}

//ֈսDHT11քܘӦ
//׵ܘ1:δݬӢսDHT11քզ՚
//׵ܘ0:զ՚
uint8_t DHT11_Check(void)
{
    uint8_t re = 0;
    DHT11_PIN_INPUT();      //ʨ׃Ϊˤɫ
    while (DHT11_PIN_IN && re < 100) //DHT11ܡ-֍40~80us
    {
        re++;
        Delay_us(1);
    };
    if(re >= 100)return 1;
    else re = 0;
    while (!DHT11_PIN_IN && re < 100) //DHT11-֍۳ܡՙՎ-ٟ40~80us
    {
        re++;
        Delay_us(1);
    };
    if(re >= 100)return 1;
    return 0;
}

//ՓDHT11ׁȡһٶλ
uint8_t DHT11_Read_Bit(void)
{
    uint8_t re = 0;
    while(DHT11_PIN_IN && re < 110) //ֈսҤΪ֍֧ƽ
    {
        re++;
        Delay_us(1);
    }
    re = 0;
    while(!DHT11_PIN_IN && re < 110) //ֈսҤٟ֧ƽ
    {
        re++;
        Delay_us(1);
    }
    Delay_us(80);//ֈս40us
    if(DHT11_PIN_IN)return 1;
    else return 0;
}

//ՓDHT11ׁȡһٶؖޚ
uint8_t DHT11_Read_Byte(void)
{
    uint8_t i, dat;
    dat = 0;
	
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

//ԵʼۯDHT11քIOࠚìͬʱݬӢDHT11քզ՚
uint8_t DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    DHT11_PIN_CLOCK;;
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PIN_PORT, &GPIO_InitStruct);

    DHT11_Rst();
    return DHT11_Check();
}


//ՓDHT11ׁȡһՎ˽ߝ
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
	
    DHT11_Rst();
    if(DHT11_Check() == 0)
    {
        for(i = 0; i < 5; i++)
        {
            buf[i] = DHT11_Read_Byte();
        }
        if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else return 1;
    return 0;
}
