
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

#define RED_COLOR    0xF800
#define GREEN_COLOR  0x07E0
#define BLUE_COLOR   0x001F
#define IMAGE_HIGH   56
#define IMAGE_WIDE   40

/**屏幕原点地址指针，不变*/
char *fbp = 0;
/**屏幕地址指针，可变*/
char *fbptemp = 0;

int sysyear = 0;
int sysmonth = 0;
int sysday = 0;

int yeartemp = 0;
int monthtemp = 0;
int daytemp = 0;
/*
 * framebuffer application code, the start code of Linux GUI application
 * compile :
 *          $/usr/local/arm/2.95.3/bin/arm-linux-gcc -o fbtest fbtest.c
 *          $cp fbtest /tftpboot/examples
 * run in target:
 *          #mount 192.168.1.180:/tftpboot/ /mnt/nfs
 *          #cd /mnt/nfs/examples
 *          #./fbtest
 */

void PutPixel(unsigned int x,unsigned int y,unsigned int c,char* f)
{
    if(x<640 && y<480) {
    	*(f + y * 640*2 + x *2) = 0x00FF&c;//低8位颜色值
    	*(f + y * 640*2 + x *2 +1) = (0xFF00&c)>>8;//高8位颜色值
    }
}
void Glib_Line(int x1,int y1,int x2,int y2,int color,char* f)
{
	int dx,dy,e;
	dx=x2-x1;
	dy=y2-y1;

	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color,f);
					if(e>0){y1+=1;e-=dx;}
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color,f);
					if(e>0){x1+=1;e-=dy;}
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color,f);
					if(e>0){y1-=1;e-=dx;}
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color,f);
					if(e>0){x1+=1;e-=dy;}
					y1-=1;
					e+=dx;
				}
			}
		}
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color,f);
					if(e>0){y1+=1;e-=dx;}
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color,f);
					if(e>0){x1-=1;e-=dy;}
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color,f);
					if(e>0){y1-=1;e-=dx;}
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color,f);
					if(e>0){x1-=1;e-=dy;}
					y1-=1;
					e+=dx;
				}
			}
		}
	}
}

void Glib_Rectangle(int x1,int y1,int x2,int y2,int color,char* f)
{
    Glib_Line(x1,y1,x2,y1,color,f);
    Glib_Line(x2,y1,x2,y2,color,f);
    Glib_Line(x1,y2,x2,y2,color,f);
    Glib_Line(x1,y1,x1,y2,color,f);
}

void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color,char* f)
{
    int i;

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color,f);
}

/**生成日历所需要的函数*/

/**根据年月得到该月总共有多少天*/
int GetDaysOfMonth(int year,int month)
{
    int days = 0;

    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
    {
        days = 31;
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        days = 30;
    }
    else
    { // 2月份，闰年29天、平年28天
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
        {
            days = 29;
        }
        else
        {
            days = 28;
        }
    }

    return days;
}

/**根据年月日得到该日的星期数*/
int GetWeek(int year,int month,int day)
{
    if(month==1||month==2)//判断month是否为1或2　
        {
        year--;
        month+=12;
        }
    int c=year/100;
    int y=year-c*100;
    int week=(c/4)-2*c+(y+y/4)+(13*(month+1)/5)+day-1;
    while(week<0){week+=7;}
    week%=7;
    switch(week)
    {
        case 1:printf("%d,%d,%d is Monday\n",year,month,day);break;
        case 2:printf("%d,%d,%d is Tuesday\n",year,month,day);break;
        case 3:printf("%d,%d,%d is Wednesday\n",year,month,day);break;
        case 4:printf("%d,%d,%d is Thursday\n",year,month,day);break;
        case 5:printf("%d,%d,%d is Friday\n",year,month,day);break;
        case 6:printf("%d,%d,%d is Saturday\n",year,month,day);break;
        case 0:printf("%d,%d,%d is Sunday\n",year,month,day);break;
    }
    return week;
}

/**在LED上绘制图片*/
void PrintImage(const unsigned char Image[])
{
    int x = 0, y = 0;
    int k = 0;
    for(y = 0; y < IMAGE_HIGH; y++)
        for(x = 0; x < IMAGE_WIDE; x++)
        {
             *(fbptemp + y * 640*2 + x*2) = Image[k];
             *(fbptemp + y * 640*2 + x*2 +1) = Image[k+1];
            k+=2;
        }
    fbptemp += IMAGE_WIDE*2;
}

void PrintNum(int num)
{
    switch(num)
    {
        case 0: PrintImage(Image0);break;
        case 1: PrintImage(Image1);break;
        case 2: PrintImage(Image2);break;
        case 3: PrintImage(Image3);break;
        case 4: PrintImage(Image4);break;
        case 5: PrintImage(Image5);break;
        case 6: PrintImage(Image6);break;
        case 7: PrintImage(Image7);break;
        case 8: PrintImage(Image8);break;
        case 9: PrintImage(Image9);break;
    }
}

/**插入空白*/
void InsertRowSpace(int SpaceNum)
{
    //if(fbptemp!=fbp){fbptemp = fbp;}
    fbptemp += SpaceNum*IMAGE_WIDE*2*2;
}

/**另起一行*/
void SetNextRowStart()
{
    fbptemp = fbptemp-(fbptemp-fbp)%(640*2)+IMAGE_HIGH*640*2;
}

/**绘制日历标题,包括年月，星期*/
void PrintBanner(int year,int month)
{
    printf("PrintBanner: %d year %d month ",year,month);
    int year0 = year%10;//个位
    int year1 = (year/10)%10;//十位
    int year2 = (year/100)%10;//百位
    int year3 = year/1000;//千位
    int month0 = month%10;
    int month1 = month/10;

    printf("/n %d  %d  %d  %d  %d  %d  ",year3,year2,year1,year0,month1,month0);

    if(year3!=0)
    {
        PrintNum(year3);
        PrintNum(year2);
        PrintNum(year1);
        PrintNum(year0);
    }
    else if(year2!=0)
    {
        PrintNum(year2);
        PrintNum(year1);
        PrintNum(year0);
    }
    else if(year1!=0)
    {
        PrintNum(year1);
        PrintNum(year0);
    }
    else PrintNum(year0);

    InsertRowSpace(3);

    if(month1!=0)
    {
        PrintNum(month1);
        PrintNum(month0);
    }
    else PrintNum(month0);
    SetNextRowStart();

    PrintImage(Sun);
    fbptemp += IMAGE_WIDE*2;
    PrintImage(Mon);
    fbptemp += IMAGE_WIDE*2;
    PrintImage(Tues);
    fbptemp += IMAGE_WIDE*2;
    PrintImage(Wed);
    fbptemp += IMAGE_WIDE*2;
    PrintImage(Thur);
    fbptemp += IMAGE_WIDE*2;
    PrintImage(Fri);
    fbptemp += IMAGE_WIDE*2;
    PrintImage(Sat);
    fbptemp += IMAGE_WIDE*2;
    SetNextRowStart();
}

/**绘制日历主体部分*/
void PrintBody(int year,int month)
{

    int x=0;
    int y=0;
    for(y = 0; y < 480; y++)
        {
            for(x = 0; x < 640; x++)
            {
                *(fbp + y * 640*2 + x*2) = 0x00;
                *(fbp + y * 640*2 + x*2 +1) = 0x00;
            }
        }

    if(fbptemp!=fbp){fbptemp = fbp;}
    int days = GetDaysOfMonth(year,month);
    int firstDayWeek = GetWeek(year,month,1);
    int week = firstDayWeek;

    PrintBanner(year,month);
    InsertRowSpace(firstDayWeek);



    int i=1;
    while(i<=days)
    {
        if(i<10)
        {

            PrintNum(i);
            fbptemp-=IMAGE_WIDE*2;
            if(year==sysyear&&month==sysmonth&&i==sysday) Glib_Rectangle(0,0,IMAGE_WIDE*2,IMAGE_HIGH,RED_COLOR,fbptemp);
            fbptemp += IMAGE_WIDE*2*2;

        }
        else
        {

            PrintNum(i/10);
            if(year==sysyear&&month==sysmonth&&i==sysday) Glib_Rectangle(0,0,IMAGE_WIDE*2,IMAGE_HIGH,RED_COLOR,fbptemp);
            PrintNum(i%10);

        }

        if(week == 6)
        {
            SetNextRowStart();
        }
        i++;
        week = (week + 1) % 7;
    }
}

int main(int argc, char **argv)
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;

    int x = 0, y = 0;
    //int k;


    /*int year = timeinfo->tm_year;

    int month = timeinfo->tm_mon;

    int day = timeinfo->tm_mday;*/

    printf("\n %d year %d month %d day\n",timeinfo->tm_year,timeinfo->tm_mon,timeinfo->tm_mday);

    int year = 2018;
    int month = 1;
    int day = 2;

    sysyear = year;
    sysmonth = month;
    sysday = day;

    yeartemp = year;

    monthtemp = month;

    daytemp = day;

    //long int location = 0;

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

    fbptemp = fbp;//赋初值

    x = 100; y = 100;       // Where we are going to put the pixel

    if(vinfo.bits_per_pixel == 16)  // 16bpp only
    {
       // 16bpp framebuffer test
        printf("16bpp framebuffer test\n");
        printf("two bytes in fbp is a pixel of LCD, just set the value of fbp to put color to LCD\n");
        printf("byte format:\n");
        printf("  bit:| 15 14 13 12 11 | 10 9 8 7 6 5 | 4 3 2 1 0 |\n");
        printf("      |     red        |   green      |    blue   |\n");
        // White Screen
        printf("Black Screen\n");
        for(y = 0; y < 480; y++)
        {
            for(x = 0; x < 640; x++)
            {
                *(fbp + y * 640*2 + x*2) = 0x00;
                *(fbp + y * 640*2 + x*2 +1) = 0x00;
            }
        }
        sleep(2);

    }else
    {
        printf("16bpp only!!!\n");
    }
    
    PrintBody(year,month);

    int flag;
    while(1)
    {
        fflush(stdin);
        printf("\nInput Num(0: reset;1: last month;2: next month;3: search)\n");
        scanf("%d",&flag);
        switch(flag)
        {
            case 0: PrintBody(year,month); break;//
            case 1:
                monthtemp--;
                if(monthtemp==0)
                {
                    year--;
                    monthtemp=12;
                }
                PrintBody(year,monthtemp); break;//
            case 2:
                monthtemp++;
                if(monthtemp==13)
                {
                    year++;
                    monthtemp=1;
                }
                PrintBody(year,monthtemp); break;//
            case 3:
                printf("year: ");
                scanf("%d",&yeartemp);
                printf("month: ");
                scanf("%d",&monthtemp);
                PrintBody(yeartemp,monthtemp);
                break;//
        }
    }

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
