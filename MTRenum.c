/**********************************************************************
**
** Program:   MTRenum.cpp
**
** Purpose:  Reads data from a file and displays it.
**
** Author: L. Johnson,
** Created: 02/19/23
** Current Version: 1.0
***********************************************************************
**  Revision   Date       Engineer       Description of Change
**  --------   --------   ------------   ------------------------------
**  1.0        19 Feb 23  L. Johnson     Initial Release (date started)
**  1.1        24 Mar 23  L. Johnson     Remove spaces & convert to Uppercase
**  1.2        26 Mar 23  L. Johnson     Added RESTORE keyword, Correct
**                                       comma recognition, treat DATA
**                                       statements like remarks                                     
**********************************************************************/
#define TRUE  1
#define FALSE 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
//using namespace std;
void cont();
int main(int argc, char** argv) {


FILE  *fi, *fo;
  int i,j;                         /* Loop index */
  int done=0;
  char fi_name[256];      // Input file name
  char fo_name[256];      // Output file name
  char str_in[256];       // Input String
  char wrk_str[256];      // Working Sttring (Input string less spaces)
  char ln_str[256];       // Line  string
  char tmpstr[256];       // Temporary string
  char dbstr[256];        // Debug string...
  int numq;               // Number of double quotes in a line
  int qi;                 // Quote index (increments on end quotes)
  int qidxb[256];          // Quote begins
  int qidxe[256];		// Quote ends
  int qfnd;		// Quote Found flag
  int scrOut=1;   // Screen output is on...
  int los;  // Length of string
  int ns;   // number of items scanned
  int nsln=1000;  // New Starting Line Number
  int nlinc=10;   // New line increment
  int ln[5000]; // Current Line number
  int nln[5000];  // New Line nUmbers
  int li=0;   // line increment (zero indixed0
  int total_lines=0;
  int remF;   // Remark Flag...Set to TRUE if a remark found otherwise FALSE
  int delSp=TRUE;   // Delete space flag set to FALSE


    // Keyword Parameters
  int srchType;   // 0: inquotes don't search, 1: Search for keywordsk
                  // 2:get line numbers
  int p2i;        // Pass 2 char increment
  int qreg;       // Quote Region - True when between quotes otherwise false
  int kwfnd;	// Number of keywords found in a line
  int kib[256];   // Key index begin
  int kie[256];   // key  index ends


    // Line Number Search Parameters
  int lnfnd;   // Number of line numbers found in the line
  int lnb[256];     // Line number begin index
  int lne[256];     // Line number end index
  int lccnt=0;   // Line character count...
  int lnv[256];  // Line number value
  int nlnv;      // New Line Number Value
  int rlnv;     // Referenced line number value

  printf("MTRenum - Version 1.2\n");


  printf("Enter input file name: ");
  scanf("%s",fi_name);

  fi = fopen(fi_name,"rb");
  if (fi==NULL) {
    printf("Unable to open %s for input.\n",fi_name);
    cont();
	exit(1);
  }


  printf("Enter output file name: ");
  scanf("%s",fo_name);
  fo = fopen(fo_name,"w");
  if (fo==NULL) {
    printf("Unable to open %s for output.\n",fo_name);
    cont();
	exit(1);
  }

  printf("Enter new starting line number: ");
  scanf("%d",&nsln);

  printf("Enter line number increment: ");
  scanf("%d",&nlinc);

  done=0;
  while (done==0) {
    printf("Delete unnecessary spaces (Y/N)? ");
    scanf("%s",tmpstr);
    if ((tmpstr[0] & 0xdf)=='Y') {
      done=1;
      delSp=TRUE;
    }
    if ((tmpstr[0] & 0xdf)=='N') {
      done=1;
      delSp=FALSE;
    }
  } // while (done==0) {

// Pass 1 = get line numbers.
  done=0;
  while (done == 0) {
    if (fgets(str_in,256,fi) != NULL){
      // Remove trailing carriage return & linefeed
      while ((str_in[strlen(str_in)-1]==13) ||
        (str_in[strlen(str_in)-1]==10) ) str_in[strlen(str_in)-1]=0;
      los=strlen(str_in);  // length of input string
      ns=sscanf(str_in,"%d %s",&ln[li], ln_str);
      // if (scrOut==1) printf("%d %s\n",ln[li],ln_str);
      li++;  // Increment line count.
      total_lines=li; // Need to store this for pass 2
    } else {
	    done = 1;
    }  //if (fgets(str_in,256,fi) != NULL){
  } // end while
  fclose(fi);   // Close input file

  // Populate new line number (nln) array & initialize line number array (nl)
  for (li=0;li<=total_lines;li++) {
    if (li==0) nln[li]=nsln; else nln[li]=nln[li-1]+nlinc;
  }

  printf("Pass 1 completed - Starting Pass 2\n");
  cont();

  // Pass 2 - do the renumber
  if (scrOut==1) printf("Starting Pass 2\n");
  fi = fopen(fi_name,"rb");
  done=0;  // Reset done for a 2nd pass
  li=0;  // Reset line increment for 2nd pass
  while (done == 0) {
    if (fgets(str_in,256,fi) == NULL) done=1;
    if (done==0) {  // Not Done
    // Remove trailing carriage return & linefeed
      for (i=strlen(str_in)-1;i>0;i--) {
        if((str_in[i]==10) || (str_in[i]==13)) str_in[i]=0; else i=-1;
      }

      los=strlen(str_in);  // length of input string

      // strip spaces (maybe) & convert all text to upper case
      //  (except for remarks and data statements)
      qfnd=FALSE;
      numq=0;
      remF=FALSE;
      j=0;  // wrk_str index
      for (i=0;i<los;i++){
        if (str_in[i] =='"') {  //The case for quotes
          numq++;
          wrk_str[j]=str_in[i];
          j++;
         } else {  // The case for non quotes
           if ((numq%2)>0) {  // We are inside a quote, copy the
                              // chararacter and do nothing more
             wrk_str[j]=str_in[i];
             j++;
           } 
           if ((numq%2)==0) {  // Outside quotes, we got work to do...
           
            //  First check for comments (treat Data statements like comments ...
              if ((((str_in[i] & 0xdf) =='R') &&  
                  ((str_in[i+1] & 0xdf) =='E') &&
                  ((str_in[i+2] & 0xdf) =='M'))  ||
                  (str_in[i] == 0x27)  || 
                  ((str_in[i]  =='D') &&  
                  (str_in[i+1] =='A') &&
                  (str_in[i+2] =='T') &&
                  (str_in[i+3] =='A'))) remF=TRUE;
              
              if (remF==TRUE) {   // It is a remark - ignore rest of line
                  strcpy(&wrk_str[j],&str_in[i]);
                  i=los;
              }

              if (remF==FALSE) {   // It is not a remark - process the character
                  if ((str_in[i]>96)  && (str_in[i]<123)) str_in[i]=str_in[i]-32;
                  if ((str_in[i] != ' ') || (delSp==FALSE)) {  // if not a space, copy to the working string
                    wrk_str[j]=str_in[i];
                    j++;
                  } // if (str_in[i] != ' ') {  // if not a space, copy to the working string
                } // if (remF==FALSE) {   // It is not a remark - process the character


           }  // if ((numq%2)==0) {  // Outside quotes, we got work to do...

        }  // if (str_in[i] =='"')  (else portion) - the case for non quotes


      } //      for (i=0;i<los;i++){

      if (remF==FALSE) wrk_str[j]=0;   // Terminate string for non remarks...


      ns=sscanf(wrk_str,"%d %s",&j, tmpstr);  // location of tmpstr (don't care about j)
      if (ns<2) {  // Either a blank line or a line with no line number.
        ln[li]=0; // no line number - set it to zero
      }

      if (ns==2) {   // We got a line number...
        strcpy(ln_str,strstr(wrk_str, tmpstr));   // basically loses the line number
        strcpy(wrk_str,ln_str);   // Working String is what we will work with  (doesn't have a line numb)
      }

      // Count double quotes
      numq=0;
      qi=0;
      qfnd=FALSE;
      for (i=0;i<(int)strlen(wrk_str);i++){
        if (wrk_str[i] =='"') {
          if ((numq%2)==0) {
            qidxb[qi]=i;
            qfnd=TRUE;
          }
          if ((numq%2)>0) {
            qidxe[qi]=i;
            qi++;
          }
          numq++;
        } // if (ln_str[i] =='"') {
      }  //for (i=0;i<strlen(wrk_str);i++){



       // PASS 2 - PHASE B (Parse keywords & line numbers)
       srchType=1;   // Search for keywords
       kwfnd=0;  // Set keyword found flag to 0
       lnfnd=0;  // Set line number found flag to 0
       for (p2i=0;p2i<(int)strlen(wrk_str);p2i++) {
            // Check if we are in a quote region
            if  (qfnd==TRUE) {
              qreg=FALSE;   // Assume not in a quote region
              for (i=0;i<qi;i++) if ((p2i>=qidxb[i]) && (p2i<=qidxe[i])) qreg=TRUE;
              if((qreg==FALSE) && (srchType==0)) srchType=1; // Go back to Key search
              if (qreg==TRUE) srchType=0;   // No key search or number search while in quote region
            }


            switch (srchType) {
                          // Check for keyword search...
            case 1:
                if (strncmp(&wrk_str[p2i],"THEN",4) ==0) {// Found THEN
                   kib[kwfnd]=p2i;
                   kie[kwfnd]=p2i+3;
                   p2i=p2i+3;
                   while (wrk_str[p2i+1]==' ')p2i++;  // skip spaces before possible number
                   if(isdigit(wrk_str[p2i+1]))srchType=2; // Time to look for numbers (maybe)
                   kwfnd++;
                }  // if (strncmp(&ln_str[p2i],"THEN",4) ==0)
                if (strncmp(&wrk_str[p2i],"ELSE",4) ==0) {// Found ELSE
                   kib[kwfnd]=p2i;
                   kie[kwfnd]=p2i+3;
                   p2i=p2i+3;
                   while (wrk_str[p2i+1]==' ')p2i++;  // Trim spaces before possible number
                   if(isdigit(wrk_str[p2i+1]))srchType=2; // Time to look for numbers (maybe)
                   kwfnd++;
                }  // if (strncmp(&ln_str[p2i],"ELSE",4) ==0)
                if (strncmp(&wrk_str[p2i],"GOTO",4) ==0) {// Found GOTO
                   kib[kwfnd]=p2i;
                   kie[kwfnd]=p2i+3;
                   p2i=p2i+3;
                   srchType=2; // Time to look for numbers
                   kwfnd++;
                }  // if (strncmp(&ln_str[p2i],"GOTO",4) ==0)
                if (strncmp(&wrk_str[p2i],"GOSUB",5) ==0) {// Found GOSUB
                   kib[kwfnd]=p2i;
                   kie[kwfnd]=p2i+4;
                   p2i=p2i+4;
                   srchType=2; // Time to look for numbers
                   kwfnd++;
                }  // if (strncmp(&ln_str[p2i],"GOSUB",5) ==0)
                if (strncmp(&wrk_str[p2i],"RESTORE",7) ==0) {// Found RESTORE
                   kib[kwfnd]=p2i;
                   kie[kwfnd]=p2i+6;
                   p2i=p2i+6;
                   srchType=2; // Time to look for numbers
                   kwfnd++;
                }  // if (strncmp(&ln_str[p2i],"RESTORE",7) ==0)

                if ((strncmp(&wrk_str[p2i],"REM",3) ==0)
                    || (wrk_str[p2i]=='\''))
                   p2i=(int)strlen(wrk_str);  // No need to look further
                break;

  // Check for line numbers search...
            case 2:
              if (isdigit(wrk_str[p2i])) {
                // Save the character
                tmpstr[lccnt]=wrk_str[p2i];
                if (lccnt==0) lnb[lnfnd]=p2i;
                lne[lnfnd]=p2i;  
                lccnt++;
              }  // if (isdigit(wrk_str[p2i])) {
              if (!isdigit(wrk_str[p2i+1]) && (lccnt>0)) {
                // If next character is not a digit and a linc number was found, terminate line string
                 tmpstr[lccnt]=0;  //  Terminate string,.
                 lnv[lnfnd]=atoi(tmpstr); // store value
                 rlnv=atoi(tmpstr); // store value
                 lnfnd++;
                 lccnt=0;
                 if (wrk_str[p2i+1]!=',') srchType=1;  // Switch search type to key words if not a comma
                 // For comma case, continue searching for line numbers for ongoto and ongosub cases
                 if (strncmp(&wrk_str[p2i],"ELSE",4)==0) {
                    p2i=p2i-1;   //Decrement so keyword is found on next loop increment
                    srchType=1; // Continuing search for keywords so "ELSE" will be found.
                 }
              }// if ((!isdigit(wrk_str[p2i]) &&  (wrk_str[p2i]!=' ') {
              break;

            } // switch (srchType) {

        } //for (p2i=0;p2i<strlen(wrk_str);p2i++) {

      // *Output line with new numbers

      if (lnfnd==0){
        fprintf(fo,"%d %s\n",nln[li],wrk_str);
        if (scrOut==1)  printf("%d %s\n",nln[li],wrk_str);
      }
      if (lnfnd>0) { // There are line numbers to adjust
        fprintf(fo,"%d ",nln[li]);
        if (scrOut==1)  printf("%d ",nln[li]);

        for (i=0;i<lnfnd;i++) {   // There might be more than one
           // Printing Stuff before the new line number
           if (i==0){  // Very 1st item - print from line start
              strcpy(tmpstr,wrk_str);
              tmpstr[lnb[0]]=0;  // Terminate string
              fprintf(fo,"%s",tmpstr);
              if (scrOut==1)  printf("%s",tmpstr);

           }
           if (i>0) { // Not the first item - print stuff from prior line num
              los=lnb[i]-(lne[i-1]+1);
              strncpy(tmpstr,&ln_str[lne[i-1]+1],los);
              tmpstr[los]=0;// Terminate string
              fprintf(fo,"%s",tmpstr);
              if (scrOut==1)  printf("%s",tmpstr);
           }
           // Now its time to print the updated line number value.
           nlnv=-1;
           for (j=0;j<total_lines;j++) {
              if (lnv[i]==ln[j]) {
                    nlnv =nln[j];
             }
           }  //for (j=0;j<total_lines;j++) {
           if (nlnv==-1) { 
              fprintf(fo," XXXX");
              if (scrOut==1)  printf(" XXXX");
           } else {
              fprintf(fo,"%d",nlnv);
              if (scrOut==1)  printf("%d",nlnv);
           }

          // DEBUG
          if (nlnv==-1) {
            printf("NO LINE NUMBER FOUND!\n");
            printf("%s\n",tmpstr);
            cont();

          }


           // print remaining string if no more line numbers
           if (i==(lnfnd-1)) {
             fprintf(fo,"%s\n",&wrk_str[lne[i]+1]);
             if (scrOut==1)  printf("%s\n",&wrk_str[lne[i]+1]);
          }

          }  // for (i=0;i<lnfnd;i++) {
      } // if lnfnd>0) { // There are line numbers to adjust

      li++;  // Increment line count.

    }  //     if (done==0)
  } // end while


  fclose(fo);
  fclose(fi);
  printf("Pass 2 completed.\n");
  cont();

  return 0;
}



/****************************************************************
** Function:  cont()
**
** Description:  Prompts operator to press enter to continue.
**
****************************************************************/

void cont() {

  getchar();   /* Seems to be necessary to flush stdin */
  printf("Press enter to continue:");
  getchar();   /* This is the one that counts */
}
