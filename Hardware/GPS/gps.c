#include "GPS.h"
#include "stdlib.h"

// 确保包含 string.h 头文件
//static const double pi = 3.14159265358979324;
//static const double a = 6378245.0;
//static const double ee = 0.00669342162296594323;

// 转换十进制度
double convertNE(const char *latStr) {
    double NE = atof(latStr);
    int degrees = (int)(NE / 100);
    double minutes = NE - degrees * 100;
    return degrees + minutes / 60.0;
}
//辅助函数
double transformLat(double x, double y) {
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
    return ret;
}
double transformLon(double x, double y) {
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
    return ret;
}

// WGS-84 to GCJ-02 转换
void GPS_Transform(double wgLon, double wgLat, double *mgLon, double *mgLat) {
    double dLat = transformLat(wgLon - 105.0, wgLat - 35.0);
    double dLon = transformLon(wgLon - 105.0, wgLat - 35.0);
    double radLat = wgLat / 180.0 * pi;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
    *mgLat = wgLat + dLat;
    *mgLon = wgLon + dLon;
}

//转换
void GPS_zhuanhuang(void){
	double lon = convertNE(Longitude);
	double lat = convertNE(Latitude);
	GPS_Transform(lon, lat, &GPS_E, &GPS_N);
	
//	char date_buff[10];
//	sprintf(date_buff,"20%d",(int)atof(date)%100);
//	
//	double jd = myjulian_day((int)atof(date_buff),(int)atof(date)/100%100,(int)atof(date)/10000,(int)atof(Utc_Time)/10000,(int)atof(Utc_Time)/100%100,(int)atof(Utc_Time)%100);
//	solar_azimuth_elevation(lon, lat, jd, &Solar_Azimuth,&Solar_Altitude);
	//Solar_Azimuth = (360.0f-Solar_Azimuth);
}

// 主函数：计算指南针当前方向与目标方向之间的夹角
int getAngleDifference(double lon1, double lat1, double lon2, double lat2) {
	 // 将经纬度转换为弧度
    double lat1_rad = toRadians(lat1);
    double lon1_rad = toRadians(lon1);
    double lat2_rad = toRadians(lat2);
    double lon2_rad = toRadians(lon2);

    // 计算经度差
    double delta_lon = lon2_rad - lon1_rad;

    // 使用球面三角公式计算方位角
    double y = sin(delta_lon) * cos(lat2_rad);
    double x = cos(lat1_rad) * sin(lat2_rad) - sin(lat1_rad) * cos(lat2_rad) * cos(delta_lon);
    double azimuth_rad = atan2(y, x);
    double azimuth_deg = fmod((azimuth_rad * 180.0 / pi + 360.0), 360.0);

    // 输出中间结果用于调试
    
    //printf("azimuth_rad: %f, azimuth_deg: %f\n", azimuth_rad, azimuth_deg);

    return (int)round(azimuth_deg); // 使用round函数四舍五入
}

///////////////////////////////////////////////////太阳方位角

//#define DEG_TO_RAD (3.14159265358979323846f / 180.0f)
//#define RAD_TO_DEG (180.0f / 3.14159265358979323846f)

//float normalize_angle(float angle) {
//    return fmodf(angle + 360.0f, 360.0f);
//}

//float julian_day(int year, int month, int day, int hour, int minute, int second) {
//    if (month <= 2) {
//        year -= 1;
//        month += 12;
//    }
//    int ac = year / 100;
//    int b = 2 - ac + (ac / 4);
//    
//    float jd = (int)(365.25f * (year + 4716)) 
//                + (int)(30.6001f * (month + 1)) 
//                + day + b - 1524.5f
//                + (hour + minute/60.0f + second/3600.0f)/24.0f;

//    if (jd >= 2299160.5f) {
//        int y = year;
//        int m = month;
//        if (m <= 2) {
//            y--;
//            m += 12;
//        }
//        int yy = y / 100;
//        b = 2 - yy + (yy >> 2);
//        jd += b;
//    }
//    return jd;
//}

//void solar_azimuth_elevation(float lat, float lon, float jd, 
//                             float *azimuth, float *elevation) {
//    float n = jd - 2451545.0f;
//    float L = fmodf(280.460f + 0.9856474f * n, 360.0f);
//    float g = fmodf(357.528f + 0.9856003f * n, 360.0f) * DEG_TO_RAD;
//    
//    float lambda = L + 1.915f * sinf(g) + 0.020f * sinf(2 * g);
//    lambda = fmodf(lambda + 360.0f, 360.0f) * DEG_TO_RAD;
//    
//    float epsilon = (23.439f - 0.0000004f * n) * DEG_TO_RAD;
//    float alpha = atan2f(cosf(epsilon) * sinf(lambda), cosf(lambda));
//    float delta = asinf(sinf(epsilon) * sinf(lambda));

//    // 2. 计算时角（H）
//    float GMST = fmodf(280.46061837f + 360.98564736629f * n, 360.0f);
//    float LST = fmodf(GMST + lon, 360.0f) * DEG_TO_RAD;
//    float H = LST - alpha;

//    // 3. 计算高度角
//    float phi = lat * DEG_TO_RAD;
//    *elevation = asinf(sinf(phi) * sinf(delta) + 
//                 cosf(phi) * cosf(delta) * cosf(H)) * RAD_TO_DEG;

//    // 4. 优化方位角计算
//    float numerator = sinf(H);
//    float denominator = cosf(H) * sinf(phi) - tanf(delta) * cosf(phi);
//    *azimuth = atan2f(numerator, denominator) * RAD_TO_DEG;
//    *azimuth = fmodf(360.0f-*azimuth, 360.0f);  // 归一化
//}




//#define DEG_TO_RAD (pi / 180.0)
//#define RAD_TO_DEG (180.0 / pi)
//// 将角度转为 0~360 范围内
//double normalize_angle(double angle) {
//    return fmod(angle + 360.0, 360.0);
//}

//double myjulian_day(int year, int month, int day, int hour, int minute, int second) {
//    if (month <= 2) {
//        year -= 1;
//        month += 12;
//    }
//    int ac = year / 100;
//    int b = 2 - ac + (ac / 4);
//    
//    double jd = (int)(365.25 * (year + 4716)) 
//                + (int)(30.6001 * (month + 1)) 
//                + day + b - 1524.5
//                + (hour + minute/60.0 + second/3600.0)/24.0;
//    if (jd >= 2299160.5) {
//        int y = year;
//        int m = month;
//        if (m <= 2) {
//            y--;
//            m += 12;
//        }
//        int yy = y / 100;
//        b = 2 - yy + (yy >> 2);  // 等价于yy/4的整数运算
//        jd += b;
//    }
//    return jd;
//}

//// 太阳方位角计算
//void solar_azimuth_elevation(double lat, double lon, double jd, 
//                             double *azimuth, double *elevation) {
//    // ... [保留变量声明] ...

//    // 1. 计算太阳黄经（λ）和赤纬（δ）
//    double n = jd - 2451545.0;
//    double L = fmod(280.460 + 0.9856474 * n, 360.0);
//    double g = fmod(357.528 + 0.9856003 * n, 360.0) * DEG_TO_RAD;
//    
//    double lambda = L + 1.915 * sin(g) + 0.020 * sin(2 * g);
//    lambda = fmod(lambda + 360.0, 360.0) * DEG_TO_RAD;
//    
//    double epsilon = (23.439 - 0.0000004 * n) * DEG_TO_RAD;
//    double alpha = atan2(cos(epsilon) * sin(lambda), cos(lambda));
//    double delta = asin(sin(epsilon) * sin(lambda));

//    // 2. 计算时角（H）
//    double GMST = fmod(280.46061837 + 360.98564736629 * n, 360.0);
//    double LST = fmod(GMST + lon, 360.0) * DEG_TO_RAD;
//    double H = LST - alpha;

//    // 3. 计算高度角
//    double phi = lat * DEG_TO_RAD;
//    *elevation = asin(sin(phi) * sin(delta) + 
//                 cos(phi) * cos(delta) * cos(H)) * RAD_TO_DEG;

//    // 4. 优化方位角计算
//    double numerator = sin(H);
//    double denominator = cos(H) * sin(phi) - tan(delta) * cos(phi);
//    *azimuth = atan2(numerator, denominator) * RAD_TO_DEG;
//    //*azimuth = fmod(*azimuth + 360.0, 360.0);  // 归一化
//}
