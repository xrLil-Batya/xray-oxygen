#pragma once
#include "items/Artefact.h"

//Это был раньше класс CElectricBall, который на самом деле являлся клиентской версией всех артефактов, не смотря на вычурное название
//И если у нас сейчас нету новой системы регистрации объектов, то удалять этот класс НЕЛЬЗЯ

class C_Arterfact : public CArtefact
{
private:
    typedef CArtefact inherited;
public:
    C_Arterfact(void);
    virtual ~C_Arterfact(void);

    virtual void Load(LPCSTR section);

protected:
    virtual void	UpdateCLChild();

};