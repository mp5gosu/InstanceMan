#pragma once

#include "c4d.h"

#include "instance_functions.h"

class Command_Split : public CommandData
{
INSTANCEOF(Command_Split, CommandData)

public:
	Int32 GetState(BaseDocument* doc) override
	{
		// Disable Menu entry if not at least 2 objects are selected selected
		const AutoAlloc<AtomArray> arr;
		doc->GetActiveObjects(arr, GETACTIVEOBJECTFLAGS::CHILDREN);
		if (!arr || arr->GetCount() < 2)
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

		// Minimum of 2 objects have to be selected
		if (activeObjects->GetCount() <= 1)
			return false;


		// Detect Key modifiers#
		BaseContainer state;
		GetInputState(BFM_INPUT_MOUSE, BFM_INPUT_MOUSELEFT, state);
		const auto bCtrl = (state.GetInt32(BFM_INPUT_QUALIFIER) & QCTRL) != 0;
		String newName;

		// Remove all objects that are not instances
		activeObjects->FilterObject(Oinstance, Oinstance, true);
		// Convert the lastly selected instance to the new reference object
		auto lastObject = static_cast<BaseObject*>(activeObjects->GetIndex(activeObjects->GetCount() - 1));
		activeObjects->Remove(lastObject);
		if (activeObjects->GetCount() == 0)
			return false;

		if (!lastObject)
			return false;

		if (lastObject->GetType() != Oinstance)
			return false;

		// Popup InputDialog to enter a new name
		if (bCtrl)
		{
			if (!RenameDialog(&newName))
				return false;
		}

		const auto refObj = g_MakeInstanceEditable(lastObject);

		if (!refObj)
			return false;

		// Set Name
		refObj->SetName(newName.IsEmpty() ? refObj->GetName() : newName);

		// Insert converted object (the new reference) into the document
		doc->InsertObject(refObj, lastObject->GetUp(), lastObject->GetPred());
		doc->AddUndo(UNDOTYPE::NEWOBJ, refObj);

		// Relink instances
		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			const auto obj = static_cast<BaseObject*>(activeObjects->GetIndex(i));

			// Update instance links
			g_LinkInstance(obj, refObj);
			obj->SetName(refObj->GetName());
		}

		doc->AddUndo(UNDOTYPE::DELETEOBJ, lastObject);
		lastObject->Remove();
		BaseObject::Free(lastObject);

		doc->EndUndo();

		EventAdd();
		return true;
	}
};
