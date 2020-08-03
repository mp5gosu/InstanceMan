#pragma once
#include "c4d.h"

#include "instance_functions.h"


class Command_MakeEditable : public CommandData
{
INSTANCEOF(Command_MakeEditable, CommandData)

public:
	Int32 GetState(BaseDocument* doc) override
	{
		// Disable Menu entry if no object is selected
		const AutoAlloc<AtomArray> arr;
		doc->GetActiveObjects(arr, GETACTIVEOBJECTFLAGS::CHILDREN);
		if (!arr || arr->GetCount() == 0)
			return 0;

		for (auto i = 0; i < arr->GetCount(); ++i)
		{
			if (static_cast<BaseObject*>(arr->GetIndex(i))->IsInstanceOf(Oinstance))
				return CMD_ENABLED;
		}
		return 0;
	}

	Bool Execute(BaseDocument* doc) override
	{
		if (!doc)
			return false;

		// Detect Key modifiers
		const auto bCtrl = g_CheckModifierKey(QCTRL);
		const auto bShift = g_CheckModifierKey(QSHIFT);
		const auto bAlt = g_CheckModifierKey(QALT);


		doc->StartUndo();

		// Create Array that holds all objects to operate on
		const AutoAlloc<AtomArray> activeObjects;
		doc->GetActiveObjects(*activeObjects, GETACTIVEOBJECTFLAGS::CHILDREN);

		// empty? quit.
		if (!activeObjects)
			return false;

		for (auto i = 0; i < activeObjects->GetCount(); ++i)
		{
			auto obj = static_cast<BaseObject*>(activeObjects->GetIndex(i));
			if (!obj)
				continue;

			// Make editable magic
			if (obj->IsInstanceOf(Oinstance))
			{
				// Convert a single instance
				auto convertedInstance = g_MakeInstanceEditable(obj, bCtrl);
				if (!convertedInstance) // Something went wrong, skip
					continue;

				// Insert it into the document
				doc->InsertObject(convertedInstance, obj->GetUp(), obj->GetPred());
				doc->AddUndo(UNDOTYPE::NEWOBJ, convertedInstance);

				// Select the new object
				doc->SetActiveObject(convertedInstance, SELECTION_ADD);
				

				// Update links - ONLY for shallow conversion. Deep conversion doesn't require TransferGoal()
				obj->TransferGoal(convertedInstance, false);

				// Copy children
				if (bShift)
					g_DeleteChildren(convertedInstance);

				if (!bAlt)
					g_MoveChildren(obj, convertedInstance);


				// Set PSR
				obj->CopyMatrixTo(convertedInstance);

				// Remove the original instance object to finally replace it with the converted one
				doc->AddUndo(UNDOTYPE::DELETEOBJ, obj);
				obj->Remove();
				BaseObject::Free(obj);
			}
		}

		doc->EndUndo();
		EventAdd();

		return true;
	}
};
