/*
** includes
*/

#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<proto/dos.h>
#include<proto/exec.h>
#include<proto/icon.h>
#include<proto/commodities.h>
#include<proto/intuition.h>
#include<proto/gadtools.h>
#include<exec/io.h>
#include<exec/memory.h>
#include<exec/interrupts.h>
#include<exec/execbase.h>
#include<exec/devices.h>
#include<dos/rdargs.h>
#include<devices/timer.h>
#include<devices/scsidisk.h>
#include"hdsleep_rev.h"
#include"gadtools.h"

/*
** version
*/

char	ver[]=VERSTAG;

/*
** defines
*/

#define TYPE_RUN 0
#define TYPE_END 1
#define TYPE_SPINUP 2
#define TYPE_SPINDOWN 3
#define TYPE_HOTKEY 4

#define TEMPLATE "CX_PRIORITY/N/K,CX_POPKEY/K,CX_POPUP/K,HDSLEEPKEY=SKEY/K,MASTERTIME=MT/N,SLAVETIME=ST/N,WARNING=WARN/S,WARNINGTIMEOUT=WT/N,LOGFILE=L/K,A4000/S,ELSAT/S"
#define ARG_PRIORITY		0
#define ARG_POPKEY			1
#define ARG_POPUP				2
#define ARG_HDSLEEPKEY	3
#define ARG_MASTERTIME	4
#define ARG_SLAVETIME		5
#define ARG_WARNING			6
#define ARG_WARNINGTO		7
#define ARG_LOGFILE			8
#define ARG_A4000				9
#define ARG_ELSAT				10
#define ARG_COUNT				11

#define OFF 0
#define ON 1
#define STOPPED 2
#define EVT_POPKEY 1
#define EVT_HOTKEY 2
#define DRV_MASTER 0
#define DRV_SLAVE 1

#define	unless(x) if ( !(x) )
#define LEAVE goto exitit

ULONG DEF_PRIORITY=0;
char	DEF_POPKEY[]="control alt h";
char	DEF_POPUP2[]="YES";
ULONG DEF_POPUP=1;
char	DEF_HDSLEEPKEY[]="control h";
ULONG DEF_MASTERTIME=360;	// 360 secs == 4 mins
ULONG DEF_SLAVETIME=0;	  // 360 secs == 4 mins
ULONG DEF_WARNING=0;
ULONG DEF_WARNINGTO=5;
char DEF_LOGFILE[]="\0";
ULONG DEF_A4000=FALSE;
ULONG DEF_ELSAT=FALSE;

/*
** prototypes
*/

void	main(int, char **);
void	__saveds __asm MyBeginIO(register __a1 struct IORequest *);
int		AddInt(void);
void	RemInt(void);
void	HandleCxMsg(void);
void	RequestOK(char *);
APTR	BusyWindow(struct Window *);
void	UnbusyWindow(APTR);
void	LogFile(UBYTE,UBYTE);

extern struct DiskObject *TTGetOurIcon(struct WBStartup *);
extern void TTFreeOurIcon(struct DiskObject *);
extern char *TTString(struct DiskObject *,char *, char *);
extern LONG TTInt(struct DiskObject *,char *, LONG);
extern int TTBool(struct DiskObject *,char *, BOOL);

//extern	void SPrintf(char *,const char *, ...);
extern	void __asm CallBeginIO(register __a1 struct IORequest *);
extern	void __asm HDDriveOff(register __d0 UBYTE);
extern	UBYTE __asm CheckHD(register __d0 UBYTE);

/*
** variables
*/

extern struct ExecBase *SysBase;
APTR OldBeginIO=NULL;
struct DiskObject *icon=NULL;
struct Library *SCSIBase=NULL;
struct MsgPort *TimerPort=NULL;
struct timerequest TimerReq;
struct IntuiMessage *message;
struct Gadget *gadget;
struct MenuItem *item;
BOOL running=TRUE,timerun=FALSE,POPUP=TRUE;//,SpinUp=FALSE;
ULONG MASTERTIME,SLAVETIME,signals,MasterTime=0,SlaveTime=0,class,code;
ULONG SigWin,WARNING=0,WARNINGTO,ComputerA4000=0,ComputerELSAT=0,Computer=0;
UBYTE MasterHD,SlaveHD;
ULONG BeginIOCounter=0;

struct BusyWin
{
	struct Requester Req;
	struct Window *Win;
};

/*
** commodities
*/

struct NewBroker newbroker=
{
	NB_VERSION,
	"HDSleep",
	"HDSleep v1.5 by Tomasz Muszynski",
	"HD noise saver for Amiga Computers",
	NBU_UNIQUE|NBU_NOTIFY,
	COF_SHOW_HIDE
};

struct MsgPort *broker_mp=NULL;
CxObj *broker=NULL, *hdsleepobj, *popkeyobj;
CxMsg *cxmsg;
ULONG msgid, msgtype;
UBYTE hdsleepstr[50],popkeystr[50],logfilestr[108];
struct Unit *MasterUnit=NULL,*SlaveUnit=NULL;

/*
** functions
*/

void main(int argc,char *argv[])
{
	struct RDArgs *rdargs;
	long *(opts[ARG_COUNT]),arg;
	struct Node *SCSINode;
	struct IORequest SCSIIOReq;
	struct MsgPort *SCSIPort;

	if(SysBase->LibNode.lib_Version<39)
	{
		RequestOK("You must have\nAmiga with OS3.0");
		LEAVE;
	}
	MasterHD=FALSE;	// MasterHD=CheckHD(DRV_MASTER);
	SlaveHD=FALSE;	// SlaveHD=CheckHD(DRV_SLAVE);

	if(argc!=0)
	{
		opts[ARG_PRIORITY]=&DEF_PRIORITY;
		opts[ARG_POPKEY]=(long *)DEF_POPKEY;
		opts[ARG_POPUP]=(long *)DEF_POPUP2;
		opts[ARG_HDSLEEPKEY]=(long *)DEF_HDSLEEPKEY;
		opts[ARG_MASTERTIME]=&DEF_MASTERTIME;
		opts[ARG_SLAVETIME]=&DEF_SLAVETIME;
		opts[ARG_WARNING]=(long *)DEF_WARNING;
		opts[ARG_WARNINGTO]=&DEF_WARNINGTO;
		opts[ARG_LOGFILE]=(long *)DEF_LOGFILE;
		opts[ARG_A4000]=(long *)DEF_A4000;
		opts[ARG_ELSAT]=(long *)DEF_ELSAT;

		if(rdargs=ReadArgs(TEMPLATE,(long *) opts, NULL))
		{
			arg=FindArg(TEMPLATE,"CX_PRIORITY");
			if(arg>-1)
				newbroker.nb_Pri=*opts[arg];

			arg=FindArg(TEMPLATE,"CX_POPUP");
			if(arg>-1)
			{
				if(!stricmp((char *)opts[arg],"NO")) POPUP=FALSE;
				if(!stricmp((char *)opts[arg],"FALSE")) POPUP=FALSE;
				if(!stricmp((char *)opts[arg],"OFF")) POPUP=FALSE;
			}

			arg=FindArg(TEMPLATE,"CX_POPKEY");
			if(arg>-1)
				strcpy(popkeystr,(char *)opts[arg]);

			arg=FindArg(TEMPLATE,"HDSLEEPKEY");
			if(arg>-1)
				strcpy(hdsleepstr,(char *)opts[arg]);

			arg=FindArg(TEMPLATE,"MASTERTIME");
			if(arg>-1)
			{
				MASTERTIME=*opts[arg];
				if(MASTERTIME) MasterHD=TRUE;
			}

			arg=FindArg(TEMPLATE,"SLAVETIME");
			if(arg>-1)
			{
				SLAVETIME=*opts[arg];
				if(SLAVETIME) SlaveHD=TRUE;
			}

			arg=FindArg(TEMPLATE,"WARNING");
			if(arg>-1)
				WARNING=(ULONG)opts[arg];

			arg=FindArg(TEMPLATE,"WARNINGTIMEOUT");
			if(arg>-1)
				WARNINGTO=*opts[arg];

			arg=FindArg(TEMPLATE,"LOGFILE");
			if(arg>-1)
				strcpy(logfilestr,(char *)opts[arg]);

			arg=FindArg(TEMPLATE,"A4000");
			if(arg>-1)
				ComputerA4000=(ULONG)opts[arg];

			arg=FindArg(TEMPLATE,"ELSAT");
			if(arg>-1)
				ComputerELSAT=(ULONG)opts[arg];

			FreeArgs(rdargs);
		}
		else
		{
			PrintFault(IoErr(),argv[0]);
			LEAVE;
		}
	}
	else
	{
		unless(icon=TTGetOurIcon(WBenchMsg)) icon=GetDiskObject(argv[0]);
		newbroker.nb_Pri=TTInt(icon,"CX_PRIORITY",DEF_PRIORITY);
		strcpy(popkeystr,TTString(icon,"CX_POPKEY",DEF_POPKEY));
		POPUP=TTBool(icon,"CX_POPUP",DEF_POPUP);
		strcpy(hdsleepstr,TTString(icon,"HDSLEEPKEY",DEF_HDSLEEPKEY));
		MASTERTIME=TTInt(icon,"MASTERTIME",DEF_MASTERTIME);
		if(MASTERTIME) MasterHD=TRUE;
		SLAVETIME=TTInt(icon,"SLAVETIME",DEF_SLAVETIME);
		if(SLAVETIME) SlaveHD=TRUE;
		WARNING=TTBool(icon,"WARNING",DEF_WARNING);
		WARNINGTO=TTInt(icon,"WARNINGTIMEOUT",DEF_WARNINGTO);
		strcpy(logfilestr,TTString(icon,"LOGFILE",DEF_LOGFILE));
		ComputerA4000=(ULONG) TTBool(icon,"A4000",DEF_A4000);
		ComputerELSAT=(ULONG) TTBool(icon,"ELSAT",DEF_ELSAT);
		TTFreeOurIcon(icon);
	}

	if(ComputerA4000) Computer=1L;
	if(ComputerELSAT) Computer=2L;

	if(MASTERTIME==0) MasterHD=FALSE;
	if(SLAVETIME==0) SlaveHD=FALSE;

	MasterTime=MASTERTIME;
	SlaveTime=SLAVETIME;

	unless(broker_mp=CreateMsgPort()) LEAVE;
	newbroker.nb_Port=broker_mp;
	unless(broker=CxBroker(&newbroker,NULL)) LEAVE;
	unless(hdsleepobj=HotKey(hdsleepstr,broker_mp,EVT_HOTKEY)) LEAVE;
	unless(popkeyobj=HotKey(popkeystr,broker_mp,EVT_POPKEY)) LEAVE;
	AttachCxObj(broker,hdsleepobj);
	if(CxObjError(hdsleepobj)) LEAVE;
	AttachCxObj(broker,popkeyobj);
	if(CxObjError(popkeyobj)) LEAVE;
	ActivateCxObj(broker,1L);

// Get scsi.device base address

	unless(SCSINode=FindName(&SysBase->DeviceList,ComputerELSAT?"elsat.device":"scsi.device")) LEAVE;
	if(SCSIPort=CreatePort(NULL,0))
	{
		SCSIIOReq.io_Message.mn_ReplyPort=SCSIPort;
		if(OpenDevice(ComputerELSAT?"elsat.device":"scsi.device",0L, &SCSIIOReq,0L)) { DeletePort(SCSIPort); LEAVE; }
		SCSIBase=&SCSIIOReq.io_Device->dd_Library;
		MasterUnit=SCSIIOReq.io_Unit;
		CloseDevice(&SCSIIOReq);
		if(!OpenDevice(ComputerELSAT?"elsat.device":"scsi.device",1L, &SCSIIOReq,0L))
		{
			SlaveUnit=SCSIIOReq.io_Unit;
			CloseDevice(&SCSIIOReq);
		}
		DeletePort(SCSIPort);
	}
	else LEAVE;

// Replace BeginIO

	Forbid();
	SumLibrary(SCSIBase);
	unless(OldBeginIO=SetFunction(SCSIBase,DEV_BEGINIO,(unsigned long (*)())MyBeginIO)) LEAVE;
	SumLibrary(SCSIBase);
	Permit();

	unless(timerun=AddInt()) LEAVE;

	SigWin=NULL;

	if(POPUP)
	{
		if(!SetupScreen())
		{
			if(!OpenMainWindow())
			{
				SigWin=1L<<MainWnd->UserPort->mp_SigBit;
				GT_SetGadgetAttrs(MainGadgets[GD_MASTEROFF],MainWnd,NULL,GTNM_Number,MasterTime,TAG_DONE);
				GT_SetGadgetAttrs(MainGadgets[GD_SLAVEOFF],MainWnd,NULL,GTNM_Number,SlaveTime,TAG_DONE);
			}
		}
	}

	LogFile(TYPE_RUN,0);

	while(running)
	{
		signals=Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D | (1L<<broker_mp->mp_SigBit) | SigWin | 1L<<TimerPort->mp_SigBit);
		if(signals & (1L<<TimerPort->mp_SigBit))
		{
			if(MainWnd)
			{
				GT_SetGadgetAttrs(MainGadgets[GD_MASTEROFF],MainWnd,NULL,GTNM_Number,MasterTime,TAG_DONE);
				GT_SetGadgetAttrs(MainGadgets[GD_SLAVEOFF],MainWnd,NULL,GTNM_Number,SlaveTime,TAG_DONE);
			}
//			if(SpinUp)
//			{
//				LogFile(TYPE_SPINUP,0);
//				SpinUp=FALSE;
//			}
			if(MasterTime>0 && MasterHD)
			{
				MasterTime--;
				if(MasterTime==0)
				{
					LogFile(TYPE_SPINDOWN,0);
					MasterTime=0;
					while(BeginIOCounter>0) Delay(10);
					if(BeginIOCounter==0) HDDriveOff(DRV_MASTER);
					if(MainWnd)
					{
						CloseMainWindow();
						CloseDownScreen();
						SigWin=NULL;
					}
				}
			}
			if(SlaveTime>0 && SlaveHD)
			{
				SlaveTime--;
				if(SlaveTime==0)
				{
					LogFile(TYPE_SPINDOWN,1);
					SlaveTime=0;
					while(BeginIOCounter>0) Delay(10);
					if(BeginIOCounter==0) HDDriveOff(DRV_SLAVE);
					if(MainWnd)
					{
						CloseMainWindow();
						CloseDownScreen();
						SigWin=NULL;
					}
				}
			}
			TimerReq.tr_time.tv_secs=1;
			TimerReq.tr_time.tv_micro=0;
			SendIO((struct IORequest *) &TimerReq);
			if(WARNING && (SlaveTime==WARNINGTO || MasterTime==WARNINGTO) && !MainWnd)
			{
				if(!SetupScreen())
				{
					if(!OpenMainWindow())
					{
						SigWin=1L<<MainWnd->UserPort->mp_SigBit;
					}
				}
			}
			if(WARNING && (SlaveTime==WARNINGTO || MasterTime==WARNINGTO) && MainWnd) WindowToFront(MainWnd);
		}
		if(signals & SIGBREAKF_CTRL_C) running=FALSE;
		if(signals & SIGBREAKF_CTRL_D)
		{
			LogFile(TYPE_HOTKEY,0);
			MasterTime=0;
			HDDriveOff(DRV_MASTER);
			SlaveTime=0;
			HDDriveOff(DRV_SLAVE);
		}
		if(signals & SigWin)
		{
			while(message=GT_GetIMsg(MainWnd->UserPort))
			{
				class=message->Class;
				code=message->Code;
				gadget=(struct Gadget *)message->IAddress;
				GT_ReplyIMsg(message);
				switch(class)
				{
					case IDCMP_CLOSEWINDOW:
						goto gdhide;
						break;
					case IDCMP_MENUPICK:
						while(code!=MENUNULL) 
						{
							item=(struct MenuItem *)ItemAddress(MainMenus,code);
							switch(ITEMNUM(code))
							{
								case 0: goto gdsleep; break;
								case 1: goto gdabort; break;
								case 3: RequestOK("HDSleep v1.5\n"\
														"     by     \n"\
														" Thom/Union \n"\
														"            \n"\
														"This program\n"\
														"is  FREEWARE");
									break;
								case 5: goto gdhide; break;
								case 6: goto gdquit; break;
							}
							code = item->NextSelect;
						}
						break;
					case IDCMP_VANILLAKEY:
						switch(code)
						{
							case 's':
							case 'S': goto gdsleep; break;
							case 'a':
							case 'A': goto gdabort; break;
							case 'q':
							case 'Q': goto gdquit; break;
						}
						break;
					case IDCMP_GADGETUP:
						switch(gadget->GadgetID)
						{
							case GD_SLEEP:
gdsleep:				while(message=(struct IntuiMessage *)GetMsg(MainWnd->UserPort)) ReplyMsg((struct Message *)message);
								CloseMainWindow();
								CloseDownScreen();
								SigWin=NULL;
								LogFile(TYPE_HOTKEY,0);
								MasterTime=0;
								HDDriveOff(DRV_MASTER);
								SlaveTime=0;
								HDDriveOff(DRV_SLAVE);
								goto skip;
								break;
							case GD_ABORT:
gdabort:				MasterTime=MASTERTIME;
								SlaveTime=SLAVETIME;
gdhide:					while(message=(struct IntuiMessage *)GetMsg(MainWnd->UserPort)) ReplyMsg((struct Message *)message);
								CloseMainWindow();
								CloseDownScreen();
								SigWin=NULL;
								goto skip;
								break;
							case GD_QUIT:
gdquit:					running=FALSE;
								break;
						}
						break;
				}
			}
		}
		if(signals & (1L<<broker_mp->mp_SigBit)) HandleCxMsg();
skip:;
	}

exitit:
	LogFile(TYPE_END,0);

	if(MainWnd)
	{
		CloseMainWindow();
		CloseDownScreen();
		SigWin=NULL;
	}
	RemInt();

	Forbid();
	SumLibrary(SCSIBase);
	if(OldBeginIO) SetFunction(SCSIBase,DEV_BEGINIO,(unsigned long (*)())OldBeginIO);
	SumLibrary(SCSIBase);
	Permit();

	if(broker) DeleteCxObjAll(broker);
	if(broker_mp) DeleteMsgPort(broker_mp);
}

void __saveds __asm MyBeginIO(register __a1 struct IORequest *io)
{
	struct SCSICmd *scsi;

	BeginIOCounter++;
	switch(io->io_Command)
	{
		case CMD_START:
			break;
		case CMD_STOP:
			break;
		case HD_SCSICMD:
			scsi=(struct SCSICmd *)((struct IOStdReq *)io)->io_Data;
			if(scsi->scsi_Flags==1) break;
		default:
			if(io->io_Unit==MasterUnit) MasterTime=MASTERTIME;
			if(io->io_Unit==SlaveUnit) SlaveTime=SLAVETIME;
			break;
	}	
	CallBeginIO(io);
	BeginIOCounter--;
//	SpinUp=TRUE;
}

int AddInt()
{
	if(TimerPort=CreatePort(NULL,0L))
	{
		if(!OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)&TimerReq,0))
		{
			MasterTime=MASTERTIME;
			SlaveTime=SLAVETIME;
			TimerReq.tr_node.io_Message.mn_ReplyPort=TimerPort;
			TimerReq.tr_node.io_Command=TR_ADDREQUEST;
			TimerReq.tr_time.tv_secs=1;
			TimerReq.tr_time.tv_micro=0;
			TimerReq.tr_node.io_Error=0;
			TimerReq.tr_node.io_Flags=0;
			SendIO((struct IORequest *)&TimerReq);
			return 1;
		}
	}
	return 0;
}

void RemInt()
{
	if(timerun) CloseDevice((struct IORequest *)&TimerReq);
	if(TimerPort) DeletePort(TimerPort);
}

void HandleCxMsg()
{
	while(cxmsg=(CxMsg *)GetMsg(broker_mp))
	{
		msgid  = CxMsgID(cxmsg);
		msgtype= CxMsgType(cxmsg);
		ReplyMsg((struct Message *)cxmsg);
		switch(msgtype)
		{
			case CXM_IEVENT:
				switch(msgid)
				{
					case EVT_HOTKEY:
						LogFile(TYPE_HOTKEY,0);
						MasterTime=0;
						HDDriveOff(DRV_MASTER);
						SlaveTime=0;
						HDDriveOff(DRV_SLAVE);
						if(MainWnd)
						{
							CloseMainWindow();
							CloseDownScreen();
							SigWin=NULL;
						}
						break;
					case EVT_POPKEY:
						if(!MainWnd)
						{
							if(!SetupScreen())
							{
								if(!OpenMainWindow())
								{
									SigWin=1L<<MainWnd->UserPort->mp_SigBit;
									GT_SetGadgetAttrs(MainGadgets[GD_MASTEROFF],MainWnd,NULL,GTNM_Number,MasterTime,TAG_DONE);
									GT_SetGadgetAttrs(MainGadgets[GD_SLAVEOFF],MainWnd,NULL,GTNM_Number,SlaveTime,TAG_DONE);
								}
							}
						}
						else
						{
							WindowToFront(MainWnd);
							ScreenToFront(Scr);
						}
						break;
				}
				break;
			case CXM_COMMAND:
				switch(msgid)
				{
					case CXCMD_DISABLE:
						ActivateCxObj(broker,0L);
						break;
					case CXCMD_ENABLE:
						ActivateCxObj(broker,1L);
						break;
					case CXCMD_APPEAR:
						if(!MainWnd)
						{
							if(!SetupScreen())
							{
								if(!OpenMainWindow())
								{
									SigWin=1L<<MainWnd->UserPort->mp_SigBit;
									GT_SetGadgetAttrs(MainGadgets[GD_MASTEROFF],MainWnd,NULL,GTNM_Number,MasterTime,TAG_DONE);
									GT_SetGadgetAttrs(MainGadgets[GD_SLAVEOFF],MainWnd,NULL,GTNM_Number,SlaveTime,TAG_DONE);
								}
							}
						}
						else
						{
							WindowToFront(MainWnd);
							ScreenToFront(Scr);
						}
						break;
					case CXCMD_DISAPPEAR:
						if(MainWnd)
						{
							CloseMainWindow();
							CloseDownScreen();
							SigWin=NULL;
						}
						break;
					case CXCMD_KILL:
					case CXCMD_UNIQUE:
						running=FALSE;
						break;
				}
				break;
		}
	}
}

void RequestOK(char *napis)
{
	struct EasyStruct easyreq;
	APTR bw=NULL;

	easyreq.es_StructSize=sizeof(struct EasyStruct);
	easyreq.es_Flags=0;
	easyreq.es_Title="About";
	easyreq.es_TextFormat=napis;
	easyreq.es_GadgetFormat="Ok";

	if(MainWnd) bw=BusyWindow(MainWnd);
	EasyRequest(MainWnd,&easyreq,NULL,NULL);
	if(bw) UnbusyWindow(bw);
}

APTR BusyWindow(struct Window *Win)
{
	struct BusyWin *bw=NULL;

	if(Win)
	{
		if(bw=AllocVec(sizeof(struct BusyWin),MEMF_ANY))
		{
			bw->Win=Win;
			InitRequester(&bw->Req);
			Request(&bw->Req,bw->Win);
			SetWindowPointer(bw->Win,WA_BusyPointer,TRUE,TAG_DONE);
		}
	}
	return((APTR) bw);
}

void UnbusyWindow(APTR bwp)
{
	struct BusyWin *bw;

	if(bwp)
	{
		bw=(struct BusyWin *)bwp;
		EndRequest(&bw->Req,bw->Win);
		SetWindowPointer(bw->Win,WA_Pointer,NULL,TAG_DONE);
		FreeVec(bwp);
	}
}

void LogFile(UBYTE type,UBYTE hd)
{
	FILE *fh;
	char texttime[100];
	unsigned char clock[8];

	if(logfilestr[0])
	{
		if(fh=fopen(logfilestr,"a"))
		{
			getclk(clock);
			sprintf(texttime,"%02ld-%02ld-%02ld %02ld:%02ld:%02ld",(ULONG)clock[3],(ULONG)clock[2],(ULONG)clock[1]+80,(ULONG)clock[4],(ULONG)clock[5],(ULONG)clock[6]);
			switch(type)
			{
				case TYPE_RUN:
					fprintf(fh,"%s  HDSleep started.\n",texttime);
					break;
				case TYPE_END:
					fprintf(fh,"%s  HDSleep exited.\n\n*********************\n\n",texttime);
					break;
//			case TYPE_SPINUP:
//				fprintf(fh,"%s  HD Spinup.\n",texttime);
//				break;
				case TYPE_SPINDOWN:
					fprintf(fh,"%s  %s HD spindown after %ld seconds inactivity.\n",texttime,hd?"Slave":"Master",hd?SLAVETIME:MASTERTIME);
					break;
				case TYPE_HOTKEY:
					fprintf(fh,"%s  All HDs have been spindown by the user.\n",texttime);
					break;
			}
			fclose(fh);
		}
		switch(type)
		{
			case TYPE_SPINDOWN:
			case TYPE_HOTKEY:
				Delay(3*50);		// Delay to write log
				break;
		}
	}
}
