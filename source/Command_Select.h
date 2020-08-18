#pragma once

#include "common_functions.h"
#include "instance_functions.h"


class Command_Select : public CommandData
{
INSTANCEOF(Command_Select, CommandData)

public:
	Int32 GetState(BaseDocument* doc, GeDialog* parentManager) override
	{
		// Disable Menu entry if no object is selected
		AutoAlloc<AtomArray> arr;
		doc->GetActiveObjects(arr, GETACTIVEOBJECTFLAGS::NONE);
		if (!arr || arr->GetCount() == 0)
			return 0;
		return CMD_ENABLED;
	}

	Bool Execute(BaseDocument* doc, GeDialog* parentManager) override
	{
		StopAllThreads();
		if (!doc)
			return false;

		// Create a cache of objects to operate on
		AutoAlloc<AtomArray> activeObjects;
		doc->GetActiveObjectsFilter(activeObjects, false, NOTOK, Obase);

		// Allocation failed
		if (!activeObjects)
			return false;

		// Detect Key modifiers
		const auto bCtrl = g_CheckModifierKey(QCTRL);
		const auto bShift = g_CheckModifierKey(QSHIFT);

		// Unselect all objects (does not affect cached selections)
		g_DeselectAllObjects(doc);

		// Iterate through all previously selected objects (cached selections)
		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto obj = static_cast<BaseObject*>(activeObjects->GetIndex(i));

			// No object was selected
			if (!obj)
				continue;


			// Currently processing an BaseObject?
			if (obj->IsInstanceOf(Obase))
			{
				// Retrieve the referenced object in the first instance selected and select all corresponding instances
				auto referenceObject = obj->IsInstanceOf(Oinstance) ? g_GetInstanceRef(obj, bCtrl) : obj; // Alternativley use getInstanceRefDeep
				auto currentObject = doc->GetFirstObject();

				// Speedup
				if (!referenceObject)
					break;


				// Iterate through all objects in the document
				while (currentObject)
				{
					if (currentObject->IsInstanceOf(Oinstance))
					{
						// Get linked object
						const auto linkedObj = g_GetInstanceRef(currentObject, bCtrl);


						if (linkedObj && linkedObj == referenceObject)
						{
							// Select reference object if Shift is held down
							if (bShift)
							{
								doc->SetActiveObject(referenceObject, SELECTION_ADD);
							}

							doc->SetActiveObject(currentObject, SELECTION_ADD);
						}
					}
					currentObject = static_cast<BaseObject*>(g_GetNextElement(static_cast<GeListNode*>(currentObject)));
				}
			}
		}

		EventAdd();

		return true;
	}
};
