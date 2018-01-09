ВОЗМОЖНЫЕ ПРОБЛЕМЫ И РЕШЕНИЯ
	1) Не открываются формочки
	== Убедитесь что проекты xrPostProcessEditor и xrSdkControls находятся в конфигурации AnyCPU. Это связано с тем что студия x32, и не может грузить x64 сборки
	== Для работы конструктора Windows Forms нужно хотя бы один раз собрать xrSdkControls и xrPostprocessEditor