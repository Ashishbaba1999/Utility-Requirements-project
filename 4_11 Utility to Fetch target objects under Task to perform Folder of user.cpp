/*4.11 Utility: Fetch target objects under Task to Perform Folder of user

* Requirments -
* Input - user ID of the user
* Export findings in csv

*/

#include<stdio.h>
#include<tc/tc_startup.h>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<stdlib.h>
#include<tccore/workspaceobject.h>
#include<sa/user.h>
#include<fclasses/tc_string.h>
#include<tc/folder.h>
#include<tccore/aom.h>
#include<tc/tc_util.h>
#include<tccore/aom_prop.h>
#include<tccore/grm.h>
#include<epm/epm.h>

void Display_Usage() {
	printf("\n\n Teamcenter Utility to ....");
	printf("\n\n -u=\t Login User ID");
	printf("\n\n -p=\t Login User Password");
	printf("\n\n -g=\t Login User Group");
	printf("\n\n -h\t to display the detailed information of utility");
	printf("\n\n -help\t to display the detailed information of utility");
	exit(0);
}

int Report_Error(int iFail)
{
	char* cError = NULL;
	if (iFail != ITK_ok) {
		EMH_ask_error_text(iFail, &cError);
		printf("\n\n Error is : %s", cError);
		MEM_free(cError);
		exit(0);
	}
}

///////////////////////////////////////////////////////////////////////////////////




#include "Header.h"

int ITK_user_main(int argc, char* argv[]) {

	int iFail = 0;
	int iCount = 0;
	int iObjectCount = 0;
	char* cError = NULL;
	char* cUserID = NULL;
	char* cPassword = NULL;
	char* cGroup = NULL;
	char* cCRrev = NULL;

	FILE* fp = NULL;

	tag_t tUser = NULLTAG;
	tag_t tNewUser = NULLTAG;
	tag_t tGroup = NULLTAG;
	WSO_search_criteria_t tCriteria;
	tag_t* tSearchList = NULLTAG;
	tag_t tRelationType = NULLTAG;
	int iSecObjCount = 0;
	tag_t* tSecObjects = NULLTAG;
	char* cProblemItem = NULL;
	char* cID = NULL;
	int iRef = 0;
	tag_t* tRefList = NULLTAG;
	tag_t tPseudoRelation = NULLTAG;
	int iTargetCount = 0;
	tag_t* tTargets = NULLTAG;
	char* cTarget = NULL;
	tag_t tRootTargetrelation = NULLTAG;
	int iRootTargetCount = 0;
	tag_t* tRootTargets = NULLTAG;
	char* cDValue = NULL;
	int nCount = NULL;
	tag_t* tAttachments = NULLTAG;
	tag_t tRootTask = NULLTAG;
	char* cValue = NULL;




	if (ITK_ask_cli_argument("-h") || ITK_ask_cli_argument("-help")) {
		Display_Usage();
	}

	if (argc == 5) {
		cUserID = ITK_ask_cli_argument("-u=");
		cPassword = ITK_ask_cli_argument("-p=");
		cGroup = ITK_ask_cli_argument("-g=");
		cID = ITK_ask_cli_argument("-id=");


		fp = TC_fopen("TargetObjects.csv", "w+");
		if (fp == NULL)
		{
			printf("\n file not found");
			return 1;
		}
		Report_Error(iFail = ITK_init_module(cUserID, cPassword, cGroup));
		printf("\n\n Login Success");

		Report_Error(iFail = WSOM_clear_search_criteria(&tCriteria));
		Report_Error(iFail = SA_find_user2(cID, &tUser));


		tc_strcpy(tCriteria.class_name, "TasksToPerform");
		tc_strcpy(tCriteria.name, "*");
		tCriteria.owner = tUser;


		Report_Error(iFail = WSOM_search(tCriteria, &iCount, &tSearchList));
		if (iCount > 0)
		{
			printf("\n Search count = %d", iCount);
			for (int j = 0; j < iCount; j++)
			{
				Report_Error(iFail = FL_ask_references(tSearchList[j], FL_fsc_by_name, &iRef, &tRefList));
				printf("\n Search count = %d", iRef);
				for (int k = 0; k < iRef; k++)
				{
					Report_Error(iFail = EPM_ask_root_task(tRefList[k], &tRootTask));
					Report_Error(iFail = EPM_ask_attachments(tRootTask, EPM_target_attachment, &nCount, &tAttachments));
					for (int l = 0; l < nCount; l++)
					{
						Report_Error(iFail = AOM_UIF_ask_value(tAttachments[l], "object_name", &cValue));
						printf("\n target Name is:%s", cValue);
					}

				}

			}
		}

		Report_Error(iFail = ITK_exit_module(TRUE));
		printf("\n\n Logout Success");
	}
	else {
		printf("\n\n Argument count is less or more");
	}
	if (tSearchList) {
		MEM_free(tSearchList);
	}
	if (tSecObjects) {
		MEM_free(tSecObjects);
	}
	return 0;
}




