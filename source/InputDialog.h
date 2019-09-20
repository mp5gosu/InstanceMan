#pragma once
#include "c4d.h"


class InputDialog : public GeModalDialog
{
INSTANCEOF(InputDialog, GeModalDialog)

public:
	Bool Command(Int32 id, const BaseContainer& msg) override
	{
		switch (id)
		{
			case DLG_OK:
				GetString(1001, _name);
				break;

			case DLG_CANCEL:
			default: break;
		}

		return GeModalDialog::Command(id, msg);
	}

	Bool CreateLayout() override
	{
		SetTitle(String("Enter name for new reference"_s));
		AddEditText(1001, BFH_SCALEFIT | BFV_TOP, 200);
		AddDlgGroup(DLG_OK | DLG_CANCEL);
		return true;
	}

	Bool InitValues() override
	{
		_name = ""_s;
		return true;
	}

	String GetName() const
	{
		return _name;
	}

	void SetName(const String& name)
	{
		_name = name;
	}

private:
	String _name;
};
