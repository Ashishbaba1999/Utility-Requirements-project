/*4.6 Utility: Search Srtucture by item ID
* Description - Expand and extract the full BOM structure for given item ID.
* Requirments -
* Fetch item -> Latest Revision
* Expand BOM recursively
* Export full intended BOM
*/

#include<stdio.h>
#include<tcinit/tcinit.h>
#include<tc/tc_startup.h>
#include<tc/emh.h>
#include<stdlib.h>
#include<tc/tc_util.h>
#include<tccore/item.h>
#include<bom/bom.h>
#include<tccore/aom_prop.h>


void Display_Usage() {
	printf("\n\n Teamcenter Utility to Import Named Referance to Dataset");
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
	if (iFail != ITK_ok)
	{
		EMH_ask_error_text(iFail, &cError);
		printf("\n Error is : %s", cError);
		if (cError)MEM_free(cError);
		exit(0);
	}
}

///////////////////////////////////////////////////////////////////////////////////



#include "Header.h"

FILE* fp = NULL;

int ITK_user_main(int argc, char* argv[])
{
	int iFail = 0;
	int iCount = 0;
	int iChildCount = 0;
	char* cError = NULL;
	char* cUserID = NULL;
	char* cPassword = NULL;
	char* cGroup = NULL;
	char* cItemID = NULL;

	tag_t tItem = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	tag_t* tChild = NULLTAG;
	tag_t tRev = NULLTAG;

	char* cName = NULL;
	char* cId = NULL;
	char* cRevId = NULL;
	char* cOwner = NULL;


	if (ITK_ask_cli_argument("-h") || ITK_ask_cli_argument("-help"))
	{
		Display_Usage();
	}

	if (argc == 5)
	{
		cUserID = ITK_ask_cli_argument("-u=");
		cPassword = ITK_ask_cli_argument("-p=");
		cGroup = ITK_ask_cli_argument("-g=");
		cItemID = ITK_ask_cli_argument("-itemID=");

		fp = TC_fopen("BOM_Report2.csv","w+");
		if (fp != NULL)
		{
			TC_fprintf(fp, "Object Name,Item ID, Revision ID,Owner");

			Report_Error(iFail = ITK_init_module(cUserID, cPassword, cGroup));
			printf("\n Login Success");

			Report_Error(iFail = ITEM_find_item(cItemID, &tItem));
			Report_Error(iFail = ITEM_ask_latest_rev(tItem, &tRev));
			
			
				Report_Error(iFail = BOM_create_window(&tWindow));
				Report_Error(iFail = BOM_set_window_top_line(tWindow, tItem, tRev, NULLTAG, &tTopLine));
				Report_Error(iFail = AOM_ask_value_string(tTopLine, "bl_rev_object_name", &cName));
				Report_Error(iFail = AOM_ask_value_string(tTopLine, "bl_item_item_id", &cId));
				Report_Error(iFail = AOM_ask_value_string(tTopLine, "bl_rev_item_revision_id", &cRevId));
				Report_Error(iFail = AOM_ask_value_string(tTopLine, "bl_rev_owning_user", &cOwner));

				TC_fprintf(fp, "\n %s,%s,%s,%s", cName, cId, cRevId, cOwner);

				Report_Error(iFail = BOM_line_ask_all_child_lines(tTopLine, &iChildCount, &tChild));
				if (iChildCount > 0)
				{
					bom_sub_child(tChild, iChildCount);
				}
				Report_Error(iFail = BOM_close_window(tWindow));

			
			Report_Error(iFail = ITK_exit_module(TRUE));
			printf("\n Logout Success");

			fflush(fp);
			fclose(fp);
		}
		else
		{
			printf("\n File not found");
		}
	}
	else
	{
		printf("Argument Count is less or more");
	}
	if (tChild)MEM_free(tChild);
	return 0;

}
int bom_sub_child(tag_t* tChild, int iChildCount)
{
	int iFail = 0;
	char* cName = NULL;
	char* cId = NULL;
	char* cRevId = NULL;
	char* cOwner = NULL;

	int iSubChildCount = 0;
	tag_t* tSubChild = NULLTAG;

	
	for (int j = 0; j < iChildCount; j++)
	{
		Report_Error(iFail = AOM_ask_value_string(tChild[j], "bl_rev_object_name", &cName));
		Report_Error(iFail = AOM_ask_value_string(tChild[j], "bl_item_item_id", &cId));
		Report_Error(iFail = AOM_ask_value_string(tChild[j], "bl_rev_item_revision_id", &cRevId));
		Report_Error(iFail = AOM_ask_value_string(tChild[j], "bl_rev_owning_user", &cOwner));
		TC_fprintf(fp, "\n %s,%s,%s,%s", cName, cId, cRevId, cOwner);

		Report_Error(iFail = BOM_line_ask_all_child_lines(tChild[j], &iSubChildCount, &tSubChild));
		if (iSubChildCount > 0)
		{
			bom_sub_child(tSubChild, iSubChildCount);
		}

		
	}

	return 0;
	

}

