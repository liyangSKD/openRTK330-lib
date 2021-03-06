
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "nav_math.h"


void fifo_init(fifo_type* fifo, uint8_t* buffer, uint16_t size)
{
	fifo->buffer = buffer;
	fifo->in = 0;
	fifo->out = 0;
	fifo->size = size;
}

void fifo_push(fifo_type* fifo, uint8_t* buffer, uint16_t size)
{
	uint16_t i;

    for (i = 0; i < size; i++)
    {
        fifo->buffer[(fifo->in+i)%fifo->size] = buffer[i];
    }
    fifo->in = (fifo->in + size)%fifo->size;
}

uint16_t fifo_get(fifo_type* fifo, uint8_t* buffer, uint16_t len)
{
	uint16_t lenght;
	uint16_t in = fifo->in;	
	uint16_t i;
	lenght = (in + fifo->size - fifo->out)%fifo->size;
	if(lenght > len)
		lenght = len;
	for(i = 0; i < lenght; i++)
	{
		buffer[i] = fifo->buffer[(fifo->out + i)%fifo->size];
	}
	fifo->out = (fifo->out + lenght)%fifo->size;
	return lenght;
}

uint16_t fifo_status(fifo_type* fifo)
{
	uint16_t lenght;
	lenght = (fifo->in + fifo->size - fifo->out)%fifo->size;
	return lenght;
}


char *i2a(int num, char *str, int radix)
{
    char index[] = "0123456789ABCDEF";
    unsigned unum;
    int i = 0, j, k;

    if (radix == 10 && num < 0)
    {
        unum = (unsigned)-num;
        str[i++] = '-';
    }
    else
    {
        unum = (unsigned)num;
    }

    do
    {
        str[i++] = index[unum % (unsigned)radix];
        unum /= radix;
    } while (unum);
    str[i] = '\0';

    if (str[0] == '-')
        k = 1;
    else
        k = 0;

    for (j = k; j <= (i - 1) / 2; j++)
    {
        char temp;
        temp = str[j];
        str[j] = str[i - 1 + k - j];
        str[i - 1 + k - j] = temp;
    }
    return str;
}

void float2arr(double data, char *a, unsigned char id, unsigned char dd)
{   
    // id:integer digits，dd:decimal digits
    uint64_t temp1, temp2;
    char i;

    if (id == 0)
    {
        if (abs(data) < 10)
        {
            id = 1;
        }
        else if (abs(data) < 100)
        {
            id = 2;
        }
        else if (abs(data) < 1000)
        {
            id = 3;
        }
        else if (abs(data) < 10000)
        {
            id = 4;
        }
        else
        {
            id = 8;
        }
    }

    if (data >= 0)
    {
        temp1 = data * pow(10, dd);
        temp2 = ((int)(data)) * pow(10, dd);
        temp2 = temp1 - temp2;
        temp1 = (int)data;
        for (i = id; i > 0; i--)
        {
            *(a + i - 1) = (int)temp1 % 10 + 0x30;
            temp1 = (int)(temp1 / 10);
        }
        *(a + id) = '.';
        for (i = id + dd; i > id; i--)
        {
            *(a + i) = temp2 % 10 + 0x30;
            temp2 = temp2 / 10;
        }
    }
    else
    {
        data = 0 - data;
        temp1 = data * pow(10, dd);
        temp2 = ((int)(data)) * pow(10, dd);
        temp2 = temp1 - temp2;
        temp1 = (int)data;
        *a = 0x2D;
        for (i = id; i > 0; i--)
        {
            *(a + i) = (int)temp1 % 10 + 0x30;
            temp1 = (int)(temp1 / 10);
        }
        *(a + id + 1) = '.';
        for (i = id + dd + 1; i > id + 1; i--)
        {
            *(a + i) = temp2 % 10 + 0x30;
            temp2 = temp2 / 10;
        }
    }
}


int print_nmea_gga(double *ep, double *xyz, int nsat, int type, double dop, 
	double age, char *buff)
{
	double h, pos[3], dms1[3], dms2[3];
	char *p = (char *)buff, *q, sum;
	char buf[20] = {0};

	if ((xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]) < 1.0)
	{
		strcpy(buff,"$GPGGA,,,,,,,,,,,,,,");
		for (q = (char *)buff + 1, sum = 0; *q; q++)
			sum ^= *q;
		strcat(buff, "*");
		memset(buf, 0, 20);
		i2a(sum, buf, 16);
		strcat(buff, buf);
		strcat(buff, "\r\n");

	}
	else
	{
		ecef2pos(xyz, pos);
		h = 0.0; 
		deg2dms(fabs(pos[0]) * RAD_TO_DEG, dms1, 7);
		deg2dms(fabs(pos[1]) * RAD_TO_DEG, dms2, 7);

		strcpy(buff,"$GPGGA,");

		float2arr(ep[3] * 10000 + ep[4] * 100 + ep[5] + 0.001, buf, 6, 2);
		strcat(buff, buf);
		strcat(buff, ",");

		memset(buf, 0, 20);
		float2arr(dms1[0] * 100 + dms1[1] + dms1[2] / 60.0, buf, 4, 7);
		strcat(buff, buf);
		strcat(buff, pos[0] >= 0 ? ",N," : ",S,");

		memset(buf, 0, 20);
		float2arr(dms2[0] * 100 + dms2[1] + dms2[2] / 60.0, buf, 5, 7);
		strcat(buff, buf);
		strcat(buff, pos[1] >= 0 ? ",E," : ",W,");

		memset(buf, 0, 20);
		// float2arr(type,buf,1,0);
		i2a(type, buf, 10);
		strcat(buff, buf);
		strcat(buff, ",");

		memset(buf, 0, 20);
		float2arr(nsat, buf, 2, 0);
		buf[2] = 0;
		strcat(buff, buf);
		strcat(buff, ",");

		memset(buf, 0, 20);
		float2arr(dop, buf, 0, 1);
		strcat(buff, buf);
		strcat(buff, ",");

		memset(buf, 0, 20);
		float2arr(pos[2] - h, buf, 0, 3);
		strcat(buff, buf);
		strcat(buff, ",M,");

		memset(buf, 0, 20);
		float2arr(h, buf, 0, 3);
		strcat(buff, buf);
		strcat(buff, ",M,");

		memset(buf, 0, 20);
		float2arr(age, buf, 0, 1);
		strcat(buff, buf);
		strcat(buff, ",");

		for (q = (char *)buff + 1, sum = 0; *q; q++)
			sum ^= *q; /* check-sum */

		strcat(buff, "*");
		memset(buf, 0, 20);
		i2a(sum, buf, 16);
		strcat(buff, buf);

		strcat(buff, "\r\n");
	}
	return strlen(buff);
}

int print_pos_gga(gtime_t time, double *pos, int num_of_sat, int fixID,
	double hdop, double age, char *gga)
{
	double ep[6] = { 0.0 };
	gtime_t ut;

    if (gga != NULL)
    {
        if (fixID)
        {
            ut = gpst2utc(time);
            time2epoch(ut, ep);
            return print_nmea_gga(ep, pos, num_of_sat, fixID, hdop, age, gga);
        }
    }

    return 0;
}