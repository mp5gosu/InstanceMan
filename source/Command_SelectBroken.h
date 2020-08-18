#pragma once

#include "common_functions.h"
#include "instance_functions.h"


class Command_SelectBroken : public CommandData
{
INSTANCEOF(Command_SelectBroken, CommandData)

public:
	Int32 GetState(BaseDocument* doc) override
	{
		// Disable Menu entry if no object is selected
		const AutoAlloc<AtomArray> arr;
		doc->GetActiveObjects(arr, GETACTIVEOBJECTFLAGS::NONE);
		if (!arr || arr->GetCount() == 0)
			return 0;
		return CMD_ENABLED;
	}

	Bool Execute(BaseDocument* doc) override
	{
		StopAllThreads();
		if (!doc)
			return false;

		g_DeselectAllObjects(doc);
		auto* obj = doc->GetFirstObject();
		while (obj)
		{
			if (!obj->IsInstanceOf(Oinstance))
				continue;
			
			auto* linkedObj = g_GetInstanceRef(obj);
			if(!linkedObj)
				doc->SetActiveObject(obj);
			
			obj = static_cast<BaseObject*>(g_GetNextElement(obj));
		}

		return true;
	}
};
