#pragma once

#include "c4d.h"

#include "c4d_symbols.h"

class SettingsDialog : public GeDialog
{
INSTANCEOF(SettingsDialog, GeDialog)

public:
	Bool CreateLayout() override
	{
		SetTitle(g_resource.LoadString(IDS_SETTINGSDIALOG_TITLE));
		GroupBegin(0, BFH_SCALEFIT | BFV_TOP, 2, 0, ""_s, BFV_GRIDGROUP_EQUALCOLS);
		GroupSpace(4, 4);
		GroupBorderSpace(4, 4, 4, 4);
		AddStaticText(IDS_LBL_INSTANCEMODE, BFH_LEFT | BFV_SCALEFIT, 0, 0, g_resource.LoadString(IDS_LBL_INSTANCEMODE), BORDER_NONE);
		AddComboBox(IDS_INSTANCEMODE, BFH_SCALEFIT | BFV_TOP);
		GroupEnd();
		return true;
	}

	Bool InitValues() override
	{
		// Populate cycle with available instance modes
		this->FreeChildren(IDS_INSTANCEMODE);
		BaseContainer renderModeitems;
		GetInstanceDescription(renderModeitems);
		AddChildren(IDS_INSTANCEMODE, renderModeitems);
		SetInt32(IDS_INSTANCEMODE, GetData());
		LayoutChanged(IDS_INSTANCEMODE);
		return true;
	}

	Bool Command(Int32 id, const BaseContainer& msg) override
	{
		SetData(); // Save the current selected instance mode
		return GeDialog::Command(id, msg);
	}

	// Get the available instance modes
	static void GetInstanceDescription(BaseContainer& bc)
	{
		AutoAlloc<Description> desc;

		// Get the description of instance objects
		if (desc && desc->LoadDescription(Oinstance))
		{
			const DescID completeId = DescLevel(INSTANCEOBJECT_RENDERINSTANCE_MODE, DTYPE_LONG, 0);

			// Get its parameters
			const auto param = desc->GetParameterI(completeId, nullptr);
			if (param)
				param->GetContainer(DESC_CYCLE).CopyTo(&bc, COPYFLAGS::NONE, nullptr);
		}
	}

	static Int32 GetData()
	{
		// Get the active instance mode from plugin settings
		const auto bc = GetWorldPluginData(PID_IM);
		if (bc)
			return bc->GetInt32(DEFAULT_INSTANCEMODE);
		return 0;
	}

private:
	void SetData()
	{
		// Read the plugin settings and set the active instance mode
		BaseContainer bc;
		GetInt32(IDS_INSTANCEMODE, _activeInstanceMode);
		bc.SetInt32(DEFAULT_INSTANCEMODE, _activeInstanceMode);
		SetWorldPluginData(PID_IM, bc, false);
	}


	Int32 _activeInstanceMode = 0;
};
