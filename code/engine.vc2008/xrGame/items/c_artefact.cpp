#include "stdafx.h"
#include "c_artefact.h"
#include "../xrphysics/PhysicsShell.h"

C_Arterfact::C_Arterfact()
{
}

C_Arterfact::~C_Arterfact()
{
}

void C_Arterfact::Load(LPCSTR section)
{
    inherited::Load(section);
}

void C_Arterfact::UpdateCLChild()
{
    inherited::UpdateCLChild();

    if (H_Parent()) XFORM().set(H_Parent()->XFORM());
};