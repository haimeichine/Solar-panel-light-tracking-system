#ifndef __GPS_H__
#define __GPS_H__

#include "stm32f10x.h"                  // Device header
#include "math.h"
#include "stdio.h"
#include "spa.h"

#define toRadians(degrees) (degrees * pi / 180.0)
#define pi 	3.14159265358979324
#define a 	6378245.0
#define ee 	0.00669342162296594323

extern char *Latitude;    					//存储纬度的字符串
extern char *Longitude;   					//存储经度的字符串
extern char *Utc_Time;     					//时间
extern char *GPS_Status;       					//定位状态
extern char *ns;           					//南北
extern char *ew;           					//东西
extern char *date;        					//日期
extern double GPS_E,GPS_N;					//火星坐标
extern double Solar_Azimuth;				//太阳方位角
extern double Solar_Altitude;				//太阳高度角
extern uint8_t GPS_If;							//是否更新
extern spa_data spa;

void 	GPS_zhuanhuang(void);
double 	Complex(double lat1, double lon1, double lat2, double lon2);

///////////////////太阳方位角
double 	normalize_angle(double angle);
double 	myjulian_day(int year, int month, int day, int hour, int minute, int second);
void 	solar_azimuth_elevation(double lat, double lon, double jd, double *azimuth, double *elevation) ;


#endif
