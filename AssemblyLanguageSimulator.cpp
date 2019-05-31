#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include <windows.h>
#include<string.h>
#include<algorithm>
#include<vector>
#include<stdio.h>
using namespace std;
COORD coord={0,0};
void gotoxy(int x,int y)
 {
   coord.X=x;
 coord.Y=y;
 SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
 }

//Global variables
//registers
int A=00,B=00,C=00,D=00;
int demo;
//flags
int CF=0,ZF=0;
//memory space
int memory[65536]={0};
//program counter
int pc=0;
//error flag
int error=0;
//stepbystep
int stepbystep=0;
//Control Signals
int Rs=0,Rd=0;  //Decimal equivalent of Register code
int *R1=&A,*R2=&A;  //Source and destination values
int immData=0;    //Immediate Data
int memIndex=0;    //Memory Index for Direct 16 bit Address Mode
int controlsignal=0; //Decimal equivalent of 4bit control signal

//Utility Function - Show integer in 16 bit hexadecimal form
char* hexsixteenbit(int n)
{
    char *arr;
    arr=new char[5];
    *(arr+0)='0';
    *(arr+1)='0';
    *(arr+2)='0';
    *(arr+3)='0';
    *(arr+4)='\0';
    int temp,i=3;
    while(n>0)
    {   temp=n%16;
        n=n/16;
        if(temp<10)
        {   *(arr+i) = temp + 48;
            i--;
        }
        else
        {   *(arr+i) = temp + 55;
            i--;
        }
    }
    return arr;
}

//Utility Function - Show integer in 8 bit hexadecimal form
char* hexeightbit(int n)
{
    char *arr;
    arr=new char[3];
    *(arr+0)='0';
    *(arr+1)='0';
    *(arr+2)='\0';
    int temp,i=1;
    while(n>0)
    {   temp=n%16;
        n=n/16;
        if(temp<10)
        {   *(arr+i) = temp + 48;
            i--;
        }
        else
        {   *(arr+i) = temp + 55;
            i--;
        }
    }
    return arr;
}

//display the contents of all the registers flags and memory locations
void displaySimulator()
{   cout<<"\n\n\t*******************************************************************\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*\t\t\tRegisters Content\t\t\t  *\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*\tA - "<<hexeightbit(A)<<"H";
    cout<<"\t\tB - "<<hexeightbit(B)<<"H";
    cout<<"\t\tC - "<<hexeightbit(C)<<"H";
    cout<<"\t\tD - "<<hexeightbit(D)<<"H   *";
    cout<<"\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*\t\t\t  Flags Content\t\t\t\t  *\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*\t\t  CF - "<<hexeightbit(CF);
    cout<<"\t\tZF - "<<hexeightbit(ZF)<<"\t\t\t  *";
    cout<<"\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*\t\t\tMemory Content\t\t\t\t  *\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*\t\t  Program Counter - "<<hexsixteenbit(pc)<<"H"<<"\t\t\t  *";
    for(int i=0;i<65536;i++)
    {
        if(memory[i]!=0)
            cout<<"\n\t*\t\t\t["<<hexsixteenbit(i)<<"H]"<<" - "<<hexeightbit(memory[i])<<"H"<<"\t\t\t\t  *";
    }
    cout<<"\n\t*\t\t\t\t\t\t\t\t  *";
    cout<<"\n\t*******************************************************************\n";
    if(stepbystep==0)
      system("pause");
}
void checkRegister(char* inst,int refer,int val,vector<int> &opcode)
{
    switch(*(inst+refer))
    {
        case 'A' :  opcode[val]=0;
                    opcode[val+1]=0;
                    break;
        case 'B' :  opcode[val]=0;
                    opcode[val+1]=1;
                    break;
        case 'C' :  opcode[val]=1;
                    opcode[val+1]=0;
                    break;
        case 'D' :  opcode[val]=1;
                    opcode[val+1]=1;
                    break;
        default  :  error=1;
     }
}
void setRsRd(char* inst,int refer,int &p)
{
    switch(*(inst+refer))
    {
        case 'A' :  p=0;
                    break;
        case 'B' :  p=1;
                    break;
        case 'C' :  p=2;
                    break;
        case 'D' :  p=3;
                    break;
        default  :  p=0;
     }
}
void getimmdata(char *inst,int refer)
{
    char t1=*(inst+refer);
    char t2=*(inst+1+refer);
    int msb,lsb;
    if(t1<='9' && t1>='0')
      msb=t1-'0';
    else if(t1<='F' && t1>='A')
      msb=t1-55;
    else
        error=1;

    if(t2<='9' && t2>='0')
      lsb=t2-'0';
    else if(t2<='F' && t2>='A')
     lsb=t2-55;
    else
       error=1;
    if(error!=1)
    immData=msb*16 + lsb;

}
void get8bit(char* inst,int refer,int val,vector<int> &opcode)
{
    char t1=*(inst+refer);
    char t2=*(inst+1+refer);
    int msb,lsb;
    if(t1<='9' && t1>='0')
      msb=t1-'0';
    else if(t1<='F' && t1>='A')
      msb=t1-55;
    else
        error=1;

    if(t2<='9' && t2>='0')
      lsb=t2-'0';
    else if(t2<='F' && t2>='A')
     lsb=t2-55;
    else
       error=1;

    if(error !=1)
    {
    opcode[val]=0;
    opcode[val+1]=0;
    opcode[val+2]=0;
    opcode[val+3]=0;
    int temp,i=val+3;
    while(msb>0)
    {   temp=msb%2;
        msb=msb/2;
        opcode[i]=temp;
        i--;
    }
    val=val+4;
    opcode[val]=0;
    opcode[val+1]=0;
    opcode[val+2]=0;
    opcode[val+3]=0;
    temp,i=val+3;
    while(lsb>0)
    {   temp=lsb%2;
        lsb=lsb/2;
        opcode[i]=temp;
        i--;
    }
    }


}
void checkInst(char fun[],vector<int> &opcode)
{
    if(strcmp(fun,"MOV")==0 || strcmp(fun,"MVI")==0 || strcmp(fun,"INC")==0 || strcmp(fun,"LDA")==0 || strcmp(fun,"HLT")==0)
     {
         opcode[3]=0;
         opcode[4]=0;
         opcode[5]=0;

     }
     else if(strcmp(fun,"ADD")==0 || strcmp(fun,"ADI")==0 || strcmp(fun,"DEC")==0 || strcmp(fun,"STA")==0 )
     {
         opcode[3]=0;
         opcode[4]=0;
         opcode[5]=1;
         controlsignal+=1;
     }
     else if(strcmp(fun,"SUB")==0 || strcmp(fun,"SBI")==0 )
     {
         opcode[3]=0;
         opcode[4]=1;
         opcode[5]=0;
         controlsignal+=2;
     }
     else if(strcmp(fun,"MUL")==0 || strcmp(fun,"MLI")==0 )
     {
         opcode[3]=0;
         opcode[4]=1;
         opcode[5]=1;
         controlsignal+=3;
     }
     else if(strcmp(fun,"ORR")==0 || strcmp(fun,"ORI")==0 )
     {
         opcode[3]=1;
         opcode[4]=0;
         opcode[5]=0;
         controlsignal+=4;
     }
     else if(strcmp(fun,"AND")==0 || strcmp(fun,"ANI")==0)
     {
         opcode[3]=1;
         opcode[4]=0;
         opcode[5]=1;
         controlsignal+=5;
     }
     else
        error=1;
}

void execute(char *inst,vector<int> &opcode)
{   for(char* c=inst; *c=toupper(*c); ++c) ;
    char fun[3];
    int length=0;
    int i;
    for(i=0;i<3;i++)
        fun[i]= *(inst+i);
        fun[i]='\0';
    if( strcmp(fun,"HLT")==0 )  //0 operand
    {   opcode.resize(6);
        opcode[0]=0;
        opcode[1]=0;
        opcode[2]=0;
        checkInst(fun,opcode);
        stepbystep=0;
        length=6;
        if( *(inst+3)!= '\0' || error==1)
           {
               error=1;
               return;
           }

          displaySimulator();


    }
    else if( strcmp(fun,"INC")==0 || strcmp(fun,"DEC")==0) //1 operand
    {   opcode.resize(8);
        opcode[0]=0;
        opcode[1]=0;
        opcode[2]=1;
        checkInst(fun,opcode);
        length=8;
        checkRegister(inst,i,6,opcode);
        if( *(inst+4)!= '\0' || error==1)
           {  error=1;
              return;
           }
        controlsignal+=12;
        setRsRd(inst,i,Rd);
        switch(Rd)
        {
            case 0: R1=&A;
                    break;
            case 1: R1=&B;
                    break;
            case 2: R1=&C;
                    break;
            case 3: R1=&D;
                    break;
        }
        switch(controlsignal)
        {
            case 12: *R1 = *R1 + 1;
                     if( *R1 >255){ *R1 =0;CF=1;ZF=1;}
                     else{CF=0;ZF=0;}
                     break;
            case 13:  *R1 = *R1 - 1;
                     if(*R1==0){CF=0;ZF=1;}
                     else if (*R1 < 0){ *R1=255;CF=0;ZF=0;}
                     else { CF=0; ZF=0;}
                     break;
        }
        if(stepbystep==1)
        {   cout<<"\n\tInstruction Type : ";
           for(int j=0;j<3;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tInstruction Opcode : ";
           for(int j=3;j<6;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tDestination Register Code : ";
           for(int j=6;j<8;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tControl ROM Content : "<<hexeightbit(controlsignal)<<", Rs = "<<Rs<<", Rd = "<<Rd<<", imm data = "<<immData<<", Mem Index = "<<memIndex;
           displaySimulator();

        }

    }
    else if( strcmp(fun,"MOV")==0 || strcmp(fun,"ADD")==0 || strcmp(fun,"SUB")==0 || strcmp(fun,"MUL")==0 ||strcmp(fun,"ORR")==0 || strcmp(fun,"AND")==0 ) //2 operand
    {  opcode.resize(10);
        opcode[0]=0;
        opcode[1]=1;
        opcode[2]=0;
        checkInst(fun,opcode);
        checkRegister(inst,i,6,opcode);
        checkRegister(inst,i+1,8,opcode);
        length=10;
        if( *(inst+5)!= '\0' || error==1)
           {
            error=1;
            return;
           }
        setRsRd(inst,i,Rd);
        setRsRd(inst,i+1,Rs);
        switch(Rd)
        {
            case 0: R1=&A;
                    break;
            case 1: R1=&B;
                    break;
            case 2: R1=&C;
                    break;
            case 3: R1=&D;
                    break;
        }
        switch(Rs)
        {
            case 0: R2=&A;
                    break;
            case 1: R2=&B;
                    break;
            case 2: R2=&C;
                    break;
            case 3: R2=&D;
                    break;
        }
        switch(controlsignal)
        {
            case 0: *R1 = *R2;
                    if(*R1 == 0){CF=0;ZF=1;}
                    else{CF=0; ZF=0;}
                    break;
            case 1: demo= *R2;
                    for(int i=0;i< demo ; i++)
                    {
                      *R1 = *R1 + 1;
                      if( *R1 >255){ *R1 =0;CF=1;}
                    }
                    if(*R1 == 0){ZF=1;}
                    else{ZF=0;}
                    break;
            case 2:  demo=*R2;
                    for(int i=0;i< demo; i++)
                    {
                      *R1 = *R1 - 1;
                     if(*R1==0){CF=0;ZF=1;}
                     else if (*R1 < 0){ *R1=255;CF=0;ZF=0;}
                     else { CF=0; ZF=0;}
                    }
                    break;
            case 3: demo=*R2;
                    *R1 = (*R1)*(demo);
                    if(*R1==0){ZF=1; CF=0;}
                    else if(*R1 > 255 ){ZF=0;CF=1;}
                    break;
            case 4: demo=*R2;
                    *R1 = *R1 | demo;
                    if(*R1==0){ZF=1; CF=0;}
                    break;
            case 5: demo=*R2;
                    *R1 = *R1 & demo;
                    if(*R1==0){ZF=1; CF=0;}
                    break;

        }
        if(stepbystep==1)
        {   cout<<"\n\tInstruction Type : ";
           for(int j=0;j<3;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tInstruction Opcode : ";
           for(int j=3;j<6;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tDestination Register Code : ";
           for(int j=6;j<8;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tSource Register Code : ";
           for(int j=8;j<10;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tControl ROM Content : "<<hexeightbit(controlsignal)<<", Rs = "<<Rs<<", Rd = "<<Rd<<", imm data = "<<immData<<", Mem Index = "<<memIndex;
           displaySimulator();

        }

    }
    else if( strcmp(fun,"MVI")==0 || strcmp(fun,"ADI")==0 || strcmp(fun,"SBI")==0 || strcmp(fun,"MLI")==0 || strcmp(fun,"ORI")==0 || strcmp(fun,"ANI")==0 || strcmp(fun,"MVI")==0)  //immediate data
    {   opcode.resize(16);
        opcode[0]=0;
        opcode[1]=1;
        opcode[2]=1;
        checkInst(fun,opcode);
        length=16;
        checkRegister(inst,i,6,opcode);
        get8bit(inst,i+1,8,opcode);
        if( *(inst+6)!= '\0' || error==1)
          {
            error=1;
            return;
          }
         controlsignal+=6;
         setRsRd(inst,i,Rd);
         getimmdata(inst,i+1);
        switch(Rd)
        {
            case 0: R1=&A;
                    break;
            case 1: R1=&B;
                    break;
            case 2: R1=&C;
                    break;
            case 3: R1=&D;
                    break;
        }
        switch(controlsignal)
        {
            case 6: *R1 = immData;
                    if(*R1 == 0){CF=0;ZF=1;}
                    else{CF=0; ZF=0;}
                    break;
            case 7: for(int i=0;i< immData ; i++)
                    {
                      *R1 = *R1 + 1;
                      if( *R1 >255){ *R1 =0;CF=1;}
                    }
                    if(*R1 == 0){ZF=1;}
                    else{ZF=0;}
                    break;
            case 8: for(int i=0;i< immData ; i++)
                    {
                      *R1 = *R1 - 1;
                     if(*R1==0){CF=0;ZF=1;}
                     else if (*R1 < 0){ *R1=255;CF=0;ZF=0;}
                     else { CF=0; ZF=0;}
                    }
                    break;
            case 9: *R1 = (*R1)*(immData);
                    if(*R1==0){ZF=1; CF=0;}
                    else if(*R1 > 255 ){ZF=0;CF=1;}
                    break;
            case 10: *R1 = *R1 | immData;
                    if(*R1==0){ZF=1; CF=0;}
                    break;
            case 11: *R1 = *R1 & immData;
                    if(*R1==0){ZF=1; CF=0;}
                    break;

        }
         if(stepbystep==1)
         {   cout<<"\n\tInstruction Type : ";
           for(int j=0;j<3;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tInstruction Opcode : ";
           for(int j=3;j<6;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tDestination Register Code : ";
           for(int j=6;j<8;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tImmediate 8 Bit Data : ";
           for(int j=8;j<16;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tControl ROM Content : "<<hexeightbit(controlsignal)<<", Rs = "<<Rs<<", Rd = "<<Rd<<", imm data = "<<immData<<", Mem Index = "<<memIndex;
           displaySimulator();

        }

    }
    else if( strcmp(fun,"LDA")==0 || strcmp(fun,"STA")==0)  //immediate address
    {   opcode.resize(22);
        opcode[0]=1;
        opcode[1]=0;
        opcode[2]=0;
        length=22;
        checkInst(fun,opcode);
        Rs=Rd=0;
        get8bit(inst,i,6,opcode);
        get8bit(inst,i+2,14,opcode);

        if( *(inst+7)!= '\0' || error==1)
          {
              error=1;
              return;
          }
         controlsignal+=14;
         getimmdata(inst,i);
         memIndex=immData*256;
         getimmdata(inst,i+2);
         memIndex=memIndex + immData;
         immData=0;
         R1=&A;
         switch(controlsignal)
         {
             case 14: *R1 = memory[memIndex];
                       if(*R1==0){ZF=1;CF=0;}
                       else{ZF=0;CF=0;}
                       break;
             case 15: memory[memIndex]= *R1;
                        break;
         }
           if(stepbystep==1)
           {   cout<<"\n\tInstruction Type : ";
           for(int j=0;j<3;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tInstruction Opcode : ";
           for(int j=3;j<6;j++)
           {
            cout<< opcode[j];
           }

           cout<<"\n\tDirect 16 Bit Address : ";
           for(int j=8;j<22;j++)
           {
            cout<< opcode[j];
           }
           cout<<"\n\tControl ROM Content : "<<hexeightbit(controlsignal)<<", Rs = "<<Rs<<", Rd = "<<Rd<<", imm data = "<<immData<<", Mem Index = "<<memIndex;
           displaySimulator();

        }

    }
    else
       {
         error=1;
         return;
       }

    for(int j=0;j<length;j++)
    {
     opcode[j]=opcode[j];
    }
}

void manual()
{   system("cls");
    gotoxy(45,0);
    cout<<"INSTRUCTION MANUAL";

    gotoxy(10,2);
    cout<<"*******************************************************************************************";
    gotoxy(10,3);
    cout<<"    DATA TRANSFER INSTRUCTIOS        ARITHMETIC INSTRUCTIONS        LOGICAL INSTRUCTIONS   ";
    gotoxy(10,4);
    cout<<" ----------------------------------------------------------------------------------------- ";
    gotoxy(10,5);
    cout<<"           MOV A B;                       ADD B C;                        ORR A B;         ";
    gotoxy(10,7);
    cout<<"           MVI A 45;                      ADI B C;                        ORI A 45;        ";
    gotoxy(10,9);
    cout<<"           LDA 2000;                      SUB B C;                        AND A B;         ";
    gotoxy(10,11);
    cout<<"           STA 2000;                      SBI B 45;                       ANI A 45;        ";
    gotoxy(10,13);
    cout<<"                                          MUL A B;                                         ";
    gotoxy(10,15);
    cout<<"                                          MLI A 45;                                        ";
    gotoxy(10,17);
    cout<<"                                          INC B;                                           ";
    gotoxy(10,19);
    cout<<"                                          DEC B;                                           ";
    gotoxy(10,20);
    cout<<"*******************************************************************************************";

    for(int i=2;i<20;i++)
    {
        gotoxy(10,i);
        cout<<"*";
        gotoxy(100,i);
        cout<<"*";
    }
    for(int i=3;i<20;i++)
    {
        gotoxy(43,i);
        cout<<"|";
        gotoxy(73,i);
        cout<<"|";
    }
    gotoxy(0,22);
    cout<<"          HLT - To Terminate the Program !";
    gotoxy(0,24);
    cout<<"         *Immediate Data is in Hexa Decimal Form.\n\n";
    system("pause");
    return;
}

int main()
{
  int choice=1;  //User Choice
  while(choice !=0)
  {
    system("cls");
    gotoxy(27,5);
    cout<<"*************************************************************";
    gotoxy(27,6);
    cout<<"              ---<  BASIC COMPUTER SIMULATOR >---            ";
    gotoxy(27,8);
    cout<<"                          DESCRIPTION                        ";
    gotoxy(27,10);
    cout<<"         It is a 8 bit microprocessor that can perform       ";
    gotoxy(27,11);
    cout<<"           Basic Arithmetic and Logical Instructions         ";
    gotoxy(27,12);
    cout<<"                 and access a memory of 64 KB.               ";
    gotoxy(27,14);
    cout<<"             (1) Read Manual                                 ";
    gotoxy(27,16);
    cout<<"             (2) Run Step by Step                            ";
    gotoxy(27,18);
    cout<<"             (3) Run All at a Time                           ";
    gotoxy(27,20);
    cout<<"             (4) EXIT                                        ";
    gotoxy(27,22);
    cout<<"             Please Enter Your Choice Here                   ";
    gotoxy(27,24);
    cout<<"*************************************************************";
    for(int i=6;i<24;i++)
    {
        gotoxy(27,i);
        cout<<"*";
        gotoxy(87,i);
        cout<<"*";
    }
    gotoxy(70,22);
    cin>>choice;
    pc=0;    //Initialize program Counter to 0
    for(int i=0;i<65536;i++)
        memory[i]=0;
    A=0;
    B=0;
    C=0;
    D=0;
    ZF=0;
    CF=0;
    switch(choice)
    {
        case 1:  manual();
                 continue;
                 break;
        case 2:  stepbystep=1;
                 break;
        case 3:  stepbystep=0;
                 break;
        case 4:   return 0;
        default:  continue;
    }
    system("cls");
    int i=0,opclength;
    char* inst;
    vector<int> opcode;
    inst=new char[10];  //instruction
    for(i=0;i<10;i++)
    {
        *(inst+i)=' ';
    }
    *(inst+10)='\0';
    i=0;
     cout<<"\n\t\t\t--<  Your Code Goes Here >--\n\n";
     //INPUT CODE
     while(1)
     {  i=0;
        error=0;
        cout<<"["<<hexsixteenbit(pc)<<"] >> ";
        pc=pc+1;
        while(1)
        {    cin>> *(inst+i);
             if( *(inst+i) == ';')
                break;

             i++;

        }
        *(inst+i)='\0';

        execute(inst,opcode);
        opclength=opcode.size();
        if(error==1)
        {   cout<<"\n\tInvalid Instruction Format !! Refer To the Manual !!\n";
            pc--;
        }
        cout<<"\n";
        //reset opcode as empty and reset other parameters -> Clear Buses
        opcode.resize(0);
        controlsignal=0;
        Rs=0;
        Rd=0;
        immData=0;
        memIndex=0;
        //Halt Instructions
        if(strcmp(inst,"hlt")==0 || strcmp(inst,"HLT")==0)
           break;
        //Reset Instructions
        for(int j=0;j<10;j++)
        {
         *(inst+j)=' ';
        }
     }

    }
}
