
#include "sys.h"
// #include "common.h"
#include "ssd2828.h"
//#include "interrupts.h"
bit ssd2828_cfg_ok=0;
code const u8 pa_tab1[]={0xff, 0x98, 0x81, 0x03};

code const u8 pa_tab4[]={0xff, 0x98, 0x81, 0x04};
code const u8 pa_tab5[]={0xff, 0x98, 0x81, 0x01};

code const u8 pa_tab8[]={0xff, 0x98, 0x81, 0x00};
code const u8 pa_tab9[]={0x11};
code const u8 pa_tab10[]={0x29};
code const u8 pa_tab11[]={0x35};



#if 0
void delay_us(u8 t){
	u8 i,j;
	for(i=0;i<t;i++){
		for(j=0;j<4;j++);
	}
}
#endif

u16 ssd2828_readID(void){
	u16 ssd2828_id=0,reg_temp=0xb0fa;
	u8 i;
	csx=0;
	sdc=0;		//??
	delay_us(5);
	for(i=0;i<16;i++){
		
		if(reg_temp&0x8000)		//msb
			sdi=1;
		else
			sdi=0;
		sck=0;
		delay_us(5);
		sck=1;
		delay_us(5);
		reg_temp<<=1;			//???
	}
	sdc=1;
	delay_us(5);
	for(i=0;i<16;i++){
		sck=0;
		delay_us(5);
		sck=1;
		delay_us(5);
		ssd2828_id<<=1;
		if(sdo)
			ssd2828_id|=1;
	}
	csx=1;
	delay_us(5);
	return ssd2828_id;
}

u16 ssd2828_readReg(u8 addr_reg){
	u16 reg_value=0,reg_temp;
	u8 i;
	reg_temp=0x00fa|(addr_reg<<8);
	csx=0;
	sdc=0;		//??
	delay_us(5);
	for(i=0;i<16;i++){
		
		if(reg_temp&0x8000)		//msb
			sdi=1;
		else
			sdi=0;
		sck=0;
		delay_us(5);
		sck=1;
		delay_us(5);
		reg_temp<<=1;			//???
	}
	sdc=1;
	delay_us(5);
	for(i=0;i<16;i++){
		sck=0;
		delay_us(5);
		sck=1;
		delay_us(5);
		reg_value<<=1;
		if(sdo)
			reg_value|=1;
	}
	csx=1;
	delay_us(5);
	return reg_value;	
}

void ssd2828_writecmd(u8 cmd){
	u8 i,ssd2828_cmd;
	ssd2828_cmd=cmd;
	csx=0;
	sdc=0;
	delay_us(5);
	for(i=0;i<8;i++){
		if(ssd2828_cmd&0x80)
			sdi=1;
		else
			sdi=0;
		sck=0;
		delay_us(5);
		sck=1;
		delay_us(5);
		ssd2828_cmd<<=1;	
	}
	csx=1;
	delay_us(5);
}

void ssd2828_regconfig(u8 reg_addr,u16 reg_data){
	u8 i,addr_temp,data_H,data_L;
	addr_temp=reg_addr;
	data_H=reg_data>>8;
	data_L=reg_data;
	csx=0;
	sdc=0;
	delay_us(5);
	for(i=0;i<8;i++){		//???????
		if(addr_temp&0x80)
			sdi=1;
		else
			sdi=0;
		sck=0;
		delay_us(5);
		sck=1;
		delay_us(5);
		addr_temp<<=1;
	}
	sdc=1;
	delay_us(5);
	for(i=0;i<8;i++){		//lsb
		if(data_L&0x80)
			sdi=1;
		else
			sdi=0;
		sck=0;
		delay_us(5);
		sck=1;
		delay_us(5);
		data_L<<=1;
	}
	for(i=0;i<8;i++){
		if(data_H&0x80)
			sdi=1;
		else
			sdi=0;
		sck=0;
		delay_us(5);
		sck=1;
		delay_us(5);
		data_H<<=1;
	}
	csx=1;
	delay_us(5);
}

void ssd2828_writepackage(u8 *pbuf,u16 len){
	u16 lenth;
	u8 i,temp;
	lenth=len;
	ssd2828_regconfig(0xbc,lenth);
	ssd2828_regconfig(0xbe,lenth);
	ssd2828_writecmd(0xbf);
	csx=0;
	sdc=1;
	delay_us(5);
	while(lenth--){
		temp=*pbuf++;
		for(i=0;i<8;i++){
			if(temp&0x80)
				sdi=1;
			else
				sdi=0;
			sck=0;
			delay_us(5);
			sck=1;
			delay_us(5);
			temp<<=1;
		}
	}
	csx=1;
	delay_us(5);
}



void W_COM(u8 addr,u8 dat){
	u16 lenth;
	u8 i,temp;
	u8 atr[2];
	u8* pbuf = atr;
	atr[0] = addr;
	atr[1] = dat;
	lenth=2;
	ssd2828_regconfig(0xbc,lenth);
	ssd2828_regconfig(0xbe,lenth);
	ssd2828_writecmd(0xbf);
	csx=0;
	sdc=1;
	delay_us(15);
	while(lenth--){
		temp=*pbuf++;
		for(i=0;i<8;i++){
			if(temp&0x80)
				sdi=1;
			else
				sdi=0;
			sck=0;
			delay_us(15);
			sck=1;
			delay_us(15);
			temp<<=1;
		}
	}
	csx=1;
	delay_us(15);
}

/******************************************
	ssd2828配置为non-burst mode，NVD=0,BLLP=0
	ssd2828仅在垂直消隐期间发送非视频数据包
//*******************************************/
//u16 ssd2828_read_mipi(u16 mipi_reg){		//仅读取一个字
//	u16 reg_value=0,mipi_reg_value=0;
//	u8 mipi_read_cnt=0;
//	do{
//		ssd2828_regconfig(0xb7,0x02CB);		//REN=1
////		ssd2828_regconfig(0xbb,0x0002);		//设定接收速率，必须比送图速率小
//		ssd2828_regconfig(0xc1,0x0001);		//设置读取mipi slave数据的最大长度
////		ssd2828_regconfig(0xc0,0x0100);		//停止送图，准备读取mipi slave寄存器
//		ssd2828_regconfig(0xbc,0x0001);		//设置带读取数据包数量
//		ssd2828_regconfig(0xbf,mipi_reg);		//读取mipi slave寄存器
//		ssd2828_regconfig(0xd4,0x00fa);		//复位SPI读寄存器
//		reg_value=ssd2828_readReg(0xc6);
//		reg_value&=0x01;		//RDR=0xc6.0
//		mipi_read_cnt++;
//	} while((reg_value==0)&&(mipi_read_cnt<3));
//	//RDR=1，则表明mipi slave已回复
//	mipi_reg_value=ssd2828_readReg(0xff);	//mipi slave返回的数据存储在0xff寄存器地址
//	ssd2828_regconfig(0xb7,0x024b);				//恢复正常显示
//	return mipi_reg_value;
//}

void W_write()
{
	ssd2828_writepackage(pa_tab1,sizeof(pa_tab1));
	W_COM(0x01,0x00);
	W_COM(0x02,0x00);
	W_COM(0x03,0x53);      //STVA=STV1~4
	W_COM(0x04,0xD3);       //STVB=STV0 
	W_COM(0x05,0x00);        
	W_COM(0x06,0x0D);       //STVA_Rise
	W_COM(0x07,0x08);       //STVB_Rise 
	W_COM(0x08,0x00);        
	W_COM(0x09,0x00);       //FTI1R(A) STV1=3.6H      
	W_COM(0x0a,0x00);          
	W_COM(0x0b,0x00);      
	W_COM(0x0c,0x00);    
	W_COM(0x0d,0x00); //FTI2F(B) STV0=3.6H    
	W_COM(0x0e,0x00);        
	W_COM(0x0f,0x28);       //CLW1(ALR) Duty=45%
	W_COM(0x10,0x28);       //CLW2(ARR) Duty=45%
	W_COM(0x11,0x00);           
	W_COM(0x12,0x00);       
	W_COM(0x13,0x00);        
	W_COM(0x14,0x00);
	W_COM(0x15,0x00);        
	W_COM(0x16,0x00);        
	W_COM(0x17,0x00);        
	W_COM(0x18,0x00);        
	W_COM(0x19,0x00);
	W_COM(0x1a,0x00);
	W_COM(0x1b,0x00);   
	W_COM(0x1c,0x00);
	W_COM(0x1d,0x00);
	W_COM(0x1e,0x40);       //CLKA 40?郯胜? C0も笆は(X8把σCLKB)
	W_COM(0x1f,0x80);       //C0
	W_COM(0x20,0x06);       //CLKA_Rise
	W_COM(0x21,0x01);       //CLKA_Fall
	W_COM(0x22,0x00);        
	W_COM(0x23,0x00);        
	W_COM(0x24,0x00);        
	W_COM(0x25,0x00);        
	W_COM(0x26,0x00);
	W_COM(0x27,0x00);
	W_COM(0x28,0x33);       //CLK Phase
	W_COM(0x29,0x33);       //CLK overlap
	W_COM(0x2a,0x00);  
	W_COM(0x2b,0x00);
	W_COM(0x2c,0x04);       //GCH R
	W_COM(0x2d,0x04);       //GCL R 
	W_COM(0x2e,0x05);       //GCH F        
	W_COM(0x2f,0x05);       //GCL F       
	W_COM(0x30,0x00);
	W_COM(0x31,0x00);
	W_COM(0x32,0x00);       //GCH/L ext2/1︽??  5E 01:31   5E 00:42      
	W_COM(0x33,0x00);
	W_COM(0x34,0x03);      
	W_COM(0x35,0x00);       //GCH/L 跋丁 00:VS玡 01:VS?? 10:阁VS 11:frameい             
	W_COM(0x36,0x00);
	W_COM(0x37,0x00);       //GCH/L      
	W_COM(0x38,0x96);	
	W_COM(0x39,0x00);
	W_COM(0x3a,0x00); 
	W_COM(0x3b,0x00);
	W_COM(0x3c,0x00);
	W_COM(0x3d,0x00);
	W_COM(0x3e,0x00);
	W_COM(0x3f,0x00);
	W_COM(0x40,0x00);
	W_COM(0x41,0x00);
	W_COM(0x42,0x00);
	W_COM(0x43,0x00);      //GCH/L      
	W_COM(0x44,0x00);
	W_COM(0x50,0x00);
	W_COM(0x51,0x23);
	W_COM(0x52,0x45);   
	W_COM(0x53,0x67);      
	W_COM(0x54,0x89);        
	W_COM(0x55,0xab);       
	W_COM(0x56,0x01); 
	W_COM(0x57,0x23);        
	W_COM(0x58,0x45);  
	W_COM(0x59,0x67);          
	W_COM(0x5a,0x89);      
	W_COM(0x5b,0xab);    
	W_COM(0x5c,0xcd);    
	W_COM(0x5d,0xef);
	W_COM(0x5e,0x00);       
	W_COM(0x5f,0x08);      
	W_COM(0x60,0x08);           
	W_COM(0x61,0x06);       
	W_COM(0x62,0x06);        
	W_COM(0x63,0x01);
	W_COM(0x64,0x01);        
	W_COM(0x65,0x00);        
	W_COM(0x66,0x00);        
	W_COM(0x67,0x02);        
	W_COM(0x68,0x15);
	W_COM(0x69,0x15);
	W_COM(0x6a,0x14);   
	W_COM(0x6b,0x14);
	W_COM(0x6c,0x0d);
	W_COM(0x6d,0x0d);       //CLKA 40?郯胜? C0も笆は(X8把σCLKB)
	W_COM(0x6e,0x0c);       //C0
	W_COM(0x6f,0x0c);       //CLKA_Rise
	W_COM(0x70,0x0f);       //CLKA_Fall
	W_COM(0x71,0x0f);        
	W_COM(0x72,0x0e);        
	W_COM(0x73,0x0e);        
	W_COM(0x74,0x02);
	W_COM(0x75,0x08); 
	W_COM(0x76,0x08);    
	W_COM(0x77,0x06);    
	W_COM(0x78,0x06);    
	W_COM(0x79,0x01);    
	W_COM(0x7a,0x01);     
	W_COM(0x7b,0x00);   
	W_COM(0x7c,0x00);    
	W_COM(0x7d,0x02);    
	W_COM(0x7e,0x15);
	W_COM(0x7f,0x15);  
	W_COM(0x80,0x14);  
	W_COM(0x81,0x14);
	W_COM(0x82,0x0d);     
	W_COM(0x83,0x0d);  
	W_COM(0x84,0x0c);     
	W_COM(0x85,0x0c);
	W_COM(0x86,0x0f);
	W_COM(0x87,0x0f);
	W_COM(0x88,0x0e);   
	W_COM(0x89,0x0e);  
	W_COM(0x8A,0x02);  
	ssd2828_writepackage(pa_tab4,sizeof(pa_tab4));
	W_COM(0x6e,0x2b);
	W_COM(0x6F,0x37);
	W_COM(0x3A,0xA4);
	W_COM(0x8D,0x1a);
	W_COM(0x87,0xBA);
	W_COM(0xB2,0xD1);
	W_COM(0x88,0x0B);
	W_COM(0x38,0x01);
	W_COM(0x39,0x00);
	W_COM(0xB5,0x07);
	W_COM(0x31,0x25);
	W_COM(0x3B,0x98);
	ssd2828_writepackage(pa_tab5,sizeof(pa_tab5));
	W_COM(0x22,0x0A);
	W_COM(0x31,0x00);
	W_COM(0x53,0x40);
	W_COM(0x55,0x40);
	W_COM(0x50,0x99);
	W_COM(0x51,0x94);
	W_COM(0x60,0x10);
	W_COM(0x62,0x20);
	W_COM(0xA0,0x00);
	W_COM(0xA1,0x00);
	W_COM(0xA2,0x15);
	W_COM(0xA3,0x14);
	W_COM(0xA4,0x1B);
	W_COM(0xA5,0x2F);
	W_COM(0xA6,0x25);
	W_COM(0xA7,0x24);
	W_COM(0xA8,0x80);
	W_COM(0xA9,0x1F);
	W_COM(0xAA,0x2C);
	W_COM(0xAB,0x6C);
	W_COM(0xAC,0x16);
	W_COM(0xAD,0x14);
	W_COM(0xAE,0x4D);
	W_COM(0xAF,0x20);
	W_COM(0xB0,0x29);
	W_COM(0xB1,0x4F);
	W_COM(0xB2,0x5F);
	W_COM(0xB3,0x23);
	W_COM(0xC0,0x00);
	W_COM(0xC1,0x2e);
	W_COM(0xC2,0x3b);
	W_COM(0xC3,0x15);
	W_COM(0xC4,0x16);
	W_COM(0xC5,0x28);
	W_COM(0xC6,0x1a);
	W_COM(0xC7,0x1c);
	W_COM(0xC8,0xa7);
	W_COM(0xC9,0x1b);
	W_COM(0xCA,0x28);
	W_COM(0xCB,0x92);
	W_COM(0xCC,0x1f);
	W_COM(0xCD,0x1c);
	W_COM(0xCE,0x4b);
	W_COM(0xCF,0x1f);
	W_COM(0xD0,0x28);
	W_COM(0xD1,0x4e);
	W_COM(0xD2,0x5C);
	W_COM(0xD3,0x23);
	ssd2828_writepackage(pa_tab8,sizeof(pa_tab8));
	ssd2828_writepackage(pa_tab9,sizeof(pa_tab9));
	delay_ms(140);
	ssd2828_writepackage(pa_tab10,sizeof(pa_tab10));
	delay_ms(120);
	ssd2828_writepackage(pa_tab11,sizeof(pa_tab11));
	delay_ms(170);
}



void lcd_pa_set(void){
//	u8 lcd_vs,lcd_hs,lcd_vbp,lcd_hbp,lcd_vfp,lcd_hfp;
//	u16 lcd_sw,lcd_bp,lcd_fp;
//	lcd_vs=VS;
//	lcd_hs=HS;
//	lcd_vbp=VBP;
//	lcd_hbp=HBP;
//	lcd_vfp=VFP;
//	lcd_hfp=HFP;
//	lcd_sw=(lcd_vs<<8)| lcd_hs;
//	lcd_bp=(lcd_vbp<<8)| lcd_hbp;
//	lcd_fp=(lcd_vfp<<8)| lcd_hfp;
//	ssd2828_regconfig(0xb1,lcd_sw);			//H:VSA,L:HSA
//	ssd2828_regconfig(0xb2,lcd_bp);			//H:VBP,L:HBP
//	ssd2828_regconfig(0xb3,lcd_fp);			//H:VFP,L:HFP
//	ssd2828_regconfig(0xb4,HACT);			//HACT
//	ssd2828_regconfig(0xb5,VACT);			//VACT
	ssd2828_regconfig(0xb1,0x0806);			//H:VSA,L:HSA
	ssd2828_regconfig(0xb2,0x1822);			//H:VBP,L:HBP
	ssd2828_regconfig(0xb3,0x6812);			//H:VFP,L:HFP
	ssd2828_regconfig(0xb4,800);			//HACT
	ssd2828_regconfig(0xb5,1280);			//VACT
}

void ssd2828_init(void){
//#if 1
//	u16 temp=0;
//	temp=ssd2828_readID();
//	if(temp!=0x2828)
//		return;
//#endif
	//??ssd2828????mipi??,????mipi??
	ssd2828_regconfig(0xb7,0x0050);
	ssd2828_regconfig(0xb8,0x0000);
	ssd2828_regconfig(0xb9,0x0000);
	//ssd2828_regconfig(0xba,0x4114);
	ssd2828_regconfig(0xba,0x8013);//
	ssd2828_regconfig(0xbb,0x0006);
	ssd2828_regconfig(0xb9,0x0001);
	//ssd2828_regconfig(0xde,0x0003);
	ssd2828_regconfig(0xde,0x0000);
	ssd2828_regconfig(0xc9,0x2302);
	delay_ms(10);
	//ssd2828_regconfig(0xb7,0x0210);
//	ssd2828_regconfig(0xb7,0x0210);
//	ssd2828_regconfig(0xbd,0x0000);
//	delay_ms(20);
	
	//??mipi?????
	//C068_code_download();
//	T088_code_download();
	//不需下载CODE
	
	//??ssd2828?????,????
	
  W_write();
	
	ssd2828_regconfig(0xb7,0x0151);
	ssd2828_regconfig(0xb8,0x0000);
	ssd2828_regconfig(0xb9,0x0000);
	//ssd2828_regconfig(0xba,0x8029);			//801B osc=12M,pll=324M;8029 osc=12M,pll=492M
	//ssd2828_regconfig(0xbb,0x0007);
//	ssd2828_regconfig(0xba,0xc03f);	//TPS02
	ssd2828_regconfig(0xba,0x8029);	//0X804B
	//ssd2828_regconfig(0xba,0xC050);			//C050 osc=12M,
	ssd2828_regconfig(0xbb,0x0005);
	ssd2828_regconfig(0xb9,0x0001);
	ssd2828_regconfig(0xc9,0x2302);
	delay_ms(10);
	ssd2828_regconfig(0xca,0x2301);		//MIPI????
	ssd2828_regconfig(0xcb,0x0510);
	ssd2828_regconfig(0xcc,0x1005);
	ssd2828_regconfig(0xd0,0x00ff);
	
	lcd_pa_set();		//??LCD?????
	
	//ssd2828_regconfig(0xb6,0x000B);		//24bit rgb
	ssd2828_regconfig(0xb6,0x00D3);	//Non burst mode 24 bit rgb
	ssd2828_regconfig(0xde,0x0003);		//4 lane
	//ssd2828_regconfig(0xde,0x0001);		//2 lane
	ssd2828_regconfig(0xd6,0x0004);		//04=BGR 05=RGB
	//ssd2828_regconfig(0xb7,0x024B);		//024b 0659 0210
	ssd2828_regconfig(0xb7,0x024b);//
	delay_ms(60);
//	ssd2828_writecmd(0x2c);
//#if bist
//	ssd2828_regconfig(0xee,0x0200);
//	delay_ms(20);
//	ssd2828_regconfig(0xee,0x0600);
//#endif
	ssd2828_cfg_ok=1;
	
}
