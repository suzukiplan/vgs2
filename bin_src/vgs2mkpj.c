#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef MAX_PATH 
#define MAX_PATH 1023 
#endif

#ifdef _WIN32
#define DS '\\'
#else
#define DS '/'
#define lstrlen strlen
#endif

void cpy(char* from,char* to,char* company,char* project);
void bin(char* from,char* to);
void replace(char* src,const char* from,const char* to);
int isalnum_str(const char* str);
#ifdef _WIN32
int isdir_str(const char* str);
#endif
int gettoken(char* src,char** token,int max);
void trimstring(char* src);
void MakeDirectoryPath(char *szPath);
char* vgs_home=NULL;

#ifdef _WIN32
#define myheapalloc(x) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, x))
#define myheapfree(x)  (HeapFree(GetProcessHeap(), 0, x))

typedef BOOL (WINAPI *SetSecurityDescriptorControlFnPtr)(
   IN PSECURITY_DESCRIPTOR pSecurityDescriptor,
   IN SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
   IN SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet);

BOOL AddAccessRights(TCHAR *lpszFileName, TCHAR *lpszAccountName, 
      DWORD dwAccessMask) {

   // SID variables.
   SID_NAME_USE   snuType;
   TCHAR *        szDomain       = NULL;
   DWORD          cbDomain       = 0;
   LPVOID         pUserSID       = NULL;
   DWORD          cbUserSID      = 0;

   // File SD variables.
   PSECURITY_DESCRIPTOR pFileSD  = NULL;
   DWORD          cbFileSD       = 0;

   // New SD variables.
   SECURITY_DESCRIPTOR  newSD;

   // ACL variables.
   PACL           pACL           = NULL;
   BOOL           fDaclPresent;
   BOOL           fDaclDefaulted;
   ACL_SIZE_INFORMATION AclInfo;

   // New ACL variables.
   PACL           pNewACL        = NULL;
   DWORD          cbNewACL       = 0;

   // Temporary ACE.
   LPVOID         pTempAce       = NULL;
   UINT           CurrentAceIndex = 0;

   UINT           newAceIndex = 0;

   // Assume function will fail.
   BOOL           fResult        = FALSE;
   BOOL           fAPISuccess;

   SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;

   // New APIs available only in Windows 2000 and above for setting 
   // SD control
   SetSecurityDescriptorControlFnPtr _SetSecurityDescriptorControl = NULL;

   __try {

      // 
      // STEP 1: Get SID of the account name specified.
      // 
      fAPISuccess = LookupAccountName(NULL, lpszAccountName,
            pUserSID, &cbUserSID, szDomain, &cbDomain, &snuType);

      // API should have failed with insufficient buffer.
      if (fAPISuccess)
         __leave;
      else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
         printf(TEXT("LookupAccountName() failed. Error %d\n"), 
               GetLastError());
         __leave;
      }

      pUserSID = myheapalloc(cbUserSID);
      if (!pUserSID) {
         __leave;
      }

      szDomain = (TCHAR *) myheapalloc(cbDomain * sizeof(TCHAR));
      if (!szDomain) {
         __leave;
      }

      fAPISuccess = LookupAccountName(NULL, lpszAccountName,
            pUserSID, &cbUserSID, szDomain, &cbDomain, &snuType);
      if (!fAPISuccess) {
         __leave;
      }

      // 
      // STEP 2: Get security descriptor (SD) of the file specified.
      // 
      fAPISuccess = GetFileSecurity(lpszFileName, 
            secInfo, pFileSD, 0, &cbFileSD);

      // API should have failed with insufficient buffer.
      if (fAPISuccess)
         __leave;
      else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
         __leave;
      }

      pFileSD = myheapalloc(cbFileSD);
      if (!pFileSD) {
         __leave;
      }

      fAPISuccess = GetFileSecurity(lpszFileName, 
            secInfo, pFileSD, cbFileSD, &cbFileSD);
      if (!fAPISuccess) {
         __leave;
      }

      // 
      // STEP 3: Initialize new SD.
      // 
      if (!InitializeSecurityDescriptor(&newSD, 
            SECURITY_DESCRIPTOR_REVISION)) {
         __leave;
      }

      // 
      // STEP 4: Get DACL from the old SD.
      // 
      if (!GetSecurityDescriptorDacl(pFileSD, &fDaclPresent, &pACL,
            &fDaclDefaulted)) {
         __leave;
      }

      // 
      // STEP 5: Get size information for DACL.
      // 
      AclInfo.AceCount = 0; // Assume NULL DACL.
      AclInfo.AclBytesFree = 0;
      AclInfo.AclBytesInUse = sizeof(ACL);

      if (pACL == NULL)
         fDaclPresent = FALSE;

      // If not NULL DACL, gather size information from DACL.
      if (fDaclPresent) {    
         
         if (!GetAclInformation(pACL, &AclInfo, 
               sizeof(ACL_SIZE_INFORMATION), AclSizeInformation)) {
            __leave;
         }
      }

      // 
      // STEP 6: Compute size needed for the new ACL.
      // 
      cbNewACL = AclInfo.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) 
            + GetLengthSid(pUserSID) - sizeof(DWORD);

      // 
      // STEP 7: Allocate memory for new ACL.
      // 
      pNewACL = (PACL) myheapalloc(cbNewACL);
      if (!pNewACL) {
         __leave;
      }

      // 
      // STEP 8: Initialize the new ACL.
      // 
      if (!InitializeAcl(pNewACL, cbNewACL, ACL_REVISION2)) {
         __leave;
      }

      // 
      // STEP 9 If DACL is present, copy all the ACEs from the old DACL
      // to the new DACL.
      // 
      // The following code assumes that the old DACL is
      // already in Windows 2000 preferred order.  To conform 
      // to the new Windows 2000 preferred order, first we will 
      // copy all non-inherited ACEs from the old DACL to the 
      // new DACL, irrespective of the ACE type.
      // 
      
      newAceIndex = 0;

      if (fDaclPresent && AclInfo.AceCount) {

         for (CurrentAceIndex = 0; 
               CurrentAceIndex < AclInfo.AceCount;
               CurrentAceIndex++) {

            // 
            // STEP 10: Get an ACE.
            // 
            if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
               __leave;
            }

            // 
            // STEP 11: Check if it is a non-inherited ACE.
            // If it is an inherited ACE, break from the loop so
            // that the new access allowed non-inherited ACE can
            // be added in the correct position, immediately after
            // all non-inherited ACEs.
            // 
            if (((ACCESS_ALLOWED_ACE *)pTempAce)->Header.AceFlags
               & INHERITED_ACE)
               break;

            // 
            // STEP 12: Skip adding the ACE, if the SID matches
            // with the account specified, as we are going to 
            // add an access allowed ACE with a different access 
            // mask.
            // 
            if (EqualSid(pUserSID,
               &(((ACCESS_ALLOWED_ACE *)pTempAce)->SidStart)))
               continue;

            // 
            // STEP 13: Add the ACE to the new ACL.
            // 
            if (!AddAce(pNewACL, ACL_REVISION, MAXDWORD, pTempAce,
                  ((PACE_HEADER) pTempAce)->AceSize)) {
               __leave;
            }

            newAceIndex++;
         }
      }

      // 
      // STEP 14: Add the access-allowed ACE to the new DACL.
      // The new ACE added here will be in the correct position,
      // immediately after all existing non-inherited ACEs.
      // 
      if (!AddAccessAllowedAce(pNewACL, ACL_REVISION2, dwAccessMask,
            pUserSID)) {
         __leave;
      }

      // 
      // STEP 15: To conform to the new Windows 2000 preferred order,
      // we will now copy the rest of inherited ACEs from the
      // old DACL to the new DACL.
      // 
      if (fDaclPresent && AclInfo.AceCount) {

         for (; 
              CurrentAceIndex < AclInfo.AceCount;
              CurrentAceIndex++) {

            // 
            // STEP 16: Get an ACE.
            // 
            if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
               __leave;
            }

            // 
            // STEP 17: Add the ACE to the new ACL.
            // 
            if (!AddAce(pNewACL, ACL_REVISION, MAXDWORD, pTempAce,
                  ((PACE_HEADER) pTempAce)->AceSize)) {
               __leave;
            }
         }
      }

      // 
      // STEP 18: Set the new DACL to the new SD.
      // 
      if (!SetSecurityDescriptorDacl(&newSD, TRUE, pNewACL, 
            FALSE)) {
         __leave;
      }

      // 
      // STEP 19: Copy the old security descriptor control flags 
      // regarding DACL automatic inheritance for Windows 2000 or 
      // later where SetSecurityDescriptorControl() API is available
      // in advapi32.dll.
      // 
      _SetSecurityDescriptorControl = (SetSecurityDescriptorControlFnPtr)
            GetProcAddress(GetModuleHandle(TEXT("advapi32.dll")),
            "SetSecurityDescriptorControl");
      if (_SetSecurityDescriptorControl) {

         SECURITY_DESCRIPTOR_CONTROL controlBitsOfInterest = 0;
         SECURITY_DESCRIPTOR_CONTROL controlBitsToSet = 0;
         SECURITY_DESCRIPTOR_CONTROL oldControlBits = 0;
         DWORD dwRevision = 0;

         if (!GetSecurityDescriptorControl(pFileSD, &oldControlBits,
            &dwRevision)) {
            __leave;
         }

         if (oldControlBits & SE_DACL_AUTO_INHERITED) {
            controlBitsOfInterest =
               SE_DACL_AUTO_INHERIT_REQ |
               SE_DACL_AUTO_INHERITED;
            controlBitsToSet = controlBitsOfInterest;
         }
         else if (oldControlBits & SE_DACL_PROTECTED) {
            controlBitsOfInterest = SE_DACL_PROTECTED;
            controlBitsToSet = controlBitsOfInterest;
         }
         
         if (controlBitsOfInterest) {
            if (!_SetSecurityDescriptorControl(&newSD,
               controlBitsOfInterest,
               controlBitsToSet)) {
               __leave;
            }
         }
      }

      // 
      // STEP 20: Set the new SD to the File.
      // 
      if (!SetFileSecurity(lpszFileName, secInfo,
            &newSD)) {
         __leave;
      }

      fResult = TRUE;

   } __finally {

      // 
      // STEP 21: Free allocated memory
      // 
      if (pUserSID)
         myheapfree(pUserSID);

      if (szDomain)
         myheapfree(szDomain);

      if (pFileSD)
         myheapfree(pFileSD);

      if (pNewACL)
         myheapfree(pNewACL);
   }
   
   return fResult;
}
#endif

/*
 *----------------------------------------------------------------------------
 * main
 *----------------------------------------------------------------------------
 */
int main(int argc,char* argv[])
{
	char buf[4096];
	char bufF[4096];
	char bufT[4096];
	char* token[16];
	size_t len;
	int toknum;
	FILE* fp;
	char* cp;
#ifdef _WIN32
	int i;
#endif

	umask(0);

	vgs_home=getenv("VGS2_HOME");
	if(NULL==vgs_home) {
		puts("ERROR: required environment variable has not defined. (VGS2_HOME)");
		return 1;
	}

	if(argc<4) {
		puts("usage: vgs2mkpj company_name project_name directory");
		return 1;
	}

	if(31<strlen(argv[1])
	|| !isalpha(argv[1][0])
	|| !isalnum_str(argv[1])
	) {
		puts("ERROR: invalid company name.");
		return 1;
	}

	if(8<strlen(argv[2])
	|| !isalpha(argv[2][0])
	|| !isupper(argv[2][0])
	|| !isalnum_str(argv[2])
	) {
		puts("ERROR: invalid project name.");
		return 1;
	}

	len=strlen(argv[3]);
#ifdef _WIN32
	if(63<len || len<4
	|| !isalpha(argv[3][0])
	|| ':'!=argv[3][1]
	|| '\\'!=argv[3][2]
	|| !isdir_str(&argv[3][3])
	|| NULL!=strstr(argv[3],"\\\\")
	) {
#else
	if(63<len) {
#endif
		puts("ERROR: invalid directory.");
		return 1;
	}

	/* テンプレートファイルを開く */
	sprintf(buf,"%s%ctemplate%c.vgs2mkpj.cfg",vgs_home,DS,DS);
	fp=fopen(buf,"rt");
	if(NULL==fp) {
		puts("ERROR: system file has not exist. (.vgs2mkpj.cfg)");
		return 1;
	}

	while(fgets(buf,sizeof(buf),fp)) {
		if(NULL!=(cp=strchr(buf,'#'))) {
			*cp='\0';
		}
#ifdef _WIN32
		for(i=0;buf[i];i++) if('/'==buf[i]) buf[i]='\\';
#endif
		trimstring(buf);
		replace(buf,"{Company}",argv[1]);
		replace(buf,"{Project}",argv[2]);
		toknum=gettoken(buf,token,16);
		if(1<toknum) {
			if(strcmp(token[0],"cpy")==0) {
				if(toknum==3 || toknum==4) {
					if(toknum==3) {
						token[3]=token[1];
					}
					sprintf(bufF,"%s%ctemplate%c%s",vgs_home,DS,DS,token[1]);
					sprintf(bufT,"%s%c%s%c",argv[3],DS,token[2],DS);
					MakeDirectoryPath(bufT);
					strcat(bufT,token[3]);
					printf("creating ... %s\n",bufT);
					cpy(bufF,bufT,argv[1],argv[2]);
				}
			}
			else if(strcmp(token[0],"bin")==0) {
				if(toknum==3 || toknum==4) {
					if(toknum==3) {
						token[3]=token[1];
					}
					sprintf(bufF,"%s%ctemplate%c%s",vgs_home,DS,DS,token[1]);
					sprintf(bufT,"%s%c%s%c",argv[3],DS,token[2],DS);
					MakeDirectoryPath(bufT);
					strcat(bufT,token[3]);
					printf("creating ... %s\n",bufT);
					bin(bufF,bufT);
				}
			}
			else if(strcmp(token[0],"dir")==0) {
				if(toknum==2) {
					sprintf(bufT,"%s%c%s%c",argv[3],DS,token[1],DS);
					MakeDirectoryPath(bufT);
				}
			}
		}
	}

	fclose(fp);
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * テキストファイルのコピー
 *----------------------------------------------------------------------------
 */
void cpy(char* from,char* to,char* company,char* project)
{
	FILE* fpR;
	FILE* fpW;
	char buf[8192];
	if(NULL==(fpR=fopen(from,"rt"))) {
		printf("ERROR: could not open file. (%s)\n",from);
		exit(-1);
	}
	if(NULL==(fpW=fopen(to,"wt"))) {
		printf("ERROR: could not open file. (%s)\n",to);
		exit(-1);
	}
	while(fgets(buf,sizeof(buf),fpR)) {
		replace(buf,"{Company}",company);
		replace(buf,"{Project}",project);
		fprintf(fpW,"%s",buf);
	}
	fclose(fpR);
	fclose(fpW);

#ifdef _WIN32
	AddAccessRights(to,"Everyone",GENERIC_ALL);
#endif
}

/*
 *----------------------------------------------------------------------------
 * バイナリファイルのコピー
 *----------------------------------------------------------------------------
 */
void bin(char* from,char* to)
{
	FILE* fpR;
	FILE* fpW;
	int size;
	char* buf;
	if(NULL==(fpR=fopen(from,"rb"))) {
		printf("ERROR: could not open file. (%s)\n",from);
		exit(-1);
	}
	if(NULL==(fpW=fopen(to,"wb"))) {
		printf("ERROR: could not open file. (%s)\n",to);
		exit(-1);
	}
	fseek(fpR,0,SEEK_END);
	size=ftell(fpR);
	fseek(fpR,0,SEEK_SET);
	if(NULL==(buf=(char*)malloc(size))) {
		puts("ERROR: no memory");
		exit(-1);
	}
	fread(buf,size,1,fpR);
	fwrite(buf,size,1,fpW);
	free(buf);
	fclose(fpR);
	fclose(fpW);

#ifdef _WIN32
	AddAccessRights(to,"Everyone",GENERIC_ALL);
#endif
}

/*
 *----------------------------------------------------------------------------
 * 文字列置換
 *----------------------------------------------------------------------------
 */
void replace(char* src,const char* from,const char* to)
{
	char* cp;
	size_t len,len2;
	char* buf;
	buf=(char*)malloc(strlen(src)+1);
	if(NULL==buf) {
		puts("ERROR: no memory.");
		exit(-1);
	}
	len=strlen(from);
	len2=strlen(to);
	while(NULL!=(cp=strstr(src,from))) {
		strcpy(buf,cp+len);
		strcpy(cp,to);
		cp+=len2;
		strcpy(cp,buf);
		src=cp;
	}
	free(buf);
}


/*
 *----------------------------------------------------------------------------
 * 英数字の文字列かチェック
 *----------------------------------------------------------------------------
 */
int isalnum_str(const char* str)
{
	int i;
	for(i=0;str[i];i++) {
		if(!isalnum(str[i])) {
			return 0;
		}
	}
	return 1;
}

/*
 *----------------------------------------------------------------------------
 * ディレクトリ文字列かチェック
 *----------------------------------------------------------------------------
 */
#ifdef _WIN32
int isdir_str(const char* str)
{
	int i;
	for(i=0;str[i];i++) {
		if(!isalpha(str[i])
		&& !isdigit(str[i])
		&& '\\'!=str[i]
		&& '-'!=str[i]
		&& '_'!=str[i]) {
			return 0;
		}
		
	}
	return 1;
}
#endif

/*
 *----------------------------------------------------------------------------
 * トークン分割
 *----------------------------------------------------------------------------
 */
int gettoken(char* src,char** token,int max)
{
    int n=0;
    if(max<1) {
        return 0;
    }
    while(1) {
        for(;*src==' '||*src=='\t';src++);
        if(*src=='\0'||*src=='\r'||*src=='\n') {
            return n;
        }
        token[n++]=src;
        for(; *src!=' '&&*src!='\t';src++) {
            if(*src=='\0') {
                return n;
            }
            if(*src=='\r'||*src=='\n') {
                *src='\0';
                return n;
            }
        }
        *src='\0';
        src++;
        if(max<=n) {
            return n;
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * トリム
 *----------------------------------------------------------------------------
 */
void trimstring(char* src)
{
	int i,j;
	int len;
	/* 前方の空白をトリム */
	for(i=0;' '==src[i];i++);
	if(i) {
		for(j=0;src[i]!='\0';j++,i++) {
			src[j]=src[i];
		}
		src[j]='\0';
	}
	/* 広報の空白をトリム */
	for(len=(int)strlen(src)-1;0<=len && ' '==src[len];len--) {
		src[len]='\0';
	}
}

/*
 *----------------------------------------------------------------------------
 * /ディレクトリ作成
 *----------------------------------------------------------------------------
 */
void MakeDirectoryPath(char *szPath)
{
    char szMakePath[MAX_PATH+1];
    int i;

    for (i = 0; i < lstrlen(szPath); i++) {
        szMakePath[i] = szPath[i];

        if (szPath[i] == DS) {

            szMakePath[i+1] = '\0';

            // 同一名称のファイルやフォルダが存在しなければフォルダ作成
#ifdef _WIN32
            if ( GetFileAttributes(szMakePath) == 0xFFFFFFFF ) {
                if ( ! CreateDirectory(szMakePath, NULL) ) {
					printf("ERROR: could not create directory. (%s)\n",szPath);
					exit(-1);
                }
            }
#else
            mkdir(szMakePath,0775);
#endif
        }
    }
}
