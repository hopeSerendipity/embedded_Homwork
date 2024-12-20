#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>

#include "Image0.h"
#include "Image1.h"
#include "Image2.h"
#include "Image3.h"
#include "Image4.h"
#include "Image5.h"
#include "Image6.h"
#include "Image7.h"
#include "Image8.h"
#include "Image9.h"
#include "Sun.h"
#include "Mon.h"
#include "Tues.h"
#include "Wed.h"
#include "Thur.h"
#include "Fri.h"
#include "Sat.h"
#include "Week.h"
#include "Month.h"
#include "Year.h"
#include "back240320.h"
#include "back640480.h"
#include "back6401.h"
// #include "back640480white.h"
#define TRUE 1
#define FALSE 0
#define RED_COLOR    0xF800
#define GREEN_COLOR  0x07E0
#define BLUE_COLOR   0x001F
#define IMAGE_HIGH   56
#define IMAGE_WIDE   40
#define RED_COLOR_8 0xE0
#define GREEN_COLOR_8 0x1C
#define BLUE_COLOR_8 0x03
#define PINK_COLOR_8 0xFC
#define YELLOW_COLOR_8 0xC3
#define EDITED_BLACK_8 0x25
#define BLACK_8 = 0x00
// 原始
char *fbp = 0;
// 绘图指针
char *fbptemp = 0;

int sysyear = 0;
int sysmonth = 0;
int sysday = 0;

int yeartemp = 0;
int monthtemp = 0;
int daytemp = 0;

int isVM = FALSE;
unsigned char originImage[4480];

int CalDayWithYearAndMonth(int year,int month);
int GetWeek(int year,int month,int day);
int judgeIsFestival(int year, int month, int day);
void Print(const unsigned char Image[], int _isVM);
void editNumImage(int today, int festival, unsigned char* originImage, unsigned const char* NumImage);
void fillImage(unsigned char* originImage, unsigned const char* image);
void PrintNumImage(int num, int today, int festival);
void InsertRowSpace(int SpaceNum);
void SetNextRowStart(int _isVM);
void PrintHeader(int year, int month, int week);
void PrintFooter();
void PrintBody(int year,int month);
void printBack(int _isVM);
void printBlack(int _isVM);

/**根据年月得到该月总共有多少天*/
int CalDayWithYearAndMonth(int year,int month)
{
    if (month == 2) {
        return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 29 : 28;
    }
    return (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) ?
        31 : 30;
}

// 蔡勒公式计算周几
int GetWeek(int year,int month,int day)
{
    int c, y, week;
    if (month == 1 || month == 2) {
        year--;
        month += 12;
    }
    c = year / 100;
    y = year - c * 100;
    week = (c/4) - 2*c + (y+y/4) + (13*(month+1) / 5) + day - 1;
    while (week < 0) {
        week+=7;
    }
    week %= 7;
    return week;
}

// 绘制图片数组
void Print(const unsigned char Image[], int _isVM)
{
    int x, y;
    int k = 0;
    if (_isVM == TRUE) {
        for(y = 0; y < IMAGE_HIGH; y++)
        for(x = 0; x < IMAGE_WIDE; x++)
        {
             *(fbptemp + y * 1024*2 + x*2) = Image[k];
             *(fbptemp + y * 1024*2 + x*2 +1) = Image[k+1];
            k+=2;
        }
    } else {
        for(y = 0; y < IMAGE_HIGH; y++)
        for(x = 0; x < IMAGE_WIDE; x++)
        {
             *(fbptemp + y * 640*2 + x*2) = Image[k];
             *(fbptemp + y * 640*2 + x*2 +1) = Image[k+1];
            k+=2;
        }
    }
    
    fbptemp += IMAGE_WIDE*2;
}

// 如果今天 红色 如果节假日 绿色
void editNumImage(int today, int festival, unsigned char* originImage, unsigned const char* NumImage) {
    if (today == TRUE) {
        int i;
        for (i = 0; i < 4480; i++) {
            if (NumImage[i] != 0X00)
                originImage[i] = NumImage[i];
            else
                originImage[i] = RED_COLOR_8;
        }
        return;
    }
    if (festival == TRUE) {
        int i;
        for (i = 0; i < 4480; i++) {
            if (NumImage[i] != 0X00)
                originImage[i] = NumImage[i];
            else
                originImage[i] = YELLOW_COLOR_8;
        }
        return;
    }
}

// 填充中间变量数组
void fillImage(unsigned char* originImage, unsigned const char* image) {
    int i;
    for (i = 0; i < 4480; i++) {
        originImage[i] = image[i];
    }
}

// 预处理数组并输出
void PrintNumImage(int num, int today, int festival)
{
    switch(num)
    {
        case 0: 
            fillImage(originImage, Image0);
            editNumImage(today, festival, originImage, Image0);
            break;
        case 1: 
            fillImage(originImage, Image1);
            editNumImage(today, festival, originImage, Image1);
            break;
        case 2: 
            fillImage(originImage, Image2);
            editNumImage(today, festival, originImage, Image2);
            break;
        case 3: 
            fillImage(originImage, Image3);
            editNumImage(today, festival, originImage, Image3);
            break;
        case 4: 
            fillImage(originImage, Image4);
            editNumImage(today, festival, originImage, Image4);
            break;
        case 5: 
            fillImage(originImage, Image5);
            editNumImage(today, festival, originImage, Image5);
            break;
        case 6: 
            fillImage(originImage, Image6);
            editNumImage(today, festival, originImage, Image6);
            break;
        case 7: 
            fillImage(originImage, Image7);
            editNumImage(today, festival, originImage, Image7);
            break;
        case 8: 
            fillImage(originImage, Image8);
            editNumImage(today, festival, originImage, Image8);
            break;
        case 9: 
            fillImage(originImage, Image9);
            editNumImage(today, festival, originImage, Image9);
            break;
    }
    Print(originImage, isVM);
}

// 插入空格 宽度图像*空格数
void InsertRowSpace(int SpaceNum)
{
    fbptemp += SpaceNum * IMAGE_WIDE * 2;
}

// 换行
void SetNextRowStart(int _isVM)
{
    if (_isVM == TRUE) {
        fbptemp = fbptemp - (fbptemp - fbp) % (1024 * 2)
            + IMAGE_HIGH * 1024 * 2 + 40 * 2;
    } else {
        fbptemp = fbptemp - (fbptemp - fbp) % (640 * 2)
            + IMAGE_HIGH * 640 * 2 + 40 * 2;
    }
}

// 绘制日历头部
void PrintHeader(int year, int month, int week)
{
    int year0 = year % 10;//个位
    int year1 = (year / 10) % 10;//十位
    int year2 = (year / 100) % 10;//百位
    int year3 = year / 1000;//千位
    int month0 = month % 10;
    int month1 = month / 10;
    fbptemp += 160 * 2;
    if(year3 != 0)
    {
        PrintNumImage(year3, 0, 0);
        PrintNumImage(year2, 0, 0);
        PrintNumImage(year1, 0, 0);
        PrintNumImage(year0, 0, 0);
    }
    else if(year2 != 0)
    {
        PrintNumImage(year2, 0, 0);
        PrintNumImage(year1, 0, 0);
        PrintNumImage(year0, 0, 0);
    }
    else if(year1 != 0)
    {
        PrintNumImage(year1, 0, 0);
        PrintNumImage(year0, 0, 0);
    }
    else PrintNumImage(year0, 0, 0);
    Print(Year, isVM);

    if (month1 != 0) {
        PrintNumImage(month1, 0, 0);
        PrintNumImage(month0, 0, 0);
    } else {
        PrintNumImage(0, 0, 0);
        PrintNumImage(month0, 0, 0);
    }
    Print(Month, isVM);    

    SetNextRowStart(isVM);

    Print(Sun, isVM);
    fbptemp += IMAGE_WIDE*2;
    Print(Mon, isVM);
    fbptemp += IMAGE_WIDE*2;
    Print(Tues, isVM);
    fbptemp += IMAGE_WIDE*2;
    Print(Wed, isVM);
    fbptemp += IMAGE_WIDE*2;
    Print(Thur, isVM);
    fbptemp += IMAGE_WIDE*2;
    Print(Fri, isVM);
    fbptemp += IMAGE_WIDE*2;
    Print(Sat, isVM);
    fbptemp += IMAGE_WIDE*2;
    SetNextRowStart(isVM);
}

// 判断当前是否节日
int judgeIsFestival(int year, int month, int day) {
    if ((month == 1 && day == 1) || (month == 3 && day == 8) 
    || (month == 5 && day >= 1 && day <=3) || (month == 5 && day == 4) 
    || (month == 6 && day == 1) || (month == 7 && day == 1) 
    || (month == 8 && day == 1) || (month == 10 && day >= 1 && day <= 7))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// 绘制日历主体
void PrintBody(int year,int month)
{
    int days, firstDayWeek, week, i;
    printBack(0); // 0 to 640*480
    if (fbptemp != fbp) {
        fbptemp = fbp; // set back to base
    }
    days = CalDayWithYearAndMonth(year, month);
    firstDayWeek = GetWeek(year, month, 1);
    week = firstDayWeek;

    PrintHeader(year, month, week);
    InsertRowSpace(firstDayWeek * 2);

    i = 1;
    while(i <= days)
    {
        if(i < 10)
        {
            if ((year == sysyear && month == sysmonth && i == sysday) && judgeIsFestival(year, month, i)) {
                PrintNumImage(i, 1, 1);
            } else if ((year == sysyear && month == sysmonth && i == sysday)) {
                PrintNumImage(i, 1, 0);
            } else if (judgeIsFestival(year, month, i)){
                PrintNumImage(i, 0, 1);
            } else {
                PrintNumImage(i, 0, 0);
            }
            InsertRowSpace(1);
        }
        else
        {
            if ((year == sysyear && month == sysmonth && i == sysday) && judgeIsFestival(year, month, i)) {
                PrintNumImage(i / 10, 1, 1);
                PrintNumImage(i % 10, 1, 1);
            } else if ((year == sysyear && month == sysmonth && i == sysday)) {
                PrintNumImage(i / 10, 1, 0);
                PrintNumImage(i % 10, 1, 0);
            } else if (judgeIsFestival(year, month, i)){
                PrintNumImage(i / 10, 0, 1);
                PrintNumImage(i % 10, 0, 1);
            } else {
                PrintNumImage(i / 10, 0, 0);
                PrintNumImage(i % 10, 0, 0);
            }
        }

        if(week == 6)
        {
            SetNextRowStart(isVM);
        }
        i++;
        week = (week + 1) % 7;
    }
    fbptemp = fbp;
}

void printBack(int _isVM) {
    int k = 0;
    int x, y;
    if (_isVM == TRUE) {
        for(y = 0; y < 320; y++)
        {
            for(x = 0; x < 240; x++)
            {
                *(fbptemp + y * 240*2 + x*2) = back240320[k];
                *(fbptemp + y * 240*2 + x*2 +1) = back240320[k+1];
                k += 2;
            }
        }
    } else {
        for(y = 0; y < 480; y++)
        {
            for(x = 0; x < 640; x++)
            {
                *(fbptemp + y * 640*2 + x*2) = backWhite640[k];
                *(fbptemp + y * 640*2 + x*2 +1) = backWhite640[k+1];
                k += 2;
            }
            // fbptemp += 384 * 2;
        }
    }
    fbptemp = fbp;
}

// 背景涂黑
void printBlack(int _isVM) {
    int x, y, k = 0;
    if (_isVM == TRUE) {
        for(y = 0; y < 728; y++)
        {
            for(x = 0; x < 1024; x++)
            {
                *(fbptemp + y * 1024*2 + x*2) = 0x00;
                *(fbptemp + y * 1024*2 + x*2 +1) = 0x00;
                k += 2;
            }
        }    
    } else {
        for(y = 0; y < 480; y++)
        {
            for(x = 0; x < 640; x++)
            {
                *(fbptemp + y * 640*2 + x*2) = 0x00;
                *(fbptemp + y * 640*2 + x*2 +1) = 0x00;
                k += 2;
            }
        }
    }
    
        fbptemp = fbp;
}

int main(int argc, char **argv)
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    // time_t rawtime; arm can not get true time
    // time(&rawtime);
    // struct tm *info; 
    // info = gmtime(&rawtime);

    // printf("\n %d year %d month %d day\n",info->tm_year,info->tm_mon,info->tm_mday);
    int year = 2020;
    int month = 11;
    int day = 5;
    int flag;
    sysyear = year;
    sysmonth = month;
    sysday = day;

    yeartemp = year;
    monthtemp = month;
    daytemp = day;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        exit(3);
    }

    // Figure out the size of the screen in bytes
    // xres:/*定义屏幕一行有多少个像素点 */
    // yres:/*定义屏幕一列由多少个像素点 */
    // bits_per_pixel /*每像素位数(多少BPP)，单位为字节 */
    // 计算屏幕字节数，该屏幕每个像素用两个字节表示，既16位。红：绿：蓝  5：6:5
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    printf("%dx%d, %dbpp, screensize = %ld\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, screensize );

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    fbptemp = fbp; // 做图游标初始化

    if(vinfo.bits_per_pixel == 16)  // 16bpp only
    {
        printBlack(isVM);
    } else {
        printf("16bpp only!!!\n");
    }
    
    /**以下是生成日历代码*/
    printf("Show calendar\n");
    PrintBody(year, month);

    while(TRUE)
    {
        fflush(stdin);
        printf("\nenter num to control calendar\n1: show this month\n2: show last month\n3: show next month\n4: enter year and month to control\n5: set today\n6: set today to last day\n");
        printf("7: set today to next day\n9: exit\n");
        scanf("%d",&flag);
        switch(flag)
        {
            case 1: 
                yeartemp = sysyear;
                monthtemp = sysmonth;
                PrintBody(yeartemp, monthtemp); 
                break;
            case 2:
                monthtemp--;
                if(monthtemp==0)
                {
                    yeartemp--;
                    monthtemp=12;
                }
                PrintBody(yeartemp, monthtemp);
                break;
            case 3:
                monthtemp++;
                if(monthtemp == 13)
                {
                    yeartemp++;
                    monthtemp = 1;
                }
                PrintBody(yeartemp, monthtemp);
                break;
            case 4:
                printf("year: ");
                scanf("%d", &yeartemp);
                printf("month: ");
                scanf("%d", &monthtemp);
                if (yeartemp <= 0) {
                    printf("error year\n");
                    continue;
                }
                if (monthtemp <= 0 || monthtemp >= 13) {
                    printf("error month\n");
                    continue;
                }
                PrintBody(yeartemp, monthtemp);
                break;
            case 5:
                printf("year: ");
                scanf("%d", &yeartemp);
                printf("month: ");
                scanf("%d", &monthtemp);
                printf("day: ");
                scanf("%d", &daytemp);
                if (yeartemp <= 0) {
                    printf("error year\n");
                    continue;
                }
                if (monthtemp <= 0 || monthtemp >= 13) {
                    printf("error month\n");
                    continue;
                }
                if (daytemp <= 0 || daytemp > CalDayWithYearAndMonth(yeartemp, monthtemp)) {
                    printf("error day\n");
                    continue;
                }
                sysyear = yeartemp;
                sysmonth = monthtemp;
                sysday = daytemp;
                PrintBody(yeartemp, monthtemp);
                break;
            case 6:
                if (--sysday < 1) {
                    if (sysmonth == 1) {
                        sysmonth = 12;
                        if (--sysyear < 1) {
                            printf("error year\n");
                            sysday = 1;
                            sysmonth = 1;
                            sysyear = 1;
                            PrintBody(1, 1);
                            break;
                        }
                    } else {
                        --sysmonth;
                    }

                    sysday = CalDayWithYearAndMonth(sysyear, sysmonth);
                }
                daytemp = sysday;
                monthtemp = sysmonth;
                yeartemp = sysyear;
                PrintBody(sysyear, sysmonth);
                break;
            case 7:
                if (++sysday > CalDayWithYearAndMonth(sysyear, sysmonth)) {
                    sysday = 1;    
                    if (++sysmonth > 12) {
                        sysmonth = 1;
                        ++sysyear;
                    }
                }
                daytemp = sysday;
                monthtemp = sysmonth;
                yeartemp = sysyear;
                PrintBody(sysyear, sysmonth);
                break;
            case 9:
                printf("Good Bye!\n");
                fbptemp = fbp;
                printBack(0);
                fbptemp += (212 * (isVM ? 1024 : 640) + 260) * 2;
                Print(Image8, isVM);
                Print(Image8, isVM);
                Print(Image6, isVM);
                return 0;
        }
    }

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
