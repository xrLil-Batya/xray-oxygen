#ifndef OGSE_CONFIG_H
#define OGSE_CONFIG_H

#include "configurator_defines.h"
// эффекты со включением через консоль
// Саншафты								r2_sunshafts [qt_off/qt_low/qt_medium/qt_high/qt_extreme]
// Объемные саншафты					r2_sunshafts_mode volumetric
// Плоские саншафты аля Crysis			r2_sunshafts_mode screen_space

// PARAMETERS 
// Используйте параметры для настройки соответствующего эффекта
/////////////////////////////////////////////////////////////
// Плоские саншафты
	#define SUNSHAFTS_QUALITY 4
	#define SS_DUST
	#define SS_INTENSITY float(1.0)			// яркость лучей, работает совместно с настройками из конфига
	#define SS_BLEND_FACTOR float(0.8)		// фактор смешивания с рассеянным светом. Чем меньше значение, тем меньше "засветка" от эффекта, но теряется "объемность"
	#define SS_LENGTH float(1.0)			// длина лучей. Чем больше длина, тем меньше производительность
	#define SS_DUST_SPEED float(0.4)		// скорость частиц пыли 
	#define SS_DUST_INTENSITY float(2.0)	// яркость пылинок
	#define SS_DUST_DENSITY float(1.0)		// плотность частиц пыли 
	#define SS_DUST_SIZE float(0.7)			// размер пылинок
	
/////////////////////////////////////////////////////////////
// Боке блюр
	#define BOKEH_QUALITY 0 // качество боке //	TODO: Связать с движком
	
	//Для дебага
    #define BOKEH_FEATHER float(0.5) // размер боке
    #define BOKEH_THRESHOLD float(0.5) // порог яркости, ниже которого выборка пикселя будет отбрасываться. Более 1,1 не ставить
    #define BOKEH_GAIN float(1) //усиление свечения
    #define BOKEH_FRINGE float(0.7) // коэффициент разброса пикселей при расчете хроматической аберрации. Чем больше поставить, тем больше эффект аберрации
    #define BOKEH_KERNEL float(2) // коэффициент размытия
    #define BOKEH_BIAS float(0.5) // коэффициент ослабления вклада от дальних выборок. При увеличении получается более резкий блюр
   
#endif