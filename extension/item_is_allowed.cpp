/**
 * =============================================================================
 * TF2 Items Improved Extension
 * Copyright (C) 2014 AzuiSleet, Asher Baker (asherkin), Powerlord.
 * All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "item_is_allowed.h"

CDetour *itemIsAllowedDetour = NULL;

IForward *g_pForwardItemIsAllowed;
IForward *g_pForwardItemIsAllowed_Post;

DETOUR_DECL_MEMBER1(ItemIsAllowed, bool, CEconItemView *, cscript)
{
	int client = gamehelpers->EntityToBCompatRef(reinterpret_cast<CBaseEntity *>(this));

	bool actualres = DETOUR_MEMBER_CALL(ItemIsAllowed)(cscript);
	if (!g_pForwardItemIsAllowed)
	{
		g_pSM->LogMessage(myself, "Invalid Forward");
		return actualres;
	}

	cell_t result = 0;
	cell_t newres = actualres ? 1 : 0;

	g_pForwardItemIsAllowed->PushCell(client);
	g_pForwardItemIsAllowed->PushCell(cscript->m_iItemDefinitionIndex);
	g_pForwardItemIsAllowed->PushCellByRef(&newres);
	g_pForwardItemIsAllowed->Execute(&result);

	if (result > Pl_Continue)
	{
		actualres = newres == 0 ? false : true;
	}

	g_pForwardItemIsAllowed_Post->PushCell(client);
	g_pForwardItemIsAllowed_Post->PushCell(cscript->m_iItemDefinitionIndex);
	g_pForwardItemIsAllowed_Post->PushCell(actualres);
	g_pForwardItemIsAllowed_Post->Execute(NULL);

	return actualres;
}

bool InitialiseItemIsAllowedDetour()
{
	itemIsAllowedDetour = DETOUR_CREATE_MEMBER(ItemIsAllowed, "ItemIsAllowed");

	if (itemIsAllowedDetour != NULL)
	{
		itemIsAllowedDetour->EnableDetour();
		return true;
	}

	g_pSM->LogError(myself, "ItemIsAllowed detour failed");
	return false;
}

void RemoveItemIsAllowedDetour()
{
	itemIsAllowedDetour->Destroy();
}
