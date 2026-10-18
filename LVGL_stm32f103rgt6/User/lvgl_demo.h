#ifndef __LVGL_DEMO_H
#define __LVGL_DEMO_H

#include <stdint.h>

int parse_buffer(uint8_t tmp_buf_rx[], int *x, int *z);

void lvgl_demo(void);   /* lvglÀý³Ì */

#endif




//		if(tmp_buf_rx[1] == '-')
//		{
//			if(tmp_buf_rx[3] == 'Z')
//			{
//				x = -(tmp_buf_rx[2]-'0');
//				if(tmp_buf_rx[8] == ' ' && tmp_buf_rx[7] != ' ')
//				{
//					z = (tmp_buf_rx[4] - '0')*1000 + (tmp_buf_rx[5] - '0')*100 + (tmp_buf_rx[6] - '0')*10 + (tmp_buf_rx[7] - '0')*1;
//				}
//				if(tmp_buf_rx[7] == ' ')
//				{
//					z = (tmp_buf_rx[4] - '0')*100 + (tmp_buf_rx[5] - '0')*10 + (tmp_buf_rx[6] - '0')*1 + (tmp_buf_rx[7] - '0')*0;
//				}
//			}
//			if(tmp_buf_rx[4] == 'Z')
//			{
//				x = -((tmp_buf_rx[2]-'0')*10 + (tmp_buf_rx[3]-'0'));
//				if(tmp_buf_rx[9] == ' ' && tmp_buf_rx[8] != ' ')
//				{
//					z = (tmp_buf_rx[5] - '0')*1000 + (tmp_buf_rx[6] - '0')*100 + (tmp_buf_rx[7] - '0')*10 + (tmp_buf_rx[8] - '0')*1;
//				}
//				if(tmp_buf_rx[8] == ' ')
//				{
//					z = (tmp_buf_rx[5] - '0')*100 + (tmp_buf_rx[6] - '0')*10 + (tmp_buf_rx[7] - '0')*1 + (tmp_buf_rx[8] - '0')*0;
//				}
//			}
//			if(tmp_buf_rx[5] == 'Z')
//			{
//				x = -((tmp_buf_rx[2]-'0')*100 + (tmp_buf_rx[3]-'0')*10 + (tmp_buf_rx[4]-'0'));
//				if(tmp_buf_rx[10] == ' ' && tmp_buf_rx[9] != ' ')
//				{
//					z = (tmp_buf_rx[6] - '0')*1000 + (tmp_buf_rx[7] - '0')*100 + (tmp_buf_rx[8] - '0')*10 + (tmp_buf_rx[9] - '0')*1;
//				}
//				if(tmp_buf_rx[9] == ' ')
//				{
//					z = (tmp_buf_rx[6] - '0')*100 + (tmp_buf_rx[7] - '0')*10 + (tmp_buf_rx[8] - '0')*1 + (tmp_buf_rx[9] - '0')*0;
//				}
//			}
//		}
//		if(tmp_buf_rx[1] != '-')
//		{
//			if(tmp_buf_rx[2] == 'Z')
//			{
//				x = (tmp_buf_rx[1]-'0');
//				if(tmp_buf_rx[7] == ' ' && tmp_buf_rx[6] != ' ')
//				{
//					z = (tmp_buf_rx[3] - '0')*1000 + (tmp_buf_rx[4] - '0')*100 + (tmp_buf_rx[5] - '0')*10 + (tmp_buf_rx[6] - '0')*1;
//				}
//				if(tmp_buf_rx[6] == ' ')
//				{
//					z = (tmp_buf_rx[3] - '0')*100 + (tmp_buf_rx[4] - '0')*10 + (tmp_buf_rx[5] - '0')*1 + (tmp_buf_rx[6] - '0')*0;
//				}
//			}
//			if(tmp_buf_rx[3] == 'Z')
//			{
//				x = (tmp_buf_rx[1]-'0')*10 + (tmp_buf_rx[2]-'0');
//				if(tmp_buf_rx[8] == ' ' && tmp_buf_rx[7] != ' ')
//				{
//					z = (tmp_buf_rx[4] - '0')*1000 + (tmp_buf_rx[5] - '0')*100 + (tmp_buf_rx[6] - '0')*10 + (tmp_buf_rx[7] - '0')*1;
//				}
//				if(tmp_buf_rx[7] == ' ')
//				{
//					z = (tmp_buf_rx[4] - '0')*100 + (tmp_buf_rx[5] - '0')*10 + (tmp_buf_rx[6] - '0')*1 + (tmp_buf_rx[7] - '0')*0;
//				}
//			}
//			if(tmp_buf_rx[4] == 'Z')
//			{
//				x = (tmp_buf_rx[1]-'0')*100 + (tmp_buf_rx[2]-'0')*10 + (tmp_buf_rx[3]-'0');
//				if(tmp_buf_rx[9] == ' ' && tmp_buf_rx[8] != ' ')
//				{
//					z = (tmp_buf_rx[5] - '0')*1000 + (tmp_buf_rx[6] - '0')*100 + (tmp_buf_rx[7] - '0')*10 + (tmp_buf_rx[8] - '0')*1;
//				}
//				if(tmp_buf_rx[8] == ' ')
//				{
//					z = (tmp_buf_rx[5] - '0')*100 + (tmp_buf_rx[6] - '0')*10 + (tmp_buf_rx[7] - '0')*1 + (tmp_buf_rx[8] - '0')*0;
//				}
//			}
//		}


