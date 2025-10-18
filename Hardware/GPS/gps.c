/////////////////////////////////////////////
//          太阳能追踪系统 (STS)           //
//                 用于                    //
//           太阳能应用领域                //
//                                         //
//              2025年5月26日              //
//                                         //
//   文件名: README.md										 //
//                                         //
//   作者: CLMN                            //
//   邮箱: <2181708694@qq.com>             //
//                                         //
//   电子信息工程学院                      //
//   柳州职业技术大学                      //
//   301实训室                             //
//   广西柳州, 中国                        //
/////////////////////////////////////////////

/////////////////////////////////////////////
//   使用方法请参考 STS.H 头文件           //
//                                         //
//   本代码基于NREL技术报告                //
//   《太阳能辐射应用的太阳位置算法》      //
//   作者: I. Reda & A. Andreas            //
//   并使用 SPA.C 进行太阳位置计算         //
/////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//
//   MIT 许可证
//
//   版权所有 (c) 2025 CLMN, 柳州职业技术大学
//
//   特此免费授予任何获得本软件及相关文档文件（以下简称"软件"）副本的人不受限制地
//   处理本软件的权限，包括但不限于使用、复制、修改、合并、发布、分发、再许可
//   和/或销售本软件副本的权利，并允许接受本软件的人这样做，但需满足以下条件：
//
//   上述版权声明和本许可声明应包含在软件的所有副本或主要部分中。
//
//   本软件按"原样"提供，不附带任何明示或暗示的担保，包括但不限于对适销性、
//   特定用途的适用性和非侵权的担保。在任何情况下，作者或版权持有人均不对
//   因软件或使用或其他软件交易而产生的任何索赔、损害或其他责任负责，
//   无论是在合同诉讼、侵权行为还是其他方面。
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//
//   系统描述：
//   本太阳能追踪系统利用NREL SPA算法实时计算太阳的方位角和高度角。
//   系统将这些计算出的角度与当前太阳能板朝向进行比较，并驱动电机以保持
//   与太阳的最佳对齐，从而实现最大能量收集。
//
//   主要特性：
//   1. 使用 SPA.C 进行实时太阳位置计算
//   2. 双轴追踪（方位角和高度角）
//   3. 面板定位电机控制
//   4. 误差校正和安全机制
//
//   修订历史：
//   2025-05-26 CLMN
//     - 初始版本创建
//     - 集成 SPA.C 太阳位置算法
//     - 实现电机控制逻辑
//
///////////////////////////////////////////////////////////////////////////////////////////////
#include "GPS.h"
#include "stdlib.h"

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
void GPS_zhuanhuang(char *Latitude,char *Longitude){
	double lon = convertNE(Longitude);
	double lat = convertNE(Latitude);
	GPS_Transform(lon, lat, &GPS_E, &GPS_N);
}
