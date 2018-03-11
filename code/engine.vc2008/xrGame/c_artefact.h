#pragma once
#include "artefact.h"

//Ёто был раньше класс CElectricBall, который на самом деле €вл€лс€ клиентской версией всех артефактов, не смотр€ на вычурное название
//» если у нас сейчас нету новой системы регистрации объектов, то удал€ть этот класс Ќ≈Ћ№«я

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