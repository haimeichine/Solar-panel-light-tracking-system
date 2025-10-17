## 太阳能板追光系统，

//**方案一：**四个角光敏电阻作用后得到的4个电压值强度进行判断

//流程：

​			__________________________________________

​			| A1	//	A2 |

​			| A3	//	A4 |

​			__________________________________________

​			A1-A2=>0或<0偏航转向

​			A1-A3=>0或<0俯仰转向



//**方案二：**GPS经纬度加当前时间，算出方位角，高度角。

//流程：当前偏航角与GPS方位角之差判断电机正转反转，高度角同理。

## **QMC5883.c/h获取偏航角度**

**引脚**

QMC5883————GPIOA; CLK=PA6；SDA=PA7；		3.3V

**函数**

`float Yaw_Angle；				//用于储存偏航角度全局变量`

`QMC5883_Get_Yaw_Angles(&Yaw_Angle);`

















## **MPU6050.c/h获取俯仰角度**

**引脚**

MPU6050————GPIOB; CLK=PB12；SDA=PB13；	3.3V-5V

**函数**

float Pitch_Angle;						//用于储存俯仰角度全局变量

MPU6050_Get_Angles(&Pitch_Angle);



















## **MOTOR_PWM.c/h电机控制**

**引脚**

`LEFT_MOTOR————GPIOA; PWM1=PA0；PWM2=PA1，EN=PA4；`

`RIGHT_MOTOR————GPIOB; PWM1=PB8；PWM2=PB9，EN=PA5；`

***偏航控制函数***

speed参数为电机旋转速度1-100

void Yaw_Motor_Left(uint8_t speed);
void Yaw_Motor_Rigth(uint8_t speed);
void Yaw_Motor_Stop(void);
void Yaw_Motor_Auto(void);

***俯仰控制函数***
`void Pitch_Motor_Left(uint8_t speed);`
`void Pitch_Motor_Rigth(uint8_t speed);`
`void Pitch_Motor_Stop(void);`
`void Pitch_Motor_Auto(void);`













## **GPS.c/h获取方位角控制**

由USART2来接收。

`double Solar_Azimuth;`							//太阳方位角
`double Solar_Altitude;`							//太阳高度角

//判断GPS是否获取到卫星数据如果获取到则为1，否则为0

`uint8_t GPS_If;`

**函数**

//开启USART接收中收中断函数获取太阳的方位角与高度角，不需要时关闭

`USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);`



**引脚**

GPS————GPIOA；TX=PA2，RX=PA3///使用的是USART2







## 





















## **CAN.c/h主要获取四个角的电压**

float ADC_Data1,ADC_Data2,ADC_Data3,ADC_Data4;//储存获取到的温度全局变量

`Get_Can_ADC_Data();`获取温度

**引脚**

CAN————GPIOA；TX=PA12，RX=PA11











## Heartbeat.c/h主程序运行在TIM3的更新中断中每秒执行一次



















​	