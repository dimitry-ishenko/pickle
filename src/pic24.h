/*
 * Copyright (C) 2005-2020 Darron Broad
 * All rights reserved.
 *
 * This file is part of Pickle Microchip PIC ICSP.
 *
 * Pickle Microchip PIC ICSP is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * Pickle Microchip PIC ICSP is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Pickle Microchip PIC ICSP. If not, see http://www.gnu.org/licenses/
 */

#ifndef _PIC24_H
#define _PIC24_H

/******************************************************************************
 * 24-bit CONFIGURATION
 *****************************************************************************/

#define PIC24_MASK (0x00FFFFFF)
#define PIC24_CALIB_MAX  (256)
#define PIC24_CONFIG_MAX (256)
#define PIC24_FUID_MAX   (256)
#define PIC24_CONFIG     (0)
#define PIC24_PART_LOWER (0)
#define PIC24_PART_UPPER (1)
#define PIC24_PART_MAX   (2)
#define PIC24_FBTSEQ     (0xFC)

struct pic24_config {
	uint32_t calib[PIC24_CALIB_MAX];			/* CALIBRATION WORDS 	 */
	uint32_t config[PIC24_PART_MAX][PIC24_CONFIG_MAX]; 	/* CONFIG BYTES/WORDS	 */
	uint32_t fboot;						/* FBOOT WORD            */
	uint32_t fuid[PIC24_FUID_MAX];				/* UNIT/FUID BYTES/WORDS */
	uint32_t deviceid;					/* FF0000 		 */
	uint32_t revision;					/* FF0002 		 */
#if 0
	char pepath[STRLEN];					/* PE FILE PATH		 */
#endif
	uint16_t tblpag;					/* TBLPAG REGISTER ADDR	 */
	uint16_t nvmcon;					/* NVMCON REGISTER ADDR	 */
	uint16_t visi;						/* VISI REGISTER ADDR	 */
	uint8_t tblnop;						/* TBL READ NOP COUNT	 */
	uint8_t gotonop;					/* GOTO NOP COUNT	 */
};

/* NVM register addresses */
#define PIC24_NVMCON	(pic24_conf.nvmcon)
#define PIC24_NVMADR	(pic24_conf.nvmcon + 2)
#define PIC24_NVMADRU	(pic24_conf.nvmcon + 4)
#define PIC24_NVMKEY	(pic24_conf.nvmcon + 6)

#if 0
struct pic24_dstab {
	uint32_t datasheet;
	char filename[STRLEN];
	uint8_t appid;
};
#endif

struct pic24_dsmap {
	char devicename[STRLEN];
	uint16_t deviceid;
	uint32_t flash;		/* flash size in words                   */
	uint16_t eeprom;	/* eeprom size in bytes 0=unsupported    */
	uint32_t datasheet;
	uint32_t configaddr;	/* CONFIG BASE                           */
	uint8_t nconfig;	/* NUMBER OF CONFIG WORDS AT CONFIG BASE */
	uint32_t fuidaddr;	/* UNIT/FUID BASE                        */
	uint8_t nfuid;		/* NUMBER OF FUID WORDS AT FUID BASE     */
	uint32_t calibaddr;	/* CALIBRATION ADDRESS                   */
	uint8_t ncalib;		/* NUMBER OF CALIBRATION WORDS           */
	uint8_t codepanelsize;	/* PROGRAM FLASH PANEL SIZE              */
	uint8_t datapanelsize;	/* DATA EEPROM PANEL SIZE                */
	uint16_t erasepanelsize;/* ERASE PANEL SIZE                      */
	uint8_t latch;		/* LATCHES AT 0xFA0000                   */
	uint32_t fbootaddr;	/* FBOOT ADDRESS                         */
	uint32_t configlower;	/* LOWER CONFIG BASE                     */
	uint8_t mempanelsize;	/* MEMORY PANEL SIZE (NVMSRCADR SUPPORT) */
};

/******************************************************************************
 * MEMORY
 *****************************************************************************/

/*
 * DUAL PARTITION FLASH
 */
#define PIC24_CODE_LOWER (0x000000) /* LOWER PARTITION */
#define PIC24_CODE_UPPER (0x400000) /* UPPER PARTITION */

/*
 * EXECUTIVE, UNIT / FUID, UDID & OTP (FOR INSPECTION ONLY)
 */
#define PIC24_EXEC_LOW      (0x00800000) /* EXECUTIVE LOW          */
#define PIC24_EXEC_HIGH     (0x00800800) /* EXEC, CALIBRATION      */
#define PIC24_GAB_EXEC_LOW  (0x00800100) /* EXECUTIVE LOW          */
#define PIC24_GA4_EXEC_HIGH (0x00801400) /* EXEC, UDID, OTP        */
#define PIC24_GA6_EXEC_HIGH (0x00801800) /* EXEC, UDID, OTP        */
#define PIC24_GA7_EXEC_HIGH (0x00801800) /* EXEC, UDID, OTP        */
#define dsPIC30F_EXEC_HIGH  (0x00800600) /* EXEC, UNIT             */
#define dsPIC33E_EXEC_HIGH  (0x00801000) /* EXEC, FUID / UDID, OTP */

/*
 * EXECUTIVE APP ID (UNSUPPORTED)
 */
#if 0
#define PIC24_APPID_ADDR     (0x008007F0) /* APP ID WORD FOR PIC24FJ   */
#define dsPIC30F_APPID_ADDR  (0x008005BE) /* APP ID WORD FOR dsPIC30F  */
#define dsPIC33EP_APPID_ADDR (0x00800FF0) /* APP ID WORD FOR dsPIC33EP */
#define dsPIC33EV_APPID_ADDR (0x00800BFE) /* APP ID WORD FOR dsPIC33EV */
#endif

/*
 * CALIBRATION
 */
#define PIC24_CALIB_ADDR (0x008007F4) /* up to 6 calibration words */

/*
 * UDID (UNSUPPORTED)
 */
#if 0
#define dsPIC33EV_GM_UDID (0x00800CF0)
#define dsPIC33EP_GS_UDID (0x00800F00)
#define PIC24_GA4_UDID    (0x00801300)
#define PIC24_GA6_UDID    (0x00801600)
#define PIC24_GA7_UDID    (0x00801600)
#endif

/*
 * OTP (UNSUPPORTED)
 */
#if 0
#define dsPIC33EV_GM_OTP (0x00800F80) /* .. 0x801000 */
#define dsPIC33EP_GS_OTP (0x00800F80) /* .. 0x801000 */
#define PIC24_GA4_OTP    (0x00801380) /* .. 0x801400 */
#define PIC24_GA6_OTP    (0x00801700) /* .. 0x801800 */
#define PIC24_GA7_OTP    (0x00801700) /* .. 0x801800 */
#endif

/*
 * DUAL PARTITION CONFIG
 *
 *  00 Privileged Dual Partition Mode
 *  01 Protected Dual Partition Mode
 *  10 Dual Partition Mode
 *  11 Standard Mode (Single Partition, default)
 */
#define dsPIC_FBOOT      (0x801000) /* dsPIC33EP64GS502                   */
#define PIC24_FBOOT      (0x801800) /* PIC24FJ256GA412 / PIC24FJ1024GA610 */
#define PIC24_FBOOT_MASK (0x000003)
#define PIC24_DUALPRIV   (0)        /* 0xFFFFFC */
#define PIC24_DUALPROT   (1)        /* 0xFFFFFD */
#define PIC24_DUAL       (2)        /* 0xFFFFFE */
#define PIC24_SINGLE     (3)        /* 0xFFFFFF */

/*
 * CONFIG SPACE
 */
#define PIC24_CONFIG_ADDR    (0x00F80000) /* base address              */
#define PIC24_CONFIG_MASK    (0x0000FFFF) /* 16-bit word mask          */
#define PIC24_CONFIG_PENDING (0x80000000) /* config/fuid write pending */

/*
 * DEVICE ID & REVISION
 */
#define PIC24_DEVID         (0x00FF0000)
#define PIC24_DEVREV        (0x00FF0002)
#define PIC24FJ_FAMILY_MASK (0x3FC0) /* FAMID<7:0>                            */
#define PIC24FJ_DEV_MASK    (0x003F) /* DEV  <5:0>                            */
#define PIC24FJ_MAJRV_MASK  (0x01C0) /* MAJRV<2:0>                            */
#define PIC24FJ_DOT_MASK    (0x0007) /* DOT  <2:0>                            */
#define dsPIC30F_MASK       (0xFFC0) /* MASK<9:0>                             */
#define dsPIC30F_VAR_MASK   (0x003F) /* VARIANT<5:0>                          */
#define dsPIC30F_PROC_MASK  (0xF000) /* PROC<3:0> = 0x001                     */
#define dsPIC30F_REV_MASK   (0x0FC0) /* REV <5:0> 0000000=A 000001=B 000010=C */
#define dsPIC30F_DOT_MASK   (0x003F) /* DOT <5:0>                             */

/*
 * PIC24FJ NVCON  NVMOP<3:0>
 *    1111 MEMORY BULK ERASE
 *    0011 MEMORY WRITE WORD
 *    0010 MEMORY PAGE ERASE
 *    0001 MEMORY WRITE ROW
 */
#if 0
#define PIC24FJ_ERASE_CHIP (0x404F) /* WREN | ERASE  | NVMOP3 | NVMOP2 | NVMOP1 | NVMOP0 */
#define PIC24FJ_WRITE_WORD (0x4003) /* WREN |                            NVMOP1 | NVMOP0 */
#define PIC24FJ_ERASE_PAGE (0x4042) /* WREN | ERASE  |                   NVMOP1          */
#define PIC24FJ_WRITE_ROW  (0x4001) /* WREN |                                     NVMOP0 */
#define PIC24FJ_ERASE_PAGE_SIZE (512)
#define PIC24FJ_WRITE_ROW_SIZE  (64)

#define dsPIC33FJ_ERASE_CHIP (0x404F) /* NEMORY BULK ERASE              */
#define dsPIC33FJ_ERASE_GEN  (0x404D) /* ERASE GENERAL SEGMENT          */
#define dsPIC33FJ_ERASE_PAGE (0x4042) /* PROGRAM MEMORY PAGE ERASE      */
#define dsPIC33FJ_WRITE_WORD (0x4003) /* MEMORY WORD PROGRAM            */
#define dsPIC33FJ_ERASE_PAGE_SIZE (512)
#define dsPIC33FJ_WRITE_ROW_SIZE  (64)

#define dsPIC30F_ERASE_ROW         (0x4071) /* WREN | PROGOP<6:0> 111 0001 */
#define dsPIC30F_ERASE_EXEC        (0x4072) /* WREN | PROGOP<6:0> 111 0010 */
#define dsPIC30F_ERASE_CHIP        (0x407F) /* WREN | PROGOP<6:0> 111 1111 */
#define dsPIC30F_ERASE_GEN         (0x404E) /* WREN | PROGOP<6:0> 100 1110 */
#define dsPIC30F_ERASE_SECGEN      (0x405E) /* WREN | PROGOP<6:0> 101 1110 */
#define dsPIC30F_ERASE_BOOTSECGEN  (0x406E) /* WREN | PROGOP<6:0> 110 1110 */
#define dsPIC30F_ERASE_EEPROM_WORD (0x4074) /* WREN | PROGOP<6:0> 111 0100 */
#define dsPIC30F_ERASE_EEPROM_ROW  (0x4075) /* WREN | PROGOP<6:0> 111 0101 */
#define dsPIC30F_ERASE_EEPROM_GEN  (0x4046) /* WREN | PROGOP<6:0> 100 0110 */
#define dsPIC30F_ERASE_EEPROM_SEC  (0x4056) /* WREN | PROGOP<6:0> 101 0110 */
#define dsPIC30F_ERASE_EEPROM_BOOT (0x4066) /* WREN | PROGOP<6:0> 110 0110 */
#define dsPIC30F_WRITE_EEPROM_ROW  (0x4005)
#define dsPIC30F_WRITE_EEPROM_WORD (0x4004)
#define dsPIC30F_WRITE_ROW         (0x4001)
#define dsPIC30F_WRITE_WORD        (0x4008)
#define dsPIC30F_WRITE_CODE_SIZE (32)
#define dsPIC30F_WRITE_DATA_SIZE (16)
#endif

/******************************************************************************
 * PICMicro devices
 *****************************************************************************/

/*
 * DS39768D
 *
 * LVP KEY ENTRY
 */
#define DS39768D (39768)
#define PIC24FJ16GA002  (0x0444) /* CONFIG1:002BFE CONFIG2:002BFC (11264) */
#define PIC24FJ16GA004  (0x044C)
#define PIC24FJ32GA002  (0x0445) /* CONFIG1:0057FE CONFIG2:0057FC (22528) */
#define PIC24FJ32GA004  (0x044D)
#define PIC24FJ48GA002  (0x0446) /* CONFIG1:0083FE CONFIG2:0083FC (33792) */
#define PIC24FJ48GA004  (0x044E)
#define PIC24FJ64GA002  (0x0447) /* CONFIG1:00ABFE CONFIG2:00ABFC (44032) */
#define PIC24FJ64GA004  (0x044F)
#define PIC24FJ64GA006  (0x0405)
#define PIC24FJ64GA008  (0x0408)
#define PIC24FJ64GA010  (0x040B)
#define PIC24FJ96GA006  (0x0406) /* CONFIG1:00FFFE CONFIG2:00FFFC (65536) */
#define PIC24FJ96GA008  (0x0409)
#define PIC24FJ96GA010  (0x040C)
#define PIC24FJ128GA006 (0x0407) /* CONFIG1:0157FE CONFIG2:0157FC (88064) */
#define PIC24FJ128GA008 (0x040A)
#define PIC24FJ128GA010 (0x040D)

/*
 * dsPIC30F Flash Programming Specification
 *
 * DO NOT REFER TO DS70102K AS IT CONTAINS INCORRECT INFORMATION
 * REGARDING ICSP MODE ENTRY. USE DS70102H WHICH IS THE LAST
 * KNOWN GOOD VERSION WHICH CORRECTLY DESCRIBES ICSP MODE ENTRY
 * AND THE CORRECT METHOD TO EXIT FROM THE RESET VECTOR UNLIKE
 * ALL LATER DOCUMENTS.
 *
 * 13V HVP 5V VDD
 *
 * DS70102H
 */
#define DS70102H (70102)
#define dsPIC30F2010  (0x0040)
#define dsPIC30F2011  (0x0240)
#define dsPIC30F2012  (0x0241)
#define dsPIC30F3010  (0x01C0)
#define dsPIC30F3011  (0x01C1)
#define dsPIC30F3012  (0x00C1)
#define dsPIC30F3013  (0x00C3)
#define dsPIC30F3014  (0x0160)
#define dsPIC30F4011  (0x0101)
#define dsPIC30F4012  (0x0100)
#define dsPIC30F4013  (0x0141)
#define dsPIC30F5011  (0x0080)
#define dsPIC30F5013  (0x0081)
#define dsPIC30F5015  (0x0200)
#define dsPIC30F5016  (0x0201)
#define dsPIC30F6010  (0x0188) /* MATURE PRODUCT 2013-11-12 */
#define dsPIC30F6010A (0x0281)
#define dsPIC30F6011  (0x0192) /* MATURE PRODUCT 2013-11-12 */
#define dsPIC30F6011A (0x02C0)
#define dsPIC30F6012  (0x0193) /* MATURE PRODUCT 2013-11-12 */
#define dsPIC30F6012A (0x02C2)
#define dsPIC30F6013  (0x0197) /* MATURE PRODUCT 2013-11-12 */
#define dsPIC30F6013A (0x02C1)
#define dsPIC30F6014  (0x0198) /* MATURE PRODUCT 2013-11-12 */
#define dsPIC30F6014A (0x02C3)
#define dsPIC30F6015  (0x0280)

/*
 * DS70659C
 */
#define DS70659C (70659)
#define dsPIC33FJ16GP101  (0x0200)
#define dsPIC33FJ16GP102  (0x0201)
#define dsPIC33FJ32GP101  (0x0A00)
#define dsPIC33FJ32GP102  (0x0A01)
#define dsPIC33FJ32GP104  (0x0A03)
#define dsPIC33FJ16MC101  (0x0202)
#define dsPIC33FJ16MC102  (0x0203)
#define dsPIC33FJ32MC101  (0x0A04)
#define dsPIC33FJ32MC102  (0x0A05)
#define dsPIC33FJ32MC104  (0x0A07)
/* */
#define dsPIC33FJ06GS001  (0x4900)
#define dsPIC33FJ06GS101A (0x4901)
#define dsPIC33FJ06GS102A (0x4904)
#define dsPIC33FJ06GS202A (0x4905)
#define dsPIC33FJ09GS302  (0x4906)

/*
 * DS39919C
 */
#define DS39919C (39919)
#define PIC24F08KA101  (0x0D08)
#define PIC24F16KA101  (0x0D01)
#define PIC24F08KA102  (0x0D0A)
#define PIC24F16KA102  (0x0D03)
/* */
#define PIC24FV16KA301 (0x4509)
#define PIC24FV16KA302 (0x4503)
#define PIC24FV16KA304 (0x4507)
#define PIC24FV32KA301 (0x4519)
#define PIC24FV32KA302 (0x4513)
#define PIC24FV32KA304 (0x4517)
/* */
#define PIC24F16KA301  (0x4508)
#define PIC24F16KA302  (0x4502)
#define PIC24F16KA304  (0x4506)
#define PIC24F32KA301  (0x4518)
#define PIC24F32KA302  (0x4512)
#define PIC24F32KA304  (0x4516)

/*
 * NO FAMILY PROGRAMMING DATA-SHEET AVAILABLE.
 * REFER TO DS39919C ABOVE.
 *
 * DEVID  UNUSED<23:16> FAM<7:0> DEV<7:0>
 * DEVREV UNUSED<23:4>  REV<3:0>
 */
#define PIC24F04KA200 (0x0D02) /* FAM D DEV 2 */
#define PIC24F04KA201 (0x0D00) /* FAM D REV 0 */

/*
 * DS30625D
 *
 * DEVID  UNUSED<23:16> FAM<7:0> DEV<7:0>
 * DEVREV UNUSED<23:4>  REV<3:0>
 */
#define DS30625D (30625)
#define PIC24F16KL402  (0x4B14)
#define PIC24F16KL401  (0x4B1E)
#define PIC24F08KL402  (0x4B04)
#define PIC24F08KL401  (0x4B0E)
#define PIC24F08KL302  (0x4B00)
#define PIC24F08KL301  (0x4B0A)
#define PIC24F08KL201  (0x4B06)
#define PIC24F08KL200  (0x4B05)
#define PIC24F04KL101  (0x4B02)
#define PIC24F04KL100  (0x4B01)
#define PIC24FV16KM204 (0x551F)
#define PIC24FV08KM204 (0x5517)
#define PIC24FV16KM104 (0x550F)
#define PIC24FV16KM202 (0x551B)
#define PIC24FV08KM202 (0x5513)
#define PIC24FV16KM102 (0x550B)
#define PIC24FV08KM102 (0x5503)
#define PIC24FV08KM101 (0x5501)
#define PIC24F16KM204  (0x551E)
#define PIC24F08KM204  (0x5516)
#define PIC24F16KM104  (0x550E)
#define PIC24F16KM202  (0x551A)
#define PIC24F08KM202  (0x5512)
#define PIC24F16KM102  (0x550A)
#define PIC24F08KM102  (0x5502)
#define PIC24F08KM101  (0x5500)

/*
 * DS70152H
 */
#define DS70152H (70152)
#define dsPIC33FJ06GS101   (0x0C00)
#define dsPIC33FJ06GS102   (0x0C01)
#define dsPIC33FJ06GS202   (0x0C02)
#define dsPIC33FJ16GS402   (0x0C04)
#define dsPIC33FJ16GS404   (0x0C06)
#define dsPIC33FJ16GS502   (0x0C03)
#define dsPIC33FJ16GS504   (0x0C05)
#define dsPIC33FJ12GP201   (0x0802)
#define dsPIC33FJ12GP202   (0x0803)
#define dsPIC33FJ12MC201   (0x0800)
#define dsPIC33FJ12MC202   (0x0801)
#define PIC24HJ12GP201     (0x080A)
#define PIC24HJ12GP202     (0x080B)
#define dsPIC33FJ16GP304   (0x0F07)
#define dsPIC33FJ16MC304   (0x0F03)
#define PIC24HJ16GP304     (0x0F17)
#define dsPIC33FJ32GP202   (0x0F0D)
#define dsPIC33FJ32GP204   (0x0F0F)
#define dsPIC33FJ32MC202   (0x0F09)
#define dsPIC33FJ32MC204   (0x0F0B)
#define PIC24HJ32GP202     (0x0F1D)
#define PIC24HJ32GP204     (0x0F1F)
#define dsPIC33FJ64GP206   (0x00C1)
#define dsPIC33FJ64GP306   (0x00CD)
#define dsPIC33FJ64GP310   (0x00CF)
#define dsPIC33FJ64GP706   (0x00D5)
#define dsPIC33FJ64GP708   (0x00D6)
#define dsPIC33FJ64GP710   (0x00D7)
#define dsPIC33FJ64MC506   (0x0089)
#define dsPIC33FJ64MC508   (0x008A)
#define dsPIC33FJ64MC510   (0x008B)
#define dsPIC33FJ64MC706   (0x0091)
#define dsPIC33FJ64MC710   (0x0097)
#define PIC24HJ64GP206     (0x0041)
#define PIC24HJ64GP210     (0x0047)
#define PIC24HJ64GP506     (0x0049)
#define PIC24HJ64GP510     (0x004B)
#define dsPIC33FJ128GP206  (0x00D9)
#define dsPIC33FJ128GP306  (0x00E5)
#define dsPIC33FJ128GP310  (0x00E7)
#define dsPIC33FJ128GP706  (0x00ED)
#define dsPIC33FJ128GP708  (0x00EE)
#define dsPIC33FJ128GP710  (0x00EF)
#define dsPIC33FJ128MC506  (0x00A1)
#define dsPIC33FJ128MC510  (0x00A3)
#define dsPIC33FJ128MC706  (0x00A9)
#define dsPIC33FJ128MC708  (0x00AE)
#define dsPIC33FJ128MC710  (0x00AF)
#define PIC24HJ128GP206    (0x005D)
#define PIC24HJ128GP210    (0x005F)
#define PIC24HJ128GP306    (0x0065)
#define PIC24HJ128GP310    (0x0067)
#define PIC24HJ128GP506    (0x0061)
#define PIC24HJ128GP510    (0x0063)
#define dsPIC33FJ256GP506  (0x00F5)
#define dsPIC33FJ256GP510  (0x00F7)
#define dsPIC33FJ256GP710  (0x00FF)
#define dsPIC33FJ256MC510  (0x00B7)
#define dsPIC33FJ256MC710  (0x00BF)
#define PIC24HJ256GP206    (0x0071)
#define PIC24HJ256GP210    (0x0073)
#define PIC24HJ256GP610    (0x007B)
#define dsPIC33FJ32GP302   (0x0605)
#define dsPIC33FJ32GP304   (0x0607)
#define dsPIC33FJ32MC302   (0x0601)
#define dsPIC33FJ32MC304   (0x0603)
#define dsPIC33FJ64GP202   (0x0615)
#define dsPIC33FJ64GP204   (0x0617)
#define dsPIC33FJ64GP802   (0x061D)
#define dsPIC33FJ64GP804   (0x061F)
#define dsPIC33FJ64MC202   (0x0611)
#define dsPIC33FJ64MC204   (0x0613)
#define dsPIC33FJ64MC802   (0x0619)
#define dsPIC33FJ64MC804   (0x061B)
#define dsPIC33FJ128GP202  (0x0625)
#define dsPIC33FJ128GP204  (0x0627)
#define dsPIC33FJ128GP802  (0x062D)
#define dsPIC33FJ128GP804  (0x062F)
#define dsPIC33FJ128MC202  (0x0621)
#define dsPIC33FJ128MC204  (0x0623)
#define dsPIC33FJ128MC802  (0x0629)
#define dsPIC33FJ128MC804  (0x062B)
#define PIC24HJ32GP302     (0x0645)
#define PIC24HJ32GP304     (0x0647)
#define PIC24HJ64GP202     (0x0655)
#define PIC24HJ64GP204     (0x0657)
#define PIC24HJ64GP502     (0x0675)
#define PIC24HJ64GP504     (0x0677)
#define PIC24HJ128GP202    (0x0665)
#define PIC24HJ128GP204    (0x0667)
#define PIC24HJ128GP502    (0x067D)
#define PIC24HJ128GP504    (0x067F)
#define dsPIC33FJ64GP206A  (0x00C1)
#define dsPIC33FJ64GP306A  (0x00CD)
#define dsPIC33FJ64GP310A  (0x00CF)
#define dsPIC33FJ64GP706A  (0x00D5)
#define dsPIC33FJ64GP708A  (0x00D6)
#define dsPIC33FJ64GP710A  (0x00D7)
#define dsPIC33FJ64MC506A  (0x0089)
#define dsPIC33FJ64MC508A  (0x008A)
#define dsPIC33FJ64MC510A  (0x008B)
#define dsPIC33FJ64MC706A  (0x0091)
#define dsPIC33FJ64MC710A  (0x0097)
#define PIC24HJ64GP206A    (0x0041)
#define PIC24HJ64GP210A    (0x0047)
#define PIC24HJ64GP506A    (0x0049)
#define PIC24HJ64GP510A    (0x004B)
#define dsPIC33FJ128GP206A (0x00D9)
#define dsPIC33FJ128GP306A (0x00E5)
#define dsPIC33FJ128GP310A (0x00E7)
#define dsPIC33FJ128GP706A (0x00ED)
#define dsPIC33FJ128GP708A (0x00EE)
#define dsPIC33FJ128GP710A (0x00EF)
#define dsPIC33FJ128MC506A (0x00A1)
#define dsPIC33FJ128MC510A (0x00A3)
#define dsPIC33FJ128MC706A (0x00A9)
#define dsPIC33FJ128MC708A (0x00AE)
#define dsPIC33FJ128MC710A (0x00AF)
#define PIC24HJ128GP206A   (0x005D)
#define PIC24HJ128GP210A   (0x005F)
#define PIC24HJ128GP306A   (0x0065)
#define PIC24HJ128GP310A   (0x0067)
#define PIC24HJ128GP506A   (0x0061)
#define PIC24HJ128GP510A   (0x0063)
#define dsPIC33FJ256GP506A (0x07F5)
#define dsPIC33FJ256GP510A (0x07F7)
#define dsPIC33FJ256GP710A (0x07FF)
#define dsPIC33FJ256MC510A (0x07B7)
#define dsPIC33FJ256MC710A (0x07BF)
#define PIC24HJ256GP206A   (0x0771)
#define PIC24HJ256GP210A   (0x0773)
#define PIC24HJ256GP610A   (0x077B)
#define dsPIC33FJ32GS406   (0x4000)
#define dsPIC33FJ64GS406   (0x4001)
#define dsPIC33FJ32GS606   (0x4002)
#define dsPIC33FJ64GS606   (0x4003)
#define dsPIC33FJ32GS608   (0x4004)
#define dsPIC33FJ64GS608   (0x4005)
#define dsPIC33FJ32GS610   (0x4006)
#define dsPIC33FJ64GS610   (0x4007)

/*
 * DS70663D
 */
#define DS70663D (70663)
#define PIC24EP32GP202    (0x1C19)
#define PIC24EP32GP203    (0x1C1A)
#define PIC24EP32GP204    (0x1C18)
#define dsPIC33EP32GP502  (0x1C0D)
#define dsPIC33EP32GP503  (0x1C0E)
#define dsPIC33EP32GP504  (0x1C0C)
#define PIC24EP32MC202    (0x1C11)
#define PIC24EP32MC203    (0x1C12)
#define PIC24EP32MC204    (0x1C10)
#define dsPIC33EP32MC202  (0x1C01)
#define dsPIC33EP32MC203  (0x1C02)
#define dsPIC33EP32MC204  (0x1C00)
#define dsPIC33EP32MC502  (0x1C05)
#define dsPIC33EP32MC503  (0x1C06)
#define dsPIC33EP32MC504  (0x1C04)
#define PIC24EP64GP202    (0x1D39)
#define PIC24EP64GP203    (0x1D3A)
#define PIC24EP64GP204    (0x1D38)
#define PIC24EP64GP206    (0x1D3B)
#define dsPIC33EP64GP502  (0x1D2D)
#define dsPIC33EP64GP503  (0x1D2E)
#define dsPIC33EP64GP504  (0x1D2C)
#define dsPIC33EP64GP506  (0x1D2F)
#define PIC24EP64MC202    (0x1D31)
#define PIC24EP64MC203    (0x1D32)
#define PIC24EP64MC204    (0x1D30)
#define PIC24EP64MC206    (0x1D33)
#define dsPIC33EP64MC202  (0x1D21)
#define dsPIC33EP64MC203  (0x1D22)
#define dsPIC33EP64MC204  (0x1D20)
#define dsPIC33EP64MC206  (0x1D23)
#define dsPIC33EP64MC502  (0x1D25)
#define dsPIC33EP64MC503  (0x1D26)
#define dsPIC33EP64MC504  (0x1D24)
#define dsPIC33EP64MC506  (0x1D27)
#define PIC24EP128GP202   (0x1E59)
#define PIC24EP128GP204   (0x1E58)
#define PIC24EP128GP206   (0x1E5B)
#define dsPIC33EP128GP502 (0x1E4D)
#define dsPIC33EP128GP504 (0x1E4C)
#define dsPIC33EP128GP506 (0x1E4F)
#define PIC24EP128MC202   (0x1E51)
#define PIC24EP128MC204   (0x1E50)
#define PIC24EP128MC206   (0x1E53)
#define dsPIC33EP128MC202 (0x1E41)
#define dsPIC33EP128MC204 (0x1E40)
#define dsPIC33EP128MC206 (0x1E43)
#define dsPIC33EP128MC502 (0x1E45)
#define dsPIC33EP128MC504 (0x1E44)
#define dsPIC33EP128MC506 (0x1E47)
#define PIC24EP256GP202   (0x1F79)
#define PIC24EP256GP204   (0x1F78)
#define PIC24EP256GP206   (0x1F7B)
#define dsPIC33EP256GP502 (0x1F6D)
#define dsPIC33EP256GP504 (0x1F6C)
#define dsPIC33EP256GP506 (0x1F6F)
#define PIC24EP256MC202   (0x1F71)
#define PIC24EP256MC204   (0x1F70)
#define PIC24EP256MC206   (0x1F73)
#define dsPIC33EP256MC202 (0x1F61)
#define dsPIC33EP256MC204 (0x1F60)
#define dsPIC33EP256MC206 (0x1F63)
#define dsPIC33EP256MC502 (0x1F65)
#define dsPIC33EP256MC504 (0x1F64)
#define dsPIC33EP256MC506 (0x1F67)
#define PIC24EP512GP202   (0x1799)
#define PIC24EP512GP204   (0x1798)
#define PIC24EP512GP206   (0x179B)
#define dsPIC33EP512GP502 (0x178D)
#define dsPIC33EP512GP504 (0x178C)
#define dsPIC33EP512GP506 (0x178F)
#define PIC24EP512MC202   (0x1791)
#define PIC24EP512MC204   (0x1790)
#define PIC24EP512MC206   (0x1793)
#define dsPIC33EP512MC202 (0x1781)
#define dsPIC33EP512MC204 (0x1780)
#define dsPIC33EP512MC206 (0x1783)
#define dsPIC33EP512MC502 (0x1785)
#define dsPIC33EP512MC504 (0x1784)
#define dsPIC33EP512MC506 (0x1787)

/*
 * DS80000577P
 */
#define dsPIC33EP128GM304 (0x1B40)
#define dsPIC33EP128GM604 (0x1B48)
#define dsPIC33EP128GM306 (0x1B43)
#define dsPIC33EP128GM706 (0x1B4B)
#define dsPIC33EP128GM310 (0x1B47)
#define dsPIC33EP128GM710 (0x1B4F)
#define dsPIC33EP256GM304 (0x1B80)
#define dsPIC33EP256GM604 (0x1B88)
#define dsPIC33EP256GM306 (0x1B83)
#define dsPIC33EP256GM706 (0x1B8B)
#define dsPIC33EP256GM310 (0x1B87)
#define dsPIC33EP256GM710 (0x1B8F)
#define dsPIC33EP512GM304 (0x1BC0)
#define dsPIC33EP512GM604 (0x1BC8)
#define dsPIC33EP512GM306 (0x1BC3)
#define dsPIC33EP512GM706 (0x1BCB)
#define dsPIC33EP512GM310 (0x1BC7)
#define dsPIC33EP512GM710 (0x1BCF)

/*
 * dsPIC30F SMPS (switch mode power supply) Flash Programming Specification
 *
 * LVP(KEY)
 *
 * DS70284B
 */
#define DS70284B (70284)
#define dsPIC30F1010 (0x0404)
#define dsPIC30F2020 (0x0400)
#define dsPIC30F2023 (0x0403)

/*
 * DS75012B
 */
#define DS75012B (75012)
#define PIC24FJ16MC101 (0x0206)
#define PIC24FJ16MC102 (0x0207)
#define PIC24FJ32MC101 (0x0A0C)
#define PIC24FJ32MC102 (0x0A0D)
#define PIC24FJ32MC104 (0x0A0F)

/*
 * DS39934B / DS30009934C
 */
#define DS39934B (39934)
#define PIC24FJ32GA102 (0x4202)
#define PIC24FJ64GA102 (0x4206)
#define PIC24FJ32GA104 (0x420A)
#define PIC24FJ64GA104 (0x420E)
#define PIC24FJ32GB002 (0x4203)
#define PIC24FJ64GB002 (0x4207)
#define PIC24FJ32GB004 (0x420B)
#define PIC24FJ64GB004 (0x420F)

/*
 * DS30009907C
 */
#define DS30009907C (30009907)
#define PIC24FJ64GA106  (0x1000)
#define PIC24FJ128GA106 (0x1008)
#define PIC24FJ192GA106 (0x1010)
#define PIC24FJ256GA106 (0x1018)
#define PIC24FJ64GA108  (0x1002)
#define PIC24FJ128GA108 (0x100A)
#define PIC24FJ192GA108 (0x1012)
#define PIC24FJ256GA108 (0x101A)
#define PIC24FJ64GA110  (0x1006)
#define PIC24FJ128GA110 (0x100E)
#define PIC24FJ192GA110 (0x1016)
#define PIC24FJ256GA110 (0x101E)
#define PIC24FJ64GB106  (0x1001)
#define PIC24FJ128GB106 (0x1009)
#define PIC24FJ192GB106 (0x1011)
#define PIC24FJ256GB106 (0x1019)
#define PIC24FJ64GB108  (0x1003)
#define PIC24FJ128GB108 (0x100B)
#define PIC24FJ192GB108 (0x1013)
#define PIC24FJ256GB108 (0x101B)
#define PIC24FJ64GB110  (0x1007)
#define PIC24FJ128GB110 (0x100F)
#define PIC24FJ192GB110 (0x1017)
#define PIC24FJ256GB110 (0x101F)

/*
 * DS39970E
 */
#define DS39970E (39970)
#define PIC24FJ128DA106 (0x4109)
#define PIC24FJ256DA106 (0x410D)
#define PIC24FJ128DA110 (0x410B)
#define PIC24FJ256DA110 (0x410F)
#define PIC24FJ128DA206 (0x4108)
#define PIC24FJ256DA206 (0x410C)
#define PIC24FJ128DA210 (0x410A)
#define PIC24FJ256DA210 (0x410E)
#define PIC24FJ64GA306  (0x46C0)
#define PIC24FJ64GA308  (0x46C4)
#define PIC24FJ64GA310  (0x46C8)
#define PIC24FJ128GA306 (0x46C2)
#define PIC24FJ128GA308 (0x46C6)
#define PIC24FJ128GA310 (0x46CA)
#define PIC24FJ128GB206 (0x4100)
#define PIC24FJ256GB206 (0x4104)
#define PIC24FJ128GB210 (0x4102)
#define PIC24FJ256GB210 (0x4106)
#define PIC24FJ64GC006  (0x4888)
#define PIC24FJ64GC008  (0x488A)
#define PIC24FJ64GC010  (0x4884)
#define PIC24FJ128GC006 (0x4889)
#define PIC24FJ128GC008 (0x488B)
#define PIC24FJ128GC010 (0x4885)

/*
 * DS30000510E
 */
#define DS30000510E (30000510)
#define PIC24FJ128GB204 (0x4C5B)
#define PIC24FJ128GB202 (0x4C5A)
#define PIC24FJ64GB204  (0x4C59)
#define PIC24FJ64GB202  (0x4C58)
#define PIC24FJ128GA204 (0x4C53)
#define PIC24FJ128GA202 (0x4C52)
#define PIC24FJ64GA204  (0x4C51)
#define PIC24FJ64GA202  (0x4C50)

/*
 * DS30010073A
 */
#define DS30010073A (30010073)
#define PIC24FJ64GA406  (0x6100)
#define PIC24FJ64GA410  (0x6101)
#define PIC24FJ64GA412  (0x6102)
#define PIC24FJ64GB406  (0x6104)
#define PIC24FJ64GB410  (0x6105)
#define PIC24FJ64GB412  (0x6106)
#define PIC24FJ128GA406 (0x6108)
#define PIC24FJ128GA410 (0x6109)
#define PIC24FJ128GA412 (0x610A)
#define PIC24FJ128GB406 (0x610C)
#define PIC24FJ128GB410 (0x610D)
#define PIC24FJ128GB412 (0x610E)
#define PIC24FJ256GA406 (0x6110)
#define PIC24FJ256GA410 (0x6111)
#define PIC24FJ256GA412 (0x6112)
#define PIC24FJ256GB406 (0x6114)
#define PIC24FJ256GB410 (0x6115)
#define PIC24FJ256GB412 (0x6116)

/*
 * DS30010057F
 */
#define DS30010057F (30010057)
#define PIC24FJ128GA606  (0x6000)
#define PIC24FJ256GA606  (0x6008)
#define PIC24FJ512GA606  (0x6010)
#define PIC24FJ1024GA606 (0x6018)
#define PIC24FJ128GA610  (0x6001)
#define PIC24FJ256GA610  (0x6009)
#define PIC24FJ512GA610  (0x6011)
#define PIC24FJ1024GA610 (0x6019)
#define PIC24FJ128GB606  (0x6004)
#define PIC24FJ256GB606  (0x600C)
#define PIC24FJ512GB606  (0x6014)
#define PIC24FJ1024GB606 (0x601C)
#define PIC24FJ128GB610  (0x6005)
#define PIC24FJ256GB610  (0x600D)
#define PIC24FJ512GB610  (0x6015)
#define PIC24FJ1024GB610 (0x601D)

/*
 * DS70005137F
 */
#define DS70005137F (70005137)
#define dsPIC33EV32GM002  (0x5D01)
#define dsPIC33EV32GM004  (0x5D00)
#define dsPIC33EV32GM006  (0x5D03)
#define dsPIC33EV32GM102  (0x5D09)
#define dsPIC33EV32GM104  (0x5D08)
#define dsPIC33EV32GM106  (0x5D0B)
#define dsPIC33EV64GM002  (0x5D11)
#define dsPIC33EV64GM004  (0x5D10)
#define dsPIC33EV64GM006  (0x5D13)
#define dsPIC33EV64GM102  (0x5D19)
#define dsPIC33EV64GM104  (0x5D18)
#define dsPIC33EV64GM106  (0x5D1B)
#define dsPIC33EV128GM002 (0x5D21)
#define dsPIC33EV128GM004 (0x5D20)
#define dsPIC33EV128GM006 (0x5D23)
#define dsPIC33EV128GM102 (0x5D29)
#define dsPIC33EV128GM104 (0x5D28)
#define dsPIC33EV128GM106 (0x5D2B)
#define dsPIC33EV256GM002 (0x5D31)
#define dsPIC33EV256GM004 (0x5D30)
#define dsPIC33EV256GM006 (0x5D33)
#define dsPIC33EV256GM102 (0x5D39)
#define dsPIC33EV256GM104 (0x5D38)
#define dsPIC33EV256GM106 (0x5D3B)

/*
 * DS30010102A
 */
#define DS30010102A (30010102)
#define PIC24FJ64GA702  (0x7506)
#define PIC24FJ128GA702 (0x750A)
#define PIC24FJ256GA702 (0x750E)
#define PIC24FJ64GA704  (0x7505)
#define PIC24FJ128GA704 (0x7509)
#define PIC24FJ256GA704 (0x750D)
#define PIC24FJ64GA705  (0x7507)
#define PIC24FJ128GA705 (0x750B)
#define PIC24FJ256GA705 (0x750F)

/*
 * DS70005160C
 */
#define DS70005160C (70005160)
#define dsPIC33EP16GS502 (0x4E01)
#define dsPIC33EP16GS504 (0x4E02)
#define dsPIC33EP16GS505 (0x4E02)
#define dsPIC33EP16GS506 (0x4E03)
#define dsPIC33EP32GS502 (0x4E11)
#define dsPIC33EP32GS504 (0x4E12)
#define dsPIC33EP32GS505 (0x4E12)
#define dsPIC33EP32GS506 (0x4E13)
#define dsPIC33EP64GS502 (0x4E21)
#define dsPIC33EP64GS504 (0x4E22)
#define dsPIC33EP64GS505 (0x4E22)
#define dsPIC33EP64GS506 (0x4E23)

/*
 * DS70005192B
 */
#define DS70005192B (70005192)
#define dsPIC33EP16GS202 (0x6D01)
#define dsPIC33EP32GS202 (0x6D11)

/*
 * DS70005256A
 */
#define DS70005256A (70005256)
#define dsPIC33EP64GS708  (0x6C03)
#define dsPIC33EP64GS804  (0x6C40)
#define dsPIC33EP64GS805  (0x6C60)
#define dsPIC33EP64GS806  (0x6C42)
#define dsPIC33EP64GS808  (0x6C43)
#define dsPIC33EP128GS702 (0x6C11)
#define dsPIC33EP128GS704 (0x6C10)
#define dsPIC33EP128GS705 (0x6C30)
#define dsPIC33EP128GS706 (0x6C12)
#define dsPIC33EP128GS708 (0x6C13)
#define dsPIC33EP128GS804 (0x6C50)
#define dsPIC33EP128GS805 (0x6C70)
#define dsPIC33EP128GS806 (0x6C52)
#define dsPIC33EP128GS808 (0x6C53)

/******************************************************************************/

#define _PCL (0x002E)
uint32_t pic24_arch(void);
void pic24_selector(void);
void pic24_bootloader(void);
void pic24_program_verify(void);
void pic24_standby(void);
void pic24_goto200(void);
uint16_t pic24_readreg(uint16_t);
void pic24_set_read_pointer(uint32_t);
void pic24_set_write_pointer(uint32_t);
void pic24_set_write_pointer_nvmadr(uint32_t);
uint32_t pic24_table_read24_post_increment(void);
void pic24_table_read48_post_increment(uint32_t *, uint32_t *);
void pic24_nvmcon_write_completion(uint16_t, uint16_t);
void pic24_nvmcon_write_external(uint32_t);
void pic24_bulk_erase(void);
void pic24_row_erase(uint32_t, uint32_t);
void pic24_erase_row(uint32_t, uint32_t);
int pic24_read_config_memory(void);
uint32_t pic24_get_program_count(void);
uint32_t pic24_get_program_size(uint32_t *, uint32_t);
uint32_t pic24_get_data_size(uint32_t *);
uint32_t pic24_get_executive_size(uint32_t *);
uint32_t pic24_read_program_memory_block(uint32_t *, uint32_t, uint32_t);
uint32_t pic24_read_data_memory_block(uint16_t *, uint32_t, uint16_t);
void pic24_write_program_init(void);
void pic24_write_program(uint32_t, uint32_t *, uint32_t);
void pic24_write_panel(uint32_t, uint32_t, uint32_t *, uint32_t);
void pic24_write_config_init1(void);
void pic24_write_config_init2(void);
void pic24_write_config_word(uint32_t, uint16_t);
uint32_t pic24_write_config_words(void);
uint32_t pic24_write_fuid_words(void);
uint32_t pic24_write_config(void);
void pic24_write_fboot(void);
uint32_t pic24_getregion(uint32_t);
void pic24_program_begin(void);
uint32_t pic24_program_data(uint32_t, pic_data *);
void pic24_program_end(int);
uint32_t pic24_verify_data(uint32_t, pic_data *, uint32_t *);
void pic24_view_data(pic_data *);
void pic24_dumpdeviceid(void);
void pic24_dumpconfig(uint32_t, uint32_t);
void pic24_dumpconfig_verbose(void);
void pic24_dumphexcode(uint32_t, uint32_t, uint32_t *);
void pic24_dumpinhxcode(uint32_t, uint32_t, uint32_t *);
void pic24_dumphexdata(uint32_t, uint32_t, uint16_t *);
void pic24_dumpinhxdata(uint32_t, uint32_t, uint16_t *);
void pic24_dumpdevice(void);

#endif /* !_PIC24_H */
