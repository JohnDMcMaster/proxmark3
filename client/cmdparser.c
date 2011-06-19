//-----------------------------------------------------------------------------
// Copyright (C) 2010 iZsh <izsh at fail0verflow.com>
//
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// Command parser
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "cmdparser.h"
#include "proxmark3.h"

//Set to true if the proxmark is not availible
int g_offline;

void CmdsHelp(const command_t Commands[])
{
	if (Commands[0].Name == NULL)
		return;
	int i = 0;
	while (Commands[i].Name)
	{
		if (!g_offline || Commands[i].Offline)
			PrintAndLog("%-16s %s", Commands[i].Name, Commands[i].Help);
		++i;
	}
}

void CmdsParse(const command_t Commands[], const char *Cmd)
{
	char cmd_name[32];
	int len = 0;
	memset(cmd_name, 0, 32);
	sscanf(Cmd, "%31s%n", cmd_name, &len);
	int i = 0;
	
	//First try to find exact match
	while (Commands[i].Name && strcmp(Commands[i].Name, cmd_name))
		++i;

	//Fallback to find exactly one prefix-match if we couldn't find an exact match
	if(!Commands[i].Name) {
		int last_match = 0;
		int matches = 0;

		for(i=0;Commands[i].Name;i++) {
			if( !strncmp(Commands[i].Name, cmd_name, strlen(cmd_name)) ) {
				last_match = i;
				matches++;
			}
		}
		if(matches == 1) i=last_match;
	}

	//Did we find something?
	if (Commands[i].Name) {
		while (Cmd[len] == ' ')
			++len;
		if (g_offline && !Commands[i].Offline) {
			PrintAndLog("%s: cannot be used offline", Commands[i].Name);
		} else {
			Commands[i].Parse(Cmd + len);
		}
	} else {
		PrintAndLog("%s: unknown command", Cmd);
		// show help for selected hierarchy or if command not recognised
		CmdsHelp(Commands);
	}
}
