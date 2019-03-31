#pragma once

#include "c4d.h"

class Command_FrameElements : public CommandData
{
INSTANCEOF(Command_FrameElements,CommandData)

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
		if (!doc)
			return false;

		doc->StartUndo();

		// Create Array that holds all objects to operate on
		const AutoAlloc<AtomArray> activeObjects;
		doc->GetActiveObjects(activeObjects, GETACTIVEOBJECTFLAGS::SELECTIONORDER | GETACTIVEOBJECTFLAGS::CHILDREN);

		// Allocation failed
		if (!activeObjects)
			return false;

		// GeData container to store individual instance mode
		GeData data;
		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto obj = static_cast<BaseObject*>(activeObjects->GetIndex(i));
			if (obj->GetType() == Oinstance)
			{
				auto bc = obj->GetDataInstance();
				if (!bc)
					continue;

				BaseContainer dataBc;

				// Save the current instance mode and set it to normal instances
				obj->GetParameter(DescID(INSTANCEOBJECT_RENDERINSTANCE_MODE), data, DESCFLAGS_GET::NONE);
				dataBc.SetInt32(INSTANCEMODE, data.GetInt32());

				obj->SetParameter(DescID(INSTANCEOBJECT_RENDERINSTANCE_MODE), GeData(INSTANCEOBJECT_RENDERINSTANCE_MODE_NONE), DESCFLAGS_SET::NONE);
				bc->SetContainer(PID_IM, dataBc);
			}
		}

		// IDM_SHOWACTIVE
		CallCommand(COMMAND_FRAMEELEMENTS);

		// Restore all instances to their previous modes
		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto obj = static_cast<BaseObject*>(activeObjects->GetIndex(i));
			if (obj->GetType() == Oinstance)
			{
				auto bc = obj->GetDataInstance();
				if (!bc)
					continue;

				const auto dataBc = bc->GetContainerInstance(PID_IM);
				if (!dataBc)
					continue;


				obj->SetParameter(DescID(INSTANCEOBJECT_RENDERINSTANCE_MODE), dataBc->GetInt32(INSTANCEMODE), DESCFLAGS_SET::NONE);
			}
		}

		doc->EndUndo();

		EventAdd();
		return true;
	}
};
