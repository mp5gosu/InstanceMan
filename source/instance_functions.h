#pragma once

#include "c4d.h"

#include "constants.h"
#include "c4d_symbols.h"

/**
 * @brief Relinks an instance to the given object
 * @param instance The instance object whose link is about to be updated.
 * @param refObj The new reference object
 * @return true if success or false if an error occured
 */
inline Bool g_LinkInstance(BaseObject* instance, BaseObject* refObj)
{
	if (!instance || !refObj)
		return false;

	if (instance->GetType() != Oinstance)
		return false;

	auto doc = refObj->GetDocument();
	if (!doc)
		return false;

	// Create a new baselink, set refObj as reference and pass it to the instance object
	AutoAlloc<BaseLink> link;

	// Allocation failed
	if (!link)
		return false;

	link->SetLink(refObj);
	GeData data;
	data.SetBaseLink(link);
	doc->AddUndo(UNDOTYPE::CHANGE_SMALL, instance);
	instance->SetParameter(DescID(INSTANCEOBJECT_LINK), data, DESCFLAGS_SET::NONE);
	instance->Message(MSG_UPDATE);

	return true;
}


/**
 * @brief Creates a new single instance from a given object, renames it and selects it. The reference object is deselected.
 * @param refObj The reference object
 * @return The instance object or nullptr if something failed
 */
inline BaseObject* g_CreateInstance(BaseObject* refObj)
{
	if (!refObj)
		return nullptr;

	auto doc = refObj->GetDocument();
	if (!doc)
		return nullptr;

	// Allocate instance object
	auto instanceObject = BaseObject::Alloc(Oinstance);
	if (!instanceObject)
		return nullptr;

	doc->AddUndo(UNDOTYPE::NEWOBJ, instanceObject);

	// Get the active instance mode from plugin settings
	auto instanceMode = 0;
	const auto bc = GetWorldPluginData(PID_IM);
	if (bc)
		instanceMode = bc->GetInt32(DEFAULT_INSTANCEMODE, 0);


	// Set Name to the same as the reference object
	doc->AddUndo(UNDOTYPE::CHANGE_SMALL, instanceObject);
	instanceObject->SetName(refObj->GetName());
	doc->AddUndo(UNDOTYPE::CHANGE_SMALL, instanceObject);
	instanceObject->SetParameter(INSTANCEOBJECT_RENDERINSTANCE_MODE, GeData(instanceMode), DESCFLAGS_SET::NONE);

	g_LinkInstance(instanceObject, refObj);


	// Deselect reference object and select new instance object
	doc->AddUndo(UNDOTYPE::BITS, refObj);
	refObj->DelBit(BIT_ACTIVE);
	doc->AddUndo(UNDOTYPE::BITS, instanceObject);
	instanceObject->SetBit(BIT_ACTIVE);

	return instanceObject;
}


/**
 * @brief Moves all children of srcObj to tgtObj. Adds Undos.
 * @param srcObj The source object
 * @param tgtObj The target object
 */
inline void g_MoveChildren(BaseObject* srcObj, BaseObject* tgtObj)
{
	if (!srcObj)
		return;

	auto doc = srcObj->GetDocument();
	if (!doc)
		return;

	auto child = srcObj->GetDown();

	while (child)
	{
		const auto temp = child->GetNext();
		doc->AddUndo(UNDOTYPE::CHANGE, child);
		child->Remove();
		child->InsertUnderLast(tgtObj);
		child = temp;
	}
}


/**
 * @brief Simply create a copy from the given instance and put it above
 * @param obj The instance object to copy
 */
inline void g_CreateInstanceCopy(BaseObject* obj)
{
	if (!obj)
		return;

	auto doc = obj->GetDocument();

	// Make a copy of the current instance and prepare it for document insertion
	const auto copyObj = static_cast<BaseObject*>(obj->GetClone(COPYFLAGS::RECURSIONCHECK, nullptr));
	if (!copyObj)
		return;

	// Deselect reference object and select new instance object
	doc->AddUndo(UNDOTYPE::BITS, obj);
	obj->DelBit(BIT_ACTIVE);

	doc->AddUndo(UNDOTYPE::NEWOBJ, copyObj);
	doc->InsertObject(copyObj, obj->GetUp(), obj->GetPred());
}


// Get shallow or deeply linked reference objects
/**
 * @brief Retrieve the direct or root linked object of an instance. Also works with nested instances
 * @param obj the instance object
 * @param deep if true, the root object of nested instances is returned
 * @return the reference or root object. Can return nullptr.
 */
inline BaseObject* g_GetInstanceRef(BaseObject* obj, const Bool deep = false)
{
	if (!obj->IsInstanceOf(Oinstance))
		return nullptr;

	// Retrieve Link from instance
	GeData data;
	if (!obj->GetParameter(DescID(INSTANCEOBJECT_LINK), data, DESCFLAGS_GET::NONE))
		return nullptr;

	// The document needs to be provided for GetLinkAtom()
	const auto doc = obj->GetDocument();
	if (!doc)
		return nullptr;

	// Get the Atom
	auto linkedEntity = static_cast<BaseObject*>(data.GetLinkAtom(doc, Obase));
	if (!linkedEntity)
		return nullptr;

	// Get down to the reference object if root object is requested
	Int32 refCountLimit = 10000;

	while (linkedEntity && deep && linkedEntity->IsInstanceOf(Oinstance))
	{
		if (refCountLimit <= 0)
		{
			MessageDialog(g_resource.LoadString(IDS_REFCOUNTWARN));
			return nullptr;
		}

		if (!linkedEntity->GetParameter(DescID(INSTANCEOBJECT_LINK), data, DESCFLAGS_GET::NONE))
			return nullptr;

		linkedEntity = static_cast<BaseObject*>(data.GetLinkAtom(doc, Obase));


		--refCountLimit;
	}

	return linkedEntity;
}


/**
 * @brief Converts an object to an instance and links it with the given reference object.
 * @param doc The target document.
 * @param refObj The reference object to link to the newly created instances.
 * @param obj The object that is about to be converted to an instance.
 */
inline void g_CreateInstancesFromSelection(BaseDocument* doc, BaseObject* refObj, BaseObject* obj, Bool moveChildren = true)
{
	if (!obj || !doc || !refObj)
		return;

	// Create a single instance
	const auto instance = g_CreateInstance(refObj);
	if (!instance)
		return;

	// Set Bits
	instance->SetAllBits(obj->GetAllBits());

	// Copy matrix
	obj->CopyMatrixTo(instance);

	// Put it into document
	doc->InsertObject(instance, obj->GetUp(), obj->GetPred());
	if (moveChildren)
		g_MoveChildren(obj, instance);


	// Remove the current object, since we want to swap the object with the instance
	// Move children beforehand!
	doc->AddUndo(UNDOTYPE::DELETEOBJ, obj);
	obj->Remove();
	BaseObject::Free(obj);
}


/**
 * @brief Replaces Cinema 4D's "Make editable" command that makes it possible to also convert instances of whatever type they are.
 * @param obj The object that shall be converted
 * @param deep if true, the root reference object is used instead of a directly linked instance
 * @return the converted object or nullptr if the operation fails
 */
inline BaseObject* g_MakeInstanceEditable(BaseObject* obj, const bool deep = false)
{
	if (!obj)
		return nullptr;

	const auto doc = obj->GetDocument();
	if (!doc)
		return nullptr;

	auto refObj = obj;
	doc->AddUndo(UNDOTYPE::CHANGE, refObj);
	if (obj->IsInstanceOf(Oinstance))
	{
		// Retrieve the linked or root object
		// obj is instance and root is requested, so simply return a copy of the root object
		refObj = g_GetInstanceRef(obj, deep);
		if (!refObj)
			return nullptr;

		return static_cast<BaseObject*>(refObj->GetClone(COPYFLAGS::NONE, nullptr));
	}
	return nullptr;
}
