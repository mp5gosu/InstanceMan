# InstanceMan
**Note:** This repository contains the R20 version.  
For R18-R19, go to: https://github.com/mp5gosu/InstanceMan-R18  
For R21, go to: https://github.com/mp5gosu/InstanceMan-R21

## Download
R21: https://github.com/mp5gosu/InstanceMan-R21/releases/latest  
R20: https://github.com/mp5gosu/InstanceMan-R20/releases/latest  
R18-R19: https://github.com/mp5gosu/InstanceMan-R18/releases/latest

## Overview
InstanceMan is a Cinema 4D plugin that helps managing instance objects.

Commands available:

#### Create
- Creates new instances or copies existing ones.
- **Ctrl**: Converts current object selection to instances and sets the last selected object as reference.
- **Shift**: Moves all children into the new reference object.

#### Select
- Selects all instances of the same reference object.
- **Ctrl**: Selects nested instances.
- **Shift**: Also select reference object.

#### Rename
- Renames all selected instances according to their reference object.
- **Ctrl**: Also renames nested instances.
- **Shift**: Enumerates the selected instances.

#### Split
- Relinks selected instances. The last selected instance is converted to a new reference object.
- **Ctrl**: Pops up a dialog to rename the new reference object.

#### Make instances editable
- Extends Cinema 4Ds **Make editable**. Converts render- and multi-instances.
- **Ctrl**: Converts nested instances directly to the root object.
- **Shift**: Removes children of the instance.
- **Alt**: Don't inherit children of the reference object.

#### Swap
- Swap a selected instance with its reference object and back. Allows to edit an instance in place.
- **Ctrl**: Additionally swap children.

#### Frame selected objects
- Replaces Cinema 4Ds **Frame selected objects**. Render- and multi-instances are also respected.

#### Frame selected elements
- Replaces Cinema 4Ds **Frame selected elements**. Render- and multi-instances are also respected

#### Settings
- Open InstanceMan settings
- **Default instance mode**: Set your desired instance mode for newly created instances.


## How to build
Clone this repository to your Cinema 4D `sdk/plugins/instanceman_r20` folder.   
Prepare your master solution's `projectdefinition.txt` to include the plugin.   
Run the projet tool to update your plugin projects.   

## ToDo
- Add additional command to replace all nested instances with their root reference object
