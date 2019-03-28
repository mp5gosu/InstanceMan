#pragma once


/**
 * @brief Object / shader browser iteration helper.\n
 * https://developers.maxon.net/?p=604
 * @param op The ListNode to walk through
 * @return The next element in the hierarchy
 */
inline GeListNode* g_GetNextElement(GeListNode* op)
{
	if (!op)
		return nullptr;

	// in case of material inspect shaders otherwise continue in usual mode
	if (op->IsInstanceOf(Mmaterial) && static_cast<BaseList2D*>(op)->GetFirstShader())
	{
		return static_cast<BaseList2D*>(op)->GetFirstShader();
	}

	if (op->GetDown())
		return op->GetDown();

	while (!op->GetNext() && op->GetUp())
		op = op->GetUp();

	return op->GetNext();
}


/**
 * @brief Deselects all objects in a given document
 * @param doc The document in which all objects shall be deselected
 */
inline void g_DeselectAllObjects(BaseDocument* doc)
{
	if (!doc)
		return;

	BaseList2D* obj = doc->GetFirstObject();

	while (obj)
	{
#if API_VERSION > 20
		doc->AddUndo(UNDOTYPE::BITS, obj);
#else
		doc->AddUndo(UNDOTYPE_BITS, obj);
#endif
		obj->DelBit(BIT_ACTIVE);
		obj = static_cast<BaseList2D*>(g_GetNextElement(obj));
	}
}


/**
 * @brief Returns all children of a given object
 * @param obj The object whose children shall be collected
 * @param arr The AtomArray to be filled with the children
 */
inline void g_GetChildren(BaseObject* obj, AtomArray& arr)
{
	if (!obj)
		return;

	auto child = obj->GetDown();
	while (child)
	{
		arr.Append(child);
		g_GetChildren(child, arr);
		child = child->GetNext();
	}
}

/**
 * @brief Removes and frees all children of a given object.
 * @param obj The object which children are about to be deleted.
 */
inline void g_DeleteChildren(BaseObject* obj)
{
	if (!obj)
		return;

	auto child = obj->GetDown();
	while (child)
	{
		const auto next = child->GetNext();
		child->Remove();
		BaseObject::Free(child);
		child = next;
	}
}


/**
 * @brief Detects currently pressed Modifier key.\n
 * Allowed values are:  QSHIFT, QCTRL, QALT, QALT2. 
 * @param key The key. Keys allowed are Cinema4D's defines.
 * @return returns true if the provided key is pressed. Else returns false.
 */
inline Bool g_CheckModifierKey(const Int32 key)
{
	// Detect Key modifiers
	BaseContainer state;
	GetInputState(BFM_INPUT_KEYBOARD, BFM_INPUT_CHANNEL, state);


	return (state.GetInt32(BFM_INPUT_QUALIFIER, 0) & key) != 0;
}
