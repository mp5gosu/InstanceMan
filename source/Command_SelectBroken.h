#pragma once

#include "common_functions.h"
#include "instance_functions.h"


class Command_SelectBroken : public CommandData
{
INSTANCEOF(Command_SelectBroken, CommandData)

public:
	Bool Execute(BaseDocument* doc) override
	{
		StopAllThreads();
		if (!doc)
			return false;

		const auto qShift = g_CheckModifierKey(QSHIFT);

		if (!qShift)
			g_DeselectAllObjects(doc);

		auto* obj = doc->GetFirstObject();
		while (obj)
		{
			if (!obj->IsInstanceOf(Oinstance))
			{
				obj = static_cast<BaseObject*>(g_GetNextElement(obj));
				continue;
			}

			auto* linkedObj = g_GetInstanceRef(obj);
			if (!linkedObj)
				doc->SetActiveObject(obj, SELECTION_ADD);

			obj = static_cast<BaseObject*>(g_GetNextElement(obj));
		}

		EventAdd();
		return true;
	}
};
