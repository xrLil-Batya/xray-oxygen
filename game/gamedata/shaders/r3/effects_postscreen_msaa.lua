function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "vignette")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", "$user$generic")
end

--// Chromatic Aberration
function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "chromatic_aberration")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", "$user$generic")
	
	shader:dx10sampler	("smp_nofilter")
end

--// Grading
local tex_color	= "shaders\\color_chart"
function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "color_grading")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", "$user$color_grading")
	shader:dx10texture	("color_chart_sampler", tex_color)
	
	shader:dx10sampler	("smp_rtlinear")
end
